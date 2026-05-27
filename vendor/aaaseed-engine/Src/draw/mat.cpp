#include "mat.h"
#include "infrastructure/bind/bind.h"
#include "infrastructure/layer/layer.h"
#include "gol/gol_light.h"
#include "infrastructure/namer.h"
#include "strnum.h"
#include "draw/rect.h"
#include "infrastructure/param/param_declare.h"

#if !defined(AAA_GL_UBO_H)
#	include "gl/ubo.h"
#endif


FACTORY_CREATE_V1( c_material, material, Material, mat );

namespace{
	gl::ubo *	ubo_static = nullptr;
}

namespace n_material
{
	CONSTEXPR INT32 PARAM_NB_MAX	=	23;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_COLOR_RGBGA_BLACK(	Ambient				)
		PARAM_DEF_FP32_ONE_ZERO(		Ambient_Occlusion	)
		PARAM_DEF_COLOR_RGBGA(			Diffuse				)
		PARAM_DEF_COLOR_RGBGA_BLACK(	Specular			)
		PARAM_DEF_FP32_ZERO_ONE(		Shininess			)
		PARAM_DEF_FP32_ONE_ZERO(		Reflection			)
		PARAM_DEF_COLOR_RGBGA_BLACK(	Emission			)
	};
}

void	c_material::param_init_pt()
{
	INT32	h=0;

	param_set_pt_rgbfa( h,	_ambient_ui				);
	param_set_pt(		h,	_ambient_occlusion_ui	);
	param_set_pt_rgbfa( h,	_diffuse_ui				);
	param_set_pt_rgbfa( h,	_specular_ui			);
	param_set_pt(		h,	_shininess_ui			);
	param_set_pt(		h,	_reflection_ui			);
	param_set_pt_rgbfa( h,	_emission_ui			);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_material )
{
	init_name_with( "Material" );
	param_init_with( n_material::param, n_material::PARAM_NB_MAX );

	if( is_obj_first() )
		ubo_static = gl::ubo::make( sizeof( st_material ), true, nullptr, "material" );
}
c_material::~c_material()
{
	if( is_obj_first() )
		gl::ubo::release_and_null( ubo_static );
}


FINLINE	static	void	build_color( FP32* dst, FP32 CONST* src )
{
	scale_v3( dst, src, src[4] );
	dst[3] = src[3];
}

FINLINE	static	void	set_color_sum_up( FP32 CONST* pt )
{
	FP32	vec[4];
	build_color( vec, pt );
	GOL::color4v( vec );
}

void	c_material::draw_sum_up()
{
	//	n_axe::draw_axe_3D();
	//GOL::color_black4();
	//draw_rect( -.1, -.1, 3.1, 1.1 );
	//draw();
	set_color_sum_up( _ambient_ui );
	draw_rect( 0, 0, 1, 1 );

	set_color_sum_up( _diffuse_ui );
	draw_rect( 1,0, 2,1 );

	set_color_sum_up( _specular_ui );
	FP32 tmp = FP32( _shininess_ui * .3333333 );
	draw_rect( 2+tmp,0+tmp, 3-tmp,1-tmp );

	set_color_sum_up( _emission_ui );
	draw_rect( 3.5,0, 4.5,1 );
}

//FINLINE	void set_material_using_factor (GLenum face, GLenum pname, FP32 CONST* in )
//{
//	build_color( f4, in );
//	GOL::materialv( face, pname, f4 );
//}

//todo check we really call
void	c_material::set_it( INT32 face )
{
	//we only deal with a front material (for now 2020 April)
	bool b_ubo = (face == GL_FRONT) || (face == GL_FRONT_AND_BACK);

	//todo: 1 ubo per object is overkill, use single one for active object.
	FP32 f4[4];
//EMISSION
	build_color( f4, _emission_ui );
	GOL::materialv( face, GL_EMISSION,	f4 );
	if( b_ubo )
		cpy_v3( _ubo_data.emissive_color,	f4 );

//AMBIENT
	build_color( f4, _ambient_ui );
	GOL::materialv( face, GL_AMBIENT,	f4 );

//DIFFUSE
	build_color( f4, _diffuse_ui );
	GOL::materialv( face, GL_DIFFUSE,	f4 );
	if( b_ubo )
		cpy_v3( _ubo_data.diffuse_color,	f4 );

//SPECULAR
	build_color( f4, _specular_ui );
	GOL::materialv( face, GL_SPECULAR,	f4 );
	if( b_ubo )
		cpy_v3( _ubo_data.specular_color,	f4 );

//SHININESS / ROUGHNESS
	GOL::material( face, GL_SHININESS, FP32(_shininess_ui * 128.) );
	if( b_ubo )
	{
		_ubo_data.roughness				=	FP32(1. - _shininess_ui);

		_ubo_data.ao_factor				=	_ambient_occlusion_ui;

		_ubo_data.reflection_intensity	=	_reflection_ui;
		_ubo_data.ambient_intensity		=	_ambient_ui[4];

		ubo_static->bind_and_write( gl::ubo_binding_index_material, &_ubo_data, sizeof( st_material ) );
	}
}

void	c_material::set_alpha( FP32 alpha_in )
{
	_diffuse_ui[3] = alpha_in;
}

FACTORY_CREATE_V1( c_materials, materials, Materials, materials );

namespace{
	INT32	index_front_last = -1;
	INT32	index_back_last = -1;
}

namespace n_materials
{
	CONSTEXPR INT32 PARAM_NB_MAX	= 1;
	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_NONE( Material_bind )
	};
}

void	c_materials::param_init_pt()
{
	INT32	h=0;

	param_attach_obj( h, _bind );

	err_param_init_pt( h );
}

void c_materials::reset()	//todo should have a reset mechanism when object are change only
{
	_index_front = -42;
	_index_back = -42;
}

CONSTRUCTOR_CREATE(c_materials)
{
	_bind = nullptr;
	_array_pt.set_nb( MATERIAL_MAX_NB );
	param_init_with( n_materials::param, n_materials::PARAM_NB_MAX );

	alloc();
	reset();
}

c_materials::~c_materials()
{
	if( cur == this )
		cur = nullptr;
	if( ui == this )
		ui = nullptr;

	dealloc();
}





void c_materials::set_last()
{
	index_front_last = _index_front;
	index_back_last	 = _index_back;
}

void c_materials::get_last()
{
	_index_front = index_front_last;
	_index_back  = index_back_last;
}

//c_material*	material_front_ui;
//c_material*	material_back_ui;

void	c_materials::set_alpha( INT32 index, FP32 value )
{
	if( index >= 0 && index < MATERIAL_MAX_NB )
	{
		get_always( index )->set_alpha( value );
	}
}


AAA_ERR	c_materials::alloc()
{
	_bind = obj_get( _bind );
	if( _bind )
	{
		_bind->set( MATERIAL_MAX_NB, "Material_Binds", "mat_bind", aaa::file::TYPE_IO_MATERIAL, 4, true, true, true );
		return AAA_OK;
	}
	return ERR_ANY ;
}

void	c_materials::dealloc()
{
	_array_pt.delete_objs();
	obj_delete( _bind );
}

void	c_materials::set_focus()
{
	_bind->set_focus();
}

void	c_materials::update()
{
	cur = this;
	reset();
	set( 0, 0 );
}

void	material_init()
{
	/*
	AAA_ERR	retcode = material_allocate();
	if( NOERR(retcode) )
	{
	//	material_front_ui = material_array;
	//	material_back_ui = material_array;
	}
	else
	{
	BOX_ERR("Can't allocate the materials");
	}
	return retcode;
	*/
	c_materials::get_def()->set_focus();
}

static	CHAR	mat_err[128];

//todo see if we regroup with c_cameras
FINLINE bool	c_materials::check_index( INT32 index, CHAR* str )
{
	if( _array_pt.check_index( index ) )
		return true;
	if( str )
		sprintf( str, "Index out of range, can't get material %d", index );
	return false;
}

c_material*	c_materials::get_always( INT32 index )
{
	if( check_index( index, mat_err )  )
	{
		c_material*	p = _array_pt.get( index );
		if( !p )
		{
			obj_get( p );
			_array_pt.set( index, p );
			//todonow	p->c_camera_init( params+which, which, get_name() ); //see c_trax_init
		}
		return p;
	}
	box_err( mat_err );
	return nullptr;
}

c_material*	c_materials::get_from_index( INT32 index )
{
	//if( this )
	//{
		if( check_index( index, mat_err ) )
			return _array_pt.get( index );
		ERR_PRINT_STRING( mat_err );
	//}
	//else
	//	ERR_PRINT_STRING( "%s() trying to use a null c_materials", __FUNCTION__ );
	return nullptr;
}

static	void	MAACALLBACK material_menu_fn( INT32	in )
{
	c_materials::get_def()->set_focus( in );	//hack was ok only when materials_def
}

void	c_materials::set_focus( INT32 in )
{
	get_always( in )->set_focus();
}

INT32	c_materials::menu_build()
{
	return _bind->menu_build( 0, material_menu_fn );
}

FINLINE	c_material*	c_materials::load_one( INT32 index, o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
	c_material*	p;

		filename.add_ext( c_material::the_factory().get_file_ext() );

		if( !c_file::is_exist(filename) )
		{
			p = nullptr;
		}
		else if( !check_index( index, mat_err )  )
		{
			p = nullptr;
			box_err( mat_err );
		}
		else
		{
			p = _array_pt.get(index);
			obj_get_load_from_existing_file( p, filename );
			c_namer::set_dir_and_file( p, filename );
			//if( p && !_array_pt.get(index) ) //	only if we created a new material
			//{
				//todonow			p->c_camera_init( params+which, which, get_name() );	 //see c_trax_init
			//}
			_array_pt.set( index, p );
		}

	o_str::pop_name();
	return p;
}

//todo we can't change add or remove on the fly
AAA_ERR	c_materials::load_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();
		_bind->load_from_file_add_ext( filename );

#if AAA_STATE_COMPILE()
		if( c_state_master::is_state_ref() )
			_array_pt.delete_objs();
#endif //AAA_STATE_COMPILE

		for( INT32 i = 0; i < MATERIAL_MAX_NB; ++i )
		{
			o_str CONST & str = _bind->get_o_str(i);
			if( !str.is_empty() )
			{
				//todo be smarter and relative
				load_one( i, str );
				//		str_material[MATERIAL_MAX_NB] = str;
			}
			//		else
			//			str_material[MATERIAL_MAX_NB] = "No";
		}
		//	layers_menu_update();
	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR	c_materials::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.drop_ext();

		CHAR def_name[] = "material/mat_000";
		for( INT32 i = 0; i < MATERIAL_MAX_NB; ++i )
		{
			auto p_material = _array_pt.get( i );
			if( p_material )
			{
				o_str CONST & str = _bind->get_o_str(i);
				if( str.is_empty() )
				{
					strnum::make( def_name + 13, 3, i );
					p_material->save_to_file_add_ext( def_name );
					if( p_material->is_my_filename() )
						_bind->set_item( i, def_name );
				}
				else
					p_material->save_to_file_add_ext( str );
			}
		}
		//saving the material eventually have completed the bind so it is saved after
		_bind->save_to_file_add_ext( filename );
	o_str::pop_name();
	return AAA_OK;
}

void	c_materials::front_cur_set_alpha( FP32 value )
{
	if( _index_front != -42 )	//	-42 mean not set
	{
		c_material*	pt = get_always( _index_front );
		FP32		diff[4];
		scale_v3_cpy_v4( diff, pt->_diffuse_ui );
		diff[3] *= value;
		GOL::materialv( GL_FRONT, GL_DIFFUSE, diff );
	}
}
void	c_materials::back_cur_set_alpha( FP32 value )
{
	if( _index_back != -42 )	//	-42 mean not set
	{
		c_material*	pt = get_always( _index_back );
		FP32	diff[4];
		scale_v3_cpy_v4( diff, pt->_diffuse_ui );
		diff[3] *= value;
		GOL::materialv( GL_BACK, GL_DIFFUSE, diff );
	}
}

c_materials*	c_materials::def = nullptr;
c_materials*	c_materials::cur = nullptr;
c_materials*	c_materials::ui = nullptr;

void c_materials::c_init()
{
	::obj_get( def );
	cur = def;
}

void c_materials::c_deinit()
{
	SAFE_DELETE( def );	
	cur = nullptr;
}


void	c_materials::set( INT32 index_front, INT32 index_back )
{
#if	AAA_DEBUG()
	if( index_front >= MATERIAL_MAX_NB )
	{
		err_print( "front material index %d too high only %d materials, defaulting to zero.", index_front, MATERIAL_MAX_NB );
		index_front = 0;
	}
	else if( index_front < 0 )
	{
		err_print( "front material index %d negative, defaulting to zero", index_front );
		index_front = 0;
	}

	if( index_back >= MATERIAL_MAX_NB )
	{
		err_print( "back material index %d too high only %d materials, defaulting to zero.", index_back, MATERIAL_MAX_NB );
		index_back = 0;
	}
	else if( index_back < 0 )
	{
		err_print( "back material index %d negative, defaulting to zero", index_back );
		index_back = 0;
	}
#endif

	if( _index_front != index_front  )
	{
		_index_front = index_front;
		if( _index_back != index_back )
		{
			_index_back = index_back;
			if( index_front == index_back )
				get_always( index_front )->set_it( GL_FRONT_AND_BACK );
			else
			{
				get_always( index_front )->set_it( GL_FRONT );
				get_always( index_back )->set_it( GL_BACK );
			}
		}
		else
			get_always( index_front )->set_it( GL_FRONT );
	}
	else if( _index_back != index_back )
	{
		_index_back = index_back;
		get_always( index_back )->set_it( GL_BACK );
	}
}
