#ifndef __vsa_H__
#define __vsa_H__

#include <stddef.h>


/// Variable size allocator
typedef void Vsa;


/// Initializes memory for Vsa, alignes @mem and @mem_size by word.
Vsa *vsa_init(void *mem, size_t mem_size);

/// Allocates @bytes in the Vsa, required number of bytes will be word alligned.
void *vsa_alloc(Vsa *p_vsa, size_t bytes);


/// Reallocates bytes in the Vsa, required number of bytes will be word alligned.
/// It will return pointer to the new location and copy all the data from @ptr
///   to that location.
/// If @bytes is 0, call has the same effect as call to vsa_free(ptr).
/// If @ptr is NULL call has the same effect as call to vsa_alloc(bytes).
void *vsa_realloc(Vsa *p_vsa, void *ptr, size_t bytes);


/// Allocates bytes in the Vsa, required number of bytes will be word alligned.
/// Zero initializes allocated memory.
void *vsa_calloc(Vsa *p_vsa, size_t nmemb, size_t memb_size);

/// Frees memory pointed by @ptr
void vsa_free(void *ptr);

typedef void (*DumpPrinter)(const char *caller_name, const char *fmt, ...);

/// Dumps state of the Vsa to the standard out
void vsa_dump(const Vsa *p_vsa, DumpPrinter printer, const char *vsa_name);

#endif // !__vsa_H__
