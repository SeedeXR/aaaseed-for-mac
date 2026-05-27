#include "err.h"
#include <stdio.h>
#include "sndinput.h"
#include "aaa_math.h"
#include "averager.h"
#include "file_csv.h"
#include "aaa_util.h"
//#include "sound.h"
#include "param.h"
#ifndef AAA_SOUND_NEW_H
#	include "Sound/sound_new.h"
#endif

c_snd_input* c_snd_input::snd_inputs[SND_INPUT_NB];

BOOL	b_verbose_fft;
void fft_verbose_set( BOOL in)
{
	b_verbose_fft = in;
	PRINT_SWITCH_STATE("FFT Verbose", b_verbose_fft);
}

void fft_verbose_flip()
{
	fft_verbose_set( !b_verbose_fft);
}


REAL	integrate_1d_real( REAL* p, REAL min, REAL max )
{
INT32	i_min;
INT32	i_max;
REAL	out;
	i_min = I_FLOOR(min);
	i_max = I_FLOOR(max);
	if ( i_min == i_max )
		{
		out = *(p+i_min)*(max-min);
		}
	else
		{
		p += i_min;
		out = *p * (1.-(min-i_min)) ;
		p++;
		while( ++i_min < i_max )
			out += *p++;
		out += *p*(max-i_max);
		}
	return out;
}
 
//static	list<c_snd_input*>	list_snd_input;
//
//extern	c_snd_input*	snd_input_find_by_id( INT32 id )
//{
//c_snd_input*	pt;
//list<c_snd_input*>::iterator	it;
//	for( it = list_snd_input.begin(); it != list_snd_input.end(); it++ )
//		{
//		pt = *it;
//		if( pt->get_id() == id )
//			{
//			return pt;
//			}
//		}
//	ERR_PRINT_STRING( "capture window unknown");
//	return NULL;
//}
//
//void	snd_input_add( c_snd_input* pt)
//{
//	list_snd_input.push_back( pt );
//}
//
//void	snd_input_remove( INT32	id )
//{
//c_snd_input*	pt;
//
//	pt = snd_input_find_by_id( id );
//	if( pt)
//		list_snd_input.remove( pt);
//}
//
//void	snd_input_deinit()
//{
//	while( !list_snd_input.empty() )
//	{
//		c_snd_input*	pt;
//		pt = *list_snd_input.begin();
//		list_snd_input.pop_front();		//hack is it thread/callback safe ?
//		delete pt;
//	}
//}


//// buffer the recorded data
//BOOL CALLBACK Bass_RecordingCallback( HRECORD handle, void *buffer, DWORD length, DWORD user )
//{
//	c_snd_input*	snd_input = (c_snd_input*)user;
//
//	//todoquick, sound_buffer size is 2048, we have to set callback timing to match this size
//	DBG_PRINT_STRING("Len of bass record buffer %d", length);
//	MEMCPY( (void*)snd_input->sound_buffer, buffer, 2048 );
//	//// increase buffer size if needed
//	//if ((reclen%BUFSTEP)+length>=BUFSTEP) {
//	//	recbuf=realloc(recbuf,((reclen+length)/BUFSTEP+1)*BUFSTEP);
//	//	if (!recbuf) {
//	//		rchan=0;
//	//		Error("Out of memory!");
//	//		MESS(10,WM_SETTEXT,0,"Record");
//	//		return FALSE; // stop recording
//	//	}
//	//}
//	//// buffer the data
//	//MEMCPY( recbuf + reclen, buffer, length );
//	//reclen += length;
//	return TRUE; // continue recording
//}


////global level function for retrieving and dumping the sound capture buffer
//DWORD CALLBACK HandleNotifications(LPVOID lpvoid)
//{
//DWORD			hr = DS_OK;
//DWORD			hRet = 0;
//UINT32			index;
//LPVOID			lpvAudioPtr1;
//DWORD			dwAudioBytes1; 
//LPVOID			lpvAudioPtr2; 
//DWORD			dwAudioBytes2;
//DWORD			capture_pos;
//DWORD			read_pos;
//
//c_snd_input*	snd_input = (c_snd_input*)lpvoid;
//
//	if( !snd_input)
//		{
//		ERR_PRINT_STRING( "DirectSound HandleNotifications() with NULL snd_input");
//		return FALSE;
//		}
//
////	SOUND_PRINT_STRING( "DirectSound HandleNotifications() with snd_input : %X", snd_input );
//	while( snd_input->b_sound_capture_thread_on_ )
//		{
//		hRet = WaitForMultipleObjects( c_snd_input::SOUND_BUFFER_NB + 1, snd_input->h_NotifyEvent_, FALSE, 1000 );
//		 
//		index = hRet - WAIT_OBJECT_0;
//		if ( index < 0 || index > c_snd_input::SOUND_BUFFER_NB )
//			{
//			if( index == WAIT_TIMEOUT )
//				{
//				ERR_PRINT_STRING( "DirectSound snd_input TimeOut" );
//				}
//			else if( index == c_snd_input::SOUND_BUFFER_NB )
//				{
//				ERR_PRINT_STRING( "DirectSound Maa: last strange Object notified" );
//				}
//			else
//				ERR_PRINT_STRING( "DirectSound snd_input Notify Error" );
//			}
//		else if ( index == c_snd_input::SOUND_BUFFER_NB )
//			{
//			break;
//			}
//		else
//			{
//		// Deal with the event that got signaled.
////		fprintf(outerr,"Notify #%d ",index);
//			if( !snd_input->b_buffer_read_ )
//				{
//				snd_input->lpDSBuffer_->GetCurrentPosition( &capture_pos, &read_pos );
//	//		fprintf(outerr,
//	//				"Capture:%d Read:%d Expected:%d ",
//	//				capture_pos,
//	//				read_pos,
//	//				(index+1) * SOUND_BYTE_PER_CAPTURE);
//	
//				if(	snd_input->lpDSBuffer_->Lock( index * c_snd_input::SOUND_BYTE_PER_CAPTURE, c_snd_input::SOUND_BYTE_PER_CAPTURE,
//												&lpvAudioPtr1, &dwAudioBytes1, &lpvAudioPtr2, &dwAudioBytes2, NULL ) == DS_OK )
//					{
//					if( ( dwAudioBytes1 + dwAudioBytes2 ) <= c_snd_input::SOUND_BYTE_PER_CAPTURE )
//						{
//						snd_input->ftt_capture_count_++;
//						if( b_verbose_fft )
//							{
//							if ( ( snd_input->ftt_capture_count_ & 0x7f ) == 0 )
//								VERBOSE_PRINTF( "capture %d", snd_input->ftt_capture_count_ );
//							}
//
//						MEMCPY( (void*)snd_input->sound_buffer_, lpvAudioPtr1, dwAudioBytes1);
//						if( dwAudioBytes2 )
//							//todo try to use MEMCPY
//							MEMCPY( (void*)( snd_input->sound_buffer_ + dwAudioBytes1 ), lpvAudioPtr2, dwAudioBytes2 );
//	//					fprintf(outerr," P1:%X size1:%d P2:%x size2:%d ",
//	//							lpvAudioPtr1,  
//	//							dwAudioBytes1,  
//	//							lpvAudioPtr2,  
//	//							dwAudioBytes2);
//						}
//					else
//						ERR_PRINT_STRING( "DirectSound !Illegal buffer size!" );
//	
//					snd_input->lpDSBuffer_->Unlock( lpvAudioPtr1, dwAudioBytes1, lpvAudioPtr2, dwAudioBytes2 );
//
//					if( snd_input->u8_fft_done_ )
//						{
//						if( b_verbose_fft )
//							VERBOSE_PRINTF("skip fft");
//						}
//					else
//						{
//						}
//					}
//				else
//					{
//					ERR_PRINT_STRING( "DirectSound !Unable to lock!");
//					}
//				}
//			}
//
//		} // while
//	return TRUE;
//} 


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void	c_snd_input::freeze()
{
	b_buffer_read_ = TRUE;
}

void	c_snd_input::unfreeze()
{
	b_buffer_read_ = FALSE;
}

void	c_snd_input::reset_fft()
{
	u8_fft_done_ = 0;
}

static unsigned int ShuffleIndex(unsigned int i, int WordLength)
// Function     : Finds the shuffle index of array elements. The array length
//                must be a power of two; The power is stored in "WordLength".
//   Return value : With "i" the source array index, "ShuffleIndex"
//                  returns the destination index for shuffling.
//   Comment      : -

{
unsigned int  NewIndex;
unsigned char BitNr;
	NewIndex = 0;
	for (BitNr = 0; BitNr < WordLength; BitNr++)
		{
		NewIndex = NewIndex << 1;
		if ( (i&1) != 0)
			NewIndex = NewIndex + 1;
		i = i >> 1;
		}
	return NewIndex;
}


#if	USE_FFT_DENPO
void	c_snd_input::alloc_fft_denpo()
{
//UINT32	i; 
UINT32	dst;
UINT32	tmp;
	//	initialize the fft index shuffling precompute array
	for( UINT32 i = 0; i < SOUND_SAMPLE_PER_CAPTURE; i++ )
		{
		//pu16_fft_shuffle[u32_counter] = UINT16(shuffle(u32_counter,SOUND_CAPTURE_SIZE_IN_BIT));
		sound_buffer[i] = 0; // the sound
		reorder[i] = i;
		}
	//	Shuffle all elements
	for( UINT32 i = 0; i < SOUND_SAMPLE_PER_CAPTURE ; i++ )
		{                              //	Find index to exchange elements
		dst = ShuffleIndex( i, SOUND_CAPTURE_SIZE_IN_BIT );
		if (dst > i)
			{	//	Exchange elements
			tmp = reorder[i];
			reorder[i] = reorder[dst]; 
			reorder[dst] = tmp;
			}
		}
}
#endif

c_ps_stuff::c_ps_stuff()
{
	b_calibrate_ = FALSE;
	power_spectrum_ = NULL;
	min_ = NULL;			
	max_ = NULL;
	factor_ = NULL;
#if	USE_FFTW
	plan_ = NULL;
	in_fftw_ = NULL;
	out_fftw_ = NULL;
#endif
#if	USE_FFT_DENPO
	reorder = NULL;
	pc_fft = NULL;
#endif
	alloc( c_snd_input::SOUND_SAMPLE_PER_CAPTURE);
}

void	c_ps_stuff::dealloc()
{
	sample_nb_ = 0;
	spectrum_nb_ = 0;

	delete [] power_spectrum_;
	delete [] min_;			
	delete [] max_;
	delete [] factor_;
#if	USE_FFTW
	if( plan_ )
		{
		fftw_destroy_plan( plan_ );
		plan_ = NULL;
		}		
	delete [] in_fftw_;
	delete [] out_fftw_;
#endif
#if	USE_FFT_DENPO
	delete [] reorder_;
	delete [] pc_fft_;
#endif
}

void	c_ps_stuff::alloc( INT32 sample_per_capture)
{
	dealloc();
	sample_nb_ = sample_per_capture;
	spectrum_nb_ = sample_per_capture / 2 + 1;

	power_spectrum_ = new REAL[spectrum_nb_];
	min_ = new REAL[spectrum_nb_];			
	max_ = new REAL[spectrum_nb_];
	factor_ = new REAL[spectrum_nb_];
#if	USE_FFTW
	in_fftw_ = new DOUBLE[sample_nb_];
	out_fftw_ = new DOUBLE[sample_nb_];
	plan_ = fftw_plan_r2r_1d( sample_nb_ / 2, in_fftw_, out_fftw_, FFTW_R2HC, FFTW_ESTIMATE);
#endif
#if	USE_FFT_DENPO
	reorder = new UINT32[sample_nb];
	pc_fft = C_COMPLEX[sample_nb];
#endif
}


//#define	NORM_SPECTRUM(a)	log((a)+1)
#define	NORM_SPECTRUM(a)	(a)

FINLINE	void	c_ps_stuff::calibrate_check_index( INT32 index, REAL in)
{
	if( calibrate_auto_step_ != 0 )
		{
//		REAL	tmp = (max[index] - min[index]) * calibrate_auto_step;
		REAL	tmp = max_[index] * calibrate_auto_step_;
		max_[index] -= tmp;
//		min[index] += tmp;
		}
	if( in > max_[index] )
		{
		max_[index] = in;
		if( max_[index] != min_[index] )
			factor_[index] = 1. / (max_[index] - min_[index] );
		}
	if ( in < min_[index] )
		{
		min_[index] = in;
		if( max_[index] != min_[index] )
			factor_[index] = 1. / (max_[index] - min_[index]);
		}
}

void	c_ps_stuff::fttw()
{
DOUBLE		tmp; 
DOUBLE		tmp2;
REAL*		pmin;
REAL*		pfac;
REAL*		ps;
INT32		nb;

	//fill the img part with 0
	fftw_execute( plan_ );

	ps = power_spectrum_;
	pmin = min_;
	pfac = factor_;


	tmp = out_fftw_[0];
	tmp = NORM_SPECTRUM( tmp * tmp );  // DC component
	if( b_calibrate_ )
		calibrate_check_index( 0, tmp );
	*ps++ = ( tmp - *pmin++ )* *pfac++;

	nb = sample_nb_ / 2;  //hack
	for( INT32 k = 1; k < ( nb + 1 ) / 2; ++k )  /* (k < N/2 rounded up) */
		{
		tmp = out_fftw_[k];
		tmp2 = out_fftw_[nb - k];
		tmp = NORM_SPECTRUM( tmp * tmp + tmp2 * tmp2 );
		if( b_calibrate_ )
			calibrate_check_index( k, tmp );
		*ps++ = ( tmp - *pmin++ )* *pfac++;
		}

	if( nb % 2 == 0 ) // N is even
		{
		tmp = out_fftw_[nb / 2];
		tmp = NORM_SPECTRUM( tmp * tmp );	// Nyquist freq
		if( b_calibrate_ )
			calibrate_check_index( nb / 2, tmp );
		*ps++ = ( tmp - *pmin++ )* *pfac++;
		}
}

void	c_ps_stuff::calibrate_set( BOOL in )
{
	if( b_calibrate_ != in )
		{
		//INT32	i;
		if( in )
			{
			REAL*	pmin = min_;
			REAL*	pmax = max_;
			for( INT32 i = sample_nb_ / 2 + 1; i > 0; i-- )
				{
				*pmin++ = CALIBRATION_MAX;
				*pmax++ = -CALIBRATION_MAX;
				}
			}
		b_calibrate_ = in;
		PRINT_SWITCH_STATE( "FFT Calibration", b_calibrate_ );
		}
}

void	c_ps_stuff::calibrate_generate_factor( const CHAR* filename )
{
//INT32	i;
INT32	err_nb = 0;

	for( INT32  i = sample_nb_ / 2 + 1; i > 0; i-- )
		{
		if( max_[i] != min_[i] )
			factor_[i] = 1 / (max_[i] - min_[i] );
		else
			{
			err_nb++;
			factor_[i] = 0.;
			}
		}
	mem::DBG_CHECK_HEAP();
	if( err_nb )
		{
		ERR_PRINT_STRING( "%d  data invalid in %s.", err_nb, filename );
		}
}

const	INT32	SND_INPUT_PARAM_NB_MAX	= 11;
ST_PARAM	snd_input_param[SND_INPUT_PARAM_NB_MAX] =
{
	{	NULL,	PARAM_BOOL,		"Active",					0., 1.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_REAL,		"value_inactive",			0., 1.,		0., 1.,				NULL, NULL },
//hack
	{	NULL,	PARAM_INT32,	"device",					1, 0,		0., PARAM_INFINI,	NULL, NULL },
	{	NULL,	PARAM_INT32,	"channel_in_nb",			0., 2,		0., PARAM_INFINI,	NULL, NULL },
//hack
	{	NULL,	PARAM_BOOL,		"flip_stereo",				1., 0.,		0., 1.,				NULL, NULL },

	{	NULL,	PARAM_BOOL,		"enable",					0., 1.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_BOOL,		"Calibration_left",			1., 0.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_BOOL,		"Calibration_right",		1., 0.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_BOOL,		"Calibration_auto",			1., 0.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_REAL,		"Calibration_auto_delay",	.001, 10.,	0, PARAM_INFINI,	NULL, NULL },

//		{	NULL,	PARAM_REAL,	"out_min",		1., 0.,		-PARAM_INFINI, PARAM_INFINI,		NULL, NULL },
//		{	NULL,	PARAM_REAL,	"out_max",		0., 1.,		-PARAM_INFINI, PARAM_INFINI,		NULL, NULL },

	{	NULL,	PARAM_REAL,		"test_freq",				1000., 0.,	0., 22000.,			NULL, NULL },
};

void	c_snd_input::param_init_pt()
{
INT32	h = 0;

	param_set_pt( h, b_active_ );
	param_set_pt( h, value_inactive_ );
	param_set_pt( h, device_ );
	param_set_pt( h, channel_nb_ui_ );
	param_set_pt( h, b_stereo_flip_ );
	param_set_pt( h, b_enabled_ui_ );
	param_set_pt_v2( h, b_calibrate_ui_ );
	param_set_pt( h, b_calibrate_auto_ );
	param_set_pt( h, calibrate_auto_delay_ );
//	param_set_pt( h, out_min_ );
//	param_set_pt( h, out_max_ );
	param_set_pt( h, freq_test_ );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_snd_input)
{
#if	USE_FFT_DENPO
	alloc_fft_denpo();
#endif
	//INT32	size = SOUND_BYTE_PER_CAPTURE;
	//INT32	size2 = sizeof( sound_buffer_ );
	//INT32	size3 = SOUND_BYTE_PER_SAMPLE;
	//INT32	size4 = SOUND_SAMPLE_PER_CAPTURE;
	//INT32	size5 = 1 << SOUND_CAPTURE_SIZE_IN_BIT;
	//INT32	size6 = SOUND_CAPTURE_SIZE_IN_BIT;

	channel_nb_ = 2;
	init();
	set_name("Sound_input");
	param_init_with( snd_input_param, SND_INPUT_PARAM_NB_MAX);
}

void	c_snd_input::init()
{
	ftt_capture_count_ = 0;
	fft_count_ = 0;

	b_enabled_ = FALSE;

	//h_notify_thread_ = (HANDLE)NULL;
	//for( i = 0; i < SOUND_BUFFER_NB; i++ )
	//	h_NotifyEvent_[i] = NULL;
	//p_DSCapture_ = NULL;
	//lpDSBuffer_ = NULL;
	//lpDsNotify_ = NULL;
//	snd_input_add( this );
}

void	c_snd_input::param_init()
{
}

c_snd_input::~c_snd_input()
{
//	TerminateThread( h_notify_thread, 0);   
	disable();
}

AAA_ERR	c_snd_input::save_do_after( char* const filename )
{
FILE	*file;
INT32	nb_write;
char	fname[_MAX_PATH];

	strcpy( fname, filename );
	strcat( fname, ".data_left" );
	file = file_csv::open( fname, TRUE );
	nb_write = file_csv::write_float( file, ps_l_.min_, 1, SOUND_SAMPLE_PER_CAPTURE / 2 + 1 );
	nb_write += file_csv::write_float( file, ps_l_.max_, 1, SOUND_SAMPLE_PER_CAPTURE / 2 + 1 );
	file_csv::close( file );

	strcpy( fname, filename );
	strcat( fname, ".data_right" );
	file = file_csv::open( fname, TRUE );
	nb_write = file_csv::write_float( file, ps_r_.min_, 1, SOUND_SAMPLE_PER_CAPTURE / 2 + 1 );
	nb_write += file_csv::write_float( file, ps_r_.max_, 1, SOUND_SAMPLE_PER_CAPTURE / 2 + 1 );
	file_csv::close( file );

	return (nb_write == ( SOUND_SAMPLE_PER_CAPTURE / 2 + 1 ) * 4 )
			?AAA_OK
			:ERR_ANY;
}

AAA_ERR	c_snd_input::load_do_after( char* const filename )
{
FILE	*file;
INT32	nb_read;
char	fname[_MAX_PATH];

	if( filename )
		{	//TODO move to c_ps_stuff
		ps_l_.calibrate_set( b_calibrate_ui_[0] );
		ps_r_.calibrate_set( b_calibrate_ui_[1] );

		mem::DBG_CHECK_HEAP();

		strcpy( fname, filename );
		strcat( fname, ".data_left" );
		file = file_csv::open( fname, FALSE );
		nb_read = file_csv::read_float( file, ps_l_.min_, 1, SOUND_SAMPLE_PER_CAPTURE / 2 + 1);
		nb_read += file_csv::read_float( file, ps_l_.max_, 1, SOUND_SAMPLE_PER_CAPTURE / 2 + 1 );
		file_csv::close( file );

		mem::DBG_CHECK_HEAP();

		strcpy( fname, filename );
		strcat( fname, ".data_right" );
		file = file_csv::open( fname, FALSE );
		nb_read = file_csv::read_float( file, ps_r_.min_, 1, SOUND_SAMPLE_PER_CAPTURE / 2 + 1 );
		nb_read += file_csv::read_float( file, ps_r_.max_, 1, SOUND_SAMPLE_PER_CAPTURE / 2 + 1 );
		file_csv::close( file );

		mem::DBG_CHECK_HEAP();

		if ( nb_read == ( SOUND_SAMPLE_PER_CAPTURE / 2 + 1 ) * 2 ) 
			{
			ps_l_.calibrate_generate_factor( filename );
			ps_r_.calibrate_generate_factor( filename );
			}
		else
			return ERR_ANY;
		}
	return AAA_OK;
}


//void CALLBACK bass_asio_proc( BOOL input, DWORD channel, void *buffer, DWORD lenght, DWORD user )
//{
//	c_snd_input*	snd_input = (c_snd_input*)user;
//
//	if( !snd_input )
//		{
//		ERR_PRINT_STRING( "Bass Asio bass_asio_proc() with NULL snd_input");
//		return;
//		}
//
//	if( input )
//		{
//		INT32 lenght_min = MIN( (INT32)lenght, c_snd_input::SOUND_BYTE_PER_CAPTURE ); //snd_input->get_asio_buf_lenght() );
//		//pos = lenght - snd_input->get_asio_buf_lenght();
//
//		if( lenght_min > 0 )
//			{
//			snd_input->ftt_capture_count_++;
//			if( b_verbose_fft )
//				{
//				if ( ( snd_input->ftt_capture_count_ & 0x7f ) == 0 )
//					VERBOSE_PRINTF( "capture %d", snd_input->ftt_capture_count_ );
//				}
//			MEMCPY( (void*)snd_input->sound_buffer_, buffer, lenght_min );
//			}
//		}
//
//}

//void	c_snd_input::update_bass()
//{
//INT32	length, pos;
//
//	if( bass_record_chan_ )
//		{
//		length = BASS_ChannelGetData( bass_record_chan_, 0, BASS_DATA_AVAILABLE );
//		//				DBG_PRINT_STRING( "ChannelGetData lenght : %d", length);
//		pos = length - SOUND_BYTE_PER_CAPTURE;
//		// todonow does it work for pos = 0 ?
//		if( pos > 0 )
//			{
//			// Flush buffer to get the last 2048 bytes
//			BASS_ChannelGetData( bass_record_chan_, 0, pos );
//			//					BASS_ChannelSetPosition( bass_record_chan, pos );
//
//			BASS_ChannelGetData( bass_record_chan_, sound_buffer_, SOUND_BYTE_PER_CAPTURE );
//			ftt_capture_count_++;
//			}
//		if( b_verbose_fft )
//			{
//			if ( ( ftt_capture_count_ & 0x7f ) == 0 )
//				VERBOSE_PRINTF( "capture %d", ftt_capture_count_ );
//			}
//		}
//}


void	c_snd_input::print_avalaible_mode( DWORD mask)
{
	if( mask & WAVE_FORMAT_1M08  ) SOUND_PRINT_STRING( "\t11.025 kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_1M16  ) SOUND_PRINT_STRING( "\t11.025 kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_1S08  ) SOUND_PRINT_STRING( "\t11.025 kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_1S16  ) SOUND_PRINT_STRING( "\t11.025 kHz, stereo, 16 bit" );

	if( mask & WAVE_FORMAT_2M08  ) SOUND_PRINT_STRING( "\t22.05  kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_2M16  ) SOUND_PRINT_STRING( "\t22.05  kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_2S08  ) SOUND_PRINT_STRING( "\t22.05  kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_2S16  ) SOUND_PRINT_STRING( "\t22.05  kHz, stereo, 16 bit" );

	if( mask & WAVE_FORMAT_44M08 ) SOUND_PRINT_STRING( "\t44.1   kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_44S08 ) SOUND_PRINT_STRING( "\t44.1   kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_44M16 ) SOUND_PRINT_STRING( "\t44.1   kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_44S16 ) SOUND_PRINT_STRING( "\t44.1   kHz, stereo, 16 bit" );

	if( mask & WAVE_FORMAT_48M08 ) SOUND_PRINT_STRING( "\t48     kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_48S08 ) SOUND_PRINT_STRING( "\t48     kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_48M16 ) SOUND_PRINT_STRING( "\t48     kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_48S16 ) SOUND_PRINT_STRING( "\t48     kHz, stereo, 16 bit" );

	if( mask & WAVE_FORMAT_96M08 ) SOUND_PRINT_STRING( "\t96     kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_96S08 ) SOUND_PRINT_STRING( "\t96     kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_96M16 ) SOUND_PRINT_STRING( "\t96     kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_96S16 ) SOUND_PRINT_STRING( "\t96     kHz, stereo, 16 bit" );	
}

//void	c_snd_input::update_bass()
//{
//INT32	length, pos;
//
//	if( bass_record_chan_ )
//		{
//		length = BASS_ChannelGetData( bass_record_chan_, 0, BASS_DATA_AVAILABLE );
//		//				printf( "ChannelGetData lenght : %d\n", length);
//		pos = length - SOUND_BYTE_PER_CAPTURE;
//		// todonow does it work for pos = 0 ?
//		if( pos > 0 )
//			{
//			// Flush buffer to get the last 2048 bytes
//			BASS_ChannelGetData( bass_record_chan_, 0, pos );
//			//					BASS_ChannelSetPosition( bass_record_chan, pos );
//
//			BASS_ChannelGetData( bass_record_chan_, sound_buffer_, SOUND_BYTE_PER_CAPTURE );
//			ftt_capture_count_++;
//			}
//		if( b_verbose_fft )
//			{
//			if ( ( ftt_capture_count_ & 0x7f ) == 0 )
//				VERBOSE_PRINTF( "capture %d", ftt_capture_count_ );
//			}
//		}
//}

void	c_snd_input::update()
{
	if ( b_enabled_ui_ != b_enabled_ )
		{
		if ( b_enabled_ui_ )
			enable();
		else
			disable();
		}
	if( b_enabled_ )
		{
		sound->update();
		CHAR*	sound_buffer_low;
		sound_buffer_low = sound->get_sound_buffer( id_ );
		if( sound_buffer_low )
			MEMCPY( sound_buffer_, sound_buffer_low, buffer_size_ );

		//if( s_sound_lib_name == SND_LIB_BASS )
		//	{
		//	update_bass();
		//	}
		ps_l_.calibrate_set( b_calibrate_ui_[0] );
		ps_r_.calibrate_set( b_calibrate_ui_[1] );
		if( b_calibrate_ui_[0] || b_calibrate_ui_[1] )
			{
			if( b_calibrate_auto_ && calibrate_auto_delay_ != 0. )
				{
//				calibrate_auto_step = REAL(SOUND_SAMPLE_PER_CAPTURE)/(REAL(SOUND_SAMPLE_PER_SEC)*calibrate_auto_delay);
				//deal with time	
				delta_t_.update();
				ps_l_.calibrate_auto_step_ = delta_t_.get_dt() / calibrate_auto_delay_ ;
				ps_r_.calibrate_auto_step_ = ps_l_.calibrate_auto_step_;
				}
			else
				ps_r_.calibrate_auto_step_ = ps_l_.calibrate_auto_step_ = 0;
			}

		do_fft();
		}
}

void	c_snd_input::disable()
{
	sound->delete_input( id_ );
	//switch( s_sound_lib_name )
	//	{
	//	case SND_LIB_DS :
	//				disable_directsound();
	//				break;
	//	case SND_LIB_BASS :
	//				disable_bass();
	//				break;
	//	case SND_LIB_BASS_ASIO :
	//				disable_bass_asio();
	//				break;
	//	}
}

//void	c_snd_input::disable_bass_asio()
//{
//	if( b_enabled_ )
//		{
//		BASS_ASIO_Stop();
//
//		// release library
//		BASS_ASIO_Free();
//		b_enabled_ = FALSE;
//		}
//}
//
//void	c_snd_input::disable_bass()
//{
//	if( b_enabled_ )
//		{
//		BASS_ChannelStop( bass_record_chan_ );
//
//		// release all BASS stuff
//		BASS_RecordFree();
//		b_enabled_ = FALSE;
////		BASS_Free();
//		}
//}
//
//void c_snd_input::disable_directsound()
//{
//UINT32 u32_return;
//
////hack ?
//	b_sound_capture_thread_on_ = OFF;
//
//	u32_return = DS_OK;
//	if( b_enabled_ )
//		{
//		if( p_DSCapture_ != NULL )
//			{
//			if( lpDSBuffer_ != NULL )
//				{
//				//kill the wait-for-notify thread
//				u32_return = lpDSBuffer_->Stop();
//				if( h_notify_thread_ )
//					{
//					while( b_sound_capture_thread_on_ );
//					release_notification_events();
//					}
//				if( lpDsNotify_ )
//					{
//					u32_return = lpDsNotify_->Release();
//					}
//				//	here is the locking call
//				u32_return = lpDSBuffer_->Release();
//				lpDSBuffer_ = NULL;
//				}
//			//	here is the second	locking call
//			//	if we skip the first one
//			p_DSCapture_->Release();
//			p_DSCapture_ = NULL;
//			}
//		b_enabled_ = FALSE;
//		}
//}
//
//void	c_snd_input::release_notification_events()
//{
//INT32	i;
//	lpDsNotify_->SetNotificationPositions( 0, NULL );
//	for( i = 0; i < SOUND_BUFFER_NB + 1; i ++ )
//		{
//		if( h_NotifyEvent_[i] )
//			{
//			CloseHandle( h_NotifyEvent_[i] );
//			h_NotifyEvent_[i] = (HANDLE)NULL;
//			}
//		}
//}

DSBPOSITIONNOTIFY	rgdsbpn[c_snd_input::SOUND_BUFFER_NB+1];

BOOL	c_snd_input::enable()
{
BOOL	retcode;
BOOL	b_stereo;
	b_stereo = FALSE;
	retcode = FALSE;
	if( sound )
		{
		if( channel_nb_ == 2 )
			b_stereo = TRUE;
		id_ = sound->create_input( device_, 0, b_stereo, 44100, 16 );
		if( id_ > 0 )
			{
			buffer_size_ = sound->get_buffer_size( id_ );
			if( buffer_size_ > 0 )
				sound_buffer_ = new UINT8[buffer_size_];
			if( sound_buffer_ )
				{
				b_enabled_ = TRUE;
				retcode = TRUE;
				}
			}
		}
	//switch( s_sound_lib_name )
	//	{
	//	case SND_LIB_DS :
	//			retcode = enable_directsound();
	//			break;
	//	case SND_LIB_BASS :
	//			retcode = enable_bass();
	//			break;
	//	case SND_LIB_BASS_ASIO :
	//			retcode = enable_bass_asio();
	//			break;
	//	}
	return retcode;
}

//BOOL c_snd_input::enable_bass_asio()
//{
//	if( !b_enabled_ )
//		{
//		//		LPGUID	p_guid;
//		SOUND_PRINT_STRING( "enabling sound_input (Bass Asio)" );
//
//		// Select Device
//		// todonow this should be a settings
//		BASS_ASIO_Init( device_ );
//
//		
//		SOUND_PRINT_STRING( "Device %d : %s", device_, BASS_ASIO_GetDeviceDescription( device_ ) );
//
//		BASS_ASIO_INFO info;
//		BASS_ASIO_GetInfo( &info );
//		SOUND_PRINT_STRING( "Name : %s", info.name );
//		SOUND_PRINT_STRING( "Version : %d", info.version );
//		SOUND_PRINT_STRING( "Nb Of Inputs : %d", info.inputs );
//		SOUND_PRINT_STRING( "Nb Of Outputs : %d", info.outputs );
//		SOUND_PRINT_STRING( "Buffer Min (samples) : %d", info.bufmin );
//		SOUND_PRINT_STRING( "Buffer Max (samples) : %d", info.bufmax );
//		SOUND_PRINT_STRING( "Default buffer (samples) : %d", info.bufpref );
//		SOUND_PRINT_STRING( "Buffer Length Granularity : %d", info.bufgran );
//
//		// Print channel info
//		BASS_ASIO_CHANNELINFO	info_chan;
//		for( INT32 i = 0; i < info.inputs; i++)
//			{
//			CHAR*	str;
//			BASS_ASIO_ChannelGetInfo( TRUE, i, &info_chan );
//			SOUND_PRINT_STRING( "Input Channel %d :", i );
//			SOUND_PRINT_STRING( "    - Name : %s", info_chan.name );
//			SOUND_PRINT_STRING( "    - Group : %d", info_chan.group );
//			switch( info_chan.format )
//				{
//				case BASS_ASIO_FORMAT_16BIT :
//					str = "    - Format : 16 bit integer";
//					break;
//				case BASS_ASIO_FORMAT_24BIT :
//					str = "    - Format : 24 bit integer";
//					break;
//				case BASS_ASIO_FORMAT_32BIT :
//					str = "    - Format : 32 bit integer";
//					break;
//				case BASS_ASIO_FORMAT_FLOAT :
//					str = "    - Format : 32 bit floating point";
//					break;
//				default :
//					str = "    - Format : unknown";
//					break;
//				}
//			SOUND_PRINT_STRING( str );
//			DOUBLE rate_chan = BASS_ASIO_ChannelGetRate( TRUE, i );
//			SOUND_PRINT_STRING( "    - Rate : %g", rate_chan );
//			}
//
//		for( INT32 i = 0; i < info.outputs; i++)
//			{
//			CHAR*	str;
//			BASS_ASIO_ChannelGetInfo( FALSE, i, &info_chan );
//			SOUND_PRINT_STRING( "Output Channel %d :", i );
//			SOUND_PRINT_STRING( "    - Name : %s", info_chan.name );
//			SOUND_PRINT_STRING( "    - Group : %d", info_chan.group );
//			switch( info_chan.format )
//				{
//				case BASS_ASIO_FORMAT_16BIT :
//					str = "    - Format : 16 bit integer";
//					break;
//				case BASS_ASIO_FORMAT_24BIT :
//					str = "    - Format : 24 bit integer";
//					break;
//				case BASS_ASIO_FORMAT_32BIT :
//					str = "    - Format : 32 bit integer";
//					break;
//				case BASS_ASIO_FORMAT_FLOAT :
//					str = "    - Format : 32 bit floating point";
//					break;
//				default :
//					str =  "    - Format : unknown";
//					break;
//				}
//			SOUND_PRINT_STRING( str );
//			SOUND_PRINT_STRING( "    - Rate : %g", BASS_ASIO_ChannelGetRate( FALSE, i ) );
//			}
//
//		// Set channel to 16 bit
//		BASS_ASIO_ChannelSetFormat( TRUE, 0, BASS_ASIO_FORMAT_16BIT );
//		BASS_ASIO_ChannelSetFormat( TRUE, 1, BASS_ASIO_FORMAT_16BIT );
//		// Set channel rate to 44khz
//		BASS_ASIO_SetRate( 44100 );
//		BASS_ASIO_ChannelSetRate( TRUE, 0, 44100);
//		BASS_ASIO_ChannelSetRate( TRUE, 1, 44100);
//
//		BASS_ASIO_ChannelEnable( TRUE, 0, bass_asio_proc, (DWORD)this );
//		BASS_ASIO_ChannelJoin( TRUE, 1, 0 );
//
//		asio_buf_lenght_ = 512 * 2 * 2;
//
//		if( !( BASS_ASIO_Start( 512 ) ) )
//			{
//			//Error("Couldn't start recording");
//			//free( recbuf );
//			//recbuf = 0;
//			b_enabled_ = FALSE;
//			}
//		else
//			{
//			double rate;
//			rate = BASS_ASIO_GetRate();	// get the sample rate
//			SOUND_PRINT_STRING( "Sample Rate : %.0f Hz", rate );
//			SOUND_PRINT_STRING( "Sample Format : 16 bit integer" );
//			INT32 latency;
//			latency = BASS_ASIO_GetLatency( TRUE );
//			SOUND_PRINT_STRING( "Input Latency  : %d samples, %g ms", latency, latency * 1000 / rate );
//			latency = BASS_ASIO_GetLatency( FALSE );
//			SOUND_PRINT_STRING( "Output Latency : %d samples, %g ms", latency, latency * 1000 / rate );
//			b_enabled_ = TRUE;
//			reset_fft();
//			}
//		}
//	return b_enabled_;
//}
//
//BOOL c_snd_input::enable_bass()
//{
//	if( !b_enabled_ )
//		{
//		//		LPGUID	p_guid;
//		SOUND_PRINT_STRING( "enabling sound_input (Bass)" );
//
//		// Select Device
//		// todonow this should be a settings
//		BASS_RecordInit( device_ );
//
//		//INT32	count = 0; // the device counter
//		//while ( BASS_RecordGetDeviceDescription( count ) ) 
//		//	{
//		//	SOUND_PRINT_STRING( "Device %d : %s", count, BASS_RecordGetDeviceDescription( count ) );
//		//	count++;
//		//	}
//		sprintf( err_str, "Device %d : %s", device_, BASS_RecordGetDeviceDescription( device_ ) );
//		SOUND_PRINT_STRING( err_str );
//		BASS_RecordSetInput( -1, BASS_INPUT_ON );
//		BASS_RECORDINFO info;
//		BASS_RecordGetInfo( &info );
//		SOUND_PRINT_STRING( "Driver : %s", info.driver );
//		SOUND_PRINT_STRING( "Nb Of Inputs %d", info.inputs );
//		if( info.formats & WAVE_FORMAT_1M08  ) SOUND_PRINT_STRING( "\t11.025 kHz, mono,   8  bit" );
//		if( info.formats & WAVE_FORMAT_1M16  ) SOUND_PRINT_STRING( "\t11.025 kHz, mono,   16 bit" );
//		if( info.formats & WAVE_FORMAT_1S08  ) SOUND_PRINT_STRING( "\t11.025 kHz, stereo, 8  bit" );
//		if( info.formats & WAVE_FORMAT_1S16  ) SOUND_PRINT_STRING( "\t11.025 kHz, stereo, 16 bit" );
//
//		if( info.formats & WAVE_FORMAT_2M08  ) SOUND_PRINT_STRING( "\t22.05  kHz, mono,   8  bit" );
//		if( info.formats & WAVE_FORMAT_2M16  ) SOUND_PRINT_STRING( "\t22.05  kHz, mono,   16 bit" );
//		if( info.formats & WAVE_FORMAT_2S08  ) SOUND_PRINT_STRING( "\t22.05  kHz, stereo, 8  bit" );
//		if( info.formats & WAVE_FORMAT_2S16  ) SOUND_PRINT_STRING( "\t22.05  kHz, stereo, 16 bit" );
//
//		if( info.formats & WAVE_FORMAT_44M08 ) SOUND_PRINT_STRING( "\t44.1   kHz, mono,   8  bit" );
//		if( info.formats & WAVE_FORMAT_44S08 ) SOUND_PRINT_STRING( "\t44.1   kHz, stereo, 8  bit" );
//		if( info.formats & WAVE_FORMAT_44M16 ) SOUND_PRINT_STRING( "\t44.1   kHz, mono,   16 bit" );
//		if( info.formats & WAVE_FORMAT_44S16 ) SOUND_PRINT_STRING( "\t44.1   kHz, stereo, 16 bit" );
//
//		if( info.formats & WAVE_FORMAT_48M08 ) SOUND_PRINT_STRING( "\t48     kHz, mono,   8  bit" );
//		if( info.formats & WAVE_FORMAT_48S08 ) SOUND_PRINT_STRING( "\t48     kHz, stereo, 8  bit" );
//		if( info.formats & WAVE_FORMAT_48M16 ) SOUND_PRINT_STRING( "\t48     kHz, mono,   16 bit" );
//		if( info.formats & WAVE_FORMAT_48S16 ) SOUND_PRINT_STRING( "\t48     kHz, stereo, 16 bit" );
//
//		if( info.formats & WAVE_FORMAT_96M08 ) SOUND_PRINT_STRING( "\t96     kHz, mono,   8  bit" );
//		if( info.formats & WAVE_FORMAT_96S08 ) SOUND_PRINT_STRING( "\t96     kHz, stereo, 8  bit" );
//		if( info.formats & WAVE_FORMAT_96M16 ) SOUND_PRINT_STRING( "\t96     kHz, mono,   16 bit" );
//		if( info.formats & WAVE_FORMAT_96S16 ) SOUND_PRINT_STRING( "\t96     kHz, stereo, 16 bit" );	
//
//		wfx_.wFormatTag = WAVE_FORMAT_PCM;
//		//		channel_nb = b_stereo_start_with?2:1;	//hack this have to be check 
//		//		channel_nb = 2;
//		wfx_.nChannels = channel_nb_ui_;
//
//		wfx_.nSamplesPerSec = SOUND_SAMPLE_PER_SEC;
//		wfx_.nAvgBytesPerSec = SOUND_BYTE_PER_SECOND_MONO * channel_nb_ui_;
//		wfx_.nBlockAlign = SOUND_BYTE_PER_SAMPLE * channel_nb_ui_;
//		wfx_.wBitsPerSample = SOUND_BIT_PER_SAMPLE;
//		wfx_.cbSize = 0;
//
//		//WAVEFORMATEX *wf;
//		//wf = (WAVEFORMATEX*)(recbuf+20);
//		//wf->wFormatTag = 1;
//		//wf->nChannels = 2;
//		//wf->wBitsPerSample = 16;
//		//wf->nSamplesPerSec = 44100;
//		//wf->nBlockAlign = wf->nChannels * wf->wBitsPerSample / 8;
//		//wf->nAvgBytesPerSec = wf->nSamplesPerSec * wf->nBlockAlign;
//		// start recording @ 44100hz 16-bit stereo
////		if( !( bass_record_chan = BASS_RecordStart( SOUND_SAMPLE_PER_SEC, channel_nb_ui, 0, &Bass_RecordingCallback, (DWORD)this ) ) )
//		if( !( bass_record_chan_ = BASS_RecordStart( SOUND_SAMPLE_PER_SEC, channel_nb_ui_, 0, 0, 0 ) ) )
//			{
//			//Error("Couldn't start recording");
//			//free( recbuf );
//			//recbuf = 0;
//			b_enabled_ = FALSE;
//			}
//		else
//			{
//			b_enabled_ = TRUE;
//			reset_fft();
//			}
//		}
//	return b_enabled_;
//}
//
//BOOL c_snd_input::enable_directsound()
//{
//UINT32 i;
////
////	directsound capture section
////
//DSCCAPS				st_capture_caps;
//DSCBUFFERDESC		st_buffer_desc;
//
//UINT32 u32_return;
//
//	if( !b_enabled_ )
//		{
//		LPGUID	p_guid;
//		SOUND_PRINT_STRING( "enabling sound_input (directsound)" );
//
//		if( device_ )
//			{
//			p_guid = ds_get_guid( device_ );	//hack we should also check we don't open twice the same device
//			if( p_guid == NULL )
//				{
//				ERR_PRINT_STRING( "can't enable sound input. the device don't exist" );
//				b_enabled_ui_ = FALSE;
//				return FALSE;
//				}
//			}
//		else
//			p_guid =  NULL;
//
//		//the direct sound object
//		//todo	enumerate the different capture objects
//		u32_return = DirectSoundCaptureCreate( p_guid, &p_DSCapture_, NULL );
//		if( u32_return != DS_OK)
//			{
//			ds_err_show( u32_return, "in c_snd_input::enable() DirectSoundCaptureCreate(), Can't create DirectSound Capture Object" );
//			b_enabled_ui_ = FALSE;
//			return FALSE;
//			}
//
//		//the capture buffer
//		st_capture_caps.dwSize = sizeof(DSCCAPS);
//		u32_return = p_DSCapture_->GetCaps( &st_capture_caps );
//		if( u32_return != DS_OK )
//			{
//			ds_err_show( u32_return, "in c_snd_input::enable() GetCaps()" );
//			}
//		else
//			{
//			SOUND_PRINT_STRING( "sound capture support these modes with %d channel :", INT32( st_capture_caps.dwChannels ) );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_1M08  ) SOUND_PRINT_STRING( "\t11.025 kHz, mono,   8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_1M16  ) SOUND_PRINT_STRING( "\t11.025 kHz, mono,   16 bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_1S08  ) SOUND_PRINT_STRING( "\t11.025 kHz, stereo, 8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_1S16  ) SOUND_PRINT_STRING( "\t11.025 kHz, stereo, 16 bit" );
//
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_2M08  ) SOUND_PRINT_STRING( "\t22.05  kHz, mono,   8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_2M16  ) SOUND_PRINT_STRING( "\t22.05  kHz, mono,   16 bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_2S08  ) SOUND_PRINT_STRING( "\t22.05  kHz, stereo, 8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_2S16  ) SOUND_PRINT_STRING( "\t22.05  kHz, stereo, 16 bit" );
//
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_44M08 ) SOUND_PRINT_STRING( "\t44.1   kHz, mono,   8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_44S08 ) SOUND_PRINT_STRING( "\t44.1   kHz, stereo, 8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_44M16 ) SOUND_PRINT_STRING( "\t44.1   kHz, mono,   16 bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_44S16 ) SOUND_PRINT_STRING( "\t44.1   kHz, stereo, 16 bit" );
//
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_48M08 ) SOUND_PRINT_STRING( "\t48     kHz, mono,   8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_48S08 ) SOUND_PRINT_STRING( "\t48     kHz, stereo, 8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_48M16 ) SOUND_PRINT_STRING( "\t48     kHz, mono,   16 bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_48S16 ) SOUND_PRINT_STRING( "\t48     kHz, stereo, 16 bit" );
//
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_96M08 ) SOUND_PRINT_STRING( "\t96     kHz, mono,   8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_96S08 ) SOUND_PRINT_STRING( "\t96     kHz, stereo, 8  bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_96M16 ) SOUND_PRINT_STRING( "\t96     kHz, mono,   16 bit" );
//			if( st_capture_caps.dwFormats & WAVE_FORMAT_96S16 ) SOUND_PRINT_STRING( "\t96     kHz, stereo, 16 bit" );	
//			}
// 
//		wfx_.wFormatTag = WAVE_FORMAT_PCM;
////		channel_nb = b_stereo_start_with?2:1;	//hack this have to be check 
////		channel_nb = 2;
//		wfx_.nChannels = channel_nb_ui_;
//
//		wfx_.nSamplesPerSec = SOUND_SAMPLE_PER_SEC;
//		wfx_.nAvgBytesPerSec = SOUND_BYTE_PER_SECOND_MONO * channel_nb_ui_;
//		wfx_.nBlockAlign = SOUND_BYTE_PER_SAMPLE * channel_nb_ui_;
//		wfx_.wBitsPerSample = SOUND_BIT_PER_SAMPLE;
//		wfx_.cbSize = 0;
//
//		st_buffer_desc.dwSize = sizeof(DSCBUFFERDESC);
//		st_buffer_desc.dwFlags = NULL;
//		st_buffer_desc.dwBufferBytes = SOUND_BYTE_PER_CAPTURE * SOUND_BUFFER_NB;
//		st_buffer_desc.dwReserved = NULL;
//		st_buffer_desc.lpwfxFormat = &wfx_;
//		st_buffer_desc.dwFXCount = 0;
//		st_buffer_desc.lpDSCFXDesc = NULL;
//		u32_return = p_DSCapture_->CreateCaptureBuffer( &st_buffer_desc, &lpDSBuffer_, NULL );
//		if( u32_return != DS_OK)
//			{
//			ds_err_show( u32_return, "in c_snd_input::enable() CreateCaptureBuffer()");
//		
//			b_enabled_ui_ = FALSE;
//			return FALSE;
//			}
//		channel_nb_ = channel_nb_ui_;
//		//the notification interface
//		u32_return = lpDSBuffer_->QueryInterface( IID_IDirectSoundNotify, (void**)&lpDsNotify_ );
//		if( u32_return != DS_OK)
//			{
//			ds_err_show( u32_return, "in c_snd_input::enable() QueryInterface(), DirectSound CaptureBuffer interface query failed");
//			return FALSE;
//			}
//			// create the events
//
//		for( i = 0; i < SOUND_BUFFER_NB; i++ )
//			{
//			if( ( h_NotifyEvent_[i] = CreateEvent( NULL, FALSE, FALSE, NULL ) ) == NULL )
//				{
//				ERR_PRINT_STRING( "event creation failed");
//				return FALSE;
//				}
//			rgdsbpn[i].dwOffset = SOUND_BYTE_PER_CAPTURE * ( i+ 1 )-1;
//			rgdsbpn[i].hEventNotify = h_NotifyEvent_[i];
//			}
//		if( ( h_NotifyEvent_[i] = CreateEvent( NULL, FALSE, FALSE, NULL ) ) == NULL )
//			{
//			ERR_PRINT_STRING( "event creation failed" );
//			return FALSE;
//			}
//		rgdsbpn[i].dwOffset = DSBPN_OFFSETSTOP;
//		rgdsbpn[i].hEventNotify = h_NotifyEvent_[i];
//		
//
//		// Now create the thread to wait on the events created.
//		u8_fft_done_ = 0;
//		b_buffer_read_ = FALSE;
//		b_sound_capture_thread_on_ = TRUE;
//
//		if ( ( h_notify_thread_ = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HandleNotifications, (LPVOID)this, 0, &dwThreadId_ ) ) == NULL)
//			{
//			ERR_PRINT_STRING( "thread creation failed");
//			return FALSE;
//			}
////		if( SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS) == 0)
////			ERR_PRINT_STRING( "can set priority Class");
////		if( SetThreadPriority( h_notify_thread, THREAD_PRIORITY_TIME_CRITICAL) == 0 )
////		if( SetThreadPriority( h_notify_thread, THREAD_PRIORITY_HIGHEST) == 0 )
//		if( SetThreadPriority( h_notify_thread_, THREAD_PRIORITY_ABOVE_NORMAL ) == 0 )
//			ERR_PRINT_STRING( "Can't set thread priority");
//		
//		u32_return = lpDsNotify_->SetNotificationPositions( SOUND_BUFFER_NB + 1, rgdsbpn );
//		if( u32_return != DS_OK )
//			{
//			ds_err_show( u32_return, "in c_snd_input::enable() SetNotificationPositions()" );
//			release_notification_events();
//			}
//
//		lpDSBuffer_->Start( DSCBSTART_LOOPING );
//		b_enabled_ = TRUE;
//		reset_fft();
//		}
//	return b_enabled_;
//}


#if	(USE_FFTW && USE_FFT_DENPO)
BOOL	b_use_fftw = TRUE;
#endif
void	c_snd_input::do_fft()
{
	if ( fft_count_ != ftt_capture_count_ )
		{
		fft_count_ = ftt_capture_count_;
		#if	(USE_FFTW && USE_FFT_DENPO)
			if ( b_use_fftw )
				fftw();
			else
				fft_denpo();
		#endif
		#if	(USE_FFTW && !USE_FFT_DENPO) 
			fftw();
		#endif
		#if	(USE_FFT_DENPO && !USE_FFTW) 
			fft_denpo();
		#endif
		//hack all that should be cleanup
		reset_fft();
		if( b_verbose_fft )
			VERBOSE_PRINTF( "%d", ftt_capture_count_ );
/*	if(!u8_fft_done)
	{
		if(b_wait)
			while(!u8_fft_done);
		else
			return FFT_NOTREADY;
	}
*/
		}
}

INT32 c_snd_input::get_spectrum_band( REAL start, REAL end, UINT32 band_nb, REAL* p_bands, BOOL b_right, BOOL b_linear, BOOL b_wait)
{
//UINT32	i;
REAL	f_sum;
REAL	delta;
REAL	low;
REAL	high;
REAL*	power_spectrum;

	if( b_right )
		power_spectrum = ps_r_.power_spectrum_;
	else
		power_spectrum = ps_l_.power_spectrum_;
	if( !b_enabled_ )
		{
		for( UINT32  i = 0; i < band_nb; i++ )
			p_bands[i] = 0.0;
		return FFT_NOTENABLED;
		}

//	do_fft();
	//normalize and swap
	start = CLAMP( start, REAL(0.), REAL(1.) );
	end = CLAMP( end, REAL(0.), REAL(1.) );
	if( start > end )
		SWAP( start, end );
	
	if( start == end )
		{
		for ( UINT32 i = 0; i < band_nb; i++)
			p_bands[i] = 0.;
		}
	else
		{
	//todo find the power_spectrum[0] bug
	/*	if ( band_nb == 1 && start == 0. && end == 1. )
			p_bands[0] = power_spectrum[0];
		else
	*/
			{	
			delta = ( end - start )/REAL( band_nb );	
			for ( UINT32 i = 0; i < band_nb; i++ )
				{
				low = start;
				high = start + delta;		
				if( b_linear )
					{
					low *= SOUND_SAMPLE_PER_CAPTURE >> 2;
					high *= SOUND_SAMPLE_PER_CAPTURE >> 2;
					f_sum = integrate_1d_real( power_spectrum + 1, low, high );
					}
				else
					{
					low = POW( SOUND_SAMPLE_PER_CAPTURE >> 2, low );
					high = POW( SOUND_SAMPLE_PER_CAPTURE >> 2, high );
					f_sum = integrate_1d_real( power_spectrum + 1, low, high );
					}
				p_bands[i] = f_sum / ( high - low );
				start += delta;
		//		f_sum = SQRT(f_sum);
				}
			}
		}

/*
UINT32 count;
UINT32 next_stop;
UINT32 cur;

	start *= SOUND_SAMPLE_PER_CAPTURE*.5;
	end *= SOUND_SAMPLE_PER_CAPTURE*.5;
	cur = UINT32(freq_cur);
	for ( i = 0; i < band_nb; i++)
		{
		freq_cur += freq_delta;
		next_stop = UINT32(freq_cur);
		count = 0;
		f_sum = 0.;
		do
			{
			f_sum += power_spectrum[cur];
			cur++;
			count++;
			}
		while( cur < next_stop);
		f_sum = SQRT(f_sum);
		p_bands[i] = f_sum/REAL(count);a
		cur = next_stop;
		}
*/
	return band_nb;
}


INT32 c_snd_input::get_spectrum_raw_band( REAL* pf_bands, BOOL b_right)
{
//INT32	i;
INT32	band_nb = spectrum_band_nb_get();

	if( !b_enabled_ )
		{
		for( INT32  i = 0; i < band_nb; i++ )
			pf_bands[i] = 0.0;
		return FFT_NOTENABLED;
		}
//	do_fft();

REAL*	power_spectrum;

	if( b_right)
		power_spectrum = ps_r_.power_spectrum_;
	else
		power_spectrum = ps_l_.power_spectrum_;

	for ( INT32 i = 0; i < band_nb; i++ )
		{
		pf_bands[i] = power_spectrum[i];
		if( power_spectrum[i] > 1.000001)
			{
			DBG_PRINT_STRING( "c_snd_input power_spectrum[%d] over 1 : %f", i, (REAL)power_spectrum[i] );
			}
		}
	return band_nb;
}

template<class T>
void	transfert_from_int( T* dst, INT32 dst_nb, void* src, INT32 src_nb, INT32 src_inc, INT32 bits_nb, T factor )
{
INT32	offset;
UINT32	nb = MIN( dst_nb, src_nb);

//	if( dst_nb == src_nb )
		{
		UINT32	i;
		UINT32	nb = src_nb;

		offset = 1<<(bits_nb-1);
		factor /= T(offset);
		--dst;
		switch( bits_nb)
			{
			case 8:
				{
				UINT8* p8 = (UINT8*)src;
				for (i = 0; i < nb ; i++)
					{
					*++dst = T(*p8) * factor;
					p8 += src_inc;
					}
				}
				break;	
			case 16:
				{
				INT16* p16 = (INT16*)src;
				for (i = 0; i < nb ; i++)
					{
					*++dst = T(*p16) * factor;
					p16 += src_inc;
					}
				}
				break;
			}
		}
/*	else if( IMOD( src_nb, dst_nb) == 0)
		{
		T	tmp;
		INT32	i;
		INT32	j;
		INT32	loop_nb;

		loop_nb = src_nb/dst_nb;
		offset = (1<<(bits_nb-1))*loop_nb;
		factor /= offset;
		switch( bits_nb)
			{
			case 8:
				{
				UINT8* p8 = (UINT8*)src;
				for (i = 0; i < dst_nb ; i++)
					{
					tmp = 0;
					for (j = 0; j < loop_nb ; j++)
						{
						tmp += T(*p8);
						p8 += src_inc;
						}
					*dst++ = tmp * factor;
					}
				}
				break;	
			case 16:
				{
				INT16* p16 = (INT16*)src;
				for (i = 0; i < dst_nb ; i++)
					{
					tmp = 0;
					for (j = 0; j < loop_nb ; j++)
						{
						tmp += T(*p16);
						p16 += src_inc;
						}
					*dst++ = tmp * factor;
					}
				}
				break;
			}
		}
	else
		{
		transfert_from_int( dst, nb, src, nb, src_inc, bits_nb, factor);
		//	DBG_PRINT_STRING( "transfert_int_to_real() case not implemented yet");
		}
*/
}

static	DOUBLE	samples[c_snd_input::SOUND_SAMPLE_PER_CAPTURE];

REAL	c_snd_input::get_area( INT32 channel_in, REAL th )
{
DOUBLE	sum;
DOUBLE*	pd;
INT32	nb;
DOUBLE	max;
DOUBLE	d;

	transfert_from_int( samples, SOUND_SAMPLE_PER_CAPTURE, sound_buffer_ + channel_in * 2, SOUND_SAMPLE_PER_CAPTURE, channel_nb_, SOUND_BIT_PER_SAMPLE, 1.);
	get_min_max_of_array( d, max, samples, SOUND_SAMPLE_PER_CAPTURE);
	max = MAX( ABS( d ), ABS( max ) );

	if( max != 0. && th != max )
		{
		th /= max;
		sum = 0;
		pd = samples - 1;
		nb = SOUND_SAMPLE_PER_CAPTURE + 1;
		while( --nb > 0 )
			{
			d = ABS( *++pd );
			d /= max;
			sum += MAX( d - th, 0.);
			}
		return sum /( DOUBLE( SOUND_SAMPLE_PER_CAPTURE * ( 1 - th ) ) );
		}
	return 0.;
}

INT32 c_snd_input::get_wave( REAL* dst, UINT32 nb, INT32 channel_in )
{
UINT32	i;
//	if( b_stereo_flip )
//		channel_in = (channel_in & 0xfffe) | (1-(channel_in & 1);
	if( freq_test_ != 0.)
		{
		REAL	factor = freq_test_ / nb;
		for (i = 0; i < nb ; i++)
			{
			*dst++ = SIN_INT( i * factor ) ;
			}
		}
	else
		{
//todonow		if( b_stereo_start_with && b_right )
			transfert_from_int( dst, nb, sound_buffer_ + channel_in * 2, SOUND_SAMPLE_PER_CAPTURE, channel_nb_, SOUND_BIT_PER_SAMPLE, REAL(1.));
//		else
//			transfert_from_int( dst, nb, sound_buffer, SOUND_SAMPLE_PER_CAPTURE, channel_nb, SOUND_BIT_PER_SAMPLE, 1.);
		}
	return nb;
}


#if	USE_FFTW
void c_snd_input::fill_fftw_array( UINT8* pu8_array_in, DOUBLE* pc_array_out, UINT32 u32_size )
{
UINT32	src;

	if( freq_test_ != 0.)
		{
		REAL factor = freq_test_ / SOUND_SAMPLE_PER_SEC;
		for( src = 0; src < u32_size ; src++ )
			{
			*pc_array_out++ = SIN_INT( src * factor ) ;
			}
		}
	else
		transfert_from_int( pc_array_out, u32_size, pu8_array_in, u32_size, channel_nb_, SOUND_BIT_PER_SAMPLE, 1.);
}

void c_snd_input::fftw()
{
INT32	nb = SOUND_SAMPLE_PER_CAPTURE/2;  //hack

	if( b_active)
		{
		fill_fftw_array( sound_buffer_ + ( b_stereo_flip_ ? 0 : SOUND_BYTE_PER_SAMPLE ), ps_l_.in_fftw_, nb );
		fill_fftw_array( sound_buffer_ + ( b_stereo_flip_ ? SOUND_BYTE_PER_SAMPLE : 0 ), ps_r_.in_fftw_, nb );

		ps_l_.fttw();
		ps_r_.fttw();
		}
	else
		{
		INT32	k;
		REAL*	ps;

		ps = ps_l_.power_spectrum_;

		for( k = 0; k < ( nb + 1 ) / 2; ++k )  /* (k < N/2 rounded up) */
			*ps++ = value_inactive_;
		if( nb % 2 == 0 ) // N is even
			*ps++ = value_inactive_;

		ps = ps_r_.power_spectrum_;

		for( k = 0; k < ( nb + 1 ) / 2; ++k )  /* (k < N/2 rounded up) */
			*ps++ = value_inactive_;
		if( nb % 2 == 0 ) // N is even
			*ps++ = value_inactive_;
		}
}
#endif

#if	USE_FFT_DENPO
static REAL CosArray[28] =
{ /* cos{-2pi/N} for N = 2, 4, 8, ... 16384 */
 -1.00000000000000f,  0.00000000000000f,  0.70710678118655f,
  0.92387953251129f,  0.98078528040323f,  0.99518472667220f,
  0.99879545620517f,  0.99969881869620f,  0.99992470183914f,
  0.99998117528260f,  0.99999529380958f,  0.99999882345170f,
  0.99999970586288f,  0.99999992646572f,
  /* cos{2pi/N} for N = 2, 4, 8, ... 16384 */
 -1.00000000000000f,  0.00000000000000f,  0.70710678118655f,
  0.92387953251129f,  0.98078528040323f,  0.99518472667220f,
  0.99879545620517f,  0.99969881869620f,  0.99992470183914f,
  0.99998117528260f,  0.99999529380958f,  0.99999882345170f,
  0.99999970586288f,  0.99999992646572f
};
static REAL SinArray[28] =
{ /* sin{-2pi/N} for N = 2, 4, 8, ... 16384 */
  0.00000000000000f, -1.00000000000000f, -0.70710678118655f,
 -0.38268343236509f, -0.19509032201613f, -0.09801714032956f,
 -0.04906767432742f, -0.02454122852291f, -0.01227153828572f,
 -0.00613588464915f, -0.00306795676297f, -0.00153398018628f,
 -0.00076699031874f, -0.00038349518757f,
  /* sin{2pi/N} for N = 2, 4, 8, ... 16384 */
  0.00000000000000f,  1.00000000000000f,  0.70710678118655f,
  0.38268343236509f,  0.19509032201613f,  0.09801714032956f,
  0.04906767432742f,  0.02454122852291f,  0.01227153828572f,
  0.00613588464915f,  0.00306795676297f,  0.00153398018628f,
  0.00076699031874f,  0.00038349518757f
};

void c_snd_input::reorder_fft_array( UINT8* pu8_array_in,
									C_COMPLEX* pc_array_out,
									UINT32 u32_size)
{	//	Shuffle all elements
UINT32	src;
C_COMPLEX*	p_complex = pc_array_out;
REAL	factor;
INT32	offset;
INT32	i;
UINT32*	p_order = reorder;

	if( freq_test != 0.)
		{
		factor = freq_test / SOUND_SAMPLE_PER_SEC;
		for (src = 0; src < u32_size ; src++)
			{
			p_complex->f_real = SIN_INT( *p_order++ * factor);
			p_complex->f_img = 0.0f;
			p_complex++;
			}
		}
	else
		{
		offset = 1<<(SOUND_BIT_PER_SAMPLE-1);
		factor = 1./offset;
		switch(SOUND_BIT_PER_SAMPLE)
			{
			case 8:
				//set the buffers
				for (src = 0; src < u32_size ; src++)
					{
					i = pu8_array_in[*p_order++];
					p_complex->f_real = REAL(i-offset) * factor;
					p_complex->f_img = 0.0f;
					p_complex++;
					}
				break;
		
			case 16:
				INT16* pi16_array_in = (INT16*)pu8_array_in;
				//set the buffers
				for (src = 0; src < u32_size ; src++)
					{
					i = pi16_array_in[*p_order++];                             
					p_complex->f_real = REAL(i-offset) * factor;
					p_complex->f_img = 0.0f;
					p_complex++;
					}
			break;
			}
		}
}

/*
UINT32 c_snd_input::shuffle(UINT32 u32_index_in,UINT32 u32_length)
{
	__asm
	{
		mov	ebx,u32_index_in
		xor eax,eax
		mov	ecx,u32_length
shffl:	rcr	bx,1
		rcl ax,1
		loop shffl
	}
}
*/

/*
void C_WHAAA::reorder_fft_array(UINT8* pu8_array_in,UINT8* pu8_array_out,UINT32 u32_size)
{
	UINT16* pu16_temp;
	pu16_temp = pu16_fft_shuffle;

	_asm
	{
		push esi
		push edi

		
		xor eax,eax
		mov	ebx,pu16_array
		mov	ecx,u32_size
		xor edx,edx
		mov esi,pu8_array_in
		mov edi,pu8_array_out

reordr: mov ax,[ebx]
		mov dl,[esi+eax]
		add ebx,2
		ror edx,8

		mov ax,[ebx]
		mov dl,[esi+eax]
		add ebx,2
		ror edx,8

		mov ax,[ebx]
		mov dl,[esi+eax]
		add ebx,2
		ror edx,8

		mov ax,[ebx]
		mov dl,[esi+eax]
		add ebx,2
		ror edx,8

		mov	[edi],edx
		add edi,4
		sub ecx,4
		jnz reordr

		pop edi
		pop esi
	}
}
*/

void c_snd_input::fft_denpo()
{
	UINT32 u32_angle;
	UINT32 u32_section;
	UINT32 u32_offset;
	UINT32 u32_pos1;
	UINT32 u32_pos2;
	UINT32 u32_offset_count;
	FLOAT f_cos;
	FLOAT f_sin;
	FLOAT f_real;
	FLOAT f_temp;
	FLOAT f_img;
	FLOAT f_temp_real;
	FLOAT f_temp_img;

	
	//fill the real part with the incoming signal
	reorder_fft_array(	sound_buffer,
						pc_fft,
						SOUND_SAMPLE_PER_CAPTURE);
	//fill the img part with 0

	u32_angle = 0;
	u32_section = 1;

	while (u32_section < SOUND_SAMPLE_PER_CAPTURE)
	{
		u32_offset = u32_section << 1;
		f_cos = CosArray[u32_angle];
		f_sin = SinArray[u32_angle];
		f_real = 1.0f;
		f_img = 0.0f;
		for (u32_offset_count = 0; u32_offset_count < u32_section; u32_offset_count++)
		{
			u32_pos1 = u32_offset_count;
			do
			{
				u32_pos2 = u32_pos1 + u32_section;
											// Perform 2-Point DFT 
				f_temp_real =	1.0 * f_real * pc_fft[u32_pos2].f_real - 
								1.0 * f_img * pc_fft[u32_pos2].f_img;
				f_temp_img =	1.0 * f_real * pc_fft[u32_pos2].f_img + 
								1.0 * f_img * pc_fft[u32_pos2].f_real;

				pc_fft[u32_pos2].f_real = pc_fft[u32_pos1].f_real - f_temp_real;   // For pf_real-part 
				pc_fft[u32_pos2].f_img = pc_fft[u32_pos1].f_img - f_temp_img;      //For pf_img-part 
				pc_fft[u32_pos1].f_real = pc_fft[u32_pos1].f_real + f_temp_real;
				pc_fft[u32_pos1].f_img = pc_fft[u32_pos1].f_img + f_temp_img;

				u32_pos1 = u32_pos1 + u32_offset;
			} while (u32_pos1 < SOUND_SAMPLE_PER_CAPTURE );

			//                 k                                  
			//  Calculate new Q = cos(ak) + j*sin(ak) = Qr + j*Qi 
			//          -2*pi                                     
			//  with: a = -----                                   
			//            N                                       
			f_temp = f_real;
			f_real = f_real * f_cos - f_img * f_sin;
			f_img = f_img * f_cos + f_temp * f_sin;
		}
		u32_section <<= 1;
		u32_angle ++;
	}
//	pu16_freq_out[0] = UINT16(	pf_real[0] * pf_real[0] + pf_img[0] * pf_img[0]);
FLOAT*	pf;
	pf = power_spectrum;
	for(u32_pos1 = 0; u32_pos1 < (SOUND_SAMPLE_PER_CAPTURE/2+1); u32_pos1 ++)
		{
		f_real = pc_fft[u32_pos1].f_real;
		f_img = pc_fft[u32_pos1].f_img;
		*pf++ = f_real * f_real + f_img * f_img;
		}
}

/*
void c_snd_input::fft()
{
	UINT32 u32_angle;
	UINT32 u32_section;
	UINT32 u32_offset;
	UINT32 u32_pos1;
	UINT32 u32_pos2;
	UINT32 u32_offset_count;
	FLOAT f_cos;
	FLOAT f_sin;
	FLOAT f_real;
	FLOAT f_temp;
	FLOAT f_img;
	FLOAT f_temp_real;
	FLOAT f_temp_img;
	C_COMPLEX	*p1;
	C_COMPLEX	*p2;
	
	//fill the real part with the incoming signal
	reorder_fft_array(	sound_buffer,
						pc_fft,
						SOUND_SAMPLE_PER_CAPTURE,
						SOUND_CAPTURE_SIZE_IN_BIT);
	//fill the img part with 0

	u32_angle = 0;
	u32_section = 1;

	while (u32_section < SOUND_SAMPLE_PER_CAPTURE)
		{
		u32_offset = u32_section << 1;
		f_cos = CosArray[u32_angle];
		f_sin = SinArray[u32_angle];
		f_real = 1.0f;
		f_img = 0.0f;
		for (u32_offset_count = 0; u32_offset_count < u32_section; u32_offset_count++)
			{
			u32_pos1 = u32_offset_count;
			do
				{
				u32_pos2 = u32_pos1 + u32_section;
				p1 = &pc_fft[u32_pos1];
				p2 = &pc_fft[u32_pos2];
											// Perform 2-Point DFT 
				f_temp_real =	f_real * p2->f_real - f_img * p2->f_img;
				f_temp_img =	f_real * p2->f_img + f_img * p2->f_real;

				p2->f_real	= p1->f_real - f_temp_real;             // For pf_real-part 
				p2->f_img	= p1->f_img - f_temp_img;             //For pf_img-part 
				p1->f_real	+= f_temp_real;
				p1->f_img	+= f_temp_img;

				u32_pos1 += u32_offset;
				}
			while (u32_pos1 < SOUND_SAMPLE_PER_CAPTURE );

			//                 k                                  
			//  Calculate new Q = cos(ak) + j*sin(ak) = Qr + j*Qi 
			//          -2*pi                                     
			//  with: a = -----                                   
			//            N                                       
			f_temp = f_real;
			f_real = f_real * f_cos - f_img * f_sin;
			f_img = f_img * f_cos + f_temp * f_sin;
			}
		u32_section <<= 1;
		u32_angle ++;
		}
//	pu16_freq_out[0] = UINT16(	pf_real[0] * pf_real[0] + pf_img[0] * pf_img[0]);
FLOAT*	pf;
	pf = power_spectrum;
	for(u32_pos1 = 0; u32_pos1 < (SOUND_SAMPLE_PER_CAPTURE/2+1); u32_pos1 ++)
		{
		f_real = pc_fft[u32_pos1].f_real;
		f_img = pc_fft[u32_pos1].f_img;
		*pf++ = f_real * f_real + f_img * f_img;
		}
	u8_fft_done = 1;
	printf( "fft %d\n", ftt_capture_count);

}
*/
#endif	//USE_FFT_DENPO