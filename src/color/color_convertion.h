/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

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
