//	13 Jan 97	Maa		First try at snapshot for the virtual baguette

#include "aaa_def.h"
#include "obj_ui/snap.h"
#include "infrastructure/seedfile.h"
#include "time/aaa_time.h"
#include "strnum.h"
#include "infrastructure/layer/app.h"
#include "image/bind_img_2d.h"
#include "draw/seeddraw.h"
#include "infrastructure/viewport.h"
#include "infrastructure/param/param_declare.h"
#include "draw/tex.h"

#ifdef	WIN32
#	ifndef _INC_MMSYSTEM
#		ifndef AAA_AAA_OS_H
#			include "aaa_os.h"
#		endif
#		include "Mmsystem.h"
#	endif
#endif

FACTORY_CREATE_V1( c_snap, snap, Shoot, snap );

c_snap*	c_snap::cur = nullptr;

static	c_img_2d*	pano_img = nullptr;
static	UINT8*	pano_buf = nullptr;
static	INT32	pano_x_dst;

static	o_str	snapshot_file_name;

enum	SNAP_TYPE : INT32
{
	SNAP_SIMPLE = 0,
	SNAP_MULTIPLE,
	SNAP_PANORAMIC,
	SNAP_NB
};

static	C_PCHAR_C	snap_type_str[SNAP_NB] =
{
	"Simple",
	"Multiple",
	"Panoramic",
};

namespace	n_snap
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 14;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(						Active				)
		PARAM_DEF_BOOL_OFF(						Run					)
		PARAM_DEF_BOOL_OFF(						Shoot				)
		PARAM_DEF_SYMBO(						type,				1, 0,	SNAP_NB-1, snap_type_str )
		PARAM_DEF_INT32_ONE(					pano_size_x			)
		PARAM_DEF_REAL_INF(						Line_pos,			0, .5	)
		PARAM_DEF_BASE(			TYPE_TIMECODE,	Start,				0, 123,	PARAM_MIN_FP32, PARAM_MAX_FP32 )
		PARAM_DEF_BASE(			TYPE_REAL,		Stop,				4, 1,	PARAM_MIN_REAL, PARAM_MAX_REAL )
		PARAM_DEF_REAL_INF(						Step,				.1, .04 )
		PARAM_DEF_INT32_POS(					Preroll,			10, 0	)
		PARAM_DEF_BASE(			TYPE_TIMECODE,	Current,			1, 0,	PARAM_MIN_FP32, PARAM_MAX_FP32 )
		PARAM_DEF_INT32_LOCKED(					Frame_index			)
		PARAM_DEF_BOOL_OFF(						load_frames			)
		PARAM_DEF_INT32_POS_ONE(				file_start_number	)
	};
}

void	c_snap::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _b_active_ui		);
	param_set_pt( h, _b_run_ui			);
	param_set_pt( h, _b_shoot_ui		);

	param_set_pt( h, _type				);
	param_set_pt( h, _pano_size_x		);
	param_set_pt( h, _line_pos			);
	param_set_pt( h, _start				);
	param_set_pt( h, _stop				);
	param_set_pt( h, _step				);
	param_set_pt( h, _preroll			);
	param_set_pt( h, _current			);
	param_set_pt( h, _frame_nb			);
	param_set_pt( h, _b_load_frame_ui	);
	param_set_pt( h, _file_start_nb		);
/*
	param_set_pt( h, tra_start[0] );
	param_set_pt( h, tra_end[0] );
	param_set_pt( h, tra_start[1] );
	param_set_pt( h, tra_end[1] );
	param_set_pt( h, tra_start[2] );
	param_set_pt( h, tra_end[2] );

	param_set_pt( h, rot_start[0] );
	param_set_pt( h, rot_end[0] );
	param_set_pt( h, rot_start[1] );
	param_set_pt( h, rot_end[1] );
	param_set_pt( h, rot_start[2] );
	param_set_pt( h, rot_end[2] );
*/
	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_snap )
{
	param_init_with( n_snap::param, n_snap::PARAM_NB_MAX ); // snap_param, SNAP_PARAM_NB_MAX );
}
c_snap::~c_snap()
{
	if( cur == this )
		cur = nullptr;
	SAFE_DELETE( pano_img );
	FREE_ALIGNED_AND_NULL( pano_buf );
}
//clean ?	return	param_save_list_to_file( filename, "Snap", 0, snap_param+3, SNAP_PARAM_NB_MAX-3 );

void	c_snap::param_init()
{
	set_active( false );
	_b_shoot = false; 
	_b_run = false;
	_frame_nb = 0;
	_b_record_trax = false;
}

void	c_snap::set_frame_nb( INT32 frame_in )
{
	_frame_nb = frame_in;
	if( _frame_nb == 0 )
		g_app->traxs_start_preroll();
	else if( _frame_nb == 1 )
		g_app->traxs_start_loop( _b_record_trax );

	if( _b_shoot && _b_load_frame_ui )
	{
		c_img_2d*	img = g_bind_img_2d->get_ui();
		if( img )
		{
			CHAR	filename[AAA_PATH_MAX()];
			strncpy( filename, img->get_filename(), AAA_PATH_MAX() );	//todo really clean
			strnum::make( filename + strlen(filename)-8, 4, _frame_nb+1 );
			g_bind_img_2d->load_texture( g_bind_img_2d->get_ui_index(), filename, false );
		}
		else
		{
			box_err( "Can't load image for shooting\nNo texture chosen." );
			flip_shoot();
		}
	}
}

static	DOUBLE	time_last;
void	c_snap::goto_start()
{
	time_last = aaa::time::get_real_time();
	_current = _start - _step*_preroll;
	set_frame_nb( 0 );
}

void	c_snap::goto_end()	//todo do it weel unused for now
{
	_current = _stop;
	set_frame_nb( INT32( ( _stop - _start ) / _step ) );
}

bool c_snap::set_shoot( bool CONST b_in )
{
	if( b_in != _b_shoot )
	{
		if( b_in )
		{
			o_str str;
			if( NOERR( aaa::file::get_type_io_filename_save( aaa::file::TYPE_IO_SNAPSHOT, str, nullptr ) ) )
			{
				snapshot_file_name.set( str );
				_b_shoot = true;
				set_active( true );
				if( _type == SNAP_PANORAMIC )
				{
					pano_img = c_img_2d::img_init_with_size( pano_img, _pano_size_x, viewport_render->get_sy(), aaa::PIXEL_FORMAT::RGBA_8, __FUNCTION__ );
					pano_buf = ( UINT8 * ) REALLOC_ALIGNED( pano_buf, viewport_render->get_sy() * 4 * 16 );
					pano_x_dst = 0;
				}
				if( _type != SNAP_PANORAMIC || ( pano_img && pano_buf ) )
					goto_start();
			}
		}
		else
		{
			if( _type == SNAP_PANORAMIC )
			{
				pano_img->write( snapshot_file_name );
				GOOD_PRINT_STRING( "Saved panoramic image as %s", snapshot_file_name.get() );
				SAFE_DELETE( pano_img );
				FREE_ALIGNED_AND_NULL( pano_buf );
			}
			_b_shoot = false;
		}
		_b_shoot_ui = _b_shoot;
		_b_active_ui = _b_shoot;
		SWITCH_PRINT_STATE( "Snap run", _b_shoot );
	}
	return _b_shoot;
}

void	c_snap::stop_shoot()	{ set_shoot( false );		}
void	c_snap::start_shoot()	{ set_shoot( true );		}
void	c_snap::flip_shoot()	{ set_shoot( !_b_shoot );	}

//todo compute digit_nb from the range
//todo	have a min too
static	INT32 CONST	DIGIT_NB = 6;
void	c_snap::do_shoot( INT32 which_buffer )
{
	if( _frame_nb > 0 )
	{
		if( _type != SNAP_PANORAMIC )
		{
			CHAR	str_digit[DIGIT_NB+1];
			str_digit[DIGIT_NB] = 0;
			o_str	filename;
			filename.set_fname_relative( snapshot_file_name );
			filename.convert_to_backslash();
			/*	was ttk
			INT32 tmp = _frame_nb;
			digit_nb = 1;
			while( tmp /= 10 )
				++digit_nb;
			digit_nb = MAX( digit_nb, 2L );
			*/

			strnum::make( str_digit, DIGIT_NB, _frame_nb-1+_file_start_nb );
			filename.add( str_digit );

			c_dir::change_to_def();
			save_frame_buffer_to_file( filename.get(), which_buffer, (_frame_nb%25)==0 );
		}
		else
		{
			GLenum	format = pano_img->get_gl_format();		
			if( format == GL_INVALID_VALUE )
			{
				ERR_PRINT_STRING( "%s() invalid Gl_format", __FUNCTION__ );
				return;
			}
			tex2d.read_pixels( (void*)pano_buf, format, pano_img->get_gl_type(),
									viewport_render->convert_cano_to_pix_x( _line_pos ), viewport_render->get_y(),
									1, viewport_render->get_sy(),
									which_buffer );
			pano_img->set_colum( pano_buf, pano_x_dst++, viewport_render->get_y() );
		}
	}
}

bool	c_snap::is_shoot()
{
	return _b_shoot;
}

void	c_snap::record_flip()
{
	if( _b_record_trax )
	{
		_b_record_trax = false;
#ifdef	WIN32
		PlaySound( nullptr, nullptr, SND_PURGE );
#endif
		g_app->traxs_stop_loop();
	}
	else
	{
		_b_record_trax = true;
		set_run( false );
		goto_start();
	}
}

//todo such a strange name to avoid the other one
bool c_snap::active_set( bool b_in )
{
	bool b  = is_active();
	if( b_in != b )
	{
		if( b_in )
		{
			set_active(true);
			goto_start();
		}
		else
			set_active( false );
		b = is_active();
		_b_active_ui = b;
		SWITCH_PRINT_STATE( "Snap run", b );
	}
	return b;
}

bool c_snap::set_run( bool b_in )
{
	if( b_in != _b_run )
	{
		_b_run = b_in;
		_b_run_ui = _b_run;
		SWITCH_PRINT_STATE( "Snap run", _b_run );
	}
	return _b_run;
}

void c_snap::run_next()
{
	if( _b_run )
	{
		REAL	step_loc;
		if( _current >= _stop )
			goto_start();
		else
		{
			if( _b_shoot )
				step_loc = _step;
			else
			{
				DOUBLE time = aaa::time::get_real_time();
				step_loc = ( REAL )( time-time_last );
				time_last = time;
			}
			_current += step_loc;
			if( _current >= _start )
				set_frame_nb( _frame_nb + 1 );
			if( _current >= _stop )
			{
				if( _b_record_trax )
					record_flip();
				stop_shoot();
			}
			aaa::time::set( _current, step_loc );
		}
	}
	else
		time_last = aaa::time::get_real_time();
}

void c_snap::run_prev()
{
	if ( _current <= _start )
		goto_end();
	else
	{
		_current -= _step;
		if ( _current >= _start )
			set_frame_nb( _frame_nb - 1 );
	}
}


void c_snap::update()
{
	if( _type == SNAP_PANORAMIC )
		_step = ( _stop - _start ) / _pano_size_x;
	set_shoot( _b_shoot_ui );
	active_set( _b_active_ui );
	set_run( _b_run_ui );
	if( is_active() )
		run_next();
}

void	c_snap::run_resume()	{	set_run( true );		}
void	c_snap::run_stop()		{	set_run( false );		}
void	c_snap::run_start()		{	set_run( false );		goto_start();	}
void	c_snap::run_flip()		{	set_run( !_b_run );		}

