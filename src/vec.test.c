#include <assert.h>
#include <stdlib.h>

#include "vec.h"
#include "allocator.h"
#include "logger.h"

void init_allocator() {
  size_t allocator_size = 4lu * 1024lu * 1024lu; // 4 MiB
  if (!allocator_init(allocator_size)) {
    abort();
  }
}


LogSeverity g_log_severity = LOG_ALL;

int main() {
  init_allocator();
  atexit(allocator_finalize);

  vec(int) v;
  vec_alloc(v);

  assert(NULL != v);
  assert(0 == vec_count(v));
  assert(0 != vec_capacity(v));
  assert(vec_is_empty(v));

  vec_push(v, 10);
  vec_push(v, 20);
  vec_push(v, 30);
  vec_push(v, 40);

  for (int at = 0; at < (int)vec_count(v); ++at) {
    int expected = (at + 1) * 10; 
    if (expected != *vec_at(v, at)) {
      printf("vec_at failed, expected = %d, acutal = %d\n", expected, *vec_at(v, at));
      assert(0);
    }
  }

  assert(4 == vec_count(v));
  assert(!vec_is_empty(v));
  assert(40 == *vec_back(v));

  vec_push(v, 50);
  vec_push(v, 60);
  vec_push(v, 70);
  vec_push(v, 80);
  vec_push(v, 90);

  #define print_vector_entry(index, val) printf("[%lu] %d\n", index, val)
  vec_for_each(v, print_vector_entry);

  assert(9 == vec_count(v));
  assert(90 == *vec_back(v));

  for (int at = 0; at < (int)vec_count(v); ++at) {
    int expected = (at + 1) * 10; 
    if (expected != *vec_at(v, at)) {
      printf("vec_at failed, expected = %d, acutal = %d\n", expected, *vec_at(v, at));
      assert(0);
    }
  }

  vec_pop(v);
  vec_pop(v);
  vec_pop(v);
  assert(6 == vec_count(v));
  assert(60 == *vec_back(v));

  for (int at = 0; at < (int)vec_count(v); ++at) {
    int expected = (at + 1) * 10; 
    if (expected != *vec_at(v, at)) {
      printf("vec_at failed, expected = %d, acutal = %d\n", expected, *vec_at(v, at));
      assert(0);
    }
  }

  while (0 != vec_count(v)) {
    vec_pop(v);
  }

  assert(vec_is_empty(v));

  vec_free(v);

  return 0;
}

