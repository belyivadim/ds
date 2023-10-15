#include <stdarg.h>
#include <stdlib.h> // exit

#include "logger.h"

#define DO_LOG(severity)\
  do {\
    va_list args;\
    va_start(args, fmt);\
    fprintf(LOG_OUT, "[%s:" severity "]: ", caller_name);\
    vfprintf(LOG_OUT, fmt, args);\
  } while (0)

void logf_trace(const char *caller_name, const char *fmt, ...) {
  if (g_log_severity > LOG_ALL) return;
  DO_LOG("TRACE");
}

void logf_info(const char *caller_name, const char *fmt, ...) {
  if (g_log_severity > LOG_INFO) return;
  DO_LOG("INFO");
}

void logf_warning(const char *caller_name, const char *fmt, ...) {
  if (g_log_severity > LOG_WARNING) return;
  DO_LOG("WARN");
}

void logf_error(const char *caller_name, const char *fmt, ...) {
  if (g_log_severity > LOG_ERROR) return;
  DO_LOG("ERROR");
}

void logf_fatal(const char *caller_name, int exit_code, const char *fmt, ...) {
  if (g_log_severity > LOG_FATAL) return;
  DO_LOG("FATAL");
  exit(exit_code);
}


void log_trace(const char *caller_name, const char *msg) {
  logf_trace(caller_name, "%s\n", msg);
}

void log_info(const char *caller_name, const char *msg) {
  logf_info(caller_name, "%s\n", msg);
}

void log_warning(const char *caller_name, const char *msg) {
  logf_warning(caller_name, "%s\n", msg);
}

void log_error(const char *caller_name, const char *msg) {
  logf_error(caller_name, "%s\n", msg);
}

void log_fatal(const char *caller_name, const char *msg, int exit_code) {
  logf_fatal(caller_name, exit_code, "%s\n", msg);
}


