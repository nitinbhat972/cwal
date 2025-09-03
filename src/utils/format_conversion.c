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

  if (strncmp(ft, "hex", 4) == 0) {
    snprintf(out, out_size, "#%02x%02x%02x", color->red, color->green,
             color->blue);
  } else if (strncmp(ft, "xhex", 5) == 0) {
    snprintf(out, out_size, "0x%02x%02x%02x", color->red, color->green,
             color->blue);
  } else if (strncmp(ft, "strip", 6) == 0) {
    snprintf(out, out_size, "%02x%02x%02x", color->red, color->green,
             color->blue);
  } else if (strncmp(ft, "rgb", 4) == 0) {
    snprintf(out, out_size, "rgb(%d,%d,%d)", color->red, color->green,
             color->blue);
  } else if (strncmp(ft, "rgba", 5) == 0) {
    snprintf(out, out_size, "rgba(%d,%d,%d,%.1f)", color->red, color->green,
             color->blue, alpha);
  } else if (strncmp(ft, "red", 4) == 0) {
    snprintf(out, out_size, "%d", color->red);
  } else if (strncmp(ft, "green", 6) == 0) {
    snprintf(out, out_size, "%d", color->green);
  } else if (strncmp(ft, "blue", 5) == 0) {
    snprintf(out, out_size, "%d", color->blue);
  } else if (strncmp(ft, "alpha_dec", 10) == 0) {
    snprintf(out, out_size, "%.1f", alpha);
  } else {
    snprintf(out, out_size, "{color.%s}", ft);
  }
}
