#include "obj_ui/bdd/bdd_sound/bdd_snd_synth_spectrum.h"
#include "time/aaa_time.h"
#include "math/gainbias.h"
#include "image/bind_img_2d.h"
#ifndef AAA_WRAP_BASS_H
#	include "lib_wrappers/wrap_bass.h"
#endif
#ifndef AAA_SND_INPUT_H
#	include "media/sound/snd_input.h"
#endif
#include "media/sound/snd_master.h"

extern	c_img_2d*	img_for_sound;

FACTORY_CREATE_PROP_V1( c_bdd_snd_synth_spectrum, bdd_snd_synth_spectrum, Sound Spectrum Synthezier, bdd_snd_synth_spectrum, sub_menu="Sound"; sel0="Sound Synth Spectrum"; );

DWORD CALLBACK write_stream_callback(HSTREAM handle, void *buffer_in, DWORD length, void* user)
{
	c_bdd_snd_synth_spectrum* obj = (c_bdd_snd_synth_spectrum*)user;

	std::lock_guard<c_bdd_snd_synth_spectrum> guard(*obj);
	return obj ? obj->write_stream( buffer_in, length) : 0;
}
/*
int _matherr( struct _exception *except )
{
	// Handle _DOMAIN errors for log or log10. 
	if( except->type == _DOMAIN )
	{
		if( strcmp( except->name, "log" ) == 0 )
		{
			except->retval = log( -(except->arg1) );
			printf( "Special: using absolute value: %s: _DOMAIN "
					 "error\n", except->name );
			return 1;
		}
		else if( strcmp( except->name, "log10" ) == 0 )
		{
			except->retval = log10( -(except->arg1) );
			printf( "Special: using absolute value: %s: _DOMAIN "
					 "error\n", except->name );
			return 1;
		}
	}
	printf( "Normal: " );
	return 0;    // Else use the default actions
}
*/


FINLINE	void dchk( float d )
{
	if( d < -1000000 || d>1000000 )
		debug_break();
return;
/*
UINT8* pt;
	pt = (UINT8*)d;
	if( (*(pt+6)==0xf8) && (*(pt+7)==0xff) )
		debug_break();
	if( (*(pt+6)==0xf0) && (*(pt+7)==0xf7) )
		debug_break();
	if (  ( *(pt+7)==0xcd )
		&& ( *(pt+6)==0xcd )
		&& ( *(pt+5)==0xcd )
		&& ( *(pt+4)==0xcd )
		&& ( *(pt+3)==0xcd )
		&& ( *(pt+2)==0xcd )
		&& ( *(pt+1)==0xcd )
		&& ( *(pt+0)==0xcd )
	   )
		debug_break();
*/
}

void c_bdd_snd_synth_spectrum::prepare_band()
{
	REAL	vol_avg;
	REAL	vol;
	REAL	f;

	vol_avg = volume_/REAL(band_nb_);
	//loop band
	for( INT32 b = 0; b < band_nb_; ++b )
	{
		//VOLUME
		vol = band_[b];
		if( vol < 0.)
			vol = -vol;
		//		vol = exp((vol-1)*vol_exp);
		if( vol != 0. && vol_power_!=0. )
			vol = exp( vol/vol_power_ );
		vol_[b] = CLAMP( vol * vol_avg, 0., 1. );
		//dchk(&vol);

		//FREQ
		if( band_nb_ <= 1)
			f = .5;
		else
			f = REAL(b)/REAL(band_nb_-1);
		bias( f, freq_bias_ );
		f = interpolate( freq_start_, freq_stop_, f );
		//f = pow( f, DOUBLE(freq_power));
		//	in fact exp( 3*ln10*f+ln20) : 0->20, 1->20000, 1/3->200, 2/3->2000
		//	checked
		f = REAL( exp( 6.907755278982137520539743640531 * f + 2.9957322735539909934352235761425) );
		if( f > REAL(40000) )	
		{
			f = REAL(40000);	// dph can't be more than 1.
			vol_[b] = REAL(0);
		}
		else if( f < REAL(0))
		{
			f = REAL(0);	// dph can't be less than 0.
			vol_[b] = REAL(0);
		}
		dph_[b] = f/REAL(44100);	//todo deal with 44100 in fix
		//		if (!vol[n])
		//			continue;
		//		f = pow(2.0,(n+3)/12.0)*TABLESIZE*440.0/44100.0;	//todo deal with 44100 in fix
		//dchk(&f);

	}
}


INT32 c_bdd_snd_synth_spectrum::write_stream( void* buffer_in, INT32 length)
{
	INT32	sample_nb = length / (sizeof(float)*2) ;
	if( sample_nb==0 )
	{
		debug_break();
		return 0;
	}

	float*	pt;
	if( volume_ <= 0. )
	{
		if( volume_ == 0. )
		{
		pt = ((float*)buffer_in) - 1;
		for( INT32 i = sample_nb*2; i >0; --i )
			*++pt = 0.;
		}
		return length;
	}

	//clear one mono buffer
	pt = (float*)buffer_in; 
	for( INT32 i = sample_nb; i >0; --i )
	{
		*pt = 0.;
		pt += 2;
	}

	bool	b_use_gain_bias;
	REAL	gain_f;
	REAL	bias_f;
	b_use_gain_bias = (shape_gain_!=.5) || (shape_bias_!=.5);
	if( b_use_gain_bias )
	{
		gain_f = gain_factor( shape_gain_ );
		bias_f = bias_factor( shape_bias_ );
	}

	REAL	vol_avg;
	REAL	vol;
	REAL	dvol;
	REAL	ph; 
	REAL	dph;
	REAL	over_sample_nb = 1/REAL(sample_nb);

	vol_avg = volume_/REAL(band_nb_);
//loop band
	for( INT32 b = 0; b < band_nb_; ++b )
	{
		dph = dph_[b];
	//		if (!vol[n])
	//			continue;
	//		f = pow(2.0,(n+3)/12.0)*TABLESIZE*440.0/44100.0;	//todo deal with 44100 in fix
//dchk(&f);

//dchk(&vol);
		// the change of volume is done in samp_nb 
		dvol = (vol_[b]-vol_last_[b]) * over_sample_nb;
//dchk(&dvol);
		vol = vol_last_[b];
		
		if( vol!=0. || dvol!=0. )
		{	//we even forget about the phase in this case	
			//PHASE
			ph = phase_[b];
			pt = (float*)buffer_in;
			if( b_use_gain_bias )
				for ( INT32 i = sample_nb; i > 0; --i )
				{
					REAL	s;
					vol += dvol;
					dchk(vol);
					ph += dph;
					if( ph > 1. )
						ph -= 1.;
					if( ph < 0. || ph > 1. )
						debug_break();
					s = c_math::sin_table_direct_get( ph ) ;
					s = s * REAL(.5) + REAL(.5);
					gain_with_factor( s, gain_f );
					bias_with_factor( s, bias_f );
					s = s * REAL(2) - REAL(1);
					*pt += vol * s ;
					pt += 2;
				}
			else
				for ( INT32 i = sample_nb; i > 0; --i )
				{
				REAL	s;
					vol += dvol;
					dchk(vol);
					ph += dph;
					if( ph >= 1. )
						ph -= 1.;
					if( ph < 0. || ph > 1. )
						debug_break();
					s = c_math::sin_table_direct_get( ph ) ;
					*pt += vol * s ;
					//*pt += vol * SIN_INT( ph ) ;
					pt += 2;
				}
//dchk(&vol);
//dchk(&ph);
			phase_[b] = ph;	//maa was there : - floor(ph);
			dchk(phase_[b]);
			vol_last_[b] = vol;
		}
	}

	min_ = 1.;
	max_ = -1.;
	pt = ((float*)buffer_in) - 1;
	for( INT32 i = sample_nb; i>0; --i )
	{
		REAL	tmp;
		++pt;
		tmp =  CLAMP( *pt, -1., 1. );
		min_ = MIN( min_, tmp);
		max_ = MAX( max_, tmp);
		*pt = tmp;
		*++pt = tmp;
	}
	return length;
}

//		img_for_sound->lock();

void c_bdd_snd_synth_spectrum::img_read_band( c_img_2d* img_src )
{
	if( img_src && band_nb_>=1 )
	{
		REAL*	pt;
		REAL	u;
		REAL	v;
		REAL	dv;
		//INT32	i;
//		if( img_src->get_size_x() == 0)	//used to avoid crashes from multitasking
//			debug_break();
		
//		img_src->lock();

		u = FMOD( REAL(aaa::time::get_real_time() * freq_u_) );
		
		if(	band_nb_==1 )
			v = .5;
		else
		{
			v = 0.;
			dv = OVER_ONE_AS_REAL(band_nb_-1);
		}

		pt = band_-1;
		for( INT32 i = band_nb_; i > 0; --i )
		{
			REAL f = img_src->get_value_from_uv( u, v, false, _s_component, false );
//			dchk(&f);
			if( level_min_==level_max_ )
				f = (f==level_min_)? REAL(1) : REAL(0) ;
			else if( OUTSIDE( REAL(f), level_min_, level_max_) )
				f = REAL(0);
			else
				f = (f-level_min_)/(level_max_-level_min_);
			*++pt = f;
			v += dv;
		}
//		img_src->unlock();
	}
}

namespace n_bdd_snd_synth_spectrum
{
	CONSTEXPR INT32 BASE_NB_MAX		=	c_bdd::GEO_PARAM_NB + 16;
	CONSTEXPR INT32 IMAGE_NB_MAX	=	4;
	CONSTEXPR INT32 BAND_NB_MAX		=	32;
	CONSTEXPR INT32 GROUP_NB_MAX	=	2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	IMAGE_NB_MAX
									+	BAND_NB_MAX
									+	GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(	active )
		PARAM_DEF_BOOL_OFF(	start_trig )
		PARAM_DEF_BOOL_OFF(	stop_trig )
		PARAM_DEF_BOOL_OFF(	init_trig )
		PARAM_DEF_INT32(	band_nb, 16, 1, 1, FFT_SYNTH_BAND_MAX )
		PARAM_DEF_BOOL_OFF(	use_image )

		PARAM_DEF_GROUP( Image, IMAGE_NB_MAX )
			PARAM_DEF_SYMBO_PSTR(	component,	1, INT32(aaa::COMPO::RED),	aaa::str_compo )
			PARAM_DEF_REAL_ZERO(	freq_u		)
			PARAM_DEF_REAL_ZERO(	level_min	)
			PARAM_DEF_REAL_ONE(		level_max	)

		PARAM_DEF_REAL_ZERO(		volume )
		PARAM_DEF_REAL_POS_ONE(		volume_power )

		PARAM_DEF_REAL_ZERO(		freq_start	)
		PARAM_DEF_REAL_ONE(			freq_stop	)
		PARAM_DEF_BIAS(				freq_bias	)
		PARAM_DEF_REAL_POS_ONE(		freq_power	)

		PARAM_DEF_GAIN(				shape_gain	)
		PARAM_DEF_BIAS(				shape_bias	)

		PARAM_DEF_REAL_LOCKED(		signal_min )
		PARAM_DEF_REAL_LOCKED(		signal_max )

		PARAM_DEF_GROUP( Bands, BAND_NB_MAX )	//archi this is a generic problem (like Midi controller
			PARAM_DEF_REAL_ZERO( band_01 )
			PARAM_DEF_REAL_ZERO( band_02 )
			PARAM_DEF_REAL_ZERO( band_03 )
			PARAM_DEF_REAL_ZERO( band_04 )
			PARAM_DEF_REAL_ZERO( band_05 )
			PARAM_DEF_REAL_ZERO( band_06 )
			PARAM_DEF_REAL_ZERO( band_07 )
			PARAM_DEF_REAL_ZERO( band_08 )
			PARAM_DEF_REAL_ZERO( band_09 )
			PARAM_DEF_REAL_ZERO( band_10 )
			PARAM_DEF_REAL_ZERO( band_11 )
			PARAM_DEF_REAL_ZERO( band_12 )
			PARAM_DEF_REAL_ZERO( band_13 )
			PARAM_DEF_REAL_ZERO( band_14 )
			PARAM_DEF_REAL_ZERO( band_15 )
			PARAM_DEF_REAL_ZERO( band_16 )
			PARAM_DEF_REAL_ZERO( band_17 )
			PARAM_DEF_REAL_ZERO( band_18 )
			PARAM_DEF_REAL_ZERO( band_19 )
			PARAM_DEF_REAL_ZERO( band_20 )
			PARAM_DEF_REAL_ZERO( band_21 )
			PARAM_DEF_REAL_ZERO( band_22 )
			PARAM_DEF_REAL_ZERO( band_23 )
			PARAM_DEF_REAL_ZERO( band_24 )
			PARAM_DEF_REAL_ZERO( band_25 )
			PARAM_DEF_REAL_ZERO( band_26 )
			PARAM_DEF_REAL_ZERO( band_27 )
			PARAM_DEF_REAL_ZERO( band_28 )
			PARAM_DEF_REAL_ZERO( band_29 )
			PARAM_DEF_REAL_ZERO( band_30 )
			PARAM_DEF_REAL_ZERO( band_31 )
			PARAM_DEF_REAL_ZERO( band_32 )
	};
}


CONSTRUCTOR_CREATE(c_bdd_snd_synth_spectrum)
{
	param_init_with( n_bdd_snd_synth_spectrum::param, n_bdd_snd_synth_spectrum::PARAM_NB_MAX);
	init();
}

c_bdd_snd_synth_spectrum::~c_bdd_snd_synth_spectrum()
{
	synth_stop();
//	dealloc();
}

void	c_bdd_snd_synth_spectrum::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _b_start_ui		);
	param_set_pt( h, _b_trig_start_ui	);
	param_set_pt( h, _b_trig_stop_ui	);
	param_set_pt( h, _b_trig_init_ui	);

	param_set_pt( h, band_nb_			);
	param_set_pt( h, _b_use_img_ui		);

	++h;
		param_set_pt( h, _s_component		);
		param_set_pt( h, freq_u_			);
		param_set_pt( h, level_min_			);
		param_set_pt( h, level_max_			);

	param_set_pt( h, volume_			);
	param_set_pt( h, vol_power_			);

	param_set_pt( h, freq_start_		);
	param_set_pt( h, freq_stop_			);
	param_set_pt( h, freq_bias_			);
	param_set_pt( h, freq_power_		);

	param_set_pt( h, shape_gain_		);
	param_set_pt( h, shape_bias_		);

	param_set_pt( h, min_				);
	param_set_pt( h, max_				);

	++h;
	param_set_pt_n( h, band_,  n_bdd_snd_synth_spectrum::BAND_NB_MAX);

	err_param_init_pt(h);
}


void c_bdd_snd_synth_spectrum::init()
{
	_b_start =	false;
	min_ = max_ = 0.;
	synth_init();
}

void c_bdd_snd_synth_spectrum::alloc()
{
//	averager = new 	c_averager[BDD_SOUND_AVERAGER_MAX_NB];
}

void c_bdd_snd_synth_spectrum::dealloc()
{
//	FREE( averager);
//	averager = nullptr;
}


void	c_bdd_snd_synth_spectrum::update()
{
	if( dll_bass.is_loaded() )
	{
		std::lock_guard<c_bdd_snd_synth_spectrum> guard(*this);

		if( _b_trig_start_ui )
		{
			synth_start();
			_b_trig_start_ui = false;
			_b_start_ui = _b_start;
		}
		if( _b_trig_stop_ui )
		{
			synth_stop();
			_b_trig_stop_ui = false;
			_b_start_ui = _b_start;
		}

		if( _b_trig_init_ui )
		{
			synth_init();
			_b_trig_init_ui = false;
		}
		if( _b_start_ui != _b_start )
		{
			if( _b_start_ui)
				synth_start();
			else
				synth_stop();
		}
		if( _b_start )
		{
			//	lock();
			img_for_sound = g_bind_img_2d->get_always( 40 );
			if( volume_ != 0. && _b_use_img_ui )
				img_read_band( img_for_sound );
			prepare_band();
			//	unlock();
		}
	}
}

void	c_bdd_snd_synth_spectrum::draw_single()
{
}

void	c_bdd_snd_synth_spectrum::synth_start()
{
	if( !_b_start)
	{
		// TODO check if BASS is loaded...

		// create a stream, stereo so that effects sound nice
		_bass_stream = dll_bass.BASS_StreamCreate( snd::SAMPLE_PER_SEC_DEF, 2, BASS_SAMPLE_FLOAT, ::write_stream_callback, this );	
		if( _bass_stream )
		{
			dll_bass.BASS_ChannelPlay( _bass_stream, FALSE);
			_b_start = true;
		}
		else
			ERR_PRINT_STRING( "Can't open Bass Channel" );
	}
}

void	c_bdd_snd_synth_spectrum::synth_stop()
{
	if( _b_start)
	{
		if( _bass_stream)
		{
			dll_bass.BASS_ChannelStop( _bass_stream );
			dll_bass.BASS_StreamFree( _bass_stream );
		}
		_b_start = false;
	}
}

void	c_bdd_snd_synth_spectrum::synth_init()
{
	for( INT32 i=0; i<FFT_SYNTH_BAND_MAX; ++i )
	{
		phase_[i] = 0.;
		vol_last_[i] = 0.;
		band_[i] = 0.;
		dph_[i] = 0.;
		vol_[i] = 0.;
		
	}
}

void	c_bdd_snd_synth_spectrum::lock()
{
	_lock.lock();
}

void	c_bdd_snd_synth_spectrum::unlock()
{
	_lock.unlock();
}
