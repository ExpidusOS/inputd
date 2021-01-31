#define G_LOG_DOMAIN "expidus-inputd"
#include <expidus-input/math.h>
#include <math.h>

static const uint8_t deglen = 15;

#define SWIPE_DEG(a, b) (a >= b - deglen && a <= b + deglen)

expidus_input_swipe_t expidus_input_calc_swipe(double x0, double y0, double x1, double y1, uint16_t threshold) {
  double t = atan2(x1 - x0, y0 - y1); 
  double degrees = 57.2957795130823209 * (t < 0 ? t + 6.2831853071795865 : t);
  double dist = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));

  if (dist < threshold) return EXPIDUS_INPUT_SWIPE_NONE;
  else if (SWIPE_DEG(degrees, 0)) return EXPIDUS_INPUT_SWIPE_DU;
  else if (SWIPE_DEG(degrees, 45)) return EXPIDUS_INPUT_SWIPE_DLUR;
  else if (SWIPE_DEG(degrees, 90)) return EXPIDUS_INPUT_SWIPE_LR;
  else if (SWIPE_DEG(degrees, 135)) return EXPIDUS_INPUT_SWIPE_ULDR;
  else if (SWIPE_DEG(degrees, 180)) return EXPIDUS_INPUT_SWIPE_UD;
  else if (SWIPE_DEG(degrees, 225)) return EXPIDUS_INPUT_SWIPE_URDL;
  else if (SWIPE_DEG(degrees, 270)) return EXPIDUS_INPUT_SWIPE_RL;
  else if (SWIPE_DEG(degrees, 315)) return EXPIDUS_INPUT_SWIPE_DRUL;
  else if (SWIPE_DEG(degrees, 360)) return EXPIDUS_INPUT_SWIPE_DU;
  return EXPIDUS_INPUT_SWIPE_NONE;
}

expidus_input_edge_t expidus_input_calc_edge(double x0, double y0, double x1, double y1, expidus_input_edge_conf_t cfg, uint32_t swidth, uint32_t sheight) {
	expidus_input_edge_t horiz = EXPIDUS_INPUT_EDGE_NONE;
	expidus_input_edge_t vert = EXPIDUS_INPUT_EDGE_NONE;

	if (x0 <= cfg.left) {
		horiz = EXPIDUS_INPUT_EDGE_LEFT;
	} else if (x0 >= swidth - cfg.right) {
		horiz = EXPIDUS_INPUT_EDGE_RIGHT;
	} else if (x1 <= cfg.left) {
		horiz = EXPIDUS_INPUT_EDGE_LEFT;
	} else if (x1 >= swidth - cfg.right) {
		horiz = EXPIDUS_INPUT_EDGE_RIGHT;
	}

	if (y0 <= cfg.top) {
		vert = EXPIDUS_INPUT_EDGE_TOP;
	} else if (y0 >= sheight - cfg.bottom) {
		vert = EXPIDUS_INPUT_EDGE_BOTTOM;
	} else if (y1 <= cfg.top) {
		vert = EXPIDUS_INPUT_EDGE_TOP;
	} else if (y1 >= sheight - cfg.bottom) {
		vert = EXPIDUS_INPUT_EDGE_BOTTOM;
	}

	if (horiz == EXPIDUS_INPUT_EDGE_LEFT && vert == EXPIDUS_INPUT_EDGE_TOP) {
		return EXPIDUS_INPUT_EDGE_CORNER_TOP_LEFT;
	} else if (horiz == EXPIDUS_INPUT_EDGE_RIGHT && vert == EXPIDUS_INPUT_EDGE_TOP) {
		return EXPIDUS_INPUT_EDGE_CORNER_TOP_RIGHT;
	} else if (horiz == EXPIDUS_INPUT_EDGE_LEFT && vert == EXPIDUS_INPUT_EDGE_BOTTOM) {
		return EXPIDUS_INPUT_EDGE_CORNER_BOTTOM_LEFT;
	} else if (horiz == EXPIDUS_INPUT_EDGE_RIGHT && vert == EXPIDUS_INPUT_EDGE_BOTTOM) {
		return EXPIDUS_INPUT_EDGE_CORNER_BOTTOM_RIGHT;
	} else if (horiz != EXPIDUS_INPUT_EDGE_NONE) {
		return horiz;
	}
	return vert;
}

expidus_input_dist_t expidus_input_calc_dist(double x0, double y0, double x1, double y1, expidus_input_swipe_t swipe, uint32_t swidth, uint32_t sheight) {
  double dist = sqrt(pow(x1 - x0, 2) + pow(y1 - y0, 2));
  double diag = sqrt(pow(swidth, 2) + pow(sheight, 2));

  switch (swipe) {
    case EXPIDUS_INPUT_SWIPE_DU:
    case EXPIDUS_INPUT_SWIPE_UD:
      if (dist >= sheight * 0.66) {
        return EXPIDUS_INPUT_DIST_FAR;
      } else if (dist >= sheight * 0.33) {
        return EXPIDUS_INPUT_DIST_LONG;
      } else {
        return EXPIDUS_INPUT_DIST_SHORT;
      }
      break;
    case EXPIDUS_INPUT_SWIPE_LR:
    case EXPIDUS_INPUT_SWIPE_RL:
      if (dist >= swidth * 0.66) {
        return EXPIDUS_INPUT_DIST_FAR;
      } else if (dist >= swidth * 0.33) {
        return EXPIDUS_INPUT_DIST_LONG;
      } else {
        return EXPIDUS_INPUT_DIST_SHORT;
      }
      break;
    case EXPIDUS_INPUT_SWIPE_ULDR:
    case EXPIDUS_INPUT_SWIPE_DRUL:
    case EXPIDUS_INPUT_SWIPE_DLUR:
    case EXPIDUS_INPUT_SWIPE_URDL:
      if (dist >= diag * 0.66) {
        return EXPIDUS_INPUT_DIST_FAR;
      } else if (dist >= diag * 0.33) {
        return EXPIDUS_INPUT_DIST_LONG;
      } else {
        return EXPIDUS_INPUT_DIST_SHORT;
      }
      break;
  case EXPIDUS_INPUT_SWIPE_NONE:
      return EXPIDUS_INPUT_DIST_ANY;
  }
  return EXPIDUS_INPUT_DIST_ANY;
}
