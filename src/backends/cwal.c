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
#include <MagickWand/MagickWand.h>
#include <stdbool.h>
#include <stdio.h>

void init_magickwand() {
  if (!IsMagickWandInstantiated()) {
    MagickWandGenesis();
  }
}

void terminate_magickwand() {
  if (IsMagickWandInstantiated()) {
    MagickWandTerminus();
  }
}

int generate_palette_cwal(RawImage *image, Palette *palette) {
  if (!image || !palette || !image->pixels) {
    return -1;
  }

  MagickWand *wand = NewMagickWand();
  if (!wand) {
    return -1;
  }

  if (MagickConstituteImage(wand, image->width, image->height, "RGBA",
                            CharPixel, image->pixels) == MagickFalse) {
    DestroyMagickWand(wand);
    return -1;
  }

  if (MagickSetImageColorspace(wand, LabColorspace) == MagickFalse) {
    DestroyMagickWand(wand);
    return -1;
  }

  if (MagickQuantizeImage(wand, 8, LabColorspace, 0, NoDitherMethod,
                          MagickFalse) == MagickFalse) {
    DestroyMagickWand(wand);
    return -1;
  }

  PixelWand *pixel = NewPixelWand();
  if (!pixel) {
    DestroyMagickWand(wand);
    return -1;
  }

  int status = 0;

  for (size_t i = 0; i < 8; i++) {
    if (MagickGetImageColormapColor(wand, i, pixel) == MagickFalse) {
      status = -1;
      break;
    }

    palette->colors[i] = (Color){
        .red = (uint8_t)(PixelGetRed(pixel) * 255),
        .green = (uint8_t)(PixelGetGreen(pixel) * 255),
        .blue = (uint8_t)(PixelGetBlue(pixel) * 255),
    };
  }

  DestroyPixelWand(pixel);
  DestroyMagickWand(wand);
  return status;
}

ImageBackend cwal = {.name = "cwal",
                     .init_backend = init_magickwand,
                     .terminate_backend = terminate_magickwand,
                     .generate_palette = generate_palette_cwal};
