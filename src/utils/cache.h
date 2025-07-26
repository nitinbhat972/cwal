#pragma once

#include "core.h"

void save_palette_to_cache(const Palette *palette, const char *cache_dir, const char *backend_name);
int load_palette_from_cache(Palette *palette, const char *cache_dir, const char *backend_name);
