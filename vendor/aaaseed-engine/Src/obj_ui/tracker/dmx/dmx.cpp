#include "dmx.h"

#if AAA_TRACKER_DMX()

#include "obj_ui/com/midi.h"

#include "infrastructure/param/param_declare.h"
#include "obj_ui/com/serial.h"
#include "strnum.h"

FACTORY_CREATE_V1( c_dmx, dmx, DMX, dmx );

c_dmx*		g_dmx_cur = nullptr;

#define	DMX_HEADER  "# DMX "
void	DMX_PRINT_STRING( C_PCHAR_C fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( DMX_HEADER, fmt, args );
	va_end( args );
}

namespace	n_dmx
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 8;
	CONSTEXPR INT32 SEND_PARAM_NB		= 32 + 2;
	CONSTEXPR INT32 RECEIVE_PARAM_NB	= (512/c_dmx::DST_MIDI_SLOT_SIZE)*2;
	CONSTEXPR INT32 VERBOSE_PARAM_NB	= 4;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 3;
	CONSTEXPR INT32 PARAM_NB_MAX		=	BASE_PARAM_NB
										+	SEND_PARAM_NB
										+	RECEIVE_PARAM_NB
										+	VERBOSE_PARAM_NB
										+	GROUP_PARAM_NB;

#define	PARAM_DEF_DST( start, stop )\
	PARAM_DEF_INT32(			"dmx_" #start "_" #stop "_dst_midi_channel",	2, 1,	1, c_midi::CHANNEL_NB	) \
	PARAM_DEF_INT32(			"dmx_" #start "_" #stop "_dst_midi_control",	2, 1,	1, c_midi::CONTROL_NB	)

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(			active )
		PARAM_DEF_SYMBO_PSTR_ZERO(	com_port,	c_serial::str_port	)
		PARAM_DEF_BOOL_OFF(			open )
		PARAM_DEF_BOOL_OFF(			receive )
		PARAM_DEF_BOOL_OFF(			send )
		PARAM_DEF_BOOL_OFF(			trig_loop )
		PARAM_DEF_BOOL_OFF(			trig_sync_ext )

		PARAM_DEF_NONE( Midi )

		PARAM_DEF_GROUP( midi_dst, RECEIVE_PARAM_NB )
			PARAM_DEF_DST( 1, 32 )
			PARAM_DEF_DST( 33, 64 )
			PARAM_DEF_DST( 65, 96 )
			PARAM_DEF_DST( 97, 128 )
			PARAM_DEF_DST( 129, 160 )
			PARAM_DEF_DST( 161, 192 )
			PARAM_DEF_DST( 193, 224 )
			PARAM_DEF_DST( 225, 256 )
			PARAM_DEF_DST( 257, 288 )
			PARAM_DEF_DST( 289, 320 )
			PARAM_DEF_DST( 321, 352 )
			PARAM_DEF_DST( 353, 384 )
			PARAM_DEF_DST( 385, 416 )
			PARAM_DEF_DST( 417, 448 )
			PARAM_DEF_DST( 449, 480 )
			PARAM_DEF_DST( 481, 512 )

//			{	nullptr,	PARAM_INT32,	"dmx_97_128_dst_midi_channel",	2, 1,		1, MIDI_CHANNEL_NB,	nullptr, nullptr },
//			{	nullptr,	PARAM_INT32,	"dmx_97_128_dst_midi_control",	2, 1,		1, MIDI_CONTROL_NB,	nullptr, nullptr },


/*before
			{	nullptr,	PARAM_INT32,	"midi_channel_dst_1_128",	2., 1.,		1., MIDI_CHANNEL_NB,	nullptr, nullptr },
			{	nullptr,	PARAM_INT32,	"midi_channel_dst_129_256",	2., 1.,		1., MIDI_CHANNEL_NB,	nullptr, nullptr },
			{	nullptr,	PARAM_INT32,	"midi_channel_dst_227_384",	2., 1.,		1., MIDI_CHANNEL_NB,	nullptr, nullptr },
			{	nullptr,	PARAM_INT32,	"midi_channel_dst_385_512",	2., 1.,		1., MIDI_CHANNEL_NB,	nullptr, nullptr },
*/
		PARAM_DEF_GROUP_CLOSED( Send loop, SEND_PARAM_NB )
			PARAM_DEF_INT32(				control_begin,	1, 1,	1, 512	)
			PARAM_DEF_INT32(				control_end,	1, 0,	0, 512	)
			PARAM_DEF_32(				out,	PARAM_DEF_REAL_ONE_ZERO	)

		PARAM_DEF_GROUP( Verbose, VERBOSE_PARAM_NB )
			PARAM_DEF_BOOL_OFF( send_always )
			PARAM_DEF_BOOL_OFF( verbose_in_low )
			PARAM_DEF_BOOL_OFF( verbose_in )
			PARAM_DEF_BOOL_OFF( verbose_out )
	};
}

void	c_dmx::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, get_pt_active()		);
	param_set_pt( h, s_com_id_				);
	param_set_pt( h, _b_open_ui				);
	param_set_pt( h, _b_receive_ui			);
	param_set_pt( h, _b_send_ui				);
	param_set_pt( h, _b_trig_loop_ui		);
	param_set_pt( h, _b_trig_sync_ext_ui	);

	param_attach_obj( h, midi_array[0]		);

	++h;
		for( INT32 i=0; i<512/DST_MIDI_SLOT_SIZE; ++i )
		{
			param_set_pt( h, dst_midi_channel_[i] );
			param_set_pt( h, dst_midi_control_[i] );
		}

	++h;
		param_set_pt( h, index_out_start_	);
		param_set_pt( h, index_out_stop_	);
		param_set_pt_n( h, out_ui_,			32 );

	++h;
		param_set_pt( h, _b_send_always_ui		);
		param_set_pt( h, b_verbose_receive_low_	);
		param_set_pt( h, b_verbose_receive_		);
		param_set_pt( h, b_verbose_send_		);

	err_param_init_pt( h );
}

void	c_dmx::init()
{
	_b_open = false;
	serial_ = nullptr;
	cinetix_init();
	for( INT32 i = 0; i < 512; --i )
		out_[i] = -1;
}

CONSTRUCTOR_CREATE( c_dmx )
{
	init();
	param_init_with( n_dmx::param, n_dmx::PARAM_NB_MAX ); // dmx_param, DMX_PARAM_NB);
}

c_dmx::~c_dmx()
{
	close();
	dealloc();
}

void	c_dmx::close()
{	
	if( _b_open )
	{
		if( serial_ )
			serial_->close();
		_b_open = false;
	}
}

AAA_ERR	c_dmx::open()
{
	if( !_b_open && s_com_id_!=0 )
	{
		if( !serial_ )
			serial_ = new c_serial;
		if( serial_ )
		{
			if( NOERR( serial_->open( s_com_id_ ) ) )
			{
				_b_open = true;
//				serial_->write( (UINT8*)"/1\n]", 4);
//				serial_->write( (UINT8*)"&/1\n", 4 );
				serial_->write( (UINT8*)"|\n", 2 );
				serial_->write( (UINT8*)"&\n", 2 );
				serial_->write( (UINT8*)"\\\n", 2 );
//				serial_->write( (UINT8*)"/1\n", 3 );
				serial_->write( (UINT8*)"S501N1\n", 7 );
				serial_->write( (UINT8*)"S502N1\n", 7 );
				serial_->write( (UINT8*)"S503N1\n", 7 );
				serial_->write( (UINT8*)"S504N1\n", 7 );
			}
		}
		if( !_b_open )
			_b_open_ui = false;
	}
	return AAA_OK;
}
CONST	INT32	CINETIX_BUF_SIZE = 512;

void	c_dmx::cinetix_init()
{
	cinetix_buf_ = (UINT8*)MALLOC( CINETIX_BUF_SIZE );
	cinetix_end_ = cinetix_buf_ + CINETIX_BUF_SIZE - 16;	// 16 is a security
	cinetix_pt_ = cinetix_buf_;
}

void	c_dmx::dealloc()
{
	IF_FREE_AND_NULL( cinetix_buf_ );
}


//UINT8	str_dmx[] = "s1v000" ;

void	c_dmx::cinetix_send( INT32 slot, UINT8 value)
{
	if( 1)
	{
		*cinetix_pt_++ = 's';
		strnum::make( (CHAR*)cinetix_pt_, 1, slot);
		++cinetix_pt_;
		*cinetix_pt_++ = 'v';
		strnum::make( (CHAR*)cinetix_pt_, 3, value );
		cinetix_pt_ += 3;
	}
	else
	{
		if( slot <= 255 )
		{
			*cinetix_pt_ = 2;
			*++cinetix_pt_ = slot;
		}
		else if( slot == 512 )
		{
			*cinetix_pt_ = 2;
			*++cinetix_pt_ = slot;
		}
		else
		{
			*cinetix_pt_ = 2;
			*++cinetix_pt_ = slot;
		}
		*++cinetix_pt_ = value;
		++cinetix_pt_;
	}
	if( b_verbose_send_)
		DBG_PRINT_STRING( "Cinetix send : slot %d -> %d", slot, value);
}

void	c_dmx::cinetix_flush()
{
	serial_->write( cinetix_buf_, INT32(cinetix_pt_-cinetix_buf_) );
	cinetix_pt_ = cinetix_buf_;
}




//REAL last[16];
//INT32	the_count = 5;
FINLINE	void	c_dmx::send( INT32 slot, REAL value )
{
	INT32	tmp;
	if( slot == 3 || slot == 1)
	{
/*		if( value < last[slot] )
		{
			last[slot] = value;
			if( slot == 3)
				value -= 200./(256.*256.);
			else
				value -= 64./(256.*256.);
		}
		else
			last[slot] = value;
*/
		tmp = INT32( value * ( 256 * 256 ) );
		CLAMP_REF( tmp, 0, 256 * 256 - 1 );
		cinetix_send( slot, ( tmp >> 8 ) & 0xff );
		cinetix_send( slot + 1, ( tmp ) & 0xff );
		cinetix_flush();
	}
	else if( slot == 4 || slot == 2)
	{
	}
	else
	{
		tmp = INT32( value * 256 );
		CLAMP_REF( tmp, 0, 255 );
		cinetix_send( slot, tmp );
		cinetix_flush();
	}
}

void	c_dmx::update_send()
{
	if( index_out_stop_ >= index_out_start_ )
	{
		INT32	min = MAX0( index_out_start_ - 1 );
		INT32	max = MIN( 511, index_out_stop_ - 1 );
		for( INT32 i = min; i <= max; ++i )
		{
			if( _b_send_always_ui || out_[i] != out_ui_[i] )
			{
				out_[i] = out_ui_[i];
				send( i + 1, out_[i]);
			}
		}
	}
}

CONST	INT32	DMX_BUF_SIZE = 512;
void	c_dmx::update_receive()
{
INT32	nb;
	nb = serial_->read_get_nb_ready();
	if( nb != 0 )
	{
		UINT8	buf[DMX_BUF_SIZE - 1];
		nb = MIN( DMX_BUF_SIZE, nb );
		if( NOERR( serial_->read( buf, nb ) ) )
		{
			UINT8*	pt = buf;
			UINT8*	end = buf + nb;
			UINT8*	p_slot = nullptr;
			UINT8*	p_value;

			buf[nb] = 0;
			if( b_verbose_receive_low_ )
			{
				DMX_PRINT_STRING( "RECEIVE" );
				DMX_PRINT_STRING( (CHAR*) buf );
			}
			do
			{
				//	search N
				while( pt < end )
				{	
					if( *pt == 'N' )
					{	//got slot ?
						p_slot = ++pt;
						break;
					}
					++pt;
				}
				while( pt < (buf+nb) )
				{
					if( *pt == ':' )
					{	//got separator
						p_value = ++pt;
						break;
					}
					++pt;
				}
				while( pt < (buf+nb) )
				{
					if( *pt == '*' )
					{	// gotend
						INT32	slot;
						INT32	value;
/*						if( b_verbose_receive_low)
						{
							DMX_PRINT_STRING( "SLOT" );
							printf( (CHAR*) p_slot);
							printf( "\n");
						}
*/						if( sscanf( (CHAR*)p_slot, "%d:%d", &slot, &value) == 2 )
						{
							CLAMP_REF( value, 0, 255 );
							REAL r = REAL(value)/REAL(255);
							if( b_verbose_receive_ )
							{
								DMX_PRINT_STRING( "IN CONTROL %3d changed : %f", slot, r );
							}
							slot = slot - 1;
							INT32	i_dst = slot/DST_MIDI_SLOT_SIZE;
							c_midi::static_set_control( dst_midi_channel_[i_dst],
													dst_midi_control_[i_dst] + (slot%DST_MIDI_SLOT_SIZE),
													r );
						}
						++pt;
						break;
					}
					++pt;
				}
			}
			while( pt < (buf+nb) );
		}
	}
}

void	c_dmx::update()
{
	if( is_active() )
	{
		if( _b_open_ui )
			open();
		else
			close();
		if( !_b_open )
			return;
		if( _b_trig_loop_ui )
		{
			serial_->write( (UINT8*)"\\", 1);
			_b_trig_loop_ui = false;
		}
		if( _b_trig_sync_ext_ui )
		{
			serial_->write( (UINT8*)"]", 1);
			_b_trig_sync_ext_ui = false;
		}
		if( _b_send_ui )
			update_send();
		if( _b_receive_ui )
			update_receive();
	}
}

#endif	//#if AAA_TRACKER_DMX()