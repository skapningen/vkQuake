/*
 * Copyright (C) 1996-1997  Id Software, Inc.
 * Copyright (C) 2007-2011  O.Sezer <sezero@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

module;
#include <cstdarg>

import memory;
// #include <cstdint>
#include <cstddef>
#include <cstring>

export module quakestring;

namespace quakestring {

// strdup that calls Mem_Alloc
export char *strdup(const char *str) {
  size_t len = strlen(str) + 1;
  char *newstr = (char *)memory::alloc(len);
  memcpy(newstr, str, len);
  return newstr;
}

/* snprintf, vsnprintf : always use our versions. */
export int snprintf(char *str, size_t size, const char *format, ...)
    __attribute__((__format__(printf, 3, 4)));

export int vsnprintf(char *str, size_t size, const char *format, va_list args)
    __attribute__((__format__(printf, 3, 0)));

} // namespace quakestring
