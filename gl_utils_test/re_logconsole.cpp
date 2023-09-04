#include "re_logconsole.h"

#define LOG_CONSOLE_ID "reLogConsole"

#if defined LINUX

#else
#define LOG_CONTROL "reLogControl"

LRESULT CALLBACK main_window(HWND, UINT, WPARAM, LPARAM)
{

}

#endif

re_logconsole::re_logconsole()
{
#if defined LINUX

#else
	/* create main window */
	WNDCLASSEXA wcex;
	memset(&wcex, 0, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.hCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW));
	wcex.hInstance = GetModuleHandleW(0);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpszClassName = LOG_CONTROL;
	wcex.lpfnWndProc

#endif
}

re_logconsole::~re_logconsole()
{
#if defined LINUX

#else


#endif
}

void re_logconsole::con_show(bool b_show)
{
}

bool re_logconsole::con_is_visible()
{
	return false;
}

bool re_logconsole::con_log_message(RE_LOGCONSOLE_MSGTYPE msg_type, const char *p_format, ...)
{
	return false;
}

bool re_logconsole::con_log_dump(const char *p_path)
{
	return false;
}

int re_logconsole::con_log_lines()
{
	return 0;
}

bool re_logconsole::con_log_grab(int line, char *p_dst, size_t dstlen)
{
	return false;
}
