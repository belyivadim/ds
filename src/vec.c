#include "vec.h"

void vec_mb_expand(void **pp_vec, size_t el_size) {
  VecHeader *p_header = vec_get_header(*pp_vec);

  if (p_header->count == p_header->capacity) {
    size_t old_cap = p_header->capacity;
    p_header->capacity *= VEC_GROW_FACTOR;
    void *tmp = 
      a_reallocate(p_header, 
                   sizeof(VecHeader) + old_cap * el_size, 
                   sizeof(VecHeader) + p_header->capacity * el_size);
    if (NULL == tmp) logf_fatal("VEC", 137, "realocation for vector with new capacity %lu failed!", p_header->capacity); 
    *pp_vec = (void*)((char*)tmp + sizeof(VecHeader));
  }
}

