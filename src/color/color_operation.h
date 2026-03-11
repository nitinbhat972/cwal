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

Color darken_color(Color clr, float amount);
Color lighten_color(Color clr, float amount);
Color saturate_color(Color clr, float amount);
float w3_luminance(Color clr);
Color adjust_alpha(Color clr, float amount);
Color binary_luminance_adjust(float luminance_desired, float hue, float s_min,
                              float s_max, float v_min, float v_max,
                              int iterations);
float calculate_contrast_ratio(Color color1, Color color2);
