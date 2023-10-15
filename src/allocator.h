#ifndef __ALOCATOR_H__
#define __ALOCATOR_H__

#include <stddef.h>
#include <stdbool.h>

bool allocator_init(size_t size);
void allocator_finalize(void);
void *a_allocate(size_t bytes);
void *a_reallocate(void *ptr, size_t old_size, size_t new_size);
void *a_callocate(size_t nmemb, size_t memb_size);
void a_free(void *ptr);

#endif // !__ALOCATOR_H__
