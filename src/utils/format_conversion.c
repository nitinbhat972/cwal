#include "format_conversion.h"
#include "core.h"
#include <string.h>

void format_color(const Color *color, const char *ft, char *out,
                  size_t out_size, float alpha) {
  if (!color || !ft || !out || out_size == 0) {
    if (out && out_size > 0)
      out[0] = '\0';
    return;
  }

  size_t alpha_num;
  if (alpha <= 0) {
    alpha_num = 100;
  }
  else{
    alpha_num = alpha * 100;
  }
  float alpha_dec = (float)alpha_num / 100;

  if (strcmp(ft, "hex") == 0) {
    snprintf(out, out_size, "#%02x%02x%02x", color->red, color->green,
             color->blue);
  } else if (strcmp(ft, "xhex") == 0) {
    snprintf(out, out_size, "0x%02x%02x%02x", color->red, color->green,
             color->blue);
  } else if (strcmp(ft, "strip") == 0) {
    snprintf(out, out_size, "%02x%02x%02x", color->red, color->green,
             color->blue);
  } else if (strcmp(ft, "rgb") == 0) {
    snprintf(out, out_size, "rgb(%d,%d,%d)", color->red, color->green,
             color->blue);
  } else if (strcmp(ft, "rgba") == 0) {
    snprintf(out, out_size, "rgba(%d,%d,%d,%0.1f)", color->red, color->green,
             color->blue, alpha_dec);
  } else if (strcmp(ft, "red") == 0) {
    snprintf(out, out_size, "%d", color->red);
  } else if (strcmp(ft, "green") == 0) {
    snprintf(out, out_size, "%d", color->green);
  } else if (strcmp(ft, "blue") == 0) {
    snprintf(out, out_size, "%d", color->blue);
  } else if (strcmp(ft, "alpha_dec") == 0) {
    snprintf(out, out_size, "%0.1f", alpha_dec);
  } else {
    snprintf(out, out_size, "{color.%s}", ft);
  }
}
