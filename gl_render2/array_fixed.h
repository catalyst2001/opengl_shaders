/*
 Author: $username$
 Date: $time$
 File: $itemname$
 
*/
#pragma once
#include <stddef.h>

template<typename _type, size_t _cap>
class array_fixed {
	size_t size;
	_type array[_cap];
public:
	array_fixed() : size(0) {}
	~array_fixed() {}

	bool push(_type elem) {
		if (size == _cap)
			return false;

		array[size++] = elem;
		return true;
	}

	bool pop(_type &dst) {
		if (size == 0)
			return false;

		dst = array[--size];
		return true;
	}

	size_t get_cap() { return _cap; }
	size_t get_size() { return size; }
	bool   is_valid_idx(size_t idx) { return idx < _cap; }

	_type &operator[](size_t idx) {
		assert(idx < _cap);
		return array[idx];
	}
};