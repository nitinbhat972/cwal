#include "backend.h"
#include "core.h"
#include "utils/utils.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

extern ImageBackend cwal;
extern ImageBackend libimagequant;

static ImageBackend *available_backends[] = {&cwal, &libimagequant, NULL};

ImageBackend *backend_get(const char *name) {
  if (!name || strlen(name) == 0)
    return &cwal;

  for (ImageBackend **backend = available_backends; *backend; backend++) {
    if (strncmp(name, (*backend)->name, strlen(name)) == 0) {
      return (*backend);
    }
  }
  logging(WARN, "cant find %s using default backend %s", name, cwal.name);
  return &cwal;
}

void list_all_backends() {
  logging(INFO, "Avaliable backends:");
  for (ImageBackend **backend = available_backends; *backend; backend++) {
    printf("\t-> %s\n", (*backend)->name);
  }
}

int process_backend(ImageBackend *initial_backend, RawImage *img, Palette *palette) {
  if (!initial_backend || !img || !palette) {
    logging(ERROR, "Null argument passed to process_backend.");
    return -1;
  }

  // Try the initial backend first
  logging(INFO, "Attempting to process with initial backend: %s", initial_backend->name);
  if (initial_backend->init_backend) {
    initial_backend->init_backend();
  }
  if (initial_backend->generate_palette(img, palette) == 0) {
    logging(INFO, "Successfully processed with initial backend: %s", initial_backend->name);
    if (initial_backend->terminate_backend) {
      initial_backend->terminate_backend();
    }
    return 0; // Success
  } else {
    logging(WARN, "Initial backend %s failed to generate palette. Trying other available backends...", initial_backend->name);
    if (initial_backend->terminate_backend) {
      initial_backend->terminate_backend();
    }
  }

  // If initial backend failed, try all other available backends
  for (ImageBackend **backend_ptr = available_backends; *backend_ptr; backend_ptr++) {
    ImageBackend *backend = *backend_ptr;

    // Skip the initial backend as it already failed
    if (backend == initial_backend) {
      continue;
    }

    logging(INFO, "Attempting to process with fallback backend: %s", backend->name);

    if (backend->init_backend) {
      backend->init_backend();
    }

    if (backend->generate_palette(img, palette) == 0) {
      logging(INFO, "Successfully processed with fallback backend: %s", backend->name);
      if (backend->terminate_backend) {
        backend->terminate_backend();
      }
      return 0; // Success
    } else {
      logging(WARN, "Fallback backend %s failed to generate palette. Trying next available backend...", backend->name);
      if (backend->terminate_backend) {
        backend->terminate_backend();
      }
    }
  }

  logging(ERROR, "All available backends failed to process the image.");
  return -1; // All backends failed
}
