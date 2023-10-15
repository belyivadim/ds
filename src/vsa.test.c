#include <stdlib.h>
#include <assert.h>

#include "vsa.h"
#include "logger.h"


void test(char *mem, size_t mem_size, const char *name) {
  logf_info("\n======== | TEST2 | ========", "%s\n", name);


  for (size_t i = 0; i < mem_size; ++i) {
    mem[i] = i + 1;
  }

  Vsa *test_vsa = vsa_init(mem, mem_size);

  vsa_dump(test_vsa, &logf_info, "test_vsa after init");

  int *arr = (int*)vsa_calloc(test_vsa, 5, sizeof(int));
  if (NULL != arr) {
    assert(0 == *arr);
  }

  vsa_dump(test_vsa, &logf_info, "test_vsa after allocation array of 5 ints");
  arr = vsa_realloc(test_vsa, arr, 6 * sizeof(int));
  vsa_dump(test_vsa, &logf_info, "test_vsa after realloc array from 5 to 6 ints");

  arr = vsa_realloc(test_vsa, arr, 7 * sizeof(int));
  vsa_dump(test_vsa, &logf_info, "test_vsa after realloc array from 6 to 7 ints");

  vsa_realloc(test_vsa, arr, 0);
  vsa_dump(test_vsa, &logf_info, "test arena after free array");

  void *ptrs[10];
  for (int i = 0; i < 10; ++i) {
    ptrs[i] = vsa_alloc(test_vsa, i);
  }
  for (int i = 0; i < 10; ++i) {
    vsa_free(ptrs[i]);
  }

  vsa_dump(test_vsa, &logf_info, "test arena after 10 alloc + free");

  vsa_free(vsa_alloc(test_vsa, 1));
  vsa_dump(test_vsa, &logf_info, "test arena alloc + free 1, should defragment all blocks");

  for (int i = 0; i < 5; ++i) {
    ptrs[i] = vsa_alloc(test_vsa, i);
  }
  for (int i = 1; i < 5; ++i) {
    vsa_free(ptrs[i]);
  }

  vsa_dump(test_vsa, &logf_info, "test arena after 5 alloc + 4 free (from 1)");

  void *ptr = vsa_realloc(test_vsa, ptrs[0], 32);
  vsa_dump(test_vsa, &logf_info, "test arena realloc ptrs[0] to 32");

  ptr = vsa_realloc(test_vsa, ptr, 7);
  vsa_dump(test_vsa, &logf_info, "test arena realloc ptrs[0] to 7");
  vsa_free(ptr);
}

void test2(char *mem, size_t mem_size, const char *name) {
  logf_info("\n======== | TEST2 | ========", "(%s)\n", name);

  Vsa *test_vsa = vsa_init(mem, mem_size);
  vsa_dump(test_vsa, &logf_info, "test_vsa after init");

  void *ptr = vsa_alloc(test_vsa, 8);
  vsa_dump(test_vsa, &logf_info, "test_vsa after alloc 8");

  ptr = vsa_realloc(test_vsa, ptr, 12);
  vsa_dump(test_vsa, &logf_info, "test_vsa after realloc ptr to 12");

  ptr = vsa_realloc(test_vsa, ptr, 2);
  vsa_dump(test_vsa, &logf_info, "test_vsa after realloc ptr to 2");

  vsa_free(ptr);

  ptr = vsa_alloc(test_vsa, 8);
  vsa_dump(test_vsa, &logf_info, "test_vsa after free and then alloc 8");
}

#define MEM_SIZE 1020
#define MEM_SIZE_SMALL 40

LogSeverity g_log_severity = LOG_ALL;

int main() {
  static char mem[MEM_SIZE];
  char *mem_heap = (char*)malloc(MEM_SIZE);

  test(mem + 1, MEM_SIZE - 1, "with static memory");
  test(mem_heap, MEM_SIZE, "with heap memory");

  test2(mem, MEM_SIZE_SMALL, "with static memory small size");
  test2(mem_heap, MEM_SIZE_SMALL, "with heap memory small size");

  free(mem_heap);
  return 0;
}
