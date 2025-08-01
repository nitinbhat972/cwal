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
    snprintf(out, out_size, "rgba(%d,%d,%d,%.1f)", color->red, color->green,
             color->blue, alpha);
  } else if (strcmp(ft, "red") == 0) {
    snprintf(out, out_size, "%d", color->red);
  } else if (strcmp(ft, "green") == 0) {
    snprintf(out, out_size, "%d", color->green);
  } else if (strcmp(ft, "blue") == 0) {
    snprintf(out, out_size, "%d", color->blue);
  } else if (strcmp(ft, "alpha_dec") == 0) {
    snprintf(out, out_size, "%.1f", alpha);
  } else {
    snprintf(out, out_size, "{color.%s}", ft);
  }
}
