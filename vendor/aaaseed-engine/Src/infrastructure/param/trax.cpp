#include "infrastructure/Data/aaa_global.h"
#include "infrastructure/param/trax.h"
#include "infrastructure/param/traxs.h"
//clean minimize
#include "strnum.h"
#include "image/bind_img_2d.h"
#include "draw/seeddraw.h"
#include "draw/box.h"
#include "image/convert/color_space.h"
#include "draw/seedcam.h"
#include "infrastructure/bind_text.h"
#include "infrastructure/flux_filter.h"
#include "infrastructure/aaa_const.h"
#include "infrastructure/data/datacube.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/modules.h"
#include "infrastructure/param/param_focus.h"
#include "infrastructure/param/param_declare.h"
#include "language/lua/aaalua_wrap.h"
#include "math/noisturb.h"
#include "math/rand.h"
#include "media/sound/snd_master.h"
#ifndef AAA_SND_INPUT_H
#	include "media/sound/snd_input.h"
#endif
#include "media/video/tex_video.h"
#include "obj_ui/multi_screen.h"
#include "obj_ui/bdd/bdd_edit/bdd_curve_edit.h"
#include "obj_ui/bdd/bdd_geo/bdd_tri.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/bdd/bdd_point/bdd_point.h"
#include "obj_ui/com/midi.h"
#include "obj_ui/com/midi_data.h"
#include "obj_ui/com/net.h"
#include "obj_ui/tracker/neat.h"
#include "obj_ui/tracker/toaster.h"
#include "time/aaa_date.h"
#include "ui/keyboard.h"
#include "ui/obj_value.h"
#include "ui/aaa_menu.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "ui/seedkey.h"
#include "infrastructure/averager.h"

FACTORY_CREATE_V1( c_trax, trax, Trax, trax );

struct	ST_TRAX_EVENT {
	REAL	timestamp;
	REAL	value;
};

REAL	trax_time;

namespace {

	//should change str_fn_type when this enum is changed
	enum FN_TYPE : INT32
	{
	//Player
		FN_PLAYER_RAW = 0,
		FN_PLAYER,
	//Fn
		FN_SINUS,
		FN_TRIANGLE,
		FN_TRIANGLE_INV,
		FN_TRIANGLE_UP_THEN_DOWN,
		FN_SQUARE,
		FN_MIN_TO_MAX_LINEAR,
		FN_MIN_TO_MAX_SINUS,
		FN_MAX_TO_MIN_LINEAR,
		FN_MAX_TO_MIN_SINUS,
		FN_RND_LIN,
		FN_RND_GAUSS,
		FN_TURBULENCE,
		FN_FRACTALSUM,
	//Math
		FN_MIN,
		FN_MAX,
		FN_MAX_WITH_DECAY,
		FN_COPY,
		FN_ADD,
		FN_AVERAGE,
		FN_INTERPOLATE,
		FN_MUL,
		FN_OVER_ONE,
		FN_LOG,
		FN_LOG10,
		FN_EXP,
		FN_POWER_BY_FREQ,
		FN_POWER_OF_FREQ,
	//Multiple
		FN_MULTIPLE_INDEX,
		FN_MULTIPLE_REAL,
		FN_MULTIPLE_NB,
		FN_MULTIPLE_INDEX_U,
		FN_MULTIPLE_REAL_U,
		FN_MULTIPLE_NB_U,
		FN_MULTIPLE_INDEX_V,
		FN_MULTIPLE_REAL_V,
		FN_MULTIPLE_NB_V,
		FN_MULTIPLE_INDEX_W,
		FN_MULTIPLE_REAL_W,
		FN_MULTIPLE_NB_W,
		FN_MULTIPLE_PARAMETER,
	//Test
		FN_EQUAL,
		FN_NOT_EQUAL,
		FN_EQUAL_ZERO,
		FN_NOT_EQUAL_ZERO,
		FN_EQUAL_ONE,
		FN_NOT_EQUAL_ONE,
		FN_EQUAL_FREQ,
		FN_NOT_EQUAL_FREQ,
		FN_MORE_THAN_FREQ,
		FN_MORE_EQUAL_THAN_FREQ,
		FN_LESS_THAN_FREQ,
		FN_LESS_EQUAL_THAN_FREQ,
		FN_INSIDE_FREQ_PHASE,
		FN_OUTSIDE_FREQ_PHASE,
	//Test Real
		FN_INSIDE_FREQ_PHASE_LINEAR,
		FN_INSIDE_FREQ_PHASE_SINUS,
		FN_PROXIMITY_FREQ_LINEAR,
		FN_PROXIMITY_FREQ_SINUS,
	//Change
		FN_CHANGE_LINEAR,
		FN_CHANGE_SINUS,
		FN_IN_IS_CHANGED,
	//Data
		FN_CONTROL_SET,
		FN_CONTROL,
		FN_VARIABLE_SET,
		FN_VARIABLE,
	//	FN_VARIABLE_STRING,
	//Curve
		FN_CURVE,
		FN_CURVE_BY_NAME,
		FN_CURVE_FROM_PHASE,
		FN_CURVE_X,
		FN_CURVE_Y,
		FN_CURVE_Z,
		FN_CURVE_X_FROM_PHASE,
		FN_CURVE_Y_FROM_PHASE,
		FN_CURVE_Z_FROM_PHASE,
	//Value
		FN_VALUE,
		FN_VALUE_MODULE,
		FN_VALUE_GLOBAL,
		FN_SET_VALUE,
		FN_SET_VALUE_MODULE,
		FN_SET_VALUE_GLOBAL,
		FN_MORE_THAN_VALUE_INDEX,
		FN_MORE_THAN_VALUE_MODULE_INDEX,
		FN_MORE_THAN_VALUE_GLOBAL_INDEX,
		FN_SORT_VALUE_INDEX_FROM_MIN,
		FN_SORT_VALUE_INDEX_FROM_MAX,
		FN_SORT_VALUE_MODULE_INDEX_FROM_MIN,
		FN_SORT_VALUE_GLOBAL_INDEX_FROM_MIN,
		FN_SORT_VALUE_MODULE_INDEX_FROM_MAX,
		FN_SORT_VALUE_GLOBAL_INDEX_FROM_MAX,
	//Trax
		FN_TRAX,
	//Input
		FN_MIDI_CONTROL,
		FN_MIDI_CONTROL_NO_OFFSET,
		FN_MIDI_CONTROL_REL,
		FN_MIDI_VELOCITY,
		FN_NEAT,
		FN_TOASTER,
		FN_NET_IN,
		FN_FFT_LINEAR,
		FN_FFT,
		FN_BEAT,
	//Keyboard and Mouse
		FN_SLIDE_INDEX,
		FN_ALPHABET,
		FN_ALPHABET_LAST,
		FN_ALPHABET_ONE,
		FN_ASCII,
		FN_ASCII_EQUAL_CONTROL,
		FN_MOUSE_X,
		FN_MOUSE_Y,
		FN_MOUSE_CLICK_LEFT,
		FN_MOUSE_CLICK_MIDDLE,
		FN_MOUSE_CLICK_RIGHT,
		FN_KEYBOARD_SHIFT,
		FN_KEYBOARD_ALT,
		FN_KEYBOARD_CTRL,
	//File
		FN_FILE_IS_CHANGED,
		FN_FILE,
		FN_FILE_STRING_LEN,
		FN_FILE_LETTER,
	//Time
		FN_TIME,
		FN_SECOND,
		FN_MINUTE,
		FN_HOUR,
		FN_DAY,
		FN_MONTH,
		FN_YEAR,
		FN_DAY_OF_WEEK,
		FN_DAY_OF_YEAR,
	//Image
		FN_IMAGE_RED,
		FN_IMAGE_GREEN,
		FN_IMAGE_BLUE,
		FN_IMAGE_ALPHA,
		FN_IMAGE_GREY,
		FN_IMAGE_SIZE_X,
		FN_IMAGE_SIZE_Y,
		FN_IMAGE_SIZE_RATIO,
	//Video
		FN_VIDEO_RED,
		FN_VIDEO_GREEN,
		FN_VIDEO_BLUE,
		FN_VIDEO_ALPHA,
		FN_VIDEO_GREY,
		FN_VIDEO_CUR_IMAGE_INDEX,
		FN_VIDEO_CUR_IMAGE_CHANGED,
	//Text
		FN_TEXT,
		FN_TEXT_LEN,
		FN_TEXT_LINE_NB,
	//Bdd Tri
		FN_BDD_TRI_NAME,
	//Net host Id
		FN_NET_HOST_ID,
		FN_NET_HOST_ID_EQUAL_FREQ,
		FN_NET_HOST_ID_NOT_EQUAL_FREQ,
		FN_NET_HOST_ID_MORE_THAN_FREQ,
		FN_NET_HOST_ID_MORE_EQUAL_THAN_FREQ,
		FN_NET_HOST_ID_LESS_THAN_FREQ,
		FN_NET_HOST_ID_LESS_EQUAL_THAN_FREQ,
		FN_MACHINE,
		FN_LANGROUP,
		FN_MACHINE_EQUAL_NAME,
		FN_LANGROUP_EQUAL_NAME,
	//Break
		FN_BREAK_EQUAL,
		FN_BREAK_NOT_EQUAL,
		FN_BREAK_EQUAL_ZERO,
		FN_BREAK_NOT_EQUAL_ZERO,
		FN_BREAK_EQUAL_ONE,
		FN_BREAK_NOT_EQUAL_ONE,
		FN_BREAK_EQUAL_FREQ,
		FN_BREAK_NOT_EQUAL_FREQ,
		FN_BREAK_MORE_THAN_FREQ,
		FN_BREAK_MORE_EQUAL_THAN_FREQ,
		FN_BREAK_LESS_THAN_FREQ,
		FN_BREAK_LESS_EQUAL_THAN_FREQ,
		FN_BREAK_INSIDE_FREQ_PHASE,
		FN_BREAK_OUTSIDE_FREQ_PHASE,
	//Screen
		FN_SCREEN_INDEX,
		FN_SCREEN_X_INDEX,
		FN_SCREEN_Y_INDEX,
		FN_SCREEN_NB,
		FN_SCREEN_X_NB,
		FN_SCREEN_Y_NB,
	//Dangerous
	//	FN_INC,
	//	FN_DEC,
		FN_COPY_BLOCK_REAL,
	//Experimental
		FN_LUA,
		FN_CELL_SIZE,
	//3D Distance
		FN_3D_DISTANCE,
		FN_3D_DISTANCE_MOCAP,
		FN_3D_DISTANCE_CAMERA,
	//3D Fn
		FN_3D_RND_LIN,
		FN_3D_RND_GAUSS,
	//3D Math
		FN_3D_COPY,
		FN_3D_ADD,
		FN_3D_AVERAGE,
		FN_3D_INTERPOLATE,
	//3D Change
		FN_3D_CHANGE_LINEAR,
		FN_3D_CHANGE_SINUS,
	//3D Data
		FN_3D_CONTROL,
		FN_3D_CONTROL_SET,
	//3D Curve
		FN_3D_CURVE,
		FN_3D_CURVE_FROM_PHASE,
	//3D Value
		FN_3D_VALUE,
		FN_3D_VALUE_MODULE,
		FN_3D_VALUE_GLOBAL,
	//3D Mocap
		FN_3D_PATH_POS,
		FN_3D_PATH_TO_WORLD,
		FN_3D_PATH_SCA,
		FN_3D_MOCAP,
		FN_3D_MOCAP_AXE_X,
		FN_3D_MOCAP_AXE_Y,
		FN_3D_MOCAP_AXE_Z,
		FN_3D_MOCAP_TGN,
		FN_3D_MOCAP_SCA,
		FN_3D_MOCAP_FEED,
	//3D Image
		FN_3D_RGB_TO_HSV,
		FN_3D_HSV_TO_RGB,
		FN_3D_IMAGE_COLOR,
		FN_3D_IMAGE_COLOR_CLAMPED,
	//3D Video
		FN_3D_VIDEO_COLOR,
		FN_3D_VIDEO_COLOR_CLAMPED,
	//3D Bdd Tri
		FN_3D_BDD_TRI_CENTER,
		FN_3D_BDD_TRI_BARYCENTER,
		FN_3D_BDD_TRI_MIN,
		FN_3D_BDD_TRI_MAX,
		FN_3D_BDD_TRI_SIZE,
		FN_3D_BDD_TRI_TEX_CENTER,
		FN_3D_BDD_TRI_TEX_SIZE,
	//	FN_3D_VARIABLE,
	//	FN_3D_VARIABLE_SET,
	//3D Experimental
		FN_3D_MULTIPLE_POS,
		FN_3D_MULTIPLE_SIZE,
	//	FN_3D_BDD_POINT_NB,
		FN_3D_BDD_POINT_AVERAGE,
		FN_3D_BDD_POINT_RAW,
		FN_3D_BDD_INTERSECTION,
		FN_3D_2_POINTS_TO_YX_ROT,
		FN_3D_CELL_POS,
		FN_TYPE_NB_MAX
	};

	INT32 CONST	INDEX_3D_FIRST	= FN_3D_RND_LIN;
	INT32 CONST	INDEX_3D_LAST	= FN_3D_CELL_POS;

	enum OUTPUT_TYPE : INT32
	{
		OUTPUT_RECORDER = 0,
		OUTPUT_FN_THRESHOLD,
		OUTPUT_FN_TRIG_UP_AND_DOWN,
		OUTPUT_FN_TRIG_DOWN,
		OUTPUT_FN_TRIG_UP,
		OUTPUT_FN,
		OUTPUT_MIN,
		OUTPUT_MIDDLE,
		OUTPUT_MAX,
		OUTPUT_IN,
		OUTPUT_TYPE_NB_MAX
	};

	static	C_PCHAR_C	str_output_type[OUTPUT_TYPE_NB_MAX] =
	{
		"RECORDER_FN",
		"FN_THRESHOLD",
		"FN_TRIGGER_UP_AND_DOWN",
		"FN_TRIGGER_DOWN",
		"FN_TRIGGER_UP",
		"FN",
		"MIN",
		"MIDDLE",
		"MAX",
		"IN"
	};

	static	C_PCHAR_C	str_output_type_short[OUTPUT_TYPE_NB_MAX] =
	{
		"RECORDER",
		"THRESHOLD",
		"TRIG_UP_DOWN",
		"TRIG_DOWN",
		"TRIG_UP",
		"",
		"MIN",
		"MIDDLE",
		"MAX",
		"IN"
	};

	INT32 CONST	FN_TYPE_GROUP_NB = 36;
	INT32		type_sub_menu[FN_TYPE_GROUP_NB];

	C_PCHAR_C	str_fn_type_group[FN_TYPE_GROUP_NB] =
	{
		"Player",
		"Fn",
		"Math",
		"Multiple",
		"Test",

		"Test Real",
		"Change",
		"Data",
		"Curve",
		"Value",

		"Trax",
		"Input",
		"Keyboard Mouse",
		"File",
		"Time",

		"Image",
		"Video",
		"Text",
		"Bdd Tri",
		"Net Host Id",

		"Break",
		"Screen",
		"Dangerous",
		"Experimental",
		"3D Distance 1D",

		"3D Fn",
		"3D Math",
		"3D Change",
		"3D Data",
		"3D Curve",

		"3D Value",
		"3D Mocap",
		"3D Image",
		"3D Video",
		"3D Bdd Tri"
		,
		"3D Experimental",
	};

	INT32	fn_type_group_item_nb[FN_TYPE_GROUP_NB] =
	{
		FN_PLAYER,
		FN_FRACTALSUM,
		FN_POWER_OF_FREQ,
		FN_MULTIPLE_PARAMETER,
		FN_OUTSIDE_FREQ_PHASE,

		FN_PROXIMITY_FREQ_SINUS,
		FN_IN_IS_CHANGED,
		FN_VARIABLE,
		FN_CURVE_Z_FROM_PHASE,
		FN_SORT_VALUE_GLOBAL_INDEX_FROM_MAX,

		FN_TRAX,
		FN_BEAT,
		FN_KEYBOARD_CTRL,
		FN_FILE_LETTER,
		FN_DAY_OF_YEAR,

		FN_IMAGE_SIZE_RATIO,
		FN_VIDEO_CUR_IMAGE_CHANGED,
		FN_TEXT_LINE_NB,
		FN_BDD_TRI_NAME,
		FN_LANGROUP_EQUAL_NAME,

		FN_BREAK_OUTSIDE_FREQ_PHASE,
		FN_SCREEN_Y_NB,
		FN_COPY_BLOCK_REAL,
		FN_CELL_SIZE,
		FN_3D_DISTANCE_CAMERA,

		FN_3D_RND_GAUSS,
		FN_3D_INTERPOLATE,
		FN_3D_CHANGE_SINUS,
		FN_3D_CONTROL_SET,
		FN_3D_CURVE_FROM_PHASE,
		FN_3D_VALUE_GLOBAL,

		FN_3D_MOCAP_FEED,
		FN_3D_IMAGE_COLOR_CLAMPED,
		FN_3D_VIDEO_COLOR_CLAMPED,
		FN_3D_BDD_TRI_TEX_SIZE,
		FN_3D_CELL_POS,
	};

	C_PCHAR_C	str_fn_type[FN_TYPE_NB_MAX] =
	{
	//Player
		"PLAYER_RAW",		//	Dec99 was 0
		"PLAYER",			//	Dec99 was 1
	//Fn
		"SINUS",			//	Dec99 was 2
		"TRIANGLE",			//	Dec99 was 3
		"TRIANGLE_INV",		//	Dec99 was 4
		"TRIANGLE_UP_THEN_DOWN",
		"SQUARE",			//	Dec99 was 5
		"min_TO_MAX_LINEAR",
		"min_TO_MAX_SINUS",
		"MAX_TO_min_LINEAR",
		"MAX_TO_min_SINUS",
		"RANDOM_LINEAR",	//	Dec99 was 14
		"RANDOM_GAUSS",		//	Dec99 was 15
		"TURBULENCE",
		"FRACTAL SUM",
	//Math
		"MIN",
		"MAX",
		"MAX_WITH_DECAY",
		"COPY",				//	Dec99 was 12
		"ADD",				//	Dec99 was 16	
		"AVERAGE",			//	Dec99 was 17
		"INTERPOLATE",
		"MUL",				//	Dec99 was 18
		"OVER ONE",
		"LOGARITHM",
		"LOGARITHM 10",
		"EXPONENTIAL",
		"POWER BY FREQ",
		"POWER OF FREQ",
	//Multiple
		"MULTIPLE_INDEX",
		"MULTIPLE_REAL",
		"MULTIPLE_NB",
		"MULTIPLE_INDEX_U",
		"MULTIPLE_REAL_U",
		"MULTIPLE_NB_U",
		"MULTIPLE_INDEX_V",
		"MULTIPLE_REAL_V",
		"MULTIPLE_NB_V",
		"MULTIPLE_INDEX_W",
		"MULTIPLE_REAL_W",
		"MULTIPLE_NB_W",
		"MULTIPLE_PARAMETER",
	//Test
		"EQUAL",
		"NOT_EQUAL",
		"EQUAL_ZERO",
		"NOT_EQUAL_ZERO",
		"EQUAL_ONE",
		"NOT_EQUAL_ONE",
		"EQUAL_FREQ",
		"NOT_EQUAL_FREQ",
		"MORE_THAN_FREQ",
		"MORE_EQUAL_THAN_FREQ",
		"LESS_THAN_FREQ",
		"LESS_EQUAL_THAN_FREQ",
		"INSIDE_FREQ_AND_PHASE",
		"OUTSIDE_FREQ_AND_PHASE",
	//Test Real
		"INSIDE_FREQ_PHASE_LINEAR",
		"INSIDE_FREQ_PHASE_SINUS",
		"PROXIMITY_FREQ_LINEAR",
		"PROXIMITY_FREQ_SINUS",
	//Change
		"CHANGE_LINEAR",
		"CHANGE_SINUS",
		"IN_IS_CHANGED",
	//Data
		"CONTROL_SET",
		"CONTROL",
		"NAME_REAL_SET",
		"NAME_REAL",
	//	"NAME_STRING",
	//Curve
		"CURVE",
		"CURVE_BY_NAME",
		"CURVE_FROM_PHASE",
		"CURVE_X",
		"CURVE_Y",
		"CURVE_Z",
		"CURVE_X_FROM_PHASE",
		"CURVE_Y_FROM_PHASE",
		"CURVE_Z_FROM_PHASE",
	//Value
		"VALUE",
		"VALUE_MODULE",
		"VALUE_GLOBAL",
		"SET_VALUE",
		"SET_VALUE_MODULE",
		"SET_VALUE_GLOBAL",
		"MORE_THAN_VALUE_INDEX",
		"MORE_THAN_VALUE_MODULE_INDEX",
		"MORE_THAN_VALUE_GLOBAL_INDEX",
		"SORT_VALUE_INDEX_FROM_MIN",
		"SORT_VALUE_INDEX_FROM_MAX",
		"SORT_VALUE_MODULE_INDEX_FROM_MIN",
		"SORT_VALUE_GLOBAL_INDEX_FROM_MIN",
		"SORT_VALUE_MODULE_INDEX_FROM_MAX",
		"SORT_VALUE_GLOBAL_INDEX_FROM_MAX",
	//Trax
		"TRAX",
	//Input
		"MIDI CONTROLLER",	//	Dec99 was 6
		"MIDI CONTROLLER NO OFFSET",
		"MIDI CONTROLLER RELATIVE",
		"MIDI VELOCITY",	//	Dec99 was 19
		"NEAT",				//	Dec99 was 7
		"TOASTER",
		"NET_IN",			//	Dec99 was 8
		"FFT_LINEAR",
		"FFT",				//	Dec99 was 11
		"BEAT",
	//Keyboard Mouse
		"SLIDE_INDEX",
		"ALPHABET",
		"ALPHABET_LAST",
		"ALPHABET_ONE",
		"ASCII",
		"ASCII_EQUAL_CONTROL",
		"MOUSE_X",
		"MOUSE_Y",
		"MOUSE_CLICK_LEFT",
		"MOUSE_CLICK_MIDDLE",
		"MOUSE_CLICK_RIGHT",
		"KEYBOARD_SHIFT",
		"KEYBOARD_ALT",
		"KEYBOARD_CTRL",
	//File
		"FILE_IS_CHANGED",
		"FILE",				//	Dec99 was 9
		"FILE_STRING_LEN",
		"FILE_LETTER",
	//Time
		"TIME",
		"SECOND",
		"MINUTE",
		"HOUR",
		"DAY",
		"MONTH",
		"YEAR",
		"DAY_OF_WEEK",
		"DAY_OF_YEAR",
	//Image
		"IMAGE RED",
		"IMAGE GREEN",
		"IMAGE BLUE",
		"IMAGE ALPHA",
		"IMAGE GREY",
		"IMAGE SIZE X",
		"IMAGE SIZE Y",
		"IMAGE SIZE RATIO",
	//Video
		"VIDEO RED",
		"VIDEO GREEN",
		"VIDEO BLUE",
		"VIDEO ALPHA",
		"VIDEO GREY",
		"VIDEO_IMAGE_INDEX",
		"VIDEO_IMAGE_CHANGED",
	//Text
		"TEXT",
		"TEXT_LEN",
		"TEXT_LINE_NB",
	//Bdd Tri
		"BDD_TRI_NAME",
	//Net host Id
		"NET_HOST_ID",
		"NET_HOST_ID_EQUAL_FREQ",
		"NET_HOST_ID_NOT_EQUAL_FREQ",
		"NET_HOST_ID_MORE_THAN_FREQ",
		"NET_HOST_ID_MORE_EQUAL_THAN_FREQ",
		"NET_HOST_ID_LESS_THAN_FREQ",
		"NET_HOST_ID_LESS_EQUAL_THAN_FREQ",
		"MACHINE",
		"LANGROUP",
		"MACHINE_EQUAL_NAME",
		"LANGROUP_EQUAL_NAME",
	//Test
		"BREAK_EQUAL",
		"BREAK_NOT_EQUAL",
		"BREAK_EQUAL_ZERO",
		"BREAK_NOT_EQUAL_ZERO",
		"BREAK_EQUAL_ONE",
		"BREAK_NOT_EQUAL_ONE",
		"BREAK_EQUAL_FREQ",
		"BREAK_NOT_EQUAL_FREQ",
		"BREAK_MORE_THAN_FREQ",
		"BREAK_MORE_EQUAL_THAN_FREQ",
		"BREAK_LESS_THAN_FREQ",
		"BREAK_LESS_EQUAL_THAN_FREQ",
		"BREAK_INSIDE_FREQ_AND_PHASE",
		"BREAK_OUTSIDE_FREQ_AND_PHASE",
	//Screen
		"SCREEN_INDEX",
		"SCREEN_X_INDEX",
		"SCREEN_Y_INDEX",
		"SCREEN_NB",
		"SCREEN_X_NB",
		"SCREEN_Y_NB",
	//Dangerous
	//	"INCREMENT",
	//	"DECREMENT",
		"COPY_BLOCK_REAL",
	//Experimental
		"LUA",
		"CELL_SIZE",
	//3D Distance 1D
		"3D_DISTANCE",
		"3D_DISTANCE_MOCAP",
		"3D_DISTANCE_CAMERA",
	//3D Fn
		"3D_RANDOM_LINEAR",
		"3D_RANDOM_GAUSS",
	//3D Math
		"3D_COPY",
		"3D_ADD",
		"3D_AVERAGE",
		"3D_INTERPOLATE",
	//3D Change
		"3D_CHANGE_LINEAR",
		"3D_CHANGE_SINUS",
	//3D Data
		"3D_CONTROL",
		"3D_CONTROL_SET",
	//3D Curve
		"3D_CURVE",
		"3D_CURVE_FROM_PHASE",
	//3D Value
		"3D_VALUE",
		"3D_VALUE_MODULE",
		"3D_VALUE_GLOBAL",
	//3D Mocap
		"3D_PATH_POS",		//	Dec99 was 22
		"3D_PATH_TO_WORLD",
		"3D_PATH_SCALE",
		"3D_MOCAP",			//	Dec99 was 21
		"3D_MOCAP_AXE_X",
		"3D_MOCAP_AXE_Y",
		"3D_MOCAP_AXE_Z",
		"3D_MOCAP_TANGENT",
		"3D_MOCAP_SCALE",
		"3D_MOCAP_FEED",
	//3D Image
		"3D_CONVERT_RGB_TO_HSV",
		"3D_CONVERT_HSV_TO_RGB",
		"3D IMAGE COLOR",
		"3D IMAGE COLOR CLAMPED",
	//3D video
		"3D VIDEO COLOR",
		"3D VIDEO COLOR CLAMPED",
	//3D Bdd Tri
		"3D_BDD_TRI_CENTER",
		"3D_BDD_TRI_BARYCENTER",
		"3D_BDD_TRI_MIN",
		"3D_BDD_TRI_MAX",
		"3D_BDD_TRI_SIZE",
		"3D_BDD_TRI_TEX_CENTER",
		"3D_BDD_TRI_TEX_SIZE",
	//	"3D_NAME_REAL",
	//	"3D_NAME_REAL_SET",
	//3D Experimental
		"3D_MULTIPLE_POS",
		"3D_MULTIPLE_SIZE",
	//	"3D_BDD_POINT_NB",
		"3D_BDD_POINT_AVERAGE",
		"3D_BDD_POINT_RAW",
		"3D_BDD_INTERSECTION",
		"3D_2_POINTS_TO_YX_ROT",
		"3D_CELL_POS",
	};

	enum LIMIT_TYPE : INT32
	{
		LIMIT_NO = 0,
		LIMIT_CLAMP,
		LIMIT_WRAP,
		LIMIT_BOUNCE,
		LIMIT_ABS,
		LIMIT_ABS_CLAMP,
		LIMIT_ABS_WRAP,
		LIMIT_ABS_BOUNCE,
		LIMIT_TYPE_NB_MAX
	};

	C_PCHAR_C	str_limit_type[LIMIT_TYPE_NB_MAX] =
	{
		"No",
		"CLAMP",
		"WRAP",
		"BOUNCE",
		"ABSOLUTE",
		"ABSOLUTE_THEN_CLAMP",
		"ABSOLUTE_THEN_WRAP",
		"ABSOLUTE_THEN_BOUNCE"
	};

//hack the averager vs Flux_filter should be solved
#define FLUX	_averager
								
	enum OUT_SEL_TYPE : INT32
	{
		OUT_SEL_NO = 0,
		OUT_SEL_X,
		OUT_SEL_Y,
		OUT_SEL_Z,
		OUT_SEL_X_MINUS,
		OUT_SEL_Y_MINUS,
		OUT_SEL_Z_MINUS,
		OUT_SEL_NB_MAX
	};

	C_PCHAR_C	str_out_sel[OUT_SEL_NB_MAX] =
	{
		"No",
		"X",
		"Y",
		"Z",
		"-X",
		"-Y",
		"-Z"
	};

	enum DRAW_TYPE : INT32
	{
		DRAW_NO = 0,
		DRAW_FILTERED,
		DRAW_RAW,
		DRAW_ALL,
		DRAW_NB_MAX
	};

	C_PCHAR_C	str_draw_type[DRAW_NB_MAX] =
	{
		"OFF",
		"FILTERED",
		"RAW",
		"ALL"
	};

	enum NET_OUT_TYPE : INT32
	{
		NET_OUT_NO = 0,
		NET_OUT_SEND,
		NET_OUT_RECEIVE,
		NET_OUT_MASTER_SEND_SLAVE_RECEIVE,
		NET_OUT_MASTER_SEND,
		NET_OUT_SLAVE_RECEIVE,
		NET_OUT_SEL_NB_MAX
	};

	C_PCHAR_C	str_net_out[NET_OUT_SEL_NB_MAX] =
	{
		"No",
		"Send",
		"Receive",
		"Master send Slave receive",
		"Only Master send",
		"Only Slave receive"
	};

	C_PCHAR_C	str_net_out_short[NET_OUT_SEL_NB_MAX] =
	{
		"",
		"S",
		"R",
		"MS",
		"OMS",
		"OSR"
	};

	FINLINE	bool	is_send_needed( INT32 s_in )
	{
		switch( s_in )
		{
		case NET_OUT_NO:						return false;
		case NET_OUT_SEND:						return true;
		case NET_OUT_RECEIVE:					return false;
		case NET_OUT_MASTER_SEND_SLAVE_RECEIVE:	return net->is_master();
		case NET_OUT_MASTER_SEND:				return net->is_master();
		case NET_OUT_SLAVE_RECEIVE:				return false;
		default:								debug_break( "strange param for net_send in trax" );
												return false;
		}
	}

	/*
	//unused
	FINLINE	bool	is_receive_needed( INT32 s_in )
	{
		switch( s_in )
		{
		case 0:		return false;				break;
		case 1:		return false;				break;
		case 2:		return true;				break;
		case 3:		return net->is_slave();		break;
		case 4:		return false;				break;
		case 5:		return net->is_slave();		break;
		default:	return false;				break;
		}
	}
	*/

	C_PCHAR_C	str_net_symbo[] =
	{
		"Implicit"
	};

	enum MAX_OUT_TYPE : INT32
	{
		MAX_OUT_NO = 0,
		MAX_OUT_SEND,
		MAX_OUT_NB_MAX
	};

	C_PCHAR_C	str_max_out[MAX_OUT_NB_MAX] =
	{
		"No",
		"Send",
	};

	INT32	trax_menu_base_id;

}	//namespace

INT32	c_trax::menu_build( INT32 base_id_in, PT_MENU_FN menu_fn )
{
	INT32	i_item = 0;
	INT32	menu_id;

	trax_menu_base_id = base_id_in;

	for( INT32 i_sub = 0; i_sub < FN_TYPE_GROUP_NB; ++i_sub )
	{
		type_sub_menu[i_sub] = menu::create(menu_fn);
		for( ; i_item <= fn_type_group_item_nb[i_sub]; ++i_item )
			menu::add_item( str_fn_type[i_item], trax_menu_base_id+i_item );
	}

	menu_id = menu::create(menu_fn);
	menu::set_cur( menu_id, __FUNCTION__ );

	for( INT32 i_sub = 0; i_sub < FN_TYPE_GROUP_NB; ++i_sub )
		menu::add_menu_sub( str_fn_type_group[i_sub], type_sub_menu[i_sub] );

	return menu_id;
}

namespace	n_trax
{
	C_PCHAR_C	str_format[2] = { "%.2f", "%.0f" };

	CONSTEXPR INT32 BASE_PARAM_NB	= 3;
	CONSTEXPR INT32 IN_PARAM_NB		= c_trax::IN_ACTIVE_BOOL_NB;
	CONSTEXPR INT32 WHAT_PARAM_NB	= 7;
	CONSTEXPR INT32 HOW_PARAM_NB	= 9;
	CONSTEXPR INT32 OUT_PARAM_NB	= 8;
	CONSTEXPR INT32 VALUE_PARAM_NB	= 8;
	CONSTEXPR INT32 MISC_PARAM_NB	= 6;
	CONSTEXPR INT32 DRAW_PARAM_NB	= 8;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 7;
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB
									+ IN_PARAM_NB
									+ WHAT_PARAM_NB
									+ HOW_PARAM_NB
									+ OUT_PARAM_NB
									+ VALUE_PARAM_NB
									+ MISC_PARAM_NB
									+ DRAW_PARAM_NB
									+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(	Active		)
		PARAM_DEF_REF(		name_symbo	)
		PARAM_DEF_STR(		Comment		)
		PARAM_DEF_GROUP_CLOSED(	In, IN_PARAM_NB )
			PARAM_DEF_08(		in_active, PARAM_DEF_BOOL_ON )
		PARAM_DEF_GROUP_CLOSED(	What, WHAT_PARAM_NB )
			PARAM_DEF_SYMBO(			Fn,				FN_SQUARE,			FN_SINUS,	FN_TYPE_NB_MAX-1,		str_fn_type )
			PARAM_DEF_SYMBO(			Output,			OUTPUT_RECORDER,	OUTPUT_FN,	OUTPUT_TYPE_NB_MAX-1,	str_output_type )
			PARAM_DEF_INT32(			Channel,		2, 1,	1, CHANNEL_NB_MAX )
			PARAM_DEF_INT32(			Control_id,		0, 1,	0, CONTROL_NB_MAX-1 )
			PARAM_DEF_REF(				Name			)
			PARAM_DEF_INT32(			Channel_bis,	2, 1,	1, CHANNEL_NB_MAX )
			PARAM_DEF_INT32(			Control_id_bis,	0, 1,	0, CONTROL_NB_MAX-1 )
		PARAM_DEF_GROUP_CLOSED(	How, HOW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			Restart_trig	)
			PARAM_DEF_REAL_POS_ZERO(	Ease_before		)
			PARAM_DEF_DOUBLE_ONE(		Freq			)
			PARAM_DEF_DOUBLE_INF(		Phase,			1, 0		)
			PARAM_DEF_REAL_POS_ZERO(	Ease_after		)
			PARAM_DEF_REAL(				Gain,			.25, .5,	0, 1 )
			PARAM_DEF_REAL(				Bias,			.25, .5,	0, 1 )
			PARAM_DEF_REAL_ZERO(		Round			)
			PARAM_DEF_REAL_INF(			Threshold,		0, .5		)
		PARAM_DEF_GROUP_CLOSED(	Out, OUT_PARAM_NB )
			PARAM_DEF_REAL_ZERO(		Min				)
			PARAM_DEF_REAL_ONE(			Max				)
			PARAM_DEF_REAL_ZERO(		Offset			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	Limit,			str_limit_type )
			PARAM_DEF_REAL_ZERO(		Limit_min		)
			PARAM_DEF_REAL_ONE(			Limit_max		)
			//todo add a filter type
			PARAM_DEF_REAL(				Filter,			.5, 0,		0, 0.99999 )
			PARAM_DEF_STR(				Format			)
		PARAM_DEF_GROUP_CLOSED(	Out Value, VALUE_PARAM_NB )
			PARAM_DEF_DOUBLE_LOCKED(	phase_internal	)
			PARAM_DEF_REAL_LOCKED(		out_fn_value	)
			PARAM_DEF_REAL_LOCKED(		out_0			)
			PARAM_DEF_REAL_LOCKED(		out_1			)
			PARAM_DEF_REAL_LOCKED(		out_2			)
			PARAM_DEF_SYMBO(			out_selector_0,	OUT_SEL_NO, OUT_SEL_X,	OUT_SEL_NB_MAX-1, str_out_sel )
			PARAM_DEF_SYMBO(			out_selector_1, OUT_SEL_NO, OUT_SEL_Y,	OUT_SEL_NB_MAX-1, str_out_sel )
			PARAM_DEF_SYMBO(			out_selector_2, OUT_SEL_NO, OUT_SEL_Z,	OUT_SEL_NB_MAX-1, str_out_sel )
		//todoqq
		//	should we keep it ?
		//	add a PARAM_STR_PT or PARAM_STR_PT_LOCKED
		//		PARAM_DEF_STR_LOCKED( out_str )
		PARAM_DEF_GROUP_CLOSED(	Net Midi, MISC_PARAM_NB )
			PARAM_DEF_SYMBO(			net_out,		NET_OUT_SEND, NET_OUT_NO, NET_OUT_MASTER_SEND_SLAVE_RECEIVE, str_net_out )
			PARAM_DEF_SYMBO_ZERO(		net_channel_id,	1,0,	0,c_net::MATRIX_CHANNEL_NB,	str_net_symbo )
			PARAM_DEF_SYMBO_ZERO(		net_channel,	1,0,	0,c_net::MATRIX_CHANNEL_NB,	str_net_symbo )
			//todoqq	suppress this one
			PARAM_DEF_SYMBO(			max_out,		MAX_OUT_SEND, MAX_OUT_NO, MAX_OUT_NB_MAX-1, str_max_out )
			PARAM_DEF_INT32(			max_channel_id, 2,1,	1,c_net::MATRIX_CHANNEL_NB )
			PARAM_DEF_INT32(			max_control_id, 2,1,	0,c_net::MATRIX_CHANNEL_NB )
			//PARAM_DEF_BOOL_OFF( midi_out )

		PARAM_DEF_GROUP_CLOSED(	Draw, DRAW_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	draw,			str_draw_type )
			PARAM_DEF_COLOR_RGBA(		draw			)
			PARAM_DEF_REAL(				draw_line_size, .1, 1, 0.001, PARAM_MAX_REAL )
			PARAM_DEF_BOOL_OFF(			draw_fixed		)
			PARAM_DEF_FP32_ONE(			draw_factor		)
	};
}

namespace {
c_rand_lin		trax_rand_lin;
c_rand_gauss	trax_rand_gauss;

thread_local  CHAR	trax_name[256];

//format is "XXXT00 o00i00"
CONSTEXPR INT32 TRAX_NAME_OFFSET_INDEX	=	3;
CONSTEXPR INT32 TRAX_NAME_OFFSET_IN		=	7;
CONSTEXPR INT32 TRAX_NAME_OFFSET_OUT	=	10;
CONSTEXPR INT32 TRAX_NAME_OFFSET_TEXT	=	14;
};

void c_trax::param_init_pt()
{
	INT32	h=0;
//	if( param_header )

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, get_name_symbo() );
	param_set_pt( h, get_comment() );

//	In
	++h;
		param_set_pt_n( h, _b_in_active, IN_ACTIVE_BOOL_NB );

//	what
	++h;
		param_set_pt( h, _fn_type_ui );
		param_set_pt( h, _output_type );
		param_set_pt( h, _channel_id );
		param_set_pt( h, _control_index_ui );
		param_set_pt( h, _var_name );
		param_set_pt( h, _channel_id_bis );
		param_set_pt( h, _control_index_bis_ui );

//	how
	++h;
		param_set_pt( h, _b_restart_trig_ui );
		param_set_pt( h, _ease_before );
		param_set_pt( h, _freq_ui );
		param_set_pt( h, _phase_ui );
		param_set_pt( h, _ease_after );
		param_set_pt( h, _gain );
		param_set_pt( h, _bias );
		param_set_pt( h, _round );
		param_set_pt( h, _threshold );

//	Out
	++h;
		param_set_pt( h, _min );
		param_set_pt( h, _max );
		param_set_pt( h, _offset );
		param_set_pt( h, _s_limit );
		param_set_pt( h, _limit_min );
		param_set_pt( h, _limit_max );
	//	param_set_pt( h, s_filter_type);
		param_set_pt( h, _filter_factor  );
		param_set_pt( h, _format );

//	Out Value
	++h;
		param_set_pt( h, _phase_cur		);
		param_set_pt( h, _value_fn_out	);
		param_set_pt_3( h, _value_out	);
		param_set_pt_3( h, _s_out_sel	);
//todoqq
//	param_set_pt( h, _str_out );

	//	net & misc
	++h;
		param_set_pt( h, _s_net_out );
		param_set_pt( h, _net_channel_id );
		param_set_pt( h, _net_channel_id );
		param_set_pt( h, _s_max_out );
		param_set_pt( h, _max_channel_id );
		param_set_pt( h, _max_control_id );
	//	param_set_pt( h, b_midi_out);

	++h;
		param_set_pt( h, _s_draw );
		param_set_pt_4( h, _color );
		param_set_pt( h, _line_size );
		param_set_pt( h, _b_draw_fixed );
		param_set_pt( h, _draw_factor );

	err_param_init_pt(h);
}
//todo do it in place and faster
void	c_trax::build_name( C_PCHAR_C name_in )
{
	CHAR*	str;
	CHAR*	pt;
	INT32	in_nb;
	INT32	out_nb;

	update_param_header();	//todoq this is far from optimum but simple

	p_param	header = get_param_header();
	in_nb = header->get_in_nb();
	out_nb = header->get_out_nb();
//deal with "XXXT00 o00i00" format
	strcpy( trax_name, header->get_name().get() );
	
	//par	trax have a reverse idea of in/out for the moment
	pt = trax_name + TRAX_NAME_OFFSET_IN;
	if( out_nb == 0 )	
		strncpy( pt, "   ", 3 );
	else
	{
		*pt = 'i';
		strnum::make( pt+1, 2, out_nb );
	}
	pt = trax_name + TRAX_NAME_OFFSET_OUT;
	if( in_nb == 0 )	
		strncpy( pt, "   ", 3 );
	else
	{
		*pt = 'o';
		strnum::make( pt+1, 2, in_nb );
	}

	str = trax_name + TRAX_NAME_OFFSET_TEXT;
	if( name_in )
		strcpy( str, name_in );
	else if( _con_last )
	{
//		c_obj_ui*	obj;
		p_param	param;	//par
//		obj = _con_last->get_in_obj();
		param = _con_last->get_in_param();
#if	AAA_DEBUG()
		if( !param)
			debug_break( "NO PARAM" );
#endif
//		sprintf( str, "%s(%d)::%s", obj->get_name(), obj->get_id(), param->get_name());
		sprintf( str, "%s", param->get_name().get() );
	}
	else
		strcpy( str, "Free" );
	get_param_header()->set_name( trax_name );
	return;
}

namespace{
	CHAR	info[1024];
}

void c_trax::prepare_for_ui()
{
	//SUPER::prepare_for_ui();

//	p_param	header = get_param_header();
//	{	//todonow this is dangerous because param_header hold a copy so no delete goes well
//		header->borrow_in ( _ins  );
//		header->borrow_out( _outs );
//	}

	CHAR* pt_name = info;
	if( _s_net_out )
	{
		strcpy( pt_name, str_net_out_short[_s_net_out] );
		pt_name += strlen( str_net_out_short[_s_net_out] );
		*pt_name++ = ' ';
	}

	switch( _output_type )
	{
	case OUTPUT_FN:
		break;
	case OUTPUT_FN_THRESHOLD:
	case OUTPUT_FN_TRIG_UP_AND_DOWN:
	case OUTPUT_FN_TRIG_DOWN:
	case OUTPUT_FN_TRIG_UP:
	case OUTPUT_RECORDER:
	case OUTPUT_MIN:
	case OUTPUT_MIDDLE:
	case OUTPUT_MAX:
	case OUTPUT_IN:
		sprintf( pt_name, "%s ", str_output_type_short[_output_type] );
		pt_name += strlen(pt_name);
		break;
	default:
		sprintf( pt_name, "Bad output_type ask Maa " );
		pt_name += strlen(pt_name);
		break;
	}
	switch( _fn_type_ui )
	{
	case FN_ALPHABET:
	case FN_ALPHABET_LAST:
		sprintf( pt_name, "%s(%c-%c)", str_fn_type[_fn_type_ui], '@'+_control_index_ui, '@'+_control_index_bis_ui );
		break;
	case FN_ALPHABET_ONE:
		sprintf( pt_name, "%s(%c)", str_fn_type[_fn_type_ui], '@'+_control_index_ui );
		break;
	case FN_ASCII_EQUAL_CONTROL:
		sprintf( pt_name, "%s(%d)", str_fn_type[_fn_type_ui], _control_index_ui );
		break;
	case FN_VARIABLE:
	case FN_VARIABLE_SET:
//		case FN_VARIABLE_STRING:
//		case FN_VARIABLE_STRING_SET:
	case FN_LUA:
		sprintf( pt_name, "%s \"%s\"", str_fn_type[_fn_type_ui], _var_name.get() );
		break;
	case FN_3D_DISTANCE_MOCAP:
		sprintf( pt_name, "%s(%d,%d,%d,%d)", str_fn_type[_fn_type_ui], _channel_id, _control_index_ui, _channel_id_bis, _control_index_bis_ui );
		break;
	case FN_FILE:
	case FN_FILE_STRING_LEN:
	case FN_CONTROL:
	case FN_CONTROL_SET:
	case FN_3D_CONTROL:
	case FN_3D_CONTROL_SET:
		sprintf( pt_name, "%s(%d,%d,%d)", str_fn_type[_fn_type_ui], _channel_id, _control_index_ui, _control_index_bis_ui );
		break;
	case FN_MORE_THAN_VALUE_INDEX:
	case FN_MORE_THAN_VALUE_MODULE_INDEX:
	case FN_MORE_THAN_VALUE_GLOBAL_INDEX:
		sprintf( pt_name, "%s(%d,%d)", str_fn_type[_fn_type_ui], _control_index_ui, _control_index_bis_ui );
		break;
	case FN_SORT_VALUE_INDEX_FROM_MIN:
	case FN_SORT_VALUE_MODULE_INDEX_FROM_MIN:
	case FN_SORT_VALUE_GLOBAL_INDEX_FROM_MIN:
	case FN_SORT_VALUE_INDEX_FROM_MAX:
	case FN_SORT_VALUE_MODULE_INDEX_FROM_MAX:
	case FN_SORT_VALUE_GLOBAL_INDEX_FROM_MAX:
		sprintf( pt_name, "%s(%d,%d,%g)", str_fn_type[_fn_type_ui], _control_index_ui, _control_index_bis_ui, _freq_ui );
		break;
	case FN_FILE_LETTER:
		sprintf( pt_name, "%s(%d,%d,%d,%g)", str_fn_type[_fn_type_ui], _channel_id, _control_index_ui, _control_index_bis_ui, _freq_ui );
		break;
//	name( _channel_id, _control_index_ui )
	case FN_BDD_TRI_NAME:
	case FN_MIDI_CONTROL_NO_OFFSET:
	case FN_NEAT:
	case FN_CURVE:
	case FN_CURVE_X:
	case FN_CURVE_Y:
	case FN_CURVE_Z:
	case FN_CURVE_FROM_PHASE:
	case FN_CURVE_X_FROM_PHASE:
	case FN_CURVE_Y_FROM_PHASE:
	case FN_CURVE_Z_FROM_PHASE:
	case FN_3D_CURVE:
	case FN_3D_CURVE_FROM_PHASE:
	case FN_3D_MOCAP:
	case FN_3D_MOCAP_AXE_X:
	case FN_3D_MOCAP_AXE_Y:
	case FN_3D_MOCAP_AXE_Z:
	case FN_3D_MOCAP_TGN:
	case FN_3D_MOCAP_SCA:
	case FN_3D_MOCAP_FEED:
	case FN_3D_BDD_TRI_CENTER:
	case FN_3D_BDD_TRI_BARYCENTER:
	case FN_3D_BDD_TRI_MIN:
	case FN_3D_BDD_TRI_MAX:
	case FN_3D_BDD_TRI_SIZE:
	case FN_3D_BDD_TRI_TEX_CENTER:
	case FN_3D_BDD_TRI_TEX_SIZE:
	case FN_3D_PATH_POS:
	case FN_3D_PATH_TO_WORLD:
	case FN_3D_PATH_SCA:
		sprintf( pt_name, "%s( mocap=%d, node=%d )", str_fn_type[_fn_type_ui], _channel_id, _control_index_ui );
		break;
	case FN_CURVE_BY_NAME:
		sprintf( pt_name, "%s( %d, %s )", str_fn_type[_fn_type_ui], _channel_id, _var_name.get() );
		break;
	case FN_MIDI_CONTROL:
	case FN_MIDI_CONTROL_REL:
	case FN_MIDI_VELOCITY:
		{
		CHAR		str_ch[32];
		CHAR		str_co[32];
		c_traxs*	traxs = (c_traxs *)get_root();
		if( traxs->get_channel_offset())
			sprintf( str_ch, "%d+%d", traxs->get_channel_offset(), _channel_id );
		else
			sprintf( str_ch, "%d", _channel_id );
		if( traxs->get_control_offset())
			sprintf( str_co, "%d+%d", traxs->get_control_offset(), _control_index_ui  );
		else
			sprintf( str_co, "%d", _control_index_ui );
		sprintf( pt_name, "%s(%s,%s)", str_fn_type[_fn_type_ui], str_ch, str_co );
		}
		break;
	case FN_TRAX:
	case FN_TOASTER:
		sprintf( pt_name, "%s(%d)", str_fn_type[_fn_type_ui], _control_index_ui );
		break;
	case FN_FILE_IS_CHANGED:
	case FN_SET_VALUE:
	case FN_SET_VALUE_MODULE:
	case FN_SET_VALUE_GLOBAL:
		sprintf( pt_name, "%s(%d)", str_fn_type[_fn_type_ui], _control_index_ui );
		break;
//	name( _control_index_ui )
	case FN_TEXT:
	case FN_TEXT_LEN:
	case FN_TEXT_LINE_NB:
	case FN_VALUE:
	case FN_VALUE_MODULE:
	case FN_VALUE_GLOBAL:
	case FN_MULTIPLE_PARAMETER:
	case FN_3D_VALUE:
	case FN_3D_VALUE_GLOBAL:
	case FN_3D_BDD_POINT_AVERAGE:
	case FN_3D_BDD_POINT_RAW:
		sprintf( pt_name, "%s(%d)", str_fn_type[_fn_type_ui], _control_index_ui );
		break;
	case FN_3D_IMAGE_COLOR:
	case FN_3D_IMAGE_COLOR_CLAMPED:
		sprintf( pt_name, "%s(%d)", str_fn_type[_fn_type_ui], _control_index_ui );
		break;
	case FN_EQUAL_FREQ:
	case FN_NOT_EQUAL_FREQ:
	case FN_MORE_THAN_FREQ:
	case FN_MORE_EQUAL_THAN_FREQ:
	case FN_LESS_THAN_FREQ:
	case FN_LESS_EQUAL_THAN_FREQ:
	case FN_PROXIMITY_FREQ_LINEAR:
	case FN_PROXIMITY_FREQ_SINUS:
	case FN_NET_HOST_ID_EQUAL_FREQ:
	case FN_NET_HOST_ID_NOT_EQUAL_FREQ:
	case FN_NET_HOST_ID_MORE_THAN_FREQ:
	case FN_NET_HOST_ID_MORE_EQUAL_THAN_FREQ:
	case FN_NET_HOST_ID_LESS_THAN_FREQ:
	case FN_NET_HOST_ID_LESS_EQUAL_THAN_FREQ:
	case FN_BREAK_EQUAL_FREQ:
	case FN_BREAK_NOT_EQUAL_FREQ:
	case FN_BREAK_MORE_THAN_FREQ:
	case FN_BREAK_MORE_EQUAL_THAN_FREQ:
	case FN_BREAK_LESS_THAN_FREQ:
	case FN_BREAK_LESS_EQUAL_THAN_FREQ:
	case FN_INTERPOLATE:
		sprintf( pt_name, "%s(%g)", str_fn_type[_fn_type_ui], _freq_ui );
		break;
	case FN_INSIDE_FREQ_PHASE:
	case FN_OUTSIDE_FREQ_PHASE:
	case FN_INSIDE_FREQ_PHASE_LINEAR:
	case FN_INSIDE_FREQ_PHASE_SINUS:
	case FN_BREAK_INSIDE_FREQ_PHASE:
	case FN_BREAK_OUTSIDE_FREQ_PHASE:
	case FN_IMAGE_RED:
	case FN_IMAGE_GREEN:
	case FN_IMAGE_BLUE:
	case FN_IMAGE_ALPHA:
	case FN_IMAGE_GREY:
	case FN_VIDEO_RED:
	case FN_VIDEO_GREEN:
	case FN_VIDEO_BLUE:
	case FN_VIDEO_ALPHA:
	case FN_VIDEO_GREY:
		sprintf( pt_name, "%s(%g,%g)", str_fn_type[_fn_type_ui], _freq_ui, (REAL)_phase_ui );
		break;
//		case FN_3D_BDD_POINT_NB:

	case FN_MACHINE_EQUAL_NAME:
	case FN_LANGROUP_EQUAL_NAME:
		sprintf( pt_name, "%s(%s)", str_fn_type[_fn_type_ui], _var_name.get() );
		break;
	case FN_ASCII:
	default:
		sprintf( pt_name, "%s", str_fn_type[_fn_type_ui] );
		break;
	}
	
	build_name( info );
	// change the PARAM_GROUP "What" this text
	get_param( n_trax::BASE_PARAM_NB + n_trax::IN_PARAM_NB + 1 )->set_comment( info );	
}

void	c_trax::param_init()
{
	_b_record = false;
	_flux = nullptr;
	_averager = nullptr;
	_fn_type = -1;

	clear_v3( _value_last_low );
//	clear_v3( value_last );
//	clear_v3( _value_out );
}

#define	TRAX_EVENT_MAX_NB	(70*5*60)	//hack thsi should be done cleany (use STL)
void c_trax::alloc_events()
{
	if ( !_p_event )
	{
		_p_event = new ST_TRAX_EVENT[TRAX_EVENT_MAX_NB];
		if( _p_event )
		{
			_p_event_cur = _p_event;
			_p_event_limit = _p_event + TRAX_EVENT_MAX_NB - 1;
			_event_index_max = 0;
		}
		else
		{
			box_err( "Trax can't allocate events");
		}
	}
}

void c_trax::dealloc_events()
{
	if( _p_event )
	{
		SAFE_DELETE_ARRAY( _p_event );
		_p_event_cur = _p_event;
		_p_event_limit = _p_event-1;
		_event_index_max = 0;
	}
}

//#define	TRAX_AVERAGE_NB	64
//#define	TRAX_DRAW_AVERAGE_NB	256

/*
void c_trax::alloc( INT32 nb )
{
}
*/

void c_trax::dealloc()
{
//	param_dealloc();
	dealloc_events();
	obj_delete( _flux );
	obj_delete( _averager );
}

void c_trax::cell_draw_obj( REAL CONST size )
{
	draw_box_sxyz( size, size*REAL(.1), size );
}


CONSTRUCTOR_CREATE(c_trax)
	,_p_event			(	nullptr		)
	,_p_event_cur		(	_p_event	)
	,_p_event_limit		(	_p_event-1	)
	,_event_index_max	(	0			)
	,_str_out			(	nullptr		)
	,_phase_cur			(	0			)
	,_s_net_out			(	0			)
	,_b_trig_last		(	false		)
	,_phase				(	0.			)
	,_phase_last		(	-1.			)	//todo buggy rnd not reset right away
	,_dim				(	1			)
	,_con_last			(	nullptr		)

{
	_format.set( "%.2f" ); 
	_letter[1] = 0;
	_b_trig_last_3d[0] = false;
	_b_trig_last_3d[1] = false;
	_b_trig_last_3d[2] = false;
	clear_v3( _value_out );

	param_init_with( n_trax::param, n_trax::PARAM_NB_MAX );
}

void	c_trax::set_fn_midi( INT32 channel_id_in , INT32 control_index_in )
{
	_fn_type_ui = FN_MIDI_CONTROL;
	_channel_id  = channel_id_in;
	_control_index_ui = control_index_in;
	set_active(true);
}

void	c_trax::set_index( INT32 index )
{
	_trax_index = index;
	strnum::make( (CHAR*) get_param_header()->get_name().get()+TRAX_NAME_OFFSET_INDEX+1, 2, _trax_index+1 );	//hack
}

INT32	c_trax::get_index()
{
	return	get_index_from_param_pt( get_param_header() );
}

void	c_trax::trax_init( INT32 index, C_PCHAR_C str_id )
{
	CHAR	str[64];
	c_param* header = get_param_header();

	strncpy( str, str_id, TRAX_NAME_OFFSET_INDEX );
	strcpy( str+TRAX_NAME_OFFSET_INDEX, "T00  None  ");

	header->set_name( str );

	set_index( index );
}

c_trax::~c_trax()
{	//todonow	make sure trax are disconnected for now we disconnect and destroy all the trax in traxs
	dealloc();
}

//todo	cache plug in at connection to be faster
FINLINE	p_param	c_trax::get_plug_in( INT32 &i )	//par
{
	p_param header = get_param_header();
	INT32 nb = header->get_out_nb();
	for( ;i<nb; ++i )
	{
		c_connex* con = header->get_out(i);
		if( ((IN_ACTIVE_BOOL_NB<=i) || _b_in_active[i]) && con->get_in_param() == get_param_header() )	//was 	if ( !param_is_obj_flag(*param) )
		{
			++i;
			return con->get_param_out_set();	
		}
	}
	return nullptr;
}

FINLINE	p_param	c_trax::get_plug_in_first()		//par	redo all these in a more elaborate way : eg get_plug_in_first_value
{
	INT32 i=0;
	return get_plug_in(i);
}

FINLINE	p_param	c_trax::get_plug_out( INT32 &i )	//par
{
	p_param header = get_param_header();
	INT32 nb = header->get_in_nb();
	for( ; i<nb; ++i )
	{
		c_connex*	con = header->get_in(i);
		if( con->get_out_param() == get_param_header() )
		{
			++i;
			return con->get_param_in_set();
		}
	}
	return nullptr;
}

FINLINE	p_param	c_trax::get_plug_out_first()	//par
{
	INT32	i = 0;
	return get_plug_out(i);
}

FINLINE	p_param	c_trax::get_plug_in_then_out_first()	//par
{
	p_param param = get_plug_in_first();
	if( param )
		return param;
	return get_plug_out_first();
}

FINLINE	void c_trax::flux_update()
{
	if( !_flux )
	{
		try
		{
			//	_flux = new c_flux_filter( 1,0)[3];
			_flux = new c_flux_filter;
			_averager = new c_averager;
		}
//		catch( bad_alloc& e)
		catch(...)
		{
			box_err( "Trax can't allocate flux filter and averager.");
			return;
		}
	}
	_flux->set_filter_factor( _filter_factor  );
	_averager->set_filter_factor( _filter_factor  );
}

void c_trax::draw()
{
	if( is_active() && FLUX )
	{
		if( !FLUX )
			flux_update();
		if( _s_draw > DRAW_NO )
		{
			FLUX->set_draw_fixed( _b_draw_fixed );
			GOL::set_line_width ( _line_size );
			FLUX->draw_curve_in_rect( 0, 0, 1, 1, (_max != _min )?_draw_factor/(_max-_min ):REAL(.9999), _s_draw, _color );
		}
	}
}

void c_trax::start_record()
{
//	value_last = -PARAM_INFINI;
	_b_record = true;
	_event_index_max = 0;

}

void c_trax::stop_record()
{
	if( _output_type == OUTPUT_RECORDER )
	{
		if( _b_record )
		{
			_output_type = OUTPUT_FN;
			_fn_type = FN_PLAYER;
			_p_event_cur->timestamp = (_p_event_cur-1)->timestamp + REAL(10000);	//toclean
			_p_event_cur->value = (_p_event_cur-1)->value;
			++_event_index_max;
		}
	}
	_b_record = false;
}

//todo deal with record/play in 3d
void c_trax::start_loop( bool record_in )
{
	if( _output_type == OUTPUT_RECORDER || _fn_type == FN_PLAYER )
	{
		_p_event_cur = _p_event;
		if( _event_index_max )
			_value_out[0] = _p_event->value;
		if( record_in && _output_type == OUTPUT_RECORDER )
			start_record();
	}
}

void c_trax::stop_loop()
{
	stop_record();
	_p_event_cur = _p_event;
}

namespace {
	CONST CHAR  data_ext[] = "data";
};

AAA_ERR	c_trax::save_data_to_file( o_str CONST & filename_in )
{
	if( !_p_event )
	{
		err_print( "No data allocated to save to file %s", filename_in );
		return ERR_ANY;
	}

	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( data_ext );

		ST_TRAX_EVENT* pt = _p_event;
		//	open file
		FILE* file = c_file::FOPEN( filename, "wt");
		if ( file == nullptr )
		{
			err_print( "Can't open file %s", filename.get() );
			o_str::pop_name();
			return  ERR_ANY;
		}
		for( INT32 i = _event_index_max; i > 0; --i )
		{
			fprintf( file, "%f %f\n", pt->timestamp, pt->value );
			//real file << pt->timestamp << " " << pt->value << "\n";
			++pt;
		}
		c_file::FCLOSE( file);
	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR	c_trax::load_data_from_file( o_str CONST & filename_in )
{
	alloc_events();
	if( !_p_event )
	{
		err_print( "No data allocated to load from file %s", filename_in );
		return ERR_ANY;
	}

	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( data_ext );

		if( c_file::is_exist(filename) )
		{//	open file
			FILE* file = c_file::FOPEN( filename, "rt");
			if( file == nullptr )
			{
				err_print( "Can't open file %s", filename.get() );
				_event_index_max = 0;
				o_str::pop_name();
				return  ERR_ANY;
			}
			ST_TRAX_EVENT* pt = _p_event;
			for( ; ; )
			{
				DOUBLE	f[2];
				if( fscanf( file, "%lf %lf\n", f, f+1 ) != 2 )
					break;
				pt->timestamp = REAL(f[0]);
				pt->value = REAL(f[1]);
				++_event_index_max;
				++pt;
			}
			c_file::FCLOSE( file );
		}
	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR	c_trax::save_do_after( o_str CONST & filename )
{
	AAA_ERR retcode = AAA_OK;
	if( _fn_type == FN_PLAYER && _event_index_max ) 
		retcode = save_data_to_file( filename );
	return retcode;
}

AAA_ERR	c_trax::load_do_after( o_str CONST & filename )
{
	AAA_ERR retcode = AAA_OK;
	if( _fn_type_ui == FN_PLAYER ) 
		retcode = load_data_from_file( filename );
/* move in traxs to get the index
#if	AAA_NET()
	if( net && net->is_slave() )
		make_slave();
#endif
*/
	return retcode;
}

void	c_trax::update_param_header()
{
	p_param	header = get_param_header();
	if( !header )
	{
		debug_break( "%s() get_param_header() return nullptr, should not happen", __FUNCTION__ );
		return;
	}

	c_connex*	con = header->get_in(0);
	if( _con_last != con )
	{
		_con_last = con;
//todo	optimize by keeping connex_last and really compare
		if( con )
		{
			p_param	param = _con_last->get_in_param();	//par
			//todo make it less heavy use a fn
#if	AAA_DEBUG()
			if( !param )
				debug_break( "%s() Null in ?", __FUNCTION__ );
#endif
			//header->set_for_trax(p_in);	//par check the init of it
			header->cpy_pt( param );	//par will stay ?
			header->set_type( param->get_type() );
			//not sure we need this
			header->set_limits( param->get_def(), param->get_ina(), param->get_min(), param->get_max() );
//				value_last = param_get_value_as_double( child );
		}
		else
		{
			header->set_type( TYPE_NONE );
			header->set_pt_null();
		}
	}
}

//todo should be a virtual fn in obj_ui
void	c_trax::plug_out( c_obj_ui* obj, p_param param )
{
	c_connex::add( obj, param, this, nullptr );
//par	update_param_header is called in c_trax::build_name
//	update_param_header();
}

//todo should be a virtual fn in obj_ui
void	c_trax::plug_in( c_obj_ui* obj, p_param param )
{
	c_connex::add( this, nullptr, obj, param );
//par	update_param_header is called in c_trax::build_name
//	update_param_header();
}

FINLINE	INT32	c_trax::get_net_channel_id()
{
	if( _net_channel_id > 0 )
		return _net_channel_id;
	//todo we should check what's going on between name/trax_name/param.set_name()
	if( get_param_header()->get_name().is_equal_nocase( "loc", 3 ) )
	{
		c_traxs*	traxs = (c_traxs *)get_root();
		c_layers*	layers = (c_layers *)traxs->get_root();
		return layers->get_id_local();
	}
//hack
	return c_net::MATRIX_CHANNEL_NB;
}

FINLINE	INT32	c_trax::get_net_control_id()
{
	if( _net_channel_id > 0 )
		return _net_channel_id;
	return _trax_index+1;
}


//hack
//todo do it better
void	c_trax::make_slave()
{
	if( _s_net_out == 3 || _s_net_out == 5 )
	{
		_fn_type_ui = FN_NET_IN;
		_offset = 0;
		_min = 0;
		_max = 1;
		_output_type = OUTPUT_FN;
		_channel_id = get_net_channel_id();
		_control_index_ui = get_net_control_id();
	}
}

#if AAA_TRACKER_NEAT()
REAL	get_neat( INT32	channel_id, INT32 control_id )
{
	if( b_neat_done_with_midi )
		return c_midi::static_get_control( channel_id, control_id );
	else
		return neat_control_get( channel_id, control_id );
}
#endif //#if AAA_TRACKER_NEAT()

//static	REAL	g_freq_factor = 1.;
//static	REAL	g_phase_offset = 0.;

FINLINE	void	c_trax::compute_phase_time()
{
//hack ?
	if( draw::is_render_first_pass() )
	{
		DOUBLE dt = _delta_t.update_v2();
		if( dt < 0. || _b_restart_trig_ui )
		{
			_phase = 0.;
			_b_restart_trig_ui = false;
			_phase_last = -1.;	//todo buggy rnd not reset right away
		}
		else
		{
			_phase += dt * _freq_ui;	// * g_freq_factor;
		}
	}
	_phase_cur = _phase + _phase_ui; // + g_phase_offset;
}

FINLINE	void	c_trax::compute_phase()
{
	p_param	param = get_plug_in_first();
	if( param )
	{
		_phase = param->get_value_as_double() * _freq_ui;
		_phase_cur = _phase + _phase_ui; // + g_phase_offset;
	}
	else
		compute_phase_time();
}

extern	INT32	slide_index;

//todoqqq deal with the text case where it is called
FINLINE	void	c_trax::get_double_in( DOUBLE& val )
{
	p_param	param = get_plug_in_first();
	val = param ? param->get_value_as_double() : 0.;
}

static	CHAR str_mmss[] = "mm:ss";
FINLINE	void	c_trax::compute_fn_1d( DOUBLE& val )
{
	val = 0.;	//avoid undefined behavior and simplifly code
	switch( _fn_type )
	{
	case FN_BDD_TRI_NAME:
		{
			c_bdd_tri*	tri = c_bdd_tri::get_from_channel( _channel_id );
			if( tri )
			{
				switch( _fn_type )
				{
				case FN_BDD_TRI_NAME:	_str_out = tri->get_name_from_control( _control_index_ui );	break;
				}
			}
		}
		break;
	case FN_SLIDE_INDEX:
		if( _b_restart_trig_ui )
			slide_index = 0;
		val = slide_index;
		break;
	case FN_ALPHABET:
		{
			INT32	tmp = c_keyboard::get_key_alphabet();
			if( _control_index_ui <= tmp && tmp <= _control_index_bis_ui )
				val = tmp;
		}
		break;
	case FN_ALPHABET_LAST:
		if( _b_restart_trig_ui )
			_b_restart_trig_ui = false;
		else
		{
			INT32	tmp = c_keyboard::get_key_alphabet();
			if( _control_index_ui <= tmp && tmp <= _control_index_bis_ui )
				val = tmp;
			else
				val = _value_last_low[0];
		}
		break;
	case FN_ALPHABET_ONE:
		val = (c_keyboard::get_key_alphabet() == _control_index_ui) ? 1 : 0;
		break;
	case FN_ASCII:
		val = c_keyboard::get_key_ascii();
		//	if ( val!=0. )
		//		printf( "%f", val );
		break;
	case FN_ASCII_EQUAL_CONTROL:
		val = (c_keyboard::get_key_ascii() == _control_index_ui) ? 1 : 0;
		break;
	//hack should have a mouse struct
	case FN_KEYBOARD_SHIFT:		val = modifier::is_shift_on() ? 1 : 0;	break;
	case FN_KEYBOARD_ALT:		val = modifier::is_alt_on() ? 1 : 0;	break;
	case FN_KEYBOARD_CTRL:		val = modifier::is_ctrl_on() ? 1 : 0;	break;
	case FN_MOUSE_X:	//todo deal with multi mouse
	case FN_MOUSE_Y:
		{	//beware of async change ?
			FP32	xy[2];
			c_mouse::get_cur()->get_xy_render( xy[0], xy[1] );
			val = xy[ (_fn_type == FN_MOUSE_X) ? 0 : 1 ];
		}
		break;
	case FN_MOUSE_CLICK_LEFT:	val = c_mouse::get_cur()->get_but_state( mouse::BUTTON_LEFT		) ? 1. : 0.;	break;
	case FN_MOUSE_CLICK_MIDDLE:	val = c_mouse::get_cur()->get_but_state( mouse::BUTTON_MIDDLE	) ? 1. : 0.;	break;
	case FN_MOUSE_CLICK_RIGHT:	val = c_mouse::get_cur()->get_but_state( mouse::BUTTON_RIGHT	) ? 1. : 0.;	break;
	case FN_FILE_IS_CHANGED:	val = g_datacube->is_changed( _channel_id ) ? 1 : 0;				break;
	case FN_FILE:
	case FN_CONTROL:
		if( g_datacube->is_number( _channel_id, _control_index_ui, _control_index_bis_ui ) )
			val = g_datacube->get_double( _channel_id, _control_index_ui, _control_index_bis_ui );
		else
			_str_out = g_datacube->get_str( _channel_id, _control_index_ui, _control_index_bis_ui ).get();
//hackqqq 				val = 0;
		break;
	case FN_COPY:
	case FN_CONTROL_SET:
	case FN_VARIABLE_SET:
	case FN_SET_VALUE:
	case FN_SET_VALUE_MODULE:
	case FN_SET_VALUE_GLOBAL:
		get_double_in( val );
		break;
//		case FN_VARIABLE_STRING:
//			break;
//		case FN_VARIABLE_STRING_GET:
//			break;
	case FN_VARIABLE:
		if(	!aaa::vars_double.get( _var_name.get(), val ) )
			err_print( "No AAASeed NAME_REAL variable with name \"%s\"", _var_name.get() );
		break;
	case FN_TEXT:			_str_out = bind_text::bind->get_str( _control_index_ui );		break;
	case FN_TEXT_LEN:		val = bind_text::bind->get_str_len( _control_index_ui );		break;
	case FN_TEXT_LINE_NB:	val = bind_text::bind->get_str_line_nb( _control_index_ui );	break;
	case FN_FILE_STRING_LEN:
		if( !g_datacube->is_number( _channel_id, _control_index_ui, _control_index_bis_ui ) )
			val = g_datacube->get_str( _channel_id, _control_index_ui, _control_index_bis_ui ).get_len();
		break;
	case FN_FILE_LETTER:
		if( g_datacube->is_number( _channel_id, _control_index_ui, _control_index_bis_ui ) )
			_letter[0] = I_FLOOR( g_datacube->get_double( _channel_id, _control_index_ui, _control_index_bis_ui ) );
		else
			_letter[0] = g_datacube->get_char( _channel_id, _control_index_ui, _control_index_bis_ui, I_FLOOR(_freq_ui) );
		_str_out = _letter;
		break;
	case FN_PLAYER_RAW:	//todoq make it work again and extend to 3D
	case FN_PLAYER:
		if( _event_index_max )
		{
			while( (_p_event_cur<_p_event_limit ) && ((_p_event_cur+1)->timestamp<trax_time) )
			{
				++_p_event_cur;
			}
			val = _p_event_cur->value + ( (_p_event_cur+1)->value - _p_event_cur->value)
										/( (_p_event_cur+1)->timestamp - _p_event_cur->timestamp)
										*( trax_time - _p_event_cur->timestamp);
		}
		break;
	case FN_SINUS:
		compute_phase();
		val = .5 + .5 * SIN_TURN(_phase_cur );
		//val = .5 + .5*sin( _phase_cur * 3.1415926535);
		break;
	case FN_TRIANGLE:
		compute_phase();
		val = FMOD(_phase_cur,1.);
		break;
	case FN_TRIANGLE_INV:
		compute_phase();
		val = 1. - FMOD(_phase_cur,1.);
		break;
	case FN_TRIANGLE_UP_THEN_DOWN:
		compute_phase();
		val = FMOD(_phase_cur,2.); 
		if( val > 1. )
			val = 2. - val;
		break;
	case FN_MIN_TO_MAX_LINEAR:
	case FN_MIN_TO_MAX_SINUS:
	case FN_MAX_TO_MIN_LINEAR:
	case FN_MAX_TO_MIN_SINUS:
		compute_phase();
		if( _phase_cur <= 0. )
			val = 0;
		else if( _phase_cur >= 1. )
			val = 1;
		else
		{
			if( _fn_type == FN_MIN_TO_MAX_SINUS || _fn_type == FN_MAX_TO_MIN_SINUS )
				val = (1. - COS_TURN( _phase_cur * .5 )) * .5;
			else
				val = _phase_cur;
		}	
		if( _fn_type == FN_MAX_TO_MIN_LINEAR || _fn_type == FN_MAX_TO_MIN_SINUS )
			val = 1. - val;
		break;
	case FN_CHANGE_LINEAR:
	case FN_CHANGE_SINUS:
		get_double_in( val);
		if( val != _change_val_new[0] )
		{
			_change_val_old[0] = _change_val_last[0];
			_change_val_new[0] = val;
			//DBG_PRINT_STRING( "CHANGE  %f", _change_val_new[0] );
			_b_restart_trig_ui = true;
		}
		compute_phase_time();
		//DBG_PRINT_STRING( "phase %f", _phase_cur );
		if( _phase_cur <= 0 )
			val = _change_val_old[0];
		else if( _phase_cur < 1 )
		{
			REAL tmp;
			if( _fn_type == FN_CHANGE_SINUS )
				tmp = ( REAL(1) - COS_TURN( _phase_cur * .5 ) ) * REAL(.5) ;
			else
				tmp = REAL(_phase_cur);

			gain_slick_if_needed( tmp, _gain );
			bias_slick_if_needed( tmp, _bias );

			val = interpolate( _change_val_old[0], val, tmp );
		}
		else
			_change_val_old[0] = _change_val_new[0];
		_change_val_last[0] = val;
		break;
	case FN_SQUARE:
		compute_phase();
		val = FMOD(_phase_cur,1.);
		break;
	case FN_TURBULENCE:
	case FN_FRACTALSUM:
		{
			REAL			vec[3];
			p_param			param;
			INT32			nb_harmo = MAX1( (INT32) _ease_before );

			if( param = get_plug_in_first() )
			{
				REAL CONST * src = param->get_pt_const_real3();	//par dangerous ?
				scale_v3( vec, src, _freq_ui );
			}
			else
			{
				compute_phase_time();
				vec[0] = 0.;
				vec[1] = REAL(_phase_cur);
				vec[2] = 0.;
			}
			if( _fn_type == FN_TURBULENCE )
				val = turbulence( vec, nb_harmo );
			else
				val = fractalsum( vec, nb_harmo );
//				val = fractalsum( vec, nb_harmo ) - .5 ) * 2.;
		}
		break;
	case FN_TIME:
		compute_phase_time();
		if( str_is_equal( _format.get(), "mm:ss") )
		{
			INT32 i = I_FLOOR(_phase_cur / 60);
			strnum::make( str_mmss, 2, i );
			i = I_FLOOR(FMOD( _phase_cur, 60. ));
			strnum::make( str_mmss+3, 2, i );
			_str_out = str_mmss;
		}
		else
			val = _phase_cur;
		//val = .5 + .5*sin( _phase_cur * 3.1415926535);
		break;
	case FN_SECOND:			val = aaa::date::get_second()		+ _phase_ui;	break;
	case FN_MINUTE:			val = aaa::date::get_minute()		+ _phase_ui;	break;
	case FN_HOUR:			val = aaa::date::get_hour()			+ _phase_ui;	break;
	case FN_DAY:			val = aaa::date::get_day()			+ _phase_ui;	break;
	case FN_MONTH:			val = aaa::date::get_month()		+ _phase_ui;	break;
	case FN_YEAR:			val = aaa::date::get_year()			+ _phase_ui;	break;
	case FN_DAY_OF_WEEK:	val = aaa::date::get_day_of_week()	+ _phase_ui;	break;
	case FN_DAY_OF_YEAR:	val = aaa::date::get_day_of_year()	+ _phase_ui;	break;
#if	AAA_TRACKER_MIDI()
	case FN_MIDI_CONTROL_NO_OFFSET:
		val = c_midi::static_get_control( _channel_id, _control_index_ui );
		break;
	case FN_MIDI_CONTROL:
		val = c_midi::static_get_control( _channel_id + c_traxs::g_channel_offset, _control_index_ui + c_traxs::g_control_offset );
		break;
	case FN_MIDI_CONTROL_REL:
		val = c_midi::static_get_control( _channel_id + c_traxs::g_channel_offset, _control_index_ui + c_traxs::g_control_offset );
		{
			REAL tmp = ABS( REAL(val) - REAL(.5) );
			tmp -= REAL(.02);
			if ( tmp < 0. )
				tmp = 0.;
			else
			{
				if( val < .5 )
					tmp = -tmp;
				tmp *= REAL(2./(1-.02));	//to have it from -1 to 1.
			}
			REAL freq_keep = REAL(_freq_ui);	//save freq
			_freq_ui *= tmp;
			compute_phase();
			_freq_ui = freq_keep;	//restore freq
			val = _phase_cur;
		}
		break;
	case FN_MIDI_VELOCITY:
		val = midi_velocity_get_float( _channel_id + c_traxs::g_channel_offset, _control_index_ui + c_traxs::g_control_offset );
		break;
#endif
	case FN_FFT:
	case FN_FFT_LINEAR:
		{
			c_snd_input* si = snd::g_master->get_by_channel( _channel_id );
			if( si )
			{
				REAL f_receipt;
				INT32 result = si->get_spectrum_band( &f_receipt, 1, _channel_id, REAL(_phase_ui), REAL(_freq_ui), _fn_type==FN_FFT_LINEAR );
				if( result > 0  )
				{
					val = f_receipt;
				}
				else
				{
					err_print( "got Error %d from get_spectrum_band()", result );
					val = 0.;
				}
			}
		}
		break;
	case FN_BEAT:
		//done in chaumont
		val = (c_snd_input::general_get_area( _channel_id, 0 ) + _phase_ui) * _freq_ui;
		break;
	//todo add c_omg::COMPO not treated here
	case FN_IMAGE_RED:
	case FN_IMAGE_GREEN:
	case FN_IMAGE_BLUE:
	case FN_IMAGE_ALPHA:
	case FN_IMAGE_GREY:
	case FN_IMAGE_SIZE_X:
	case FN_IMAGE_SIZE_Y:
	case FN_IMAGE_SIZE_RATIO:
		if( c_img_2d* img = g_bind_img_2d->get_ready(_control_index_ui) )
		{
			switch( _fn_type )
			{
			case FN_IMAGE_RED:			val = img->get_value_from_uv( REAL(_freq_ui), REAL(_phase_ui), false, aaa::COMPO::RED,	  true );	break;
			case FN_IMAGE_GREEN:		val = img->get_value_from_uv( REAL(_freq_ui), REAL(_phase_ui), false, aaa::COMPO::GREEN, true );	break;
			case FN_IMAGE_BLUE:			val = img->get_value_from_uv( REAL(_freq_ui), REAL(_phase_ui), false, aaa::COMPO::BLUE,  true );	break;
			case FN_IMAGE_ALPHA:		val = img->get_value_from_uv( REAL(_freq_ui), REAL(_phase_ui), false, aaa::COMPO::ALPHA, true );	break;
			case FN_IMAGE_GREY:			val = img->get_value_from_uv( REAL(_freq_ui), REAL(_phase_ui), false, aaa::COMPO::GREY,  true );	break;
			case FN_IMAGE_SIZE_X:		val = img->get_size_x();	break;
			case FN_IMAGE_SIZE_Y:		val = img->get_size_y();	break;
			case FN_IMAGE_SIZE_RATIO:	val = img->get_ratio_x();	break;
			}
		}
		break;
	case FN_VIDEO_RED:
	case FN_VIDEO_GREEN:
	case FN_VIDEO_BLUE:
	case FN_VIDEO_ALPHA:
	case FN_VIDEO_GREY:
		//todo not all the COMPO are here
		if( c_tex_video::cur )	//hack this look strange to me now (Maa)
		{
			auto flux_index = _channel_id-1;
			auto* image_flux = c_tex_video::cur->get_image_flux( flux_index );
			if( image_flux )
			{
				bool b_img_locked = false;
				//todo why we do +1 below. We should deal more globally with this issue and have a way to ask last frame (by passing -1 ?)
				c_img_2d* img = image_flux->lock_and_get_image_index( c_tex_video::cur->get_image_index(flux_index), b_img_locked );
				if( img )
				{
					aaa::COMPO sel;
					switch(_fn_type )
					{ 
					case FN_VIDEO_RED:		sel = aaa::COMPO::RED;		break;
					case FN_VIDEO_GREEN:	sel = aaa::COMPO::GREEN;	break;
					case FN_VIDEO_BLUE:		sel = aaa::COMPO::BLUE;		break;
					case FN_VIDEO_ALPHA:	sel = aaa::COMPO::ALPHA;	break;
					case FN_VIDEO_GREY:		sel = aaa::COMPO::GREY;		break;
					}
					val = img->get_value_from_uv( REAL(_freq_ui), REAL(_phase_ui), false, sel, true );
					if( b_img_locked )
						img->unlock();
				}
			}
		}
		break;
	case FN_VIDEO_CUR_IMAGE_INDEX:
		if( c_tex_video::cur )
			val = c_tex_video::cur->get_image_index(0);
		break;
	case FN_VIDEO_CUR_IMAGE_CHANGED:
		if( c_tex_video::cur )
			val = c_tex_video::cur->is_image_changed(0) ? 1 : 0;
		break;
	case FN_NEAT:
#if	AAA_TRACKER_NEAT()
		val = get_neat( _channel_id, _control_index_ui );
#else
		val = 0;
		err_print_unused( "Neat Device", __FUNCTION__ );
#endif
		break;
	case FN_TOASTER:
#if	AAA_TRACKER_TOASTER()
		if( g_toaster_cur )
			val = g_toaster_cur->get_data_out( _control_index_ui );
#else
		val = 0;
		err_print_unused( "Toaster Device", __FUNCTION__ );
#endif
		break;
//#if AAA_NET()
	case FN_NET_IN:
		if( is_obj_exist_and_active(net) )
			val = c_net::get_matrix_value( _channel_id, _control_index_ui );
		break;
//#endif
	case FN_IN_IS_CHANGED:
		{
			DOUBLE	tmp;
			get_double_in(val);
			tmp = (val == _value_last_low[0]) ? 0 : 1;
			_value_last_low[0] = REAL(val);
			val = tmp;
		}
		break;
	case FN_COPY_BLOCK_REAL:
		{
			p_param	param_in = get_plug_in_first();
			INT32	size = sizeof(REAL) * I_FLOOR( _freq_ui );
			if( param_in && size>0 )
			{
				INT32 i = 0;
				while( p_param param_out = get_plug_out(i) )
					MEMMOVE( (void*)param_out->get_pt_real3(), (void*)param_in->get_pt_const_real3(), size );
			}
		}
		break;
	case FN_OVER_ONE:
		{
			get_double_in( val );
			val = OVER_ONE( val );
		}
		break;
	case FN_LOG:
	case FN_LOG10:
	case FN_EXP:
	case FN_POWER_OF_FREQ:
	case FN_POWER_BY_FREQ:
		{
			p_param	param = get_plug_in_first();
			if( param )
			{
				val = param->get_value_as_real();
				switch(_fn_type )
				{
					case FN_LOG:			val = LOG( val);			break;
					case FN_LOG10:			val = LOG10( val);			break;
					case FN_EXP:			val = EXP( val);			break;
					case FN_POWER_OF_FREQ:	val = POW_D( _freq_ui, val );	break;
					case FN_POWER_BY_FREQ:	val = POW_D( val, _freq_ui );	break;
				}
			}
		}
		break;
	case FN_MIN:
		{
			p_param	param;
			if( param = get_plug_in_first() )
			{
				INT32	i = 1;			
				val = param->get_value_as_real();
				while( param = get_plug_in(i) )
					val = MIN( val, param->get_value_as_real() );
			}
		}
		break;
	case FN_MAX:
	case FN_MAX_WITH_DECAY:
		{
			p_param	param;
			if( param = get_plug_in_first() )
			{
				INT32	i = 1;			
				val = param->get_value_as_real();
				while( param = get_plug_in(i) )
					val = MAX( val, param->get_value_as_real() );
				//exp
				if ( _fn_type == FN_MAX_WITH_DECAY && _value_last_low[0] >= val )
				{
					DOUBLE dt = _delta_t.update_v2();
					if( dt > 0. )
						val = MAX0( _value_last_low[0] - dt*_freq_ui );
				}
			}
		}
		break;
	case FN_ADD:
	case FN_AVERAGE:
		{
			p_param	param;
			INT32	i = 0;
			INT32	count = 0;
			while( param = get_plug_in(i) )
			{
				++count;
				val += param->get_value_as_real();
			}
			if ( _fn_type == FN_AVERAGE && count > 1)
				val /= count;
		}
		break;
	case FN_INTERPOLATE:
		{
			INT32	i = 0;
			p_param param = get_plug_in(i);
			if( param )
			{
				p_param	param_bis = get_plug_in(i);
				if( param_bis )
					val = interpolate( param->get_value_as_real(), param_bis->get_value_as_real(), _freq_ui ); 
			}
		}
		break;
	case FN_MUL:
		{
			INT32	i = 0;
			val = 1;
			while( p_param param = get_plug_in(i) )
				val *= param->get_value_as_real();
		}
		break;
	case FN_NOT_EQUAL:
	case FN_EQUAL:
	case FN_BREAK_NOT_EQUAL:
	case FN_BREAK_EQUAL:
		{
			INT32 i = 0;
			bool  b = false;
			p_param param = get_plug_in(i);
			if( param )
			{
				b = true;
				if( param->is_type_text() )
				{
					o_str& o_ref = o_str::push_name();
					o_str& o_cmp = o_str::push_name();
						param->get_value_as_str( o_ref );
						while( param = get_plug_in(i) )
						{
							if( param->is_type_text() )
							{
								param->get_value_as_str( o_cmp );
								b &= o_ref.is_equal( o_cmp );
								if( !b )
									break;
							}
							else
							{
								b = false;
								break;
							}
						}
					o_str::pop_name();
					o_str::pop_name();
				}
				else
				{
					REAL tmp = param->get_value_as_real();
					while( param = get_plug_in(i) )
					{
						b &= (tmp == param->get_value_as_real());
						if( !b )
							break;
					}
				}
			}
			switch( _fn_type )
			{
			case FN_EQUAL:				val = b?1:0;	break;
			case FN_NOT_EQUAL:			val = b?0:1;	break;
			case FN_BREAK_EQUAL:
				if( b && c_layers::get_cur() )
					c_layers::get_cur()->set_skip_trig();
				break;
			case FN_BREAK_NOT_EQUAL:
				if( !b && c_layers::get_cur() )
					c_layers::get_cur()->set_skip_trig();
				break;
			}
		}
		break;
	case FN_EQUAL_FREQ:
	case FN_NOT_EQUAL_FREQ:
	case FN_EQUAL_ZERO:
	case FN_NOT_EQUAL_ZERO:
	case FN_EQUAL_ONE:
	case FN_NOT_EQUAL_ONE:
	case FN_MORE_THAN_FREQ:
	case FN_MORE_EQUAL_THAN_FREQ:
	case FN_LESS_THAN_FREQ:
	case FN_LESS_EQUAL_THAN_FREQ:
	case FN_INSIDE_FREQ_PHASE:
	case FN_OUTSIDE_FREQ_PHASE:
	case FN_BREAK_EQUAL_FREQ:
	case FN_BREAK_NOT_EQUAL_FREQ:
	case FN_BREAK_EQUAL_ZERO:
	case FN_BREAK_NOT_EQUAL_ZERO:
	case FN_BREAK_EQUAL_ONE:
	case FN_BREAK_NOT_EQUAL_ONE:
	case FN_BREAK_MORE_THAN_FREQ:
	case FN_BREAK_MORE_EQUAL_THAN_FREQ:
	case FN_BREAK_LESS_THAN_FREQ:
	case FN_BREAK_LESS_EQUAL_THAN_FREQ:
	case FN_BREAK_INSIDE_FREQ_PHASE:
	case FN_BREAK_OUTSIDE_FREQ_PHASE:
		{
			get_double_in(val);
			bool b;
			switch( _fn_type )
			{
			case FN_BREAK_EQUAL_FREQ:
			case FN_EQUAL_FREQ:				b = ( val == _freq_ui );					break;
			case FN_BREAK_NOT_EQUAL_FREQ:
			case FN_NOT_EQUAL_FREQ:			b = ( val != _freq_ui );					break;
			case FN_BREAK_EQUAL_ZERO:
			case FN_EQUAL_ZERO:				b = ( val == 0. );						break;
			case FN_BREAK_NOT_EQUAL_ZERO:
			case FN_NOT_EQUAL_ZERO:			b = ( val != 0. );						break;
			case FN_BREAK_EQUAL_ONE:
			case FN_EQUAL_ONE:				b = ( val == 1. );						break;
			case FN_BREAK_NOT_EQUAL_ONE:
			case FN_NOT_EQUAL_ONE:			b = ( val != 1. );						break;
			case FN_BREAK_MORE_THAN_FREQ:
			case FN_MORE_THAN_FREQ:			b = ( val > _freq_ui );					break;
			case FN_BREAK_MORE_EQUAL_THAN_FREQ:
			case FN_MORE_EQUAL_THAN_FREQ:	b = ( val >= _freq_ui );					break;
			case FN_BREAK_LESS_THAN_FREQ:
			case FN_LESS_THAN_FREQ:			b = ( val < _freq_ui );					break;
			case FN_BREAK_LESS_EQUAL_THAN_FREQ:
			case FN_LESS_EQUAL_THAN_FREQ:	b = ( val <= _freq_ui );					break;
			case FN_BREAK_INSIDE_FREQ_PHASE:
			case FN_INSIDE_FREQ_PHASE:		b = INSIDE( val, _freq_ui, _phase_ui );	break;
			case FN_BREAK_OUTSIDE_FREQ_PHASE:
			case FN_OUTSIDE_FREQ_PHASE:		b = OUTSIDE( val, _freq_ui, _phase_ui );	break;
			}
			switch( _fn_type )
			{
			case FN_BREAK_EQUAL_FREQ:
			case FN_BREAK_NOT_EQUAL_FREQ:
			case FN_BREAK_EQUAL_ZERO:
			case FN_BREAK_NOT_EQUAL_ZERO:
			case FN_BREAK_EQUAL_ONE:
			case FN_BREAK_NOT_EQUAL_ONE:
			case FN_BREAK_MORE_THAN_FREQ:
			case FN_BREAK_MORE_EQUAL_THAN_FREQ:
			case FN_BREAK_LESS_THAN_FREQ:
			case FN_BREAK_LESS_EQUAL_THAN_FREQ:
			case FN_BREAK_INSIDE_FREQ_PHASE:
			case FN_BREAK_OUTSIDE_FREQ_PHASE:
				if( b && c_layers::get_cur() )
					c_layers::get_cur()->set_skip_trig();
				break;
			}
			val = b ? 1 : 0;
		}
		break;
	//todo regroup with proximity without losing perf (template ?)
	case FN_INSIDE_FREQ_PHASE_LINEAR:
	case FN_INSIDE_FREQ_PHASE_SINUS:
		{
			p_param param = get_plug_in_first();
			if( param )
			{
				val = param->get_value_as_real();
				REAL	a = REAL(_freq_ui);
				REAL	b = REAL(_phase_ui);
				ORDER( a, b );
				if( val < a-_ease_before )
				{}
				else if ( val < a )
				{
					val = (_ease_before+val-a)/_ease_before;
					if( _fn_type == FN_INSIDE_FREQ_PHASE_SINUS )
						val = (1. - COS_TURN( val * .5  ) ) * .5 ;
				}
				else if ( val <= b )
				{
					val = 1.;
				}
				else if ( val < b+_ease_after )
				{
					val = (_ease_after+b-val)/_ease_after;
					if( _fn_type == FN_INSIDE_FREQ_PHASE_SINUS )
						val = (1. - COS_TURN( val * .5 )) * .5 ;
				}
			}
		}
		break;
	case FN_PROXIMITY_FREQ_LINEAR:
	case FN_PROXIMITY_FREQ_SINUS:
		{
			p_param param = get_plug_in_first();
			if( param )
			{
				val = param->get_value_as_real();
				REAL	a = REAL(_freq_ui);
				if( val < a-_ease_before )
				{}
				else if ( val < a )
				{
					val = (_ease_before+val-a)/_ease_before;
					if( _fn_type == FN_PROXIMITY_FREQ_SINUS )
						val = (1. - COS_TURN( val * .5 ) ) * .5 ;
				}
				else if ( val <= a )
				{
					val = 1.;
				}
				else if ( val < a + _ease_after )
				{
					val = ( _ease_after+ a - val ) / _ease_after;
					if( _fn_type == FN_PROXIMITY_FREQ_SINUS )
						val = (1. - COS_TURN( val * .5 ) ) * .5 ;
				}
			}
		}
		break;
	case FN_VALUE:
		if( c_layers::get_cur() )
			val = c_layers::get_cur()->get_values()->get_from_index( _control_index_ui );
		else
			err_print( "no current layers here" );
		break;
	case FN_VALUE_MODULE:
		if( c_module::get_cur() )
			val = c_module::get_cur()->get_values()->get_from_index( _control_index_ui );
		else
			err_print( "no current module here" );
		break;
	case FN_VALUE_GLOBAL:
		val = c_modules::get_cur()->get_values()->get_from_index( _control_index_ui );
		break;
//todoqq	implement local version
	case FN_MORE_THAN_VALUE_INDEX:
	case FN_MORE_THAN_VALUE_GLOBAL_INDEX:
		{
			c_obj_value* values;

			if( _fn_type == FN_MORE_THAN_VALUE_GLOBAL_INDEX )
				values = c_modules::get_cur()->get_values();
			else if( _fn_type == FN_MORE_THAN_VALUE_MODULE_INDEX )
				if( c_module::get_cur() )
					values = c_module::get_cur()->get_values();
				else
				{
					err_print( "no current module here" );
					values = 0;
				}		
			else
				if( c_layers::get_cur() )
					values = c_layers::get_cur()->get_values();
				else
				{
					err_print( "no current layers here" );	//hacknow move to a trax message
					values = 0;
				}


			get_double_in( val );
			
			INT32 i;
			for( i = _control_index_ui; i <= _control_index_bis_ui; ++i )
			{
				if( val <= values->get_from_index( i ) )
					break;
			}
			val = i-_control_index_ui;
		}
		break;
	case FN_SORT_VALUE_INDEX_FROM_MIN:
		if( c_layers::get_cur() )
			val = c_layers::get_cur()->get_values()->get_sort_index_from_min( _control_index_ui, _control_index_bis_ui, INT32(_freq_ui) );
		else
			err_print( "no current layers here" );
		break;
	case FN_SORT_VALUE_GLOBAL_INDEX_FROM_MIN:
		val = c_modules::get_cur()->get_values()->get_sort_index_from_min( _control_index_ui, _control_index_bis_ui, INT32(_freq_ui) );
		break;
	case FN_SORT_VALUE_MODULE_INDEX_FROM_MIN:
		if( c_module::get_cur() )
			val = c_module::get_cur()->get_values()->get_sort_index_from_min( _control_index_ui, _control_index_bis_ui, INT32(_freq_ui) );
		else
			err_print( "no current module here" );
		break;
	case FN_SORT_VALUE_INDEX_FROM_MAX:
		if( c_layers::get_cur() )
			val = c_layers::get_cur()->get_values()->get_sort_index_from_max( _control_index_ui, _control_index_bis_ui, INT32(_freq_ui) );
		else
		{
			err_print( "no current layers here" );
			val = 0;
		}
		break;
	case FN_SORT_VALUE_MODULE_INDEX_FROM_MAX:
		if( c_module::get_cur() )
			val = c_module::get_cur()->get_values()->get_sort_index_from_max( _control_index_ui, _control_index_bis_ui, INT32(_freq_ui) );
		else
			err_print( "no current module here" );
		break;
	case FN_SORT_VALUE_GLOBAL_INDEX_FROM_MAX:
		val = c_modules::get_cur()->get_values()->get_sort_index_from_max( _control_index_ui, _control_index_bis_ui, INT32(_freq_ui) );
		break;
	case FN_TRAX:
		{
			//HACK	Deal with 3D case
			c_trax* pt = ((c_traxs*)get_root())->get_trax( _control_index_ui );
			//HACK	no access to flux here
			if( pt && pt->FLUX )
				val = pt->FLUX->get();
		}
		break;
	case FN_MULTIPLE_INDEX:		val = c_multiple::cur ? c_multiple::cur->get_index()   *_freq_ui	: 0. ;	break;
	case FN_MULTIPLE_INDEX_U:	val = c_multiple::cur ? c_multiple::cur->get_index_u() *_freq_ui	: 0. ;	break;
	case FN_MULTIPLE_INDEX_V:	val = c_multiple::cur ? c_multiple::cur->get_index_v() *_freq_ui	: 0. ;	break;
	case FN_MULTIPLE_INDEX_W:	val = c_multiple::cur ? c_multiple::cur->get_index_w() *_freq_ui	: 0. ;	break;

	case FN_MULTIPLE_REAL:		val = c_multiple::cur ? c_multiple::cur->get_real()   *_freq_ui		: 0. ;	break;
	case FN_MULTIPLE_REAL_U:	val = c_multiple::cur ? c_multiple::cur->get_real_u() *_freq_ui		: 0. ;	break;
	case FN_MULTIPLE_REAL_V:	val = c_multiple::cur ? c_multiple::cur->get_real_v() *_freq_ui		: 0. ;	break;
	case FN_MULTIPLE_REAL_W:	val = c_multiple::cur ? c_multiple::cur->get_real_w() *_freq_ui		: 0. ;	break;

	case FN_MULTIPLE_NB:		val = c_multiple::cur ? c_multiple::cur->get_nb()   *_freq_ui		: 0. ;	break;
	case FN_MULTIPLE_NB_U:		val = c_multiple::cur ? c_multiple::cur->get_nb_u() *_freq_ui		: 0. ;	break;
	case FN_MULTIPLE_NB_V:		val = c_multiple::cur ? c_multiple::cur->get_nb_v() *_freq_ui		: 0. ;	break;
	case FN_MULTIPLE_NB_W:		val = c_multiple::cur ? c_multiple::cur->get_nb_w() *_freq_ui		: 0. ;	break;

	case FN_MULTIPLE_PARAMETER:	val = c_multiple::cur ? c_multiple::cur->get_parameter( _control_index_ui ) *_freq_ui	: 0. ;	break;
	case FN_NET_HOST_ID:		val = net ? net->get_host_id() : 0;										break;

	case FN_NET_HOST_ID_EQUAL_FREQ:
	case FN_NET_HOST_ID_NOT_EQUAL_FREQ:
	case FN_NET_HOST_ID_MORE_THAN_FREQ:
	case FN_NET_HOST_ID_MORE_EQUAL_THAN_FREQ:
	case FN_NET_HOST_ID_LESS_THAN_FREQ:
	case FN_NET_HOST_ID_LESS_EQUAL_THAN_FREQ:
		if( net )
		{
			bool	b;
			val = net->get_host_id();
			switch( _fn_type )
			{
			case FN_NET_HOST_ID_EQUAL_FREQ:				b = ( val == _freq_ui );	break;
			case FN_NET_HOST_ID_NOT_EQUAL_FREQ:			b = ( val != _freq_ui );	break;
			case FN_NET_HOST_ID_MORE_THAN_FREQ:			b = ( val > _freq_ui );	break;
			case FN_NET_HOST_ID_MORE_EQUAL_THAN_FREQ:	b = ( val >= _freq_ui );	break;
			case FN_NET_HOST_ID_LESS_THAN_FREQ:			b = ( val < _freq_ui );	break;
			case FN_NET_HOST_ID_LESS_EQUAL_THAN_FREQ:	b = ( val <= _freq_ui );	break;
			}
			val = b ? 1 : 0 ;
		}
		break;
	case FN_MACHINE:
	case FN_LANGROUP:
		if( net )
		{
			_str_out = (_fn_type==FN_MACHINE) ? net->get_machine() : net->get_langroup();
		}
		break;
	case FN_MACHINE_EQUAL_NAME:
	case FN_LANGROUP_EQUAL_NAME:
		if( net )
		{
			C_PCHAR_C s = (_fn_type==FN_MACHINE_EQUAL_NAME) ? net->get_machine() : net->get_langroup();
			val = str_is_equal_nocase( s, _var_name.get() ) ? 1. : 0. ;
		}
		break;
	case FN_RND_LIN:
	case FN_RND_GAUSS:
		{
			compute_phase();
			auto tmp = _phase_cur - _phase_last;
			if( tmp >= 1. || tmp < 0. ) //todo when the phase cross up or down an integer phase
			{
//				value = trax_rand_lin.get_ufloat_with_seed( *((UINT32*)(&_phase )) );
				if( _fn_type == FN_RND_GAUSS )
					val = trax_rand_gauss.get_fp32_01();
				else
					val = trax_rand_lin.get_fp32_01();

				_phase_last = F_FLOOR(_phase_cur );
//todoqq	bug in Release
//				DBG_PRINT_STRING( "%d %d" , _phase_last, _phase_cur );
			}
			else
				val = _value_last_low[0];
		}
		break;
/*	
	case FN_BREAK:
	{
		p_param	param = get_plug_in_first();
		if( param )
		{
			val = param->get_value_as_real();
			if( val >= .5 && c_layers::get_cur() )
				c_layers::get_cur()->set_skip_trig();
		}
	}
	break;
*/
	case FN_SCREEN_INDEX:	val = c_multi_screen::cur->get_index();		break;
	case FN_SCREEN_X_INDEX:	val = c_multi_screen::cur->get_x_index();	break;
	case FN_SCREEN_Y_INDEX:	val = c_multi_screen::cur->get_y_index();	break;
	case FN_SCREEN_NB:		val = c_multi_screen::cur->get_nb();		break;
	case FN_SCREEN_X_NB:	val = c_multi_screen::cur->get_x_nb();		break;
	case FN_SCREEN_Y_NB:	val = c_multi_screen::cur->get_y_nb();		break;

	case FN_CURVE_FROM_PHASE:
	case FN_CURVE_X_FROM_PHASE:
	case FN_CURVE_Y_FROM_PHASE:
	case FN_CURVE_Z_FROM_PHASE:
	case FN_CURVE:
	case FN_CURVE_BY_NAME:
	case FN_CURVE_X:
	case FN_CURVE_Y:
	case FN_CURVE_Z:
		if( c_bdd_curve_edit* bdd_curve_edit = c_bdd_curve_edit::get_from_channel( _channel_id ) )
		{	
			switch( _fn_type )
			{	
			case FN_CURVE_FROM_PHASE:	compute_phase();
										val = bdd_curve_edit->control_get_value( _control_index_ui+1, REAL(_phase_cur) );
										break;
			case FN_CURVE_X_FROM_PHASE:
			case FN_CURVE_Y_FROM_PHASE:
			case FN_CURVE_Z_FROM_PHASE:	compute_phase();
										val = bdd_curve_edit->control_get_value_axe( _control_index_ui+1, REAL(_phase_cur), _fn_type-FN_CURVE_X_FROM_PHASE );
										break;
			case FN_CURVE:				val = bdd_curve_edit->control_get_value( _control_index_ui+1 );
										break;
			case FN_CURVE_BY_NAME:		val = bdd_curve_edit->control_get_value_by_name( _var_name.get() );
										break;
			case FN_CURVE_X:
			case FN_CURVE_Y:
			case FN_CURVE_Z:			val = bdd_curve_edit->control_get_value_axe( _control_index_ui+1, _fn_type-FN_CURVE_X );
										break;
			}
		}
		break;
	case FN_LUA:	//todo deal with string here and in trax in general
		if( !_var_name.is_empty() )
			val = g_lua_wrap_cur->do_trax( this, _var_name.get() );
		break;
	case FN_CELL_SIZE:
		{
			c_obj_ui* obj = focus_param::get_obj();
			if( obj )
				val = obj->get_cell_size();
		}
		break;
	case FN_3D_DISTANCE:
	//	add square
		{
			INT32	i = 0;
			p_param	param  = get_plug_in(i);
			val = 0;
			if( param )
			{
				p_param	param_bis = get_plug_in(i);
				if( param_bis )
					val = dist_v3r( param->get_pt_const_real3(), param_bis->get_pt_const_real3() );
				else
					val = norm_v3r( param->get_pt_const_real3() );
			}
		}
		break;
	case FN_3D_DISTANCE_MOCAP:
		{
			c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _channel_id );
			if( mocap )
			{
				c_bdd_mocap*	mocap_bis = c_bdd_mocap::get_from_channel( _channel_id_bis );
				if( mocap_bis )
				{
					DOUBLE	src1[3];
					DOUBLE	src2[3];
					mocap->get_tra_cur( src1, _control_index_ui+1 );
					mocap_bis->get_tra_cur( src2, _control_index_bis_ui+1 );
					val = dist_v3r( src1, src2 );
				}
			}
		}
		break;
	case FN_3D_DISTANCE_CAMERA:
		{	// we use the current camera so the one used in the previous layers
			if( c_seedcam::get_cur() )
			{
				p_param	param = get_plug_in_first();
				if( param )
					val = c_seedcam::get_cur()->get_dist( param->get_pt_const_real3() );
				else
					val = norm_v3r( c_seedcam::get_cur()->get_position_pt() );
			}
		}
		break;
	default:
		err_print( "This trax type unimplemented : %s", str_fn_type[_fn_type] );
		break;
	}	//switch( _fn_type )
	_value_fn_out =  REAL(val);
}

FINLINE	void	c_trax::compute_fn_3d( DOUBLE* val )
{
	switch( _fn_type )
	{
	case FN_3D_MOCAP:
		{
			c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _channel_id );
			if( mocap )
				mocap->get_tra_cur( val, _control_index_ui+1 );
		}
		break;
	case FN_3D_MOCAP_SCA:
		{
			c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _channel_id );
			if( mocap )
				mocap->get_sca_cur( val, _control_index_ui+1 );
		}
		break;
	case FN_3D_BDD_TRI_CENTER:
	case FN_3D_BDD_TRI_BARYCENTER:
	case FN_3D_BDD_TRI_MIN:
	case FN_3D_BDD_TRI_MAX:
	case FN_3D_BDD_TRI_SIZE:
	case FN_3D_BDD_TRI_TEX_CENTER:
	case FN_3D_BDD_TRI_TEX_SIZE:
		{
			c_bdd_tri*	tri = c_bdd_tri::get_from_channel( _channel_id );
			if( tri )
			{
				switch( _fn_type )
				{
				case FN_3D_BDD_TRI_CENTER:		tri->get_origin_from_control(		val, _control_index_ui+1 );	break;
				case FN_3D_BDD_TRI_BARYCENTER:	tri->get_barycenter_from_control(	val, _control_index_ui+1 );	break;
				case FN_3D_BDD_TRI_MIN:			tri->get_min_from_control(			val, _control_index_ui+1 );	break;
				case FN_3D_BDD_TRI_MAX:			tri->get_max_from_control(			val, _control_index_ui+1 );	break;
				case FN_3D_BDD_TRI_SIZE:		tri->get_size_from_control(			val, _control_index_ui+1 );	break;
				case FN_3D_BDD_TRI_TEX_CENTER:	tri->get_tex_center_from_control(	val, _control_index_ui+1 );	break;
				case FN_3D_BDD_TRI_TEX_SIZE:	tri->get_tex_size_from_control(		val, _control_index_ui+1 );	break;
				}
			}
		}
		break;
	case FN_3D_MOCAP_AXE_X:
	case FN_3D_MOCAP_AXE_Y:
	case FN_3D_MOCAP_AXE_Z:
		{
			c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _channel_id );
			if( mocap )
				mocap->get_axe_cur( val, _control_index_ui+1, _fn_type - FN_3D_MOCAP_AXE_X );
		}
		break;
	case FN_3D_MOCAP_TGN:
		{
			c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _channel_id );
			if( mocap )
			{
				REAL vec[3];
				mocap->get_tangent( vec, _control_index_ui+1 );
				cpy_v3( val, vec );
			}
		}
		break;
	case FN_3D_MOCAP_FEED:
		{
			c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _channel_id );
			if( mocap )
			{
				p_param	param;
				if ( param = get_plug_in_first() )
				{
					REAL vec[3];	//todo used to handle possible conversion : do better	
					cpy_v3( val, param->get_pt_const_real3() );
					cpy_v3( vec, val );
					mocap->store_tra( _control_index_ui+1, vec );
				}
			}
		}
		break;
	case FN_3D_CONTROL_SET:
		{
			p_param	param;
			if ( param = get_plug_in_first() )
			{
				cpy_v3( val, param->get_pt_const_real3() );
			}
		}
		break;
//		case FN_3D_VARIABLE:
/*		{
			REAL	vec[3];
			if(	aaa::vars_double_3d.get( _var_name.get(), vec ) )
				cpy_v3r( val, vec );
		}
*/
//		break;
//		case FN_3D_VARIABLE_SET:
/*			{
		p_param	param;
		if ( param = get_plug_in_first() )
			{
			REAL	vec[3];
			cpy_v3r( vec, (REAL *)(param->get_pt()) );
			aaa::vars_double_3d.set( _var_name.get(), vec );
			}
		}
*/
//			break;
	case FN_3D_PATH_POS:
		{	//	go from a phase on the 3D_PATH to the xyz position
			c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _channel_id );
			if( mocap )
			{
				REAL vec[3];
				compute_phase();
				mocap->get_tra( vec, _control_index_ui+1, REAL(_phase_cur) );
				cpy_v3( val, vec );
			}
		}
		break;
	case FN_3D_PATH_TO_WORLD:
		{	//	go from a coor relative to the 3D_PATH ( s t y ) to the world xyz position
			c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _channel_id );
			if( mocap )
			{
				p_param	param;
				if( param = get_plug_in_first() )
				{
					REAL vec[3];
					mocap->coor_to_world_one( vec, _control_index_ui+1, param->get_pt_const_real3() );	//todo make it better
					cpy_v3( val, vec );
				}
			}
		}
		break;
	case FN_3D_PATH_SCA:
		{
			c_bdd_mocap*	mocap = c_bdd_mocap::get_from_channel( _channel_id );
			if( mocap )
			{
				REAL vec[3];
				compute_phase();
				mocap->get_sca( vec, _control_index_ui+1, REAL(_phase_cur) );
				cpy_v3( val, vec );
			}
		}
		break;
	case FN_3D_CHANGE_LINEAR:
	case FN_3D_CHANGE_SINUS:
		{
			p_param	param = get_plug_in_first();
			if( param )
				cpy_v3( val, param->get_pt_const_real3() );
			else
				clear_v3( val );
			if( is_diff_v3( val, _change_val_new ) )
			{
				cpy_v3( _change_val_old, _change_val_last );
				cpy_v3( _change_val_new, val );
	//				DBG_PRINT_STRING( "CHANGE  %f", _change_val_new[0] );
				_b_restart_trig_ui = true;
			}
			compute_phase_time();
	//			DBG_PRINT_STRING( "phase %f", _phase_cur );
			if( _phase_cur <= 0 )
				cpy_v3( val, _change_val_old );
			else if( _phase_cur < 1 )
			{
				REAL tmp;
				if( _fn_type == FN_3D_CHANGE_SINUS )
					tmp = (  REAL(1) - COS_TURN( _phase_cur * .5 ) ) *  REAL(.5) ;
				else
					tmp =  REAL(_phase_cur);

				gain_slick_if_needed( tmp, _gain );
				bias_slick_if_needed( tmp, _bias );

				interpolate_v3( val, _change_val_old, val, tmp );
			}
			else
				cpy_v3( _change_val_old, _change_val_new );
			cpy_v3( _change_val_last, val );
		}
		break;
	case FN_3D_COPY:
		{
			p_param	param = get_plug_in_first();
			if( param )
				cpy_v3( val, param->get_pt_const_real3() );
		}
		break;
	case FN_3D_INTERPOLATE:
		{
			INT32		i = 0;
			clear_v3( val );
			p_param param = get_plug_in(i);
			if( param )
			{
				p_param	param_bis = get_plug_in(i);
				if( param_bis )
					interpolate_v3( val, param->get_pt_const_real3(), param_bis->get_pt_const_real3(), _freq_ui ); 
			}
		}
		break;
	case FN_3D_ADD:
	case FN_3D_AVERAGE:
		{
			INT32		i = 0;
			INT32		count = 0;
			clear_v3( val );
			while( p_param param = get_plug_in( i ) )
			{
				++count;
				add_v3( val, param->get_pt_const_real3() );
			}
			if ( _fn_type == FN_3D_AVERAGE && count > 1 )
				scale_v3( val, 1. / count );
		}
		break;
	case FN_3D_RND_LIN:
	case FN_3D_RND_GAUSS:
		compute_phase();
		if( _phase_cur - _phase_last >= 1. )
		{
//									value = trax_rand_lin.get_ufloat_with_seed( *((UINT32*)(&_phase )) );
			if ( _fn_type == FN_3D_RND_GAUSS )
			{
				val[0] = trax_rand_gauss.get_fp32_01();
				val[1] = trax_rand_gauss.get_fp32_01();
				val[2] = trax_rand_gauss.get_fp32_01();
			}
			else
			{
				val[0] = trax_rand_lin.get_fp32_01();
				val[1] = trax_rand_lin.get_fp32_01();
				val[2] = trax_rand_lin.get_fp32_01();
			}
			_phase_last = F_FLOOR(_phase_cur );
		}
		else
		{
			cpy_v3( val, _value_last_low );
		}
		break;
	case FN_3D_RGB_TO_HSV:
	case FN_3D_HSV_TO_RGB:
		{
			p_param	param = get_plug_in_first();
			if( param )
			{
				//todoq there is a pb here with FP32 param should be change
				FP32 src[3];
				cpy_v3( src, param->get_pt_const_real3() );
				FP32 f[3];
				switch( _fn_type )
				{
				case FN_3D_RGB_TO_HSV:	aaa::color::hsv_from_rgb( f, src );	break;
				case FN_3D_HSV_TO_RGB:	aaa::color::rgb_from_hsv( f, src );	break;
				}
				cpy_v3( val, f );
			}
		}
		break;
	case FN_3D_IMAGE_COLOR:
	case FN_3D_IMAGE_COLOR_CLAMPED:
		if( c_img_2d* img = g_bind_img_2d->get_ready(_control_index_ui) )
		{
			FP32 f[3];
			img->get_color3r_from_uv( f, REAL(_freq_ui), REAL(_phase_ui), _fn_type==FN_3D_IMAGE_COLOR_CLAMPED );
			cpy_v3( val, f );
		}
		break;
	case FN_3D_VIDEO_COLOR:
	case FN_3D_VIDEO_COLOR_CLAMPED:
		if( c_tex_video::cur )
			if( c_img_2d* img = (c_img_2d*) c_tex_video::cur->get_image_flux(_channel_id-1) )
			{
				FP32 f[3];
				img->get_color3r_from_uv( f, REAL(_freq_ui), REAL(_phase_ui), _fn_type==FN_3D_VIDEO_COLOR_CLAMPED );
				cpy_v3( val, f );
			}
		break;
	case FN_3D_CONTROL:	//todoopt do a unique fn for this
		val[0] = g_datacube->get_double( _channel_id, _control_index_ui, _control_index_bis_ui );
		val[1] = g_datacube->get_double( _channel_id, _control_index_ui, _control_index_bis_ui+1);
		val[2] = g_datacube->get_double( _channel_id, _control_index_ui, _control_index_bis_ui+2);
		break;
	case FN_3D_CURVE:
		{
			clear_v3( val );
			c_bdd_curve_edit*	bdd_curve_edit = c_bdd_curve_edit::get_from_channel( _channel_id );
			if( bdd_curve_edit )
			{
				val[0] = bdd_curve_edit->control_get_value_axe( _control_index_ui+1, 0 );
				val[1] = bdd_curve_edit->control_get_value_axe( _control_index_ui+1, 1 );
				val[2] = bdd_curve_edit->control_get_value_axe( _control_index_ui+1, 2 );
			}
		}
		break;
	case FN_3D_CURVE_FROM_PHASE:
		{
			clear_v3( val );
			c_bdd_curve_edit*	bdd_curve_edit = c_bdd_curve_edit::get_from_channel( _channel_id );
			if( bdd_curve_edit )
			{
				compute_phase();
				val[0] = bdd_curve_edit->control_get_value_axe( _control_index_ui+1, REAL(_phase_cur), 0 );
				val[1] = bdd_curve_edit->control_get_value_axe( _control_index_ui+1, REAL(_phase_cur), 1 );
				val[2] = bdd_curve_edit->control_get_value_axe( _control_index_ui+1, REAL(_phase_cur), 2 );
			}
		}
		break;
	case FN_3D_VALUE:
		if( c_layers::get_cur() )
		{
			//todoopt by adding one call to c_obj_value
			c_obj_value* values = c_layers::get_cur()->get_values();
			val[0] = values->get_from_index( _control_index_ui );
			val[1] = values->get_from_index( _control_index_ui + 1 );
			val[2] = values->get_from_index( _control_index_ui + 2 );
		}
		else
		{
			err_print( "no current layers here" );
		}
		break;
	case FN_3D_VALUE_MODULE:
		if( c_layers::get_cur() )
		{
			//todoopt by adding one call to c_obj_value
			c_obj_value* values = c_layers::get_cur()->get_values();
			val[0] = values->get_from_index( _control_index_ui );
			val[1] = values->get_from_index( _control_index_ui + 1 );
			val[2] = values->get_from_index( _control_index_ui + 2 );
		}
		else
		{
			err_print( "no current module here" );
		}
		break;
	case FN_3D_VALUE_GLOBAL:
		{
			//todoopt by adding one call to c_obj_value
			c_obj_value* values = c_modules::get_cur()->get_values();
			val[0] = values->get_from_index( _control_index_ui );
			val[1] = values->get_from_index( _control_index_ui + 1 );
			val[2] = values->get_from_index( _control_index_ui + 2 );
		}
		break;
	case FN_3D_MULTIPLE_POS:
		if( c_multiple::cur )
			cpy_v3( val, c_multiple::cur->get_pos_draw() );
		break;
	case FN_3D_MULTIPLE_SIZE:
		if( c_multiple::cur )
			cpy_v3( val, c_multiple::cur->get_size_draw() );
		break; 
	case FN_3D_BDD_POINT_RAW:
	case FN_3D_BDD_POINT_AVERAGE:
		{
			REAL vec[3];
			bdd_point_cur->ctl_get_pos( vec, _control_index_ui, _fn_type==FN_3D_BDD_POINT_AVERAGE );
			cpy_v3( val, vec );
		}
		break; 				
	//case FN_3D_BDD_POINT_NB:
	//	bdd_point_cur->ctl_get_nb_active( val, _control_index_ui, _fn_type==FN_3D_BDD_POINT_AVERAGE );
	//	break; 			
	case FN_3D_BDD_INTERSECTION:
		{
			c_bdd*	bdd;
			
			clear_v3( val );
			if( !_var_name.is_empty() )
			{
				//if( !bdd || !bdd->get_root() || strcmp( target_name_symbo.get(), bdd->get_name_symbo()->get())!=0 )
				bdd = find_bdd_by_name_symbo( _var_name );
			}
			else
				bdd = nullptr;
			if( bdd )
			{
				INT32		i = 0;
				p_param param = get_plug_in(i);
				if( param )
				{
					p_param	param_bis = get_plug_in(i);
					if( param_bis )
					{
						REAL vec[3];
						bdd->compute_intersection( param->get_pt_const_real3(), param_bis->get_pt_const_real3(), vec ); 
						cpy_v3( val, vec );
					}
				}
			}
		}
		break;
	case FN_3D_2_POINTS_TO_YX_ROT:
		{
			INT32		i = 0;
			p_param param = get_plug_in(i);
			clear_v3( val );
			if( param )
			{
				p_param	param_bis = get_plug_in(i);
				if( param_bis )
				{
					REAL	vec[3];
					sub_v3( vec, param_bis->get_pt_const_real3(), param->get_pt_const_real3() );
					val[1] = atan2( vec[0], vec[2] ) / PI_TIME_2 + .5;
					val[0] = atan2( vec[1], sqrt(vec[0]*vec[0]+vec[2]*vec[2]) ) / PI_TIME_2;		
				}
			}
		}
		break;
	case FN_3D_CELL_POS:
		{
			c_obj_ui*	obj = focus_param::get_obj();
			if( obj )
				cpy_v3( val, obj->get_cell_pos() );
		}
		break;
	default:
		err_print( "This trax type unimplemented : %s", str_fn_type[_fn_type] );
		break;
	}	//switch( _fn_type )
}

FINLINE	void	c_trax::update_low_1d()
{
	DOUBLE	val;
	CHAR*	str = nullptr;
	switch( _output_type )
	{
	case OUTPUT_IN:
		{
			p_param	param = get_plug_in_first();
			if( param )
				val = param->get_value_as_real();
			else
				val = 0.;
	/*				c_connex* con = get_parent_connex(0);
			if( con )
				val = con->get_param_in_set()->get_value_as_real();
			else
				val = 0;
*/		}
		break;
	case OUTPUT_MIN:	val = 0;	break;
	case OUTPUT_MIDDLE:	val = .5;	break;
	case OUTPUT_MAX:	val = 1.;	break;

	case OUTPUT_FN_THRESHOLD:
	case OUTPUT_FN_TRIG_UP_AND_DOWN:
	case OUTPUT_FN_TRIG_DOWN:
	case OUTPUT_FN_TRIG_UP:
	case OUTPUT_FN:
	case OUTPUT_RECORDER:
		compute_fn_1d( val );
		if( _isnan( val ) )
		{
			err_print( "c_trax::compute_fn_1d() type %.64s return a Nan : Not a number", str_fn_type[_fn_type] );
			val = 0.;
		}
/*
		else
		{
			GOOD_PRINT_STRING( "c_trax::compute_fn_1d() type %.64s return OK, from %.256s", str_fn_type[_fn_type], get_my_filename() );
		}
*/
		break;
	}
	if( !_str_out )
	{
		REAL r_val = REAL(val);
		if( _output_type == OUTPUT_RECORDER ) //todoq treat 3D case
		{
			if( _b_record )
			{
				if ( !_p_event )
					alloc_events();
				if( r_val != _value_last_low[0] )
				{
					if( _p_event_cur <= _p_event_limit )
					{
						_p_event_cur->timestamp = trax_time;
						_p_event_cur->value = r_val;
						++_p_event_cur;
						++_event_index_max;
					}
					else
					{
						if ( _p_event )
							box_err( "Too many events in this trax" );
						else
							box_err( "No events buffer allocated in this trax" );		
					}
				}
			}
		}
		if( _fn_type != FN_IN_IS_CHANGED )
			_value_last_low[0] = r_val;
		switch( _fn_type )
		{
		case FN_SQUARE:
			gain_slick_if_needed( val, _gain );
			bias_slick_if_needed( val, _bias );
			if ( val < .5)
				val = 0;
			else
				val = 1;
			break;
		case FN_CHANGE_LINEAR:
		case FN_CHANGE_SINUS:
			break;
		default:
			gain_slick_if_needed( val, _gain );
			bias_slick_if_needed( val, _bias );
			break;
		}
		if( _round != 0. )
			val = ROUND_FLOOR( val, _round );
		switch( _output_type )
		{
		case OUTPUT_FN_THRESHOLD:
			val = (val >= _threshold ) ? 1. : 0.;
			break;
		case OUTPUT_FN_TRIG_UP_AND_DOWN:
			{
				bool b = (val >= _threshold );
				val = (b != _b_trig_last ) ? 1 : 0;
				_b_trig_last = b;
			}
			break;
		case OUTPUT_FN_TRIG_DOWN:
			{
				bool b = (val >= _threshold );
				val = (!b && (b != _b_trig_last )) ? 1 : 0;
				_b_trig_last = b;
			}
			break;
		case OUTPUT_FN_TRIG_UP:
			{
				bool b = (val >= _threshold );
				val = (b && (b != _b_trig_last )) ? 1 : 0;
				_b_trig_last = b;
			}
			break;
		}
		val = _min + (_max-_min )*val + _offset;
		switch( _s_limit )
		{
		case LIMIT_ABS_CLAMP:
			val = ABS(val);
		case LIMIT_CLAMP:
			CLAMP_REF( val, _limit_min, _limit_max );
			break;
		case LIMIT_ABS_WRAP:
			val = ABS(val);
		case LIMIT_WRAP:
			val = FWRAP( val, _limit_min, _limit_max );
			break;
		case LIMIT_ABS_BOUNCE:
			val = ABS(val);
		case LIMIT_BOUNCE:
			val = BOUNCE( val, _limit_min, _limit_max );
			break;
		case LIMIT_ABS:
			val = ABS(val);
			break;
		default:
			break;
		}
		r_val = REAL(val);
		if( FLUX->put( r_val, this ) )
		{
			//todo should we use FLUX->get() below
//#if	AAA_NET()
			if( is_obj_exist_and_active(net) )
			{
				if( is_send_needed( _s_net_out ) )
					net->send_trax( get_net_channel_id(), get_net_control_id(), r_val );	//todoq treat 3D case
				if( _s_max_out == MAX_OUT_SEND )
					net->send_midi_control( _max_channel_id, _max_control_id, r_val );	//todoq treat 3D case
			}
//#endif
//hack
			if( _s_max_out == 1 )
			{
				if( midi_array[0])
				{
					midi_array[0]->set_channel( _max_channel_id );
					midi_array[0]->send_control_change( _max_control_id, (INT32)r_val );
				}
			}
		}
	}
}

FINLINE	void	c_trax::update_low_3d()
{
	DOUBLE	val[3];
	switch( _output_type )
	{
	case OUTPUT_IN:
		{	//todo do better
			p_param header = get_param_header();
			c_connex* con = header->get_in(0);
			clear_v3(val);
			if( con )
				val[0] = con->get_param_in_set()->get_value_as_real();
		}
		break;
	case OUTPUT_MIN:
		clear_v3(val);
		break;
	case OUTPUT_MIDDLE:
		set_v3( val, .5 );
		break;
	case OUTPUT_MAX:
		set_v3( val, 1. );
		break;
	case OUTPUT_FN_THRESHOLD:
	case OUTPUT_FN_TRIG_UP_AND_DOWN:
	case OUTPUT_FN_TRIG_DOWN:
	case OUTPUT_FN_TRIG_UP:
	case OUTPUT_FN:
	case OUTPUT_RECORDER:
		clear_v3( val );
		compute_fn_3d( val );
		break;
	}
	cpy_v3( _value_last_low, val);
	switch( _fn_type )
	{
	case FN_3D_CHANGE_LINEAR:
	case FN_3D_CHANGE_SINUS:
		break;
	default:
		gain_slick_if_needed_3d( val, _gain );
		bias_slick_if_needed_3d( val, _bias );
		break;
	}
	if( _round != 0. )
	{
		DOUBLE r = DOUBLE(_round );
		val[0]= ROUND_FLOOR( val[0], r );
		val[1]= ROUND_FLOOR( val[1], r );
		val[2]= ROUND_FLOOR( val[2], r );
	}
	switch( _output_type )
	{
	case OUTPUT_FN_THRESHOLD:
		val[0] = (val[0] >= _threshold ) ? 1. : 0.;
		val[1] = (val[1] >= _threshold ) ? 1. : 0.;
		val[2] = (val[2] >= _threshold ) ? 1. : 0.;
		break;
	case OUTPUT_FN_TRIG_UP_AND_DOWN:
		{
			bool b;
			b = (val[0] >= _threshold );
			val[0] = (b != _b_trig_last_3d[0]) ? 1 : 0;
			_b_trig_last_3d[0] = b;
			b = (val[1] >= _threshold );
			val[1] = (b != _b_trig_last_3d[1]) ? 1 : 0;
			_b_trig_last_3d[1] = b;
			b = (val[2] >= _threshold );
			val[2] = (b != _b_trig_last_3d[2]) ? 1 : 0;
			_b_trig_last_3d[2] = b;
		}
		break;
	case OUTPUT_FN_TRIG_DOWN:
		{
			bool b;
			b = (val[0] >= _threshold );
			val[0] = (!b && (b != _b_trig_last_3d[0])) ? 1 : 0;
			_b_trig_last_3d[0] = b;
			b = (val[1] >= _threshold );
			val[1] = (!b && (b != _b_trig_last_3d[1])) ? 1 : 0;
			_b_trig_last_3d[1] = b;
			b = (val[2] >= _threshold );
			val[2] = (!b && (b != _b_trig_last_3d[2])) ? 1 : 0;
			_b_trig_last_3d[2] = b;
		}
		break;
	case OUTPUT_FN_TRIG_UP:
		{
			bool b;
			b = (val[0] >= _threshold );
			val[0] = (b && (b != _b_trig_last_3d[0])) ? 1 : 0;
			_b_trig_last_3d[0] = b;
			b = (val[1] >= _threshold );
			val[1] = (b && (b != _b_trig_last_3d[1])) ? 1 : 0;
			_b_trig_last_3d[1] = b;
			b = (val[2] >= _threshold );
			val[2] = (b && (b != _b_trig_last_3d[2])) ? 1 : 0;
			_b_trig_last_3d[2] = b;
		}
		break;
	}
	{
		REAL tmp = _max - _min;
		val[0] = _min + tmp * val[0];
		val[1] = _min + tmp * val[1];
		val[2] = _min + tmp * val[2];
	}
	switch( _s_limit )
	{
	case LIMIT_ABS_CLAMP:
		val[0] = ABS(val[0]);
		val[1] = ABS(val[1]);
		val[2] = ABS(val[2]);
	case LIMIT_CLAMP:
		CLAMP_REF( val[0], _limit_min, _limit_max );
		CLAMP_REF( val[1], _limit_min, _limit_max );
		CLAMP_REF( val[2], _limit_min, _limit_max );
		break;
	case LIMIT_ABS_WRAP:
		val[0] = ABS(val[0]);
		val[1] = ABS(val[1]);
		val[2] = ABS(val[2]);
	case LIMIT_WRAP:
		val[0] = FWRAP( val[0], _limit_min, _limit_max );
		val[1] = FWRAP( val[1], _limit_min, _limit_max );
		val[2] = FWRAP( val[2], _limit_min, _limit_max );
		break;
	case LIMIT_ABS_BOUNCE:
		val[0] = ABS(val[0]);
		val[1] = ABS(val[1]);
		val[2] = ABS(val[2]);
	case LIMIT_BOUNCE:
		val[0] = BOUNCE( val[0], _limit_min, _limit_max );
		val[1] = BOUNCE( val[1], _limit_min, _limit_max );
		val[2] = BOUNCE( val[2], _limit_min, _limit_max );
		break;
	case LIMIT_ABS:
		val[0] = ABS( val[0] );
		val[1] = ABS( val[1] );
		val[2] = ABS( val[2] );
		break;
	default:
		break;
	}
	_flux->put_3d(val);
//	value_last = val[0];
}

void	c_trax::update_low()
{
//todoqq
//hack	this is wrong (i don't remember why)
	p_param header = get_param_header();
	if( header->is_in() || header->is_out() )	// if( we really need to do something )
	{
		if( _fn_type != _fn_type_ui )
		{
			_fn_type = _fn_type_ui;
			if ( _fn_type < INDEX_3D_FIRST || _fn_type > INDEX_3D_LAST )
				_dim = 1;
			else
				_dim = 3;
		}
		_str_out = nullptr;
		flux_update();
		if( _dim == 3 )
		{
			update_low_3d();
			DOUBLE	vec[3];
			_flux->get_3d(vec);
			switch( _fn_type )
			{
			case FN_3D_CONTROL_SET:
				//todoopt	should not call three fns
				g_datacube->set_double( _channel_id, _control_index_ui, _control_index_bis_ui,   vec[0]	);
				g_datacube->set_double( _channel_id, _control_index_ui, _control_index_bis_ui+1, vec[1]	);
				g_datacube->set_double( _channel_id, _control_index_ui, _control_index_bis_ui+2, vec[2]	);
				break;
/*
			case FN_3D_VARIABLE_SET:
				tmp = FLUX->get();
				aaa::vars_double.set( _var_name.get(), tmp );
				break;
*/
			default:
				break;
			}

			INT32	sel;
			INT32	sel_next;
			if( sel = _s_out_sel[0] )
			{
				sel_next = 1;
				if( sel >= OUT_SEL_X_MINUS )
					_value_out[0] = REAL(-vec[sel - OUT_SEL_X_MINUS]);
				else
					_value_out[0] = REAL( vec[sel - OUT_SEL_X]);
			}
			else
				sel_next = 0;

			if( sel = _s_out_sel[1] )
			{
				sel_next |= 2;
				if( sel >= OUT_SEL_X_MINUS )
					_value_out[1] = REAL(-vec[sel - OUT_SEL_X_MINUS]);
				else
					_value_out[1] = REAL( vec[sel - OUT_SEL_X]);
			}

			if( sel = _s_out_sel[2] )
			{
				sel_next |= 4;
				if( sel >= OUT_SEL_X_MINUS )
					_value_out[2] = REAL(-vec[sel - OUT_SEL_X_MINUS]);
				else
					_value_out[2] = REAL( vec[sel - OUT_SEL_X]);
			}
			if( sel_next )
			{
				p_param	param;
				INT32	i;
//					for( i=0; i<nb_child; ++i )
//					{
//						REAL* pt;
//						pt = (REAL *)get_child_connex(i)->get_param_in_set()->get_pt();
				i = 0;
				while( param = get_plug_out( i) )
				{
					REAL* pt = param->get_pt_real3();
					switch( sel_next )
					{
					case 1:
						*pt = *_value_out;
						break;
					case 2:
						*(pt+1) = *(_value_out+1);
						break;
					case 3:
						*pt = *_value_out;
						*(pt+1) = *(_value_out+1);
						break;
					case 4:
						*(pt+2) = *(_value_out+2);
						break;
					case 5:
						*pt = *_value_out;
						*(pt+2) = *(_value_out+2);
						break;
					case 6:
						*(pt+1) = *(_value_out+1);
						*(pt+2) = *(_value_out+2);
						break;
					case 7:
						cpy_v3( pt, _value_out );
						break;
					}
				}
//						}
			}
		}
		else //of if ( _dim == 3 )
		{
			update_low_1d();
			if( _str_out )
			{
				INT32	i = 0;
				while( p_param param = get_plug_out(i) )
				{
//					param = get_child_connex(i)->get_param_in_set();
//					if ( !param_is_obj_flag(*param) )
					{//todoqq should not be here but in param.cpp
						//todo test and encapsulate
						if( param->is_type_text() )
							param->set_value_str( _str_out );
						else
							param->set_value_num_from_double( 0 );	//hack ?? to be decided
					}
				}
				_value_out[0] = 0;
			}
			else
			{
				DOUBLE	tmp;
				switch( _fn_type )
				{
				case FN_PLAYER_RAW:
					tmp = FLUX->get_raw();
					break;
				case FN_CONTROL_SET:
					tmp = FLUX->get();
					g_datacube->set_double( _channel_id, _control_index_ui, _control_index_bis_ui, tmp );
					break;
				case FN_VARIABLE_SET:
					tmp = FLUX->get();
					aaa::vars_double.set( _var_name.get(), tmp );
					break;
				case FN_SET_VALUE:
					if( c_layers::get_cur() )
					{
						tmp = FLUX->get();
						c_layers::get_cur()->get_values()->set_to_index( _control_index_ui, REAL(tmp) );
					}
					else
						err_print( "no current layers here" );
					break;
				case FN_SET_VALUE_MODULE:
					if( c_module::get_cur() )
					{
						tmp = FLUX->get();
						c_module::get_cur()->get_values()->set_to_index( _control_index_ui, REAL(tmp) );//todonow check also trax and value
					}
					else
						err_print( "no current module here" );
					break;
				case FN_SET_VALUE_GLOBAL:
					tmp = FLUX->get();
					c_modules::get_cur()->get_values()->set_to_index( _control_index_ui, REAL(tmp) ) ;//todonow check also trax and value
					break;
				case FN_COPY_BLOCK_REAL:
					return;
				default:
					tmp = FLUX->get();
					break;
				}
				_value_out[0] = REAL(tmp);

//				for( i=0; i<nb_child; ++i )
				INT32 i = 0;
				while( p_param param = get_plug_out(i) )
				{
//					param = get_child_connex(i)->get_param_in_set();
//					if ( !param_is_obj_flag(*param) )
					{
						//todo regroup
						if( param->is_type_text() )
							param->set_value_str_from_double( !_format.is_empty() ? _format.get() : "%f", tmp );
						else
							param->set_value_num_from_double( tmp );
					}
				}
			}
		}
/*					break;
			}

*/
	}
/*
	//todoqq this don't fit in the model
	//todo deal with 3d and make sure it's always done
	if ( b_midi_out && midi_cur)
	{
		REAL	tmp;
		if ( nb_parent )
		{
//todoqq
//hack	this is wrong
			tmp = param_get_value_as_real( get_parent_connex(0)->get_param_out_set() );
			tmp = (tmp-_min )/_max;
//				midi_cur->set_channel( _channel_id );
//				midi_cur->send_control_change( _control_index_ui, (INT32)tmp);
		}
	}
*/
//		_b_restart_trig_ui = false;
}
//par check
INT32	c_trax::get_index_from_param_pt( c_param* p_in )
{
	C_PCHAR_C	str = p_in->get_name().get();
	INT32	index = (*(str+4)-'0')*10+(*(str+5)-'0');
	return	index - 1;
}
//par check
C_PCHAR_C	c_trax::get_type_from_param_pt( c_param* p_in )
{
	return p_in->get_name().get();
}

/*
#define	TRAX_HEADER  "TRAX "

void	c_trax::err_print( C_PCHAR_C str )
{
	HEADER_PRINT_STRING( TRAX_HEADER, "%s %s", get_param_header()->get_name(), str );
}

void	c_trax::err_print( C_PCHAR_C stra, C_PCHAR_C strb )
{
	HEADER_PRINT_STRING( TRAX_HEADER, "%s %s %s", get_param_header()->get_name(), stra, strb );
}
*/