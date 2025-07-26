#include "backend.h"
#include "core.h"

#include <libimagequant.h>

#define RESIZE_FACTOR 0.20f

int generate_palette_libimagequant(RawImage *image, Palette *palette) {
    if (!image || !palette || !image->pixels) {
        
        return -1;
    }

    liq_attr *attr = liq_attr_create();
    if (!attr) {
        
        return -1;
    }

    liq_set_max_colors(attr, 8); // Generate 8 colors

    liq_image *liq_img = liq_image_create_rgba(attr, image->pixels, image->width, image->height, 0);
    if (!liq_img) {
        
        liq_attr_destroy(attr);
        return -1;
    }

    liq_result *res;
    if (liq_image_quantize(liq_img, attr, &res) != LIQ_OK) {
        
        liq_image_destroy(liq_img);
        liq_attr_destroy(attr);
        return -1;
    }

    const liq_palette *liq_pal = liq_get_palette(res);

    for (int i = 0; i < liq_pal->count; ++i) {
        palette->colors[i].red = liq_pal->entries[i].r;
        palette->colors[i].green = liq_pal->entries[i].g;
        palette->colors[i].blue = liq_pal->entries[i].b;
    }

    liq_result_destroy(res);
    liq_image_destroy(liq_img);
    liq_attr_destroy(attr);

    return 0;
}

ImageBackend libimagequant = {
    .name = "libimagequant",
    .init_backend = NULL,
    .terminate_backend = NULL,
    .generate_palette = generate_palette_libimagequant
};