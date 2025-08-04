#include "utils.h"
#include "color/color_convertion.h"
#include "color/color_operation.h"
#include "core.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static bool quiet_mode = false;

float clamp_value(float amount) {
  if (amount < 0.0f)
    return 0.0f;
  if (amount > 1.0f)
    return 1.0f;
  return amount;
}

uint8_t clamp_byte(float value) {
  if (value < 0.0f)
    return 0;
  if (value > 255.0f)
    return 255;
  return (uint8_t)(value + 0.5f);
}

static inline int compare_by_luminance(const void *a, const void *b) {
  const Color *colorA = (const Color *)a;
  const Color *colorB = (const Color *)b;
  float lumA = w3_luminance(*colorA);
  float lumB = w3_luminance(*colorB);
  return (lumB < lumA) - (lumB > lumA); // Sort dark to light
}

void sort_color(Palette *palette) {
  if (!palette)
    return;
  qsort(palette->colors, PALETTE_MAX_SIZE, sizeof(Color), compare_by_luminance);
}

bool is_color_too_bright_for_dark_mode(float hue, float saturation,
                                       float luminance_threshold) {
  HSV hsv = {hue, saturation, 1.0f}; // hue is already 0-360 from rgb_to_hsv
  Color rgb = hsv_to_rgb(hsv);

  return w3_luminance(rgb) >= luminance_threshold;
}

void preview_palette() {
  printf("\n");
  for (int i = 0; i < 16; i++) {
    printf("\033[48;5;%dm    \033[0m", i);
    if (i % 8 == 7)
      printf("\n");
  }
  printf("\n");
}

void set_quiet_mode(bool quiet) { quiet_mode = quiet; }

void logging(int log_level, const char *format, ...) {
  if (quiet_mode) {
    return;
  }

  static const char *color[] = {BLUE, YELLOW, RED};
  static const char type[] = {'I', 'W', 'E'};
  static const int max_level = sizeof(type) / sizeof(type[0]);

  if (log_level < 0 || log_level >= max_level)
    log_level = 0;

  va_list args;
  va_start(args, format);
  printf("[%s%c%s]: ", color[log_level], type[log_level], RESET);
  vprintf(format, args);
  printf("\n");
  va_end(args);
}
