#include "strutl.h"

size_t str_set_by_utf8(strbuf_t *p_dst, const char *p_utf8_string, size_t length)
{
	//size_t num_of_chars_converted;
	//assert(p_dst && p_utf8_string);
	//mbstowcs_s(&num_of_chars_converted, p_dst->p_data, p_utf8_string, (p_dst->length < length) ? length : p_dst->length);
	//return num_of_chars_converted;
	assert(0); //TOOD: IMPLEMENT ME
	return 0;
}

size_t str_set_by_utf16(strbuf_t *p_dst, const strwchar_t *p_utf16_string, size_t length)
{
	assert(p_dst && p_utf16_string);
	return r_wcscpy_s(p_dst->p_data, p_dst->length, p_utf16_string);
}

size_t str_to_utf8(char *p_dst, size_t dstlen, const strbuf_t *p_src)
{
	return 0;
}
