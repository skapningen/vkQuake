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
#include <cstring>

module cvar;
import memory;
import console;
import command;

namespace cvar {

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
    console::printf("Can't register variable %s, already defined\n", variable->name);
    return;
  }

  // check for overlap with a command
  if (Cmd_Exists(variable->name)) {
    console::printf("Cvar_RegisterVariable: %s is a command\n", variable->name);
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
set
equivelant to "<name> <variable>" typed at the console
============
*/
void set(const char *var_name, const char *value) {
  CVar *var;

  var = find_var(var_name);
  if (!var) { // there is an error in C code if this happens
    console::printf("cvar::set: variable %s not found\n", var_name);
    return;
  }

  set_quick(var, value);
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
    console::printf("\"%s\" is \"%s\"\n", v->name, v->string);
    return true;
  }

  set(v->name, Cmd_Argv(1));
  return true;
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
    console::safe_printf("%s%s %s \"%s\"\n", (cvar->flags & ARCHIVE) ? "*" : " ",
                   (cvar->flags & NOTIFY) ? "s" : " ", cvar->name,
                   cvar->string);
    count++;
  }

  console::safe_printf("%i cvars", count);
  if (partial) {
    console::safe_printf(" beginning with \"%s\"", partial);
  }
  console::safe_printf("\n");
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
    console::printf("inc <cvar> [amount] : increment cvar\n");
    break;
  case 2:
    set_value(Cmd_Argv(1), variable_value(Cmd_Argv(1)) + 1);
    break;
  case 3:
    set_value(Cmd_Argv(1),
                  variable_value(Cmd_Argv(1)) + atof(Cmd_Argv(2)));
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
    console::printf("%s <cvar> <value>\n", Cmd_Argv(0));
    return;
  }
  if (Cmd_Argc() > 3) {
    console::warning("%s \"%s\" command with extra args\n", Cmd_Argv(0), varname);
    return;
  }
  var = create(varname, varvalue);
  set_quick(var, varvalue);

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
    console::printf("toggle <cvar> : toggle cvar\n");
    break;
  case 2:
    if (variable_value(Cmd_Argv(1)))
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
    console::printf(
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
      if (!strcmp(Cmd_Argv(i), variable_string(Cmd_Argv(1))))
        break;
    } else {
      if (atof(Cmd_Argv(i)) == variable_value(Cmd_Argv(1)))
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
    console::printf("reset <cvar> : reset cvar to default\n");
    break;
  case 2:
    reset(Cmd_Argv(1));
    break;
  }
}

/*
============
reset -- johnfitz
============
*/
void reset(const char *name) {
  CVar *var = find_var(name);
  if (!var)
    console::printf("variable \"%s\" not found\n", name);
  else
    set_quick(var, var->default_string);
}

}
