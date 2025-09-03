#include "template.h"
#include "core.h"
#include "utils/format_conversion.h"
#include "utils/path.h"
#include "utils/utils.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_RESULT_SIZE 256

static void generate_sequence(const char *out_path, const Palette *palette) {
  if (!out_path || !palette) {
    fprintf(stderr, "Invalid arguments\n");
    return;
  }

  FILE *out = fopen(out_path, "w");
  if (!out) {
    perror("Error opening file");
    return;
  }

  for (int i = 0; i < 16; ++i) {
    fprintf(out, "\033]4;%d;#%02x%02x%02x\033\\", i, palette->colors[i].red,
            palette->colors[i].green, palette->colors[i].blue);
  }

  Color fg = palette->colors[15];
  Color bg = palette->colors[0];

  fprintf(out, "\033]10;#%02x%02x%02x\033\\", fg.red, fg.green, fg.blue);
  fprintf(out, "\033]11;#%02x%02x%02x\033\\", bg.red, bg.green, bg.blue);
  fprintf(out, "\033]12;#%02x%02x%02x\033\\", fg.red, fg.green, fg.blue);
  fprintf(out, "\033]13;#%02x%02x%02x\033\\", fg.red, fg.green, fg.blue);
  fprintf(out, "\033]17;#%02x%02x%02x\033\\", fg.red, fg.green, fg.blue);
  fprintf(out, "\033]19;#%02x%02x%02x\033\\", bg.red, bg.green, bg.blue);
  fprintf(out, "\033]4;232;#%02x%02x%02x\033\\", bg.red, bg.green, bg.blue);
  fprintf(out, "\033]4;256;#%02x%02x%02x\033\\", fg.red, fg.green, fg.blue);
  fprintf(out, "\033]4;257;#%02x%02x%02x\033\\", bg.red, bg.green, bg.blue);
  fprintf(out, "\033]708;#%02x%02x%02x\033\\", bg.red, bg.green, bg.blue);

  fclose(out);
}

static char *resolve_placeholder(const char *placeholder,
                                 const Palette *palette) {
  char key[64] = {0};
  char format[32] = "hex";
  const char *dot = strrchr(placeholder, '.');

  if (dot && dot != placeholder) {
    strncpy(key, placeholder, dot - placeholder);
    key[dot - placeholder] = '\0';
    strncpy(format, dot + 1, sizeof(format) - 1);
    format[sizeof(format) - 1] = '\0';
  } else {
    strncpy(key, placeholder, sizeof(key) - 1);
    key[sizeof(key) - 1] = '\0';
  }

  if (strncmp(format, "value", 6) == 0) {
    snprintf(format, sizeof(format), "%s", "hex");
  }

  char *result = malloc(MAX_RESULT_SIZE);
  if (!result)
    return NULL;

  if (strncmp(key, "color", 5) == 0) {
    const char *num_str = key + 5;
    if (*num_str) {
      char *endptr;
      long index = strtol(num_str, &endptr, 10);
      if (*endptr == '\0' && index >= 0 && index < PALETTE_MAX_SIZE) {
        format_color(&palette->colors[index], format, result, MAX_RESULT_SIZE,
                     palette->alpha);
      } else {
        snprintf(result, MAX_RESULT_SIZE, "{%s}", placeholder);
      }
    } else {
      snprintf(result, MAX_RESULT_SIZE, "{%s}", placeholder);
    }
  } else if (strncmp(key, "background", 11) == 0) {
    format_color(&palette->colors[0], format, result, MAX_RESULT_SIZE, palette->alpha);
  } else if ((strncmp(key, "foreground", 11) == 0 ||
              strncmp(key, "cursor", 7) == 0)) {
    format_color(&palette->colors[PALETTE_MAX_SIZE - 1], format, result, MAX_RESULT_SIZE,
                 palette->alpha);
  } else if (strncmp(key, "wallpaper", 10) == 0 && palette->wallpaper) {
    snprintf(result, MAX_RESULT_SIZE, "%s", palette->wallpaper);
  } else {
    snprintf(result, MAX_RESULT_SIZE, "{%s}", placeholder);
  }

  return result;
}

static void replacement(FILE *in, FILE *out, const Palette *palette) {
  fseek(in, 0, SEEK_END);
  long size = ftell(in);
  fseek(in, 0, SEEK_SET);
  char *buffer = malloc(size + 1);
  if (!buffer)
    return;
  fread(buffer, 1, size, in);
  buffer[size] = '\0';

  size_t result_cap = size * 2;
  char *result = malloc(result_cap);
  if (!result) {
    free(buffer);
    return;
  }
  result[0] = '\0';
  size_t result_len = 0;

  char *cursor = buffer;
  while (*cursor) {
    char *start = strchr(cursor, '{');
    if (!start) {
      size_t remaining = result_cap - strlen(result);
      snprintf(result + strlen(result), remaining, "%s", cursor);
      break;
    }

    strncat(result, cursor, start - cursor);
    result_len += (start - cursor);

    char *end = strchr(start + 1, '}');
    if (!end) {
      size_t remaining = result_cap - strlen(result);
      snprintf(result + strlen(result), remaining, "%s", start);
      break;
    }

    size_t p_len = end - (start + 1);
    char p_name[128];
    if (p_len >= sizeof(p_name)) {
      strncat(result, start, 1);
      result_len++;
      cursor = start + 1;
      continue;
    }
    strncpy(p_name, start + 1, p_len);
    p_name[p_len] = '\0';

    int is_valid = 1;
    for (size_t i = 0; i < p_len; i++) {
      if (!isalnum(p_name[i]) && p_name[i] != '.' && p_name[i] != '_') {
        is_valid = 0;
        break;
      }
    }

    if (is_valid) {
      char *resolved = resolve_placeholder(p_name, palette);
      if (resolved) {
        size_t resolved_len = strlen(resolved);
        if (result_len + resolved_len >= result_cap) {
          result_cap = (result_len + resolved_len) * 2;
          result = realloc(result, result_cap);
        }
        size_t remaining = result_cap - result_len;
        snprintf(result + result_len, remaining, "%s", resolved);
        result_len += resolved_len;
        free(resolved);
      }
      cursor = end + 1;
    } else {
      strncat(result, start, 1);
      result_len++;
      cursor = start + 1;
    }
  }

  fputs(result, out);
  free(buffer);
  free(result);
}

int process_template(const char *output_dir, const Palette *palette) {
  char *out_base = build_path(output_dir, "");

  // Define template search paths
  const char *system_template_dir = "/usr/local/share/cwal/templates/";
  char *user_local_template_dir = expand_home("~/.local/share/cwal/templates/");
  char *user_config_template_dir = expand_home("~/.config/cwal/templates/");

  const char *template_dirs[] = {system_template_dir, user_local_template_dir,
                                 user_config_template_dir};
  int num_template_dirs = sizeof(template_dirs) / sizeof(template_dirs[0]);

  if (validate_or_create_dir(out_base) == -1) {
    logging(ERROR, "Failed to validate or create output directory: %s",
            out_base);
    free(out_base);
    return 1;
  }

  for (int i = 0; i < num_template_dirs; ++i) {
    const char *current_template_dir = template_dirs[i];
    if (current_template_dir == NULL) {
      continue;
    }

    struct stat st = {0};
    if (stat(current_template_dir, &st) == -1 || !S_ISDIR(st.st_mode)) {
      logging(INFO, "Skipping non-existent template directory: %s",
              current_template_dir);
      continue;
    }

    logging(INFO, "Processing templates from %s", current_template_dir);
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(current_template_dir)) != NULL) {
      while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.')
          continue;

        char *in_path = build_path(current_template_dir, entry->d_name);
        char *out_path = build_path(out_base, entry->d_name);

        FILE *in = fopen(in_path, "r");
        if (!in) {
          logging(WARN, "Could not open template file %s", in_path);
          free(in_path);
          free(out_path);
          continue;
        }
        FILE *out_file = fopen(out_path, "w");
        if (!out_file) {
          logging(WARN, "Could not open output file %s for template %s",
                  out_path, entry->d_name);
          fclose(in);
          free(in_path);
          free(out_path);
          continue;
        }
        replacement(in, out_file, palette);
        fclose(in);
        fclose(out_file);
        free(in_path);
        free(out_path);
      }
      closedir(dir);
    }
  }

  // Free memory allocated by expand_home
  free(user_local_template_dir);
  free(user_config_template_dir);

  char *out_path = build_path(out_base, "sequences");
  generate_sequence(out_path, palette);
  free(out_path);
  free(out_base);

  return 0;
}
