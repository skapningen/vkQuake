/*
Copyright (C) 1996-2001 Id Software, Inc.
Copyright (C) 2002-2009 John Fitzgibbons and others
Copyright (C) 2010-2014 QuakeSpasm developers

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

module;
#include <cstdint>
#include <cstddef>

export module console;
import quakestddef;

//
// console
//

namespace console {

export bool con_forcedup; // because no entities to refresh
export bool con_initialized;
export uint8_t *con_chars;
typedef struct cb_context_s cb_context_t;

export char con_lastcenterstring[1024]; // johnfitz

export void check_resize(void);
export void init(void);
export void draw_console(cb_context_t *cbx, int32_t lines, bool drawinput);
export void printf(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
export void Dwarning(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
export void warning(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
export void Dprintf(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
export void Dprintf2(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
export void safe_printf(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
export void draw_notify(cb_context_t *cbx);
export void clear_notify(void);
export void toggle_console_f(void);
export bool is_redirected(void); // returns true if its redirected. this generally
                             // means that things are a little more verbose.
export void redirect(void (*flush)(const char *text));

export void notify_box(const char *text); // during startup for sound / cd warnings

export const char *quakebar(int32_t len);
export void tab_complete(void);
export void log_center_print(const char *str);

//
// debuglog
//
export void LOG_Init(QuakeParms *parms);
export void LOG_Close(void);
export void debug_log(const char *msg);

} // namespace console
