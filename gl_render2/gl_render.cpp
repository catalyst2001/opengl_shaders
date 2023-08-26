#include "gl_render.h"
#include <stdlib.h>
#include <stdint.h>

#include <assert.h>
#include "murmurhash.h"
#include "list.h"

#include <vector> //TEMP USE!

/* material */

/* mesh instance */
struct r_mesh_instance_t {
	int mode; //RENDER_MESH_RENDER_MODE
	int flags; //RENDER MESH INSTANCES FLAGS
	hmesh_t h_mesh;
	mat4x4 transform;
};

list_t materials_list;
list_t meshes_list;
std::vector<render_mesh_t *> meshes;

int render_init()
{
	list_init(&materials_list);
	list_init(&meshes_list);
	return 0;
}

int render_shutdown()
{
	list_free(&materials_list);
	list_free(&meshes_list);
	return 0;
}

int render_dump_resouces(const char *p_reslist)
{
	return 0;
}

int render_reload_resources(const char *p_reslist)
{
	return 0;
}

int render_mesh_create(hmesh_t *p_dst_hmesh, int mesh_hint, int mesh_type, void *p_verts_data, unsigned int num_verts)
{
	return 0;
}

int render_mesh_update(hmesh_t h_mesh, int mesh_hint, int mesh_type, void *p_verts_data, unsigned int num_verts)
{
	return 0;
}

int render_mesh_delete(hmesh_t h_mesh)
{
	return 0;
}

int render_mesh_get_type(int *p_dst_type, hmesh_t h_mesh)
{
	return 0;
}

int render_mesh_get_flags(int *p_dst_flags, hmesh_t h_mesh)
{
	return 0;
}

int render_mesh_animate(hmesh_t h_mesh, int animate_type, hmesh_t h_anim, float bias)
{
	return 0;
}

int render_anim_create(hanim_t *p_dst_hanim, const char *p_anim_path)
{
	return 0;
}

int render_anim_delete(hanim_t h_anim)
{
	return 0;
}

int render_change_screen_mode(int screen_mode)
{
	return 0;
}

int render_change_resolution(int width, int height)
{
	return 0;
}

int render_enable(int param)
{
	return 0;
}

int render_disable(int param)
{
	return 0;
}

int render_extension_is_supported(const char *p_extname)
{
	return 0;
}
