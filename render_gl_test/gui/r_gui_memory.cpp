#include <stdlib.h>
#include <stdio.h>
#include "r_gui_memory.h"

#ifdef RGUI_HEAP_LOGGING
#define RGUI_LOG_HEAP(format, ...) fprintf(stdout, format "\n", __VA_ARGS__);
#else
#define RGUI_LOG_HEAP(format, ...)
#endif

void *operator new(size_t _size)
{
	void *_p_heap_block = calloc(1, _size);
	RGUI_LOG_HEAP("[rgui] void *operator new(size_t _size) Heap block allocated with size (%zd) on address (0x%p)", _size, _p_heap_block);
	return _p_heap_block;
}

void *operator new[](size_t _size)
{
	void *_p_heap_block = calloc(1, _size);
	RGUI_LOG_HEAP("[rgui] void *operator new[](size_t _size) Heap block allocated with size (%zd) on address (0x%p)", _size, _p_heap_block);
	return _p_heap_block;
}

void operator delete(void *_p_block)
{
	RGUI_LOG_HEAP("[rgui] void operator delete(void *_p_block) Block (0x%p) freed", _p_block);
	free(_p_block);
}

void operator delete[](void *_p_block)
{
	RGUI_LOG_HEAP("[rgui] void operator delete[](void *_p_block) Block (0x%p) freed", _p_block);
	free(_p_block);
}