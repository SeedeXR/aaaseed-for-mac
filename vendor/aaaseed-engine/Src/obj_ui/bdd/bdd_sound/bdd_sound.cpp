#include "obj_ui/bdd/bdd_sound/bdd_sound.h"
#include "math/rand.h"
#include "obj_ui/bdd/bdd_geo/bdd_tri.h"
#include "media/sound/snd_master.h"
#include "obj_ui/com/midi_data.h"
#include "draw/model.h"
//todoq reactivate or delete things link to it
//#include "infrastructure/averager.h"	
#include "gol/gol_matrix.h"
#include "gol/gol_draw.h"
#include "infrastructure/data/datacube.h"
#include "infrastructure/data/data_src.h"
#include "draw/render.h"

/*
typedef enum	{
VISU_DRAW_PREV,
VISU_DRAW_CUBE,
VISU_DRAW_FACE,
VISU_DRAW_GRID,
VISU_DRAW_LINE,
VISU_DRAW_TYPE_MAX_NB,
}
VISU_DRAW_TYPE;

C_PCHAR_C	str_visu_draw_type[VISU_DRAW_TYPE_MAX_NB] =
{
	"PREV",
	"CUBE",
	"FACE",
	"GRID",
	"LINE",
};
*/

FACTORY_CREATE_PROP_V1( c_bdd_sound, bdd_sound, sound visualization, bddsound, sub_menu="Sound"; );

namespace n_bdd_sound
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 21 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 GROUP_NB		= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB
									+ GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_POINT_XYZ(		origin	)
		PARAM_DEF_SYMBO_PSTR_ZERO(	type,		gstr_data_src	)
		PARAM_DEF_INT32(			channel,	2, 1,	1, c_snd_input::SOUND_CHANNEL_NB_MAX	)

		// hack test band_nb to be sure we don't go to far
		PARAM_DEF_INT32(			col_start,	2, 1,	1, PARAM_MAX_INT32	)
		PARAM_DEF_INT32(			col_nb,		1, 4,	1, PARAM_MAX_INT32	)
		PARAM_DEF_INT32(			row_nb,		1, 4,	1, PARAM_MAX_INT32	)
		PARAM_DEF_BOOL_OFF(			draw_loop	)
		PARAM_DEF_INT32(			strobe,		0, 1,	1, 100				)
		//	{	nullptr,	PARAM_SYMBOLIC,	"draw_type",	0, 1,	0, VISU_DRAW_TYPE_MAX_NB-1,	nullptr, str_visu_draw_type },
		PARAM_DEF_BOOL_OFF(			use_max		)
		PARAM_DEF_POINT_UVA(		offset		)
		PARAM_DEF_POINT_UVA(		size		)
		PARAM_DEF_POINT_UVA(		direction	)
		PARAM_DEF_BOOL_OFF(			cylinder	)
		//		{	nullptr,	PARAM_REAL,	"filter",		1, 0,	0, FLUX_FILTER_MAX,	nullptr, nullptr },
	};
	PARAM_DEF_MAKE_INDEX( channel );
};

void	c_bdd_sound::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3(	h, _origin );
	param_set_pt(		h, _type_ui );

	param_attach_obj_no_inc( h, snd::g_master->get_by_channel( _channel_ui ) );	//todo we also have _sound_input_ui
	param_set_pt(		h, _channel_ui );

	param_set_pt(		h, _col_start_ui );
	param_set_pt(		h, _col_nb_ui );
	param_set_pt(		h, _row_nb_ui );
//	param_set_pt(		h, col_index_cur);
	param_set_pt(		h, _b_draw_loop_ui );
	param_set_pt(		h, _strobe_ui );
//	param_set_pt(		h, s_draw_type);

	param_set_pt(		h, _b_use_max_ui );
	param_set_pt_3(	h, _size_offset );
	param_set_pt_3(	h, _size );
	param_set_pt_3(	h, _size_direction );
	param_set_pt(		h, _b_cylinder_ui );
//	param_set_pt(		h, filter_factor_ui);

	err_param_init_pt( h );
}


void c_bdd_sound::init_data(  UINT32 nb  )	//todoq should change with dynamic structure for band_buf
{
	MEMCLEAR( (void*)_data, nb*sizeof(REAL) );
}


void c_bdd_sound::reset_max_factor()
{
	for( INT32 i = 0; i < _band_nb; ++i )
	{
		_max[i] = 0.;
		_max_factor[i] = 1.;
	}	
}


void c_bdd_sound::init()
{
	_sound_input_ui = nullptr;
	_channel = 0;

	_band_nb = 0;
	_band_allocated = 0;
	_data_allocated = 0; 

	_data = nullptr;
	_max = nullptr;
	_max_factor = nullptr;

//	averager = nullptr;
	_strobe_count = 0;

	_b_valid = false;

//	col_start_last_ = -1;
//	col_nb_last_ = -1;
//	row_nb_last_ = -1;

	_receipt_index = 0;
}

CONSTRUCTOR_CREATE(c_bdd_sound )
	,_channel_ui	(1)
{
	init();
	param_init_with( n_bdd_sound::param, n_bdd_sound::PARAM_NB_MAX );
}

c_bdd_sound::~c_bdd_sound()
{
	dealloc_band();
	dealloc_data();
}

void	c_bdd_sound::alloc_data( INT32 row_nb, INT32 col_nb )
{
	INT32	nb = row_nb * col_nb;
	if( nb > _data_allocated )
	{
		//dealloc_data();
		_data = (REAL*) REALLOC_ALIGNED( _data, nb*sizeof(REAL) );
		if( _data )
		{
			_receipt_index = 0;
			_data_allocated = nb;
			init_data( nb );
		}
		else
		{
			_data_allocated = 0;
		}
	}
}

void	c_bdd_sound::alloc_band( INT32 band_nb )
{
	_b_valid = false;
	if( band_nb > _band_allocated )
	{
		_max		= (REAL*) REALLOC_ALIGNED( _max,		band_nb * sizeof(REAL) );	//hack +1 ?
		_max_factor	= (REAL*) REALLOC_ALIGNED( _max_factor, band_nb * sizeof(REAL) );
		if( _max && _max_factor )
		{
			_receipt_index = 0;
			_band_allocated = band_nb;
		}
		else
		{
			_band_allocated = 0;
		}
	}
}

void	c_bdd_sound::dealloc_data()
{
	IF_FREE_ALIGNED_AND_NULL( _data );
	_data_allocated = 0;
}

void	c_bdd_sound::dealloc_band()
{
	IF_FREE_ALIGNED_AND_NULL( _max );
	IF_FREE_ALIGNED_AND_NULL( _max_factor );
	_band_allocated = 0;
}

void	c_bdd_sound::update()
{
//	if( !averager)
//		alloc();
//	c_file::dir_change_to_def();

/*	if( filter_factor != filter_factor_ui )
		{
		filter_factor = filter_factor_ui;
		for( i=0; i<BDD_SOUND_AVERAGER_MAX_NB; ++i )pri
			averager[i].set_filter_factor( filter_factor);
		}

	src = col_nb + col_index_cur;
	dst = 0;
	for( i=0; i<nb_row; ++i )
		{
		for( j=0; j<col_nb; ++j )
			{
			averager[dst++].put(data[src++]);
			}
		}
*/
	_b_valid = false;
	switch( _type_ui )
	{
		case SOUND_SPECTRUM_RAW:
		case SOUND_SPECTRUM_LINEAR:
		case SOUND_SPECTRUM:
		case SOUND_WAVE:
			//if( _channel != _channel_ui )
			{
				_sound_input_ui = snd::g_master->get_by_channel( _channel_ui );
				param_attach_obj_no_inc( n_bdd_sound::PARAM_INDEX_channel, _sound_input_ui );
				if( _sound_input_ui )
				{
					_channel = _channel_ui;
				}
				else
				{
					_channel = 0;	//	to be sure we recheck next time
					return;
				}
			}
			break;
		default:
			break;
		}


	INT32 col_nb = 0;
	switch( _type_ui )
	{
	case SOUND_SPECTRUM_RAW:	col_nb = _sound_input_ui->get_spectrum_band_nb();	break;
	case SOUND_SPECTRUM_LINEAR:
	case SOUND_SPECTRUM:
	case SOUND_WAVE:
	case DATACUBE:				col_nb = _col_nb_ui;								break;
	case MIDI_CONTROL:			col_nb = MIN( _col_nb_ui, c_midi::CONTROL_NB );		break;
	case MIDI_VELOCITY:			col_nb = MIN( _col_nb_ui, c_midi::NOTE_NB );		break;
	}
	
	if( _b_use_max_ui )
	{
		alloc_band( col_nb );
		if( !_band_allocated )
		{
			return;
		}
		if( _band_nb != col_nb )
		{
			_band_nb = col_nb;
			reset_max_factor();
		}
	}

	alloc_data( col_nb, _row_nb_ui );
	if( !_data )
	{
		_col_nb = 0;
		_row_nb = 0;
		return;
	}

	_b_valid = true;
	_col_nb = col_nb;
	_row_nb = _row_nb_ui;

	if( _col_start_ui > _col_nb )
		_col_start_ui = _col_nb;
//	if( _col_nb_ui > col_nb )
//		_col_nb_ui = _band_nb;

/*
	if( col_nb_last_ != _col_nb || col_start_last_ != _col_start_ui )	//|| row_nb_last_ != _row_nb )
	{
		col_nb_last_ = _col_nb;
		col_start_last_ = _col_start_ui;
		row_nb_last_ = _row_nb;
		reset_max_factor();
	}
*/

	INT32 nb;
	switch( _type_ui )
	{
	case SOUND_SPECTRUM_RAW:
	case SOUND_SPECTRUM_LINEAR:
	case SOUND_SPECTRUM:
	case SOUND_WAVE:
		nb = _sound_input_ui ? _sound_input_ui->get_count_new() : 0 ;
		break;
	default:
		nb = 1;
		break;
	}
		
	if( ++_strobe_count >= _strobe_ui )
	{
		_strobe_count = 0;
		for( INT32 i=0; i<nb; ++i )
		{
			_receipt_index = IMOD( _receipt_index + 1, _row_nb );
			REAL*	dst = &_data[_receipt_index * _col_nb];
			//	printf( "index %d\n", receipt_index );
			INT32 col_nb;
			switch( _type_ui )
			{
			case SOUND_SPECTRUM_RAW:
				col_nb = _sound_input_ui->get_spectrum_raw_band( dst, _channel ); // c_snd_input::get( _channel )->spectrum_band_nb_get();
				if( col_nb <= 0 )
					goto exit;
				break;
			case SOUND_SPECTRUM_LINEAR:
			case SOUND_SPECTRUM:
				col_nb = _sound_input_ui->get_spectrum_band( dst, _col_nb, _channel, .0, 1., _type_ui==SOUND_SPECTRUM_LINEAR );
				if( col_nb <= 0 )
					goto exit;
				break;
			case SOUND_WAVE:
				//	col_nb_ = _sound_input_ui->general_get_wave( dst, col_nb_, _channel );
				col_nb = _sound_input_ui->get_wave( dst, _col_nb, _channel );
				if( col_nb <= 0 )
					goto exit;
				//	if ( c_snd_input::get( _channel ) )
				//	col_nb_ = c_snd_input::general_get_wave( dst, col_nb_, _channel );
				break;
			case MIDI_CONTROL:
				midi_control_get_band( _channel_ui, _col_start_ui, _col_nb, dst );
				break;
			case MIDI_VELOCITY:
				midi_velocity_get_band( _col_start_ui, _col_nb, dst );
				break;
			case DATACUBE: 
				for( INT32 r = 0; r < _row_nb; ++r )
				{
					for( INT32 c = 0; c < _col_nb; ++c )
					{
						_data[r * _col_nb + c] = g_datacube->get_fp32( 1, r + 1, c + 1 );
					}
				}
				break;
			}
		}
	}
	return;
exit:
	DBG_PRINT_STRING( "%s() Could not read data", __FUNCTION__ );
}

/*
void	c_bdd_sound::draw()
{
INT32	c;
INT32	r;
REAL	u;
REAL	v;
REAL	du;
REAL	dv;
REAL	daxe;

	du = bdd_size[0]/nb_row;
	dv = bdd_size[1]/col_nb;
	daxe = bdd_size[2];

REAL	tmp;
REAL*	pr;

INT32	itmp;
REAL	pos[3];

	pos[1] = origin[1];
	u = origin[0] -.5*bdd_size[0] + du*.5;
	for ( r=0; r<nb_row; ++r )
		{
		pos[0]= u;
		v = origin[2]-.5*bdd_size[1] + dv*.5;
		if( b_draw_loop)
			pr = &band_buf[(itmp = IMOD( receipt_index-r, HISTO_NB))*BAND_NB];
		else
			pr = &band_buf[itmp =r*BAND_NB];
//		printf( "%d ", itmp );
		for ( c=0; c<col_nb; ++c )
			{
			pos[2]= v;

			tmp = *pr;
			if ( tmp > max[c] )
				{
				max[c] = tmp;
				max_factor[c] = 1./tmp;
				}
			if( b_use_max )
				tmp *= max_factor[c];
			switch( s_draw_type )
				{
				case VISU_DRAW_PREV:
					GOL::matrix::push();
						GOL_translate( u, tmp*.5, v);
						GOL_scale(	du*		(size_offset[0]+size[0]*tmp),
									daxe*	(size_offset[2]+size[2]*tmp),
									dv*		(size_offset[1]+size[1]*tmp)
								 );
						get_bdd_prev()->draw();
					GOL::matrix::pop();
					break;
				case VISU_DRAW_CUBE:
					draw_box_at(	du*		(size_offset[0]+size[0]*tmp),
									daxe*	(size_offset[2]+size[2]*tmp),
									dv*		(size_offset[1]+size[1]*tmp),
									pos
								);
					break;
				case VISU_DRAW_FACE:
					pos[1] = origin[2] + daxe*	(size_offset[2]+size[2]*tmp);
					draw_rect_at(	du*		(size_offset[0]+size[0]*tmp),
									dv*		(size_offset[1]+size[1]*tmp),
									pos, 1
								);
					break;
				}
			++pr;
			v += dv;
			}
		u += du;
		}
//	printf( "\n", tmp );
}
*/
FINLINE INT32 c_bdd_sound::get_draw_row_index( INT32 CONST index )
{
	return _b_draw_loop_ui ? IMOD( _receipt_index - index, _row_nb ) : index;
}

CONSTEXPR REAL LOCAL_BIG = 10000000.;
FINLINE REAL c_bdd_sound::process_max( REAL val, INT32 CONST index )
{
	if( !_b_use_max_ui )
		return val;

	if( !_finite(val) )
	{
		debug_break( "%s() Not a number", __FUNCTION__ );
		val = LOCAL_BIG;
	}
	if( val > _max[index] )
	{
		if( val == 0. )
		{
			debug_break( "%s() 0.", __FUNCTION__ );
			val = REAL( 1. / LOCAL_BIG );
		}
		_max[index] = val;
		_max_factor[index] = OVER_ONE_AS_REAL(val);
		return 1.;
	}
	return val * _max_factor[index];
}

void	c_bdd_sound::draw_single()
{
	if( !_b_valid )
		return;

	INT32	i_axe;
	INT32	i_u;
	INT32	i_v;
	c_model::cur->get_axes( i_u, i_v, i_axe );	

	REAL	bdd_size[3];
	c_model::cur->get_size_v3( bdd_size );

	REAL	du		= bdd_size[0] / _row_nb;
	REAL	dv		= bdd_size[1] / _col_nb;
	REAL	daxe	= bdd_size[2];

	REAL	u = _origin[i_u] - REAL(.5) * bdd_size[0] + du * REAL(.5);
	for( INT32 r = 0; r < _row_nb; ++r )
	{
		REAL	pos[3];
		pos[i_u] = u;
		REAL	v = _origin[i_v] - REAL(.5) * bdd_size[1] + dv * REAL(.5);

		INT32	itmp = get_draw_row_index( r );

//		printf( "%d ", itmp );
		REAL*	pr = &_data[ itmp * _col_nb ] - 1;
		GOL::begin( c_render::get_cur()->get_draw_primitive() );
			for( INT32 c = 0; c < _col_nb; ++c )
			{
				pos[i_v] = v;

				REAL tmp = process_max( *++pr, c );

				REAL size_axe =  daxe * ( _size_offset[2] + _size[2] * tmp );
				pos[i_axe] = _origin[i_axe] + size_axe * ( _size_direction[2] - REAL(1) ) * REAL(.5);
				GOL::vertex3v( pos );

				pos[i_axe] += size_axe;
				GOL::vertex3v( pos );

				v += dv;
			}
		GOL::end();
		u += du;
	}
}

void	c_bdd_sound::draw_multiple()
{
	if( !_b_valid )
		return;

	INT32	i_axe;
	INT32	i_u;
	INT32	i_v;
	c_model::cur->get_axes( i_u, i_v, i_axe );

	REAL	bdd_size[3];
	c_model::cur->get_size_v3( bdd_size );

	REAL du = bdd_size[0] / _row_nb;
	REAL dv = bdd_size[1] / _col_nb;
	REAL daxe = bdd_size[2];

	c_multiple::cur->set_nb( _col_nb, _row_nb );

	REAL	u = _origin[i_u] - REAL(.5) * bdd_size[0] + du * REAL(.5);
	for( INT32 r = 0; r < _row_nb; ++r )
	{
		//DBG_PRINT_STRING( "Sound %d row", r );
		REAL	pos[3];
		pos[i_u]= u;
		REAL v = _origin[i_v] - REAL(.5) * bdd_size[1] + dv * REAL(.5);

		INT32	itmp = get_draw_row_index( r );

//		printf( "%d ", itmp );
		REAL*	pr = &_data[ itmp * _col_nb ] - 1;
		for( INT32 c = 0; c < _col_nb; ++c )
		{

			//DBG_PRINT_STRING( "%d", c );
			
			REAL tmp = process_max( *++pr, c );

			REAL size_axe =  daxe * ( _size_offset[2] + _size[2] * tmp );
			REAL loc_size[3];
			loc_size[i_u]	=	du		*	( _size_offset[0] + _size[0] * tmp );
			loc_size[i_v]	=	dv		*	( _size_offset[1] + _size[1] * tmp );
			loc_size[i_axe] =	size_axe;
			if( is_not_null_v3( loc_size ) )
			{
				pos[i_axe]		=	_origin[i_axe]	+	size_axe * _size_direction[2] * REAL(.5);
				if( _b_cylinder_ui )
				{
					GOL::matrix::push();
						GOL::matrix::rotate_z( v * REAL(.5) );
						GOL::matrix::translate_y( REAL(2) );
						pos[i_v]= 0;

						c_multiple::cur->align_then_draw( pos, loc_size );
					GOL::matrix::pop();
				}
				else
				{
					pos[i_v]= v;
					c_multiple::cur->align_then_draw( pos, loc_size );
				}
			}

			c_multiple::cur->next_index_u();
			v += dv;
		}
		c_multiple::cur->next_index_v();
		u += du;
	}

//	printf( "\n", tmp );
}

/*
FINLINE void c_bdd_uv::draw_low_multiple()
{
REAL*	c = point_to_draw;
INT32	u;
INT32	v;

		for( u = nb_u; u>=0; --u )
			{
			c_tex_anim::cur->bind_this( u );
			for( v = nb_v; v>=0; --v )
				{
				c_multiple::cur->align_then_draw( c );
				multiple_index_next();
				c += 3;
				}
			}
		}
}
*/
