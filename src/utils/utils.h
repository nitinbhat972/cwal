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
#include "core.h"
#include <stdint.h>

float clamp_value(float amount);
uint8_t clamp_byte(float value);
void sort_color(Palette *palette);
void preview_palette();

#define RED "\x1b[38;2;220;50;47m"
#define BLUE "\x1b[38;2;38;139;210m"
#define YELLOW "\x1b[38;2;181;137;0m"
#define RESET "\x1b[0m"

// Enumeration for different logging levels.
enum Logging { INFO, WARN, ERROR };
void logging(int log_level, const char *format, ...);
void set_quiet_mode(bool quiet);
char *replace_placeholder(const char *str, const char *old, const char *new_str);
int execute_command(const char *command);
