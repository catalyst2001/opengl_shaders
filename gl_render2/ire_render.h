/*
 React Game Engine Renderer

 Purpose: General game engine rendering API's implemented on OpenGL
 Author: Deryabin K. "catalyst" & Iminhodzaev O. "Nemox"
 Date: 28.08.2023 3:10
 File: ire_render.h
 
 TODO: ADD SORT INDEXER TO MESHES LIST!
 
 struct sort_indexer_t {
	int texture;
	int mesh;

	// all doubled data this

	size_t index; //starts matched objects
	size_t count; //count matched objects
 };

*/
#pragma once
#include <stdint.h>
#include <glm/glm.hpp>

enum RENDER_INIT_TYPE {
	RENDER_INIT_GL_LEGACY_COMPAT = 0,
	RENDER_INIT_GL_CORE
};

/* BASIC HANDLE TYPES */
typedef void *handle_t;
typedef handle_t htex_t;
typedef handle_t hmesh_t;
typedef handle_t hanim_t;
typedef handle_t hmdl_t;
typedef handle_t hskl_t;
typedef handle_t hshader_program_t;

/* RENDER STATUS CODES */
enum RENDER_STATUS {
	RENDER_STATUS_OK = 0,
	RENDER_STATUS_ERROR_OUT_OF_MEMORY,
	RENDER_STATUS_ERROR_INIT_WINDOW_SUBSYSTEM,
	RENDER_STATUS_ERROR_WINDOW,
	RENDER_STATUS_ERROR_CREATE_CONTEXT,
	RENDER_STATUS_ERROR_UNSUPPORTED_DISPLAY_RESOLUTION,
	RENDER_STATUS_ERROR_INVALID_PARAMETER,
	RENDER_STATUS_ERROR_PARAMETER_NOT_CHANGED,
	RENDER_STATUS_ERROR_NEEDED_EXTENSIONS_NO_AVALIBLE

};

#define RENDER_UNUSED_PARAM(x)

struct window_size_t {
	int width, height;
};

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

/* MESHES */

struct render_anim_frame_t {
	glm::mat4x4 transform;
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

struct mesh_draw_command_t {
	uint32_t material;
	uint32_t start_index;
	uint32_t count_indices;
};

/* UI MESH VERTEX STRUCT */
struct ui_mesh_vertex_t {
	glm::vec2 vertex;
	glm::vec2 texcoord;
	glm::vec3 color;
};

/* NO ANIMATED MESH VERTEX STRUCT */
struct mesh_vertex_t {
	glm::vec3 vertex;
	glm::vec3 normal;
	glm::vec2 texcoord;
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

struct shader_program_compile_log_t {
	size_t size;
	char *p_buffer;
};

/* render init struct */
#define RENDER_INIT_FLAG_DEFAULT (0)
#define RENDER_INIT_FLAG_CENTER (1 << 0)
#define RENDER_INIT_FLAG_FULLSCREEN (1 << 1)
#define RENDER_INIT_FLAG_MULTISAMPLING (1 << 2)

/* render global flags */
#define RENDER_WINDOW_FLAG_NONE    (0)
#define RENDER_WINDOW_FLAG_RUNNING (1 << 0)

struct re_render_init_info_t {
	int flags;
	int width;
	int height;
	int color_bits;
	int depth_bits;
	int stencil_bits;
};

enum RENDER_DEVICE_TYPE {
	RENDER_DEVICE_TYPE_UNKNOWN = 0,
	RENDER_DEVICE_TYPE_MONITOR,
	RENDER_DEVICE_TYPE_VIDEO_ADAPTER,
	RENDER_DEVICE_TYPE_KEYBOARD,
	RENDER_DEVICE_TYPE_MOUSE,
	RENDER_DEVICE_TYPE_JOYSTIK
};

/* HARDWARE DEVICE ACTION HOOKS */
class ire_hardware_callbacks
{
public:
	virtual void device_disabled(RENDER_DEVICE_TYPE device) = 0;
	virtual void device_enabled(RENDER_DEVICE_TYPE device) = 0;
	//virtual void device_
};

/* PER MONITOR INFO STRUCT */
struct re_monitor_info_t {
	char name[64];
	char description[64];
	uint32_t width;
	uint32_t height;
	uint32_t supported_modes_count;
};

/* PER MONITOR MODE INFO STRUCT */
struct re_monitor_mode_info_t {
	uint32_t width;
	uint32_t height;
	uint32_t colorbits;
};

class ire_visual_info
{
public:
	virtual int get_monitors_count(int *p_dst_monitors_count) = 0;
	virtual int get_primary_monitor_id(int *p_dst_primary_monitor) = 0;
	virtual int get_monitor_info(int monitor, re_monitor_info_t *p_dst_info) = 0;
	virtual int get_monitor_modes(int monitor, re_monitor_mode_info_t *p_dst_mode_info, uint32_t num_modes) = 0;


	virtual int set_device_action_callbacks(ire_hardware_callbacks *p_devs_act_callbacks) = 0;


};

/* RENDER LOG CALLBACK */
enum RENDER_LOG_MSG {
	RENDER_LOG_MSG_NOTIFY = 0,
	RENDER_LOG_MSG_WARNING,
	RENDER_LOG_MSG_CRITICAL_ERROR
};

typedef void (*render_log_message_callback)(RENDER_LOG_MSG msgtype, const char *p_text);

class ire_window_callbacks
{
public:
	virtual void on_mouse_move(int x, int y) = 0;
	virtual void on_mouse_keys(int x, int y, int button, int state) = 0;
	virtual void on_window_size(int width, int height) = 0;
	virtual void on_key(int key, int state) = 0;
	virtual void on_window_action() = 0;
	virtual void on_screen_power() = 0;
	virtual void on_printscreen() = 0;
};

class ire_render
{
public:
	/* render init/shutdown */
	virtual int init(char *p_dsterr, size_t maxlen, const re_render_init_info_t *p_init_info, render_log_message_callback p_log_callback) = 0;
	virtual int shutdown() = 0;
	virtual int render_cycle() = 0;

	virtual int set_window_title(const char *p_title, ...) = 0;
	virtual int get_window_title(char *p_dst, size_t dstlen) = 0;
	virtual int get_window_size(window_size_t *p_dst_size) = 0;

	/* textures (draft 1) */
	virtual int texture_load(htex_t *p_dst_texture, const texture_create_info_t *p_create_info) = 0;
	virtual int texture_free(htex_t h_texture) = 0;

	/* shaders (draft 1) */
	virtual int shader_program_create(hshader_program_t *p_dst_hshaderprog, shader_program_compile_log_t *p_dstlog, const char *p_file_path) = 0;
	virtual int shader_program_create_from_mem(hshader_program_t *p_dst_hshaderprog, shader_program_compile_log_t *p_dstlog, const shader_program_sources_ptrs_t *p_sources) = 0;
	virtual int shader_program_create_from_SPIRV(hshader_program_t *p_dst_hshaderprog, shader_program_compile_log_t *p_dstlog, uint8_t *p_spirv_code, size_t size) = 0;
	virtual int shader_program_get_binary(uint8_t **p_dst_ptr, size_t *p_dst_size, hshader_program_t h_shaderprog) = 0;
	virtual int shader_program_binary_free_ptr(uint8_t *p_ptr) = 0;
	virtual int shader_program_delete(hshader_program_t h_shaderprog) = 0;
	virtual int shaders_reload_all() = 0;

	virtual int set_lighting_shader(hshader_program_t h_shaderprog) = 0;
	virtual int set_shadow_shader(hshader_program_t h_shaderprog) = 0;
	virtual int set_postprocess_shader() = 0;

	/* settings */
	virtual int enable(int param) = 0;
	virtual int disable(int param) = 0;

	virtual int change_screen_mode(int screen_mode) = 0;
	virtual int change_screen_resolution(int width, int height) = 0;
	virtual int set_multisampling_samples(char *p_dsterr, size_t maxlen, int n_samples) = 0;

	/* extensions */
	virtual bool ext_avalible(const char *p_extname) = 0;

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
	virtual int model_add_mesh_part(hmdl_t h_mdl, hmesh_t h_mesh, unsigned int offset) = 0;
	virtual int model_skel_is_compatible(hmdl_t h_mdl, hskl_t h_skel) = 0;
	virtual int model_anim_select(RENDER_ANIM_SELECT_TYPE anim_op_type, hmdl_t h_mdl, hanim_t h_anim, unsigned int start_frame, unsigned int end_frame, float blend_factor) = 0;
	virtual int model_anim_set_blend_factor(hmdl_t h_mdl, hanim_t h_anim, float blend_factor) = 0;
	virtual int model_anim_set_playback_speed(hmdl_t h_mdl, hanim_t h_anim, float fps) = 0;
	virtual int model_info(model_info_t *p_dst_info, hmdl_t h_mdl) = 0;
	virtual int model_delete(hmdl_t h_mdl) = 0;

	/* draw functions */
	virtual int draw_list_add_mesh(hmesh_t h_mesh) = 0;
	virtual int draw_list_remove_mesh(hmesh_t h_mesh) = 0;
	virtual int draw_list_add_model(hmdl_t h_model) = 0;
	virtual int draw_list_remove_model(hmdl_t h_model) = 0;

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