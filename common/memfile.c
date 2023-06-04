#include "memfile.h"

MEMFILE_STATUS memfile_load(memfile_t **p_dst, const char *p_filename)
{
	FILE *fp;
	fpos_t file_size;
	memfile_t *p_memfile;

	fopen_s(&fp, p_filename, "rb");
	if (!fp)
		return MEMFILE_STATUS_FAILED_TO_OPEN_FILE;

	fseek(fp, 0, SEEK_END);
	fgetpos(fp, &file_size);
	if (!file_size) {
		fclose(fp);
		return MEMFILE_STATUS_EMPTY_FILE;
	}

	/* allocate memory */
	p_memfile = (memfile_t *)calloc(1, sizeof(memfile_t) + (size_t)file_size);
	if (!p_memfile) {
		fclose(fp);
		return MEMFILE_STATUS_ALLOCATION_FAILED;
	}
	p_memfile->size = (size_t)file_size;
	
	/* reading from file to heap */
	if (fread(p_memfile->data, 1, p_memfile->size, fp) != p_memfile->size) {
		free(p_memfile);
		fclose(fp);
		return MEMFILE_STATUS_READING_FAILED;
	}
	return MEMFILE_STATUS_OK;
}

uint8_t *memfile_data(const memfile_t *p_src)
{
	return (uint8_t *)p_src->data;
}

MEMFILE_STATUS memfile_free(memfile_t **p_src)
{
	if (*p_src) {
		free(*p_src);
		*p_src = NULL;
		return MEMFILE_STATUS_OK;
	}
	return MEMFILE_STATUS_UNALLOCATED;
}