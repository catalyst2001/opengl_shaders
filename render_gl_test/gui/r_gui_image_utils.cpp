#include "r_gui_image_utils.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../../common/stb/stb_image.h"

r_image::r_image() : flags(0), bpp(0), width(0), height(0), size(0), p_data(NULL), status(IMGLDR_STATUS_OK)
{
}

r_image::r_image(uint8_t *_p_data, int _width, int _height, int _bpp)
{
	flags = R_IMAGE_MANUALLY_INITIALIZED;
	bpp = _bpp;
	width = _width;
	height = _height;
	size = width * height * (_bpp / 8);
	p_data = _p_data;
	status = IMGLDR_STATUS_OK;
}

r_image::r_image(const char *p_filename, bool b_flip_vertical)
{
	flags = 0;
	if(!load_from_file(p_filename, b_flip_vertical))
		status = IMGLDR_STATUS_UNKNOWN_ERROR;
}

r_image::~r_image()
{
	if(p_data && !(flags & R_IMAGE_MANUALLY_INITIALIZED))
		stbi_image_free(p_data);
}

bool r_image::load_from_file(const char *p_filename, bool b_flip_vertical)
{
	p_data = (uint8_t *)stbi_load(p_filename, &width, &height, &bpp, 4);
	if (!p_data) {
		//stbi_failure_reason();
		return false;
	}

	if (b_flip_vertical)
		stbi__vertical_flip(p_data, width, height, bpp);

	return true;
}

r_image::IMGLDR_STATUS r_image::get_status()
{
	return status;
}

int r_image::get_bpp()
{
	return bpp;
}

int r_image::get_width()
{
	return width;
}

int r_image::get_height()
{
	return height;
}

uint8_t *r_image::get_image_data()
{
	return p_data;
}
