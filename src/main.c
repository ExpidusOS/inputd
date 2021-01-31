#define G_LOG_DOMAIN "expidus-inputd"
#include <expidus-input/math.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <devident.h>
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <libinput.h>
#include <libudev.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xlib.h>

#define CLEAR_MOTION -999999

static int open_restricted(const char* path, int flags, void* data) {
	int fd = open(path, flags);
	return fd < 0 ? -errno : fd;
}
 
static void close_restricted(int fd, void* data) {
	close(fd);
}
 
const static struct libinput_interface libinput_iface = {
	.open_restricted = open_restricted,
	.close_restricted = close_restricted
};

static expidus_input_swipe_t pending_swipe;
static expidus_input_edge_t pending_edge;
static expidus_input_dist_t pending_dist;
static double start[20][2];
static double end[20][2];
static int n_down = 0, n_pending = 0;
static struct timespec timedown;
static uint32_t screen_width;
static uint32_t screen_height;

static void monitors_changed(GdkDisplay* disp, GdkMonitor* monitor, gpointer data) {
	screen_width = DisplayWidth(GDK_DISPLAY_XDISPLAY(disp), DefaultScreen(GDK_DISPLAY_XDISPLAY(disp)));
	screen_height = DisplayHeight(GDK_DISPLAY_XDISPLAY(disp), DefaultScreen(GDK_DISPLAY_XDISPLAY(disp)));
}

static void monitor_invalidated(GdkMonitor* monitor, gpointer data) {
	GdkDisplay* disp = gdk_monitor_get_display(monitor);
	monitors_changed(disp, monitor, NULL);
}

static void monitors_added(GdkDisplay* disp, GdkMonitor* monitor, gpointer data) {
	g_signal_connect(monitor, "invalidated", G_CALLBACK(monitor_invalidated), NULL);
	monitors_changed(disp, monitor, data);
}

int main(int argc, char** argv) {
	gdk_init(&argc, &argv);
	gchar* seat_name = (gchar*)g_getenv("XDG_SEAT");
	if (seat_name == NULL) seat_name = "seat0";

	struct libinput* libinput_ctx = libinput_udev_create_context(&libinput_iface, NULL, udev_new());
	libinput_udev_assign_seat(libinput_ctx, seat_name);
	libinput_dispatch(libinput_ctx);

	struct libinput_event* ev;

	for (int i = 0; i < 20; i++) {
		start[i][0] = CLEAR_MOTION;
		start[i][1] = CLEAR_MOTION;

		end[i][0] = CLEAR_MOTION;
		end[i][1] = CLEAR_MOTION;
	}

	GError* error = NULL;
	devident_t* devident = devident_new(&error);
	if (devident == NULL) {
		g_error("Failed to identify the device: %s", error->message);
		g_clear_error(&error);
		return EXIT_FAILURE;
	}

  expidus_input_edge_conf_t edge_sizes = {
    0.50, 0.50, 0.50, 0.50
  };

	uint16_t timeoutms = 800;

	GdkDisplay* disp = gdk_display_get_default();
	g_signal_connect(disp, "monitor-added", G_CALLBACK(monitors_added), NULL);
	g_signal_connect(disp, "monitor-removed", G_CALLBACK(monitors_changed), NULL);

	if (devident->touchinput_path != NULL) {
		struct libinput_device* dev = libinput_path_add_device(libinput_ctx, devident->touchinput_path);
		if (dev == NULL) {
			g_error("Failed to add touchscreen input device");
			libinput_unref(libinput_ctx);
			devident_destroy(devident);
			return EXIT_FAILURE;
		}

		if (libinput_device_config_send_events_set_mode(dev, LIBINPUT_CONFIG_SEND_EVENTS_ENABLED) != LIBINPUT_CONFIG_STATUS_SUCCESS) {
			g_error("Failed to enable events on touchscreen input device");
			libinput_unref(libinput_ctx);
			devident_destroy(devident);
			return EXIT_FAILURE;
		}
	}

	while (true) {
		if ((ev = libinput_get_event(libinput_ctx)) != NULL) {
			switch (libinput_event_get_type(ev)) {
				case LIBINPUT_EVENT_TOUCH_DOWN:
					{
						struct libinput_event_touch* touch = libinput_event_get_touch_event(ev);
						int slot = libinput_event_touch_get_slot(touch);
						if (slot > 20) break;
						start[slot][0] = libinput_event_touch_get_x(touch);
						start[slot][1] = libinput_event_touch_get_y(touch);
						if (n_down == 0) clock_gettime(CLOCK_MONOTONIC_RAW, &timedown);
						n_down++;
					}
					break;
        case LIBINPUT_EVENT_TOUCH_MOTION:
          {
						struct libinput_event_touch* touch = libinput_event_get_touch_event(ev);
						int slot = libinput_event_touch_get_slot(touch);
						if (slot > 20) break;
						end[slot][0] = libinput_event_touch_get_x(touch);
						end[slot][1] = libinput_event_touch_get_y(touch);
          }
          break;
				case LIBINPUT_EVENT_TOUCH_UP:
					{
						struct libinput_event_touch* touch = libinput_event_get_touch_event(ev);
						int slot = libinput_event_touch_get_slot(touch);
						if (slot > 20) break;
						n_down--;

						struct timespec now;
						clock_gettime(CLOCK_MONOTONIC_RAW, &now);

						if (start[slot][0] == CLEAR_MOTION || start[slot][1] == CLEAR_MOTION
							|| end[slot][0] == CLEAR_MOTION || end[slot][1] == CLEAR_MOTION) break;

            expidus_input_swipe_t swipe = expidus_input_calc_swipe(start[slot][0], start[slot][1], end[slot][0], end[slot][1], 300);
            expidus_input_edge_t edge = expidus_input_calc_edge(start[slot][0], start[slot][1], end[slot][0], end[slot][1], edge_sizes, screen_width, screen_height);
						expidus_input_dist_t dist = expidus_input_calc_dist(start[slot][0], start[slot][1], end[slot][0], end[slot][1], swipe, screen_width, screen_height);

						if (n_pending == 0) {
							pending_swipe = swipe;
							pending_edge = edge;
							pending_dist = dist;
						}
						if (pending_swipe == swipe) n_pending++;	

						start[slot][0] = CLEAR_MOTION;
						start[slot][1] = CLEAR_MOTION;

						end[slot][0] = CLEAR_MOTION;
						end[slot][1] = CLEAR_MOTION;

						if (n_down == 0) {
							if (timeoutms > ((now.tv_sec - timedown.tv_sec) * 1000000 + (now.tv_nsec - timedown.tv_nsec) / 1000) / 1000) {
								g_debug("Received gesture (S: %d, E: %d, D: %d, F: %d)", swipe, edge, dist, n_pending);
							}
							n_pending++;
						}
					}
					break;
				default:
					break;
			}
			libinput_event_destroy(ev);
			libinput_dispatch(libinput_ctx);
		}
	}

	libinput_unref(libinput_ctx);
	devident_destroy(devident);
	return EXIT_SUCCESS;
}
