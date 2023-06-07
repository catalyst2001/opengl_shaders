#pragma once
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


typedef enum RLOG_FILTER_ {
	RLOG_FILTER_NOTIFIES = (1 << 0),
	RLOG_FILTER_WARNINGS = (1 << 1),
	RLOG_FILTER_ERRORS = (1 << 2),
	RLOG_FILTER_CRITICALS = (1 << 3),
	RLOG_FILTER_VERBOSE = (1 << 4),
	RLOG_FILTER_FLUSH_TO_FILE = (1 << 5),
	RLOG_FILTER_ALL = (RLOG_FILTER_NOTIFIES | RLOG_FILTER_WARNINGS | RLOG_FILTER_ERRORS | RLOG_FILTER_CRITICALS | RLOG_FILTER_VERBOSE | RLOG_FILTER_FLUSH_TO_FILE)
} RLOG_FILTER;

typedef enum RLOG_MESSAGE_TYPE_ {
	RLOG_MESSAGE_TEXT = 0,
	RLOG_MESSAGE_NOTIFY,
	RLOG_MESSAGE_WARNING,
	RLOG_MESSAGE_ERROR,
	RLOG_MESSAGE_CRITICAL
} RLOG_MESSAGE_TYPE;

typedef void (*log_verbose_routine)(const char *p_message, RLOG_MESSAGE_TYPE type);

typedef struct r_log_s {
	FILE *fp;
	RLOG_FILTER filter;
	log_verbose_routine p_routine;
} r_log_t;

bool r_log_open(r_log_t *p_dst, log_verbose_routine p_verb_routine, RLOG_FILTER filter, const char *p_filename);
void r_log_puts(const r_log_t *p_src, RLOG_MESSAGE_TYPE msgtype, const char *p_str);
void r_log_printf(const r_log_t *p_src, RLOG_MESSAGE_TYPE msgtype, const char *p_format, ...);
void r_log_close(r_log_t *p_src);

/* COMMON MACROSES */
#define RLOG_PUTS_TEXT(plog, str) r_log_puts(plog, RLOG_MESSAGE_TEXT, str "\n")
#define RLOG_PUTS_NOTIFY(plog, str) r_log_puts(plog, RLOG_MESSAGE_NOTIFY, str "\n")
#define RLOG_PUTS_WARNING(plog, str) r_log_puts(plog, RLOG_MESSAGE_WARNING, str "\n")
#define RLOG_PUTS_ERROR(plog, str) r_log_puts(plog, RLOG_MESSAGE_ERROR, str "\n")
#define RLOG_PUTS_CRITICAL(plog, str) r_log_puts(plog, RLOG_MESSAGE_CRITICAL, str "\n")

#define RLOG_PRINTF_TEXT(plog, format, ...) r_log_printf(plog, RLOG_MESSAGE_TEXT, format "\n", __VA_ARGS__)
#define RLOG_PRINTF_NOTIFY(plog, format, ...) r_log_printf(plog, RLOG_MESSAGE_NOTIFY, format "\n", __VA_ARGS__)
#define RLOG_PRINTF_WARNING(plog, format, ...) r_log_printf(plog, RLOG_MESSAGE_WARNING, format "\n", __VA_ARGS__)
#define RLOG_PRINTF_ERROR(plog, format, ...) r_log_printf(plog, RLOG_MESSAGE_ERROR, format "\n", __VA_ARGS__)
#define RLOG_PRINTF_CRITICAL(plog, format, ...) r_log_printf(plog, RLOG_MESSAGE_CRITICAL, format "\n", __VA_ARGS__)