#include "string_builder.h"


void string_builder_append_rune(StringBuilder *p_sb, int rune) {
  assert(rune >= -128 && rune <= 127);
  *vec_back(p_sb->data) = (char)rune;
  vec_push(p_sb->data, '\0');
}

void string_builder_append_string_view(StringBuilder *p_sb, const StringView *p_sv) {
  for (size_t i = 0; i < p_sv->length; ++i) {
    string_builder_append_rune(p_sb, p_sv->p_begin[i]);
  }
}

void string_builder_append_cstr(StringBuilder *p_sb, const char *cstr) {
  string_builder_append_string_view(p_sb, &string_view_from_cstr(cstr));
}

char *string_builder_get_cstr(const StringBuilder *p_sb) {
  return p_sb->data;
}

