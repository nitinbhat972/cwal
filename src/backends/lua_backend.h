#pragma once

#include "core.h"

void lua_backend_init(void);
void lua_backend_terminate(void);
int lua_generate_palette(const char *script_path, const char *image_path,
                         Palette *palette);
