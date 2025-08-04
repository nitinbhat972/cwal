#include "color_convertion.h"
#include "core.h"
#include "utils/utils.h"
#include <math.h>
#include <stdint.h>

HSL rgb_to_hsl(Color clr) {
  float r = clr.red / 255.0f;
  float g = clr.green / 255.0f;
  float b = clr.blue / 255.0f;

  float max = fmaxf(fmaxf(r, g), b);
  float min = fminf(fminf(r, g), b);
  float sum = max + min;
  float range = max - min;

  float l = sum / 2.0f;
  float h = 0.0f, s = 0.0f;

  if (range != 0.0f) {
    s = (l <= 0.5f) ? (range / sum) : (range / (2.0f - sum));

    if (r == max)
      h = (g - b) / range;
    else if (g == max)
      h = 2.0f + (b - r) / range;
    else
      h = 4.0f + (r - g) / range;

    h = fmodf(h / 6.0f, 1.0f);
    if (h < 0.0f)
      h += 1.0f;
  }

  HSL hls = {h, l, s};
  return hls;
}

static float calc(float t, float temp1, float temp2) {
  if (6.0f * t < 1.0f)
    return temp2 + (temp1 - temp2) * 6.0f * t;
  if (2.0f * t < 1.0f)
    return temp1;
  if (3.0f * t < 2.0f)
    return temp2 + (temp1 - temp2) * (2.0f / 3.0f - t) * 6.0f;
  return temp2;
}

Color hls_to_rgb(HSL hls) {
  float h = hls.h, s = hls.s, l = hls.l;
  float r, g, b;

  if (s == 0.0f) {
    r = g = b = l;
  } else {
    float temp1 = (l < 0.5f) ? (l * (1.0f + s)) : (l + s - l * s);
    float temp2 = 2.0f * l - temp1;

    float tempr = h + 1.0f / 3.0f;
    float tempg = h;
    float tempb = h - 1.0f / 3.0f;

    tempr = fmodf(tempr, 1.0f);
    tempb = fmodf(tempb + 1.0f, 1.0f);

    r = calc(tempr, temp1, temp2);
    g = calc(tempg, temp1, temp2);
    b = calc(tempb, temp1, temp2);
  }

  Color color = {
      .red = clamp_byte(r * 255.0f),
      .green = clamp_byte(g * 255.0f),
      .blue = clamp_byte(b * 255.0f),
  };
  return color;
}

HSV rgb_to_hsv(Color c) {
  float r = c.red / 255.0f;
  float g = c.green / 255.0f;
  float b = c.blue / 255.0f;
  float h, s, v;

  float max = fmaxf(r, fmaxf(g, b));
  float min = fminf(r, fminf(g, b));
  float delta = max - min;

  v = max;

  if (delta < 0.00001f) {
    h = 0.0f;
    s = 0.0f;
  } else {
    s = delta / max;

    if (r >= max)
      h = (g - b) / delta;
    else if (g >= max)
      h = 2.0f + (b - r) / delta;
    else
      h = 4.0f + (r - g) / delta;

    h *= 60.0f;
    if (h < 0.0f)
      h += 360.0f;
  }

  HSV hsv = {h, s, v};
  return hsv;
}

Color hsv_to_rgb(HSV hsv) {
  float h = hsv.h, s = hsv.s, v = hsv.v;
  float r, g, b;

  if (s == 0.0f) {
    r = g = b = v;
  } else {
    h /= 60.0f;
    int i = (int)h;
    float f = h - i;
    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (i) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
    }
  }

  Color color = {
      .red = clamp_byte(r * 255.0f),
      .green = clamp_byte(g * 255.0f),
      .blue = clamp_byte(b * 255.0f),
  };
  return color;
}
