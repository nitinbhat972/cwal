#pragma once

#include "core.h"

typedef enum {
    RANDOM_NONE,
    RANDOM_DARK,
    RANDOM_LIGHT,
    RANDOM_ALL
} RandomMode;

int load_theme(Palette *palette, const char *theme_name);
int load_random_theme(Palette *palette, RandomMode mode);
void list_themes();
