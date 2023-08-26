#pragma once
#include <stdint.h>

/* basic types */
typedef unsigned int ui32_t;

/* RENDER PBR MATERIAL */
#define RENDER_MATERIAL_HAS_DIFFUSE           (1 << 0)
#define RENDER_MATERIAL_HAS_NORMAL            (1 << 1)
#define RENDER_MATERIAL_HAS_ROUGHNESS         (1 << 2)
#define RENDER_MATERIAL_HAS_METALLIC          (1 << 3)
#define RENDER_MATERIAL_HAS_AMBIENT_OCCLUSION (1 << 4)
#define RENDER_MATERIAL_HAS_HEIGHT            (1 << 5)
#define RENDER_MATERIAL_HAS_OPACITY           (1 << 6)

struct render_material_t {
	ui32_t flags;
	ui32_t diffuse_tex;
	ui32_t normal_tex;
	ui32_t roughness_tex;
	ui32_t metallic_tex;
	ui32_t ambient_occlusion_tex;
	ui32_t height_tex;
	ui32_t opacity_tex;
};

typedef void *handle_t;
typedef handle_t hmesh_t;
typedef handle_t hanim_t;
typedef handle_t hmdl_t;
typedef handle_t hskl_t;

/* MESHES */
typedef float mat4x4[4][4];
typedef float vec3[3];
typedef float vec2[2];

struct render_anim_frame_t {
	mat4x4 transform;
};

struct render_anim_t {
	float fps;
	uint32_t hash;
	uint32_t num_frames;
	uint32_t num_joints;
	uint32_t frames_vbo;
};

#define MAX_ACTIVE_ANIMS 16

struct render_mesh_t {
	int hint;
	int type;
	int num_active_anims;
	render_material_t *p_mat;
};

enum RENDER_MESH_HINT {
	RENDER_MESH_HINT_WORLD = 0, // World mesh part.
	RENDER_MESH_HINT_PROP,      // World object part.
	RENDER_MESH_HINT_ACTOR      // Player or NPC part.
};

enum RENDER_MESH_TYPE {
	RENDER_MESH_TYPE_STATIC = 0,
	RENDER_MESH_TYPE_ANIMATED
};

enum RENDER_MESH_UPADTE_TYPE {
	RENDER_MESH_UPADTE_TYPE_STATIC = 0,
	RENDER_MESH_UPADTE_TYPE_DYNAMIC,
	RENDER_MESH_UPADTE_TYPE_AUTO
};

/* NO ANIMATED MESH VERTEX STRUCT */
struct mesh_vertex_t {
	vec3 vertex;
	vec3 normal;
	vec2 texcoord;
};

/* ANIMATED MESH VERTEX STRUCT */
#define MAX_VERTEX_WEIGHTS 8

struct mesh_animated_vertex_t : mesh_vertex_t {
	int num_weights;
	int indices[MAX_VERTEX_WEIGHTS]; //bone indexes
	float weights[MAX_VERTEX_WEIGHTS]; //bone weights
};

// RENDER MESH MODES
enum RENDER_MESH_RENDER_MODE {
	RENDER_MESH_RENDER_MODE_NONE = 0,
	RENDER_MESH_RENDER_MODE_DEFAULT,
	RENDER_MESH_RENDER_MODE_WIREFRAME
};

// RENDER MESH INSTANCES FLAGS
#define RENDER_MESH_INSTANCE_FLAG_ALPHA (1 << 0)
#define RENDER_MESH_INSTANCE_FLAG_WITH_SHADOW (1 << 1)
#define RENDER_MESH_INSTANCE_FLAG_CULL_FACE (1 << 2)
#define RENDER_MESH_INSTANCE_FLAG_LIGHTING (1 << 3)
#define RENDER_MESH_INSTANCE_FLAG_OUTLINE_LIGHTING (1 << 4)

enum RENDER_MESH_ANIMATE_TYPE {
	RENDER_MESH_ANIMATE_TYPE_PLAY = 0,
	RENDER_MESH_ANIMATE_TYPE_PLAY_BLEND,
	RENDER_MESH_ANIMATE_TYPE_STOP,
	RENDER_MESH_ANIMATE_TYPE_PAUSE
};

int render_mesh_animate(hmesh_t h_mesh, int animate_type, hmesh_t h_anim, float bias);

/* ANIMATIONS */
int render_anim_create(hanim_t *p_dst_hanim, const char *p_anim_path);
int render_anim_delete(hanim_t h_anim);

/* RENDER PARAMS */
enum RENDER_SCREEN_MODE {
	RENDER_SCREEN_MODE_WINDOWED = 0,
	RENDER_SCREEN_MODE_FULLSCREEN
};



/* callbacks for physics */
class ire_render_notify_callbacks
{
public:


};

enum RENDER_PROPERTY {
	RENDER_PROPERTY_MULTISAMPLED = 0,
};

class ire_render
{
public:
	/* render init/shutdown */
	virtual int init() = 0;
	virtual int shutdown() = 0;

	/* settings */
	virtual int enable(int param) = 0;
	virtual int disable(int param) = 0;

	virtual int change_screen_mode(int screen_mode) = 0;
	virtual int change_screen_resolution(int width, int height) = 0;

	/* extensions */
	virtual int ext_avalible(const char *p_extname) = 0;

	/* save loaded resources for restore context if it is loss */
	virtual int res_list_dump(const char *p_reslist) = 0;
	virtual int res_list_reload(const char *p_reslist) = 0;

	/* meshe */
	virtual int mesh_create(hmesh_t *p_dst_hmesh, const char *p_name, RENDER_MESH_UPADTE_TYPE update_type, RENDER_MESH_TYPE mesh_type, RENDER_MESH_HINT hint, void *p_verts_data, unsigned int num_verts) = 0;
	virtual int mesh_find(hmesh_t *p_dst_mesh, const char *p_name) = 0;
	virtual int mesh_info(hmesh_t h_mesh) = 0;
	virtual int mesh_update(hmesh_t h_mesh, int mesh_hint, int mesh_type, void *p_verts_data, unsigned int num_verts) = 0;
	virtual int mesh_delete(hmesh_t h_mesh) = 0;
	virtual int mesh_get_flags(int *p_dst_flags, hmesh_t h_mesh) = 0;

	/* skeleton */
	virtual int skel_create(hskl_t *p_dst_hskl, const char *p_name) = 0;
	virtual int skel_load(hskl_t *p_dst_hskl, const char *p_filename) = 0;
	virtual int skel_find(hskl_t *p_dst_hskl, const char *p_name) = 0;
	virtual int skel_dump(hskl_t h_skl, const char *p_filename) = 0;
	virtual int skel_delete(hskl_t h_skl) = 0;
	
	
	/* model */
	virtual int model_create(hmdl_t *p_dst_hmdl, const char *p_name) = 0;
	virtual int model_set_skeleton(hmdl_t h_mdl) = 0;
	virtual int model_skel_compatible(hmdl_t h_mdl, hskl_t h_skel) = 0;
	virtual int model_anim_select(hmdl_t h_mdl, hanim_t h_anim, unsigned int start_frame, unsigned int end_frame, float blend_weight) = 0;

	/* WARNING: IT IS LOW PERFORMANCE METHOD! DON'T USE THIS IN EVERY FRAME!!!  */
	int model_anim_set_bindpose_by_skel(hmdl_t h_mdl, const char *p_skelname) {
		int status;
		hskl_t h_skel;
		status = skel_find(&h_skel, p_skelname);
		if (!h_skel)
			return status;

		//TODO: SKELETON EQUALS ONE FRAME OF ANIM !!!
		return model_anim_select(h_mdl, (hanim_t)h_skel, 0, 1, 1.f);
	}

};