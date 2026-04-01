/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "backend.h"
#include "core.h"
#include "lua_backend.h"
#include "utils/path.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

extern ImageBackend cwal;
extern ImageBackend libimagequant;

#define MAX_BACKENDS 64
static ImageBackend *available_backends[MAX_BACKENDS];
static int num_backends = 0;
static char *lua_script_paths[MAX_BACKENDS];
static int num_lua_scripts = 0;

static void init_builtin_backends() {
  available_backends[num_backends++] = &cwal;
  available_backends[num_backends++] = &libimagequant;
  available_backends[num_backends] = NULL;
}

static int is_lua_file(const char *filename) {
  size_t len = strlen(filename);
  return len > 4 && strcmp(filename + len - 4, ".lua") == 0;
}

static char *get_script_name(const char *filepath) {
  const char *basename = strrchr(filepath, '/');
  if (!basename)
    basename = filepath;
  else
    basename++;
  size_t len = strlen(basename);
  if (len > 4 && strcmp(basename + len - 4, ".lua") == 0)
    len -= 4;
  char *name = malloc(len + 1);
  if (!name)
    return NULL;
  strncpy(name, basename, len);
  name[len] = '\0';
  return name;
}

static void scan_lua_backends(void) {
  char *config_home = get_config_home();
  char *backends_dir = build_path(config_home, "cwal", "backends");
  free(config_home);

  if (!backends_dir)
    return;

  if (validate_or_create_dir(backends_dir) != 0) {
    free(backends_dir);
    return;
  }
  DIR *dir = opendir(backends_dir);
  if (!dir) {
    free(backends_dir);
    return;
  }
  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL && num_lua_scripts < MAX_BACKENDS - 2) {
    if (entry->d_type == DT_REG && is_lua_file(entry->d_name)) {
      char *script_path = build_path(backends_dir, entry->d_name);
      if (script_path)
        lua_script_paths[num_lua_scripts++] = script_path;
    }
  }
  closedir(dir);
  free(backends_dir);
}

int is_lua_backend(ImageBackend *backend) {
  for (int i = 0; i < num_lua_scripts; i++) {
    char *script_name = get_script_name(lua_script_paths[i]);
    if (script_name && strcmp(backend->name, script_name) == 0) {
      free(script_name);
      return i;
    }
    free(script_name);
  }
  return -1;
}

static void create_lua_backends() {
  for (int i = 0; i < num_lua_scripts && num_backends < MAX_BACKENDS - 1; i++) {
    char *script_name = get_script_name(lua_script_paths[i]);
    if (!script_name)
      continue;
    ImageBackend *lua_backend = calloc(1, sizeof(ImageBackend));
    if (!lua_backend) {
      free(script_name);
      continue;
    }
    lua_backend->name = script_name;
    lua_backend->init_backend = lua_backend_init;
    lua_backend->terminate_backend = lua_backend_terminate;
    lua_backend->generate_palette = NULL;
    available_backends[num_backends++] = lua_backend;
  }
  available_backends[num_backends] = NULL;
}

static int run_lua_backend(ImageBackend *backend, const char *script_path,
                           const char *image_path, Palette *palette) {
  if (!backend || !script_path || !image_path || !palette) {
    return -1;
  }

  if (backend->init_backend) {
    backend->init_backend();
  }

  int status = lua_generate_palette(script_path, image_path, palette);

  if (backend->terminate_backend) {
    backend->terminate_backend();
  }

  return status;
}

static int run_raw_backend(ImageBackend *backend, RawImage *raw_img,
                           Palette *palette) {
  if (!backend || !raw_img || !palette || !backend->generate_palette) {
    return -1;
  }

  if (backend->init_backend) {
    backend->init_backend();
  }

  int status = backend->generate_palette(raw_img, palette);

  if (backend->terminate_backend) {
    backend->terminate_backend();
  }

  return status;
}

int process_with_fallback(ImageBackend *backend, const char *image_path,
                          Palette *palette) {
  if (!backend || !image_path || !palette) {
    return -1;
  }

  RawImage *raw_img = NULL;
  bool processed = false;

  int lua_index = is_lua_backend(backend);
  if (lua_index >= 0) {
    processed = run_lua_backend(backend, lua_script_paths[lua_index],
                                image_path, palette) == 0;
  } else {
    raw_img = image_load_from_file(image_path);
    if (raw_img) {
      processed = run_raw_backend(backend, raw_img, palette) == 0;
    }
  }

  for (ImageBackend **backend_ptr = available_backends; *backend_ptr;
       backend_ptr++) {
    if (processed) {
      break;
    }

    ImageBackend *fallback = *backend_ptr;
    if (fallback == backend) {
      continue;
    }

    int lua_idx = is_lua_backend(fallback);
    if (lua_idx >= 0) {
      processed = run_lua_backend(fallback, lua_script_paths[lua_idx],
                                  image_path, palette) == 0;
    } else {
      if (!raw_img) {
        raw_img = image_load_from_file(image_path);
      }

      if (raw_img) {
        processed = run_raw_backend(fallback, raw_img, palette) == 0;
      }
    }
  }

  if (raw_img) {
    image_free(raw_img);
  }
  return processed ? 0 : -1;
}

void init_backends() {
  num_backends = 0;
  num_lua_scripts = 0;
  init_builtin_backends();
  scan_lua_backends();
  create_lua_backends();
}

ImageBackend *backend_get(const char *name) {
  if (!name || strlen(name) == 0)
    return &cwal;
  for (ImageBackend **backend = available_backends; *backend; backend++) {
    if (strcmp(name, (*backend)->name) == 0)
      return (*backend);
  }
  return NULL;
}

void list_all_backends() {
  for (ImageBackend **backend = available_backends; *backend; backend++) {
    printf("\t-> %s\n", (*backend)->name);
  }
}

int process_backend(ImageBackend *initial_backend, RawImage *img,
                    Palette *palette) {
  if (!initial_backend || !img || !palette)
    return -1;
  if (initial_backend->init_backend)
    initial_backend->init_backend();
  if (initial_backend->generate_palette(img, palette) == 0) {
    if (initial_backend->terminate_backend)
      initial_backend->terminate_backend();
    return 0;
  }
  if (initial_backend->terminate_backend)
    initial_backend->terminate_backend();
  for (ImageBackend **backend_ptr = available_backends; *backend_ptr;
       backend_ptr++) {
    ImageBackend *backend = *backend_ptr;
    if (backend == initial_backend)
      continue;
    if (backend->init_backend)
      backend->init_backend();
    if (backend->generate_palette(img, palette) == 0) {
      if (backend->terminate_backend)
        backend->terminate_backend();
      return 0;
    }
    if (backend->terminate_backend)
      backend->terminate_backend();
  }
  return -1;
}
