
#include "snd_input_bass_asio.h"
#include "sound_bass_asio.h"
#include "sound_bass.h"
#include "snd_master.h"


bool	b_verbose_callback = false;


c_sound_input_bass_asio::c_sound_input_bass_asio()
{
	init();
}

void	c_sound_input_bass_asio::init()
{
	c_sound_input::init();
	//	INT32	i;
	//	ftt_capture_count = 0;
	//	fft_count = 0;
//	_b_enable = false;
}
void	c_sound_input_bass_asio::deinit()
{
	//	INT32	i;
	//	ftt_capture_count = 0;
	//	fft_count = 0;
//	_b_enable = false;
}

c_sound_input_bass_asio::~c_sound_input_bass_asio()
{
	//	TerminateThread( h_notify_thread, 0);   
	disable();
}


DWORD	CALLBACK	bass_asio_record_proc( BOOL b_input, DWORD channel, void *buffer, DWORD len, void* user )
{
	if( b_input )	//recorder
	{
		c_sound_input_bass_asio*	input = (c_sound_input_bass_asio*)user;

		if( !input )
			debug_break( "%s() with NULL user data so NULL sound input" , __FUNCTION__ );
		else
		{
/*
			INT32 len_min = MIN( (INT32)lenght, c_sound_bass_asio::SOUND_BYTE_PER_CAPTURE );
			if( len_min > 0 )
			{
				//input->inc_capture_count();
				//if( gb_verbose_fft )
				//{
				//	if( ( input->ftt_capture_count_ & 0x7f ) == 0 )
				//	{
				//		sprintf( err_str, "capture %d", input->ftt_capture_count_ );
				//		VERBOSE_PRINTF( err_str );
				//	}
				//}
				MEMCPY( (void*)soundinput_input->get_sound_buffer(), buffer, len_min );
			
			}
			return len_min;
*/
//todo in ds perhaps here ?		if( !input->_b_buffer_read )
			{
				if( b_verbose_callback )
					snd::PRINT( "%s() channel %d data len %d", __FUNCTION__, channel, len );
				//Here all the data ( the different channels all together, interleaved ? 
				input->get_capture_stream().put( (UINT8*)buffer, len );
			}
		}
				
	}
	else
	{
		debug_break( "rhis %s() don't deal with output yet", __FUNCTION__ );
	}
	return 0;	//unused
}

void	c_sound_input_bass_asio::update()
{

}

void	report_error( C_PCHAR_C str_fn )
{
	c_sound_bass_asio::report_error( str_fn );
}

bool	c_sound_input_bass_asio::disable_low()
{
	if( !BASS_ASIO_Stop() )
	{
		report_error( "BASS_ASIO_Stop" );
		return false;
	}

	if( !BASS_ASIO_ChannelEnable(	TRUE, 0, nullptr, this ) )
		report_error( "BASS_ASIO_ChannelEnable Null" );

	if( !BASS_ASIO_ChannelReset( TRUE, -1 ,BASS_ASIO_RESET_ENABLE|BASS_ASIO_RESET_JOIN) )// disable & unjoin all output channels
	{
		report_error( "BASS_ASIO_ChannelReset" );
		return false;
	}	

	return true;
}

//todo use BASS_ASIO_SetDevice(outdev[devn]); // set the ASIO device to work with

bool	c_sound_input_bass_asio::enable_low()
{
	bool b_ret;

	// Select Device
	// todonow this should be a settings
	if( !BASS_ASIO_Init( get_device(), BASS_ASIO_THREAD ) )
	{
		report_error( "BASS_ASIO_Init" );
		return false;
	}

	BASS_ASIO_INFO info;
	if( !BASS_ASIO_GetInfo( &info ) )
	{
		report_error( "BASS_ASIO_GetInfo" );
		return false;
	}

	set_input_nb(	info.inputs );
	set_output_nb(	info.outputs );
	
	c_sound_bass_asio::print_info( &info );

	// Print channel info
	for( UINT32 i = 0; i < info.inputs; ++i )
		c_sound_bass_asio::print_channel_info( true, i );

	for( UINT32 i = 0; i < info.outputs; ++i )
		c_sound_bass_asio::print_channel_info( false, i );

//	return true;
//	_asio_buf_len = 512 * 2 * 2;	//todo

	INT32 nb = get_input_nb();

	
	if( !BASS_ASIO_ChannelEnable(	TRUE, 0, (ASIOPROC*)bass_asio_record_proc, this ) )
		report_error( "BASS_ASIO_ChannelEnable" );

	for( INT32 i=1; i < nb; ++i )
	{
		if( !BASS_ASIO_ChannelJoin(	TRUE, i, 0 )  )
			report_error( "BASS_ASIO_ChannelJoin" );
	}

	// Set channel to 16 bit
	if( !BASS_ASIO_ChannelSetFormat( TRUE, 0, BASS_ASIO_FORMAT_16BIT ) )
		report_error( "BASS_ASIO_ChannelSetFormat" );
// Set channel rate to 44khz
	if( !BASS_ASIO_ChannelSetRate(	TRUE, 0, snd::SAMPLE_PER_SEC_DEF ) )
		report_error( "BASS_ASIO_ChannelSetRate" );
	if( !BASS_ASIO_SetRate( snd::SAMPLE_PER_SEC_DEF ) )
		report_error( "BASS_ASIO_SetRate" );


	if( !BASS_ASIO_Start( 0,0 ) )
	{
		report_error( "BASS_ASIO_Start" );
		//Error("Couldn't start recording");
		//free( recbuf );
		//recbuf = 0;
		b_ret = false;
	}
	else
	{
		_sound->_rate = REAL(c_sound_bass_asio::get_rate());
		_sound->_latency_in = c_sound_bass_asio::get_latency( true );
		c_sound_bass_asio::print_rate_latency( true, _sound->_rate, _sound->_latency_in );
		b_ret = true;
		//reset_fft();
	}
	
	return b_ret;
}
