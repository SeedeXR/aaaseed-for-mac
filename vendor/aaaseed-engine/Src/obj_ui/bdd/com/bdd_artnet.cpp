#include "bdd_artnet.h"
#include "obj_ui/com/net_link.h"	//because winsock2 need to be first


FACTORY_CREATE_PROP_V1( c_bdd_artnet, bdd_artnet, ArtNet link, bdd_artnet, sub_menu="Com"; );

namespace
{
	CONSTEXPR INT32 BASE_PARAM_NB	=	5 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32 PARAM_TEST_NB	=	9;	
	CONSTEXPR INT32 GROUP_PARAM_NB	=	1;	

	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	PARAM_TEST_NB
									+	GROUP_PARAM_NB;
			
	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_NET_LINK(		net_link,			2, 1	)
		PARAM_DEF_BOOL_OFF(		active				)
		PARAM_DEF_BOOL_OFF(		open_asked			)
		PARAM_DEF_BOOL_LOCKED(	open_state			)
		PARAM_DEF_BOOL_OFF(		verbose				)

		PARAM_DEF_GROUP( Test, PARAM_TEST_NB )
			PARAM_DEF_BOOL_OFF(		test					)
			PARAM_DEF_INT32(		test_universe_begin,	2, 1,	1, 128*256-1	)
			PARAM_DEF_INT32(		test_universe_end,		4, 2,	1, 128*256-1	)
			PARAM_DEF_INT32(		test_led_nb,			2, 80,	1, 512			)

			PARAM_DEF_REAL_INF(		amplitude,				0, 1	)	
			PARAM_DEF_REAL_INF(		offset,					1, 0	)	
			PARAM_DEF_REAL_INF(		freq_space,				80, 2	)
			PARAM_DEF_REAL_INF(		freq_time,				2, 1	)
//			PARAM_DEF_REAL_INF(		freq_time_shift,		1, 0	)
			PARAM_DEF_REAL_INF(		phase_shift,			1, 0	)	
	};								
									
	o_str	o_buf;						
}

	
void	c_bdd_artnet::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_attach_obj_no_inc( h, _link );
	param_set_pt( h, _link_index_ui		);

	param_set_pt( h, get_pt_active()	);
	param_set_pt( h, _b_open_asked		);
	param_set_pt( h, _b_open			);
	param_set_pt( h, _b_verbose			);

	++h;
		param_set_pt( h, _b_test					);
		param_set_pt( h, _test_universe_begin_ui	);
		param_set_pt( h, _test_universe_end_ui		);
		param_set_pt( h, _test_led_nb_ui			);

		param_set_pt( h, _amplitude					);
		param_set_pt( h, _offset					);
		param_set_pt( h, _freq_phase				);
		param_set_pt( h, _freq_time					);
//		param_set_pt( h, _freq_time_shift			);
		param_set_pt( h, _phase_shift				);

	err_param_init_pt(h);
}
			

CONSTRUCTOR_CREATE(c_bdd_artnet)
	,_link(nullptr)
//	,_link_index(-42)
	,_b_open(false)
	,_phase(0.)
{
	param_init_with( param, PARAM_NB_MAX );
}

c_bdd_artnet::~c_bdd_artnet()
{
	close();
}

void	c_bdd_artnet::update_low()
{	
	if( _delta_t.update() )
		_phase = 0.;
	else
		_phase += _delta_t.get_dt() * _freq_time;

	o_buf.alloc( 18 + 512 );
	if( _b_test )
	{
		for( INT32 i=_test_universe_begin_ui; i<=_test_universe_end_ui; ++i )
			send_blk( o_buf, i );
	}
}


void	c_bdd_artnet::update()
{
	if( !is_active() )
		return;

	if( _b_open != _b_open_asked )
	{
		if( _b_open_asked )
			open();
		else
			close();
	}
	if( _b_open )
	{
		if( _link && (_link_index_ui != _link->get_index()) )
		{
			close();
			_link = net->get_link( _link_index_ui );
			open();
		}
		update_low();
	}
	else if( !_link || (_link_index_ui != _link->get_index()) )
	{
		_link = net->get_link( _link_index_ui );
	}
}

void	c_bdd_artnet::close()
{
	//todo	make sure we interup a running protocol
	_b_open = false;
//	if( _serial )
//	{
//		_serial->close();
//		obj_delete( _serial );	
//	}
}

void	c_bdd_artnet::open()
{
	_b_open = true;
	prepare_blk( o_buf );
}

//	http://artisticlicence.com/WebSiteMaster/User%20Guides/art-net.pdf
//	https://art-net.org.uk/structure/streaming-packets/artdmx-packet-definition/

AAA_ERR	c_bdd_artnet::prepare_blk( o_str& o )
{
	o.set( "Art-Net\0x00", 8, -1 );
	o.add_uint16(	0x5000	);	//	+8	OpCodeLo OpCodeHi 0x5000 ArtDmx
	o.add_int16(	0xe00	);	//	+10	Protocol Version Hi Lo this 14
	o.add_char(		0		);	//	+12	Sequence
	o.add_char(		0		);	//	+13	Physical The physical port that sent the packet in range 0-3
	return AAA_OK;
}
	

AAA_ERR	c_bdd_artnet::send_blk( o_str& o, INT32 universe )
{
	UINT8* pt = ( (UINT8*) o.get_changeable() ) + 14;
	//UINT8* pt = (UINT8*)o.get_changeable( 14, 4+512 );
							//	+14 SubUni low 8 bits port address destination
							//	+15 Net high 7 bits port address destination
	*(UINT16*)pt = (0<<8) + universe;
	pt += 2;

	INT32 nb = _test_led_nb_ui * 3;
	*pt++ = nb >> 8;		// +16 Length Hi
	*pt++ = nb & 0xff;		// +17 Length Lo

	DOUBLE f_space = 1. / _freq_phase;
	REAL phase = REAL(_phase);
	for( INT32 i=0; i<_test_led_nb_ui; ++i )
	{
		if( false && i>=40 && i<=60 && (i&3)==0 )
		{
			*pt++ = UINT8(255 * phase);
			*pt++ = 0;
			*pt++ = UINT8(255 * phase);
		}
		else
		{
			DOUBLE plase_space = i*f_space;
			*pt++ =						(UINT8) CLAMP( INT32( (SIN_TURN( plase_space + _phase + _phase_shift*0 ) * _amplitude + _offset) * 256), INT32(0), INT32(255) );

			if( universe & 1 )	*pt++ = (UINT8) CLAMP( INT32( (SIN_TURN( plase_space + _phase + _phase_shift*1 ) * _amplitude + _offset) * 256), INT32(0), INT32(255) );
			else				*pt++ = 0;

			if( universe & 2 )	*pt++ = (UINT8) CLAMP( INT32( (SIN_TURN( plase_space + _phase + _phase_shift*2 ) * _amplitude + _offset) * 256), INT32(0), INT32(255) );
			else				*pt++ = 0;
		}
	}

	net->send_raw( _link_index_ui, o.get(), 18+nb );
	return AAA_OK;
}