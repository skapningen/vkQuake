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
// #include "pr_comp.h"  [> defs shared with qcc <]

export module progs;
import quakestddef;
import common;
import progscompiler;

#include "progdefs.h" /* generated by program cdefs */

namespace progs {

// eval_t
union Val {
  string_t string;
  float _float;
  float vector[3];
  func_t function;
  int32_t _int;
  int32_t edict;
};

constexpr size_t MAX_ENT_LEAFS = 32;
// #define MAX_ENT_LEAFS 32
// edict_t
struct Dict {
  common::Link area; /* linked to a division node or leaf */

  uint32_t num_leafs;
  int32_t leafnums[MAX_ENT_LEAFS];

  entity_state_t baseline;
  unsigned char alpha; /* johnfitz -- hack to support alpha since it's not part
                          of entvars_t */
  bool sendinterval;   /* johnfitz -- send time until nextthink to client for
                          better lerp timing */

  vec3_t predthinkpos; /* expected edict origin once its nextthink arrives
                          (sv_smoothplatformlerps) */
  float lastthink;     /* time when predthinkpos was updated, or 0 if not valid
                          (sv_smoothplatformlerps) */

  float freetime; /* sv.time when the object was freed */
  bool free;
  struct Dict *prev_free;
  struct Dict *next_free;

  entvars_t v; /* C exported fields from progs */

  /* other fields from progs come immediately after */
};

#define EDICT_FROM_AREA(l) STRUCT_FROM_LINK(l, Dict, area)

//============================================================================

typedef void (*builtin_t)(void);
typedef struct qcvm_s qcvm_t;

void PR_Init(void);

void PR_ExecuteProgram(func_t fnum);
void PR_ClearProgs(qcvm_t *vm);
bool PR_LoadProgs(const char *filename, bool fatal, unsigned int needcrc,
                  builtin_t *builtins, size_t numbuiltins);

// from pr_ext.c
void PR_InitExtensions(void);
void PR_EnableExtensions(
    ddef_t *pr_globaldefs); // adds in the extra builtins etc
void PR_AutoCvarChanged(
    cvar_t *var); // updates the autocvar_ globals when their cvar is changed
void PR_ShutdownExtensions(void); // nooooes!
void PR_ReloadPics(bool purge);   // for gamedir or video changes
func_t PR_FindExtFunction(const char *entryname);
void PR_DumpPlatform_f(
    void); // console command: writes out a qsextensions.qc file
// special hacks...
int32_t PF_SV_ForceParticlePrecache(const char *s);
int32_t SV_Precache_Model(const char *s);

// from pr_edict, for pr_ext. reflection is messy.
bool ED_ParseEpair(void *base, ddef_t *key, const char *s, bool zoned);
const char *PR_UglyValueString(int32_t type, Val *val);
ddef_t *ED_FindField(const char *name);
ddef_t *ED_FindGlobal(const char *name);
dfunction_t *ED_FindFunction(const char *fn_name);

const char *PR_GetString(int32_t num);
int32_t PR_SetEngineString(const char *s);
int32_t PR_AllocString(int32_t bufferlength, char **ptr);
void PR_ClearEdictStrings();
void PR_ClearEngineString(int32_t num);

void PR_Profile_f(void);

Dict *ED_Alloc(void);
void ED_Free(Dict *ed);
void ED_RemoveFromFreeList(Dict *ed);

void ED_Print(Dict *ed);
void ED_Write(FILE *f, Dict *ed);
const char *ED_ParseEdict(const char *data, Dict *ent);

void ED_WriteGlobals(FILE *f);
const char *ED_ParseGlobals(const char *data);

void ED_LoadFromFile(const char *data);

/*
#define EDICT_NUM(n)		((Dict *)(sv.edicts+ (n)*pr_edict_size))
#define NUM_FOR_EDICT(e)	(((byte *)(e) - sv.edicts) / pr_edict_size)
*/
Dict *EDICT_NUM(int32_t n);
int32_t NUM_FOR_EDICT(Dict *e);

#define NEXT_EDICT(e) ((Dict *)((byte *)e + qcvm->edict_size))

#define EDICT_TO_PROG(e) ((byte *)e - (byte *)qcvm->edicts)
#define PROG_TO_EDICT(e) ((Dict *)((byte *)qcvm->edicts + e))

#define G_FLOAT(o) (qcvm->globals[o])
#define G_INT(o) (*(int *)&qcvm->globals[o])
#define G_EDICT(o) ((Dict *)((byte *)qcvm->edicts + *(int *)&qcvm->globals[o]))
#define G_EDICTNUM(o) NUM_FOR_EDICT(G_EDICT(o))
#define G_VECTOR(o) (&qcvm->globals[o])
#define G_STRING(o) (PR_GetString(*(string_t *)&qcvm->globals[o]))
#define G_FUNCTION(o) (*(func_t *)&qcvm->globals[o])

#define G_VECTORSET(r, x, y, z)                                                \
  do {                                                                         \
    G_FLOAT((r) + 0) = x;                                                      \
    G_FLOAT((r) + 1) = y;                                                      \
    G_FLOAT((r) + 2) = z;                                                      \
  } while (0)
#define E_FLOAT(e, o) (((float *)&e->v)[o])
#define E_INT(e, o) (*(int *)&((float *)&e->v)[o])
#define E_VECTOR(e, o) (&((float *)&e->v)[o])
#define E_STRING(e, o) (PR_GetString(*(string_t *)&((float *)&e->v)[o]))

extern int type_size[8];

FUNC_NORETURN void PR_RunError(const char *error, ...) FUNC_PRINTF(1, 2);
void PR_RunWarning(const char *error, ...) FUNC_PRINTF(1, 2);

void ED_PrintEdicts(void);
void ED_PrintNum(int ent);

Val *GetEdictFieldValue(Dict *ed,
                        int fldofs); // handles invalid offsets with a null
int ED_FindFieldOffset(const char *name);

#define GetEdictFieldValid(fld) (qcvm->extfields.fld >= 0)
#define GetEdictFieldEval(ed, fld)                                             \
  ((Val *)((char *)&ed->v +                                                    \
           qcvm->extfields.fld * 4)) // caller must validate the field first

// from pr_cmds, no longer static so that pr_ext can use them.
sizebuf_t *WriteDest(void);
char *PR_GetTempString(void);
int PR_MakeTempString(const char *val);
char *PF_VarString(int first);
#define STRINGTEMP_BUFFERS 1024
#define STRINGTEMP_LENGTH 1024
void PF_Fixme(void); // the 'unimplemented' builtin. woot.

struct pr_extfuncs_s {
/*all vms*/
#define QCEXTFUNCS_COMMON                                                      \
  QCEXTFUNC(GameCommand,                                                       \
            "void(string cmdtext)") /*obsoleted by m_consolecommand, included  \
                                       for dp compat.*/                        \
/*csqc+ssqc*/
#define QCEXTFUNCS_GAME                                                        \
  QCEXTFUNC(EndFrame, "void()")                                                \
/*ssqc*/
#define QCEXTFUNCS_SV                                                          \
  QCEXTFUNC(SV_ParseClientCommand, "void(string cmd)")                         \
  QCEXTFUNC(SV_RunClientCommand, "void()")                                     \
/*csqc*/
#define QCEXTFUNCS_CS                                                          \
  QCEXTFUNC(CSQC_Init,                                                         \
            "void(float apilevel, string enginename, float engineversion)")    \
  QCEXTFUNC(CSQC_Shutdown, "void()")                                           \
  QCEXTFUNC(                                                                   \
      CSQC_DrawHud,                                                            \
      "void(vector virtsize, float showscores)") /*simple: for the             \
                                                    simple(+limited) hud-only  \
                                                    csqc interface.*/          \
  QCEXTFUNC(                                                                   \
      CSQC_DrawScores,                                                         \
      "void(vector virtsize, float showscores)") /*simple: (optional) for the  \
                                                    simple hud-only csqc       \
                                                    interface.*/               \
  QCEXTFUNC(CSQC_InputEvent,                                                   \
            "float(float evtype, float scanx, float chary, float devid)")      \
  QCEXTFUNC(CSQC_ConsoleCommand, "float(string cmdstr)")                       \
  QCEXTFUNC(CSQC_Parse_Event, "void()")                                        \
  QCEXTFUNC(CSQC_Parse_Damage, "float(float save, float take, vector dir)")    \
  QCEXTFUNC(CSQC_Parse_CenterPrint, "float(string msg)")                       \
  QCEXTFUNC(CSQC_Parse_Print, "void(string printmsg, float printlvl)")

#define QCEXTFUNC(n, t) func_t n;
  QCEXTFUNCS_COMMON
  QCEXTFUNCS_GAME
  QCEXTFUNCS_SV
  QCEXTFUNCS_CS
#undef QCEXTFUNC
};
extern cvar_t pr_checkextension; // if 0, extensions are disabled (unless they'd
                                 // be fatal, but they're still spammy)

struct pr_extglobals_s {
#define QCEXTGLOBALS_COMMON                                                    \
  QCEXTGLOBAL_FLOAT(time)                                                      \
  QCEXTGLOBAL_FLOAT(frametime)                                                 \
  // end
#define QCEXTGLOBALS_GAME                                                      \
  QCEXTGLOBAL_FLOAT(input_timelength)                                          \
  QCEXTGLOBAL_VECTOR(input_movevalues)                                         \
  QCEXTGLOBAL_VECTOR(input_angles)                                             \
  QCEXTGLOBAL_FLOAT(input_buttons)                                             \
  QCEXTGLOBAL_FLOAT(input_impulse)                                             \
  QCEXTGLOBAL_INT(input_weapon)                                                \
  QCEXTGLOBAL_VECTOR(input_cursor_screen)                                      \
  QCEXTGLOBAL_VECTOR(input_cursor_trace_start)                                 \
  QCEXTGLOBAL_VECTOR(input_cursor_trace_endpos)                                \
  QCEXTGLOBAL_FLOAT(input_cursor_entitynumber)                                 \
  QCEXTGLOBAL_FLOAT(physics_mode)                                              \
  // end
#define QCEXTGLOBALS_CSQC                                                      \
  QCEXTGLOBAL_FLOAT(cltime)                                                    \
  QCEXTGLOBAL_FLOAT(clframetime)                                               \
  QCEXTGLOBAL_FLOAT(maxclients)                                                \
  QCEXTGLOBAL_FLOAT(intermission)                                              \
  QCEXTGLOBAL_FLOAT(intermission_time)                                         \
  QCEXTGLOBAL_FLOAT(player_localnum)                                           \
  QCEXTGLOBAL_FLOAT(player_localentnum)                                        \
  // end
#define QCEXTGLOBAL_FLOAT(n) float *n;
#define QCEXTGLOBAL_INT(n) int *n;
#define QCEXTGLOBAL_VECTOR(n) float *n;
  QCEXTGLOBALS_COMMON
  QCEXTGLOBALS_GAME
  QCEXTGLOBALS_CSQC
#undef QCEXTGLOBAL_FLOAT
#undef QCEXTGLOBAL_INT
#undef QCEXTGLOBAL_VECTOR
};

struct pr_extfields_s { // various fields that might be wanted by the engine. -1
                        // == invalid

#define QCEXTFIELDS_ALL                                                        \
  /*renderscene means we need a number of fields here*/                        \
  QCEXTFIELD(alpha, ".float")       /*float*/                                  \
  QCEXTFIELD(scale, ".float")       /*float*/                                  \
  QCEXTFIELD(colormod, ".vector")   /*vector*/                                 \
  QCEXTFIELD(tag_entity, ".entity") /*entity*/                                 \
  QCEXTFIELD(tag_index, ".float")   /*float*/                                  \
  QCEXTFIELD(modelflags, ".float")  /*float, the upper 8 bits of .effects*/    \
  QCEXTFIELD(origin, ".vector")     /*for menuqc's addentity builtin.*/        \
  QCEXTFIELD(angles, ".vector")     /*for menuqc's addentity builtin.*/        \
  QCEXTFIELD(frame, ".float")       /*for menuqc's addentity builtin.*/        \
  QCEXTFIELD(skin, ".float")        /*for menuqc's addentity builtin.*/        \
                                    /*end of list*/
#define QCEXTFIELDS_GAME                                                       \
  /*stuff used by csqc+ssqc, but not menu*/                                    \
  QCEXTFIELD(customphysics, ".void()") /*function*/                            \
  QCEXTFIELD(gravity, ".float")        /*float*/                               \
                                       // end of list
#define QCEXTFIELDS_SS                                                         \
  /*ssqc-only*/                                                                \
  QCEXTFIELD(items2, "//.float")                                  /*float*/    \
  QCEXTFIELD(movement, ".vector")                                 /*vector*/   \
  QCEXTFIELD(viewmodelforclient, ".entity")                       /*entity*/   \
  QCEXTFIELD(exteriormodeltoclient, ".entity")                    /*entity*/   \
  QCEXTFIELD(traileffectnum, ".float")                            /*float*/    \
  QCEXTFIELD(emiteffectnum, ".float")                             /*float*/    \
  QCEXTFIELD(button3, ".float")                                   /*float*/    \
  QCEXTFIELD(button4, ".float")                                   /*float*/    \
  QCEXTFIELD(button5, ".float")                                   /*float*/    \
  QCEXTFIELD(button6, ".float")                                   /*float*/    \
  QCEXTFIELD(button7, ".float")                                   /*float*/    \
  QCEXTFIELD(button8, ".float")                                   /*float*/    \
  QCEXTFIELD(viewzoom, ".float")                                  /*float*/    \
  QCEXTFIELD(SendEntity, ".float(entity to, float changedflags)") /*function*/ \
  QCEXTFIELD(SendFlags, ".float") /*float. :( */                               \
                                  // end of list

#define QCEXTFIELD(n, t) int n;
  QCEXTFIELDS_ALL
  QCEXTFIELDS_GAME
  QCEXTFIELDS_SS
#undef QCEXTFIELD
};

typedef struct {
  int s;
  dfunction_t *f;
} prstack_t;

typedef struct areanode_s {
  int axis; // -1 = leaf node
  float dist;
  struct areanode_s *children[2];
  common::Link trigger_edicts;
  common::Link solid_edicts;
} areanode_t;
#define VANILLA_AREA_DEPTH 4
#define MAX_AREA_DEPTH 9
#define AREA_NODES (2 << MAX_AREA_DEPTH)

typedef struct hash_map_s hash_map_t;

struct qcvm_s {
  dprograms_t *progs;
  dfunction_t *functions;
  hash_map_t *function_map;
  dstatement_t *statements;
  float *globals;    /* same as pr_global_struct */
  ddef_t *fielddefs; // yay reflection.
  hash_map_t *fielddefs_map;

  int edict_size; /* in bytes */

  builtin_t builtins[1024];
  int numbuiltins;

  int argc;

  bool trace;
  dfunction_t *xfunction;
  int xstatement;

  unsigned short progscrc; // crc16 of the entire file
  unsigned int progshash;  // folded file md4
  unsigned int progssize;  // file size (bytes)

  struct pr_extglobals_s extglobals;
  struct pr_extfuncs_s extfuncs;
  struct pr_extfields_s extfields;

  // was static inside pr_edict
  char *strings;
  int stringssize;
  const char **knownstrings;
  bool *knownstringsowned;
  int maxknownstrings;
  int numknownstrings;
  int progsstrings; // allocated by PR_MergeEngineFieldDefs (), not tied to
                    // edicts
  int freeknownstrings;
  ddef_t *globaldefs;
  hash_map_t *globaldefs_map;

  unsigned char *knownzone;
  size_t knownzonesize;

  // originally defined in pr_exec, but moved into the switchable qcvm struct
#define MAX_STACK_DEPTH 1024 /*was 64*/ /* was 32 */
  prstack_t stack[MAX_STACK_DEPTH];
  int depth;

#define LOCALSTACK_SIZE 16384 /* was 2048*/
  int localstack[LOCALSTACK_SIZE];
  int localstack_used;

  // originally part of the sv_state_t struct
  // FIXME: put worldmodel in here too.
  double time;
  int num_edicts;
  int reserved_edicts;
  int max_edicts;
  int min_edicts; // for savegame compatibility
  Dict *edicts;   // can NOT be array indexed, because Dict is variable
                  // sized, but can be used to reference the world ent
  Dict *free_edicts_head;
  Dict *free_edicts_tail;
  struct qmodel_s *worldmodel;
  struct qmodel_s *(*GetModel)(
      int modelindex); // returns the model for the given index, or null.

  // originally from world.c
  areanode_t areanodes[AREA_NODES];
  int numareanodes;
};
extern globalvars_t *pr_global_struct;

extern qcvm_t *qcvm;
void PR_SwitchQCVM(qcvm_t *nvm);

extern builtin_t pr_ssqcbuiltins[];
extern int pr_ssqcnumbuiltins;
extern builtin_t pr_csqcbuiltins[];
extern int pr_csqcnumbuiltins;

} // namespace progs
