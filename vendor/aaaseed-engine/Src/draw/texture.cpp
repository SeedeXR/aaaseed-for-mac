#include "texture.h"
#include "draw/map.h"
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "ui/strsymbo.h"
#include "infrastructure/namer.h"
#include "infrastructure/param/param_declare.h"
#include "shaders/shading.h"


FACTORY_CREATE_V1( c_texture_unit, texture_unit, Texture Unit, tex_unit );

namespace n_texture_unit
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 8;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	= 	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(			active		)
		PARAM_DEF_REF(				name_symbo	)

		PARAM_DEF_BIND_1D(			bind_1d	)
		PARAM_DEF_BANK_2D(			bank_2d		)		// careful max is set again in is_obj_first() below
		PARAM_DEF_BIND_2D(			bind_2d		)		// careful max is set again in is_obj_first() below
		PARAM_DEF_BANK_BIND_2D_OUT(	bind_2d_out	)
		PARAM_DEF_BIND_3D(			bind_3d		)
		PARAM_DEF_BOOL_ON(			mapping		)
	};
}

namespace {
	INT32 index_bank_2d = 0;
	INT32 index_bind_2d_out = 0;
	INT32 index_map = 0;
	o_str l_o_sum_up;
}

void	c_texture_unit::prepare_for_ui()
{
	c_img_2d*	img = g_bind_img_2d->get( _bind_2d_out );
	get_param(index_bind_2d_out)->set_comment( img ? img->get_filename() : nullptr );
	get_param(index_bank_2d)->set_comment( g_bind_img_2d->get_tex_index_str( _bank_2d_ui, _bind_2d_ui ) );
	_map->build_sum_up( l_o_sum_up );
	get_param(index_map)->set_comment( l_o_sum_up );
}

void	c_texture_unit::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, get_name_symbo() );
	param_set_pt( h, _bind_1d_ui );

	index_bank_2d = h;
//todo done in object first but dangerous
//	param_set_max_no_inc( h, g_bind_img_2d->get_bank_nb() - 1 );
	param_set_pt( h, _bank_2d_ui );
//todo done in object first but dangerous
//	param_set_max_no_inc( h, g_bind_img_2d->get_bank_size() - 1 );
	param_set_pt( h, _bind_2d_ui );

	index_bind_2d_out = h;
	param_set_pt( h, _bind_2d_out );

	param_set_pt( h, _bind_3d_ui );

	index_map = h;
	param_attach_obj_no_inc( h,  _map	);
	param_set_pt(			 h, _b_mapping_ui	);
	
	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_texture_unit)
,_map			(	nullptr	)
,_bind_2d_out	(	0		)
,_gol_tex_unit	(	0		)	//would start at 1 but we need to init index build_sum_up 
{
	if( is_obj_first() )
	{
		//todo find by name instead of hard coded index
		n_texture_unit::param[3].set_max( REAL(g_bind_img_2d->get_bank_nb()	  - 1) );
		n_texture_unit::param[4].set_max( REAL(g_bind_img_2d->get_bank_size() - 1) );
	}
	param_init_with( n_texture_unit::param, n_texture_unit::PARAM_NB_MAX );
	obj_get( _map );
}
c_texture_unit::~c_texture_unit()
{
	obj_delete( _map );
}

FINLINE	bool	c_texture_unit::update_low()
{
	_bind_2d_out = g_bind_img_2d->make_tex_index( _bank_2d_ui, _bind_2d_ui );

	if( !_b_mapping_ui )
		return false;

	GOL::set_tex_unit( _gol_tex_unit );
//	_map->update_implicit();
	_map->update();
	return true;
}

void	c_texture_unit::update()
{
	if( is_active() )
		update_low();
}
//unused April 2020 see layer
//FINLINE	void	c_texture_unit::draw_only_active_low()
//{
//	if( !is_active() )
//	{
//		GOL::disable_tex_unit( _gol_tex_unit );
//		return;
//	}
//
//	if( !_b_mapping )
//		return;
//	switch( _map->get_tex_use() )	//todotex
//	{
//	case 0:
//		GOL::disable_tex_unit( _gol_tex_unit );
//		return;
//	case 1:
//	case 2:
//	case 3:
//		GOL::set_tex_unit( _gol_tex_unit );
//		_map->update_implicit();
//		_map->tex_draw();
//		break;
//	}
//	c_shading::update_from_map( gol_tex_unit, _map );
//}

#if 0
FINLINE	void	c_texture_unit::draw_low()
{
	switch( _map->get_tex_use() )	//todotex
	{
	case 0:
		GOL::disable_tex_unit( _gol_tex_unit );
		return;
	case 1:
		GOL::set_tex_unit( _gol_tex_unit );
		GOL::set_texture_dim( 1 );
		tex_1d_bind( _bind_1d_ui );
		_map->draw_1d();
		break;
	case 2:	
		GOL::set_tex_unit( _gol_tex_unit );
		GOL::set_texture_dim( 2 );
			//todoq		if( b_force_reload_ui )
		if( _map->is_texture_feedback() )
			tex_2d_bind( -1 );
		else //no tex_anim here if( s_tex_anim == 1 || !c_tex_anim::cur->is_active() )	//todo check tex_anim_strategie
		{
			//if( _bind_2d_out == 138 )
			//	DBG_PRINT_STRING( "Aux Fraises" );
			//else
			//{
			//}
			tex_2d_bind( _bind_2d_out );
		}
		//todo avoid systematic call
		_map->draw_2d();
		break;
	case 3:
		GOL::set_tex_unit( _gol_tex_unit );
		GOL::set_texture_dim( 3 );
		tex_3d_bind( _bind_3d_ui );
		//todo avoid systematic call
		_map->draw_3d();
		break;
	}
	c_shading::update_from_map( _gol_tex_unit, _map );
}
#else
//maa 2020 April : simplify see what happen
FINLINE	void	c_texture_unit::draw_low()
{
	switch( _map->get_tex_use() )	//todotex
	{
	case 0:
		GOL::disable_tex_unit( _gol_tex_unit );
		return;
	case 1:
		GOL::set_tex_unit( _gol_tex_unit );
		GOL::set_texture_dim( 1 );
		tex_1d_bind( _bind_1d_ui );
		//todo avoid systematic call and refine wrap for tex unit not texture
		_map->do_wrap_1d();
		break;
	case 2:	
		GOL::set_tex_unit( _gol_tex_unit );
		GOL::set_texture_dim( 2 );
		tex_2d_bind( _bind_2d_out );
		//todo avoid systematic call and refine wrap for tex unit not texture
		_map->do_wrap_2d();
		break;
	case 3:
		GOL::set_tex_unit( _gol_tex_unit );
		GOL::set_texture_dim( 3 );
		tex_3d_bind( _bind_3d_ui );
		//todo avoid systematic call and refine wrap for tex unit not texture
		_map->do_wrap_3d();
		break;
	}
	//c_shading::update_from_map( _gol_tex_unit, _map );
}
#endif

AAA_ERR	c_texture_unit::load_do_after( o_str CONST & filename )
{
	try_obj_load_add_ext( _map, filename );
	return AAA_OK;
}

AAA_ERR	c_texture_unit::save_do_after( o_str CONST & filename )
{
	if_obj_save_add_ext( _map, filename );
	return AAA_OK;
}

c_texturing*	c_texturing::def	= nullptr;
c_texturing*	c_texturing::cur	= nullptr;
c_texturing*	c_texturing::ui		= nullptr;

FACTORY_CREATE_V1( c_texturing, texturing, Texturing, texturing );


namespace n_texturing
{
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	= 	c_texturing::TEXTURING_UNIT_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( unit_1 )
		PARAM_DEF_BOOL_OFF( unit_2 )
		PARAM_DEF_BOOL_OFF( unit_3 )
	};
}

void	c_texturing::param_init_pt()
{
	INT32	h=0;
	for( UINT32 i = 0; i < TEXTURING_UNIT_NB; ++i )
	{
		c_texture_unit* unit = _tex_units[i];
		param_attach_obj_no_inc( h, unit );
		param_set_pt( h, unit->get_pt_active() );
	}
	err_param_init_pt( h );
}	

void	c_texturing::build_sum_up( o_str& o )
{
	if( _tex_units[0]->is_active() )
		o.set_char( '1' );
	else
		o.erase();

	if( _tex_units[1]->is_active() )
		o.add_char( '2' );

	if( _tex_units[2]->is_active() )
		o.add_char( '3' );
}

CONSTRUCTOR_CREATE(c_texturing)
{
	for( INT32 i = 0; i < TEXTURING_UNIT_NB; ++i )
	{
		c_texture_unit* unit;
		obj_new( unit );
		if( unit )
		{
			unit->set_gol_tex_unit(i+1);
			_tex_units[i] = unit;
		}
		else
			debug_break( "%s() can't alloc unit", __FUNCTION__ );
	}
	param_init_with( n_texturing::param, n_texturing::PARAM_NB_MAX);
}

c_texturing::~c_texturing()
{
	for( UINT32 i = 0; i < TEXTURING_UNIT_NB; ++i )
		obj_delete( _tex_units[i] );

	if( cur == this )
		cur = nullptr;
	if( ui == this )
		ui = nullptr;
}

//maa april 2020 do less update/draw
//	i try more because most shaders we use don't deal with infos by unit
//maa	we should go back to it after monaco
void	c_texturing::update()
{
	bool b_clean = false;
	c_texture_unit** tus = _tex_units;
	for( INT32 nb = TEXTURING_UNIT_NB; nb>0; --nb )
	{
		c_texture_unit* tu = *tus;
		if( tu->is_active() )
			b_clean |= tu->update_low();
		++tus;
	}
	if( b_clean )
		GOL::set_tex_unit( 0 );
	cur = this;
}
//unused April 2020
//void	c_texturing::draw_only_active()
//{
//	for( INT32 i=1; i < GOL::tex_unit_nb; ++i )
//		_tex_units[i-1]->draw_only_active();
//	GOL::set_tex_unit( 0 );
//}

void	c_texturing::draw()
{
	c_texture_unit** tus = _tex_units;
	for( INT32 nb = TEXTURING_UNIT_NB; nb>0; --nb )
	{
		c_texture_unit* tu = *tus;
		if( tu->is_active() && tu->is_mapping() )
			tu->draw_low();
		else
		{
			//maa we should not need it because of call in layer to c_texturing::disable()
			//and if we do it we need the texture unit to be set before
			GOL::disable_tex_unit( tu->get_gol_tex_unit() );
		}
		++tus;
	}
	GOL::set_tex_unit( 0 );
}

AAA_ERR	c_texturing::load_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( c_texture_unit::the_factory().get_file_ext() );
		filename.add( "_1" );

		for( UINT32 i = 0; i < TEXTURING_UNIT_NB; ++i )
		{
			filename.set_char( -1, '1' + i );
			c_namer::set_file_forced( _tex_units[i], filename );
			try_obj_load_with_this_filename( _tex_units[i], filename );
		}
	o_str::pop_name();
	return AAA_OK;
}

AAA_ERR	c_texturing::save_do_after( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( c_texture_unit::the_factory().get_file_ext() );
		filename.add( "_1" );

		for( UINT32 i = 0; i < TEXTURING_UNIT_NB; ++i )
		{
			if( _tex_units[i] )
			{
				filename.set_char( -1, '1' + i );
				_tex_units[i]->save_to_file( filename );
			}
		}
	o_str::pop_name();
	return AAA_OK;
}
