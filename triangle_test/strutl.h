#pragma once
#include <stdbool.h>
#include <string.h>
#include <assert.h>

typedef short strwchar_t;

typedef struct strbuf_s {
	strwchar_t *p_data;
	size_t length;
} strbuf_t;

//#undef _MSC_VER

#ifdef _MSC_VER
#include <Windows.h>

#define r_strcmp(str, cmpstr)                strcmp(str, cmpstr)
#define r_strncmp(str, cmpstr, count)        strncmp(str, cmpstr, count)
#define r_strcpy_s(dst, dstlen, src)         strcpy_s(dst, dstlen, src)
#define r_wcscmp(wcsstr, cmpwcsstr)          wcscmp(wcsstr, cmpwcsstr)
#define r_wcsncmp(wcsstr, cmpwcsstr, count)  wcsncmp(wcsstr, cmpwcsstr, count)
#define r_wcscpy_s(wcsdst, dstlen, wcssrc)   wcscpy_s(wcsdst, dstlen, wcssrc)
#else
#define r_strcmp(str, cmpstr)                strcmp(str, cmpstr)
#define r_strncmp(str, cmpstr, count)        strncmp(str, cmpstr, count)
#define r_strcpy_s(dst, dstlen, src)         strcpy(dst, src)
#define r_wcscmp(wcsstr, cmpwcsstr)          wcscmp(wcsstr, cmpwcsstr)
#define r_wcsncmp(wcsstr, cmpwcsstr, count)  wcsncmp(wcsstr, cmpwcsstr, count)
#define r_wcscpy_s(wcsdst, dstlen, wcssrc)   wcscpy_s(wcsdst, dstlen, wcssrc)
#endif

#define STR_ARRSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define STR_INIT(varptr, ptr, len) (varptr)->p_data = ptr; (varptr)->length = len;
#define STR_DECL_BUFFER_FIXED(varname, size) strwchar_t varname##_arr[size]; strbuf_t varname = { varname##_arr, STR_ARRSIZE(varname##_arr) };

size_t str_set_by_utf8(strbuf_t *p_dst, const char *p_utf8_string, size_t length);
size_t str_set_by_utf16(strbuf_t *p_dst, const strwchar_t *p_utf16_string, size_t length);
size_t str_to_utf8(char *p_dst, size_t dstlen, const strbuf_t *p_src);

inline void test()
{
	STR_DECL_BUFFER_FIXED(buffer, 100);
	str_set_by_utf8(&buffer, "chokoladka", 0);
}