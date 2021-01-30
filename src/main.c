#define G_LOG_DOMAIN "expidus-inputd"
#include <errno.h>
#include <fcntl.h>
#include <glib.h>
#include <libinput.h>
#include <libudev.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

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

int main(int argc, char** argv) {
	gchar* seat_name = (gchar*)g_getenv("XDG_SEAT");
	if (seat_name == NULL) seat_name = "seat0";

	struct libinput* libinput_ctx = libinput_udev_create_context(&libinput_iface, NULL, udev_new());
	libinput_udev_assign_seat(libinput_ctx, seat_name);
	libinput_dispatch(libinput_ctx);

	struct libinput_event* ev;

	double gesture_start[2];
	double gesture_end[2];
	while (true) {
		if ((ev = libinput_get_event(libinput_ctx)) != NULL) {
			switch (libinput_event_get_type(ev)) {
				case LIBINPUT_EVENT_GESTURE_SWIPE_BEGIN:
					{
						struct libinput_event_touch* touch = libinput_event_get_touch_event(ev);
						gesture_start[0] = libinput_event_touch_get_x(touch);
						gesture_start[1] = libinput_event_touch_get_y(touch);
					}
					break;
				case LIBINPUT_EVENT_GESTURE_SWIPE_END:
					{
						struct libinput_event_gesture* gesture = libinput_event_get_gesture_event(ev);
						gesture_end[0] = gesture_start[0] + libinput_event_gesture_get_dx(gesture);
						gesture_end[1] = gesture_start[1] + libinput_event_gesture_get_dy(gesture);

						g_debug("Gesture start: (%f, %f) Gesture end: (%f, %f)", gesture_start[0],
							gesture_start[1], gesture_end[0], gesture_end[1]);
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
	return EXIT_SUCCESS;
}
