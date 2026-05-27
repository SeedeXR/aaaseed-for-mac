#include "bdd_kramer_p2k.h"
#include "obj_ui/com/serial.h"
#include "time/aaa_time.h"

FACTORY_CREATE_PROP_V1( c_bdd_kramer_p2k, bdd_kramer_p2k, Kramer protocol 2000, bdd_kramer_p2k, sub_menu="Hardware"; );

enum KRAMER_TYPE : INT32
{
	VS_606 = 0,
	TYPE_NB
};


static	C_PCHAR_C	type_str[TYPE_NB]=
{
	"VS_606"	
};

namespace	n_bdd_kramer_p2k
{
	CONSTEXPR INT32	BASE_PARAM_NB	=	6 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	OUTPUT_NB		=	16;
	CONSTEXPR INT32	GROUP_PARAM_NB	=	1;	

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	OUTPUT_NB
									+	GROUP_PARAM_NB;
			
	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(			active )
		PARAM_DEF_INT32(			com_port_nb, 2, 1, 1, 32 )
		PARAM_DEF_BOOL_OFF(			open )
		PARAM_DEF_BOOL_OFF(			verbose )
		PARAM_DEF_DOUBLE(			inter_command_delay, 1, 0, 0, PARAM_MAX_DOUBLE )
		PARAM_DEF_SYMBO_PSTR_ZERO(	type,	type_str	)

		PARAM_DEF_GROUP( Output, OUTPUT_NB )
			PARAM_DEF_INT32( video_out_1, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_2, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_3, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_4, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_5, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_6, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_7, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_8, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_9, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_10, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_11, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_12, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_13, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_14, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_15, 1, 0, 0, 16 )
			PARAM_DEF_INT32( video_out_16, 1, 0, 0, 16 )
	};
}

	

void	c_bdd_kramer_p2k::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _com_port_id_ui );
	param_set_pt( h, _b_open_ui );
	param_set_pt( h, _b_verbose );
	param_set_pt( h, _inter_command_delay );
	param_set_pt( h, _s_type );

	++h;
	for( INT32 i=0; i<n_bdd_kramer_p2k::OUTPUT_NB; ++i )
		param_set_pt( h, _output_ui[i]);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_kramer_p2k)
,_b_open		{false}	
{
	param_init_with( n_bdd_kramer_p2k::param, n_bdd_kramer_p2k::PARAM_NB_MAX );
	_serial = nullptr;
}

c_bdd_kramer_p2k::~c_bdd_kramer_p2k()
{
	close();
}

void	c_bdd_kramer_p2k::set_port( INT32 port_id)
{
	_com_port_id_ui = port_id;
}

namespace {
	UCHAR buf[3*32];
}

void	c_bdd_kramer_p2k::update_low()
{
	if( _s_type == VS_606 )
	{
		DOUBLE	t = aaa::time::get_real_time();
		if( (t-_command_time_last ) > _inter_command_delay )
		{
			UCHAR* p;
			p = buf;
			for( INT32 i=0; i<6; ++i )
			{
				if( _output[i] != _output_ui[i] )
				{
					if( _b_verbose )
						VERBOSE_PRINT_STRING( "Kramer switch output %d to input %d",  i+1, _output_ui[i] );
					*p++ = 0x00;
					*p++ = 0x80 + i + 1;
					*p++ = 0x80 + _output_ui[i];

					_output[i] = _output_ui[i];
					break;
				}
			}
			if( p != buf )
			{
				_serial->write( buf, INT32(p-buf) );
				_command_time_last = t;
			}
		}
	}
}

void	c_bdd_kramer_p2k::update()
{
	if( is_active() )
	{
		if( _b_open != _b_open_ui )
		{
			if( _b_open_ui )
				open();
			else
				close();
		}
		if( _b_open )
			update_low();
	}
}

void	c_bdd_kramer_p2k::draw()
{
}

void	c_bdd_kramer_p2k::close()
{
	if( _serial )
	{
		_serial->close();
		obj_delete( _serial );	
	}
	_b_open = false;
}

void	c_bdd_kramer_p2k::open()
{
	if( !_serial )
	{
		_serial = new c_serial;
		if( _serial )
		{
			_serial->set_baudrate( c_serial::BAUDRATE_9600 );
			if( NOERR(_serial->open( _com_port_id_ui )) )
			{
				_b_open = true;
				_command_time_last = aaa::time::get_real_time();
			}
			else
			{
				err_print( "Can't start serial for Kramer on COM%d:", _com_port_id_ui );
				close();
			}
		}
		else
			err_print( "Can't create serial for Kramer");
	}
}
