#include "gl_render.h"

gl_render render;

void render_message_callback(RENDER_LOG_MSG msgtype, const char *p_text)
{
	static const char *p_types[] = {
		"RENDER_LOG_MSG_NOTIFY",
		"RENDER_LOG_MSG_WARNING",
		"RENDER_LOG_MSG_CRITICAL_ERROR"
	};
	printf("%s: %s\n", p_types[msgtype], p_text);
}

int render_test()
{
	char error[128];

	re_render_init_info_t r_init;
	r_init.color_bits = 24;
	r_init.depth_bits = 32;
	r_init.width = 800;
	r_init.height = 600;
	r_init.flags = RENDER_INIT_FLAG_CENTER|RENDER_INIT_FLAG_MULTISAMPLING;

	int status = render.init(error, sizeof(error), &r_init, render_message_callback);
	if (status != RENDER_STATUS_OK) {
		printf("Failed to initialize render. Error %d  '%s'\n", status, error);
		return 1;
	}

	render.render_cycle();
	render.shutdown();
}

int main()
{
	render_test();
	return 0;
}