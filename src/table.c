#include <assert.h>
#include <string.h>

#include "table.h"
#include "allocator.h"
#include "string_view.h"

#define TABLE_MAX_LOAD .75
#define TOMBSTONE_VAL (void*)0x1

#define ARR_MIN_CAPACITY 8
#define ARR_GROW_FACTOR 2

/// returns capacity multiplied by MEM_GROW_FACTOR
/// or MEM_MIN_CAPACITY in case if the initial capacity less than MEM_MIN_CAPACITY
#define GROW_CAPACITY(capacity) \
  ((capacity) < ARR_MIN_CAPACITY ? \
    ARR_MIN_CAPACITY : \
    (capacity) * ARR_GROW_FACTOR)

/// realocates array pointed by ptr with new_size
#define GROW_ARRAY(T, ptr, old_count, new_count) \
  (T*)a_reallocate(ptr, sizeof(T) * (old_count), sizeof(T) * (new_count))


/// Looking up for an entry with key in entries array
///
/// @param entries: pointer to the array to look in
/// @param capacity: capacity of the entries array
/// @param key: pointer to the key to look up entry by
/// @return Entry*, pointer to the found Entry
///   if key in found entry is NULL, then entry was not found,
///   and the nearest empty bucket for the key is returned
static Entry *find_entry(Entry* entries, size_t capacity, const void *key,
                         HashFunc hash_func, KeyCmpFunc key_cmp_func);

/// Adjustes capacity of table's underlying array of entries to the desirable capacity
/// by reallocating it,
/// inserts all entries from old array to the new one,
/// frees old array
///
/// @param table: table to adjust
/// @param capacity: desirable capacity
static void adjust_capacity(Table *table, size_t capacity);

static void table_init_impl(Table *table, HashFunc hash_func, KeyCmpFunc key_cmp_func,
                            FreeKeyValFunc free_kv_func) {
  table->hash_func = hash_func;
  table->key_cmp_func = key_cmp_func;
  table->free_kv_func = free_kv_func;
  table->count = 0;
  table->capacity = -1;
  table->tombstones_count = 0;
  table->entries = NULL;
}

void table_init(Table *table, HashFunc hash_func, KeyCmpFunc key_cmp_func,
                FreeKeyValFunc free_kv_func) {
  assert(NULL != table);
  assert(NULL != hash_func);
  assert(NULL != key_cmp_func);
  table_init_impl(table, hash_func, key_cmp_func, free_kv_func);
}

void table_free(Table *table) {
  assert(NULL != table);
  if (NULL != table->free_kv_func) {
    for (ssize_t i = 0; i <= table->capacity; ++i) {
      Entry *pentry = table->entries + i;
      if (NULL != pentry->key) {
        table->free_kv_func(pentry->key, pentry->value);
      }
    }
  }
  a_free(table->entries);
  table_init_impl(table, NULL, NULL, NULL);
}

bool table_set(Table* table, const void *key, void *value) {
  assert(NULL != table);

  if ((table->capacity + 1) * TABLE_MAX_LOAD <= table->count + table->tombstones_count) {
    size_t capacity = GROW_CAPACITY(table->capacity + 1) - 1;
    adjust_capacity(table, capacity);
  }

  Entry *pentry = find_entry(table->entries, table->capacity, key, 
                             table->hash_func, table->key_cmp_func);

  bool is_new_key = NULL == pentry->key;
  table->count += is_new_key;

  pentry->key = key;
  pentry->value = value;

  return is_new_key;
}

bool table_get(const Table* table, const void *key, void **value) {
  assert(NULL != table);

  if (0 == table->count) return false;

  Entry *pentry = find_entry(table->entries, table->capacity, key, 
                             table->hash_func, table->key_cmp_func);
  if (NULL == pentry->key) return false;

  *value = pentry->value;
  return true;
}

bool table_delete(Table *table, const void *key) {
  assert(NULL != table);

  if (0 == table->count) return false;

  Entry *pentry = find_entry(table->entries, table->capacity, key, 
                             table->hash_func, table->key_cmp_func);
  if (NULL == pentry->key) return false;

  if (NULL != table->free_kv_func) {
    table->free_kv_func(pentry->key, pentry->value);
  }

  // tombstone
  pentry->key = NULL;
  pentry->value = TOMBSTONE_VAL;
  --table->count;
  ++table->tombstones_count;

  return true;
}

void table_add_all(Table* dest, const Table *src) {
  assert(NULL != dest);
  assert(NULL != src);

  for (ssize_t i = 0; i <= src->capacity; ++i) {
    const Entry *pentry = src->entries + i;
    if (NULL != pentry->key) {
      table_set(dest, pentry->key, pentry->value);
    }
  }
}

static Entry *find_entry(Entry* entries, size_t capacity, const void *key, 
                         HashFunc hash_func, KeyCmpFunc key_cmp_func) {
  assert(NULL != entries);
  assert(NULL != key);
  assert(NULL != hash_func);
  assert(NULL != key_cmp_func);

  size_t hash = hash_func(key);
  size_t index = hash & capacity;
  Entry *tombstone = NULL;

  for (;;) {
    Entry *pentry = entries + index;

    if (NULL == pentry->key) {
      if (NULL == pentry->value) {
        // empty entry
        return NULL != tombstone ? tombstone : pentry;
      } else {
        // tombstone
        if (NULL == tombstone) tombstone = pentry;
      }
    } else if (key_cmp_func(key, pentry->key)) {
      // the key is found
      return pentry;
    }

    index = (index + 1) & capacity;
  }
}

static void adjust_capacity(Table *table, size_t capacity) {
  assert(NULL != table);

  Entry *entries = a_allocate(sizeof(Entry) * (capacity + 1));

  for (size_t i = 0; i <= capacity; ++i) {
    entries[i].key = NULL;
    entries[i].value = NULL;
  }
  
  for (ssize_t i = 0; i <= table->capacity; ++i) {
    Entry *pentry = table->entries + i;
    if (NULL == pentry->key) continue;

    Entry *dest = find_entry(entries, capacity, pentry->key, 
                             table->hash_func, table->key_cmp_func);
    dest->key = pentry->key;
    dest->value = pentry->value;
  }

  a_free(table->entries);
  table->entries = entries;
  table->capacity = capacity;
  table->tombstones_count = 0;
}




size_t hash_cstr_with_len(const char *str, size_t len) {
  assert(NULL != str);

  size_t hash = 2166136261lu;

  for (size_t i = 0; i < len; ++i) {
    hash ^= str[i];
    hash *= 16777619;
  }

  return hash;
}

size_t hash_cstr_default(const void *value) {
  assert(NULL != value);
  const char *str = (const char*)value;
  return hash_cstr_with_len(str, strlen(str));
}

size_t hash_string_view_default(const void *value) {
  assert(NULL != value);
  const StringView *sv = (const StringView*)value;
  return hash_cstr_with_len(sv->p_begin, sv->length);
}



bool key_cmp_default(const void *lhs, const void *rhs) {
  return lhs == rhs;
}
