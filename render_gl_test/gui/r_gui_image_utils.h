#pragma once
#include <stdint.h>

#define R_IMAGE_MANUALLY_INITIALIZED (1 << 0)

class r_image
{
public:
	enum IMGLDR_STATUS {
		IMGLDR_STATUS_OK = 0,
		IMGLDR_STATUS_UNSUPPORTED_FORMAT,
		IMGLDR_STATUS_OUT_OF_MEMORY,
		IMGLDR_STATUS_UNKNOWN_ERROR
	};

private:
	int flags;
	int bpp;
	int width;
	int height;
	size_t size;
	uint8_t *p_data;
	IMGLDR_STATUS status;

public:
	r_image();
	r_image(uint8_t *p_data, int width, int height, int bpp);
	r_image(const char *p_filename, bool b_flip_vertical);
	~r_image();

	bool load_from_file(const char *p_filename, bool b_flip_vertical);

	IMGLDR_STATUS get_status();
	int get_bpp();
	int get_width();
	int get_height();
	uint8_t *get_image_data();
};

