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

static inline int resize_image(MagickWand *wand, float amount) {
  size_t height = MagickGetImageHeight(wand);
  size_t width = MagickGetImageWidth(wand);

  size_t new_height = (size_t)(height * amount);
  size_t new_width = (size_t)(width * amount);

  if (MagickAdaptiveResizeImage(wand, new_width, new_height) == MagickFalse) {
    return -1;
  }
  return 0;
}

int generate_palette_cwal(RawImage *image, Palette *palette) {
  if (!image || !palette || !image->pixels) {
    return -1;
  }

  MagickWand *wand = NewMagickWand();
  if (!wand) {
    return -1;
  }

  // Create a new MagickWand from the raw pixel data
  if (MagickConstituteImage(wand, image->width, image->height, "RGBA",
                            CharPixel, image->pixels) == MagickFalse) {
    DestroyMagickWand(wand);
    return -1;
  }

  if (resize_image(wand, 0.20) != 0) {
    DestroyMagickWand(wand);
    return -1;
  }

  if (MagickSetImageColorspace(wand, LabColorspace) == MagickFalse) {
    DestroyMagickWand(wand);
    return -1;
  }

  size_t colors = 8;

  for (size_t tries = 0; tries < 20; tries++) {
    size_t color_count = colors + tries;
    if (MagickQuantizeImage(wand, color_count, LabColorspace, color_count * 2,
                            FloydSteinbergDitherMethod,
                            MagickFalse) != MagickFalse) {
      break;
    }
    if (tries == 19) {
      DestroyMagickWand(wand);
      return -1;
    }
  }

  PixelWand *pixel = NewPixelWand();
  if (!pixel) {
    DestroyMagickWand(wand);
    return -1;
  }

  int status = 0;

  for (size_t i = 0; i < colors; i++) {
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
