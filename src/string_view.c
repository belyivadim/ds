#include "string_view.h"
#include <assert.h>


bool string_view_equals(const StringView *p_lhs, const StringView *p_rhs) {
  assert(NULL != p_lhs);
  assert(NULL != p_rhs);

  return p_lhs->length == p_rhs->length
    && 0 == strncmp(p_lhs->p_begin, p_rhs->p_begin, p_lhs->length);
}

bool string_view_starts_with_cstr(const StringView *p_sv, const char *p_start) {
  assert(NULL != p_sv);

  size_t start_len = strlen(p_start);
  return p_sv->length >= start_len
    && 0 == strncmp(p_sv->p_begin, p_start, start_len);
}

bool string_view_ends_with_cstr(const StringView *p_sv, const char *p_end) {
  assert(NULL != p_sv);

  size_t end_len = strlen(p_end);
  return p_sv->length >= end_len
    && 0 == strncmp(p_sv->p_begin + p_sv->length - end_len, p_end, end_len);
}

size_t string_view_index_of(const StringView *p_sv, int rune) {
  assert(NULL != p_sv);

  char *found = strchr(p_sv->p_begin, rune);
  return found - p_sv->p_begin;
}

size_t string_view_last_index_of(const StringView *p_sv, int rune) {
  assert(NULL != p_sv);

  char *found = strrchr(p_sv->p_begin, rune);
  return found - p_sv->p_begin;
}
