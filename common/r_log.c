#include <time.h>
#include <stdarg.h>

#include "r_log.h"

bool r_log_open(r_log_t *p_dst, log_verbose_routine p_verb_routine, RLOG_FILTER filter, const char *p_filename)
{
	p_dst->filter = filter;
	p_dst->p_routine = p_verb_routine;
	fopen_s(&p_dst->fp, p_filename, "a");
	if (!p_dst->fp)
		return false;

	r_log_printf(p_dst, RLOG_MESSAGE_TEXT,
		"\n\n\n--- logging (%d.%d.%d %d:%d:%d) started ---\n",
		0, 0, 0, 0, 0, 0
		);

	return true;
}

#define VERBOSE_CALL(plog, ptext, msgtype) \
	if (((plog)->filter & RLOG_FILTER_VERBOSE) && (plog)->p_routine) \
		(plog)->p_routine(ptext, msgtype);

inline const char *log_get_prefix_by_type(RLOG_MESSAGE_TYPE type)
{
	static const char *p_prefs[] = {
		""
	};
}

void r_log_puts(const r_log_t *p_src, RLOG_MESSAGE_TYPE msgtype, const char *p_str)
{
	char buffer[1024];
	VERBOSE_CALL(p_src, p_str, msgtype)


	fputs(p_str, p_src->fp);
}

void r_log_printf(const r_log_t *p_src, RLOG_MESSAGE_TYPE msgtype, const char *p_format, ...)
{
	char buffer[1024];
	va_list argptr;
	va_start(argptr, p_format);
	vsprintf_s(buffer, sizeof(buffer), p_format, argptr);
	va_end(argptr);

	VERBOSE_CALL(p_src, buffer, msgtype)
}

void r_log_close(r_log_t *p_src)
{
}
