#pragma once
#include <stddef.h>

#define BUILD_DIR "build"
#define GENERATED_DIR BUILD_DIR "/generated"
#define SRC_DIR "src"

/*
  Uncomment and add your preferred compiler.
  Otherwise `nob_cc` will resolve compiler for you.
*/
// #define CUSTOM_CC "clang"

static const char *const INCLUDES[] = {
    "include",
    "src",
    NULL,
};

/*
  Add your custom compiler flags.
  By default -Wall and -Wextra are added by `nob_cc_flags`.
*/
static const char *const CFLAGS[] = {
    /* flags */
    NULL,
};

static const char *const LIBS[] = {
    "m",
    NULL,
};

/*
  Just put the name of the pkgconf package it will be resolved internally.
*/
static const char *const PKGS[] = {
    /* libs */
    "MagickWand",
    "imagequant",
    "luajit",
    NULL,
};
