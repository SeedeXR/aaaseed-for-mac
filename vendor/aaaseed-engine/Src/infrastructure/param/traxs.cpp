#include "infrastructure/param/traxs.h"
#include "infrastructure/param/trax.h"
#include "infrastructure/data/datacube.h"
#include "strnum.h"
#include "draw/box.h"
#include "time/aaa_time.h"
#include "ui/seedkey.h"
#include "obj_ui/com/net.h"
#include "infrastructure/namer.h"
#include "infrastructure/param/param_declare.h"
#include "infrastructure/seedfile.h"
#include "spy.h"

FACTORY_CREATE_V1( c_traxs, traxs, Traxs, traxs );

bool	c_traxs::gb_update = true;
INT32	c_traxs::g_channel_offset = 0;
INT32	c_traxs::g_control_offset = 0;

namespace {
	c_param_def*	param_defs		= nullptr;
	INT32			param_defs_nb	= 0;
}

//	TRAXS FN
void	traxs_init()
{
	g_datacube = new c_datacube;
	g_datacube->init();
}
void	traxs_deinit()
{
	g_datacube->dealloc();
	SAFE_DELETE( g_datacube );
}

extern	REAL	trax_time;
void	traxs_before()
{
/*
	float pf_receipt[1024];
	sndinput.get_freq_band(0,0.5,1024,pf_receipt,1);
	float f_receipt;
	sndinput.get_freq_band(0.05,0.07,1,&f_receipt,1);
*/
	trax_time = REAL(aaa::time::get());

	g_datacube->update();	//todoqqq should be link to app
}

void	traxs_after()
{
	c_keyboard::clear();
}

//	TRAXS
CONSTRUCTOR_CREATE(c_traxs)
	,_channel_offset	(	0	)
	,_control_offset	(	0	)
{
}

c_traxs::~c_traxs()
{
	if( b_aaa_exiting && b_aaa_exiting_fast )
		return;

	dealloc();

	if( is_obj_first() )
	{
		SAFE_DELETE_ARRAY( param_defs );	//	we deallocate only when allocated
		param_defs_nb = 0;
	}
}

void	c_traxs::set_nb( INT32 nb )
{
	alloc( nb );
}

void	c_traxs::param_init_pt()
{
	INT32	h = 0;
	h += _array_pt.get_nb();
	err_param_init_pt(h);
}

void	c_traxs::prepare_for_ui()
{
	//SUPER::prepare_for_ui();

	INT32 i_end = _array_pt.get_index_end();
	for( INT32 i=_array_pt.get_index_begin(); i<=i_end; ++i )
	{
		auto param = get_param(i);
		auto p_trax = _array_pt.get(i);
		if( p_trax )
		{
			p_trax->prepare_for_ui();	//todo so for now we probably call it twice if we display trax too (2023 Feb)
			auto trax_header = p_trax->get_param_header();
			param->set_name(	trax_header->get_name() );
			param->set_type(	trax_header->get_type() );
			param->borrow_in(	trax_header->get_in()	);
			param->borrow_out(	trax_header->get_out()	);
			param->cpy_pt(		trax_header	);
			param->set_save( false );
		}
	}

	//	param_connex_set();
}

void c_traxs::alloc( INT32 nb )
{
	if( _array_pt.get_nb() != nb )
	{	
		if( get_factory()->get_param_data_size() < nb )	//par redid correctly
		{
			//we allocate only when needed
			if( param_defs_nb < nb )
			{
				SAFE_DELETE_ARRAY( param_defs );
				param_defs = new c_param_def[nb];
				if( !param_defs )
				{
					box_err( "Can't allocate params for c_traxs" );
					param_defs_nb = 0;
					return;
				}
				param_defs_nb = nb;
				c_param_def*	p = param_defs-1;
				for( INT32 i = 0; i < nb; ++i )
				{
					CHAR str[] = "T00";
					strnum::make( str + 1, 2, i + 1 );
					(++p)->set_all( TYPE_NONE|M_SAVE_NOT, str );
				}
			}
		}
		else
			param_defs = nullptr;
		_array_pt.set_nb( nb );
		param_init_with( param_defs, nb );	//	if param_defs is non null it will be copied in the factory by this function 
	}	
}

void c_traxs::dealloc()
{
	trax_free_all();
}

void c_traxs::trax_free_all()
{
	disconnect_all();

	INT32 beg = _array_pt.get_index_begin();
	for( INT32 i=_array_pt.get_index_end(); i>=beg; --i )
	{
		c_trax* trax = _array_pt.get(i);
		if( trax )
		{
			trax->remove_in_all();	//todo something is wrong 
			trax->remove_out_all();	//todo
		}
	}
	_array_pt.delete_objs();
}


void c_traxs::cell_draw_obj( REAL CONST size )
{
	draw_cube( size );
}

void c_traxs::draw()
{
	_array_pt.do_method< &c_trax::draw > ();
}

namespace{
static	CHAR	str_trax_err[128];
}

FINLINE bool	c_traxs::check_trax_index( CHAR* str, INT32 index )
{
	if( _array_pt.check_index( index ) )
		return true;

	sprintf( str, "Index out of range, can't get trax %d", index );
	return false;
}

bool	c_traxs::swap_trax( INT32 src_id, INT32 dst_id )
{
	//	don't move the extreme	//todo extend ?
	if( !_array_pt.check_index(dst_id-1) )
		return false;
	if( !_array_pt.check_index(src_id-1) )
		return false;

	//	swap c_trax*
	_array_pt.swap( src_id-1, dst_id-1 );

	//	swap param_header
	swap_param( src_id, dst_id );

	//param_header have to be adjusted
	//	par should be done thru a fn
	c_trax*	pt = _array_pt.get( dst_id-1 );
//param
//	pt->set_param_header_low( get_param( dst ) );
	//set index for name
	if( pt )
		pt->set_index( dst_id-1 );

	pt = _array_pt.get( src_id-1 );
	if( pt )
	{	//if trax
		//param_header have to be adjusted
//param
//		pt->set_param_header_low( get_param( src ) );
		//set index for name
		pt->set_index( src_id-1 );
	}
	else
	{
		//keep nb coherent	
		//par should not be here so explicitly
//		get_param( src )->set_data_forced( get_factory()->get_param_data( src ) );
		//par
		//what happen to the other one
//		strnum::make( params_[index_src].get_name()+1, 2, index_src+1 );
	}
	return true;
}

c_trax*	c_traxs::get_trax_always( INT32 index )
{
	if( check_trax_index( str_trax_err, index )  )
	{
		auto trax = _array_pt.get( index );	
		if( !trax )
		{
			obj_get( trax );
			trax->trax_init( index, get_name_str() );
			_array_pt.set( index, trax );
		}
		return trax;
	}
	box_err( str_trax_err);
	return nullptr;
}

c_trax*	c_traxs::get_trax( INT32 index )
{
	if( check_trax_index( str_trax_err, index ) )
		return _array_pt.get(index);
	ERR_PRINT_STRING( str_trax_err );
	return nullptr;
}

c_trax*	c_traxs::trax_plug_out( c_obj_ui* obj, p_param param )
{
	INT32	index = _array_pt.get_index_free();
	if( index >= 0 )
		return trax_plug_out( index, obj, param );
	box_err( "can't find a free trax.");	//par make it dynamic	
	return nullptr;
}

c_trax*	c_traxs::trax_plug_out( INT32 index, c_obj_ui* obj, p_param param )
{
	if( !param->is_pluggable() )
		box_err( "This is a place holder, not a real param.\nCan't plug in or out here, there is no data." );
	else if( param->is_lock() )
		box_err( "Param %0.128s is locked so a trax can't change it, AAASeed will not plug Out a trax here.", param->get_name().get() );
	else
	{
		c_trax*	a_trax = get_trax_always( index );
		if( a_trax )
			a_trax->plug_out( obj, param );
		return a_trax;	
	}
	return nullptr;
}

c_trax*	c_traxs::trax_plug_in( INT32 index, c_obj_ui* obj, p_param param )
{
	if( param->is_pluggable() )
	{
		c_trax*	a_trax = get_trax_always( index );
		if( a_trax )
			a_trax->plug_in( obj, param );
		return a_trax;
	}
	box_err( "This is a place holder, not a real param.\nCan't plug in or out here, there is no data." );
	return nullptr;
}

//todooptqq	I should not parse all
void	c_traxs::update( INT32 channel_offset, INT32 control_offset )
{
	if( gb_update )
	{
		SPY_PUSH_RANGE( "traxs::update", spy::LAYERS );

			//these two are here only for the ui display
			_channel_offset = channel_offset;
			_control_offset = control_offset;
			//these two do the update of trax
			g_channel_offset = _channel_offset;
			g_control_offset = _control_offset;

			_array_pt.do_method< &c_trax::update >();

		SPY_POP_RANGE();	
	}
}

void	c_traxs::start_loop( bool CONST b_record_in )
{
	_array_pt.do_method_bool_const<	&c_trax::start_loop >( b_record_in );
}
void	c_traxs::stop_loop()
{
	_array_pt.do_method< &c_trax::stop_loop >();
}

void	c_traxs::start_preroll()						{	start_loop( false);		}

void	c_traxs::disconnect_all()
{	// out connections are done thru param not object
	_array_pt.do_method< &c_trax::unplug_in_all > ();

	//we have to remove pointer in these params their where set in prepare_for_ui() but hold only a copy of the original pointer
	INT32 i_end = _array_pt.get_index_end();
	for( INT32 i=_array_pt.get_index_begin(); i<=i_end; ++i )
	{
		auto param = get_param(i);
		if( _array_pt.get(i) )
		{
			param->unborrow_in();
			param->unborrow_out();
		}
	}
}	
void	c_traxs::make_slave()
{
	_array_pt.do_method< &c_trax::make_slave >();
}

AAA_ERR	c_traxs::save_do_before( o_str CONST & filename_in )
{
	set_param_nb_used( 0 );	//param find more elegant method
	return	AAA_OK;
}

AAA_ERR	c_traxs::save_do_after( o_str CONST & filename_in )
{
	INT32 nb = _array_pt.get_nb();
	set_param_nb_used( nb );	//param find more elegant method

	o_str& filename = o_str::push_name();
		filename.set_fname_relative( filename_in );
		filename.replace_ext( "00.trax" );

		INT32 i_end = _array_pt.get_index_end();
		for( INT32 i=_array_pt.get_index_begin(); i<=i_end; ++i )
		{
			filename.set_digits( -7, 2, i+1 );
			c_trax* p = _array_pt.get(i);
			if( p )
				p->save_to_this_file( filename );
			else
				c_file::FREMOVE( filename );
		}
	o_str::pop_name();
	return	AAA_OK;
}

FINLINE	c_trax*	c_traxs::load_one_from_existing_file( INT32 index, o_str CONST & filename )
{
	if( !check_trax_index( str_trax_err, index ) )
	{
		box_err( str_trax_err );
		return nullptr;
	}

	c_trax*	p_before = _array_pt.get(index);
	c_trax*	trax = p_before;
	obj_get_load_from_existing_file( trax, filename );
	if( !p_before ) //	only if we created a new trax
	{
		if( trax )
		{
			c_namer::set_file_only( trax, filename );	//todo check
			trax->trax_init( index, get_name_str() );
			get_param(index)->attach_param( trax->get_param_begin(), trax->get_param_nb_used() );	//param
//hack should be in trax ?
//#if	AAA_NET()
			if( net && net->is_slave() )
				trax->make_slave();
//#endif
		}
		_array_pt.set( index, trax );
	}
	return trax;
}

AAA_ERR	c_traxs::load_do_before( o_str CONST & filename_in )
{
	set_param_nb_used( 0 );	//param
	return	AAA_OK;
}

AAA_ERR	c_traxs::load_do_after( o_str CONST & filename_in )
{
//	disconnect_all();	//hack to make sure the connexion with c_pref are not erased
//	trax_free_all();

	INT32 nb = _array_pt.get_nb();
	set_param_nb_used( nb );	//param

	o_str& filename = o_str::push_name();
		filename.set_fname_relative( filename_in );
		filename.replace_ext( "00.trax" );

		for( INT32 i=0; i<nb; ++i )
		{
			filename.set_digits( -7, 2, i+1 );
			if( c_file::is_exist(filename) )
				load_one_from_existing_file( i, filename );
		}
	o_str::pop_name();
	return	AAA_OK;
}

