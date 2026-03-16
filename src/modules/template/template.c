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
#include <ctype.h>
#include <dirent.h>
#include <pthread.h>
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
    size_t key_len = dot - placeholder;
    if (key_len >= sizeof(key)) key_len = sizeof(key) - 1;
    strncpy(key, placeholder, key_len);
    key[key_len] = '\0';
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
  if (size <= 0) return;

  char *buffer = malloc(size + 1);
  if (!buffer)
    return;
  size_t read_size = fread(buffer, 1, size, in);
  buffer[read_size] = '\0';

  size_t result_cap = size * 2 + 1024;
  char *result = malloc(result_cap);
  if (!result) {
    free(buffer);
    return;
  }
  size_t result_len = 0;

  char *cursor = buffer;
  while (*cursor) {
    char *start = strchr(cursor, '{');
    if (!start) {
      size_t remaining = strlen(cursor);
      if (result_len + remaining >= result_cap) {
        result_cap = result_len + remaining + 1;
        result = realloc(result, result_cap);
      }
      strcpy(result + result_len, cursor);
      result_len += remaining;
      break;
    }

    // Copy text before '{'
    size_t before_len = start - cursor;
    if (result_len + before_len >= result_cap) {
        result_cap = (result_len + before_len) * 2;
        result = realloc(result, result_cap);
    }
    memcpy(result + result_len, cursor, before_len);
    result_len += before_len;
    result[result_len] = '\0';

    char *end = strchr(start + 1, '}');
    if (!end) {
      // No matching '}', copy the rest and finish
      size_t remaining = strlen(start);
      if (result_len + remaining >= result_cap) {
        result_cap = result_len + remaining + 1;
        result = realloc(result, result_cap);
      }
      strcpy(result + result_len, start);
      result_len += remaining;
      break;
    }

    size_t p_len = end - (start + 1);
    char p_name[128];
    if (p_len >= sizeof(p_name)) {
      if (result_len + 1 >= result_cap) {
          result_cap *= 2;
          result = realloc(result, result_cap);
      }
      result[result_len++] = '{';
      result[result_len] = '\0';
      cursor = start + 1;
      continue;
    }
    memcpy(p_name, start + 1, p_len);
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
        memcpy(result + result_len, resolved, resolved_len);
        result_len += resolved_len;
        result[result_len] = '\0';
        free(resolved);
      }
      cursor = end + 1;
    } else {
      if (result_len + 1 >= result_cap) {
          result_cap *= 2;
          result = realloc(result, result_cap);
      }
      result[result_len++] = '{';
      result[result_len] = '\0';
      cursor = start + 1;
    }
  }

  fputs(result, out);
  free(buffer);
  free(result);
}

typedef struct {
  char *in_path;
  char *out_path;
  const Palette *palette;
} TemplateTask;

static void *template_worker(void *arg) {
  TemplateTask *task = (TemplateTask *)arg;
  
  FILE *in = fopen(task->in_path, "r");
  if (in) {
    FILE *out = fopen(task->out_path, "w");
    if (out) {
      replacement(in, out, task->palette);
      fclose(out);
    }
    fclose(in);
  }

  free(task->in_path);
  free(task->out_path);
  free(task);
  return NULL;
}

static void process_dir(const char *current_dir, const char *out_base, const Palette *palette) {
  struct stat st = {0};
  if (stat(current_dir, &st) == -1 || !S_ISDIR(st.st_mode)) {
    return;
  }

  logging(INFO, "Processing templates from %s", current_dir);
  DIR *dir = opendir(current_dir);
  if (!dir) return;

  struct dirent *entry;
  pthread_t threads[256]; 
  int thread_count = 0;

  while ((entry = readdir(dir)) != NULL && thread_count < 256) {
    if (entry->d_name[0] == '.') continue;

    char *full_in = build_path(current_dir, entry->d_name);
    struct stat fst;
    if (stat(full_in, &fst) == 0 && S_ISREG(fst.st_mode)) {
      TemplateTask *task = malloc(sizeof(TemplateTask));
      if (!task) {
          free(full_in);
          continue;
      }
      task->in_path = full_in;
      task->out_path = build_path(out_base, entry->d_name);
      task->palette = palette;

      if (pthread_create(&threads[thread_count], NULL, template_worker, task) != 0) {
          free(task->in_path);
          free(task->out_path);
          free(task);
      } else {
          thread_count++;
      }
    } else {
      free(full_in);
    }
  }
  closedir(dir);

  for (int i = 0; i < thread_count; i++) {
    pthread_join(threads[i], NULL);
  }
}

int process_template(const char *output_dir, const Palette *palette) {
  char *out_base = build_path(output_dir, "");

  if (validate_or_create_dir(out_base) == -1) {
    logging(ERROR, "Failed to validate or create output directory: %s",
            out_base);
    free(out_base);
    return 1;
  }

  // 1. Process System Data Directories
  char **system_dirs = get_data_dirs();
  if (system_dirs) {
    for (int i = 0; system_dirs[i] != NULL; i++) {
      char *full_path = build_path(system_dirs[i], "cwal", "templates");
      process_dir(full_path, out_base, palette);
      free(full_path);
      free(system_dirs[i]);
    }
    free(system_dirs);
  }

  // 2. Process User Local Data
  char *data_home = get_data_home();
  char *user_local_template_dir = build_path(data_home, "cwal", "templates");
  process_dir(user_local_template_dir, out_base, palette);
  free(data_home);
  free(user_local_template_dir);

  // 3. Process User Config
  char *config_home = get_config_home();
  char *user_config_template_dir = build_path(config_home, "cwal", "templates");
  process_dir(user_config_template_dir, out_base, palette);
  free(config_home);
  free(user_config_template_dir);

  char *out_path = build_path(out_base, "sequences");
  generate_sequence(out_path, palette);
  free(out_path);
  free(out_base);

  return 0;
}
