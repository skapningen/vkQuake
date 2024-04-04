/*
Copyright (C) 1996-2001 Id Software, Inc.
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
#include <cstddef>
#include <cstdint>

export module progscompiler;

// this file is shared by quake and qcc

export {
  // typedef unsigned int func_t;
  using func_t = uint32_t;
  // typedef int string_t;
  using string_t = int32_t;

  enum class etype_t {
    ev_bad = -1,
    ev_void = 0,
    ev_string,
    ev_float,
    ev_vector,
    ev_entity,
    ev_field,
    ev_function,
    ev_pointer,

    ev_ext_integer
  };

  constexpr uint32_t OFS_NULL = 0;
  constexpr uint32_t OFS_RETURN = 1;
  constexpr uint32_t OFS_PARM0 = 4; // leave 3 ofs for each parm to hold vectors
  constexpr uint32_t OFS_PARM1 = 7;
  constexpr uint32_t OFS_PARM2 = 10;
  constexpr uint32_t OFS_PARM3 = 13;
  constexpr uint32_t OFS_PARM4 = 16;
  constexpr uint32_t OFS_PARM5 = 19;
  constexpr uint32_t OFS_PARM6 = 22;
  constexpr uint32_t OFS_PARM7 = 25;
  constexpr uint32_t RESERVED_OFS = 28;

  enum {
    OP_DONE,
    OP_MUL_F,
    OP_MUL_V,
    OP_MUL_FV,
    OP_MUL_VF,
    OP_DIV_F,
    OP_ADD_F,
    OP_ADD_V,
    OP_SUB_F,
    OP_SUB_V,

    OP_EQ_F,
    OP_EQ_V,
    OP_EQ_S,
    OP_EQ_E,
    OP_EQ_FNC,

    OP_NE_F,
    OP_NE_V,
    OP_NE_S,
    OP_NE_E,
    OP_NE_FNC,

    OP_LE,
    OP_GE,
    OP_LT,
    OP_GT,

    OP_LOAD_F,
    OP_LOAD_V,
    OP_LOAD_S,
    OP_LOAD_ENT,
    OP_LOAD_FLD,
    OP_LOAD_FNC,

    OP_ADDRESS,

    OP_STORE_F,
    OP_STORE_V,
    OP_STORE_S,
    OP_STORE_ENT,
    OP_STORE_FLD,
    OP_STORE_FNC,

    OP_STOREP_F,
    OP_STOREP_V,
    OP_STOREP_S,
    OP_STOREP_ENT,
    OP_STOREP_FLD,
    OP_STOREP_FNC,

    OP_RETURN,
    OP_NOT_F,
    OP_NOT_V,
    OP_NOT_S,
    OP_NOT_ENT,
    OP_NOT_FNC,
    OP_IF,
    OP_IFNOT,
    OP_CALL0,
    OP_CALL1,
    OP_CALL2,
    OP_CALL3,
    OP_CALL4,
    OP_CALL5,
    OP_CALL6,
    OP_CALL7,
    OP_CALL8,
    OP_STATE,
    OP_GOTO,
    OP_AND,
    OP_OR,

    OP_BITAND,
    OP_BITOR
  };

  struct dstatement_t {
    uint16_t op;
    int16_t a, b, c;
  };

  struct ddef_t {
    uint16_t type; // if DEF_SAVEGLOBAL bit is set
                   // the variable needs to be saved in savegames
    uint16_t ofs;
    int32_t s_name;
  };

  constexpr uint32_t DEF_SAVEGLOBAL = 1 << 15;
  // #define DEF_SAVEGLOBAL (1 << 15)

  constexpr size_t MAX_PARMS = 8;
  // #define MAX_PARMS 8

  struct dfunction_t {
    int32_t first_statement; // negative numbers are builtins
    int32_t parm_start;
    int32_t locals; // total ints of parms + locals

    int32_t profile; // runtime

    int32_t s_name;
    int32_t s_file; // source file defined in

    int32_t numparms;
    uint8_t parm_size[MAX_PARMS];
  };

  constexpr int32_t PROG_VERSION = 6;
  // #define PROG_VERSION 6
  struct dprograms_t {
    int32_t version;
    int32_t crc; // check of header file

    int32_t ofs_statements;
    int32_t numstatements; // statement 0 is an error

    int32_t ofs_globaldefs;
    int32_t numglobaldefs;

    int32_t ofs_fielddefs;
    int32_t numfielddefs;

    int32_t ofs_functions;
    int32_t numfunctions; // function 0 is an empty

    int32_t ofs_strings;
    int32_t numstrings; // first string is a null string

    int32_t ofs_globals;
    int32_t numglobals;

    int32_t entityfields;
  };

} // namespace progscompiler
