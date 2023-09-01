/*
 Author: $username$
 Date: $time$
 File: $itemname$
 
*/
#pragma once

template<typename _type>
class re_base_container
{
	size_t size;
	size_t capacity;
	_type *p_data;
public:
	re_base_container() : size(0), capacity(0), p_data(nullptr) {}
	re_base_container(size_t start_cap) : size(0), capacity(start_cap), p_data(nullptr) {}
	~re_base_container() {}




};