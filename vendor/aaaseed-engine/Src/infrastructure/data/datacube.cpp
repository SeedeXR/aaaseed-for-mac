#include "infrastructure/data/datacube.h"
#include "infrastructure/data/datagrid.h"
#include "infrastructure/obj/root.h"
#include "strnum.h"
#include "infrastructure/namer.h"
#include "file/aaa_dir.h"


static	c_node_ui*		node_datagrid = nullptr;

bool	datagrid_refresh_need[DATAGRID_NB];
static	c_bdd_datagrid*	datagrid_2d[DATAGRID_NB];

static	CHAR	datagrid_filename[] = "pref/traxfile00.csv";

void	c_datacube::init()
{
	node_datagrid = new c_node_ui;
	node_datagrid->set_name( "NODE_DATAGRID" );

	for( INT32 i = 0; i < DATAGRID_NB; ++i )
	{
		datagrid_2d[i] = nullptr;
		node_datagrid->obj_get( datagrid_2d[i] );
		//datagrid_2d[i]->set_size();	//todoqqq check it's ok

		strnum::make( datagrid_filename+13, 2, i );
		datagrid_2d[i]->set_data_filename( datagrid_filename );
		datagrid_2d[i]->set_focus();
		datagrid_refresh_need[i] = true;
	}
}

void	c_datacube::dealloc()
{
	for( INT32 i = 0; i < DATAGRID_NB; ++i )
		SAFE_DELETE( datagrid_2d[i] );
}

FINLINE	void	c_datacube::make_valid_index_channel( INT32& channel ) CONST
{
	channel	= IMOD( channel-1,	DATAGRID_NB);
}

bool	c_datacube::is_changed( INT32 channel ) CONST
{
	make_valid_index_channel( channel );
	datagrid_refresh_need[ channel] = true;
	return datagrid_2d[channel]->is_changed();
}
void	c_datacube::push_row(	INT32 channel )
{
	make_valid_index_channel( channel );
	datagrid_2d[channel]->push();
}
void	c_datacube::pop_row(	INT32 channel )
{
	make_valid_index_channel( channel );
	datagrid_2d[channel]->pop();
}

INT32	c_datacube::get_row_nb(	INT32 channel ) CONST
{
	make_valid_index_channel( channel );
	return datagrid_2d[channel]->get_y_max();
}

bool	c_datacube::is_number( INT32 channel, INT32 CONST row, INT32 CONST col ) CONST
{
	make_valid_index_channel( channel );
	datagrid_refresh_need[ channel] = true;
	return datagrid_2d[channel]->is_number( col, row );
}

bool	c_datacube::has_data( INT32 channel, INT32 CONST row, INT32 CONST col ) CONST
{
	make_valid_index_channel( channel );
	datagrid_refresh_need[ channel] = true;
	return datagrid_2d[channel]->has_data( col, row );
}

FP32	c_datacube::get_fp32( INT32 channel, INT32 CONST row, INT32 CONST col ) CONST
{
	make_valid_index_channel( channel );
	datagrid_refresh_need[ channel] = true;
	return datagrid_2d[channel]->get_fp32( col, row );
}

DOUBLE	c_datacube::get_double( INT32 channel, INT32 CONST row, INT32 CONST col ) CONST
{
	make_valid_index_channel( channel );
	datagrid_refresh_need[ channel] = true;
	return datagrid_2d[channel]->get_double( col, row );
}

o_str CONST &	c_datacube::get_str( INT32 channel, INT32 CONST row, INT32 CONST col ) CONST
{
	make_valid_index_channel( channel );
	datagrid_refresh_need[ channel] = true;
	return datagrid_2d[channel]->get_str( col, row );
}

/*
INT32	c_datacube::get_str_len( INT32 channel, INT32 row, INT32 col ) CONST
{
	make_valid_index_channel( channel );
	datagrid_refresh_need[ channel] = true;
	return (INT32)strlen( datagrid_2d[channel]->get_str( col, row ) );
}
*/

CHAR	c_datacube::get_char( INT32 channel, INT32 CONST row, INT32 CONST col, INT32 CONST char_index	) CONST
{
	make_valid_index_channel( channel );
	datagrid_refresh_need[ channel] = true;
	return datagrid_2d[channel]->get_char( col, row, char_index );
}

void	c_datacube::set_double( INT32 channel, INT32 row, INT32 CONST col, DOUBLE CONST r )
{
	make_valid_index_channel( channel );
	if( !row )	//this to prepare the push
		row = -1;
	datagrid_2d[channel]->set_double( col, row, r );
}

void	c_datacube::set_str( INT32 channel, INT32 row, INT32 CONST col, C_PCHAR_C str )
{
	make_valid_index_channel( channel );
	if( !row )	//this to prepare the push
		row = -1;
	datagrid_2d[channel]->set_str( col, row, str );
}

//todoopt add fn to insert several value at a time
void	c_datacube::insert_double( INT32 channel, INT32 row, INT32 CONST col, DOUBLE r )
{
	make_valid_index_channel( channel );
	if( !row )	//this to prepare the push
		row = -1;
	datagrid_2d[channel]->insert_double( col, row, r );
}

void	c_datacube::insert_str( INT32 channel, INT32 row, INT32 CONST col, C_PCHAR_C str )
{
	make_valid_index_channel( channel );
	if( !row )	//this to prepare the push
		row = -1;
	datagrid_2d[channel]->insert_str( col, row, str );
}

void	c_datacube::update()
{
	c_dir::change_to_def();
	for( INT32 i = 0; i < DATAGRID_NB; ++i )
	{
		//if( datagrid_refresh_need[i] )
		{
			datagrid_2d[i]->update();
			datagrid_refresh_need[i] = false;
		}
	}
}

namespace {
	CONST CHAR  datagrid_ext[] = "00";
};

void	c_datacube::load( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( datagrid_ext );

		for( INT32 i = 0; i < DATAGRID_NB; ++i )
		{
			filename.set_digits( -2, 2, i );
			c_namer::set_file_only( datagrid_2d[i], filename );	//todocheck
			datagrid_2d[i]->load_from_file_add_ext( filename );
		}
	o_str::pop_name();
}

void	c_datacube::save( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( datagrid_ext );

		for( INT32 i = 0; i < DATAGRID_NB; ++i )
		{
			filename.set_digits( -2, 2, i );
			datagrid_2d[i]->save_to_file_add_ext( filename );
		}
	o_str::pop_name();
}

c_datacube*	g_datacube = nullptr;
