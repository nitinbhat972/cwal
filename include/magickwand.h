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

#if __has_include(<MagickWand/MagickWand.h>)
#include <MagickWand/MagickWand.h>
#elif __has_include(<wand/MagickWand.h>)
#include <wand/MagickWand.h>
#else
#error "MagickWand header not found. Please install ImageMagick development libraries."
#endif
