#ifndef __TABLE_H__
#define __TABLE_H__

#include <stddef.h>
#include <sys/types.h>
#include <stdbool.h>

/// Represents an entry in a Table
typedef struct {
  /// Key for finding entry in a Table
  const void* key;

  /// Value associated to the key
  void *value;
} Entry;

typedef size_t (*HashFunc)(const void* key);
typedef bool (*KeyCmpFunc)(const void* lhs, const void *rhs);
typedef void (*FreeKeyValFunc)(const void *key, void *value);

/// Represents a hash table based on open addresing approach
typedef struct {
  HashFunc hash_func;
  KeyCmpFunc key_cmp_func;
  FreeKeyValFunc free_kv_func;

  /// Number of active elements in entries array
  size_t count;

  /// Capacity of entries array
  /// Note: actual capacity is capacity + 1
  ///   for table look-up optimisation
  ssize_t capacity;

  /// Number of tombstones elements in entries array
  size_t tombstones_count;

  /// Array of entries in hashtable
  Entry *entries;
} Table;


/// Initializes the table to all zeros
///
/// @param table: pointer to the table to be initialized
/// @param hash_func: pointer to the hash function for keys
/// @param key_cmp_func: pointer to the function that compares keys for equality
/// @param free_kv_func: pointer to the callback function for freeing key and value pairs
///   when deleting key or on table_free, it will not be called if NULL is passed
/// @return void
void table_init(Table *table, 
                HashFunc hash_func, KeyCmpFunc key_cmp_func,
                FreeKeyValFunc free_kv_func);

/// Frees underlying array of entries in the table 
/// and initializes all fields in the table to zeros.
///
/// @param table: pointer to the table to be freed
/// @return void
void table_free(Table *table);

/// Inserts new value to the table by key,
/// if key already exists, then overrides old value associated to that key
///
/// @param table: pointer to table to insert value in
/// @param key: pointer to the key to associate value with
/// @param value: pointer to the value to insert
///
/// @return bool, true if key is new, otherwise false
bool table_set(Table* table, const void *key, void *value);

/// Looks up for a value in the tabel associated with the key
///
/// @param table: pointer to the table to look up in
/// @param key: pointer to the key value associated with
/// @outparam value: pointer to the found value (untouched if not found)
/// @return bool, true if value was found, false otherwise
bool table_get(const Table* table, const void *key, void **value);

/// Deletes entry associated with the key in the table
///
/// @param table: pointer to the table to delete from
/// @param key: pointer to the key value associated with
/// @return bool, true if entry was found and deleted, false otherwise
bool table_delete(Table *table, const void *key);

/// Inserts all entries from src to dest
///
/// @param dest: destination table
/// @param src: source table
/// @return void
void table_add_all(Table* dest, const Table *src);



size_t hash_cstr_default(const void *value);
size_t hash_string_view_default(const void *value);

/// Default comparison function
/// Simply compares pointers: lhs == rhs
/// @return bool
bool key_cmp_default(const void *lhs, const void *rhs);

#endif // !__TABLE_H__
