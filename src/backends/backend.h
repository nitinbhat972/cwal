/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#pragma once
#include "color/image.h"
#include "core.h"

typedef struct {
  const char *name;
  void (*init_backend)(void);
  void (*terminate_backend)(void);
  int (*generate_palette)(RawImage *image, Palette *palette);
} ImageBackend;

ImageBackend *backend_get(const char *name);
void list_all_backends(void);
int process_backend(ImageBackend *backend, RawImage *img, Palette *palette);
int process_with_fallback(ImageBackend *backend, const char *image_path,
                          Palette *palette);
void init_backends(void);
int is_lua_backend(ImageBackend *backend);
