#include "aka.h"
#include "midi.h"
#include <stdarg.h>
#include "infrastructure/param/param_st.h"
#ifndef AAA_INCLUDED_OSCRECEIVEDELEMENTS_H
#	include "osc/OscReceivedElements.h"
#endif


//todo	move index_midi in the object
static	CONST	CHAR	AKA_HEADER[] = "# AKA ";
INT32	c_aka::aka_nb;

void	AKA_PRINT_STRING( CONST CHAR* CONST fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	TRACKER_PRINT_STRING( AKA_HEADER, fmt, va_pass( args ) );
	va_end( args );
}

FACTORY_CREATE_V1( c_aka, aka, Akamatsu iphone 2.1, aka );

c_aka*	aka_array[AKA_OBJ_MAX_NB];

void	c_aka::c_init()
{
}

//todo add lock on certain param

namespace	n_aka
{
	static	CONST	INT32	BASE_PARAM_NB					= 22;
	static	CONST	INT32	GROUP_NB						= 0;

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_BOOL_OFF( verbose )
		ST_PARAM_INT32_ZERO( aka_device_id )
		ST_PARAM_INT32_LOCKED( page )
		ST_PARAM_INT32( sliders_button_midi_channel,	2, 1, 1, MIDI_CHANNEL_NB )
		ST_PARAM_INT32( sliders_button_midi_control,	2, 1, 1, MIDI_CONTROL_NB )
		ST_PARAM_INT32( sliders_midi_channel,			2, 1, 1, MIDI_CHANNEL_NB )
		ST_PARAM_INT32( sliders_midi_control,			2, 1, 1, MIDI_CONTROL_NB )
		ST_PARAM_INT32( grid_button_midi_channel,		2, 1, 1, MIDI_CHANNEL_NB )
		ST_PARAM_INT32( grid_button_midi_control,		2, 1, 1, MIDI_CONTROL_NB )
		ST_PARAM_INT32( grid_midi_channel,				2, 1, 1, MIDI_CHANNEL_NB )
		ST_PARAM_INT32( grid_midi_control,				2, 1, 1, MIDI_CONTROL_NB )
		ST_PARAM_INT32( xy_button_midi_channel,			2, 1, 1, MIDI_CHANNEL_NB )
		ST_PARAM_INT32( xy_button_midi_control,			2, 1, 1, MIDI_CONTROL_NB )
		//{	NULL,	PARAM_INT32, "sliders_button_midi_channel",	2, 1,	1, MIDI_CHANNEL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "sliders_button_midi_control",	2, 1,	1, MIDI_CONTROL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "sliders_midi_channel",	2, 1,	1, MIDI_CHANNEL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "sliders_midi_control",	2, 1,	1, MIDI_CONTROL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "grid_button_midi_channel",	2, 1,	1, MIDI_CHANNEL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "grid_button_midi_control",	2, 1,	1, MIDI_CONTROL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "grid_midi_channel",	2, 1,	1, MIDI_CHANNEL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "grid_midi_control",	2, 1,	1, MIDI_CONTROL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "xy_button_midi_channel",	2, 1,	1, MIDI_CHANNEL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "xy_button_midi_control",	2, 1,	1, MIDI_CONTROL_NB,	NULL, NULL },
		ST_PARAM_BOOL_OFF( xy_centered )
		ST_PARAM_REAL( xy_center_size, .0, 0.25, 0., 1. )
		ST_PARAM_INT32( xy_midi_channel,			2, 1, 1, MIDI_CHANNEL_NB )
		ST_PARAM_INT32( xy_midi_control,			2, 1, 1, MIDI_CONTROL_NB )
		ST_PARAM_INT32( matrix_button_midi_channel,	2, 1, 1, MIDI_CHANNEL_NB )
		ST_PARAM_INT32( matrix_button_midi_control,	2, 1, 1, MIDI_CONTROL_NB )
		ST_PARAM_INT32( matrix_midi_channel,		2, 1, 1, MIDI_CHANNEL_NB )
		ST_PARAM_INT32( matrix_midi_control,		2, 1, 1, MIDI_CONTROL_NB )
		//{	NULL,	PARAM_REAL, "xy_center_size",	.0, 0.25,	0., 1.,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "xy_midi_channel",	2, 1,	1, MIDI_CHANNEL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "xy_midi_control",	2, 1,	1, MIDI_CONTROL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "matrix_button_midi_channel",	2, 1,	1, MIDI_CHANNEL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "matrix_button_midi_control",	2, 1,	1, MIDI_CONTROL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "matrix_midi_channel",	2, 1,	1, MIDI_CHANNEL_NB,	NULL, NULL },
		//{	NULL,	PARAM_INT32, "matrix_midi_control",	2, 1,	1, MIDI_CONTROL_NB,	NULL, NULL },
	};
};

void	c_aka::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_verbose );
	param_set_pt( h, _iphone_id_to_receive );
	param_set_pt( h, _page );
	param_set_pt( h, _sliders_but_dst_channel );
	param_set_pt( h, _sliders_but_dst_control );
	param_set_pt( h, _sliders_dst_channel );
	param_set_pt( h, _sliders_dst_control );
	param_set_pt( h, _grid_but_dst_channel );
	param_set_pt( h, _grid_but_dst_control );
	param_set_pt( h, _grid_dst_channel );
	param_set_pt( h, _grid_dst_control );
	param_set_pt( h, _xy_but_dst_channel );
	param_set_pt( h, _xy_but_dst_control );
	param_set_pt( h, _b_xy_centered );
	param_set_pt( h, _xy_center_size );
	param_set_pt( h, _xy_dst_channel );
	param_set_pt( h, _xy_dst_control );
	param_set_pt( h, _matrix_but_dst_channel );
	param_set_pt( h, _matrix_but_dst_control );
	param_set_pt( h, _matrix_dst_channel );
	param_set_pt( h, _matrix_dst_control );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_aka )
,_page(0)
{
	if( get_factory()->is_obj_first() )
		c_aka::c_init();

	CHAR	str[] = "Aka_a";
	str[4] = 'a' + get_factory()->get_obj_count() - 1;
	set_name( str );

	param_init_with( n_aka::param, n_aka::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_aka )

void c_aka::update()
{
	param_init_pt();	//hack architecture bug on updating parameter with changing adresses
}

BOOL	c_aka::osc_receive( CONST osc::ReceivedMessage& msg )
{
	if( !_b_active )
		return FALSE;

	osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
	osc::ReceivedMessageArgumentIterator it_end = msg.ArgumentsEnd();

	//	ARG 1 is the device Id set on the aka pref page
	osc::ReceivedMessageArgument	arg = *it;
	if( !arg.IsInt32() )
		return FALSE;
	INT32	id = arg.AsInt32();
	if( id != _iphone_id_to_receive )
		return FALSE;

	//	ARG 2 is the page
	arg = *++it;
	if( !arg.IsInt32() )
		return FALSE;
	INT32	page = arg.AsInt32();

	//	ARG 3 is a controller number 
	arg = *++it;
	if( !arg.IsInt32() )
		return FALSE;
	INT32	ctl = arg.AsInt32();
	if( ctl == -1 || _page != page )	//	-1 for a change of page
	{
		_page = page;
		if( _b_verbose )
			AKA_PRINT_STRING( "%d switch to page %d", id, _page );
		if( ctl == -1 )
			return TRUE;
	}	

	//	ARG 4 is controller type
	if( ++it == it_end )
		return TRUE;
	arg = *it;
	if( !arg.IsInt32() )
		return TRUE;
	INT32	type = arg.AsInt32();

	//	ARG 5 is controller state (0 release)
	if( ++it == it_end )
		return TRUE;
	arg = *it;
	if( !arg.IsInt32() )
		return TRUE;
	INT32	state = arg.AsInt32();
	if( _b_verbose )
		AKA_PRINT_STRING( "%d page %d : controller %d is type %d in state %d", id, _page, ctl, type, state );

	if( page==2 )
	{
		if( ctl==10 && state>=0 )
		{
			if( ++it == it_end )
				return TRUE;
			arg = *it;
			if( !arg.IsFloat() )
				return TRUE;
			REAL	x = arg.AsFloat();

			if( ++it == it_end )
				return TRUE;
			arg = *it;
			if( !arg.IsFloat() )
				return TRUE;
			REAL	y = arg.AsFloat();

/*
			if( _b_xy_centered )
			{
			if( x < 0.5-_xy_center*.5 )
				x = 2.*x/(1.-_xy_center)-1.;
			else if( x <= 0.5+_xy_center*.5 )
				x = 0.
			else
				x = 2.*(x-2.)/(1.-_xy_center)+1.;	//todo
			}
			*/
			c_midi::static_set_control( _xy_dst_channel, _xy_dst_control, x );
			c_midi::static_set_control( _xy_dst_channel, _xy_dst_control+1, y );
		}
	}




//	{
//		print_osc_arg( *it );
//		it++;
//	}
	return TRUE;
}

void	c_aka::c_osc_receive( CONST osc::ReceivedMessage& msg )
{
	if( msg.ArgumentCount()<5 )
		return;
	for( INT32 i=0; i<c_aka::aka_nb; i++ )
	{
		if( aka_array[i] )
		{
			if( aka_array[i]->osc_receive( msg ) )
				return;
		}
	}
}
