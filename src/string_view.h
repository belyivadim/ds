#ifndef __STRING_VIEW_H__
#define __STRING_VIEW_H__

#include "stddef.h"
#include "string.h"
#include "stdbool.h"

typedef struct {
  const char *p_begin;
  size_t length;
} StringView;

#define string_view_empty ((StringView){0})
#define string_view_from_cstr(p_str) ((StringView){.p_begin = p_str, .length = strlen(p_str)})
#define string_view_from_cstr_slice(p_str, offset, len) ((StringView){.p_begin = (p_str) + (offset), .length = len})
#define string_view_from_constant(c) ((StringView){.p_begin = (c), .length = sizeof((c))})

#define string_view_farg "%.*s"
#define string_view_expand(sv) (int)(sv).length, (sv).p_begin

#define string_view_slice(sv, offset, len) (string_view_from_cstr_slice((sv).p_begin, (offset), (len)))

bool string_view_equals(const StringView *p_lhs, const StringView *p_rhs);
bool string_view_starts_with_cstr(const StringView *p_sv, const char *p_start);
bool string_view_ends_with_cstr(const StringView *p_sv, const char *p_end);

size_t string_view_index_of(const StringView *p_sv, int rune);
size_t string_view_last_index_of(const StringView *p_sv, int rune);


#endif // !__STRING_VIEW_H__
