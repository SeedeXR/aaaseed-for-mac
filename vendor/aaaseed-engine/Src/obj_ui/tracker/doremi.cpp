#include "doremi.h"

#if AAA_TRACKER_DOREMI()

#include "obj_ui/com/serial.h"
#include "infrastructure/param/param_declare.h"

FACTORY_CREATE_V1( c_doremi, doremi, Doremi, doremi );

c_doremi*		doremi = nullptr;

namespace n_doremi
{
	CONSTEXPR INT32 BASE_PARAM_NB_MAX	=	3;
	CONSTEXPR INT32 TRIG_PARAM_NB_MAX	=	24;
	CONSTEXPR INT32 GROUP_PARAM_NB_MAX	=	1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB_MAX
									+	TRIG_PARAM_NB_MAX
									+	GROUP_PARAM_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_BOOL_OFF( open )
		PARAM_DEF_BOOL_OFF( verbose )

		PARAM_DEF_GROUP( TRIG, TRIG_PARAM_NB_MAX )
			PARAM_DEF_BOOL_OFF( start_trig )
			PARAM_DEF_BOOL_OFF( stop_trig )
			PARAM_DEF_INT32( to_go_seq, 2, 1, 0, 9999 )
			PARAM_DEF_BOOL_OFF( to_go_trig )
			PARAM_DEF_BOOL_OFF( seq_1_trig )
			PARAM_DEF_BOOL_OFF( seq_2_trig )
			PARAM_DEF_BOOL_OFF( seq_3_trig )
			PARAM_DEF_BOOL_OFF( seq_4_trig )
			PARAM_DEF_BOOL_OFF( seq_5_trig )
			PARAM_DEF_BOOL_OFF( seq_6_trig )
			PARAM_DEF_BOOL_OFF( seq_7_trig )
			PARAM_DEF_BOOL_OFF( seq_8_trig )
			PARAM_DEF_BOOL_OFF( seq_9_trig )
			PARAM_DEF_BOOL_OFF( seq_10_trig )
			PARAM_DEF_BOOL_OFF( seq_11_trig )
			PARAM_DEF_BOOL_OFF( seq_12_trig )
			PARAM_DEF_BOOL_OFF( seq_13_trig )
			PARAM_DEF_BOOL_OFF( seq_14_trig )
			PARAM_DEF_BOOL_OFF( seq_15_trig )
			PARAM_DEF_BOOL_OFF( seq_16_trig )
			PARAM_DEF_BOOL_OFF( seq_17_trig )
			PARAM_DEF_BOOL_OFF( seq_18_trig )
			PARAM_DEF_BOOL_OFF( seq_19_trig )
			PARAM_DEF_BOOL_OFF( seq_20_trig )
	};
}

void	c_doremi::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open );
	param_set_pt( h, b_verbose_ );

	++h;
		param_set_pt(	h, _b_trig_start_ui	);
		param_set_pt(	h, _b_trig_stop_ui	);
		param_set_pt(	h, to_go_seq_		);
		param_set_pt(	h, _b_to_go_trig_ui	);
		param_set_pt_n( h, _b_trig_seq_ui,	20	);

//	param_set_pt( h, time_long);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE(c_doremi)
{
	param_init_with( n_doremi::param, n_doremi::PARAM_NB_MAX );
	serial_ = nullptr;
}

c_doremi::~c_doremi()
{
	close();
}

void	c_doremi::set_port( INT32 port_id )
{
	com_port_nb_ = port_id;
}

void	c_doremi::send_goto( INT32 seq_index)
{
	UCHAR buf[8];
	INT32	checksum;

	buf[0] = 0x25;
	buf[1] = 0xa7;
	buf[2] = 0x40;
	buf[3] = 0x01;
	buf[4] = 0x00;
	buf[5] = 0x00;

	buf[6] = ( ( seq_index / 10 ) << 4 ) + ( seq_index % 10 ) ;
	checksum = 0;
	for( INT32 i = 0; i < 7; ++i )
		checksum += buf[i];
	buf[7] = checksum & 0xff;
	serial_->write( buf, 8 );
}

void	c_doremi::update_low()
{
	if( _b_trig_start_ui )
	{
		UCHAR buf[3];
		buf[0] = 0x20;
		buf[1] = 0x01;
		buf[2] = 0x21;
		serial_->write( buf, 3 );
		_b_trig_start_ui = false;
	}
	if( _b_trig_stop_ui )
	{
		UCHAR buf[3];
		buf[0] = 0x20;
		buf[1] = 0x00;
		buf[2] = 0x20;
		serial_->write( buf, 3 );
		_b_trig_stop_ui = false;
	}
	if( _b_to_go_trig_ui )
	{
		send_goto( to_go_seq_ );
		_b_to_go_trig_ui = false;
	}
	for( INT32 i = 0; i < 20; ++i )
	{
		if( _b_trig_seq_ui[i] )
		{
			send_goto( i + 1 );
			_b_trig_seq_ui[i] = false;
		}
	}
}

void	c_doremi::update()
{
	if( is_active() )
	{
		if( _b_open )
		{
			if( !serial_ )
				open();
			if( serial_ )
				update_low();
		}
		else
		{
			if( serial_ )
				close();
		}
	}
}

void	c_doremi::close()
{
	if( serial_ )
	{
		serial_->close();
		obj_delete( serial_ );
	}
	_b_open = false;
}

void	c_doremi::open()
{
	if( !serial_ )
	{
		bool	b = false;
		serial_ = new c_serial;
		if( serial_ )
		{
			serial_->set_baudrate( c_serial::BAUDRATE_38400 );
			if( NOERR( serial_->open( com_port_nb_, ODDPARITY ) ) )
				b = true;
		}
		if( !b )
		{
			box_err( "Can't start serial for Doremi" );
			close();
		}
	}
}

#endif //#if AAA_TRACKER_DOREMI()
