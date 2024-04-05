/*
Copyright (C) 2023 Axel Gneiting

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

module hashmap;
import memory;

//#include "quakedef.h"

namespace hashmap {

//#define MIN_KEY_VALUE_STORAGE_SIZE 16
//#define MIN_HASH_SIZE 32

/*
=================
GetKeyImpl
=================
*/
void *GetKeyImpl(hash_map_t *map, uint32_t index) {
  return (uint8_t *)map->keys + (map->key_size * index);
}

/*
=================
GetValueImpl
=================
*/
void *GetValueImpl(hash_map_t *map, uint32_t index) {
  return (uint8_t *)map->values + (map->value_size * index);
}

/*
=================
Rehash
=================
*/
static void Rehash(hash_map_t *map, const uint32_t new_size) {
  if (map->hash_size >= new_size)
    return;
  map->hash_size = new_size;
  map->hash_to_index =
      Mem_Realloc(map->hash_to_index, map->hash_size * sizeof(uint32_t));
  memset(map->hash_to_index, 0xFF, map->hash_size * sizeof(uint32_t));
  for (uint32_t i = 0; i < map->num_entries; ++i) {
    void *key = GetKeyImpl(map, i);
    const uint32_t hash = map->hasher(key);
    const uint32_t hash_index = hash & (map->hash_size - 1);
    map->index_chain[i] = map->hash_to_index[hash_index];
    map->hash_to_index[hash_index] = i;
  }
}

/*
=================
ExpandKeyValueStorage
=================
*/
//static void ExpandKeyValueStorage(hash_map_t *map, const uint32_t new_size) {
  //map->keys = Mem_Realloc(map->keys, new_size * map->key_size);
  //map->values = Mem_Realloc(map->values, new_size * map->value_size);
  //map->index_chain = Mem_Realloc(map->index_chain, new_size * sizeof(uint32_t));
  //map->key_value_storage_size = new_size;
//}

/*
=================
CreateImpl
=================
*/
//hash_map_t *CreateImpl(const uint32_t key_size, const uint32_t value_size,
                       //uint32_t (*hasher)(const void *const),
                       //bool (*comp)(const void *const, const void *const)) {
  //hash_map_t *map = memory::alloc(sizeof(hash_map_t));
  //map->key_size = key_size;
  //map->value_size = value_size;
  //map->hasher = hasher;
  //map->comp = comp;
  //return map;
//}

/*
=================
Destroy
=================
*/
void Destroy(hash_map_t *map) {
  Mem_Free(map->hash_to_index);
  Mem_Free(map->index_chain);
  Mem_Free(map->keys);
  Mem_Free(map->values);
  Mem_Free(map);
}

/*
=================
Reserve
=================
*/
void Reserve(hash_map_t *map, int capacity) {
  const uint32_t new_key_value_storage_size = Q_nextPow2(capacity);
  if (map->key_value_storage_size < new_key_value_storage_size)
    ExpandKeyValueStorage(map, new_key_value_storage_size);
  const uint32_t new_hash_size = Q_nextPow2(capacity + (capacity / 4));
  if (map->hash_size < new_hash_size)
    Rehash(map, new_hash_size);
}

/*
=================
InsertImpl
=================
*/
//bool InsertImpl(hash_map_t *map, const uint32_t key_size,
                    //const uint32_t value_size, const void *const key,
                    //const void *const value) {
  //assert(map->key_size == key_size);
  //assert(map->value_size == value_size);

  //if (map->num_entries >= map->key_value_storage_size)
    //ExpandKeyValueStorage(map, q_max(map->key_value_storage_size * 2,
                                     //MIN_KEY_VALUE_STORAGE_SIZE));
  //if ((map->num_entries + (map->num_entries / 4)) >= map->hash_size)
    //Rehash(map, q_max(map->hash_size * 2, MIN_HASH_SIZE));

  //const uint32_t hash = map->hasher(key);
  //const uint32_t hash_index = hash & (map->hash_size - 1);
  //{
    //uint32_t storage_index = map->hash_to_index[hash_index];
    //while (storage_index != UINT32_MAX) {
      //const void *const storage_key = GetKeyImpl(map, storage_index);
      //if (map->comp ? map->comp(key, storage_key)
                    //: (memcmp(key, storage_key, key_size) == 0)) {
        //memcpy(GetValueImpl(map, storage_index), value, value_size);
        //return true;
      //}
      //storage_index = map->index_chain[storage_index];
    //}
  //}

  //map->index_chain[map->num_entries] = map->hash_to_index[hash_index];
  //map->hash_to_index[hash_index] = map->num_entries;
  //memcpy(GetKeyImpl(map, map->num_entries), key, key_size);
  //memcpy(GetValueImpl(map, map->num_entries), value, value_size);
  //++map->num_entries;

  //return false;
//}

/*
=================
EraseImpl
=================
*/
bool EraseImpl(hash_map_t *map, const uint32_t key_size,
                   const void *const key) {
  assert(key_size == map->key_size);
  if (map->num_entries == 0)
    return false;

  const uint32_t hash = map->hasher(key);
  const uint32_t hash_index = hash & (map->hash_size - 1);
  uint32_t storage_index = map->hash_to_index[hash_index];
  uint32_t *prev_storage_index_ptr = NULL;
  while (storage_index != UINT32_MAX) {
    const void *storage_key = GetKeyImpl(map, storage_index);
    if (map->comp ? map->comp(key, storage_key)
                  : (memcmp(key, storage_key, key_size) == 0)) {
      {
        // Remove found key from index
        if (prev_storage_index_ptr == NULL)
          map->hash_to_index[hash_index] = map->index_chain[storage_index];
        else
          *prev_storage_index_ptr = map->index_chain[storage_index];
      }

      const uint32_t last_index = map->num_entries - 1;
      const uint32_t last_hash = map->hasher(GetKeyImpl(map, last_index));
      const uint32_t last_hash_index = last_hash & (map->hash_size - 1);

      if (storage_index == last_index) {
        --map->num_entries;
        return true;
      }

      {
        // Remove last key from index
        if (map->hash_to_index[last_hash_index] == last_index)
          map->hash_to_index[last_hash_index] = map->index_chain[last_index];
        else {
          bool found = false;
          for (uint32_t last_storage_index =
                   map->hash_to_index[last_hash_index];
               last_storage_index != UINT32_MAX;
               last_storage_index = map->index_chain[last_storage_index]) {
            if (map->index_chain[last_storage_index] == last_index) {
              map->index_chain[last_storage_index] =
                  map->index_chain[last_index];
              found = true;
              break;
            }
          }
          (void)found;
          assert(found);
        }
      }

      {
        // Copy last key to current key position and add back to index
        memcpy(GetKeyImpl(map, storage_index), GetKeyImpl(map, last_index),
               map->key_size);
        memcpy(GetValueImpl(map, storage_index), GetValueImpl(map, last_index),
               map->value_size);
        map->index_chain[storage_index] = map->hash_to_index[last_hash_index];
        map->hash_to_index[last_hash_index] = storage_index;
      }

      --map->num_entries;
      return true;
    }
    prev_storage_index_ptr = &map->index_chain[storage_index];
    storage_index = map->index_chain[storage_index];
  }
  return false;
}

/*
=================
LookupImpl
=================
*/
void *LookupImpl(hash_map_t *map, const uint32_t key_size,
                 const void *const key) {
  assert(map->key_size == key_size);

  if (map->num_entries == 0)
    return NULL;

  const uint32_t hash = map->hasher(key);
  const uint32_t hash_index = hash & (map->hash_size - 1);
  uint32_t storage_index = map->hash_to_index[hash_index];
  while (storage_index != UINT32_MAX) {
    const void *const storage_key = GetKeyImpl(map, storage_index);
    if (map->comp ? map->comp(key, storage_key)
                  : (memcmp(key, storage_key, key_size) == 0))
      return (uint8_t *)map->values + (storage_index * map->value_size);
    storage_index = map->index_chain[storage_index];
  }

  return NULL;
}

/*
=================
Size
=================
*/
uint32_t Size(hash_map_t *map) { return map->num_entries; }

#ifdef _DEBUG
/*
=================
TestAssert
=================
*/
#define TestAssert(cond, what)                                                 \
  if (!(cond)) {                                                               \
    Con_Printf("%s", what);                                                    \
    abort();                                                                   \
  }

/*
=================
BasicTest
=================
*/
static void BasicTest(const bool reserve) {
  const int TEST_SIZE = 1000;
  hash_map_t *map = Create(int32_t, int64_t, &HashInt32, NULL);
  if (reserve)
    Reserve(map, TEST_SIZE);
  for (int i = 0; i < TEST_SIZE; ++i) {
    int64_t value = i;
    TestAssert(!Insert(map, &i, &value),
               va("%d should not be overwritten\n", i));
  }
  for (int i = 0; i < TEST_SIZE; ++i)
    TestAssert(*Lookup(int64_t, map, &i) == i, va("Wrong lookup for %d\n", i));
  for (int i = 0; i < TEST_SIZE; i += 2)
    Erase(map, &i);
  for (int i = 1; i < TEST_SIZE; i += 2)
    TestAssert(*Lookup(int64_t, map, &i) == i, va("Wrong lookup for %d\n", i));
  for (int i = 0; i < TEST_SIZE; i += 2)
    TestAssert(Lookup(int64_t, map, &i) == NULL,
               va("Wrong lookup for %d\n", i));
  for (int i = 0; i < TEST_SIZE; ++i)
    Erase(map, &i);
  TestAssert(Size(map) == 0, "Map is not empty");
  for (int i = 0; i < TEST_SIZE; ++i)
    TestAssert(Lookup(int64_t, map, &i) == NULL,
               va("Wrong lookup for %d\n", i));
  Destroy(map);
}

/*
=================
BasicTest
=================
*/
static void StressTest(void) {
  srand(0);
  const int TEST_SIZE = 10000;
  TEMP_ALLOC(int64_t, keys, TEST_SIZE);
  hash_map_t *map = Create(int64_t, int32_t, &HashInt64, NULL);
  for (int j = 0; j < 10; ++j) {
    for (int i = 0; i < TEST_SIZE; ++i) {
      keys[i] = i;
    }
    for (int i = TEST_SIZE - 1; i > 0; --i) {
      const int swap_index = rand() % (i + 1);
      const int temp = keys[swap_index];
      keys[swap_index] = keys[i];
      keys[i] = temp;
    }
    for (int i = 0; i < TEST_SIZE; ++i)
      Insert(map, &keys[i], &i);
    for (int i = 0; i < TEST_SIZE; ++i)
      TestAssert(*Lookup(int32_t, map, &keys[i]) == i,
                 va("Wrong lookup for %d\n", i));
    for (int i = TEST_SIZE - 1; i >= 0; --i)
      Erase(map, &keys[i]);
    TestAssert(Size(map) == 0, "Map is not empty");
  }
  Destroy(map);
  TEMP_FREE(keys);
}

/*
=================
Testf
=================
*/
void Testf(void) {
  BasicTest(false);
  BasicTest(true);
  StressTest();
}
#endif

} // namespace hashmap
