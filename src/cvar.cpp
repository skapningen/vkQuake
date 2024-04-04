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
#include <cstdio>
#include <cstring>
#include <cstdlib>

export module cvar;
import quakestring;

/*
cvar_t variables are used to hold scalar or string variables that can
be changed or displayed at the console or prog code as well as accessed
directly in C code.

it is sufficient to initialize a cvar_t with just the first two fields,
or you can add a ,true flag for variables that you want saved to the
configuration file when the game is quit:

CVar r_draworder = {"r_draworder","1"};
CVar scr_screensize = {"screensize","1",true};

Cvars must be registered before use, or they will have a 0 value instead
of the float interpretation of the string.
Generally, all cvar_t declarations should be registered in the apropriate
init function before any console commands are executed:

register_variable (&host_framerate);


C code usually just references a cvar in place:
if ( r_draworder.value )

It could optionally ask for the value to be looked up for a string name:
if (variable_value ("r_draworder"))

Interpreted prog code can access cvars with the cvar(name) or
set (name, value) internal functions:
teamplay = cvar("teamplay");
set ("registered", "1");

The user can access cvars from the console in two ways:
r_draworder		prints the current value
r_draworder 0		sets the current value to 0

Cvars are restricted from having the same names as commands to keep this
interface from being ambiguous.

*/

namespace cvar {

export constexpr uint32_t NONE = 0;
export constexpr uint32_t ARCHIVE =
    1U << 0; // if set, causes it to be saved to config
export constexpr uint32_t NOTIFY =
    1U << 1; // changes will be broadcasted to all players (q1)
export constexpr uint32_t SERVERINFO =
    1U << 2; // added to serverinfo will be sent to
             // clients (q1/net_dgrm.c and qwsv)
export constexpr uint32_t USERINFO =
    1U << 3; // added to userinfo, will be sent to server (qwcl)
export constexpr uint32_t CHANGED = 1U << 4;
export constexpr uint32_t ROM = 1U << 6;
export constexpr uint32_t LOCKED = 1U << 8; // locked temporarily
export constexpr uint32_t REGISTERED =
    1U << 10; // the var is added to the list of variables
export constexpr uint32_t CALLBACK = 1U << 16; // var has a callback
export constexpr uint32_t USERDEFINED =
    1U << 17; // cvar was created by the user/mod, and
              // needs to be saved a bit differently.
export constexpr uint32_t AUTOCVAR =
    1U << 18; // cvar changes need to feed back to qc global changes.
export constexpr uint32_t SETA = 1U << 19; // cvar will be saved with seta.

export struct CVar;
using CVarCallback = void (*)(CVar *);

struct CVar {
  const char *name;
  const char *string;
  int32_t flags;
  float value;
  const char
      *default_string; // johnfitz -- remember defaults for reset function
  CVarCallback callback;
  struct CVar *next;
};

CVar *cvar_vars;
char cvar_null_string[] = "";

/*
============
find_var
============
*/
CVar *find_var(const char *var_name) {
  CVar *var;

  for (var = cvar_vars; var; var = var->next) {
    if (!strcmp(var_name, var->name))
      return var;
  }

  return nullptr;
}

CVar *find_var_after(const char *prev_name, uint32_t with_flags) {
  CVar *var;

  if (*prev_name) {
    var = find_var(prev_name);
    if (!var)
      return nullptr;
    var = var->next;
  } else
    var = cvar_vars;

  // search for the next cvar matching the needed flags
  while (var) {
    if ((var->flags & with_flags) || !with_flags)
      break;
    var = var->next;
  }
  return var;
}

// these two accept a cvar pointer instead of a var name,
// but are otherwise identical to the "non-Quick" versions.
// the cvar MUST be registered.
void set_quick(CVar *var, const char *value);

void set_value_quick(CVar *var, const float value) {
  char val[32], *ptr = val;

  if (value == (float)((int)value))
    quakestring::snprintf(val, sizeof(val), "%i", (int)value);
  else {
    quakestring::snprintf(val, sizeof(val), "%f", value);
    // kill trailing zeroes
    while (*ptr)
      ptr++;
    while (--ptr > val && *ptr == '0' && ptr[-1] != '.')
      *ptr = '\0';
  }

  set_quick(var, val);
}

void register_variable(CVar *variable);

CVar *create(const char *name, const char *value);

/*
============
set_callback

Set a callback function to the var
============
*/
void set_callback(CVar *var, CVarCallback func) {
  var->callback = func;
  if (func)
    var->flags |= CALLBACK;
  else
    var->flags &= ~CALLBACK;
}

void set(const char *var_name, const char *value);

/*
============
Cvar_SetValue

expands value to a string and calls set
============
*/
void set_value(const char *var_name, const float value) {
  char val[32], *ptr = val;

  if (value == (float)((int)value))
    quakestring::snprintf(val, sizeof(val), "%i", (int)value);
  else {
    quakestring::snprintf(val, sizeof(val), "%f", value);
    // kill trailing zeroes
    while (*ptr)
      ptr++;
    while (--ptr > val && *ptr == '0' && ptr[-1] != '.')
      *ptr = '\0';
  }

  set(var_name, val);
}

/*
============
Cvar_SetROM
============
*/
void set_ROM(const char *var_name, const char *value) {
  CVar *var = find_var(var_name);
  if (var) {
    var->flags &= ~ROM;
    set_quick(var, value);
    var->flags |= ROM;
  }
}

/*
============
Cvar_SetValueROM

sets a ROM variable from within the engine
============
*/
void set_value_ROM(const char *var_name, const float value) {
  CVar *var = find_var(var_name);
  if (var) {
    var->flags &= ~ROM;
    set_value_quick(var, value);
    var->flags |= ROM;
  }
}

/*
============
Cvar_VariableValue

returns 0 if not defined or non numeric
============
*/
double variable_value(const char *var_name) {
  CVar *var = find_var(var_name);
  if (!var)
    return 0;
  return atof(var->string);
}

/*
============
Cvar_VariableString

returns an empty string if not defined
============
*/
const char *variable_string(const char *var_name) {
  CVar *var = find_var(var_name);
  if (!var)
    return cvar_null_string;
  return var->string;
}

bool command(void);

/*
============
Cvar_WriteVariables

Writes lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/
void write_variables(FILE *f) {
  CVar *var;

  for (var = cvar_vars; var; var = var->next) {
    if (var->flags & ARCHIVE) {
      if (var->flags & (USERDEFINED | SETA))
        fprintf(f, "seta ");
      fprintf(f, "%s \"%s\"\n", var->name, var->string);
    }
  }
}

/*
============
Cvar_LockVar
============
*/
void lock_var(const char *var_name) {
  CVar *var = find_var(var_name);
  if (var)
    var->flags |= LOCKED;
}

void unlock_var(const char *var_name) {
  CVar *var = find_var(var_name);
  if (var)
    var->flags &= ~LOCKED;
}

void unlock_all(void) {
  CVar *var;

  for (var = cvar_vars; var; var = var->next) {
    var->flags &= ~LOCKED;
  }
}

void list_f(void);

void inc_f(void);

void set_f(void);

void toggle_f(void);

void cycle_f(void);

void reset_f(void);

void reset(const char *name);

/*
============
Cvar_ResetAll_f -- johnfitz
============
*/
void reset_all_f(void) {
  CVar *var;

  for (var = cvar_vars; var; var = var->next)
    reset(var->name);
}

/*
============
Cvar_ResetCfg_f -- QuakeSpasm
============
*/
void reset_cfg_f(void) {
  CVar *var;

  for (var = cvar_vars; var; var = var->next)
    if (var->flags & ARCHIVE)
      reset(var->name);
}

void init(void);

/*
============
Cvar_CompleteVariable

attempts to match a partial variable name for command line completion
returns nullptr if nothing fits
============
*/
const char *complete_variable(const char *partial) {
  CVar *cvar;
  int len;

  len = strlen(partial);
  if (!len)
    return nullptr;

  // check functions
  for (cvar = cvar_vars; cvar; cvar = cvar->next) {
    if (!strncmp(partial, cvar->name, len))
      return cvar->name;
  }

  return nullptr;
}

} // namespace cvar
