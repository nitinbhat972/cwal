#include "color_operation.h"
#include "color_convertion.h"
#include "core.h"
#include "image.h"
#include "utils/utils.h"
#include <math.h>
#include <stdio.h>

Color darken_color(Color clr, float amount) {
  amount = clamp_value(amount);
  return (Color){
      .red = (uint8_t)(clr.red * (1.0f - amount)),
      .green = (uint8_t)(clr.green * (1.0f - amount)),
      .blue = (uint8_t)(clr.blue * (1.0f - amount)),
  };
}

Color lighten_color(Color clr, float amount) {
  amount = clamp_value(amount);
  return (Color){
      .red = (uint8_t)(clr.red + (255 - clr.red) * amount),
      .green = (uint8_t)(clr.green + (255 - clr.green) * amount),
      .blue = (uint8_t)(clr.blue + (255 - clr.blue) * amount),
  };
}

Color saturate_color(Color clr, float amount) {
  HSL hsl = rgb_to_hsl(clr);
  hsl.s = clamp_value(hsl.s + amount);
  Color result = hls_to_rgb(hsl);
  return result;
}

float w3_luminance(Color clr) {
  float channels[3] = {clr.red / 255.0f, clr.green / 255.0f, clr.blue / 255.0f};

  for (int i = 0; i < 3; ++i) {
    channels[i] = (channels[i] <= 0.04045f)
                      ? channels[i] / 12.92f
                      : powf((channels[i] + 0.055f) / 1.055f, 2.4f);
  }

  return 0.2126f * channels[0] + 0.7152f * channels[1] + 0.0722f * channels[2];
}

Color binary_luminance_adjust(float luminance_desired, float hue, float s_min,
                              float s_max, float v_min, float v_max,
                              int iterations) {
  HSV result = {hue, (s_min + s_max) / 2, (v_min + v_max) / 2, 1.0f};

  for (int i = 0; i < iterations; i++) {
    result.s = (s_min + s_max) / 2.0f;
    result.v = (v_min + v_max) / 2.0f;

    Color test_color = hsv_to_rgb(result);
    float current_lum = w3_luminance(test_color);

    if (current_lum >= luminance_desired) {
      s_min = result.s;
      v_max = result.v;
    } else {
      s_max = result.s;
      v_min = result.v;
    }
  }

  result.s = (s_min + s_max) / 2.0f;
  result.v = (v_min + v_max) / 2.0f;
  return hsv_to_rgb(result);
}

Color get_average_color(const char *filename) {
  RawImage *img = image_load_from_file(filename);

  if (!img) {
    fprintf(stderr, "Error: Failed to load image for average color calculation: %s\n", filename);
    return (Color){0, 0, 0}; // Return black color on failure
  }

  uint64_t r_sum = 0, g_sum = 0, b_sum = 0, a_sum = 0;
  int pixel_count = img->width * img->height;

  for (int i = 0; i < pixel_count; ++i) {
    r_sum += img->pixels[i * 4 + 0];
    g_sum += img->pixels[i * 4 + 1];
    b_sum += img->pixels[i * 4 + 2];
    a_sum += (img->channels == 4) ? img->pixels[i * 4 + 3] : 255;
  }

  image_free(img);

  return (Color){
      .red = (uint8_t)(r_sum / pixel_count),
      .green = (uint8_t)(g_sum / pixel_count),
      .blue = (uint8_t)(b_sum / pixel_count),
  };
}

float calculate_contrast_ratio(Color color1, Color color2) {
    float lum1 = w3_luminance(color1);
    float lum2 = w3_luminance(color2);

    if (lum1 > lum2) {
        return (lum1 + 0.05f) / (lum2 + 0.05f);
    } else {
        return (lum2 + 0.05f) / (lum1 + 0.05f);
    }
}
