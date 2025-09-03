
#include "backend.h"
#include "core.h"
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static lua_State *lua_state = NULL;

void lua_backend_init() {
  if (lua_state)
    lua_close(lua_state);
  lua_state = luaL_newstate();
  if (!lua_state)
    return;
  luaL_openlibs(lua_state);
}

void lua_backend_terminate() {
  if (lua_state) {
    lua_close(lua_state);
    lua_state = NULL;
  }
}

static int load_lua_script(const char *script_path) {
  if (!lua_state)
    return -1;
  if (luaL_loadfile(lua_state, script_path) != LUA_OK) {
    lua_pop(lua_state, 1);
    return -1;
  }
  if (lua_pcall(lua_state, 0, 0, 0) != LUA_OK) {
    lua_pop(lua_state, 1);
    return -1;
  }
  return 0;
}

static int extract_colors_from_lua(Palette *palette) {
  if (!lua_state)
    return -1;
  if (!lua_istable(lua_state, -1))
    return -1;
  size_t table_len = lua_rawlen(lua_state, -1);
  if (table_len != 16)
    return -1;
  for (int i = 0; i < 16; i++) {
    lua_rawgeti(lua_state, -1, i + 1);
    if (!lua_istable(lua_state, -1)) {
      lua_pop(lua_state, 1);
      return -1;
    }
    lua_rawgeti(lua_state, -1, 1);
    lua_rawgeti(lua_state, -2, 2);
    lua_rawgeti(lua_state, -3, 3);
    if (!lua_isnumber(lua_state, -3) || !lua_isnumber(lua_state, -2) ||
        !lua_isnumber(lua_state, -1)) {
      lua_pop(lua_state, 3);
      return -1;
    }
    uint8_t r = (uint8_t)lua_tonumber(lua_state, -3);
    uint8_t g = (uint8_t)lua_tonumber(lua_state, -2);
    uint8_t b = (uint8_t)lua_tonumber(lua_state, -1);
    palette->colors[i] = (Color){r, g, b};
    lua_pop(lua_state, 4);
  }
  lua_pop(lua_state, 1);
  return 0;
}

int lua_generate_palette(const char *script_path, const char *image_path,
                         Palette *palette) {
  if (!script_path || !image_path || !palette)
    return -1;
  if (load_lua_script(script_path) != 0)
    return -1;
  lua_getglobal(lua_state, "Main");
  if (!lua_isfunction(lua_state, -1)) {
    lua_pop(lua_state, 1);
    return -1;
  }
  lua_pushstring(lua_state, image_path);
  if (lua_pcall(lua_state, 1, 1, 0) != LUA_OK) {
    lua_pop(lua_state, 1);
    return -1;
  }
  if (extract_colors_from_lua(palette) != 0)
    return -1;
  return 0;
}
