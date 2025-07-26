#pragma once

#include "core.h"

typedef struct {
  float h, l, s, a;
} HSL;

typedef struct {
  float h, s, v, a;
} HSV;

HSL rgb_to_hsl(Color clr);
Color hls_to_rgb(HSL hls);
HSV rgb_to_hsv(Color clr);
Color hsv_to_rgb(HSV hsv);
