#pragma once
#include <stdint.h>

#define RENDER_UNUSED_PARAM(x)

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

enum RENDER_MATERIAL_MAP {
	RENDER_MATERIAL_MAP_DIFFUSE = 0,
	RENDER_MATERIAL_MAP_NORMAL,
	RENDER_MATERIAL_MAP_ROUGHNESS,
	RENDER_MATERIAL_MAP_METALLIC,
	RENDER_MATERIAL_MAP_AMBIENT_OCCLUSION,
	RENDER_MATERIAL_MAP_HEIGHT,
	RENDER_MATERIAL_MAP_OPACITY,

	RENDER_MATERIAL_MAP_MAX_COUNT
};
RENDER_UNUSED_PARAM(sizeof(RENDER_MATERIAL_MAP));

struct render_material_t {
	uint32_t flags;
	htex_t textures[RENDER_MATERIAL_MAP_MAX_COUNT];
};
RENDER_UNUSED_PARAM(sizeof(render_material_t));

typedef void *handle_t;
typedef handle_t htex_t;
typedef handle_t hmesh_t;
typedef handle_t hanim_t;
typedef handle_t hmdl_t;
typedef handle_t hskl_t;
typedef handle_t hshader_program_t;

/* MESHES */
typedef float mat4x4[4][4];
typedef float vec3[3];
typedef float vec2[2];

struct render_anim_frame_t {
	mat4x4 transform;
};
RENDER_UNUSED_PARAM(sizeof(render_anim_frame_t));

struct render_anim_t {
	float fps;
	uint32_t hash;
	uint32_t num_frames;
	uint32_t num_joints;
	uint32_t frames_vbo;
};
RENDER_UNUSED_PARAM(sizeof(render_anim_t));

struct render_mesh_t {
	int hint;
	int type;
	int num_active_anims;
	render_material_t *p_mat;
};
RENDER_UNUSED_PARAM(sizeof(render_mesh_t));

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
RENDER_UNUSED_PARAM(sizeof(mesh_vertex_t));

/* ANIMATED MESH VERTEX STRUCT */
#define MAX_VERTEX_WEIGHTS 4

struct mesh_animated_vertex_t : mesh_vertex_t {
	int num_weights;
	int indices[MAX_VERTEX_WEIGHTS]; //bone indexes
	float weights[MAX_VERTEX_WEIGHTS]; //bone weights
};
RENDER_UNUSED_PARAM(sizeof(mesh_animated_vertex_t));

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

enum RENDER_ANIM_SELECT_TYPE {
	RENDER_ANIM_SELECT_TYPE_REPLACE_AND_PLAY = 0,
	RENDER_ANIM_SELECT_TYPE_SMOOTHLY_MORPH,
	RENDER_ANIM_SELECT_TYPE_PLAY_BLENDED
};

enum RENDER_TEXTURE_LDR_TYPE {
	RENDER_TEXTURE_LDR_TYPE_FROM_FILE = 0,
	RENDER_TEXTURE_LDR_TYPE_FROM_MEMORY
};

struct anim_info_t {
	int32_t skelid;
	uint32_t num_joints;
	uint32_t num_keyframes;
	float default_fps;
	float current_fps;
};

#define MAX_MODEL_ACTIVE_ANIMS 16

struct model_info_t {
	const char *p_path;
	const char *p_name;
	uint32_t num_active_anims;
	uint32_t num_model_meshes;
	hanim_t *p_active_anims;
	hmesh_t *p_meshes;
};

struct texture_create_info_t {
	const char *p_path;
	const char *p_name;
	unsigned int size;
	RENDER_TEXTURE_LDR_TYPE ldtype;
};

typedef handle_t hshader_obj_t;
typedef handle_t hshader_prog_t;

enum RENDER_SHADER_LDR_TYPE {
	RENDER_SHADER_LDR_TYPE_FROM_FILE = 0,
	RENDER_SHADER_LDR_TYPE_FROM_MEMORY
};

enum RENDER_SHADER_TYPE {
	RENDER_SHADER_TYPE_VERTEX = 0,
	RENDER_SHADER_TYPE_GEOMETRY,
	RENDER_SHADER_TYPE_TESSELATION,
	RENDER_SHADER_TYPE_FRAGMENT
};

enum RENDER_SHADER_LDR_TYPE {
	RENDER_SHADER_LDR_TYPE_FROM_FILE = 0,
	RENDER_SHADER_LDR_TYPE_FROM_FILE_ALWAYS,
	RENDER_SHADER_LDR_TYPE_FROM_MEMORY,
	RENDER_SHADER_LDR_TYPE_FROM_MEMORY_ALWAYS
};

struct shader_program_sources_ptrs_t {
	const char *p_vert_source;
	const char *p_geom_source;
	const char *p_tess_source;
	const char *p_frag_source;
};

class ire_render
{
public:
	/* render init/shutdown */
	virtual int init() = 0;
	virtual int shutdown() = 0;

	/* textures (draft 1) */
	virtual int texture_load(htex_t *p_dst_texture, const texture_create_info_t *p_create_info) = 0;
	virtual int texture_free(htex_t h_texture) = 0;

	/* shaders (draft 1) */
	virtual int shader_program_create(hshader_program_t *p_dst_hshaderprog, const char *p_file_path) = 0;
	virtual int shader_program_create_from_SPIRV(hshader_program_t *p_dst_hshaderprog, uint8_t *p_spirv_code, size_t size) = 0;
	virtual int shader_program_get_binary(uint8_t **p_dst_ptr, size_t *p_dst_size, hshader_program_t h_shaderprog) = 0;
	virtual int shader_program_binary_free_ptr(uint8_t *p_ptr) = 0;
	virtual int shader_program_delete(hshader_program_t h_shaderprog) = 0;


	//shader_program_sources_ptrs_t



	virtual int shaders_reload_all() = 0;




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

	/* mesh (draft 1) */
	virtual int mesh_create(hmesh_t *p_dst_hmesh, const char *p_name, RENDER_MESH_UPADTE_TYPE update_type, RENDER_MESH_TYPE mesh_type, RENDER_MESH_HINT hint, void *p_verts_data, unsigned int num_verts) = 0;
	virtual int mesh_find(hmesh_t *p_dst_mesh, const char *p_name) = 0;
	virtual int mesh_info(hmesh_t h_mesh) = 0;
	virtual int mesh_update(hmesh_t h_mesh, int mesh_hint, int mesh_type, void *p_verts_data, unsigned int num_verts) = 0;
	virtual int mesh_delete(hmesh_t h_mesh) = 0;
	virtual int mesh_get_flags(int *p_dst_flags, hmesh_t h_mesh) = 0;

	/* skeleton (draft 1) */
	virtual int skel_create(hskl_t *p_dst_hskl, const char *p_name) = 0;
	virtual int skel_load(hskl_t *p_dst_hskl, const char *p_filename) = 0;
	virtual int skel_find(hskl_t *p_dst_hskl, const char *p_name) = 0;
	virtual int skel_dump(hskl_t h_skl, const char *p_filename) = 0;
	virtual int skel_delete(hskl_t h_skl) = 0;

	/* animation (draft 1)*/
	virtual int anim_create(hanim_t *p_dst_hanim, const char *p_name, float fps, uint32_t num_keyframes) = 0;
	virtual int anim_load(hanim_t *p_dst_hanim, const char *p_path, const char *p_name) = 0;
	virtual int anim_info(anim_info_t *p_dst_info, hanim_t h_anim) = 0;
	virtual int anim_dump(hanim_t h_anim, const char *p_filename) = 0;
	virtual int anim_delete(hanim_t h_anim) = 0;
	
	/* model (draft 1) */
	virtual int model_create(hmdl_t *p_dst_hmdl, const char *p_name) = 0;
	virtual int model_set_base_skeleton(hmdl_t h_mdl, hskl_t h_skel) = 0;
	virtual int model_add_mesh_part(hmesh_t h_mesh, unsigned int offset) = 0;
	virtual int model_skel_is_compatible(hmdl_t h_mdl, hskl_t h_skel) = 0;
	virtual int model_anim_select(RENDER_ANIM_SELECT_TYPE anim_op_type, hmdl_t h_mdl, hanim_t h_anim, unsigned int start_frame, unsigned int end_frame, float blend_factor) = 0;
	virtual int model_anim_set_blend_factor(hmdl_t h_mdl, hanim_t h_anim, float blend_factor) = 0;
	virtual int model_anim_set_playback_speed(hmdl_t h_mdl, hanim_t h_anim, float fps) = 0;
	virtual int model_info(model_info_t *p_dst_info, hmdl_t h_mdl) = 0;
	virtual int model_delete(hmdl_t h_mdl) = 0;

	/* WARNING: IT IS LOW PERFORMANCE METHOD! DON'T USE THIS IN EVERY FRAME!!!  */
	int model_anim_set_bindpose_by_skel(hmdl_t h_mdl, const char *p_skelname) {
		hskl_t h_skel;
		int status = skel_find(&h_skel, p_skelname);
		if (!h_skel)
			return status;

		//TODO: SKELETON EQUALS ONE FRAME OF ANIM !!!
		return model_anim_select(RENDER_ANIM_SELECT_TYPE_REPLACE_AND_PLAY, h_mdl, (hanim_t)h_skel, 0, 1, 1.f);
	}

};