/*
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
#include <pthread.h>
#include <cstdint>

module memory;

namespace memory {

void init() {
  pthread_attr_t attr;
  size_t stacksize;
  if (pthread_attr_init(&attr) != 0)
    return;
  if (pthread_attr_getstacksize(&attr, &stacksize) != 0)
    return;
  max_thread_stack_alloc_size = (size_t)CLAMP(
      0ll, (int64_t)stacksize - THREAD_STACK_RESERVATION, MAX_STACK_ALLOC_SIZE);
  pthread_attr_destroy(&attr);
}

} // namespace memory
