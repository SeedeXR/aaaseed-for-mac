#include "slice.h"
#include "mackie.h"
#include "infrastructure/obj/root.h"
#include "infrastructure/param/param_declare.h"
#include "infrastructure/namer.h"
#include "spy.h"
#include "obj_ui/tracker/trackers.h"

c_node_ui* node_slice()
{
	static	c_node_ui* node_slice_trick = nullptr;
	if( !node_slice_trick )
	{
		obj_new( node_slice_trick )->set_name( "NODE_MIDI_SLICE" );
		node_slice_trick->set_root( trackers::node_tracker );
	}
	return	node_slice_trick;
}

FACTORY_CREATE_V1( c_slice, slice, Slice, slice );

namespace n_slice
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 11;
	CONSTEXPR INT32 GROUP_NB		= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_STR(		name			)
		PARAM_DEF_STR(		fader_info		)
		PARAM_DEF_INT32(	fader_bis_nb,	1, 0,	0, c_mackie::FADER_NB )
		PARAM_DEF_STR(		fader_info_bis	)
		PARAM_DEF_STR(		vpot_info		)
		PARAM_DEF_INT32(	vpot_bis_nb,	1, 0,	0, c_mackie::VPOT_NB )	//max was FADER_NB ???
		PARAM_DEF_STR(		vpot_info_bis	)
		PARAM_DEF_BOOL_OFF(	but_exclusive_a )
		PARAM_DEF_BOOL_OFF(	but_exclusive_b )
		PARAM_DEF_BOOL_OFF(	but_exclusive_c )
		PARAM_DEF_BOOL_OFF(	but_exclusive_d )
	};
}

void	c_slice::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _info[0] );
	param_set_pt( h, _info[1] );
	param_set_pt( h, _shift_nb[0] );
	param_set_pt( h, _info[2] );
	param_set_pt( h, _info[3] );
	param_set_pt( h, _shift_nb[1] );
	param_set_pt( h, _info[4] );
	
	param_set_pt_4( h, _b_but_exclusive );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_slice )
{
	param_init_with( n_slice::param, n_slice::PARAM_NB_MAX );
	set_root( node_slice() );
}
EMPTY_DESTRUCTOR( c_slice )


void c_slice::save( c_slice* pt, INT32 nb, o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );	//for the tracker used twice
		filename.add( "_01" );
		for( INT32 i = 0; i < nb; ++i )
		{
			filename.set_digits( -2, 2, i+1 );
			pt[i].save_to_file_add_ext( filename );
		}
	o_str::pop_name();
}

void c_slice::load( c_slice* pt, INT32 nb, o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );	//for the tracker used twice
		filename.add( "_01" );
		for( INT32 i = 0; i < nb; ++i )
		{
			filename.set_digits( -2, 2, i+1 );
			c_namer::set_file_only( &pt[i], filename );
			SPY_PUSH_RANGE( filename, spy::FILE );
		//		slices[i].set_root( g_app )	;
				pt[i].load_from_file_add_ext( filename );
			SPY_POP_RANGE();
		}
	o_str::pop_name();
}

/*
c_slice*	slices_get_pt( INT32 index )
{
	return &slices[ CLAMP( index, 0, c_mackie::SLICE_NB) ];
}
*/
