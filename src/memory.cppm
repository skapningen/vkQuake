/*
 * memory.cppm -- memory allocator
 *
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
// #include "quakedef.h"
#include <cstddef>
#include <mimalloc.h>

export module memory;

export namespace memory {

// Mem_Alloc will always return zero initialized memory
// A lot of old code was assuming this and overhead is negligible

size_t thread_local thread_stack_alloc_size = 0;
size_t max_thread_stack_alloc_size = 0;

void init();

/*
====================
alloc
====================
*/
// TODO: require mimalloc for now
void *alloc(const size_t size) { return mi_calloc(1, size); }

void *alloc_nonzero(const size_t size);
void *realloc(void *ptr, const size_t size);

/*
====================
free
====================
*/
void free(const void *ptr) { mi_free((void *)ptr); }

#define SAFE_FREE(ptr)                                                         \
  do {                                                                         \
    free(ptr);                                                                 \
    ptr = nullptr;                                                             \
  } while (false)

#define TEMP_ALLOC_TEMPLATE(type, var, size, zeroed, cond)                     \
  type *var;                                                                   \
  qboolean temp_alloc_##var##_on_heap = false;                                 \
  const size_t temp_alloc_##var##_size = sizeof(type) * (size);                \
  if (cond)                                                                    \
    if ((thread_stack_alloc_size + temp_alloc_##var##_size) >                  \
        max_thread_stack_alloc_size) {                                         \
      if (zeroed)                                                              \
        var = (type *)Mem_Alloc(temp_alloc_##var##_size);                      \
      else                                                                     \
        var = (type *)Mem_AllocNonZero(temp_alloc_##var##_size);               \
      temp_alloc_##var##_on_heap = true;                                       \
    } else {                                                                   \
      var = (type *)alloca(temp_alloc_##var##_size);                           \
      if (zeroed)                                                              \
        memset(var, 0, temp_alloc_##var##_size);                               \
      thread_stack_alloc_size += temp_alloc_##var##_size;                      \
    }                                                                          \
  else                                                                         \
    var = (type *)NULL;

#define TEMP_ALLOC(type, var, size)                                            \
  TEMP_ALLOC_TEMPLATE(type, var, size, false, true)
#define TEMP_ALLOC_ZEROED(type, var, size)                                     \
  TEMP_ALLOC_TEMPLATE(type, var, size, true, true)
#define TEMP_ALLOC_COND(type, var, size, cond)                                 \
  TEMP_ALLOC_TEMPLATE(type, var, size, false, cond)
#define TEMP_ALLOC_ZEROED_COND(type, var, size, cond)                          \
  TEMP_ALLOC_TEMPLATE(type, var, size, true, cond)

#define TEMP_FREE(var)                                                         \
  if (temp_alloc_##var##_on_heap) {                                            \
    Mem_Free(var);                                                             \
  } else {                                                                     \
    thread_stack_alloc_size -= temp_alloc_##var##_size;                        \
  }

} // namespace memory
