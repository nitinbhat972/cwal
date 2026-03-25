/*
 *  cwal: Blazing-fast pywal-like color palette generator written in C.
 *  Copyright (c) 2026 Nitin Bhat <nitinbhat972@gmail.com>
 *  Repository: https://github.com/nitinbhat972/cwal
 *
 *  Licensed under the GNU General Public License v3.0.
 *  If you find this code useful, please consider giving it a star on GitHub!
 *  Any contributions or forks must retain this original header.
 */

#include "image.h"
#include "magickwand.h"
#include <limits.h>
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
  char actual_path[PATH_MAX];

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

  size_t width = MagickGetImageWidth(wand);
  size_t height = MagickGetImageHeight(wand);

  size_t nw = width / 5 ?: 1;
  size_t nh = height / 5 ?: 1;

  if (MagickSampleImage(wand, nw, nh) == MagickFalse) {
    fprintf(stderr, "Failed to sample image: %s\n", path);
    DestroyMagickWand(wand);
    return NULL;
  }

  RawImage *raw_image = (RawImage *)malloc(sizeof(RawImage));
  if (!raw_image) {
    fprintf(stderr, "Failed to allocate memory for RawImage.\n");
    DestroyMagickWand(wand);
    return NULL;
  }

  raw_image->width = nw;
  raw_image->height = nh;
  raw_image->channels = 4;

  // Allocate memory for the pixel data
  size_t buffer_size = nw * nh * 4;
  raw_image->pixels = (unsigned char *)malloc(buffer_size);
  if (!raw_image->pixels) {
    fprintf(stderr, "Failed to allocate memory for pixel buffer.\n");
    free(raw_image);
    DestroyMagickWand(wand);
    return NULL;
  }

  MagickExportImagePixels(wand, 0, 0, nw, nh, "RGBA", CharPixel,
                          raw_image->pixels);

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
