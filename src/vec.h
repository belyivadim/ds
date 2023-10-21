#ifndef __VEC_H__
#define __VEC_H__

#include <stdlib.h> // abort
#include <string.h>
#include <assert.h>

#include "allocator.h"
#include "logger.h"

typedef struct {
  size_t count;
  size_t capacity;
} VecHeader;

#define VEC_GROW_FACTOR 2
#define VEC_INITIAL_CAPACITY 8

#define vec(T) T*

#define vec_get_header(v) (((VecHeader*)(v)) - 1)

#define vec_alloc(v) vec_alloc_reserved((v), VEC_INITIAL_CAPACITY)

#define vec_alloc_reserved(v, cap) \
  do {\
    size_t capacity = (cap);\
    assert(capacity > 0 && "Capacity should be greater than 0.");\
    (v) = a_allocate(sizeof(VecHeader) + capacity * sizeof(*(v))); \
    VecHeader *header = (VecHeader*)(v);\
    header->count = 0;\
    header->capacity = capacity;\
    (v) = (void*)((char*)v + sizeof(VecHeader));\
  } while (0)

#define vec_free(v) a_free(vec_get_header((v)))


#define vec_count(v) (vec_get_header((v))->count)
#define vec_capacity(v) (vec_get_header((v))->capacity)
#define vec_is_empty(v) (0 == vec_count((v)))

#define vec_maybe_expand(v) \
  do {\
    if (vec_count(v) == vec_capacity(v)) {\
      size_t old_cap = vec_capacity(v);\
      vec_capacity(v) *= VEC_GROW_FACTOR;\
      void *tmp = a_reallocate(vec_get_header(v), old_cap * sizeof(*(v)), sizeof(VecHeader) + vec_capacity(v) * sizeof(*(v)));\
      if (NULL == tmp) log_fatal("VEC", "realocation for vector with new capacity %lu failed!", 137, vec_capacity(v)); \
      v = (void*)((char*)tmp + sizeof(VecHeader));\
    }\
  } while(0)

void vec_mb_expand(void **pp_vec, size_t el_size);

#define vec_push(v, e) \
  do {\
    vec_mb_expand((void**)&(v), sizeof(*(v)));\
    (v)[vec_count((v))++] = (e);\
  } while (0)

#define vec_pop(v) (--vec_count((v)))

#define vec_back(v) (assert(0 != vec_count((v))), v + vec_count((v)) - 1)

#define vec_at(v, i) ((v) + (i))

#define vec_for_each(v, iter, body)\
  do {\
    size_t count = vec_count(v);\
    for (size_t iter = 0; iter < count; ++iter) body;\
  } while (0)

#define vec_append(v1, v2)\
  do {\
    size_t count = vec_count((v2));\
    for (size_t i = 0; i < count; ++i) vec_push((v1), (v2)[count]);\
  } while (0)

#endif // !__VEC_H__
