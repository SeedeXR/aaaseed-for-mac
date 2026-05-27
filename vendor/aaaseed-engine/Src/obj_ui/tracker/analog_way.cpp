#include "analog_way.h"

#if AAA_TRACKER_ANALOG_WAY()
#include "obj_ui/com/serial.h"
#include "infrastructure/param/param_declare.h"

FACTORY_CREATE_V1( c_analog_way, analog_way, Analog Way, analog_way );

c_analog_way*		analog_way = nullptr;

namespace	n_analog_way
{
	CONSTEXPR INT32	BASE_PARAM_NB		= 3;
	CONSTEXPR INT32	GSW611R_PARAM_NB	= 1;
	CONSTEXPR INT32	GROUP_PARAM_NB		= 1;
	CONSTEXPR INT32	PARAM_NB_MAX		=	BASE_PARAM_NB
										+	GSW611R_PARAM_NB
										+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_BOOL_OFF( open )
		PARAM_DEF_BOOL_OFF( verbose )

		PARAM_DEF_GROUP( GSW611-R, GSW611R_PARAM_NB )
			PARAM_DEF_BOOL_OFF( cut_trig )
	};
}

void	c_analog_way::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open);
	param_set_pt( h, b_verbose);

	++h;
		param_set_pt( h, _b_trig_switch);

//	param_set_pt( h, time_long);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_analog_way)
{
	param_init_with( n_analog_way::param, n_analog_way::PARAM_NB_MAX ); // analog_way_param, ANALOG_WAY_PARAM_NB_MAX);
	serial = nullptr;
}

c_analog_way::~c_analog_way()
{
	close();
}

void	c_analog_way::set_port( INT32 port_id)
{
	com_port_nb = port_id;
}

void	c_analog_way::update_low()
{
	if( _b_trig_switch )
	{
		UCHAR buf[] = "8Y";
		serial->write( buf, 2);
		_b_trig_switch = false;
	}
}

void	c_analog_way::update()
{
	if( is_active() )
	{
		if( _b_open )
		{
			if(!serial )
				open();
			if( serial )
				update_low();
		}
		else
		{
			if( serial )
				close();
		}
	}
}

void	c_analog_way::close()
{
	if( serial )
		{
		serial->close();
		obj_delete( serial );
		}
	_b_open = false;
}

void	c_analog_way::open()
{
	if( !serial )
	{
		bool b = false;
		serial = new c_serial;
		if( serial )
		{
			serial->set_baudrate( c_serial::BAUDRATE_9600 );
			if( NOERR(serial->open( com_port_nb)) )
				b = true;
		}
		if( !b )
		{
			box_err( "Can't start Analog Way" );
			close();
		}
	}
}

#endif //#if AAA_TRACKER_ANALOG_WAY()