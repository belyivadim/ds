#ifndef __LOGGER_H__
#define __LOGGER_H__


#include <stdio.h>

#ifndef LOG_OUT
#define LOG_OUT stdout
#endif // !LOG_OUT

#ifndef LOG_ERR
#define LOG_ERR stderr
#endif // !LOG_ERR

typedef enum {
  LOG_ALL,
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
  LOG_FATAL,
  LOG_NONE
} LogSeverity;

extern LogSeverity g_log_severity;

void log_trace(const char *caller_name, const char *msg);
void log_info(const char *caller_name, const char *msg);
void log_warning(const char *caller_name, const char *msg);
void log_error(const char *caller_name, const char *msg);
void log_fatal(const char *caller_name, const char *msg, int exit_code);

void logf_trace(const char *caller_name, const char *fmt, ...);
void logf_info(const char *caller_name, const char *fmt, ...);
void logf_warning(const char *caller_name, const char *fmt, ...);
void logf_error(const char *caller_name, const char *fmt, ...);
void logf_fatal(const char *caller_name, int exit_code, const char *fmt, ...);


#endif // !__LOGGER_H__
