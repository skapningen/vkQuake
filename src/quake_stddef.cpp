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
#include <cstddef>
#include <cstdint>

export module quakestddef;

export {

  const uint8_t Q_MAXCHAR = 0x7f;
  const uint16_t Q_MAXSHORT = 0x7fff;
  const uint32_t Q_MAXINT = 0x7fffffff;
  const uint32_t Q_MAXLONG = 0x7fffffff;

  const uint8_t Q_MINCHAR = 0x80;
  const uint16_t Q_MINSHORT = 0x8000;
  const uint32_t Q_MININT = 0x80000000;
  const uint32_t Q_MINLONG = 0x80000000;

  using vec_t = float;
  using vec2_t = vec_t[2];
  using vec3_t = vec_t[3];
  using vec4_t = vec_t[4];
  using vec5_t = vec_t[5];
  using fixed4_t = int32_t;
  using fixed8_t = int32_t;
  using fixed16_t = int32_t;

  // TODO: better way to specify this
  const size_t MAX_OSPATH = 1024;

  const size_t MAX_QPATH = 64;

  const size_t MAX_NUM_ARGVS = 50;

  struct QuakeParms {
    const char *basedir;
    const char *userdir; // user's directory on UNIX platforms.
                         // if user directories are enabled, basedir
                         // and userdir will point to different
                         // memory locations, otherwise to the same.
    int32_t argc;
    char **argv;
    int32_t errstate;
  };

}
