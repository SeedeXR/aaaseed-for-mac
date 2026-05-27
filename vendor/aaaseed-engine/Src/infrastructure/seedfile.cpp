#include "aaa_def.h"
		 
#include "infrastructure/seedfile.h"
#include "infrastructure/aaa_env.h"

#include "draw/stereo.h"
#include "ui/file_dlg.h" 
#include "image/bind_img_2d.h"
#include "image/bind_img_3d.h"
#include "infrastructure/bind_text.h"
//		 
#include "ui/seedmenu.h" 
#include "draw/seedcam.h" 
//		 
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/layer/app.h"
#include "infrastructure/param/traxs.h"

#ifndef AAA_OURTRUETYPE_H
#	include "truetype/ourtruetype.h"
#endif

#include "obj_ui/bdd/bdd_geo/bdd_tube.h"
#include "obj_ui/bdd/bdd_geo/bdd_quak.h"
#include "draw/bspfile.h"
#include "draw/leveldata.h"
#include "obj_ui/bdd/bdd_geo/bdd_tri.h"

#include "obj_ui/bdd/bdd_point/bdd_point.h"
#include "obj_ui/bdd/bdd_edit/bdd_curve_edit.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/bdd/bdd_fbx.h"
#include "obj_ui/bdd/bdd_mesh_static.h"
#include "obj_ui/bdd/bdd_sound/bdd_snd_wave.h"
#include "obj_ui/bdd/bdd_old/lenticular.h"

#include "media/video/aaa_video.h"	 
#include "draw/tex_anim.h"
#include "draw/color.h"
#include "draw/lights.h" 
#include "draw/render.h"
#include "draw/stencil.h"
#include "media/video/tex_video.h"
#include "draw/fog.h"
#include "draw/model.h"
#include "obj_ui/deformer/def_node.h"

extern	void	bank_load_from_file(	o_str CONST & filename_in );
extern	void	bank_save_to_file(		o_str CONST & filename_in );

namespace aaa {
namespace file {

bool	gb_image_save_from_texture	= 0;
INT32	g_image_save_tex_bind		= 0;


//todo	this a double with the filter array
static CONSTEXPR C_PCHAR_C file_str[TYPE_IO_MAX_NB] =
	{
	"TYPE_IO None",

	"Texture",
	"Texture 3D",
	"Snapshot",
	"Height Map",
	"Particle Emission Mask",
	
	"Particle Emission Map",
	"Particle Color Map",
	"Tube Data",
	"Deformer",
	"Deformer List",
	
	"Bind List",
	"Camera",
	"Seed Global Pref",
	"Rendering",
	"Light Group",
	
	"Environment",	
	"Demo",
	"bsp",
	"fog",
	"color transfer",
	
	"Particle",
	"Nichimen Geo",
	"Tracker data",
	"Source Code",
	"Face param",
	
	"Anim param",
	"Image Proc param",
	"Lenticular Image",
	"3D Model",
	"Triangle Object",

	"Text 2D",
	"TRAXS",
	"Trax",
	"Array",
	"Layer",

	"Midi",
	"Cone",
	"Fn 3D",
	"Net",
	"Grid",

	"Motion Capture Object",
	"Motion Capture Data",
	"Sound Wave",
	"Material",
	"Circle",

	"Tube Path",
	"Joystick",
	"Lights Switch",
	"ISense",
	"Model",

	"Bdd Movie",
	"Movie",
	"Tube path",
	"Picking",
	"Sphere",

	"Torus",
	"Gypsy Actor",
	"Algo Maa",
	"Ship",
	"Image cpp",

	"Info",
	"Value",
	"Null Object",
	"Mapping",
	"Image Bank",

	"Boid",
	"Layer Selector",
	"Stencil",
	"Font",
	"Video",

	"Texture Video",
	"Bdd Boxes",
	"Multiple",
	"Color Modifier",
	"Comma Separated Values",

	"France Telecom Face",
	"Text",
	"Bdd Miguel 2003",
	"Point network 2003",
	"Adjustable grid",

	"Bdd Time Watcher",
	"Time Buffer Master",
	"Curve Edit",
	"Texture Flux Master",
	"Clip",

	"Layers",
	"FeedBack",
	"Line",
	"Image Gradient",
	"Censys 3d",

	"Triclops",
	"Bdd_point",
	"Shading",
	"Module",
	"Image Proc",

	"Lua",
	"Lua editor",
	"OpenCL",
	"AVI",
	//	"Tree",
	"LandMark Detector",

	"Volume 3D",
	"FBX",
	"Shader",
};

static o_str o_file_mess_open;
static o_str o_file_mess_save;

//	filter string
static CONSTEXPR CHAR filter_none[]					=	"All\0*.*\0\0";

static CONSTEXPR CHAR filter_img_in[]				=	"Reconnus\0*.bmp;*.gif;*.jpg;*.sgi;*.rgb;*.tga;*.tif;*.tiff;*.png;*.yuv;*.422;*.exr;*.hdr;*.dds\0"
														"Bmp\0*.bmp\0"
														"Gif\0*.gif\0"
														"Jpeg\0*.jpg\0"
														"RGB Sgi\0*.sgi;*.rgb\0"
														"Tga\0*.tga\0"
														"Tiff\0*.tif;*.tiff\0"
														"Png\0*.png\0"
														"EXR\0*.exr\0"
														"HDR\0*.hdr\0"
														"Yuv/422\0*.yuv;*.422\0"
														"DDS\0*.dds\0"
														"All\0*.*\0"
														"\0";
static CONSTEXPR CHAR filter_img_out[]				=	"Targa\0*.tga\0Png\0*.png\0JPEG\0*.jpg\0TIFF\0*.tif\0EXR\0*.exr\0\0"; //	filter string
//static CONSTEXPR CHAR filter_tex3d[]				=	"Vtk\0*.vtk\0Pvm\0*.pvm\0Raw\0*.raw\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_tex3d[]				=	"Reconnus\0*.vtk;*.pvm;*.raw\0"
														"Vtk\0*.vtk\0"
														"Pvm\0*.pvm\0"
														"DirectX\0*.dds\0"
														"Raw\0*.raw\0"
														"All\0*.*\0"
														"\0";
static CONSTEXPR CHAR filter_bdd_tube[]				=	"Tube\0*.tub\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_def[]					=	"Deformer\0*.def\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_dfl[]					=	"Deformer List\0*.dfl\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bind[]					=	"Bind List\0*.bind\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_cam[]					=	"Camera\0*.cam\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_app[]					=	"Seed Global Pref\0*.app\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_ren[]					=	"Renderingt\0*.ren\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_lightg[]				=	"Light Group\0*.lightg\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_env[]					=	"Environment\0*.app\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_demo[]					=	"Demo\0*.demo\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bsp[]					=	"Quake Data\0*.bsp\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_fog[]					=	"Fog\0*.fog\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_color[]				=	"Colt\0*.colt\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_part[]				=	"Particle\0*.part\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_geo[]					=	"Nichimen Geo\0*.geo\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_tracker[]				=	"Tracker data\0*.dat\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_cpp[]					=	"C++\0*.cpp\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_face[]				=	"Face\0*.face\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_anim[]					=	"Anim\0*.anim\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_proc[]					=	"Proc\0*.proc\0All\0*.*\0\0";
//todo can't deal with model first
static CONSTEXPR CHAR filter_obj_wavefront[]		=	"Alias Wavefront Obj\0*.obj\0Model\0*.obj;*.geo;\0Nichimen Geo\0*.geo\0SVG\0*.svg\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_tri[]				=	"Triangle Object\0*.tri\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_text_2d[]			=	"Text 2d\0*.tex2d\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_traxs[]				=	"TRAXS\0*.trax\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_trax[]					=	"Trax\0*.trax\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_array[]			=	"Array\0*.array\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_layer[]				=	"Layer\0*.layer\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_midi[]					=	"MidiPref\0*.midipref\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_cone[]				=	"Cone\0*.cone\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_fn3d[]					=	"Fn 3D\0*.fn3d\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_net[]					=	"Net\0*.net\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_grid[]				=	"Grid\0*.grid\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_mocap[]			=	"Motion Capture Object\0*.mocap\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_mocap_data[]		=	"All\0*.*\0TRC\0*.trc\0Softimage HRC\0*.hrc\0Gypsy BVH\0*.bvh\0\0";
static CONSTEXPR CHAR filter_snd_wave[]				=	"Wave file\0*.wav\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_material[]				=	"Material\0*.mat\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_circle[]			=	"Circle\0*.circle\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_tube_path[]		=	"Tube path\0*.tube_path\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_joy[]					=	"Joystick\0*.joy\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_lights_switch[]		=	"Lights Switch\0*.lights_switch\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_isense[]				=	"ISense\0*.isense\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_model[]				=	"Model\0*.model\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_movie[]			=	"Bdd Movie\0*.movie\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_movie[]				=	"Movie\0*.avi;*.mpg;*.mpeg;*.mov;*.qt\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_proj_cone[]		=	"Projection cone\0*.bdd_proj_cone\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_picking[]				=	"Picking\0*.picking\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_sphere[]			=	"Sphere\0*.sphere\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_torus[]			=	"Torus\0*.torus\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_mocap_actor[]		=	"GypsyActor\0*.gyp\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_algo_maa[]			=	"Algo Maa\0*.algo_maa\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_ship[]					=	"Ship\0*.ship\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_image_cpp[]			=	"Image cpp\0*.cpp\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_info[]					=	"Infop\0*.info\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_value[]				=	"Value\0*.values\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_null[]				=	"Null Object\0*.null\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_map[]					=	"Mapping\0*.map\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bank[]					=	"Image Bank\0*.txt\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_boid[]					=	"Boid\0*.boid\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_layer_selector[]		=	"Layer Selector\0*.layer_selector\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_stencil[]				=	"Stencil\0*.stencil\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_font[]					=	"Font\0*.ttf;*.otf\0Sdf\0*.json\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_video[]				=	"Avi\0*.*\0Movie\0*.avi;*.mpg;*.mpeg;*.mov;*.qt;*.wmv;*.mp4\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_tex_video[]			=	"Texture Video\0*.tex_video\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_boxes[]			=	"Bdd Boxes\0*.bdd_boxes\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_multiple[]				=	"Multiple\0*.multiple\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_color_mod[]			=	"Color Modifier\0*.color_mod\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_csv[]					=	"Comma Separated Values (.csv)\0*.csv\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_ft_face[]			=	"France Telecom Face\0*.ft_face\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_txt[]					=	"Text\0*.txt\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_miguel_2003nb[]	=	"Bdd Miguel 2003 NuitBlanche\0*.bdd_miguel_2003nb\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_point_network_2003nb[]	=	"Point network 2003 NB\0*.bdd_point_network_2003nb\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_grid_adjustable[]	=	"Adjustable Grid\0*.grid_adjustable\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_time_watcher[]		=	"Bdd Time Watcher\0*.bdd_time_watcher\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_time_buffer_master[]	=	"Time Buffer Master\0*.time_buffer_master\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_curve_edit[]		=	"Curve edit\0*.bdd_curve_edit\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_tex_flux_master[]		=	"Texture Flux Master\0*.tex_flux_master\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_clip[]					=	"Clip\0*.clip\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_layers_new[]			=	"Layers\0*.layers\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_feedback[]			=	"Bdd Feedback\0*.feedback\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_line[]				=	"Bdd Line\0*.bdd_line\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_img_gradient[]		=	"Image Gradient\0*.image_gradient\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_censys[]				=	"Capture Censys 3d\0*.cap_censys\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_triclops[]				=	"Capture Triclops\0*.cap_triclops\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_point[]			=	"Bdd Point\0*.bdd_point\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_module[]				=	"Module\0*.module\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_shading[]				=	"Shading\0*.shading\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_bdd_img_proc[]			=	"Image Proc\0*.image_proc\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_lua[]					=	"Lua script\0*.lua\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_lua_editor[]			=	"Lua editor\0*.exe\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_opencl[]				=	"OpenCL code\0*.cl\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_avi[]					=	"AVI Movie\0*.avi\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_landmark_detector[]	=	"LandMark Detector\0*.dat\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_image_3d[]				=	"LandMark Detector\0*.dat\0All\0*.*\0\0";	//todo relly fill; it 
static CONSTEXPR CHAR filter_fbx[]					=	"FBX\0*.fbx\0All\0*.*\0\0";
static CONSTEXPR CHAR filter_shader[]				=	"Shaders\0*.vert;*.geom;*.frag;*.comp;*.glsl\0"
														"Vertex\0*.vert\0"
														"Geometry\0*.geom\0"
														"Fragment\0*.frag\0"
														"Compute\0*.comp\0"
														"Glsl\0*.glsl\0"
														"All\0*.*\0"
														"\0";

static CONSTEXPR C_PCHAR_C filter_array[TYPE_IO_MAX_NB] =
	{
	filter_none,

	filter_img_in,
	filter_tex3d,
	filter_img_out,
	filter_img_in,
	filter_img_in,

	filter_img_in,
	filter_img_in,
	filter_bdd_tube,
	filter_def,
	filter_dfl,
	
	filter_bind,
	filter_cam,
	filter_app,
	filter_ren,
	filter_lightg,
	
	filter_env,
	filter_demo,
	filter_bsp,
	filter_fog,
	filter_color,
	
	filter_bdd_part,
	filter_geo,
	filter_tracker,
	filter_cpp,
	filter_bdd_face,
	
	filter_anim,
	filter_proc,
	filter_img_out,
	filter_obj_wavefront,
	filter_bdd_tri,

	filter_bdd_text_2d,
	filter_traxs,
	filter_trax,
	filter_bdd_array,
	filter_layer,

	filter_midi,
	filter_bdd_cone,
	filter_fn3d,
	filter_net,
	filter_bdd_grid,

	filter_bdd_mocap,
	filter_bdd_mocap_data,
	filter_snd_wave,
	filter_material,
	filter_bdd_circle,

	filter_bdd_tube_path,
	filter_joy,
	filter_lights_switch,
	filter_isense,
	filter_model,

	filter_bdd_movie,
	filter_movie,
	filter_bdd_proj_cone,
	filter_picking,
	filter_bdd_sphere,

	filter_bdd_torus,
	filter_bdd_mocap_actor,
	filter_bdd_algo_maa,
	filter_ship,
	filter_image_cpp,

	filter_info,
	filter_value,
	filter_bdd_null,
	filter_map,
	filter_bank,

	filter_boid,
	filter_layer_selector,
	filter_stencil,
	filter_font,
	filter_video,

	filter_tex_video,
	filter_bdd_boxes,
	filter_multiple,
	filter_color_mod,
	filter_csv,

	filter_bdd_ft_face,
	filter_txt,
	filter_bdd_miguel_2003nb,
	filter_point_network_2003nb,
	filter_bdd_grid_adjustable,

	filter_bdd_time_watcher,
	filter_time_buffer_master,
	filter_bdd_curve_edit,
	filter_tex_flux_master,
	filter_clip,

	filter_layers_new,
	filter_bdd_feedback,
	filter_bdd_line,
	filter_bdd_img_gradient,
	filter_censys,

	filter_triclops,
	filter_bdd_point,
	filter_shading,
	filter_module,
	filter_bdd_img_proc,

	filter_lua,
	filter_lua_editor,
//	filter_bdd_arbre,
	filter_opencl,
	filter_avi,
	filter_landmark_detector,

	filter_tex3d,				//todo refine
	filter_fbx,
	filter_shader,

	};

C_PCHAR_C get_filter_from_type_io( TYPE_IO CONST type_io )
{
	if( OUTSIDE( type_io, TYPE_IO(0),TYPE_IO_MAX_NB ) )
		return nullptr;
	return filter_array[type_io];
}

//C_PCHAR_C get_ext_from_type_io( TYPE_IO CONST type_io )
//{
//	if( OUTSIDE( type_io, TYPE_IO(0),TYPE_IO_MAX_NB ) )
//		return nullptr;
//	C_PCHAR	str = filter_array[type_io];
//	while( *str++ != 0 );
//	return str+2;
//}

#ifdef	WIN32
#	include <process.h>
#endif

		bool		b_dialog_load = false;
		bool		b_dialog_save = false;
static	bool		b_load_waiting = false;
static	c_obj_ui*	obj_loading;

static	bool		b_saving = false;
static	bool		b_save_waiting = false;
static	c_obj_ui*	obj_saving;

static	o_str		filename_load;
static	o_str		filename_save;
static	TYPE_IO		type_io_load;
static	TYPE_IO		type_io_save;


static	bool		b_dialog_save_waiting = false;
static	p_param		param_dialog_save = nullptr;
static	o_str		filename_save_dialog;
static	TYPE_IO		type_io_save_dialog;

static	c_img_2d*	img_snap;

void
#ifdef	WIN32
	__cdecl 
#endif
th_get_type_io_filename_load( void *dummy )
{
	if( NOERR( get_type_io_filename_load( type_io_load, filename_load, nullptr ) ) )
		b_load_waiting = true;	//	we go anyway to the real load in case of cleanup
	b_dialog_load = false;
	//	_endthread is implied
	//	_endthread();
}
 
static void
#ifdef	WIN32
	__cdecl 
#endif
th_get_type_io_filename_save( void* dummy )
{
	if( NOERR( get_type_io_filename_save( type_io_save, filename_save, nullptr ) ) )
		b_save_waiting = true;	//	we go anyway to the real save in case of cleanup
	b_saving = false;
	//	_endthread is implied
	//	_endthread();
}

static void
#ifdef	WIN32
	__cdecl 
#endif
th_get_filename_save_dialog( void* dummy )
{
	if( NOERR( get_type_io_filename_save( type_io_save_dialog, filename_save_dialog, nullptr ) ) )
		b_dialog_save_waiting = true;	//	we go anyway to the real save in case of cleanup
	b_dialog_save = false;
	//	_endthread is implied
	//	_endthread();
}

void	aaa::file::ask_type_io_load( TYPE_IO CONST type_io, c_obj_ui* obj, C_PCHAR_C filename_prev )
{
	if( !b_dialog_load && !b_load_waiting )
	{
		type_io_load = type_io;
		b_dialog_load = true;
		obj_loading = obj;
		filename_load.set( filename_prev );
#ifdef	WIN32
			c_thread::begin( "get_type_io_filename_load", th_get_type_io_filename_load );
#else
			th_get_type_io_filename_load( nullptr );
#endif
	}
	else
		BOX_WAR( "There is still a load going on !\nFind the dialog." );
}

void	ask_type_io_save( TYPE_IO CONST type_io, c_obj_ui* obj, C_PCHAR_C filename_prev )
{
	if( !b_saving && !b_save_waiting )
	{
		type_io_save = type_io;
		b_saving = true;
		obj_saving = obj;
		filename_save.set( filename_prev );
		if( type_io == TYPE_IO_SNAPSHOT )
		{
			img_snap = get_img_frame_buffer( g_stereo->get_gl_buffer_displayed() );
			DBG_HEAP_CHECK();
		}
#ifdef	WIN32
			c_thread::begin( "get_type_io_filename_save", th_get_type_io_filename_save );
#else
			th_get_type_io_filename_save( nullptr );
#endif
	}
	else
		BOX_WAR( "There is still a save going on !\nFind the dialog." );
}

void	update()
{
	if( b_load_waiting )
	{
		load_type_io( type_io_load, filename_load, obj_loading );
		b_load_waiting = false;
		obj_loading = nullptr;
	}
#if	!AAA_DEMO_LOCKED()
	if( b_save_waiting )
	{
		save_type_io( type_io_save, filename_save, obj_saving );
		b_save_waiting = false;
		obj_saving = nullptr;
	}
#endif
	if( b_dialog_save_waiting )
	{
		param_dialog_save->set_value_str( filename_save_dialog );
		b_dialog_save_waiting = false;
		param_dialog_save = nullptr;
	}
}

void	save_dialog( TYPE_IO CONST type_io, p_param param )
{
	if( !b_dialog_save && !b_dialog_save_waiting )
	{
		type_io_save_dialog = type_io;
		b_dialog_save = true;
		param_dialog_save = param;
#ifdef	WIN32
		c_thread::begin( "get_filename_save_dialog", th_get_filename_save_dialog );
#else
		th_get_filename_save_dialog( nullptr );
#endif
	}
	else
		BOX_WAR( "There is still a save dialog going on !\nFind the dialog." );
}


AAA_ERR get_type_io_filename_save( TYPE_IO CONST type_io, o_str & filename, system_window* parent )
{
	AAA_ERR err = ERR_OBJ_NULL;
	if( type_io >= 0 )
	{
		switch( type_io )
		{
		case TYPE_IO_LAYERS_NEW:
		case TYPE_IO_MODULE_NEW:
			{
				C_PCHAR_C fname = type_io==TYPE_IO_MODULE_NEW ? "default" : "fx";
				o_file_mess_save.set( "Choose a folder for new " );
				o_file_mess_save.add( file_str[type_io] );
				o_file_mess_save.add( ", it will be named \"" );
				o_file_mess_save.add( fname );
				o_file_mess_save.add( "\" in this folder" );
				err = get_folder( filename, o_file_mess_save.get(), parent );
				if( NOERR(err) )
				{
					filename.add_slash();
					filename.add( fname );
				}
			}
			break;
		default:
			o_file_mess_save.set( "Save " );
			o_file_mess_save.add( file_str[type_io] );
			err = get_filename_save( filename, o_file_mess_save.get(), parent, filter_array[type_io] );
			break;
		}
	}
	return err;
}

AAA_ERR	get_type_io_filename_load( TYPE_IO CONST type_io, o_str & filename, system_window* parent )
{
	if( type_io >= 0 )
	{
		o_file_mess_open.set( "Open " );
		o_file_mess_open.add( file_str[type_io] );
		//	multiple file only for texture now
		//todo check and extend this 
		return get_filename_open( filename, o_file_mess_open.get(), parent, filter_array[type_io], type_io==TYPE_IO_TEXTURE_2D );	
	}
	return ERR_OBJ_NULL;
}

AAA_ERR	load_type_io( TYPE_IO CONST type_io, o_str CONST & filename, c_obj_ui* obj )
{
	AAA_ERR retcode = AAA_OK;
	if( !filename.is_empty() )
	{
		c_dir::change_to_def();
		switch( type_io )
		{
#if	!AAA_DEMO()
		case TYPE_IO_TEXTURE_2D:
			g_bind_img_2d->load_texture( g_bind_img_2d->get_bind()->get_reset_index_for_next_load_save(), filename, true );
			break;
		case TYPE_IO_TEXTURE_3D:
			g_bind_img_3d->load_texture( g_bind_img_3d->get_bind()->get_reset_index_for_next_load_save(), filename, true );
			break;
		case TYPE_IO_FONT:
			{
				auto index = aaa::font::g_bank_def->get_reset_index_for_next_load_save();
				if( index > 0 )
					aaa::font::g_bank_def->set_index_cur( index );
			}	
			aaa::font::load_from_file( filename );
			break;
		case TYPE_IO_HEIGHT_MAP:
			if( c_layer::get_ui() )
				c_layer::get_ui()->bdd_get_always<c_bdd_tube>()->load_img_height( filename );
			break;

/*		case TYPE_IO_BDD_PART:
		case TYPE_IO_PART_EMISSION_MASK:
		case TYPE_IO_PART_EMISSION_IMAGE:
		case TYPE_IO_PART_COLOR_MAP:
			if ( !obj )
				if( c_layer::get_ui() )
					obj = c_layer::get_ui()->get_bdd_particle();
			if( obj )
				obj->load_data_from_filename( filename, type_io );
			break;
		case TYPE_IO_BDD_TUBE:
			if( c_layer::get_ui() )
				c_layer::get_ui()->get_bdd_tube()->load_from_file( filename );
			break;
*/
		case TYPE_IO_CSV:
			if( obj )
				obj->load_data_from_filename( filename, type_io );
			break;
		case TYPE_IO_TXT:
			if( obj )
				obj->load_data_from_filename( filename, type_io );
			break;
/*		case TYPE_IO_DEF:
			//todo could be done better ?
			deformer_ui->insert_from_file( filename );
			deformer_menu_update();
			ERR_PRINT_STRING( "load one deformer not implemented" );
			break;
*/
		case TYPE_IO_DFL:
			c_def_node::get_ui()->load_from_file( filename );
			c_menu::deformer_update();
			break;
		case TYPE_IO_BIND_LIST_IMG:
			g_bind_img_2d->get_bind()->load_from_file( filename );
			if( c_layer::get_ui() )
				bind_ui_set( c_layer::get_ui()->get_bind_2d() );
			break;
		//todocam
		case TYPE_IO_CAM:
			if( auto cam = c_seedcam::get_ui_or_find_unlock() )
				cam->load_from_file( filename	);
			break;
		case TYPE_IO_APP:
			g_app->load_from_file( filename );
			if( c_layer::get_ui() )
				bind_ui_set( c_layer::get_ui()->get_bind_2d() );
			reset_erased_screens();
			break;
		case TYPE_IO_RENDER:		c_render::get_ui()->load_from_file(	filename		);	break;
		case TYPE_IO_MAP:			c_map::get_ui()->load_from_file(	filename		);	break;
		case TYPE_IO_STENCIL:		c_stencil::ui->load_from_file(		filename		);	break;
		case TYPE_IO_TEX_VIDEO:		c_tex_video::ui->load_from_file(	filename		);	break;
		case TYPE_IO_BANK:			bank_load_from_file(				filename		);	break;
		case TYPE_IO_LIGHT_GROUP:	c_lights::get_ui()->load_from_file(	filename		);	break;
		case TYPE_IO_ENV:			aaa::env::load(						filename.get()	);	break;
		case TYPE_IO_DEMO:

#if	AAA_DEMO_INTERGRAPH()
			demo = new c_demo( filename );
#	if	!AAA_MENU_LOCKED()
			men::detach( mouse::MOUSE_BUTTON_RIGHT );
#	endif
#endif
			break;
		case TYPE_IO_BSP:
			destroyLevelData();
			if( openBSP( filename.get() ) == FAILURE )
				BOX_ERR( "Error opening bsp file." );
			else if ( bdd_quak_load_level_data() )
				BOX_ERR( "Error loading level." );
			closeBSP();
			break;
		case TYPE_IO_FOG:
			if( c_fog::ui )
				c_fog::ui->load_from_file( filename );
			break;
		case TYPE_IO_COLOR:
			c_color::ui->load_from_file( filename );
			break;
/*
		case TYPE_IO_BDD_FACE:
			if( c_layer::get_ui() )
				c_layer::get_ui()->get_bdd_face()->load_from_file( filename );
			break;
		case TYPE_IO_BDD_BOID:
			if( c_layer::get_ui() )
				c_layer::get_ui()->get_bdd_boid()->load_from_file( filename );
			break;
*/
#if	!AAA_WATCHDOG()
		case TYPE_IO_BDD_POINT :
			{
				c_bdd_point*	obj_bdd_point = nullptr;
				if( !obj_bdd_point && c_layer::get_ui() )
					obj_bdd_point = c_layer::get_ui()->bdd_get_always<c_bdd_point>();
				if( obj_bdd_point)
					obj_bdd_point->load_data_from_filename( filename, type_io);
			}
			break;

		case TYPE_IO_BDD_CURVE_EDIT :
			{
				c_bdd_curve_edit*	obj_bdd_curve_edit = nullptr;
				if( !obj_bdd_curve_edit && c_layer::get_ui() )
					obj_bdd_curve_edit = c_layer::get_ui()->bdd_get_always<c_bdd_curve_edit>();
				if( obj_bdd_curve_edit )
					obj_bdd_curve_edit->load_data_from_filename( filename, type_io );
			}
			break;
#endif	//AAA_WATCHDOG
		case TYPE_IO_TEX_ANIM:	c_tex_anim::ui->load_from_file( filename );		break;
//		case TYPE_IO_PROC:
//			img_proc_cur->load_from_file( filename );
//			break;
//		case TYPE_IO_FEEDBACK:
//			c_feedback::ui->load_from_file( filename );
//			break;
		case TYPE_IO_OBJ_WAVEFRONT:
		case TYPE_IO_BDD_TRI:
			if( !obj && c_layer::get_ui() )
				obj = c_layer::get_ui()->bdd_get_always<c_bdd_tri>();
			if( obj )
				obj->load_data_from_filename( filename, type_io );
			break;
		case TYPE_IO_FBX:
			if( !obj && c_layer::get_ui() )
			{
				obj = c_layer::get_ui()->bdd_get_always<c_bdd_fbx>();
				if( !obj )
					obj = c_layer::get_ui()->bdd_get_always<c_bdd_mesh_static>();
			}
			if( obj )
				obj->load_data_from_filename( filename, type_io );
			break;
		case TYPE_IO_BDD_MOCAP:
			if( c_layer::get_ui() )
				c_layer::get_ui()->bdd_get_always<c_bdd_mocap>()->load_from_file( filename );
			break;
#if	!AAA_WATCHDOG()
		case TYPE_IO_BDD_MOCAP_DATA:
		case TYPE_IO_BDD_MOCAP_ACTOR:
			if( !obj )
				obj = c_bdd_mocap::ui;
			obj->load_data_from_filename( filename, type_io );
			break;
#endif	//AAA_WATCHDOG
			/*		case TYPE_IO_BDD_TEXT_2D:
			if( c_layer::get_ui() )
				c_layer::get_ui()->get_bdd_text_2d()->load_from_file( filename );
			break;
		case TYPE_IO_BDD_ARRAY:
			if( c_layer::get_ui() )
				c_layer::get_ui()->get_bdd_array()->load_from_file( filename );
			break;
		case TYPE_IO_BDD_MOVIE:
			if( c_layer::get_ui() )
				c_layer::get_ui()->get_bdd_movie()->set_movie_filename( filename );
			break;
*/
		case TYPE_IO_TRAXS:
			c_module::get_ui()->get_traxs()->load_from_file( filename );
			break;
		case TYPE_IO_LAYER:
//todoqq	revive this
//			if( c_layer::get_ui() )
//				c_layer::get_ui()->load_from_file( filename, false );
			break;
/*
#if	AAA_TRACKER_MIDI()
		case TYPE_IO_MIDI:
			if( midi_cur )
				midi_cur->load_from_file( filename );
			break;
#endif
*/
		case TYPE_IO_SND_WAVE:
			if( c_layer::get_ui() )
				c_layer::get_ui()->bdd_get_always<c_bdd_snd_wave>()->set_wave_filename( filename );
			break;
		case TYPE_IO_LIGHTS_SWITCH:
			if( c_lights_switch::ui )
				c_lights_switch::ui->load_from_file( filename );
			break;
		case TYPE_IO_MODEL:
			if( c_layer::get_ui() )
				c_layer::get_ui()->get_model()->load_from_file( filename );
			break;
		case TYPE_IO_VIDEO:	video_set_bind_name( filename );	break;
		case TYPE_IO_LUA:
		case TYPE_IO_LUA_EDITOR:
			if( obj )
				obj->load_data_from_filename( filename, type_io );
			break;

/*		case TYPE_IO_BDD_ARBRE:
			if( c_layer::get_ui() )
				c_layer::get_ui()->get_bdd_arbre()->set_tree_filename( filename );
			break;
*/
#endif
		default:
			retcode = ERR_TYPE_UNUSED;
			BOX_ERR( "I don't deal with loading data type %d", type_io );
			break;
		}
	}
	return retcode;
}

#if	!AAA_DEMO_LOCKED()
//todo get rid of numbers in case
void	aaa::file::save_type_io( TYPE_IO CONST type_io, o_str CONST & filename, c_obj_ui* obj )
{
	if( !filename.is_empty() )
	{
		c_dir::change_to_def();
		switch( type_io )
		{
		case TYPE_IO_SNAPSHOT:	//todo done twice with dialog trig a crash ?
			img_snap->write( filename );
			break;
//stl
		case TYPE_IO_DEF:
			ERR_PRINT_STRING( "save one deformer not implemented");
			{
				c_deformer*	def = c_def_node::get_ui()->get_item_cur();
				def->save_to_file( filename );
			}
			break;
		case TYPE_IO_DFL:				c_def_node::get_ui()->save_to_file(				filename		);	break;
		case TYPE_IO_BIND_LIST_IMG:		g_bind_img_2d->get_bind()->save_to_file(		filename		);	break;
		//todocam attention crash on null
		case TYPE_IO_CAM:				if( auto cam = c_seedcam::get_ui_or_find() )
											cam->save_to_file(							filename		);	break;
		case TYPE_IO_APP:				g_app->save_to_file(							filename		);	break;
		case TYPE_IO_RENDER:			c_render::get_ui()->save_to_file(				filename		);	break;
		case TYPE_IO_MAP:				c_map::get_ui()->save_to_file(					filename		);	break;
		case TYPE_IO_STENCIL:			c_stencil::ui->save_to_file(					filename		);	break;
		case TYPE_IO_TEX_VIDEO:			c_tex_video::ui->save_to_file(					filename		);	break;
		case TYPE_IO_BANK:				bank_save_to_file(								filename		);	break;
		case TYPE_IO_LIGHT_GROUP:		c_lights::get_ui()->save_to_file(				filename		);	break;
		case TYPE_IO_ENV:				aaa::env::save(									filename.get()	);	break;
		case TYPE_IO_FOG:				if(c_fog::ui)
											c_fog::ui->save_to_file( filename );
										break;
		case TYPE_IO_COLOR:				c_color::ui->save_to_file(						filename		);	break;
		case TYPE_IO_TEX_ANIM:			c_tex_anim::ui->save_to_file(					filename		);	break;
//		case TYPE_IO_PROC:				img_proc_cur->save_to_file(						filename		);	break;
//		case TYPE_IO_FEEDBACK:			c_feedback::ui->save_to_file(					filename		);	break;
		case TYPE_IO_LENTICULAR_IMAGE:	if(c_lenti::cur)
											c_lenti::cur->save_image( filename );
										break;
/*
		case TYPE_IO_BDD_TUBE:			if( c_layer::get_ui() )		c_layer::get_ui()->get_bdd_tube()		->save_to_file( filename );	break;
		case TYPE_IO_BDD_PART:			if( c_layer::get_ui() )		c_layer::get_ui()->get_bdd_particle()	->save_to_file( filename );	break;
		case TYPE_IO_BDD_FACE:			if( c_layer::get_ui() )		c_layer::get_ui()->get_bdd_face()		->save_to_file( filename );	break;
		case TYPE_IO_BDD_BOID:			if( c_layer::get_ui() )		c_layer::get_ui()->get_bdd_boid()		->save_to_file( filename );	break;
		case TYPE_IO_BDD_TRI:			if( c_layer::get_ui() )		c_layer::get_ui()->get_bdd_tri()		->save_to_file( filename );	break;
		case TYPE_IO_BDD_MOCAP:			if( c_layer::get_ui() )		c_layer::get_ui()->get_bdd_mocap()		->save_to_file( filename );	break;
		case TYPE_IO_BDD_TEXT_2D:		if( c_layer::get_ui() )		c_layer::get_ui()->get_bdd_text_2d()	->save_to_file( filename );	break;
		case TYPE_IO_BDD_ARRAY:			if( c_layer::get_ui() )		c_layer::get_ui()->get_bdd_array()		->save_to_file( filename );	break;
*/
		case TYPE_IO_TRAXS:
			{	//todoqq check this
			//CHAR * ext;
			//ext = filename+strlen(filename);
			//if( str_is_equal_nocase( ext-5, ".trax", 7) )
			//	strcpy( ext, "00.trax");
				c_module::get_ui()->get_traxs()->save_to_file(								filename	);
			}
			break;
		case TYPE_IO_LAYER:				c_layer::get_ui()->save_to_file(					filename	);		break;
/*
#if	AAA_TRACKER_MIDI()
		case TYPE_IO_MIDI:
			if( midi_cur)
				midi_cur->save_to_file( filename );
			break;
#endif
*/
		case TYPE_IO_LIGHTS_SWITCH:		c_lights_switch::ui->save_to_file(					filename	);	break;
		case TYPE_IO_OBJ_WAVEFRONT:
			if( c_layer::get_ui() )
				c_layer::get_ui()->get_bdd()->save_obj_file(								filename	);
			break;
		case TYPE_IO_MODEL:				c_layer::get_ui()->get_model()->save_to_file(		filename	);	break;
		case TYPE_IO_IMAGE_CPP:			g_bind_img_2d->get_ui()->write(						filename,	c_img_utils::FILE_TYPE::CPP );	break;
		case TYPE_IO_IMAGE_3D_WRITE:	g_bind_img_3d->get_ui()->write(						filename	);	break;
		case TYPE_IO_LAYERS_NEW:		((c_module*)obj)->layers_new_from_file(				filename	);	break;
		case TYPE_IO_MODULE_NEW:		((c_modules*)obj)->module_new_from_file(			filename	);	break;
		default:
			BOX_ERR( "AAASeed don't deal yet with saving data type %d: %128s", type_io, filter_array[type_io] );
			break;
		}
	}
	//	cleanup
	switch( type_io )
	{
	case TYPE_IO_SNAPSHOT:
		SAFE_DELETE( img_snap );
		break;
	}
}
#endif	//!AAA_DEMO_LOCKED()

}	//namespace file
}	//namespace aaa




