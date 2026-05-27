
#ifdef AAA_SEEDFILE_H
#error "SEEDFILE_H included more than once."
#endif
#define AAA_SEEDFILE_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class c_obj_ui;
class c_param;
class system_window;

namespace aaa {
namespace file {

	extern	bool	gb_image_save_from_texture;
	extern	INT32	g_image_save_tex_bind;

	enum TYPE_IO : INT32
	{
		TYPE_IO_NONE = 0,

		TYPE_IO_TEXTURE_2D,
		TYPE_IO_TEXTURE_3D,
	//	TYPE_IO_BACKGROUND,		//2006 unused now
		TYPE_IO_SNAPSHOT,
		TYPE_IO_HEIGHT_MAP,
		TYPE_IO_PART_EMISSION_MASK,

		TYPE_IO_PART_EMISSION_IMAGE,
		TYPE_IO_PART_COLOR_MAP,
		TYPE_IO_BDD_TUBE,
		TYPE_IO_DEF,
		TYPE_IO_DFL,

		TYPE_IO_BIND_LIST_IMG,
		TYPE_IO_CAM,
		TYPE_IO_APP,
		TYPE_IO_RENDER,
		TYPE_IO_LIGHT_GROUP,

		TYPE_IO_ENV,
		TYPE_IO_DEMO,
		TYPE_IO_BSP,
		TYPE_IO_FOG,
		TYPE_IO_COLOR,

		TYPE_IO_BDD_PART,
		TYPE_IO_GEO,
		TYPE_IO_TRACKER,
		TYPE_IO_CPP,
		TYPE_IO_BDD_FACE,

		TYPE_IO_TEX_ANIM,
		TYPE_IO_PROC,
		TYPE_IO_LENTICULAR_IMAGE,
		TYPE_IO_OBJ_WAVEFRONT,
		TYPE_IO_BDD_TRI,

		TYPE_IO_BDD_TEXT_2D,
		TYPE_IO_TRAXS,
		TYPE_IO_TRAX,
		TYPE_IO_BDD_ARRAY,
		TYPE_IO_LAYER,

		TYPE_IO_MIDI,
		TYPE_IO_BDD_CONE,
		TYPE_IO_FN3D,
		TYPE_IO_NET,
		TYPE_IO_BDD_GRID,

		TYPE_IO_BDD_MOCAP,
		TYPE_IO_BDD_MOCAP_DATA,
		TYPE_IO_SND_WAVE,
		TYPE_IO_MATERIAL,
		TYPE_IO_BDD_CIRCLE,

		TYPE_IO_BDD_TUBE_PATH,
		TYPE_IO_JOY,
		TYPE_IO_LIGHTS_SWITCH,
		TYPE_IO_ISENSE,
		TYPE_IO_MODEL,

		TYPE_IO_BDD_MOVIE,
		TYPE_IO_MOVIE,
		TYPE_IO_BDD_PROJ_CONE,
		TYPE_IO_PICKING,
		TYPE_IO_BDD_SPHERE,

		TYPE_IO_BDD_TORUS,
		TYPE_IO_BDD_MOCAP_ACTOR,
		TYPE_IO_BDD_ALGO_MAA,
		TYPE_IO_SHIP,
		TYPE_IO_IMAGE_CPP,

		TYPE_IO_INFO,
		TYPE_IO_VALUE,
		TYPE_IO_BDD_NULL,
		TYPE_IO_MAP,
		TYPE_IO_BANK,

		TYPE_IO_BDD_BOID,
		TYPE_IO_BDD_LAYER_SELECTOR,
		TYPE_IO_STENCIL,
		TYPE_IO_FONT,
		TYPE_IO_VIDEO,

		TYPE_IO_TEX_VIDEO,
		TYPE_IO_BDD_BOXES,
		TYPE_IO_MULTIPLE,
		TYPE_IO_COLOR_MOD,
		TYPE_IO_CSV,

		TYPE_IO_BDD_FT_FACE,
		TYPE_IO_TXT,
		TYPE_IO_BDD_MIGUEL,
		TYPE_IO_BDD_POINT_NETWORK,
		TYPE_IO_BDD_GRID_ADJUSTABLE,

		TYPE_IO_BDD_TIME_WATCHER,
		TYPE_IO_TIME_BUFFER_MASTER,
		TYPE_IO_BDD_CURVE_EDIT,
		TYPE_IO_TEX_FLUX_MASTER,
		TYPE_IO_CLIP,

		TYPE_IO_LAYERS_NEW,
		TYPE_IO_BDD_FEEDBACK,
		TYPE_IO_BDD_LINE,
		TYPE_IO_BDD_IMG_GRADIENT,
		TYPE_IO_CAP_CENSYS,

		TYPE_IO_CAP_TRICLOPS,
		TYPE_IO_BDD_POINT,
		TYPE_IO_SHADING,
		TYPE_IO_MODULE_NEW,
		TYPE_IO_BDD_IMG_PROC,

		TYPE_IO_LUA,
		TYPE_IO_LUA_EDITOR,
	//	TYPE_IO_BDD_FLUID,
	//	TYPE_IO_BDD_ARBRE,
		TYPE_IO_OPENCL,
		TYPE_IO_MOVIE_AVI,
		TYPE_IO_LANDMARK_DETECTOR,

		TYPE_IO_IMAGE_3D_WRITE,
		TYPE_IO_FBX,
		TYPE_IO_GLSL,

		TYPE_IO_MAX_NB
	};

	extern	C_PCHAR_C	get_filter_from_type_io(	TYPE_IO CONST type_io	);
//	extern	C_PCHAR_C	get_ext_from_type_io(		TYPE_IO CONST type_io	);

	extern	AAA_ERR		get_type_io_filename_load(	TYPE_IO CONST type_io,	o_str & filename,			system_window* parent	);
	extern	AAA_ERR		load_type_io(				TYPE_IO CONST type_io,	o_str CONST & filename,		c_obj_ui* obj			);
	extern	void		ask_type_io_load(			TYPE_IO CONST type_io,	c_obj_ui* obj = nullptr,	C_PCHAR_C filename_prev = nullptr );

	extern	AAA_ERR		get_type_io_filename_save(	TYPE_IO CONST type_io,	o_str & filename,			system_window* parent	);
	extern	void		save_type_io(				TYPE_IO CONST type_io,	o_str CONST & filename,		c_obj_ui* obj			);
	extern	void		ask_type_io_save(			TYPE_IO CONST type_io,	c_obj_ui* obj = nullptr,	C_PCHAR_C filename_prev = nullptr );

	extern	void		save_dialog(				TYPE_IO CONST type_io,	c_param* st					);

	extern	void		update();

	//todo clean this was quick and dirty
	extern	bool	b_dialog_load;
	extern	bool	b_dialog_save;
}	//namespace file
}	//namespace aaa



