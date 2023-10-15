#include <stdio.h>
#include <stdlib.h>

#include "string_builder.h"
#include "allocator.h"

void init_allocator() {
  size_t allocator_size = 4lu * 1024lu * 1024lu; // 4 MiB
  if (!allocator_init(allocator_size)) {
    log_fatal("INITIALIZER", "Cannot initialize allocator.", 137);
  }
}

LogSeverity g_log_severity = LOG_ALL;

int main() {
  init_allocator();
  atexit(allocator_finalize);
  
  StringBuilder sb;
  string_builder_init(sb);


  string_builder_append_cstr(&sb, "Hello, World!");
  puts(string_builder_get_cstr(&sb));

  string_builder_append_string_view(&sb, &string_view_from_cstr_slice("I hope there is no errors...", 6, 20));

  puts(string_builder_get_cstr(&sb));

  string_builder_free(sb);

  return 0;
}
