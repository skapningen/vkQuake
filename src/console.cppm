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

export module console;

//
// console
//

namespace console {

int32_t con_linewidth;

float con_cursorspeed = 4;

constexpr uint32_t CON_TEXTSIZE =
    1024 * 1024; // ericw -- was 65536. johnfitz -- new default size
constexpr uint32_t CON_MINSIZE =
    16384; // johnfitz -- old default, now the minimum size
// #define CON_TEXTSIZE (1024 * 1024) // ericw -- was 65536. johnfitz -- new
// default size #define CON_MINSIZE	 16384		   // johnfitz -- old
// default, now the minimum size

int32_t con_buffersize; // johnfitz -- user can now override default

bool con_forcedup; // because no entities to refresh

int32_t con_totallines; // total lines in console scrollback
int32_t con_backscroll; // lines up from bottom to display
int32_t con_current;    // where next message will be printed
int32_t con_x;          // offset in current line for next print
char *con_text = nullptr;

CVar con_notifytime = {"con_notifytime", "3", CVAR_NONE};         // seconds
CVar con_logcenterprint = {"con_logcenterprint", "1", CVAR_NONE}; // johnfitz

char con_lastcenterstring[1024]; // johnfitz
void (*con_redirect_flush)(
    const char *buffer); // call this to flush the redirection buffer (for rcon)
char con_redirect_buffer[8192];

#define NUM_CON_TIMES 4
float con_times[NUM_CON_TIMES]; // realtime time the line was generated
                                // for transparent notify lines

int32_t con_vislines;

bool con_debuglog = false;

bool con_initialized;

SDL_mutex *con_mutex;

extern bool con_forcedup; // because no entities to refresh
extern bool con_initialized;
extern uint8_t *con_chars;
typedef struct cb_context_s cb_context_t;

extern char con_lastcenterstring[]; // johnfitz

void check_resize(void);
void init(void);
void draw_console(cb_context_t *cbx, int32_t lines, bool drawinput);
void printf(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void Dwarning(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void warning(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void Dprintf(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void Dprintf2(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void safe_printf(const char *fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
void draw_notify(cb_context_t *cbx);
void clear_notify(void);
void toggle_console_f(void);
bool is_redirected(void); // returns true if its redirected. this generally
                             // means that things are a little more verbose.
void redirect(void (*flush)(const char *text));

void notify_box(const char *text); // during startup for sound / cd warnings

const char *quakebar(int32_t len);
void tab_complete(void);
void log_center_print(const char *str);

//
// debuglog
//
void LOG_Init(quakeparms_t *parms);
void LOG_Close(void);
void Con_DebugLog(const char *msg);

} // namespace console
