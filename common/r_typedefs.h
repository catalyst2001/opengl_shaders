/*
 Author: $username$
 Date: $time$
 File: $itemname$
 
*/
#pragma once
#include <stdint.h>

// ADDR TYPEDEFS
#if defined(_M_IX86)
typedef uint32_t addr_t;
#else
typedef uint64_t addr_t;
#endif