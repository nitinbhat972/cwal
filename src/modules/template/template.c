/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "template.h"
#include "core.h"
#include "utils/format_conversion.h"
#include "utils/path.h"
#include "utils/utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define FMT_COUNT 9
#define COLOR_STR_MAX 32

typedef struct {
  char fmt[FMT_COUNT][COLOR_STR_MAX];
} ColorEntry;

typedef struct {
  ColorEntry colors[PALETTE_MAX_SIZE];
  char wallpaper[4096];
  float alpha;
} ColorTable;

static const char *fmt_names[] = {"hex", "xhex", "strip", "rgb", "rgba", "red", "green", "blue", "alpha_dec"};

static ColorTable *build_color_table(const Palette *palette) {
  ColorTable *ct = malloc(sizeof(ColorTable));
  if (!ct) return NULL;
  ct->alpha = palette->alpha;
  snprintf(ct->wallpaper, sizeof(ct->wallpaper), "%s", palette->wallpaper ? palette->wallpaper : "");
  for (int i = 0; i < PALETTE_MAX_SIZE; i++) {
    for (int f = 0; f < FMT_COUNT; f++) {
      format_color(&palette->colors[i], fmt_names[f], ct->colors[i].fmt[f], COLOR_STR_MAX, palette->alpha);
    }
  }
  return ct;
}

static const char *resolve_placeholder(const char *placeholder, const ColorTable *ct) {
  char key[64] = {0};
  int fmt_idx = 0; // default hex
  const char *dot = strrchr(placeholder, '.');

  if (dot && dot != placeholder) {
    size_t key_len = (size_t)(dot - placeholder);
    if (key_len >= sizeof(key)) key_len = sizeof(key) - 1;
    memcpy(key, placeholder, key_len);
    key[key_len] = '\0';
    const char *fmt_str = dot + 1;
    int format_found = 0;
    for (int f = 0; f < FMT_COUNT; f++) {
      if (strcmp(fmt_str, fmt_names[f]) == 0) {
        fmt_idx = f;
        format_found = 1;
        break;
      }
    }
    if (!format_found) return NULL;
  } else {
    strncpy(key, placeholder, sizeof(key) - 1);
  }

  if (strncmp(key, "color", 5) == 0 && key[5] != '\0') {
    char *endptr;
    long index = strtol(key + 5, &endptr, 10);
    if (*endptr == '\0' && index >= 0 && index < PALETTE_MAX_SIZE)
      return ct->colors[index].fmt[fmt_idx];
  } else if (strcmp(key, "background") == 0) {
    return ct->colors[0].fmt[fmt_idx];
  } else if (strcmp(key, "foreground") == 0 || strcmp(key, "cursor") == 0) {
    return ct->colors[PALETTE_MAX_SIZE - 1].fmt[fmt_idx];
  } else if (strcmp(key, "wallpaper") == 0 && ct->wallpaper[0]) {
    return ct->wallpaper;
  }
  return NULL;
}

static void replacement(FILE *in, FILE *out, const ColorTable *ct) {
  fseek(in, 0, SEEK_END);
  long size = ftell(in);
  fseek(in, 0, SEEK_SET);
  if (size <= 0) return;

  char *buf = malloc(size + 1);
  if (!buf) return;
  size_t read_size = fread(buf, 1, size, in);
  buf[read_size] = '\0';

  size_t out_len = 0;
  char *p = buf;
  while (*p) {
    char *brace = strchr(p, '{');
    if (!brace) { out_len += strlen(p); break; }
    out_len += (size_t)(brace - p);
    char *close = strchr(brace + 1, '}');
    if (!close) { out_len += strlen(brace); break; }
    
    char name[128] = {0};
    size_t len = (size_t)(close - (brace + 1));
    if (len < sizeof(name)) {
      memcpy(name, brace + 1, len);
      const char *res = resolve_placeholder(name, ct);
      out_len += res ? strlen(res) : (len + 2);
    } else out_len += 1;
    p = close + 1;
  }

  char *result = malloc(out_len + 1);
  if (!result) { free(buf); return; }
  size_t pos = 0; p = buf;
  while (*p) {
    char *brace = strchr(p, '{');
    if (!brace) { strcpy(result + pos, p); pos += strlen(p); break; }
    size_t before = (size_t)(brace - p);
    memcpy(result + pos, p, before); pos += before;
    char *close = strchr(brace + 1, '}');
    if (!close) { strcpy(result + pos, brace); pos += strlen(brace); break; }
    
    char name[128] = {0};
    size_t len = (size_t)(close - (brace + 1));
    if (len < sizeof(name)) {
      memcpy(name, brace + 1, len);
      const char *res = resolve_placeholder(name, ct);
      if (res) { size_t rlen = strlen(res); memcpy(result + pos, res, rlen); pos += rlen; }
      else { memcpy(result + pos, brace, len + 2); pos += len + 2; }
    } else result[pos++] = '{';
    p = close + 1;
  }
  result[pos] = '\0';
  fwrite(result, 1, pos, out);
  free(result); free(buf);
}

static void generate_sequence(const char *out_path, const Palette *palette) {
  char seq[4096];
  int n = 0;
  for (int i = 0; i < 16; i++)
    n += snprintf(seq + n, sizeof(seq) - n, "\033]4;%d;#%02x%02x%02x\033\\", i, palette->colors[i].red, palette->colors[i].green, palette->colors[i].blue);
  const Color *fg = &palette->colors[15], *bg = &palette->colors[0];
  n += snprintf(seq + n, sizeof(seq) - n, "\033]10;#%02x%02x%02x\033\\\033]11;#%02x%02x%02x\033\\\033]12;#%02x%02x%02x\033\\\033]13;#%02x%02x%02x\033\\\033]17;#%02x%02x%02x\033\\\033]19;#%02x%02x%02x\033\\\033]4;232;#%02x%02x%02x\033\\\033]4;256;#%02x%02x%02x\033\\\033]4;257;#%02x%02x%02x\033\\\033]708;#%02x%02x%02x\033\\",
    fg->red, fg->green, fg->blue, bg->red, bg->green, bg->blue, fg->red, fg->green, fg->blue, fg->red, fg->green, fg->blue, fg->red, fg->green, fg->blue, bg->red, bg->green, bg->blue, bg->red, bg->green, bg->blue, fg->red, fg->green, fg->blue, bg->red, bg->green, bg->blue, bg->red, bg->green, bg->blue);
  FILE *out = fopen(out_path, "w");
  if (out) { fwrite(seq, 1, n, out); fclose(out); }
}

static void process_dir(const char *current_dir, const char *out_base, const ColorTable *ct) {
  struct stat st;
  if (stat(current_dir, &st) == -1 || !S_ISDIR(st.st_mode)) return;
  logging(INFO, "Processing templates from %s", current_dir);
  DIR *dir = opendir(current_dir);
  if (!dir) return;
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_name[0] == '.') continue;
    char *full_in = build_path(current_dir, entry->d_name);
    struct stat fst;
    if (stat(full_in, &fst) == 0 && S_ISREG(fst.st_mode)) {
      char *full_out = build_path(out_base, entry->d_name);
      FILE *in = fopen(full_in, "r"), *out = fopen(full_out, "w");
      if (in && out) replacement(in, out, ct);
      if (in) fclose(in); if (out) fclose(out);
      free(full_out);
    }
    free(full_in);
  }
  closedir(dir);
}

int process_template(const char *output_dir, const Palette *palette) {
  char *out_base = build_path(output_dir, "");
  if (validate_or_create_dir(out_base) == -1) { free(out_base); return 1; }
  ColorTable *ct = build_color_table(palette);
  if (!ct) { free(out_base); return 1; }

  char **system_dirs = get_data_dirs();
  if (system_dirs) {
    for (int i = 0; system_dirs[i]; i++) {
      char *p = build_path(system_dirs[i], "cwal", "templates");
      process_dir(p, out_base, ct);
      free(p); free(system_dirs[i]);
    }
    free(system_dirs);
  }

  char *data_home = get_data_home(), *config_home = get_config_home();
  char *d1 = build_path(data_home, "cwal", "templates"), *d2 = build_path(config_home, "cwal", "templates");
  process_dir(d1, out_base, ct); process_dir(d2, out_base, ct);
  
  generate_sequence(build_path(out_base, "sequences"), palette);
  free(data_home); free(config_home); free(d1); free(d2); free(ct); free(out_base);
  return 0;
}
