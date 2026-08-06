#pragma once
#include <stddef.h>

#define BUILD_DIR "build"
#define GENERATED_DIR BUILD_DIR "/generated"
#define SRC_DIR "src"
#define INSTALL_DIR "/usr/local"

/*
  Uncomment and add your preferred compiler.
  Otherwise `nob_cc` will resolve compiler for you.
*/
// #define CUSTOM_CC "clang"

/*
  Number of parallel compiler processes.
  0 = auto (nobs's default)
*/
#define PROCS 0

/*
  Type of build you want to perfom
  Options: DEBUG, RELEASE
*/
#define BUILD_TYPE RELEASE

/*
  Include directories relative to the project root.
  Internally converted to -I<dir>.
*/
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

/*
 Pairs of source (relative to root ) and destination (relative to `INSTALL_DIR`
 or env `CWAL_INSTALL_DIR`) to be installed.
*/
static const struct {
  const char *src;
  const char *dest;
} INSTALL_FILES[] = {
    {BUILD_DIR "/cwal", "bin/cwal"},
    {"templates", "share/cwal/templates"},
    {"themes", "share/cwal/themes"},
    {"shell/bash/cwal", "share/bash-completion/completions/cwal"},
    {"shell/zsh/_cwal", "share/zsh/site-functions/_cwal"},
    {"shell/fish/cwal.fish", "share/fish/vendor_completions.d/cwal.fish"},
    {NULL},
};
