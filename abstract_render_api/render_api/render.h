/*
 React Engine Render API
 
 Author: Deryabin K.
 Date: 29.06.2023
 File: render.h
 
*/
#pragma once

#define DEVICE_FLAG_ACTIVE (1 << 0)

#define R_RGBA(r, g, b, a) (r | (g << 8) | (b << 16) | (a << 24))

struct r_render_driver;

enum R_RENDER_DRIVER {
	/* NULL DEVICE DRIVER */
	REACT_RENDER_DRIVER_NULL = 0,

	/* CROSS-PLATFORM */
	REACT_RENDER_DRIVER_OPENGL,
	REACT_RENDER_DRIVER_VULKAN,
	REACT_RENDER_DRIVER_SOFTWARE,

	/* WINDOWS */
	REACT_RENDER_DRIVER_D3D8,
	REACT_RENDER_DRIVER_D3D9,
	REACT_RENDER_DRIVER_D3D10,
	REACT_RENDER_DRIVER_D3D11
};

/* DEVICES ENUMERATOR */
struct r_video_adapter_info
{
	char device_name[64];
	char manufacturer[32];
};

struct r_monitor_info
{
	char device_name[64];
	char manufacturer[32];
};

typedef void *r_ptr_t;
struct r_handle_t { r_ptr_t unused; };

typedef r_handle_t r_hvertex_buffer;
typedef r_handle_t r_hindex_buffer;

class r_render_api
{
public:
	/* ENUMERATE VIDEO ADAPTERS */
	virtual int              get_video_adapters_number() = 0;
	virtual bool             get_video_adapter_info(r_video_adapter_info *p_dst_info, int adapter_index) = 0;
		             
	/* ENUMERATE MONITORS  */
	virtual int              get_monitors_number() = 0;
	virtual int              get_default_monitor_index() = 0;
	virtual bool             get_monitor_info(r_monitor_info *p_dst_info, int monitor_index) = 0;

	/* RENDER DEVICES */
	virtual r_render_device *device_create(R_RENDER_DRIVER driver, int color_bits, int depth_bits, int stencil_bits) = 0;
	virtual int              device_remove(r_render_device *p_device) = 0;
};

#define R_VERTEX_BUFFER 0
#define R_INDEX_BUFFER 1

#define R_BUFFER_ACCESS_STATIC 0
#define R_BUFFER_ACCESS_DYNAMIC 1

#define R_BUFFER_MEMORY_ALL 0
#define R_BUFFER_MEMORY_READONLY 1
#define R_BUFFER_MEMORY_WRITEONLY 2

struct r_render_device
{
	/* INIT DEVICE */
	virtual int  run_device() = 0;

	/* DEVICE INFO */
	const char      *(*get_device_name)();
	r_render_driver *(*get_render_driver)();

	void            *(*set_color_bits)(int color_bits);
	void            *(*set_depth_bits)(int depth_bits);
	void            *(*set_stencil_bits)(int stencil_bits);
				    
	void            *(*set_clear_color)(int color);
	void            *(*clear)(int color);

	/* WINDOW */
	void             (*set_window_caption)(const char *p_title);
	const char      *(*get_window_caption)();
	void            *(*set_window_size_callback)();
	void            *(*set_window_events_callback)();
	
	/* CHECKING STATUS */
	bool             (*set_flags_var)(int *p_flags);
	void            *(*begin_scene)();
	void            *(*end_scene)();

	/* HARDWARE BUFFERS */
	int              (*buffer_alloc)(r_hvertex_buffer *p_dst_buffer, int buffer_type, int buffer_memory_access, size_t bytes);
	int              (*buffer_bind)(r_hvertex_buffer h_buffer);
	int              (*buffer_free)(r_hvertex_buffer h_buffer);
	int              (*buffer_copy)(r_hvertex_buffer h_dest_buffer, r_hvertex_buffer h_src_buffer);

	int              (*buffer_write)();
	int              (*buffer_read)();

	int              (*buffer_map)(r_hvertex_buffer h_buffer, void **p_ptr);
	int              (*buffer_unmap)(r_hvertex_buffer h_buffer, void **p_ptr);

	/* RENDER TARGETS */
	int              (*create_render_target)();
};

r_render_api *r_get_render_api();

struct r_render_driver
{

};

#if 1

#define PRINT(x, ...)

int render_flags;

void test_render()
{
	r_render_api *p_render = r_get_render_api();
	
	/* ENUM MONITORS */
	r_monitor_info minfo;
	for (int i = 0; i < p_render->get_monitors_number(); i++) {
		if (p_render->get_monitor_info(&minfo, i)) {
			PRINT("Device name: %s", minfo.device_name);
			PRINT("Manufacturer: %s", minfo.manufacturer);
		}
	}

	/* ENUM VIDEO ADAPTERS */
	r_video_adapter_info ainfo;
	for (size_t i = 0; i < p_render->get_video_adapters_number(); i++) {
		if (p_render->get_video_adapter_info(&ainfo, i)) {
			PRINT("Device name: %s", minfo.device_name);
			PRINT("Manufacturer: %s", minfo.manufacturer);
		}
	}

	r_render_device *p_device = p_render->device_create(REACT_RENDER_DRIVER_OPENGL, 24, 32, 16);
	if (!p_device) {
		PRINT("Failed to create device");
		return;
	}

	p_device->set_window_caption("Test render application");
	p_device->set_clear_color(R_RGBA(20, 20, 20, 255));
	if (p_device->run_device()) {
		r_hvertex_buffer vertex_buffer;
		p_device->buffer_alloc(&vertex_buffer, R_BUFFER_ACCESS_STATIC, R_BUFFER_MEMORY_ALL, 10000);

		p_device->set_flags_var(&render_flags);
		while (render_flags & DEVICE_FLAG_ACTIVE) {
			p_device->begin_scene();





			p_device->end_scene();
		}
	}
	p_render->device_remove(p_device);
}
#endif