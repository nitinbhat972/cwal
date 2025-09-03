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
