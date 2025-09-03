#include "colors.h"
#include "color_convertion.h"
#include "color_operation.h"
#include <math.h>

#define MIN_BRIGHTNESS_THRESHOLD 0.85f
#define TARGET_LIGHTEN_AMOUNT 0.95f
#define DARKEN_AMOUNT 0.70f
#define LIGHTEN_AMOUNT 0.03f
#define SATURATE_AMOUNT 0.40f
#define COLOR_THRESHOLD 16
#define MIN_VALUE 0.25f
#define MIN_SATURATION 0.20f
#define MIN_CONTRAST_RATIO_LIGHT 4.5f
#define MIN_CONTRAST_RATIO_DARK 5.5f
#define VALUE_BOOST 0.3f
#define SATURATION_BOOST 0.4f

// Pre-processes the palette to boost colors from dark images
static void boost_dark_colors(Palette *palette) {
  // Iterate through the 6 primary accent colors
  for (int i = 1; i < 7; i++) {
    HSV hsv = rgb_to_hsv(palette->colors[i]);

    bool needs_boost = false;

    // If the color is too dark, boost its value (brightness)
    if (hsv.v < MIN_VALUE) {
      hsv.v += VALUE_BOOST * (1.0f - hsv.v);
      needs_boost = true;
    }

    // If the color is too desaturated (grayish), boost its saturation
    if (hsv.s < MIN_SATURATION) {
      hsv.s += SATURATION_BOOST * (1.0f - hsv.s);
      needs_boost = true;
    }

    // If changes were made, convert back to RGB and update the palette
    if (needs_boost) {
      palette->colors[i] = hsv_to_rgb(hsv);
    }
  }
}

static void saturate_all_colors(Palette *palette, float amount) {
  if (amount != 0.0f) {
    for (int i = 0; i < PALETTE_MAX_SIZE; i++) {
      if (i != 7 && i != 15) {
        palette->colors[i] = saturate_color(palette->colors[i], amount);
      }
    }
  }
}

static void ensure_contrast(Palette *palette, float contrast, bool light,
                            Color background_color) {
  if (contrast == 1.0f) {
    return;
  }

  if (contrast == 0.0f) {
    return;
  }

  float background_luminance = w3_luminance(background_color);
  // Define a minimum acceptable contrast ratio (e.g., WCAG AA for normal text)
  float MIN_CONTRAST_RATIO =
      light ? MIN_CONTRAST_RATIO_LIGHT : MIN_CONTRAST_RATIO_DARK;

  for (int i = 1; i < 15; i++) {
    Color current_color = palette->colors[i];
    float current_lum = w3_luminance(current_color);

    float actual_contrast = (fmaxf(current_lum, background_luminance) + 0.05f) /
                            (fminf(current_lum, background_luminance) + 0.05f);

    if (actual_contrast >= fmaxf(contrast, MIN_CONTRAST_RATIO)) {
      continue;
    }

    float target_luminance;

    if (background_luminance > 0.5f) {
      target_luminance =
          (background_luminance + 0.05f) / MIN_CONTRAST_RATIO - 0.05f;
      target_luminance = fmaxf(0.0f, target_luminance);
    } else {
      target_luminance =
          (background_luminance + 0.05f) * MIN_CONTRAST_RATIO - 0.05f;
      target_luminance = fminf(1.0f, target_luminance);
    }

    HSV hsv = rgb_to_hsv(current_color);

    palette->colors[i] = binary_luminance_adjust(target_luminance, hsv.h, hsv.s,
                                                 hsv.s, 0.0f, 1.0f, 10);
  }
}

static void generate_16_colors(Palette *palette) {
  if (palette->mode == LIGHT) {
    palette->colors[7] = darken_color(palette->colors[0], 0.60);
    palette->colors[8] = darken_color(palette->colors[0], 0.30);
    palette->colors[15] = darken_color(palette->colors[0], 0.90);
  } else {
    palette->colors[7] = lighten_color(palette->colors[0], 0.60);
    palette->colors[7] = saturate_color(palette->colors[7], 0.05);
    palette->colors[8] = lighten_color(palette->colors[0], 0.40);
    palette->colors[8] = saturate_color(palette->colors[8], 0.10);
    palette->colors[15] = lighten_color(palette->colors[0], 0.80);
  }

  // Generate bright accent colors (9-14) from base accent colors (1-6)
  if (palette->mode == DARK) {
    for (int i = 1; i < 7; i++) {
      palette->colors[i + 8] = lighten_color(palette->colors[i], 0.25);
      palette->colors[i + 8] = saturate_color(palette->colors[i + 8], 0.30);
    }
  } else {
    if (palette->cols16_mode == LIGHTEN) {
      for (int i = 1; i < 7; i++) {
        palette->colors[i + 8] = lighten_color(palette->colors[i], 0.15);
      }
    } else {
      for (int i = 1; i < 7; i++) {
        palette->colors[i + 8] = darken_color(palette->colors[i], 0.15);
      }
    }
  }
}

void process_colors(Palette *palette, float saturation_amount,
                    float contrast_ratio) {
  // First, check if we are in dark mode and boost colors if they are too
  // dark/desaturated
  if (palette->mode == DARK) {
    boost_dark_colors(palette);
  }

  Color background_color = get_average_color(palette->wallpaper);

  // Initial background adjustment based on light/dark mode
  if (palette->mode == LIGHT) {
    float current_lum = w3_luminance(palette->colors[0]);
    if (current_lum <
        MIN_BRIGHTNESS_THRESHOLD) { // If background is not bright enough
      palette->colors[0] = lighten_color(
          palette->colors[0],
          TARGET_LIGHTEN_AMOUNT - current_lum); // Lighten to target luminance
    }
  } else {
    bool saturate_more = (palette->colors[0].red < COLOR_THRESHOLD ||
                          palette->colors[0].green < COLOR_THRESHOLD ||
                          palette->colors[0].blue < COLOR_THRESHOLD);

    if (palette->colors[0].red >= COLOR_THRESHOLD) {
      palette->colors[0] = darken_color(palette->colors[0], DARKEN_AMOUNT);
    }

    if (saturate_more) {
      palette->colors[0] = lighten_color(palette->colors[0], LIGHTEN_AMOUNT);
      palette->colors[0] = saturate_color(palette->colors[0], SATURATE_AMOUNT);
    }
  }

  // Generate the 16 colors
  generate_16_colors(palette);

  // Apply overall saturation and contrast adjustments
  saturate_all_colors(palette, saturation_amount);
  ensure_contrast(palette, contrast_ratio, palette->mode == LIGHT,
                  background_color);
}
