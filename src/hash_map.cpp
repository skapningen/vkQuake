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
#include <algorithm>
#include <cstdint>
#include <mimalloc-new-delete.h>

export module hashmap;
import memory;

namespace hashmap {

constexpr size_t MIN_KEY_VALUE_STORAGE_SIZE = 16;
constexpr size_t MIN_HASH_SIZE = 32;

// hash_map_t
template <typename K, typename V> struct HashMap {
  using HasherType = uint32_t (*)(const K &);
  using CompareType = bool (*)(const K &, const K &);
  using IndexSize = std::size_t;

  std::size_t num_entries{0};
  uint32_t hash_size{0};
  std::size_t key_value_storage_size{0};
  HasherType hasher{nullptr};
  CompareType comp{nullptr};
  IndexSize *hash_to_index{nullptr};
  std::size_t *index_chain{nullptr};
  void *keys{nullptr};
  void *values{nullptr};

  HashMap(HasherType hasher, CompareType comp) : hasher{hasher}, comp{comp} {}

  void expand_key_value_storage(const std::size_t new_size) {
    keys = memory::realloc(keys, new_size * sizeof(K));
    values = memory::realloc(values, new_size * sizeof(V));
    index_chain = memory::realloc(index_chain, new_size * sizeof(std::size_t));
    key_value_storage_size = new_size;
  }

  void rehash(const std::size_t new_size) {
    if (hash_size >= new_size) {
      return;
    }
    hash_size = new_size;
    hash_to_index =
        memory::realloc(hash_to_index, hash_size * sizeof(IndexSize));
    memset(hash_to_index, 0xFF, hash_size * sizeof(IndexSize));
    for (std::size_t i{}; i < num_entries; ++i) {
      void *key = GetKeyImpl(map, i);
      const uint32_t hash = hasher(key);
      const uint32_t hash_index = hash & (hash_size - 1);
      index_chain[i] = hash_to_index[hash_index];
      hash_to_index[hash_index] = i;
    }
  }

  bool insert(const K &key, const V &value) {
    if (num_entries >= key_value_storage_size) {
      expand_key_value_storage(
          std::max(key_value_storage_size * 2, MIN_KEY_VALUE_STORAGE_SIZE));
    }
    if ((num_entries + (num_entries / 4)) >= hash_size) {
      rehash(std::max(hash_size * 2, MIN_HASH_SIZE));
    }

    //const uint32_t hash = hasher(key);
    // hash_size is always a power of 2
    const uint32_t hash_index = hasher(key) & (hash_size - 1);
    {
      uint32_t storage_index = hash_to_index[hash_index];
      // check for collision
      while (storage_index != UINT32_MAX) {
        const void *const storage_key = GetKeyImpl(map, storage_index);
        if (comp ? comp(key, storage_key)
                      : (memcmp(key, storage_key, key_size) == 0)) {
          memcpy(GetValueImpl(map, storage_index), value, value_size);
          return true;
        }
        storage_index = index_chain[storage_index];
      }
    }

    index_chain[num_entries] = hash_to_index[hash_index];
    hash_to_index[hash_index] = num_entries;
    memcpy(GetKeyImpl(map, map->num_entries), key, key_size);
    memcpy(GetValueImpl(map, map->num_entries), value, value_size);
    ++num_entries;

    return false;
  }
};

hash_map_t *CreateImpl(const uint32_t key_size, const uint32_t value_size,
                       uint32_t (*hasher)(const void *const),
                       bool (*comp)(const void *const, const void *const));
void Destroy(hash_map_t *map);
void Reserve(hash_map_t *map, int capacity);
bool InsertImpl(hash_map_t *map, const uint32_t key_size,
                const uint32_t value_size, const void *const key,
                const void *const value);
bool EraseImpl(hash_map_t *map, const uint32_t key_size, const void *const key);
void *LookupImpl(hash_map_t *map, const uint32_t key_size,
                 const void *const key);
uint32_t Size(hash_map_t *map);
void *GetKeyImpl(hash_map_t *map, uint32_t index);
void *GetValueImpl(hash_map_t *map, uint32_t index);

template <typename K, typename V>
hash_map_t *Create(uint32_t (*hasher)(const void *const),
                   bool (*comp)(const void *const, const void *const)) {
  return CreateImpl(sizeof(K), sizeof(V), hasher, comp);
}
//#define Create(key_type, value_type, hasher, comp)                             \
  //CreateImpl(sizeof(key_type), sizeof(value_type), hasher, comp)

bool Insert(hash_map_t *map, const void *const key, const void *const values) {
  return InsertImpl(map, sizeof(*key), sizeof(*value), key, value);
}
//#define Insert(map, key, value)                                                \
  //InsertImpl(map, sizeof(*key), sizeof(*value), key, value)

#define Erase(map, key) EraseImpl(map, sizeof(*key), key)
#define Lookup(type, map, key) ((type *)LookupImpl(map, sizeof(*key), key))
#define GetKey(type, map, index) ((type *)GetKeyImpl(map, index))
#define GetValue(type, map, index) ((type *)GetValueImpl(map, index))

// Murmur3 fmix32
static inline uint32_t HashInt32(const void *const val) {
  uint32_t h = *(uint32_t *)val;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}

// Murmur3 fmix64
static inline uint32_t HashInt64(const void *const val) {
  uint64_t k = *(uint64_t *)val;
  k ^= k >> 33;
  k *= 0xff51afd7ed558ccdull;
  k ^= k >> 33;
  k *= 0xc4ceb9fe1a85ec53ull;
  k ^= k >> 33;
  // Truncates, but all bits should be equally good
  return k;
}

static inline uint32_t HashFloat(const void *const val) {
  uint32_t float_bits;
  memcpy(&float_bits, val, sizeof(uint32_t));
  if (float_bits == 0x80000000)
    float_bits = 0;
  return HashInt32(&float_bits);
}

static inline uint32_t HashPtr(const void *const val) {
  if (sizeof(void *) == sizeof(uint64_t))
    return HashInt64(val);
  return HashInt32(val);
}

// Murmur3 hash combine
static inline uint32_t HashCombine(uint32_t a, uint32_t b) {
  a *= 0xcc9e2d51;
  a = (a >> 17) | (a << 15);
  a *= 0x1b873593;
  b ^= a;
  b = (b >> 19) | (b << 13);
  return (b * 5) + 0xe6546b64;
}

static inline uint32_t HashVec2(const void *const val) {
  vec2_t *vec = (vec2_t *)val;
  return HashCombine(HashFloat(&(*vec)[0]), HashFloat(&(*vec)[1]));
}

static inline uint32_t HashVec3(const void *const val) {
  vec3_t *vec = (vec3_t *)val;
  return HashCombine(HashFloat(&(*vec)[0]),
                     HashCombine(HashFloat(&(*vec)[1]), HashFloat(&(*vec)[2])));
}

// FNV-1a hash
static inline uint32_t HashStr(const void *const val) {
  const unsigned char *str = *(const unsigned char **)val;
  static const uint32_t FNV_32_PRIME = 0x01000193;

  uint32_t hval = 0;
  while (*str) {
    hval ^= (uint32_t)*str;
    hval *= FNV_32_PRIME;
    ++str;
  }

  return hval;
}

static inline bool HashStrCmp(const void *const a, const void *const b) {
  const char *str_a = *(const char **)a;
  const char *str_b = *(const char **)b;
  return strcmp(str_a, str_b) == 0;
}

#ifdef _DEBUG
void Testf(void);
#endif

} // namespace hashmap
