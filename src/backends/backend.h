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
