#pragma once

void *operator new(size_t _size);
void *operator new[](size_t _size);
void operator delete(void *_p_block);
void operator delete[](void *_p_block);