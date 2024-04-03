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

// comndef.h  -- general definitions

module;
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>

export module common;
import quakestddef;
import memory;
import cvar;

static char *largv[MAX_NUM_ARGVS + 1];
static char argvdummy[] = " ";

int32_t safemode;

cvar::CVar registered = {
    "registered", "1",
    cvar::ROM}; /* set to correct value in COM_CheckRegistered() */
cvar::CVar cmdline = {"cmdline", "",
                  cvar::ROM /*|CVAR_SERVERINFO*/}; /* sending cmdline upon
                                                     CCREQ_RULE_INFO is evil */

static bool com_modified; // set true if using non-id files

bool fitzmode;
bool multiuser;

export namespace common {

// TODO: replace with standard templates
//  #define GENERIC_TYPES(x, separator) \
  //x(int, i) separator x(unsigned int, u) \
  //separator x(long, l) \
  //separator x(unsigned long, ul) \
  //separator x(long long, ll) \
  //separator x(unsigned long long, ull) \
  //separator x(float, f) \ separator x(double, d)

// #define COMMA ,
// #define NO_COMMA

//#define IMPL_GENERIC_FUNCS(type, suffix)                                       \
  //static inline type q_min_##suffix(type a, type b) {                          \
    //return (a < b) ? a : b;                                                    \
  //}                                                                            \
  //static inline type q_max_##suffix(type a, type b) {                          \
    //return (a > b) ? a : b;                                                    \
  //}                                                                            \
  //static inline type clamp_##suffix(type minval, type val, type maxval) {      \
    //return (val < minval) ? minval : ((val > maxval) ? maxval : val);          \
  //}

// GENERIC_TYPES(IMPL_GENERIC_FUNCS, NO_COMMA)

//#define SELECT_Q_MIN(type, suffix)                                             \
  //type:                                                                        \
  //q_min_##suffix
//#define q_min(a, b)                                                            \
  //_Generic((a) + (b), GENERIC_TYPES(SELECT_Q_MIN, COMMA))(a, b)

//#define SELECT_Q_MAX(type, suffix)                                             \
  //type:                                                                        \
  //q_max_##suffix
//#define q_max(a, b)                                                            \
  //_Generic((a) + (b), GENERIC_TYPES(SELECT_Q_MAX, COMMA))(a, b)

//#define SELECT_CLAMP(type, suffix)                                             \
  //type:                                                                        \
  //clamp_##suffix
//#define CLAMP(minval, val, maxval)                                             \
  //_Generic((minval) + (val) + (maxval),                                        \
      //GENERIC_TYPES(SELECT_CLAMP, COMMA))(minval, val, maxval)

//#define GENERIC_INT_TYPES(x, separator)                                        \
  //x(int, i) separator x(unsigned int, u)                                       \
  //separator x(long, l)                                                         \
  //separator x(unsigned long, ul)                                               \
  //separator x(long long, ll)                                                   \
  //separator x(unsigned long long, ull)

//#define IMPL_GENERIC_INT_FUNCS(type, suffix)                                   \
  //static inline type q_align_##suffix(type size, type alignment) {             \
    //return ((size & (alignment - 1)) == 0)                                     \
               //? size                                                          \
               //: (size + alignment - (size & (alignment - 1)));                \
  //}

// GENERIC_INT_TYPES(IMPL_GENERIC_INT_FUNCS, NO_COMMA)

//#define SELECT_ALIGN(type, suffix)                                             \
  //type:                                                                        \
  //q_align_##suffix
//#define q_align(size, alignment)                                               \
  //_Generic((size) + (alignment),                                               \
      //GENERIC_INT_TYPES(SELECT_ALIGN, COMMA))(size, alignment)
// clang-format on

#define countof(x) (sizeof(x) / sizeof((x)[0]))

#define ZEROED_STRUCT(type, name)                                              \
  type name;                                                                   \
  memset(&name, 0, sizeof(type));
#define ZEROED_STRUCT_ARRAY(type, name, count)                                 \
  type name[count];                                                            \
  memset(name, 0, sizeof(type) * count);

#define CHAIN_PNEXT(next_ptr, chained)                                         \
  {                                                                            \
    *next_ptr = &chained;                                                      \
    next_ptr = &chained.pNext;                                                 \
  }

typedef struct sizebuf_s {
  bool allowoverflow; // if false, do a Sys_Error
  bool overflowed;    // set to true if the buffer size failed
  uint8_t *data;
  int maxsize;
  int cursize;
} sizebuf_t;

void SZ_Alloc(sizebuf_t *buf, int startsize);
void SZ_Free(sizebuf_t *buf);
void SZ_Clear(sizebuf_t *buf);
void *SZ_GetSpace(sizebuf_t *buf, int length);
void SZ_Write(sizebuf_t *buf, const void *data, int length);
void SZ_Print(sizebuf_t *buf, const char *data); // strcats onto the sizebuf

//============================================================================

typedef struct link_s {
  struct link_s *prev, *next;
} link_t;

void ClearLink(link_t *l);
void RemoveLink(link_t *l);
void InsertLinkBefore(link_t *l, link_t *before);
void InsertLinkAfter(link_t *l, link_t *after);

// (type *)STRUCT_FROM_LINK(link_t *link, type, member)
// ent = STRUCT_FROM_LINK(link,entity_t,order)
// FIXME: remove this mess!
#define STRUCT_FROM_LINK(l, t, m) ((t *)((uint8_t *)l - offsetof(t, m)))

//============================================================================

extern short (*BigShort)(short l);
extern short (*LittleShort)(short l);
extern int (*BigLong)(int l);
extern int (*LittleLong)(int l);
extern float (*BigFloat)(float l);
extern float (*LittleFloat)(float l);

//============================================================================

void MSG_WriteChar(sizebuf_t *sb, int c);
void MSG_WriteByte(sizebuf_t *sb, int c);
void MSG_WriteShort(sizebuf_t *sb, int c);
void MSG_WriteLong(sizebuf_t *sb, int c);
void MSG_WriteFloat(sizebuf_t *sb, float f);
void MSG_WriteStringUnterminated(sizebuf_t *sb, const char *s);
void MSG_WriteString(sizebuf_t *sb, const char *s);
void MSG_WriteCoord(sizebuf_t *sb, float f, unsigned int flags);
void MSG_WriteAngle(sizebuf_t *sb, float f, unsigned int flags);
void MSG_WriteAngle16(sizebuf_t *sb, float f, unsigned int flags); // johnfitz
void MSG_WriteEntity(sizebuf_t *sb, unsigned int index,
                     unsigned int pext2); // spike
struct entity_state_s;
void MSG_WriteStaticOrBaseLine(sizebuf_t *buf, int idx,
                               struct entity_state_s *state,
                               unsigned int protocol_pext2,
                               unsigned int protocol,
                               unsigned int protocolflags); // spike

extern int msg_readcount;
extern bool msg_badread; // set if a read goes beyond end of message

void MSG_BeginReading(void);
int MSG_ReadChar(void);
int MSG_ReadByte(void);
int MSG_ReadShort(void);
int MSG_ReadLong(void);
float MSG_ReadFloat(void);
const char *MSG_ReadString(void);

float MSG_ReadCoord(unsigned int flags);
float MSG_ReadAngle(unsigned int flags);
float MSG_ReadAngle16(unsigned int flags);       // johnfitz
uint8_t *MSG_ReadData(unsigned int length);      // spike
unsigned int MSG_ReadEntity(unsigned int pext2); // spike

void COM_Effectinfo_Enumerate(
    int (*cb)(const char *pname)); // spike -- for dp compat

//============================================================================

int wildcmp(const char *wild, const char *string);
void Info_RemoveKey(char *info, const char *key);
void Info_SetKey(char *info, size_t infosize, const char *key, const char *val);
const char *Info_GetKey(const char *info, const char *key, char *out,
                        size_t outsize);
void Info_Print(const char *info);
void Info_Enumerate(const char *info,
                    void (*cb)(void *ctx, const char *key, const char *value),
                    void *cbctx);

#include "strl_fn.h"

/* locale-insensitive strcasecmp replacement functions: */
int q_strcasecmp(const char *s1, const char *s2);
int q_strncasecmp(const char *s1, const char *s2, size_t n);

/* locale-insensitive case-insensitive alternative to strstr */
char *q_strcasestr(const char *haystack, const char *needle);

/* locale-insensitive strlwr/upr replacement functions: */
char *q_strlwr(char *str);
char *q_strupr(char *str);



//============================================================================

extern thread_local char com_token[1024];
extern bool com_eof;

const char *COM_Parse(const char *data);

extern int com_argc;
extern char **com_argv;

extern int safemode;
/* safe mode: in true, the engine will behave as if one
   of these arguments were actually on the command line:
   -nosound, -nocdaudio, -nomidi, -stdvid, -dibonly,
   -nomouse, -nojoy, -nolan
 */

int COM_CheckParm(const char *parm);

void COM_Init(void);
void COM_InitArgv(int argc, char **argv);
void COM_InitFilesystem(void);

const char *COM_SkipPath(const char *pathname);
void COM_StripExtension(const char *in, char *out, size_t outsize);
void COM_FileBase(const char *in, char *out, size_t outsize);
void COM_AddExtension(char *path, const char *extension, size_t len);
#if 0 /* COM_DefaultExtension can be dangerous */
void COM_DefaultExtension (char *path, const char *extension, size_t len);
#endif
const char *COM_FileGetExtension(const char *in); /* doesn't return NULL */
void COM_ExtractExtension(const char *in, char *out, size_t outsize);
void COM_CreatePath(char *path);

char *va(const char *format, ...) __attribute__((__format__(__printf__, 1, 2)));
// does a varargs printf into a temp buffer

unsigned COM_HashString(const char *str);

// localization support for 2021 rerelease version:
void LOC_Init(void);
void LOC_Shutdown(void);
const char *LOC_GetRawString(const char *key);
const char *LOC_GetString(const char *key);
bool LOC_HasPlaceholders(const char *str);
size_t LOC_Format(const char *format,
                  const char *(*getarg_fn)(int idx, void *userdata),
                  void *userdata, char *out, size_t len);

//============================================================================

// QUAKEFS
typedef struct {
  char name[MAX_QPATH];
  int filepos, filelen;
} packfile_t;

typedef struct pack_s {
  char filename[MAX_OSPATH];
  int handle;
  int numfiles;
  packfile_t *files;
} pack_t;

typedef struct searchpath_s {
  unsigned int path_id; // identifier assigned to the game directory
                        // Note that <install_dir>/game1 and
                        // <userdir>/game1 have the same id.
  char filename[MAX_OSPATH];
  pack_t *pack; // only one of filename / pack will be used
  struct searchpath_s *next;
} searchpath_t;

extern searchpath_t *com_searchpaths;
extern searchpath_t *com_base_searchpaths;

extern thread_local int com_filesize;
struct cache_user_s;

extern char com_basedir[MAX_OSPATH];
extern char com_gamedir[MAX_OSPATH];
extern thread_local int
    file_from_pak; // global indicating that file came from a pak

const char *COM_GetGameNames(bool full);
bool COM_GameDirMatches(const char *tdirs);
bool COM_ModForbiddenChars(const char *p);

void COM_WriteFile(const char *filename, const void *data, int len);
int COM_OpenFile(const char *filename, int *handle, unsigned int *path_id);
int COM_FOpenFile(const char *filename, FILE **file, unsigned int *path_id);
bool COM_FileExists(const char *filename, unsigned int *path_id);
void COM_CloseFile(int h);

uint8_t *COM_LoadFile(const char *path, unsigned int *path_id);

// Opens the given path directly, ignoring search paths.
// Returns NULL on failure, or else a '\0'-terminated malloc'ed buffer.
// Loads in "t" mode so CRLF to LF translation is performed on Windows.
uint8_t *COM_LoadMallocFile_TextMode_OSPath(const char *path, long *len_out);

// Attempts to parse an int, followed by a newline.
// Returns advanced buffer position.
// Doesn't signal parsing failure, but this is not needed for savegame loading.
const char *COM_ParseIntNewline(const char *buffer, int *value);

// Attempts to parse a float followed by a newline.
// Returns advanced buffer position.
const char *COM_ParseFloatNewline(const char *buffer, float *value);

// Parse a string of non-whitespace into com_token, then tries to consume a
// newline. Returns advanced buffer position.
const char *COM_ParseStringNewline(const char *buffer);

namespace fs {
#define FS_ENT_NONE (0)
#define FS_ENT_FILE (1 << 0)
#define FS_ENT_DIRECTORY (1 << 1)

/* The following FS_*() stdio replacements are necessary if one is
 * to perform non-sequential reads on files reopened on pak files
 * because we need the bookkeeping about file start/end positions.
 * Allocating and filling in the Handle structure is the users'
 * responsibility when the file is initially opened. */

struct Handle {
  FILE *file;
  bool pak;       /* is the file read from a pak */
  int32_t start;  /* file or data start position */
  int32_t length; /* file or data size */
  int32_t pos;    /* current position relative to start */
};

size_t fread(void *ptr, size_t size, size_t nmemb, Handle *fh) {
  if (!fh) {
    errno = EBADF;
    return 0;
  }
  if (!ptr) {
    errno = EFAULT;
    return 0;
  }
  if (!size || !nmemb) { /* no error, just zero bytes wanted */
    errno = 0;
    return 0;
  }

  int32_t byte_size = nmemb * size;
  if (byte_size > fh->length - fh->pos) /* just read to end */
    byte_size = fh->length - fh->pos;
  int32_t bytes_read = fread(ptr, 1, byte_size, fh->file);
  fh->pos += bytes_read;

  /* fread() must return the number of elements read,
   * not the total number of bytes. */
  int32_t nmemb_read = bytes_read / size;
  /* even if the last member is only read partially
   * it is counted as a whole in the return value. */
  if (bytes_read % size)
    nmemb_read++;

  return nmemb_read;
}

int32_t fseek(Handle *fh, long offset, int32_t whence) {
  /* I don't care about 64 bit off_t or fseeko() here.
   * the quake/hexen2 file system is 32 bits, anyway. */

  if (!fh) {
    errno = EBADF;
    return -1;
  }

  /* the relative file position shouldn't be smaller
   * than zero or bigger than the filesize. */
  switch (whence) {
  case SEEK_SET:
    break;
  case SEEK_CUR:
    offset += fh->pos;
    break;
  case SEEK_END:
    offset = fh->length + offset;
    break;
  default:
    errno = EINVAL;
    return -1;
  }

  if (offset < 0) {
    errno = EINVAL;
    return -1;
  }

  if (offset > fh->length) /* just seek to end */
    offset = fh->length;

  int32_t ret = fseek(fh->file, fh->start + offset, SEEK_SET);
  if (ret < 0)
    return ret;

  fh->pos = offset;
  return 0;
}

int32_t ftell(Handle *fh) {
  if (!fh) {
    errno = EBADF;
    return -1;
  }
  return fh->pos;
}

void rewind(Handle *fh) {
  if (!fh)
    return;
  clearerr(fh->file);
  fseek(fh->file, fh->start, SEEK_SET);
  fh->pos = 0;
}

int32_t feof(Handle *fh) {
  if (!fh) {
    errno = EBADF;
    return -1;
  }
  if (fh->pos >= fh->length)
    return -1;
  return 0;
}

int32_t ferror(Handle *fh) {
  if (!fh) {
    errno = EBADF;
    return -1;
  }
  return ferror(fh->file);
}

int32_t fclose(Handle *fh) {
  if (!fh) {
    errno = EBADF;
    return -1;
  }
  return fclose(fh->file);
}

int32_t fgetc(Handle *fh) {
  if (!fh) {
    errno = EBADF;
    return EOF;
  }
  if (fh->pos >= fh->length)
    return EOF;
  fh->pos += 1;
  return fgetc(fh->file);
}

char *fgets(char *s, int size, Handle *fh) {
  char *ret;

  if (feof(fh))
    return nullptr;

  if (size > (fh->length - fh->pos) + 1)
    size = (fh->length - fh->pos) + 1;

  ret = fgets(s, size, fh->file);
  fh->pos = ftell(fh->file) - fh->start;

  return ret;
}

int32_t filelength(Handle *fh) {
  if (!fh) {
    errno = EBADF;
    return -1;
  }
  return fh->length;
}

} // namespace fs

extern struct cvar_s registered;
extern bool standard_quake, rogue, hipnotic;
extern bool fitzmode;
/* if true, run in fitzquake mode disabling custom quakespasm hacks */

} // namespace common
