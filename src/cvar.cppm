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
import memory;

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

// #define CVAR_NONE 0
// #define CVAR_ARCHIVE (1U << 0) // if set, causes it to be saved to config
// #define CVAR_NOTIFY (1U << 1) // changes will be broadcasted to all players
// (q1)
// #define CVAR_SERVERINFO \ (1U << 2) // added to serverinfo will be sent to
// clients (q1/net_dgrm.c and
//// qwsv)
// #define CVAR_USERINFO \ (1U << 3) // added to userinfo, will be sent to
// server (qwcl)
//  #define CVAR_CHANGED (1U << 4)
//  #define CVAR_ROM (1U << 6)
// #define CVAR_LOCKED (1U << 8)      // locked temporarily
// #define CVAR_REGISTERED (1U << 10) // the var is added to the list of
// variables #define CVAR_CALLBACK (1U << 16)   // var has a callback
// #define CVAR_USERDEFINED \ (1U << 17) // cvar was created by the user/mod,
// and needs to be saved a bit
//// differently.
// #define CVAR_AUTOCVAR \ (1U << 18) // cvar changes need to feed back to qc
// global changes.
// #define CVAR_SETA (1U << 19) // cvar will be saved with seta.

// typedef void (*cvarcallback_t)(struct cvar_s *);
struct CVar;
using CVarCallback = void (*)(CVar *);

struct CVar {
  const char *name;
  const char *string;
  unsigned int flags;
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
void set_quick(CVar *var, const char *value) {
  if (var->flags & (ROM | LOCKED))
    return;
  if (!(var->flags & REGISTERED))
    return;

  if (!var->string)
    var->string = quakestring::strdup(value);
  else {
    int len;

    if (!strcmp(var->string, value))
      return; // no change

    var->flags |= CHANGED;
    len = strlen(value);
    if (len != strlen(var->string)) {
      memory::free((void *)var->string);
      var->string = (char *)memory::alloc(len + 1);
    }
    memcpy((char *)var->string, value, len + 1);
  }

  var->value = atof(var->string);

  // johnfitz -- save initial value for "reset" command
  if (!var->default_string)
    var->default_string = quakestring::strdup(var->string);
  // johnfitz -- during initialization, update default too
  else if (!host_initialized) {
    //	Sys_Printf("changing default of %s: %s -> %s\n",
    //		   var->name, var->default_string, var->string);
    memory::free((void *)var->default_string);
    var->default_string = quakestring::strdup(var->string);
  }
  // johnfitz

  if (var->callback)
    var->callback(var);
  if (var->flags & AUTOCVAR)
    PR_AutoCvarChanged(var);
}

void set_value_quick(CVar *var, const float value) {
  char val[32], *ptr = val;

  if (value == (float)((int)value))
    q_snprintf(val, sizeof(val), "%i", (int)value);
  else {
    q_snprintf(val, sizeof(val), "%f", value);
    // kill trailing zeroes
    while (*ptr)
      ptr++;
    while (--ptr > val && *ptr == '0' && ptr[-1] != '.')
      *ptr = '\0';
  }

  set_quick(var, val);
}

/*
============
register_variable

Adds a freestanding variable to the variable list.

registers a cvar that already has the name, string, and optionally
the archive elements set.
============
*/
void register_variable(CVar *variable) {
  char value[512];
  bool set_rom;
  CVar *cursor, *prev; // johnfitz -- sorted list insert

  // first check to see if it has already been defined
  if (find_var(variable->name)) {
    Con_Printf("Can't register variable %s, already defined\n", variable->name);
    return;
  }

  // check for overlap with a command
  if (Cmd_Exists(variable->name)) {
    Con_Printf("Cvar_RegisterVariable: %s is a command\n", variable->name);
    return;
  }

  // link the variable in
  // johnfitz -- insert each entry in alphabetical order
  if (cvar_vars == nullptr ||
      strcmp(variable->name, cvar_vars->name) < 0) // insert at front
  {
    variable->next = cvar_vars;
    cvar_vars = variable;
  } else // insert later
  {
    prev = cvar_vars;
    cursor = cvar_vars->next;
    while (cursor && (strcmp(variable->name, cursor->name) > 0)) {
      prev = cursor;
      cursor = cursor->next;
    }
    variable->next = prev->next;
    prev->next = variable;
  }
  // johnfitz
  variable->flags |= REGISTERED;

  // copy the value off, because future sets will Mem_Free it
  q_strlcpy(value, variable->string, sizeof(value));
  variable->string = nullptr;
  variable->default_string = nullptr;

  if (!(variable->flags & CALLBACK))
    variable->callback = nullptr;

  // set it through the function to be consistent
  set_rom = (variable->flags & ROM);
  variable->flags &= ~ROM;
  set_quick(variable, value);
  if (set_rom)
    variable->flags |= ROM;
}

/*
============
create -- spike

Creates a cvar if it does not already exist, otherwise does nothing.
Must not be used until after all other cvars are registered.
Cvar will be persistent.

creates+registers a cvar, otherwise just returns it.
============
*/
CVar *create(const char *name, const char *value) {
  CVar *newvar;
  newvar = find_var(name);
  if (newvar)
    return newvar; // already exists.
  if (Cmd_Exists(name))
    return nullptr; // error! panic! oh noes!

  newvar = alloc(sizeof(CVar) + strlen(name) + 1);
  newvar->name = (char *)(newvar + 1);
  strcpy((char *)(newvar + 1), name);
  newvar->flags = USERDEFINED;

  newvar->string = value;
  register_variable(newvar);
  return newvar;
}

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

/*
============
set
equivelant to "<name> <variable>" typed at the console
============
*/
void set(const char *var_name, const char *value) {
  CVar *var;

  var = find_var(var_name);
  if (!var) { // there is an error in C code if this happens
    Con_Printf("cvar::set: variable %s not found\n", var_name);
    return;
  }

  set_quick(var, value);
}

/*
============
Cvar_SetValue

expands value to a string and calls set
============
*/
void set_value(const char *var_name, const float value) {
  char val[32], *ptr = val;

  if (value == (float)((int)value))
    q_snprintf(val, sizeof(val), "%i", (int)value);
  else {
    q_snprintf(val, sizeof(val), "%f", value);
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
// returns an empty string if not defined
============
*/
const char *variable_string(const char *var_name) {
  CVar *var find_var(var_name);
  if (!var)
    return cvar_null_string;
  return var->string;
}

/*
============
Cvar_Command

Handles variable inspection and changing from the console
// called by Cmd_ExecuteString when Cmd_Argv(0) doesn't match a known
// command.  Returns true if the command was a variable reference that
// was handled. (print or change)
============
*/
bool command(void) {
  // check variables
  CVar *v = find_var(Cmd_Argv(0));
  if (!v)
    return false;

  // perform a variable print or set
  if (Cmd_Argc() == 1) {
    Con_Printf("\"%s\" is \"%s\"\n", v->name, v->string);
    return true;
  }

  set(v->name, Cmd_Argv(1));
  return true;
}

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

/*
============
Cvar_List_f -- johnfitz
============
*/
void list_f(void) {
  CVar *cvar;
  const char *partial;
  int len, count;

  if (Cmd_Argc() > 1) {
    partial = Cmd_Argv(1);
    len = strlen(partial);
  } else {
    partial = nullptr;
    len = 0;
  }

  count = 0;
  for (cvar = cvar_vars; cvar; cvar = cvar->next) {
    if (partial && strncmp(partial, cvar->name, len)) {
      continue;
    }
    Con_SafePrintf("%s%s %s \"%s\"\n", (cvar->flags & ARCHIVE) ? "*" : " ",
                   (cvar->flags & NOTIFY) ? "s" : " ", cvar->name,
                   cvar->string);
    count++;
  }

  Con_SafePrintf("%i cvars", count);
  if (partial) {
    Con_SafePrintf(" beginning with \"%s\"", partial);
  }
  Con_SafePrintf("\n");
}

/*
============
Cvar_Inc_f -- johnfitz
============
*/
void inc_f(void) {
  switch (Cmd_Argc()) {
  default:
  case 1:
    Con_Printf("inc <cvar> [amount] : increment cvar\n");
    break;
  case 2:
    Cvar_SetValue(Cmd_Argv(1), Cvar_VariableValue(Cmd_Argv(1)) + 1);
    break;
  case 3:
    Cvar_SetValue(Cmd_Argv(1),
                  Cvar_VariableValue(Cmd_Argv(1)) + atof(Cmd_Argv(2)));
    break;
  }
}

/*
============
Cvar_Set_f -- spike

both set+seta commands
============
*/
void set_f(void) {
  // q2: set name value flags
  // dp: set name value description
  // fte: set name some freeform value with spaces or whatever //description
  // to avoid politics, its easier to just stick with name+value only.
  // that leaves someone else free to pick a standard for what to do with extra
  // args.
  const char *varname = Cmd_Argv(1);
  const char *varvalue = Cmd_Argv(2);
  CVar *var;
  if (Cmd_Argc() < 3) {
    Con_Printf("%s <cvar> <value>\n", Cmd_Argv(0));
    return;
  }
  if (Cmd_Argc() > 3) {
    Con_Warning("%s \"%s\" command with extra args\n", Cmd_Argv(0), varname);
    return;
  }
  var = Cvar_Create(varname, varvalue);
  Cvar_SetQuick(var, varvalue);

  if (!strcmp(Cmd_Argv(0), "seta"))
    var->flags |= ARCHIVE | SETA;
}

/*
============
Cvar_Toggle_f -- johnfitz
============
*/
void toggle_f(void) {
  switch (Cmd_Argc()) {
  default:
  case 1:
    Con_Printf("toggle <cvar> : toggle cvar\n");
    break;
  case 2:
    if (Cvar_VariableValue(Cmd_Argv(1)))
      set(Cmd_Argv(1), "0");
    else
      set(Cmd_Argv(1), "1");
    break;
  }
}

/*
============
Cvar_Cycle_f -- johnfitz
============
*/
void cycle_f(void) {
  int i;

  if (Cmd_Argc() < 3) {
    Con_Printf(
        "cycle <cvar> <value list>: cycle cvar through a list of values\n");
    return;
  }

  // loop through the args until you find one that matches the current cvar
  // value. yes, this will get stuck on a list that contains the same value
  // twice. it's not worth dealing with, and i'm not even sure it can be dealt
  // with.
  for (i = 2; i < Cmd_Argc(); i++) {
    // zero is assumed to be a string, even though it could actually be zero.
    // The worst case is that the first time you call this command, it won't
    // match on zero when it should, but after that, it will be comparing
    // strings that all had the same source (the user) so it will work.
    if (atof(Cmd_Argv(i)) == 0) {
      if (!strcmp(Cmd_Argv(i), Cvar_VariableString(Cmd_Argv(1))))
        break;
    } else {
      if (atof(Cmd_Argv(i)) == Cvar_VariableValue(Cmd_Argv(1)))
        break;
    }
  }

  if (i == Cmd_Argc())
    set(Cmd_Argv(1), Cmd_Argv(2)); // no match
  else if (i + 1 == Cmd_Argc())
    set(Cmd_Argv(1), Cmd_Argv(2)); // matched last value in list
  else
    set(Cmd_Argv(1), Cmd_Argv(i + 1)); // matched earlier in list
}

/*
============
Cvar_Reset_f -- johnfitz
============
*/
void reset_f(void) {
  switch (Cmd_Argc()) {
  default:
  case 1:
    Con_Printf("reset <cvar> : reset cvar to default\n");
    break;
  case 2:
    reset(Cmd_Argv(1));
    break;
  }
}

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

/*
============
Cvar_Init -- johnfitz
============
*/
void init(void) {
  Cmd_AddCommand("cvarlist", list_f);
  Cmd_AddCommand("toggle", toggle_f);
  Cmd_AddCommand("cycle", cycle_f);
  Cmd_AddCommand("inc", inc_f);
  Cmd_AddCommand("reset", reset_f);
  Cmd_AddCommand("resetall", reset_all_f);
  Cmd_AddCommand("resetcfg", reset_cfg_f);
  Cmd_AddCommand("set", set_f);
  Cmd_AddCommand("seta", set_f);
}

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

/*
============
reset -- johnfitz
============
*/
void reset(const char *name) {
  CVar *var;

  var = find_var(name);
  if (!var)
    Con_Printf("variable \"%s\" not found\n", name);
  else
    Cvar_SetQuick(var, var->default_string);
}

} // namespace cvar
