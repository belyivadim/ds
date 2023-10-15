#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <stdio.h>
#include <sys/types.h>

#include "vsa.h"

typedef struct {
  size_t size;
} VsaHeader;

#define WORD_SIZE (sizeof(size_t))
#define WORD_BIT_SIZE (WORD_SIZE * CHAR_BIT)

#define BLOCK_IS_FREE(header) (!((header)->size >> (WORD_BIT_SIZE - 1)))
#define BLOCK_GET_HEADER(block) (((VsaHeader*)(block)) - 1)

#define HEADER_GET_SIZE(header) (((header)->size & ~(1LU << (WORD_BIT_SIZE - 1))))
#define HEADER_SET_FREE(header) (((header)->size &= ~(1LU << (WORD_BIT_SIZE - 1))))
#define HEADER_SET_TAKEN(header) (((header)->size |= (1LU << (WORD_BIT_SIZE - 1))))
#define HEADER_GET_BLOCK(header) ((void*)((header) + 1))


static size_t size_align(size_t size) {
  if (0 == size) return WORD_SIZE;
  
  int mod = size % WORD_SIZE;
  int diff = WORD_SIZE - mod;
  return 0 == mod ? size : size + diff;
}

static void *memory_align(void *mem) {
  int mod = (size_t)mem % WORD_SIZE;
  int diff = WORD_SIZE - mod;
  return 0 == mod ? mem : (char*)mem + diff;
}

static VsaHeader *header_get_next(const VsaHeader *p_header) {
  return (VsaHeader*)((char*)p_header + HEADER_GET_SIZE(p_header) + sizeof(VsaHeader));
}

static void header_try_defrag(VsaHeader *p_header) {
  assert(NULL != p_header);

  if (!BLOCK_IS_FREE(p_header) || 0 == p_header->size) return;

  VsaHeader *p_next = header_get_next(p_header);
  while (BLOCK_IS_FREE(p_next) && p_next->size) {
    p_header->size += p_next->size + sizeof(VsaHeader);
    p_next = header_get_next(p_next);
  }
}


Vsa *vsa_init(void *mem, size_t mem_size) {
  assert(NULL != mem);
  assert(mem_size > sizeof(VsaHeader) * 2);

  void *aligned_mem = memory_align(mem);

  // align size 
  {
    mem_size -= ((char*)aligned_mem - (char*)mem);

    int mod = mem_size % WORD_SIZE;
    int diff = WORD_SIZE - mod;
    mem_size = 0 == mod ? mem_size : mem_size - diff;
  }

  mem = aligned_mem;

  VsaHeader *p_header = (VsaHeader*)mem;
  p_header->size = mem_size - sizeof(VsaHeader) * 2;

  p_header = header_get_next(p_header);
  p_header->size = 0;

  return mem;
}


void *vsa_alloc(Vsa *p_vsa, size_t bytes) {
  assert(NULL != p_vsa);

  bytes = size_align(bytes);

  VsaHeader *p_header = (VsaHeader*)p_vsa;

  header_try_defrag(p_header);
  while(HEADER_GET_SIZE(p_header) < bytes || !BLOCK_IS_FREE(p_header)) {
    if (0 == HEADER_GET_SIZE(p_header)) {
      return NULL;
    }

    p_header = header_get_next(p_header);
    header_try_defrag(p_header);
  }

  ssize_t next_size = p_header->size - bytes - sizeof(VsaHeader);
  if (next_size >= (ssize_t)WORD_SIZE) {
    VsaHeader *p_next = NULL;
    p_header->size = bytes;
    p_next = header_get_next(p_header);
    p_next->size = next_size;

  }

  HEADER_SET_TAKEN(p_header);
  return HEADER_GET_BLOCK(p_header);
}



void *vsa_calloc(Vsa *p_vsa, size_t nmemb, size_t memb_size) {
  assert(NULL != p_vsa);

  size_t bytes = size_align(nmemb * memb_size);
  char *allocated = (char*)vsa_alloc(p_vsa, bytes);

  if (NULL == allocated) return NULL;

  memset(allocated, 0, bytes);

  return allocated;
}

void *vsa_realloc(Vsa *p_vsa, void *ptr, size_t bytes) {
  assert(NULL != p_vsa);

  if (0 == bytes) {
    vsa_free(ptr);
    return ptr;
  }

  size_t ptr_size = HEADER_GET_SIZE(BLOCK_GET_HEADER(ptr));

  // TODO: maybe free ptr here,
  // because it is impossible to do another allocation
  // and data will be preserved, but block will be marked as free,
  // so new allocation may return the same defragmented block
  // if there is enough memory
  void *allocated = vsa_alloc(p_vsa, bytes);
  if (NULL == allocated) {
    return NULL;
  }

  if (NULL != ptr) {
    memcpy(allocated, ptr, ptr_size);
    vsa_free(ptr);
  }

  return allocated;
}


void vsa_free(void *ptr) {
  if (NULL == ptr) return;
  assert(!BLOCK_IS_FREE(BLOCK_GET_HEADER(ptr)));
  HEADER_SET_FREE(BLOCK_GET_HEADER(ptr));
}


void vsa_dump(const Vsa *p_vsa, DumpPrinter printer, const char *vsa_name) {
  assert(NULL != p_vsa);
  assert(NULL != printer);

  const VsaHeader *p_header = (const VsaHeader *)p_vsa;
  printer("VSA", "%s DUMP\n", vsa_name);

  while (0 != HEADER_GET_SIZE(p_header)) {
    printer("VSA", "block (%s)\t[%p], size: %lu\n", 
             BLOCK_IS_FREE(p_header) ? "free" : "taken", 
             HEADER_GET_BLOCK(p_header), 
             HEADER_GET_SIZE(p_header));

    p_header = header_get_next(p_header);
  }
}


