#pragma once

#include <stdint.h>

typedef enum {
	EXPIDUS_INPUT_SWIPE_NONE = 0,
	EXPIDUS_INPUT_SWIPE_DU,
	EXPIDUS_INPUT_SWIPE_UD,
	EXPIDUS_INPUT_SWIPE_LR,
	EXPIDUS_INPUT_SWIPE_RL,
	EXPIDUS_INPUT_SWIPE_DLUR,
	EXPIDUS_INPUT_SWIPE_DRUL,
	EXPIDUS_INPUT_SWIPE_URDL,
	EXPIDUS_INPUT_SWIPE_ULDR
} expidus_input_swipe_t;

typedef enum {
	EXPIDUS_INPUT_EDGE_NONE = 0,
	EXPIDUS_INPUT_EDGE_LEFT,
	EXPIDUS_INPUT_EDGE_RIGHT,
	EXPIDUS_INPUT_EDGE_TOP,
	EXPIDUS_INPUT_EDGE_BOTTOM,
	EXPIDUS_INPUT_EDGE_CORNER_TOP_LEFT,
	EXPIDUS_INPUT_EDGE_CORNER_TOP_RIGHT,
	EXPIDUS_INPUT_EDGE_CORNER_BOTTOM_LEFT,
	EXPIDUS_INPUT_EDGE_CORNER_BOTTOM_RIGHT,
	EXPIDUS_INPUT_EDGE_ANY
} expidus_input_edge_t;

typedef enum {
	EXPIDUS_INPUT_DIST_ANY = 0,
	EXPIDUS_INPUT_DIST_SHORT,
	EXPIDUS_INPUT_DIST_LONG,
	EXPIDUS_INPUT_DIST_FAR
} expidus_input_dist_t;

typedef struct {
	double left;
	double right;
	double top;
	double bottom;
} expidus_input_edge_conf_t;

expidus_input_swipe_t expidus_input_calc_swipe(double x0, double y0, double x1, double y1, uint16_t threshold);
expidus_input_edge_t expidus_input_calc_edge(double x0, double y0, double x1, double y1, expidus_input_edge_conf_t cfg, uint32_t swidth, uint32_t sheight);
expidus_input_dist_t expidus_input_calc_dist(double x0, double y0, double x1, double y1, expidus_input_swipe_t swipe, uint32_t swidth, uint32_t sheight);
