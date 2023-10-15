#ifndef __STRING_BUILDER_H__
#define __STRING_BUILDER_H__

#include "string_view.h"
#include "vec.h"

typedef struct {
  vec(char) data;
} StringBuilder;

#define string_builder_init(sb) do { vec_alloc(sb.data); vec_push(sb.data, '\0'); } while (0)
#define string_builder_init_with_capacity(sb, cap) vec_alloc_reserved(char, sb.data, (cap))
#define string_builder_free(sb) vec_free((sb).data)

#define string_builder_get_length(sb) vec_count((sb).data)


void string_builder_append_rune(StringBuilder *p_sb, int rune);
void string_builder_append_string_view(StringBuilder *p_sb, const StringView *p_sv);
void string_builder_append_cstr(StringBuilder *p_sb, const char *cstr);
char *string_builder_get_cstr(const StringBuilder *p_sb);


#endif // !__STRING_BUILDER_H__
