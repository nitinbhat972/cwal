#include "image.h"
#include <MagickWand/MagickWand.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Initialize MagickWand on first use.
void init_magickwand_once() {
  static int is_initialized = 0;
  if (!is_initialized) {
    MagickWandGenesis();
    is_initialized = 1;
  }
}

RawImage *image_load_from_file(const char *path) {
  init_magickwand_once();
  MagickWand *wand = NewMagickWand();
  if (!wand) {
    fprintf(stderr, "Failed to create MagickWand.\n");
    return NULL;
  }
  char actual_path[1024];

  const char *ext = strrchr(path, '.');
  if (ext && (strcasecmp(ext, ".gif") == 0)) {
    // Append [0] to GIF files to only read the first frame
    snprintf(actual_path, sizeof(actual_path), "%s[0]", path);
  } else {
    // For all other formats, use path as-is
    snprintf(actual_path, sizeof(actual_path), "%s", path);
  }

  // Read the image
  if (MagickReadImage(wand, actual_path) == MagickFalse) {
    fprintf(stderr, "Failed to read image: %s\n", path);
    DestroyMagickWand(wand);
    return NULL;
  }

  RawImage *raw_image = (RawImage *)malloc(sizeof(RawImage));
  if (!raw_image) {
    fprintf(stderr, "Failed to allocate memory for RawImage.\n");
    DestroyMagickWand(wand);
    return NULL;
  }

  raw_image->width = MagickGetImageWidth(wand);
  raw_image->height = MagickGetImageHeight(wand);
  raw_image->channels = 4; // We'll standardize on RGBA for simplicity

  // Allocate memory for the pixel data
  size_t buffer_size =
      raw_image->width * raw_image->height * raw_image->channels;
  raw_image->pixels = (unsigned char *)malloc(buffer_size);
  if (!raw_image->pixels) {
    fprintf(stderr, "Failed to allocate memory for pixel buffer.\n");
    free(raw_image);
    DestroyMagickWand(wand);
    return NULL;
  }

  // Extract the pixels into our buffer in RGBA format
  MagickExportImagePixels(wand, 0, 0, raw_image->width, raw_image->height,
                          "RGBA", CharPixel, raw_image->pixels);

  DestroyMagickWand(wand);
  return raw_image;
}

void image_free(RawImage *img) {
  if (img) {
    if (img->pixels) {
      free(img->pixels);
    }
    free(img);
  }
}
