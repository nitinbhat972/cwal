#pragma once

#include "core.h"

Color darken_color(Color clr, float amount);
Color lighten_color(Color clr, float amount);
Color saturate_color(Color clr, float amount);
float w3_luminance(Color clr);
Color get_average_color(const char *filename);
Color adjust_alpha(Color clr, float amount);
Color binary_luminance_adjust(float luminance_desired, float hue, float s_min,
                              float s_max, float v_min, float v_max,
                              int iterations);
float calculate_contrast_ratio(Color color1, Color color2);
