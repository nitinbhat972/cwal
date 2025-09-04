#pragma once
#include "core.h"
#include <stdint.h>

float clamp_value(float amount);
uint8_t clamp_byte(float value);
void sort_color(Palette *palette);
bool is_color_too_bright_for_dark_mode(float hue, float saturation,
                                       float luminance_threshold);
void preview_palette();
void to_lowercase(char *str);

#define RED "\x1b[38;2;220;50;47m"
#define BLUE "\x1b[38;2;38;139;210m"
#define YELLOW "\x1b[38;2;181;137;0m"
#define RESET "\x1b[0m"

// Enumeration for different logging levels.
enum Logging { INFO, WARN, ERROR };
void logging(int log_level, const char *format, ...);
void set_quiet_mode(bool quiet);
