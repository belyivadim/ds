#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "vsa.h"
#include "allocator.h"
#include "logger.h"

static void *g_memory;
static Vsa *gp_vsa;

#ifdef ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
static size_t g_number_of_allocs;
static size_t g_number_of_frees;
#endif // !ALLOCATOR_DUMP_MEMORY_ON_FINALIZE

bool allocator_init(size_t size) {
  if (NULL != g_memory) {
    log_warning("ALLOCATOR", "already initialized! Finalize before intialize again.");
    return false;
  }

  g_memory = calloc(sizeof(char), size);
  if (NULL == g_memory) {
    logf_error("ALLOCATOR", "cannot initialize with size of memory %lu\n", size);
    return false;
  }

  gp_vsa = vsa_init(g_memory, size);

  return true;
}

void allocator_finalize(void) {
#ifdef ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
  vsa_dump(gp_vsa, logf_trace, "VSA_ALLOCATOR");
  logf_trace("ALLOCATOR", "Number of allocations/frees: %lu / %lu\n", 
             g_number_of_allocs, g_number_of_frees);
#endif // !ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
  free(g_memory);
  g_memory = NULL;
}

void *a_allocate(size_t bytes) {
  assert(NULL != gp_vsa);
#ifdef ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
  ++g_number_of_allocs;
#endif // !ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
  return vsa_alloc(gp_vsa, bytes);
}

void *a_reallocate(void *ptr, size_t old_size, size_t new_size) {
  assert(NULL != gp_vsa);
  (void)old_size;
  void *new_ptr = vsa_realloc(gp_vsa, ptr, new_size);
#ifdef ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
  bool is_new_memory = new_ptr != ptr;
  g_number_of_allocs += is_new_memory;
  g_number_of_frees += is_new_memory;
#endif // !ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
  return new_ptr;
}

void *a_callocate(size_t nmemb, size_t memb_size) {
  assert(NULL != gp_vsa);
#ifdef ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
  ++g_number_of_allocs;
#endif // !ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
  return vsa_calloc(gp_vsa, nmemb, memb_size);
}

void a_free(void *ptr) {
  assert(NULL != gp_vsa);
  vsa_free(ptr);
#ifdef ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
  ++g_number_of_frees;
#endif // !ALLOCATOR_DUMP_MEMORY_ON_FINALIZE
}

