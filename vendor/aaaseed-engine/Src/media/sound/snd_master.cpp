#include "snd_master.h"
#include "infrastructure/param/param_declare.h"
#include "obj_ui/tracker/trackers.h"

#ifndef _MMEAPI_H_
#	ifndef AAA_OS_H
#		include "aaa_os.h"
#	endif
#	include "mmeapi.h"
#endif

#include "sound_directsound.h"
#include "sound_bass.h"
#include "sound_bass_asio.h"
#include "snd_input.h"

#include "buffer/buffer_blk.h"

namespace snd {
C_PCHAR_C sample_rate_str[SAMPLE_RATE_MAX_NB] =
{
	"11025 Hz",
	"22050 Hz",
	"44.1 kHz",
	"48 kHz",
	"96 kHz",
	"192 kHz",
	"Unknown",
};

C_PCHAR_C sample_bits_str[SAMPLE_BITS_MAX_NB] =
{
	"8 bits",
	"16 bits",
	"24 bits",
	"32 bits",
	"32 bits fp",
	"Unknown",
};

C_PCHAR_C lib_name_str[ LIB_NB_MAX ] =
{
	"No",
	"DirectSound",
	"Bass",
	"Bass Asio",
};

SAMPLE_RATE	sample_rate( INT32 sample_rate )
{
	SAMPLE_RATE	ret;
	switch( sample_rate )
	{
	case 11025 :	ret = SAMPLE_RATE_11025;	break;
	case 22050 :	ret = SAMPLE_RATE_22050;	break;
	case 44100 :	ret = SAMPLE_RATE_44100;	break;
	case 48000 :	ret = SAMPLE_RATE_48000;	break;
	case 96000 :	ret = SAMPLE_RATE_96000;	break;
	case 192000 :	ret = SAMPLE_RATE_192000;	break;
	default :		ret = SAMPLE_RATE_UNKNOWN;	break;
	}
	return ret;
}

SAMPLE_BITS	sample_bits( INT32 sample_bits )
{
	SAMPLE_BITS	ret;
	switch( sample_bits )
	{
		case 8 :	ret = SAMPLE_BITS_8;		break;
		case 16 :	ret = SAMPLE_BITS_16;		break;
		case 24 :	ret = SAMPLE_BITS_24;		break;
		case 32 :	ret = SAMPLE_BITS_32;		break;
		default :	ret = SAMPLE_BITS_UNKNOWN;	break;
	}
	return ret;
}

C_PCHAR_C	header[ snd::LIB_NB_MAX ] =
{
	"# SOUND ",
	"# DIRECT SOUND ",
	"# BASS ",
	"# BASS ASIO ",
};
C_PCHAR_C	header_err[ snd::LIB_NB_MAX ] =
{
	" SOUND ",
	" DIRECT SOUND ",
	" BASS ",
	" BASS ASIO ",
};
	
void PRINT( C_PCHAR fmt, ... )
{									
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( header[g_master->_s_lib_used], fmt, args );
	va_end(args);
}
	
void ERR_PRINT( C_PCHAR fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	ERR_HEADER_PRINT_STRING_VA( header_err[g_master->_s_lib_used], fmt, args );
	va_end(args);
}
	
void print_device( INT32 id, C_PCHAR_C name, C_PCHAR_C driver )
{
	PRINT( "device %d", id );
	PRINT( "\t %s", name );
	PRINT( "\t %s", driver );
}

void print_wave_modes( DWORD mask )
{
	if( mask & WAVE_FORMAT_1M08 )	snd::PRINT( "\t11.025 kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_1M16 )	snd::PRINT( "\t11.025 kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_1S08 )	snd::PRINT( "\t11.025 kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_1S16 )	snd::PRINT( "\t11.025 kHz, stereo, 16 bit" );

	if( mask & WAVE_FORMAT_2M08 )	snd::PRINT( "\t22.05  kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_2M16 )	snd::PRINT( "\t22.05  kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_2S08 )	snd::PRINT( "\t22.05  kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_2S16 )	snd::PRINT( "\t22.05  kHz, stereo, 16 bit" );

	if( mask & WAVE_FORMAT_44M08 )	snd::PRINT( "\t44.1   kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_44S08 )	snd::PRINT( "\t44.1   kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_44M16 )	snd::PRINT( "\t44.1   kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_44S16 )	snd::PRINT( "\t44.1   kHz, stereo, 16 bit" );

	if( mask & WAVE_FORMAT_48M08 )	snd::PRINT( "\t48     kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_48S08 )	snd::PRINT( "\t48     kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_48M16 )	snd::PRINT( "\t48     kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_48S16 )	snd::PRINT( "\t48     kHz, stereo, 16 bit" );

	if( mask & WAVE_FORMAT_96M08 )	snd::PRINT( "\t96     kHz, mono,   8  bit" );
	if( mask & WAVE_FORMAT_96S08 )	snd::PRINT( "\t96     kHz, stereo, 8  bit" );
	if( mask & WAVE_FORMAT_96M16 )	snd::PRINT( "\t96     kHz, mono,   16 bit" );
	if( mask & WAVE_FORMAT_96S16 )	snd::PRINT( "\t96     kHz, stereo, 16 bit" );	
}

FACTORY_CREATE_V1( c_snd_master, snd_master, Sound Master, snd_master );

namespace	n_snd_master
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 11;
	CONSTEXPR INT32	GROUP_NB		= 0;

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(				active					)
		PARAM_DEF_SYMBO_PSTR_ZERO(		lib_asked,				snd::lib_name_str	)
		PARAM_DEF_SYMBO_LOCKED_PSTR(	lib_used,				snd::lib_name_str	)
		PARAM_DEF_BOOL_OFF(				enum_trig				)
//		PARAM_DEF_INT32(				sound_input_nb_asked,	1, 0,				0, c_snd_input::SOUND_INPUT_NB		)
//		PARAM_DEF_INT32_LOCKED(			sound_input_nb																	)
		PARAM_DEF_REAL_POS_ONE(			volume					)
		PARAM_DEF_REAL(					pan,					0.5, 0.,			-1., 1.	)
		PARAM_DEF_INT32_LOCKED(			blk_in					)
		PARAM_DEF_INT32_LOCKED(			blk_out					)
		PARAM_DEF_NONE(					direct_sound			)
		PARAM_DEF_NONE(					bass					)
		PARAM_DEF_NONE(					bass_asio				)
	};
}

void	c_snd_master::param_init_pt()
{
	INT32	h = 0;

	param_set_pt(		h,	get_pt_active()		);
	param_set_pt(		h,	_s_lib_asked_ui		);
	param_set_pt(		h,	_s_lib_used			);
	param_set_pt(		h,	_b_enum_trig		);
	param_set_pt(		h,	_volume				);
	param_set_pt(		h,	_pan				);
	param_set_pt(		h,	c_buffer_stream::get_count_in_pt() );
	param_set_pt(		h,	c_buffer_stream::get_count_out_pt() );
	param_attach_obj(	h,	_lib_ds				);
	param_attach_obj(	h,	_lib_bass			);
	param_attach_obj(	h,	_lib_bass_asio		);
	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_snd_master)
	,_lib				{nullptr}
	,_lib_ds			{nullptr}
	,_lib_bass			{nullptr}
	,_lib_bass_asio		{nullptr}
	,_s_lib_asked_ui	{snd::LIB_DS}
	,_s_lib_used		{0}
	,_sound_input		{nullptr}
{
	c_snd_input::c_init();
	param_init_with( n_snd_master::param, n_snd_master::PARAM_NB_MAX );
}

c_snd_master::~c_snd_master()
{
	if( _lib )
		_lib->delete_input( _sound_input );

	SAFE_DELETE( _lib_ds );
	SAFE_DELETE( _lib_bass );
	SAFE_DELETE( _lib_bass_asio );
	c_sound_bass::close_lib();
	c_sound_bass_asio::close_lib();

	c_snd_input::c_deinit();
}

C_PCHAR_C	c_snd_master::get_lib_name()		{ return lib_name_str[_s_lib_used]; }

c_sound_input*	c_snd_master::get_device_input()
{
	auto lib = get_lib();
	if( !lib )
		return	nullptr;
	if( !_sound_input )	
		_sound_input = lib->add_input();
	return _sound_input;
}

void	c_snd_master::update()
{
	if( !is_active() )
		return;

	auto lib = get_lib();
	if( lib )
	{
		if( _b_enum_trig )
		{
			lib->enumerate();
			_b_enum_trig = false;
		}
		lib->update();
	}
}

c_sound*	c_snd_master::get_lib()
{
	if( !_lib || _s_lib_used != _s_lib_asked_ui )
	{
		if( _lib )
		{
			_lib->dealloc_snd_input();
			_lib->close();
		}
		c_sound* lib = nullptr;
		INT32 lib_asked = _s_lib_asked_ui;
		switch( lib_asked )
		{
#ifdef WIN32
		case snd::LIB_DS:
			if( !_lib_ds )
				_lib_ds = new c_sound_directsound;
			lib = _lib_ds;
			break;
		case snd::LIB_BASS:
			if( c_sound_bass::open_lib() )
			{
				if( !_lib_bass )
					_lib_bass = new c_sound_bass;
				lib = _lib_bass;
			}
			break;
		case snd::LIB_BASS_ASIO:
			if( c_sound_bass_asio::open_lib() )
			{
				if( !_lib_bass_asio )
					_lib_bass_asio = new c_sound_bass_asio;
				lib = _lib_bass_asio;
			}
			break;
#endif
		}

		if( lib )
		{
			_s_lib_used = lib_asked;
			_lib = lib;
			_lib->enumerate();
		}
	}
	return _lib;
}

c_snd_input* c_snd_master::get_by_channel( INT32 channel_in )
{
	auto lib = get_lib();
	return lib ? lib->get_by_channel( channel_in ) : nullptr;
}

c_sound_player*	c_snd_master::add_player( INT32 device_index )
{
	if( _lib )
		return _lib->add_player( device_index );
	return nullptr;
}
bool	c_snd_master::delete_player( c_sound_player* pt )
{
	if( _lib )
		return _lib->delete_player( pt );
	return false;
}


AAA_ERR	c_snd_master::save_do_after( o_str CONST & filename_in )
{
	//hack need to be cleaned
	auto lib = get_lib();	
	if( lib )
		lib->save_snd_input( filename_in );	//	remove ext ?
	if_obj_save_add_ext( _lib_ds, filename_in );
	if_obj_save_add_ext( _lib_bass, filename_in );
	if_obj_save_add_ext( _lib_bass_asio, filename_in );
	return AAA_OK;
}

AAA_ERR	c_snd_master::load_do_after( o_str CONST & filename_in )
{
	//hack need to be cleaned
	auto lib = get_lib();
	if( lib )
		lib->load_snd_input( filename_in );	//	remove ext ?
	if_obj_load_add_ext( _lib_ds, filename_in );
	if_obj_load_add_ext( _lib_bass, filename_in );
	if_obj_load_add_ext( _lib_bass_asio, filename_in );
	return AAA_OK;
}

c_snd_master*	g_master = nullptr;

}	//namespace snd