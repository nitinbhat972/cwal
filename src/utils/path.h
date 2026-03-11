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

char *expand_home(const char *path);
int validate_or_create_dir(const char *dir_in);
char *get_random_image_path(const char *directory);
char *normalize_cli_path(const char *path);
char *build_path(const char *path1, const char *path2);
