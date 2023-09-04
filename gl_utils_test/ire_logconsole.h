/*
 Author: $username$
 Date: $time$
 File: $itemname$
 
*/
#pragma once

enum RE_LOGCONSOLE_MSGTYPE {
	LOGCONSOLE_MSGTYPE_NOTIFY = 0,
	LOGCONSOLE_MSGTYPE_WARNING,
	LOGCONSOLE_MSGTYPE_CRITICAL_ERROR
};

class ire_logconsole
{
public:
	virtual void con_show(bool b_show) = 0;
	virtual bool con_is_visible() = 0;
	virtual bool con_log_message(RE_LOGCONSOLE_MSGTYPE msg_type, const char *p_format, ...) = 0;
	virtual bool con_log_dump(const char *p_path) = 0;
	virtual int  con_log_lines() = 0;
	virtual bool con_log_grab(int line, char *p_dst, size_t dstlen) = 0;
};