#include "bdd.h"
#include "obj_ui/bdd/util/factory_group_bdd.h"
#include "obj_ui/bdd/bdd_geo/bdd_gene.h"
#include "ui/aaa_menu.h"
#include "infrastructure/layer/layer.h"
//todo remove or make list conditionnally compiled
#include "gol/gol_list.h"
#include "gol/gol_matrix.h"
#include "gol/gol_tex.h"
#include "gol/gol_light.h"
#include "gol/gol_draw.h"
#include "gol/gol.h"
#include "gol/gol_color.h"
#include "draw/model.h"
#include "draw/seedcam.h"
#include "draw/render.h"
#include "draw/colorrnd.h"
#include "draw/tex_anim.h"
#include "draw/mat.h"
#include "shaders/shading.h"
#include "draw/texture.h"
#include "draw/shape.h"
#include "time/aaa_time.h"




FACTORY_ABSTRACT_CREATE_V1( c_bdd, bdd, Bdd );

c_factory_group_bdd*	c_bdd::factory_group		= nullptr;
c_bdd_empty*			c_bdd::bdd_empty			= nullptr;
c_bdd*					c_bdd::sta_cur				= nullptr;	
c_bdd*					c_bdd::sta_ui				= nullptr;
bool					c_bdd::gb_allow_clipping	= true;

namespace {
	static	std::set<c_bdd*>	ui_bdd_set;
	static	std::vector<c_bdd*>	ui_bdds;

	c_shading* shading_last = nullptr;	//todo perhaps we should do it with a stack
}


bool	c_bdd::is_mouse_use()								{	return is_ui_intercept();	}
bool	c_bdd::mouse_down(	FP32& u_start, FP32& v_start )	{	u_start = 0.; v_start = 0.; return false; }
void	c_bdd::mouse_move(	FP32 u_in, FP32 v_in )			{}
void	c_bdd::mouse_up(	FP32 u_in, FP32 v_in )			{}
bool	c_bdd::mouse_wheel( FP32 factor )					{ return false; }
//bool	c_bdd::mouse_cam()									{ return false; }


c_layer*	c_bdd::get_layer() CONST
{
	c_obj_ui*	obj_ui = get_root();
	// this test is a little heavy for realtime but fn called only for param_init_pt
	return( obj_ui && c_layer::is_instance(obj_ui) ) ? (c_layer*) obj_ui : nullptr;
}

void	bdd_menu_change( INT32 index_in ) 
{	//todo perhaps we could do something here
	if( menu::is_on() )
		return;
}


AAA_ERR	c_bdd::build_menu( INT32 menu_id, PT_MENU_FN menu_fn )
{
	return c_bdd::factory_group->build_menu( menu_id, menu_fn );
}

static	CONST	INT32	MULTIPLE_RENDER_LIST_NB = 8;	//so we can achieve a depth of 8 multiple
static			INT32	MULTIPLE_RENDER_LIST = 0;
static			INT32	multiple_render_list; 

void	c_bdd::c_init()
{
//	build_ref_to_order();
//todomonaco improve MULTIPLE_RENDER_LIST so it is not done at init
#if 1
	if( MULTIPLE_RENDER_LIST==0 )
	{
		MULTIPLE_RENDER_LIST = GOL::gen_lists( MULTIPLE_RENDER_LIST_NB );
		multiple_render_list = MULTIPLE_RENDER_LIST;
	}
	else
	{
		debug_break( "%s() already called, mention to Maa or Franz.", __FUNCTION__ );
	}
#else
#endif
}

void	c_bdd::ui_register( c_bdd* bdd_add )
{
	if( ui_bdd_set.find(bdd_add) == ui_bdd_set.end() )	//	in case of multiple we don't want to overload it
	{	
		ui_bdd_set.insert( bdd_add );
		ui_bdds.push_back( bdd_add );
	}
}

void	c_bdd::ui_start_frame()
{
	ui_bdd_set.clear();
	ui_bdds.clear();
}

//this is called every frame at start
void	c_bdd::clear_ui_intercept_all()
{
	for( INT32 i = (INT32)ui_bdds.size()-1; i>=0 ; --i )
		ui_bdds[i]->set_ui_intercept( false ); 
}

void	c_bdd::flip_ui_intercept_static()
{
	// todoqq bdd_ui not initialized here
	bool b_stop = false;
	auto bdd_ui = c_bdd::get_ui();
	if( bdd_ui )
	{
		b_stop = bdd_ui->is_ui_intercept();
		bdd_ui->flip_ui_intercept();
	}
	else
		b_stop = true;

	if( b_stop )
		clear_ui_intercept_all();
}

c_bdd*	c_bdd::ui_start_mouse_down()
{
	for( INT32 i = (INT32)ui_bdds.size()-1; i>=0 ; --i )
	{
		if( ui_bdds[i]->is_mouse_use() )	//todoq do a subscribe mechanism
			return ui_bdds[i];
	}
	return nullptr;
}

bool	c_bdd::ui_do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y )
{
	for( INT32 i = (INT32)ui_bdds.size()-1; i>=0 ; --i )
	{
		if( ui_bdds[i]->do_key( c, b_special, modifiers, x, y ) )	//todoq do a subscribe mechanism
			return true;
	}
	return false;
}



CONSTRUCTOR_ABSTRACT_CREATE(c_bdd)
,_b_draw_before(false)
{
	if( c_bdd::the_factory().is_obj_first() )
	{
		c_bdd::factory_group = new c_factory_group_bdd;
		c_bdd::factory_group->init();
		bdd_empty = new c_bdd_empty;
	}
}

c_bdd::~c_bdd()
{
	if( this == sta_cur )
		sta_cur	= nullptr;
	if( this == sta_ui  )
		sta_ui	= nullptr;
	if( c_bdd::the_factory().is_obj_first() )
	{
		obj_delete( c_bdd::factory_group );
	//	obj_delete( bdd_empty );	//we erase all the bdd at exit so bdd_empty is already gone
	}
}

void c_bdd::print_empty_mess( C_PCHAR_C fn_name )
{
	dbg_print( "%s() call with bdd_empty", fn_name );
}
void c_bdd::alloc()
{
	print_empty_mess( __FUNCTION__ );
}

void c_bdd::dealloc()
{
	print_empty_mess( __FUNCTION__ );
}

void c_bdd::update()
{
	print_empty_mess( __FUNCTION__ );
}

bool	c_bdd::get_point_rnd( REAL* CONST dst, REAL CONST t_in )
{
	clear_v3( dst );
	return false;
}
bool	c_bdd::get_point_and_speed_rnd_time_abs( REAL* CONST dst, REAL* CONST speed, REAL CONST t_in )
{
	clear_v3( speed );
	return get_point_rnd( dst, t_in );
}
bool	c_bdd::get_point_and_speed_rnd_time_rel( REAL* CONST dst, REAL* CONST speed, REAL CONST t_in )
{
	clear_v3( speed );
	return get_point_rnd( dst, t_in );
}

void c_bdd::cell_draw_obj( REAL CONST size )
{
	GOL::matrix::push();
		GOL::matrix::scale( REAL(.865) * size );
		c_bdd_gene::draw_tetrahedron( GL_TRIANGLES );
	GOL::matrix::pop();
}

void c_bdd::draw()
{
//	DBG_PRINT_STRING( "c_bdd::draw() call so this object have no draw() fn." );
}

void	c_bdd::ui_draw_point( REAL CONST * CONST pos, REAL CONST size )
{
	GOL::begin( GL_POINTS );
		GOL::vertex3v( pos );
	GOL::end();
	if( size > 0. )
	{
		FP32 t = WRAP_01( REAL(aaa::time::get_real_time()) * REAL(2) );	//todo cash in update
		draw_mul_line_xyz( pos, t*size );
	}
}
/*
c_multiple*	c_bdd::get_multiple_for_bdd()
{
	c_layer*	p_layer = get_layer();
	if( p_layer )
		return p_layer->get_multiple_for_bdd();
	else
		return nullptr;
}
*/

bool	c_bdd::is_normal_draw()		{	return c_layer::get_cur()->is_normal_draw();	}
bool	c_bdd::is_normal_needed()	{	return c_layer::get_cur()->is_normal_needed();	}

//b_global if the bdd don't have a local context (layer)
INT32	c_bdd::param_init_pt_no_geo()
{
	INT32		h = 0;
	//par	this should be done at init only
	param_set_pt( h, get_name_symbo() );

	return h;
}

//b_global if the bdd don't have a local context (layer)
INT32	c_bdd::prepare_for_ui_geo( INT32& h )	//par
{
	if( c_layer* p_layer = get_layer() )
	{	
		param_attach_obj_no_inc( h, p_layer->get_model_for_bdd() );
		param_set_pt( h, p_layer->get_b_model_pt() );
		param_attach_obj_no_inc( h,  p_layer->get_multiple_for_bdd() );
		param_set_pt( h, p_layer->get_s_multiple_pt() );
	}
	else
	{
		param_attach_obj( h, nullptr );
		param_attach_obj( h, nullptr );
	}
	return h;
}

//b_global if the bdd don't have a local context (layer)
INT32	c_bdd::param_init_pt_geo()	//par
{
	INT32		h = 0;
	//par	this should be done at init only
	param_set_pt( h, get_name_symbo() );
	
//par
/*
	//todo remove b_global take care of //par
	if( b_global)
	{	//unused for now
		param_attach_obj_no_inc( h, nullptr );
		param_set_lock_no_inc( h );
		param_set_pt_null( h );

		param_attach_obj_no_inc( h, nullptr );
		param_set_lock_no_inc( h );
		param_set_pt_null( h );	
	}
	else
*/
	prepare_for_ui_geo( h );

	return h;
}

//b_global if the bdd don't have a local context (layer)
void	c_bdd::param_init_add_model( INT32& h )	//par
{
	if( c_layer* p_layer = get_layer() )
	{	
		param_attach_obj_no_inc( h, p_layer->get_model_for_bdd() );
		param_set_pt( h, p_layer->get_b_model_pt() );
	}
	else
		param_attach_obj( h, nullptr );
}
				

void c_bdd::begin_ui()
{
	GOL::push_att();	//( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT );
//todo this probably wrong the pop attrib will overwrite GOL on texture
		GOL::push_texture_dim( 0 );
		c_texturing::get_cur()->disable();
//		GOL::disable_tex_unit_from_and_force_update_later();

		GOL::push_lighting( false );
		GOL::push_line_width();
		GOL::push_point_size();
		GOL::push_color();

		auto cam = c_seedcam::get_cur();
		GOL::normal3v( cam->get_axe_unit(2) );

		//todo include a general mecanism (bdd_clear_screen too)
		shading_last = c_shading::get_cur();
		if( shading_last )	
			shading_last->unbind_render();
}

void c_bdd::end_ui()
{
	if( shading_last )
		shading_last->bind_render();
	GOL::pop_att();
}

c_bdd*  c_bdd::update_bdd_target( o_str CONST & name, c_bdd* target )
{
	if( name.is_empty() )
		return nullptr;	
	if( name.is_str_equal( "prev", 4  ) )
		return get_bdd_prev();
	if( target && target->is_name_symbo(name) && target->get_root() )
		return target;
	return find_bdd_by_name_symbo( name );
}

INT32	c_bdd::get_point_dataset_nb()							{	err_print_method_unimplemented( __FUNCTION__ );	return 0;			}
bool	c_bdd::set_point_dataset( INT32 dataset_id )			{	err_print_method_unimplemented( __FUNCTION__ );	return false;		}

INT32	c_bdd::get_point_nb()									{	err_print_method_unimplemented( __FUNCTION__ );	return 0;			}
void	c_bdd::set_point_nb(	INT32 nb )						{	err_print_method_unimplemented( __FUNCTION__ );						}

REAL*	c_bdd::get_points()										{	err_print_method_unimplemented( __FUNCTION__ );	return nullptr;		}
void	c_bdd::erase_points()									{	err_print_method_unimplemented( __FUNCTION__ );						}

REAL*	c_bdd::get_point_pt(	INT32 CONST index )				{	err_print_method_unimplemented( __FUNCTION__ );	return nullptr;		}
bool	c_bdd::get_point(		REAL* CONST dst, INT32 CONST index )
{	
	REAL CONST * CONST pt = get_point_pt(index);
	if( pt )
	{
		cpy_v3( dst, pt );
		return true;
	}
	clear_v3( dst );
	return false;
}
bool	c_bdd::set_point(		INT32 CONST index, REAL CONST * CONST src )
{	
	REAL * CONST pt = get_point_pt(index);
	if( pt )
	{
		cpy_v3( pt, src );
		return true;
	}
	return false;
}

INT32	c_bdd::get_point_and_id( REAL* dst, INT32 CONST index )	{	return get_point( dst, index ) ? index : 0;	}
bool	c_bdd::get_points_3d( REAL* dst, INT32 nb )
{
	INT32 nb_there = get_point_nb();
	if( nb <= nb_there )
	{
		for( INT32 i=0; i<nb; ++i )
		{
			//todo check ?
			get_point( dst, i );
			dst += 3;
		}
		return true;
	}
	return false;
	//err_print_method_not_implemented( __FUNCTION__ );	return false;
}

REAL*	c_bdd::get_normals()									{	err_print_method_unimplemented( __FUNCTION__ );	return nullptr;		}

INT32	c_bdd::get_segment_nb()									{	err_print_method_unimplemented( __FUNCTION__ );	return 0;			}
REAL*	c_bdd::get_segments()									{	err_print_method_unimplemented( __FUNCTION__ );	return nullptr;		}
void	c_bdd::get_segment( REAL* a, REAL* b, INT32 index )		{	err_print_method_unimplemented( __FUNCTION__ );	clear_v3( a );	clear_v3( b ); }

void	c_bdd::set_color(		INT32 index, FP32* col )		{	err_print_method_unimplemented( __FUNCTION__ );	}
void	c_bdd::use_color(		INT32* index, INT32 nb )		{	err_print_method_unimplemented( __FUNCTION__ );	}

void	c_bdd::set_color_map(	INT32 index,	INT32	bind,	REAL u, REAL v, REAL fu, REAL fv		)
																{	err_print_method_unimplemented( __FUNCTION__ );	}
void	c_bdd::use_color_map(	INT32* index, INT32 nb )		{	err_print_method_unimplemented( __FUNCTION__ );	}


AAA_ERR	c_bdd::load_data()		{	err_print_method_unimplemented( "load_data" );		return ERR_UNIMPLEMENTED_YET;	}

void	c_bdd::save_obj_file( o_str CONST & filename )
{
	//	open file
	FILE* file = c_file::FOPEN( filename, "wt" );
	if( !file )
	{
		ERR_PRINT_STRING( "In c_bdd::save_obj_file() Can't open file : ", filename.get() );
	}
	else
	{
		fprintf( file, "# Object saved with AAASeed (c) Maa 1997-2026\n\n" );
		save_obj_file( file );
	}
	c_file::FCLOSE( file );
}

void	c_bdd::save_obj_file(			FILE* CONST file )	{	err_print_method_unimplemented( "save_obj_file" );			}
void	c_bdd::save_obj_file_points(	FILE* CONST file )	{	err_print_method_unimplemented( "save_obj_file_points" );	}
void	c_bdd::save_obj_file_normals(	FILE* CONST file )	{	err_print_method_unimplemented( "save_obj_file_normals" );	}

void	c_bdd::save_obj_file_points( FILE* CONST file, REAL CONST * point, INT32 nb )
{
	for( ; nb>0; --nb )
	{
		fprintf( file, "v %f %f %f\n", *point, *(point+1), *(point+2) );
//real		file << "v " << *point << "n" << " " << *(point+1) << " " << *(point+2) << "\n";
		point += 3;
	}
/*	to save in csv file
	for( INT32 i=1; i<=nb; ++i )
		{
		fprintf( file, "%d;%f;%f;%f\n", i, *point, *(point+1), *(point+2) );
//real		file << "v " << *point << "n" << " " << *(point+1) << " " << *(point+2) << "\n";
		point += 3;
		}
*/
}
void	c_bdd::save_obj_file_normals( FILE* CONST file, REAL CONST * normal, INT32 nb )
{
	for( ; nb>0; --nb )
	{
		fprintf( file, "vn %f %f %f\n", *normal, *(normal+1), *(normal+2) );
//real		file << "v " << *normal << "n" << " " << *(normal+1) << " " << *(normal+2) << "\n";
		normal += 3;
	}
}

c_bdd*	c_bdd::get_bdd_prev()
{
	return ((c_layer*)get_root())->get_bdd_prev();
}

void	c_bdd::transfer_blobs_to( BLOBS_CONT& blobs )
{
	DBG_PRINT_STRING( "%s() unimplemented for class %s", __FUNCTION__, get_class_name() );
}



FACTORY_ABSTRACT_CREATE_V1( c_bdd_multiple, bdd_multiple, Bdd Multiple );

c_bdd_multiple*		c_bdd_multiple::cur = nullptr;

CONSTRUCTOR_ABSTRACT_CREATE(c_bdd_multiple)
,_render_list_multiple_name(0)
{
}

c_bdd_multiple::~c_bdd_multiple()
{
	if( cur == this )
		cur = nullptr;
}

void	c_bdd_multiple::draw_multiple()
{

	INT32	nb = get_point_nb();
	if( !nb )
	{
		return;
	}

	REAL*	pt = get_points();
	if( !pt )
	{
		debug_break( "empty or should be implemented" );
		return;
	}

	bool b_color_by_elt = c_render::get_cur()->get_random_on_color() > 0;

	c_multiple::cur->set_nb( nb );
	if( b_color_by_elt )
	{
		c_color_random::begin();
		if( c_tex_anim::cur->is_tex_several() )
			while( nb-->0 )
			{
				c_color_random::set();
				c_tex_anim::cur->bind_next();
				c_multiple::cur->align_then_draw( pt );
				c_multiple::cur->next_index();
				pt += 3;
			}
		else
			while( nb-->0 )
			{
				c_color_random::set();
				c_multiple::cur->align_then_draw( pt );
				c_multiple::cur->next_index();
				pt += 3;
			}
		c_color_random::end();
	}
	else
	{
		if( c_tex_anim::cur->is_tex_several() )
			while( nb-->0 )
			{
				c_tex_anim::cur->bind_next();
				c_multiple::cur->align_then_draw( pt );
				c_multiple::cur->next_index();
				pt += 3;
			}
		else
			while( nb-->0 )
			{
				c_multiple::cur->align_then_draw( pt );
				c_multiple::cur->next_index();
				pt += 3;
			}
	}
}

void	c_bdd_multiple::draw_single()
{
	ERR_PRINT_STRING( "c_bdd_multiple::draw_single() called");
}

void	c_bdd_multiple::multiple_compile_one( REAL CONST* size, INT32 axe )
{
	if( MULTIPLE_RENDER_LIST==0 )
		return;

	c_materials::get_cur()->set_last();	//hack cqn multiple_cur change ?
	
	c_multiple* multiple = c_multiple::cur;
	multiple->begin();	//hack	avoid crash or p_layers_cached_ = nullptr;
		GOL::compile_list( multiple_render_list );
			//todo what happen when we really call it badly
			c_multiple::cur->draw_one_low( size, axe );
		GOL::end_list();
	multiple->end();	//hack see before
	
	_render_list_multiple_name = multiple_render_list;	//store it
	if( ++multiple_render_list >= (MULTIPLE_RENDER_LIST+MULTIPLE_RENDER_LIST_NB) )
		multiple_render_list = MULTIPLE_RENDER_LIST;

	//todo do it faster
//maa 2016 removed this and don't know why it was there : tex_2d_bind( -2 );
	//hack Ok for material but what about the rest
	c_materials::get_cur()->get_last();
}

void	c_bdd_multiple::draw()
{
	if( c_multiple::cur->is_render_regular() )
		draw_single();
	if( c_multiple::cur->is_render_multiple() )
	{
		c_multiple* multiple = c_multiple::cur;
		if( multiple->begin() )
			draw_multiple();
		multiple->end();
	}
}

//todoq	find a better architecture
//used by bdd_uvw
void	c_bdd_multiple::draw_w( INT32 iw )
{
	c_tex_anim::cur->set_iw( iw );
	if( c_multiple::cur->is_render_regular() )
	{
		//DBG_PRINT_STRING( "\tdraw_single()" );
		draw_single();
	}
	if( c_multiple::cur->is_render_multiple() )
	{
		//DBG_PRINT_STRING( "\tdraw_multiple()" );
		draw_multiple();
	}
}

FACTORY_CREATE_PROP_V1( c_bdd_empty, bdd_empty, Bdd empty, bdd_empty, sub_menu="Internal"; );
CONSTRUCTOR_CREATE(c_bdd_empty)	//facto suppress
{
	param_init_with( nullptr, 0 );
}
EMPTY_DESTRUCTOR(c_bdd_empty)
