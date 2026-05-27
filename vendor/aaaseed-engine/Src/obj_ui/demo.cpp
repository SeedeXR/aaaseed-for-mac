
#include "obj_ui/demo.h"

#if AAA_DEMO()

#include "file/aaa_file.h"
#include "infrastructure/aaa_env.h"
#include "time/aaa_time.h"
#include "infrastructure/layer/layer.h"
#include "checksum.h"
#include "draw/seeddraw.h"
#ifndef AAA_HMD_H
#	include "obj_ui/tracker/hmd.h"
#endif
#include "obj_ui/bdd/util/bdd.h"
#include "obj_ui/tracker/iglasses.h"
#include "stream_util.h"
#include "ui/aaa_menu.h"


#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif

#ifdef	WIN32
#	ifndef _INC_MMSYSTEM
#		ifndef AAA_AAA_OS_H
#			include "aaa_os.h"
#		endif
#		include "Mmsystem.h"
#	endif
#endif

#define	DEMO_SEE_SIGNATURE	TRUE

namespace {
	bool	b_demo_log = false;
	C_PCHAR sound_filename = "C:\\Pfizer\\data\\Events\\PfizerMarch98\\zoloft-mix.WAV";
}

#define	DEMO_HEADER	"# Demo ---> "

extern	c_checksum checksum_param;

c_demo* demo = nullptr;

void	demo_set_log( bool flag )
{
	b_demo_log = flag;
	SWITCH_PRINT_STATE("Demo log", b_demo_log);
}


//todo make it a obj_active_ui
//todo should add destructor
c_demo::c_demo( C_PCHAR_C filename )
{
FILE		*file;
char		str[AAA_PATH_MAX()];
//INT32		index;

	_env_nb = 0;
	_env_index = 0;

	_hd_filename = nullptr;
	_length = nullptr;
	_b_erase = nullptr;
	_e_action = nullptr;
	_b_tracker = nullptr;
	_checksum = nullptr;

	_time_stop = 0.;
	_state = 0;
	_b_loaded_ok = false;

	_b_active = false;

	if( filename )
	{
		//	open file
		file = c_file::FOPEN( filename, "rt" );
		if( !file )
		{
			ERR_PRINT_STRING( "demo %s n'existe pas.", filename );
			_b_loaded_ok = false;
		}
		else
		{
			DOUBLE	f;

			HEADER_PRINT_STRING( DEMO_HEADER, "Parsing demo file %s : ", filename );

			fscanf( file, "%260s", str);	//hack MAX_PATH_ could be different
			_env_nb = atoi(str);
			HEADER_PRINT_STRING( DEMO_HEADER, "There is %d environments :", _env_nb);
			//todo	check error
			_hd_filename = (char **) MALLOC( sizeof(char *) * _env_nb );
			_length = (DOUBLE *) MALLOC( sizeof(REAL) * _env_nb );
			_b_erase = (bool *) MALLOC( sizeof(bool) * _env_nb );
			_e_action = (INT32 *) MALLOC( sizeof(INT32) * _env_nb );
			_b_tracker = (bool *) MALLOC( sizeof(bool) * _env_nb );
			_checksum = (UINT32 *) MALLOC( sizeof(UINT32) * _env_nb );

			
			for( INT32 index = 0; index < _env_nb; ++index )
			{
				stream_fscanf_string( file, str, AAA_PATH_MAX() );

				_hd_filename[index] = (CHAR *) MALLOC( sizeof(CHAR) * strlen(str)+1 );
				strcpy( _hd_filename[index], str );

				_b_erase[index] = false;
				_b_tracker[index] = false;
				_e_action[index] = DEMO_TIME;
				_length[index] = 0.;
				_checksum[index] = 0;

				if ( fscanf( file, " %lf", &f) == 1 )
				{
					_e_action[index] = DEMO_TIME;
					fscanf( file, "%lf", &f);
					_length[index] = f; 
				}
				while( true )
				{
					stream_fscanf_string( file, str, AAA_PATH_MAX() );
					if ( str_is_equal_nocase( str, "time") )
					{
						_e_action[index] = DEMO_TIME;
						fscanf( file, "%lf", &f);
						_length[index] = f; 
					}
					else if ( str_is_equal_nocase( str, "wait_fix") )
					{
						_e_action[index] = DEMO_WAIT_FOR_FIX;
						fscanf( file, "%lf", &f);
						_length[index] = f; 
					}
					else if ( str_is_equal_nocase( str, "wait_hook") )
					{
						_e_action[index] = DEMO_WAIT_FOR_HMD_FROM_HOOK;
						fscanf( file, "%lf", &f);
						_length[index] = f; 
					}
					else if ( str_is_equal_nocase( str, "music_start") )
					{
						_e_action[index] = DEMO_MUSIC_START;
						fscanf( file, "%lf", &f);
						_length[index] = f; 
					}
					else if ( str_is_equal_nocase( str, "music_stop") )
					{
						_e_action[index] = DEMO_MUSIC_STOP;
						fscanf( file, "%lf", &f);
						_length[index] = f; 
					}
					else if ( str_is_equal_nocase( str, "clear") )
						_b_erase[index] = true;
					else if ( str_is_equal_nocase( str, "noclear") )
						_b_erase[index] = false;
					else if ( str_is_equal_nocase( str, "tracker_on") )
						_b_tracker[index] = true;
					else if ( str_is_equal_nocase( str, "tracker_off") )
						_b_tracker[index] = false;
					else if ( str_is_equal_nocase( str, "_checksum") )
						fscanf( file, "0x%x", &_checksum[index]);
					else if ( str_is_equal_nocase( str, ";") )
						break;
					else
						BOX_ERR( str );
				}
				HEADER_PRINT_STRING( DEMO_HEADER, "Environment %s for %lf", _hd_filename[index], _length[index] ); 
			}
			_b_loaded_ok = true;
			c_file::FCLOSE(file);
		}
	}
	start();
}

c_demo::~c_demo()
{
	dealloc();
}

void	c_demo::dealloc()
{
	FREE( _hd_filename );
	FREE( _length );
	FREE( _b_erase );
	FREE( _e_action );
	FREE( _b_tracker );
	FREE( _checksum );
}

void c_demo::start()
{
	if( !_b_active && _b_loaded_ok )
	{
		_state = 0;
		_env_index = -1;
		resume();
	}
}

void c_demo::resume()
{
	if( !_b_active && _b_loaded_ok )
	{
		if( b_demo_log )
			c_file::start_log_open_file( "demo.log");
		_b_active = true;
	}
}

void c_demo::stop()
{
	if( _b_active )
	{
		if( b_demo_log)
			c_file::stop_log_open_file();
		_b_active = false;
	}
}

void c_demo::set_env( INT32	index_in)
{
	if ( b_demo_log && (index_in >= _env_nb) )
	{
		c_file::stop_log_open_file();
	}
	#if	DEMO_SEE_SIGNATURE
		index_in = IMOD( index_in, _env_nb+1);
	#else
		index_in = IMOD( index_in, env_nb);
	#endif
	if ( index_in == _env_nb )
	{
		if( c_layer::get_ui() )
			c_layer::get_ui()->bdd_switch( "Signature" );
		_time_stop = REAL(aaa::time::get_real_time()) + REAL(4);
	}
	else
	{
#if	AAA_CHECKSUM_PARAM_DO()
		UINT32	checksum_calculated;
		checksum_param.reset();
#endif

		aaa::env::load( _hd_filename[index_in] );

#if	AAA_CHECKSUM_PARAM_DO()
		checksum_calculated = checksum_param.get();
		if ( _checksum[index_in] == 0 )
		{
#if	AAA_DEBUG()
				DBG_PRINT_STRING( "_checksum of %s is 0x%8x but there is nothing to check against.", _hd_filename[index_in], checksum_calculated );
				BOX_WAR( get_mess_str(1) );
#else
				#if	AAA_CHECKSUM_ENV_LOCKED()
					exit( 0 );
				#endif
#endif
		}
		else
		{
			if ( _checksum[index_in] == checksum_calculated )
			{
#if	AAA_DEBUG()
					GOOD_PRINT_STRING( "Checksum of environment %s is correct.", _hd_filename[index_in] );
#endif
			}
			else
			{
#if	AAA_DEBUG()
					BOX_ERR( "Checksum of environment %s calculated 0x%8x\nwhen it should be 0x%8x",
								_hd_filename[index_in], checksum_calculated, _checksum[index_in]);
#else
	#if	AAA_CHECKSUM_ENV_LOCKED()
					exit( 0 );
	#endif
#endif
			}
		}
#endif	//AAA_CHECKSUM_PARAM_DO()

		if ( _b_erase[index_in] )
			reset_erased_screens();
#if AAA_TRACKER_HMD()
		b_hmd_active = _b_tracker[index_in];
#endif //AAA_TRACKER_HMD
		_time_stop = REAL(aaa::time::get_real_time() + _length[index_in]);

		wchar_t * wch = sysutils::utf8_to_unicode( sound_filename );
		switch( _e_action[index_in] )
		{
			case DEMO_MUSIC_START:
#ifdef	WIN32
				PlaySoundW( wch, nullptr, SND_ASYNC);
#endif
				break;
			case DEMO_MUSIC_STOP:
#ifdef	WIN32
				PlaySoundW( nullptr, nullptr, SND_PURGE);
#endif
				break;
		}
		sysutils::free_str_tmp( wch );
	}
	_env_index = index_in;
}

void c_demo::next_env()
{
	set_env( _env_index+1 );
}

void c_demo::prev_env()
{
	set_env( _env_index-1 );
}

#if AAA_TRACKER_HMD()

namespace{
	REAL	xl, yl, zl;
	REAL	time_fix;
	bool	b_fix;
}

#define	FIX_DELTA	.5
bool c_demo::hmd_is_fix()
{
	bool	b_fix_now;
	REAL	x, y, z;

	//pfizer
	b_fix_now = false;
	x = euler_cur.x_filtered();
	y = euler_cur.y_filtered();
	z = euler_cur.z_filtered();
	if (	ABS(x-xl) < FIX_DELTA 
		&&	ABS(y-yl) < FIX_DELTA 
		&&	ABS(z-zl) < FIX_DELTA )
		b_fix_now = true;
	xl = x;
	yl = y;
	zl = z;
	if( b_fix_now != b_fix )
	{
		DBG_PRINT_STRING( b_fix_now ?
			"HMD got stable":
			"HMD moved");
	}
	if( b_fix )
	{
		if( b_fix_now )
		{
			if( (time_fix + 2.) < aaa::time::get_real_time() )
			{
				b_fix = false;
				return true;
			}
		}
		else
		{
			b_fix = false;
		}
	}
	else
	{
		if( b_fix_now)
		{
			time_fix = aaa::time::get_real_time();
			b_fix = true;
		}
	}
	return false;
}
	
bool	b_hmd_on_hook = false;
bool	b_hmd_ready_to_go = false;
REAL	time_in;
bool	b_in_now_last;

bool c_demo::hmd_out_of_hook()
{
	REAL	tmp;
	bool	b_in_now;

	//pfizer
	//	check if hmd is not used
	b_in_now = false;
	tmp = euler_cur.x_filtered();
	if( tmp < -25 )
	{
		tmp = euler_cur.y_filtered();
		if(  -19. < tmp && tmp < -10. )
		{
			tmp = euler_cur.z_filtered();
			if(  -48. < tmp && tmp < -33. )
				b_in_now = true;
		}
	}
	if( b_in_now_last != b_in_now )
	{
		DBG_PRINT_STRING( b_in_now ?
			"HMD got close from hook":
			"HMD got far from hook");
		b_in_now_last = b_in_now ;
	}
	if( b_hmd_on_hook )
	{
		if( b_in_now )
		{
			if( (time_in + 3.) < aaa::time::get_real_time() )
				b_hmd_ready_to_go = true;
		}
		else
		{
			if( b_hmd_ready_to_go )
			{
				b_hmd_on_hook = false;
				//go
				return true;
			}
			else
				b_hmd_on_hook = false;
		}
	}
	else
	{
		if( b_in_now)
		{
			time_in = aaa::time::get_real_time();
			b_hmd_on_hook = true;
			b_hmd_ready_to_go = false;
		}
		else
			b_hmd_on_hook = false;
	}
	return false;
}
#endif //AAA_TRACKER_HMD

void c_demo::update()
{
	if( !menu::is_on() )
	{
		if( _b_active )
		{
			switch(_state)
			{
			case 0:
				next_env();
				_state = 1;
				break;
			case 1:
				switch( _e_action[_env_index] )
				{
				case DEMO_WAIT_FOR_HMD_FROM_HOOK:
#if AAA_TRACKER_HMD()
					if ( hmd_out_of_hook() )
					{
						_env_index = -1;
						_state = 0;
					}
					else if ( aaa::time::get_real_time() > _time_stop )
						_state = 0;
					break;
				case DEMO_WAIT_FOR_FIX:
					if ( hmd_is_fix() )
					{
						_state = 0;
						vio_tracker_set_origin();
					}
#endif //AAA_TRACKER_HMD
				default:
					if ( aaa::time::get_real_time() > _time_stop )
						_state = 0;
					break;
				}
			}
		}
		else
		{
//pfizer
/*	#if	AAA_DEMO_LOCKED()
			switch(_state)
			{
			case 0:
				c_layer::ui->bdd_switch(BDD_SIGNATURE);
				_state = 1;
				break;
			case 1:
				break;
			}
	#endif
*/		}

	}
}

#endif // #if AAA_DEMO()