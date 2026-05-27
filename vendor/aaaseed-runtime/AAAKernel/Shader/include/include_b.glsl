// we redefine here the quantities defined in AAASeed C++
const int AAA_VERTEX_FLOAT_NB	= 8;
const int AAA_VERTEX_INT_NB		= 4;
const int AAA_VERTEX_VEC4_NB	= 12;

const int AAA_GEOMETRY_FLOAT_NB	= 4;
const int AAA_GEOMETRY_INT_NB	= 4;
const int AAA_GEOMETRY_VEC4_NB	= 4;

const int AAA_FRAGMENT_FLOAT_NB	= 24;
const int AAA_FRAGMENT_INT_NB	= 4;
const int AAA_FRAGMENT_VEC4_NB	= 8;

const int AAA_COMPUTE_FLOAT_NB	= 24;
const int AAA_COMPUTE_INT_NB	= 4;
const int AAA_COMPUTE_VEC4_NB	= 8;

#define USE_AAA_UNIFORM 1
#if USE_AAA_UNIFORM == 1
	// AAAseed uniform shader
	// vertex
	uniform float	aaa_vu_float[AAA_VERTEX_FLOAT_NB	];
	uniform int		aaa_vu_int	[AAA_VERTEX_INT_NB		];
	uniform vec4	aaa_vu_vec4	[AAA_VERTEX_VEC4_NB		];
	// geometry
	uniform	float	aaa_gu_float[AAA_GEOMETRY_FLOAT_NB	];
	uniform	int		aaa_gu_int	[AAA_GEOMETRY_INT_NB	];
	uniform	vec4	aaa_gu_vec4	[AAA_GEOMETRY_VEC4_NB	];
	// fragment (pixel)
	uniform float	aaa_fu_float[AAA_FRAGMENT_FLOAT_NB	];
	uniform int		aaa_fu_int	[AAA_FRAGMENT_INT_NB	];
	uniform vec4	aaa_fu_vec4	[AAA_FRAGMENT_VEC4_NB	];
	// compute
	uniform float	aaa_cu_float[AAA_COMPUTE_FLOAT_NB	];
	uniform int		aaa_cu_int	[AAA_COMPUTE_INT_NB		];
	uniform vec4	aaa_cu_vec4	[AAA_COMPUTE_VEC4_NB	];

	uniform vec3	iMouse;	// now like this to be compatible with ShaderToy
//	uniform float       fu_mouse_x, fu_mouse_y;
#else
	layout(binding = 8) uniform st_aaa
	{
	// vertex
		float		vu_float[AAA_VERTEX_FLOAT_NB	];
		int			vu_int	[AAA_VERTEX_INT_NB		];
		vec4		vu_vec4	[AAA_VERTEX_VEC4_NB		];
	// geometry
		float		gu_float[AAA_GEOMETRY_FLOAT_NB	];
		int			gu_int	[AAA_GEOMETRY_INT_NB	];
		vec4		gu_vec4	[AAA_GEOMETRY_VEC4_NB	];
	// fragment (pixel)
		float		fu_float[AAA_FRAGMENT_FLOAT_NB	];
		int			fu_int	[AAA_FRAGMENT_INT_NB	];
		vec4		fu_vec4	[AAA_FRAGMENT_VEC4_NB	];
	// compute
		float		cu_float[AAA_COMPUTE_FLOAT_NB	];
		int			cu_int	[AAA_COMPUTE_INT_NB		];
		vec4		cu_vec4	[AAA_COMPUTE_VEC4_NB	];
	//todo add to defines below or remove these or just remove it from ubo. TBD
		float		fu_external_mouse	[ 3 ];
		float		fu_external_time;
		float		fu_src;
		float		fu_out;
	} aaa;
	// AAAseed uniform shader
	// vertex
	#define aaa_vu_float	aaa.vu_float
	#define aaa_vu_int		aaa.vu_int
	#define aaa_vu_vec4 	aaa.vu_vec4
	// geometry
	#define aaa_gu_float	aaa.gu_float
	#define aaa_gu_int		aaa.gu_int
	#define aaa_gu_vec4 	aaa.gu_vec4
	// fragment (pixel)
	#define aaa_fu_float	aaa.fu_float
	#define aaa_fu_int		aaa.fu_int
	#define aaa_fu_vec4 	aaa.fu_vec4
	// compute
	#define aaa_cu_float	aaa.cu_float
	#define aaa_cu_int		aaa.cu_int
	#define aaa_cu_vec4 	aaa.cu_vec4

	#define iMouse 			aaa.fu_external_mouse
#endif

// Dimension of textures : 0, 1, 2, 3 dimension, -1 if unused per Tex Unit
uniform	int		aaa_tex_dim[4];
// 0 Implicit, 1 Object, 2 Eye, 3 Sphere, 4 Reflection, 5 Normal,	PROJ_CAMERA and PROJ_CAMERA_MODELVIEW 
uniform int		aaa_tex_gen[4];	//	mapping method per tex unit

#if AAA_GOL_VERSION >= 420
layout(binding = 11) uniform model_block
{
	mat4	matrix;
} aaa_model;

layout(binding = 12) uniform camera_block
{
	mat4	view;
	mat4	projection;
	mat4	view_projection;
	mat4	view_inverse;
	mat4	projection_inverse;
	mat4	view_projection_inverse;
	vec3	camera_position;
	float	plane_near;
	float	plane_far;
	vec3	pad;
	vec4	coc_factors;
} aaa_cam;

layout(binding = 14) uniform scene_block
{
	mat4	matrix;
} aaa_scene;
#endif


//#define MATRIX_DEPRECIATED_USE() 1
#define AAA_DEFINE_FN_POINT_NORMAL()\
void	AAA_get_point_normal( out vec4 vertex, out vec3 normal )\
{\
	vertex = gl_Vertex;\
	normal = gl_Normal;\
}\
void	AAA_get_point( out vec4 vertex )\
{\
	vertex = gl_Vertex;\
}

#if 0		// comments where problematic on amd because of the backslash
#define AAA_DEFINE_MATRIX_NAME()\
#	define aaa_matrix_modelview		aaa_cam.view\
#	define aaa_matrix_projection	aaa_cam.projection

#define AAA_DEFINE_MATRIX_NAME_DEPRECIATED()\
#	define aaa_matrix_modelview		gl_ModelViewMatrix\
#	define aaa_matrix_projection	gl_ProjectionMatrix

#define AAA_DEFINE_MATRIX_FNS()	\
vec4 aaa_transform_model_to_view(			in vec4 pos_in )	{	return aaa_matrix_modelview * pos_in;	}\
vec3 aaa_transform_model_to_view_normal(	in vec3 nor_in )	{	return (aaa_matrix_modelview * vec4(nor_in,0.) ).xyz;	}\
vec4 aaa_transform_view_to_projection(		in vec4 pos_in )	{	return aaa_matrix_projection * pos_in;	}
#endif

#define AAA_DEFINE_FN_BASE()\
AAA_DEFINE_FN_POINT_NORMAL()\
vec4 AAA_transform_model_to_view(			in vec4 pos_in )	{	return aaa_cam.view * pos_in;	}\
vec3 AAA_transform_model_to_view_normal(	in vec3 nor_in )	{	return (aaa_cam.view * vec4(nor_in,0.) ).xyz;	}\
vec4 AAA_transform_view_to_projection(		in vec4 pos_in )	{	return aaa_cam.projection * pos_in;	}

#define AAA_DEFINE_FN_BASE_DEPRECIATED()\
AAA_DEFINE_FN_POINT_NORMAL()\
vec4 AAA_transform_model_to_view(			in vec4 pos_in )	{	return gl_ModelViewMatrix * pos_in;	}\
vec3 AAA_transform_model_to_view_normal(	in vec3 nor_in )	{	return (gl_ModelViewMatrix * vec4(nor_in,0.) ).xyz;	}\
vec4 AAA_transform_view_to_projection(		in vec4 pos_in )	{	return gl_ProjectionMatrix * pos_in;	}\
vec4 AAA_transform_model_to_projection(		in vec4 pos_in )	{	return gl_ModelViewProjectionMatrix * pos_in;	}


#ifndef	AAA_FIX_TEX_UNIT_NB
#	define	AAA_FIX_TEX_UNIT_NB		4
#endif

#define AAA_FIX_TEX_COOR_DIM	4	
#if (AAA_FIX_TEX_COOR_DIM == 2)
	#define AAA_FIX_TEX_COOR_VEC	vec2
#else
	#define AAA_FIX_TEX_COOR_VEC	vec4
#endif

struct ST_AAA_BV	//	Block Vertex Fix path
{
	vec4					pos_world;
	vec4					nor_world;
	vec4					pos_ec;
	vec3					nor_ec;
	float					alpha;
	vec4					color;
	AAA_FIX_TEX_COOR_VEC	tex_coor[AAA_FIX_TEX_UNIT_NB];
//	float	fall_off;
//	float	z_to_eye;
};

#define EMITTER_NB_MAX 2
struct st_img_filter
{
    uint	_image_sx;
    uint	_image_sy;
    int		_coverage_enable;
    float	_coverage;
    vec4 	_color_min;
    vec4	_color_max;
	vec4	_color_selector;
};

struct st_emitter
{
    vec3 _pos;
	uint _rate;
    vec4 _size;
    vec4 _vel;
    vec4 _jitter_pos;
    vec4 _jitter_vel;
};

struct st_flex_solver
{
	uint	_active_nb;
	uint	_emit_offset;
	uint	_max_particles_nb;
	float	_dt;
	uint	_frame_index;
	vec3	_pad;
};

struct particle_full
{
	vec4 pos;
	vec4 vel;
	vec4 pos_b;
	vec4 dum;
	vec4 accel;
	vec4 pos_c;
	float mass;
	float radius;
	float life;
	float life_over_one;
};

struct particle_compact
{
	vec4	pos;
	vec4	vel;
	vec4	pos_b;
	vec4	dum;
};

































//end should be 270 lines for this file (300 with previous one include_a.glsl)
