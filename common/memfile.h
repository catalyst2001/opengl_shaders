#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct memfile_s {
	size_t size;
	uint8_t data[];
} memfile_t;

typedef enum MEMFILE_STATUS_ {
	MEMFILE_STATUS_OK = 0,
	MEMFILE_STATUS_FAILED_TO_OPEN_FILE,
	MEMFILE_STATUS_ALLOCATION_FAILED,
	MEMFILE_STATUS_EMPTY_FILE,
	MEMFILE_STATUS_READING_FAILED,
	MEMFILE_STATUS_UNALLOCATED
} MEMFILE_STATUS;

MEMFILE_STATUS memfile_load(memfile_t **p_dst, const char *p_filename);
uint8_t *memfile_data(const memfile_t *p_src);
uint8_t *memfile_data(const memfile_t *p_src);
MEMFILE_STATUS memfile_free(memfile_t **p_src);
