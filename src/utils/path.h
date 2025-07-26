#pragma once

char *expand_home(const char *path);
int validate_or_create_dir(const char *dir_in);
char *get_random_image_path(const char *directory);
char *normalize_cli_path(const char *path);
char *build_path(const char *path1, const char *path2);
