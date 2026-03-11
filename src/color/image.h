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

// This struct will hold the raw image data.
typedef struct {
  unsigned char *pixels; // Raw pixel data
  int width;
  int height;
  int channels;
} RawImage;

RawImage *image_load_from_file(const char *path);
void image_free(RawImage *img);
