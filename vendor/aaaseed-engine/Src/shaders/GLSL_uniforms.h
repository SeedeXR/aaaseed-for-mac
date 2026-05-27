
#ifdef AAA_GLSL_UNIFORMS_H
#error "GLSL_UNIFORMS_H included more than once."
#endif
#define AAA_GLSL_UNIFORMS_H 1

#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef	AAA_GOL_SHADER_H
#	include	"gol/gol_shader.h"
#endif
#ifndef	AAA_GOL_TEX_H
#	include	"gol/gol_tex.h"
#endif
#ifndef AAA_SHADER_CONST_H
#	include "shader_const.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif

#if 1
#	define CHECK_AND_CPY(a,b,c)	check_and_cpy_vn( (a), (b), (c) )
#else
//slower but for test
#	define CHECK_AND_CPY(a,b,c)	true
#endif

class c_uniforms_glsl : public c_obj
{
protected:
	typedef	INT32	LOCATION;
private:
	LOCATION	_loc_tex2d;							//	texture_2d sampler old version
	LOCATION	_loc_samp[GOL::TEX_SAMPLER_NB_MAX];	//	sampler can be 1d 2d 3d individually
protected:
	//AMD
	static CONSTEXPR INT32 tex2d_sampler[32] =
	{
		0,	1,	2,	3,	4,	5,	6,	7,
		8,	9,	10,	11,	12,	13,	14,	15,
		16,	17,	18,	19,	20,	21,	22,	23,
		24,	25,	26,	27,	28,	29,	30,	31,
	};

	FINLINE static void SET_1I(			LOCATION CONST location, INT32   CONST v1 )								{ if( location != -1 ) GOL::set_uniform_1i(		location, v1 );				}
	FINLINE static void SET_1UI(		LOCATION CONST location, UINT32  CONST v1 )								{ if( location != -1 ) GOL::set_uniform_1u(		location, v1 );				}
	FINLINE static void CHECK_SET_1IV(	LOCATION CONST location, GLsizei CONST count, GLint CONST * CONST pvalue, GLint* CONST last )
	{ 
		if( location != -1 && check_and_cpy_vn( last, pvalue, count ) )
		{
			//GOL::check_error( "before" );
			GOL::set_uniform_1iv( location, count, pvalue );
			//GOL::check_error( "after" );
		}
	}

	FINLINE static void SET_1F(	LOCATION CONST location, GLfloat CONST v1 )															{ if( location != -1) GOL::set_uniform_1f( location, v1 );	}
	FINLINE static void SET_2F(	LOCATION CONST location, GLfloat CONST v1, GLfloat CONST v2)										{ if( location != -1) GOL::set_uniform_2f( location, v1, v2); }
	FINLINE static void SET_3F(	LOCATION CONST location, GLfloat CONST v1, GLfloat CONST v2, GLfloat CONST v3)						{ if( location != -1) GOL::set_uniform_3f( location, v1, v2, v3); }
	FINLINE static void SET_4F(	LOCATION CONST location, GLfloat CONST v1, GLfloat CONST v2, GLfloat CONST v3, GLfloat CONST v4)	{ if( location != -1) GOL::set_uniform_4f( location, v1, v2, v3, v4); }

	FINLINE static void SET_1FV( LOCATION CONST location, GLsizei CONST count, GLfloat CONST * CONST pvalue )	{ if( location != -1) GOL::set_uniform_1fv(	location, count, pvalue );	}
	FINLINE static void SET_2FV( LOCATION CONST location, GLsizei CONST count, GLfloat CONST * CONST pvalue )	{ if( location != -1) GOL::set_uniform_2fv(	location, count, pvalue );	}
	FINLINE static void SET_3FV( LOCATION CONST location, GLsizei CONST count, GLfloat CONST * CONST pvalue )	{ if( location != -1) GOL::set_uniform_3fv(	location, count, pvalue );	}
	FINLINE static void SET_4FV( LOCATION CONST location, GLsizei CONST count, GLfloat CONST * CONST pvalue )	{ if( location != -1) GOL::set_uniform_4fv( location, count, pvalue );	}

	void	init_tex( GLuint CONST program );

public:
//	c_uniforms_glsl();
//	~c_uniforms_glsl();

	virtual void	init( CONST 	GLuint program ) = 0;
	virtual	void	set_last() = 0;
private:
	//infact we do it once when program is linked
	// named in shader: aaa_tex2d[]
	FINLINE	void	update_tex2d_sampler()
	{
		GOL::set_uniform_1iv( _loc_tex2d, GOL::TEX_SAMPLER_NB_MAX, tex2d_sampler );	//	this is pretty dummy but have to be done this way, we do it once
	}
	// named in shader: aaa_samp0, aaa_samp1 ....
	FINLINE	void	update_sampler(		INT32	CONST	index )		{	SET_1I(			_loc_samp[index],		index						);								}
	FINLINE	void	update_samplers()
	{
		for( INT32 index=0; index < GOL::TEX_SAMPLER_NB_MAX; ++index )
			update_sampler( index );
	}
public:
	FINLINE void init_aaatex_and_samplers()
	{
		update_tex2d_sampler();
		update_samplers();
	}
};


class c_uniforms_render_glsl final : public c_uniforms_glsl
{
private:
	bool		_b_tex_dim_gen_size;
//	bool		_b_tex_sampler;
//	INT32		_tex_sampler_max_used;

protected:
	// For GL, UniformT = GLint
	LOCATION	_loc_lights;

// VERTEX
	LOCATION	_loc_vertex_float;
	LOCATION	_loc_vertex_int;
	LOCATION	_loc_vertex_vec4;

// FRAGMENT
	LOCATION	_loc_fragment_src;
	LOCATION	_loc_fragment_out;
	LOCATION	_loc_fragment_float;
	LOCATION	_loc_fragment_int;
	LOCATION	_loc_fragment_vec4;

// TEXTURE
	LOCATION	_loc_tex_gen;		//	texture implicit + coor generation
	LOCATION	_loc_tex_dim;		//	texture dimension
	//LOCATION	_loc_tex_size;		//	size of texture sampler 

	LOCATION	_loc_tex_env_mode;
//not used anymore, shadertoy stuff
// 	LOCATION	_loc_tex_dim_external;
// GEOMETRY
	LOCATION	_loc_geometry_float;
	LOCATION	_loc_geometry_int;
	LOCATION	_loc_geometry_vec4;

	LOCATION	_loc_external_mouse;
	LOCATION	_loc_external_time;

	INT32		_vertex_int_last[	n_shader::VERTEX_INT_NB		];
	INT32		_geometry_int_last[	n_shader::GEOMETRY_INT_NB	];
	INT32		_fragment_int_last[	n_shader::FRAGMENT_INT_NB	];

	INT32		_tex_dim_last[		GOL::TEX_UNIT_NB_MAX		];
	INT32		_tex_gen_last[		GOL::TEX_UNIT_NB_MAX		];
	INT32		_tex_env_mode_last[	GOL::TEX_UNIT_NB_MAX		];

	//FP32		_tex_size_last[		GOL::TEX_UNIT_NB_MAX*2		];
public:
//	c_uniforms_render_glsl();
//	~c_uniforms_render_glsl();

	virtual	void	init( CONST 	GLuint program );
	virtual	void	set_last();

// VERTEX
	FINLINE	void	update_vertex_float(		FP32 CONST *	CONST val	)	CONST	{	SET_1FV(		_loc_vertex_float,		n_shader::VERTEX_FLOAT_NB,	val		);						}
	FINLINE	void	update_vertex_int(			INT32	CONST *	CONST val	)			{	CHECK_SET_1IV(  _loc_vertex_int,		n_shader::VERTEX_INT_NB,	val,	_vertex_int_last );		}
	FINLINE	void	update_vertex_vec4(			FP32 CONST *	CONST val	)	CONST	{	SET_4FV(		_loc_vertex_vec4,		n_shader::VERTEX_VEC4_NB,	val		);						}
// GEOMETRY
	FINLINE	void	update_geometry_float(		FP32 CONST *	CONST val	)	CONST	{	SET_1FV(		_loc_geometry_float,	n_shader::GEOMETRY_FLOAT_NB,val		);						}
	FINLINE	void	update_geometry_int(		INT32	CONST *	CONST val	)			{	CHECK_SET_1IV(  _loc_geometry_int,		n_shader::GEOMETRY_INT_NB,	val,	_geometry_int_last );	}
	FINLINE	void	update_geometry_vec4(		FP32 CONST *	CONST val	)	CONST	{	SET_4FV(		_loc_geometry_vec4,		n_shader::GEOMETRY_VEC4_NB,	val		);						}
// FRAGMENT
	FINLINE	void	update_fragment_float(		FP32 CONST *	CONST val	)	CONST	{	SET_1FV(		_loc_fragment_float,	n_shader::FRAGMENT_FLOAT_NB, val	);						}
	FINLINE	void	update_fragment_float(		FP32 CONST *	CONST val,	FP32 CONST src,	FP32 CONST out  )
																				CONST	{
																							//todo cache these
																							SET_1F( _loc_fragment_src, src );
																							SET_1F( _loc_fragment_out, out );
																							update_fragment_float( val );
																						}
	FINLINE	void	update_fragment_int(		INT32	CONST *	CONST val	)			{	CHECK_SET_1IV(	_loc_fragment_int,		n_shader::FRAGMENT_INT_NB,	val,	_fragment_int_last );	}
	FINLINE	void	update_fragment_vec4(		FP32 CONST *	CONST val	)	CONST	{	SET_4FV(		_loc_fragment_vec4,		n_shader::FRAGMENT_VEC4_NB,	val		);						}
	FINLINE void	update_fragment_external(	FP32 CONST *	CONST mouse, FP32 CONST time )
																				CONST	{
																							SET_3FV( _loc_external_mouse, 1, mouse );
																							SET_1F( _loc_external_time, time );
																						}


	FINLINE	void	update_lights(				UINT32 CONST value			)	CONST	{	SET_1UI( _loc_lights, value );	}
	FINLINE	bool	is_lights()													CONST	{	return _loc_lights != -1 ;		}

//TEXTURE
	FINLINE void	update_tex_dim(			INT32 CONST * CONST	val	)	{	CHECK_SET_1IV(  _loc_tex_dim,			GOL::tex_unit_nb,			val,	_tex_dim_last );		}
	FINLINE void	update_tex_gen(			INT32 CONST * CONST	val	)	{	CHECK_SET_1IV(  _loc_tex_gen,			GOL::tex_unit_nb,			val,	_tex_gen_last );		}
	FINLINE void	update_tex_env_mode(	INT32 CONST * CONST	val	)	{	CHECK_SET_1IV(  _loc_tex_env_mode,		GOL::tex_unit_nb,			val,	_tex_env_mode_last );	}


//	FINLINE INT32	get_tex_sampler_max_used()	CONST	{	return _tex_sampler_max_used;	}
	FINLINE bool	is_tex_dim_gen_size()		CONST	{	return _b_tex_dim_gen_size;		}


	//todoq remove
	//FINLINE void	update_tex_size(		FP32 CONST * CONST val	)
	//{
	//	if( _loc_tex_size != -1 && CHECK_AND_CPY( _tex_size_last, val, GOL::tex_unit_nb*2 ) )
	//		GOL::set_uniform_2fv( _loc_tex_size, GOL::tex_unit_nb, val );
	//}

#ifdef	AAA_SCREEN_DEFORMATION	//used for Nibelung
#ifdef	SHADER_SCREEN_DEFORMATION
protected:
	LOCATION	_uni_vertex_def_type;
	LOCATION	_uni_vertex_def_value;
public:
	FINLINE void	update_deformation()
	{
		if( _uni_vertex_def_type == -1 || _uni_vertex_def_value == -1 )
			return;
		FP32	val[5];
		INT32	def;
		if( GOL::s_screen_def_type == GOL::VERTEX_DEF_SHADER )
		{		
			val[0] = GOL::screen_def_cos_freq;
			val[1] = GOL::screen_def_cos_limit;
			val[2] = GOL::screen_def_z_factor;
			val[3] = GOL::screen_def_y_origin; 
			val[4] = GOL::screen_def_y_factor;
			def = GOL::s_screen_def_type;
		}
		else
		{
			clear_vr( val, 5 );
			def = 0;
		}
		GOL::set_uniform_1i( _uni_vertex_def_type, def );
		SET_1FV( _uni_vertex_def_value, 6, val );
	}
#endif	//	#ifdef	SHADER_SCREEN_DEFORMATION
#endif	//	#ifdef	AAA_SCREEN_DEFORMATION
};


class c_uniforms_compute_glsl final : public c_uniforms_glsl
{

public:
	// COMPUTE
	LOCATION	_loc_compute_float;
	LOCATION	_loc_compute_int;
	LOCATION	_loc_compute_vec4;

	INT32		_compute_int_last[	n_shader::COMPUTE_INT_NB	];

//	c_uniforms_compute_glsl();
//	~c_uniforms_compute_glsl();

	virtual	void	init( CONST 	GLuint program );
	virtual	void	set_last();

	// COMPUTE
	FINLINE	void	update_compute_float(	INT32 CONST program,	FP32 CONST *	CONST val	)
	{	
		if( _loc_compute_float != -1 )
			GOL::set_program_uniform_1fv(	program,	_loc_compute_float,	n_shader::COMPUTE_FLOAT_NB, val );
	}
	FINLINE	void	update_compute_int(		INT32 CONST program,	INT32	CONST *	CONST val	)
	{
		if( _loc_compute_int != -1 && check_and_cpy_vn( _compute_int_last, val, n_shader::COMPUTE_INT_NB ) )
			GOL::set_program_uniform_1iv(	program,	_loc_compute_int,	n_shader::COMPUTE_INT_NB, val );
	}
	FINLINE	void	update_compute_vec4(	INT32 CONST program,	FP32 CONST *	CONST val	)
	{
		if( _loc_compute_vec4 != -1 )
			GOL::set_program_uniform_4fv(	program,	_loc_compute_vec4,	n_shader::COMPUTE_VEC4_NB, val );
	}
};

