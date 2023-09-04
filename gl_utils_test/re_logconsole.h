#pragma once
#include "ire_logconsole.h"

#if defined LINUX

#else
#include <Windows.h>

#endif

class re_logconsole : public ire_logconsole
{
#if defined LINUX

#else
	HWND h_wnd;
	HWND h_log;

#endif

public:
	re_logconsole();
	~re_logconsole();

	virtual void con_show(bool b_show);
	virtual bool con_is_visible();
	virtual bool con_log_message(RE_LOGCONSOLE_MSGTYPE msg_type, const char *p_format, ...);
	virtual bool con_log_dump(const char *p_path);
	virtual int  con_log_lines();
	virtual bool con_log_grab(int line, char *p_dst, size_t dstlen);
};

