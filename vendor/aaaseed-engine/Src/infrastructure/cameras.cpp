#include "infrastructure/cameras.h"
#include "draw/seedcam.h"
#include "strnum.h"
#include "infrastructure/namer.h"

FACTORY_CREATE_V1( c_cameras, cameras, Cameras, cams );

CONSTRUCTOR_CREATE(c_cameras)
{
//	params = nullptr;

//	set_name( str_id );
	param_init_with( nullptr, 0 );
}

c_cameras::~c_cameras()
{
	dealloc();
}

//todofocus this object have no param_header so we had to avoid set_focus() doing its job
//todofocus do a class for that, obj_ui with no param ?
void	c_cameras::set_focus()		{}
void	c_cameras::param_init_pt()	{}

void	c_cameras::set_nb( INT32 nb )
{
	dealloc();
	_array_pt.set_nb( nb );
	//	params = nullptr;

	//	set_name( str_id );
	//	param_init_with( params, nb );
}

void c_cameras::dealloc()
{
	_array_pt.delete_objs();
}

void c_cameras::draw()		{	_array_pt.do_method_index< &c_seedcam::draw > ();	}

static	CHAR	cam_err[128];
FINLINE bool	c_cameras::check_index( CHAR* str, INT32 index )
{
	if( _array_pt.check_index( index ) )
		return true;
	sprintf( str, "Index out of range, can't get camera %d", index );
	return false;
}

c_seedcam*	c_cameras::get_always( INT32 index )
{
	if( check_index( cam_err, index )  )
	{
		auto p = _array_pt.get( index );
		if( !p )
		{
			obj_get( p );
//todonow	p->c_camera_init( params+index, index, get_name() ); //see c_trax_init
			_array_pt.set( index, p );
		}
		return p;
	}
	box_err( cam_err );
	return nullptr;
}

c_seedcam*	c_cameras::get( INT32 index )
{
	if( check_index( cam_err, index )  )
	{
		return	_array_pt.get( index );
	}
	ERR_PRINT_STRING( cam_err );
	return nullptr;
}

c_seedcam*	c_cameras::get_free()	//todonow extend when not enough
{
	INT32 i = _array_pt.get_index_free();
	if( 0 <= i )
	{
		get_always(i);
	}
	return nullptr;	
}

FINLINE	c_seedcam*	c_cameras::load_one( INT32 index, o_str CONST & filename )
{
	if( !check_index( cam_err, index ) )
	{
		box_err( cam_err );
		return nullptr;
	}
	c_seedcam* p = _array_pt.get(index);
	if( !c_file::is_exist(filename) )
	{
		if( p )
			p->param_set_to_ina();
		return p;
	}
	obj_get_load_from_existing_file( p, filename );
	if( p )
	{
		if( index != 0 )
			c_namer::set_file_only( p, filename );

		//if( !_array_pt.get(index) ) //	only if we created a new cam
		//{
		//todonow		p->c_camera_init( params+index, index, get_name() );	 //see c_trax_init
		//}
	}
	_array_pt.set( index, p );
	return p;
}

//todooptqq	I should not parse all
void	c_cameras::update()
{
//		c_seedcam*	p;
/*		for( INT32 i=i_start; i<=i_stop; ++i )
			if ( p = v_cameras[i] )
				p->update();
*/
}

static C_PCHAR_C cam_ext = "cam";

AAA_ERR	c_cameras::load_do_after( o_str CONST & filename_in )
{
//	if( c_state_master::is_state_ref() )
//		dealloc();
	o_str& filename = o_str::push_name( filename_in );

//		fname::make_name_relative( filename, filename_in );
		filename.replace_ext( cam_ext );

		load_one( 0, filename );

		filename.replace_ext( "00.cam" );
	
		INT32 nb = _array_pt.get_nb();
		for( INT32	i = 1; i < nb; ++i )
		{
			filename.set_digits( -6, 2, i );
			load_one( i, filename );
		}

	o_str::pop_name();
	return	AAA_OK;
}

AAA_ERR	c_cameras::save_do_after( o_str CONST &  filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.replace_ext( cam_ext );

		c_seedcam*	p = _array_pt.get(0);
		if( p )
			p->save_to_this_file( filename );

		filename.replace_ext( "00.cam" );

		INT32 nb = _array_pt.get_nb();
		for( INT32 i = 1; i < nb; ++i )
		{
			p = _array_pt.get(i);
			if( p )
			{
				filename.set_digits( -6, 2, i );
				p->save_to_file( filename);
			}
		}
	o_str::pop_name();
	return AAA_OK;
}
