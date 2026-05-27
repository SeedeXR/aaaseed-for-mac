#include "def_data.h"
#include "math/gainbias.h"
#include "media/sound/snd_master.h"
#include "media/sound/snd_input.h"
#include "ui/strsymbo.h"
#include "math/v.h"
#include "infrastructure/data/data_src.h"
#include "infrastructure/param/param_declare.h"

namespace array
{
	template<class T>
	CONSTEXPR void abs( T * p, INT32 nb ) NOEXCEPT
	{
		for( ; nb>0; --nb )
		{
			T v = *p;
			if( v < T(0) )
				*p = -v;
			++p;
		}
	}

	template<class T>
	CONSTEXPR void abs_to_value( T * p, INT32 nb, T th ) NOEXCEPT
	{
		for( ; nb>0; --nb )
		{
			T v = *p;
			if( v >= T(0) )
				*p = v>th ? v-th : 0;
			else
				*p = v<-th ? -v-th : 0;
			++p;
		}
	}

	template<class T>
	CONSTEXPR void clamp_0( T * p, INT32 nb ) NOEXCEPT
	{
		for( ; nb>0; --nb )
		{
			if( *p < T(0) )
				*p = T(0);
			++p;
		}
	}

	template<class T>
	CONSTEXPR void clamp_to_value( T * p, INT32 nb, T th ) NOEXCEPT
	{
		for( ; nb>0; --nb )
		{
			T v = *p;
			if( v >= th )
				*p = v-th;
			else
				*p = 0;
			++p;
		}
	}

	template<class T>
	CONSTEXPR void mul_add( T * p, INT32 nb, T CONST f, T CONST a ) NOEXCEPT
	{
		for( ; nb>0; --nb )
		{
			T v = *p;
			*p = v*f + a;
			++p;
		}
	}

	template<class T>
	CONSTEXPR void gainbias( T* p, INT32 nb, REAL gain_in, REAL bias_in, T min, T max )
	{
		if( min != max )
		{
			T dif = max - min;
			T fac = REAL(1.) / dif;
			for( INT32 i=nb; i>0; --i )
			{
				T f = *p;
				f = (f-min) * fac;
				gain( f, gain_in );
				bias( f, bias_in );
				*p++ = f * dif + min;
			}
		}
	}
	template<class T>
	CONSTEXPR void pow( REAL* p, INT32 nb, REAL exp )
	{
		--p;
		for( INT32 i=nb; i>0; --i )
		{
			T f = *++p;
			if( f < T(0) )
				*p = -T(POW( -f, exp ));
			else
				*p = T(POW( f, exp ));
		}	
	}

}

FACTORY_INSTANCE_V1( c_def_data, def_data, Deformer Data, def );

static C_PCHAR_C str_data_filter[ c_def_data::DATA_FILTER_TYPE_MAX_NB ] =
{
	"No",
	"Envelope",
	"Average_2",
	"Average_3",
	"Average_4",
	"Average_5",
	"Average_6",
	"Average_7",
	"Average_8",
};
static C_PCHAR_C str_data_abs_clamp[ c_def_data::DATA_ABS_CLAMP_MAX_NB ] =
{
	"No",
	"Abs",
	"Clamp_0",
};
namespace n_def_data
{
	enum PROCESS_TYPE : INT32
	{
		PROCESS_DIRECT = 0,
		PROCESS_AVERAGE,
		PROCESS_MAX,
		PROCESS_MAX_WITH_DECAY,
		PROCESS_INTERPOLATE,
		PROCESS_TYPE_MAX_NB,
	};
	
	C_PCHAR_C	str_process[PROCESS_TYPE_MAX_NB] =
	{
		"No",
		"Average",
		"Max",
		"Max with decay",
		"Interpolate",
	};

	CONSTEXPR INT32 BASE_PARAM_NB	= 24 + 	c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 ORIGIN_PARAM_NB	= 3;
	CONSTEXPR INT32 GROUP_NB		= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	ORIGIN_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_SYMBO_MIN_MAX(	data_src,				1, 3,		1, 3 /*DATA_SRC_TYPE_NB - 1*/, gstr_data_src )
		PARAM_DEF_INT32(			channel,				2,	1,		1, c_snd_input::SOUND_CHANNEL_NB_MAX	)		//hack should we define a MAX ?
		PARAM_DEF_INT32(			sample_nb,				1,	512,	1, c_def_data::SAMPLE_NB_MAX				)

		PARAM_DEF_AXE_X(			axe_src					)
		PARAM_DEF_AXE_Y(			axe_dst					)
		PARAM_DEF_REAL_ONE(			scale					)

		PARAM_DEF_REAL_ZERO(		strenght				)
		PARAM_DEF_REAL_ONE(			offset					)

		PARAM_DEF_SYMBO_PSTR_ZERO(	abs_clamp,				str_data_abs_clamp	)
		PARAM_DEF_REAL_ZERO(		abs_clamp_value			)

		PARAM_DEF_SYMBO_PSTR_ZERO(	data_filter,			str_data_filter	)
		PARAM_DEF_REAL_ZERO(		data_filter_value		)

		//		PARAM_DEF_REAL_ONE(			exponent				)
		PARAM_DEF_SYMBO_PSTR_ZERO(	sound_process,			str_process )
		PARAM_DEF_INT32(			interval,				0,	1,		1, c_def_data::BUFFER_NB_MAX	)
		PARAM_DEF_INT32_POS_ZERO(	strobe					)

		PARAM_DEF_GAIN(				shape_gain				)
		PARAM_DEF_BIAS(				shape_bias				)

//		PARAM_DEF_BOOL_OFF(			clamp_zero				)
		PARAM_DEF_BOOL_OFF(			force_zero				)
		PARAM_DEF_BOOL_OFF(			abs						)


		PARAM_DEF_BOOL_OFF(			lissajou				)
		PARAM_DEF_INT32(			lissajou_channel,		1,	2,		1, c_snd_input::SOUND_CHANNEL_NB_MAX	)		//hack should we define a MAX ?
		PARAM_DEF_BOOL_OFF(			lissajou_fake			)
		PARAM_DEF_REAL_ZERO(		lissajou_src_factor		)
		PARAM_DEF_INT32_INF(		lissajou_fake_interval,	0,	128		)

		PARAM_DEF_GROUP(			Origin,	ORIGIN_PARAM_NB )
			PARAM_DEF_POINT_XYZ(		origin	)
	};
}



CONSTRUCTOR_CREATE(c_def_data)
	,_channel			(1)
	,_lissa_channel		(2)
{
	init_name_with( "Data" );
	param_init_with( n_def_data::param, n_def_data::PARAM_NB_MAX);
	init();
}

void c_def_data::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_attach_obj_no_inc( h, snd::g_master->get_by_channel( _channel ) );
	param_set_pt( h, _s_data_src	);
	param_set_pt( h, _channel		);
	param_set_pt( h, _sample_nb		);

	param_set_pt( h, _src_axe		);
	param_set_pt( h, _dst_axe		);
	param_set_pt( h, _src_scale		);

	param_set_pt( h, _data_factor );
	param_set_pt( h, _data_offset );

	param_set_pt( h, _s_data_abs_clamp		);
	param_set_pt( h, _abs_clamp_th			);

	param_set_pt( h, _s_data_filter_type	);
	param_set_pt( h, _data_filter_value		);

	param_set_pt( h, _s_process_type );
	param_set_pt( h, _average_count );
	param_set_pt( h, _skip );

	//	param_set_pt( h, snd_exp_ );
	param_set_pt( h, _shape_gain );
	param_set_pt( h, _shape_bias );

//	param_set_pt( h, _b_clamp_0 );
	param_set_pt( h, _b_force_0 );
	param_set_pt( h, _b_range );


	param_set_pt( h, _b_lissa			);
	param_attach_obj_no_inc( h, snd::g_master->get_by_channel( _lissa_channel ) );
	param_set_pt( h, _lissa_channel		);
	param_set_pt( h, _b_lissa_fake		);
	param_set_pt( h, lissa_src_factor_	);
	param_set_pt( h, _lissa_fake_offset	);
	
	++h;
		param_set_pt_3( h, _origin );

	err_param_init_pt(h);
}

void c_def_data::init()
{
	_count_in = 0;
	count_a_last_ = 0;
	count_b_last_ = 0;
	interval_last_ = 1;
}

c_def_data::~c_def_data()
{
}

//todo not really convincing
void	c_def_data::filter_envelope( REAL* pt, INT32 nb )
{
	REAL fl = _data_filter_value;
	REAL fc = REAL(1.) - fl;

	REAL vl = abs(*pt);
	for( INT32 i=nb-1; i>0; --i )
	{
		vl = fc * abs(*++pt) + fl * vl;
		*pt = vl;
	}
}

void	c_def_data::filter_average( REAL* pt, INT32 nb, INT32 nb_ave )
{
	REAL	acc;
	REAL	over;
	REAL*	in;

	over = REAL(1.)/nb_ave;
	
	for( INT32 i=nb-nb_ave; i>0; --i )
	{
		in = pt;
		acc = 0.;
		for( INT32 j=nb_ave; j>0; --j )
			acc += *in++;
		*pt = acc * over;
		++pt;
	}
	for( INT32 i=nb_ave; i>0; --i )
	{
		*pt = *(pt-1);
		++pt;
	}	
}

void	c_def_data::filter( REAL* pt, INT32 nb, DATA_FILTER_TYPE CONST type )
{
	if( _s_data_filter_type == DATA_FILTER_ENVELOPE )
		filter_envelope( pt, nb );
	else
		filter_average( pt, nb, _s_data_filter_type );
}


void	c_def_data::update()
{
	update_channel( _result[0], 0, _channel );

	if( _b_lissa && !!_b_lissa_fake )
		update_channel( _result[1], 1, _lissa_channel );

	++_count_in;
	set_deforming( true );
}

void	c_def_data::update_channel( REAL* dst, INT32 index, INT32 ch )
{
	//	prepare the axes
//	axe_build_index( i_u, i_v, i_axe);
//	scale_v3r( size, size_ui, size_ui[3] );
	c_snd_input* si = snd::g_master->get_by_channel( ch );
	if( si )
	{
		INT32	c = IMOD( _count_in, BUFFER_NB_MAX );
		auto data_src =  _data[index][c];
		INT32	type_to_do;
		switch( _s_data_src )
		{
/*			case SOUND_SPECTRUM_RAW:
			{
			c_snd_input* si;
			if( si = c_snd_input::get(channel_ ) )
				si->get_spectrum_raw_band( data_[index][c], sample_nb_, ch, 0, 1, true, false);	//todo b_linear_, false );
			}
			break;
*/
		case SOUND_SPECTRUM_LINEAR:
		case SOUND_SPECTRUM:
			si->get_spectrum_band( data_src, _sample_nb, ch, 0., 1., _s_data_src==SOUND_SPECTRUM_LINEAR );
			break;
		case SOUND_WAVE:
			c_snd_input::general_get_wave( data_src, _sample_nb, ch );
			break;
		}
//todo other types
/*
		case SOUND_SPECTRUM_RAW:
				col_nb_ = snd_input_->get_spectrum_raw_band( dst, channel_ != 0 ); // c_snd_input::get( channel_ )->spectrum_band_nb_get();		
				break;
		case SOUND_WAVE:
			//				col_nb_ = snd_input_->general_get_wave( dst, col_nb_, channel_ );
			col_nb_ = snd_input_->get_wave( dst, col_nb_, ( channel_ - 1 ) & 1 );

			//if ( c_snd_input::get( channel_ ) )
			//	col_nb_ = c_snd_input::general_get_wave( dst, col_nb_, channel_ );
			break;
		case MIDI_CONTROL:
			midi_control_get_band( col_start_, col_nb_, dst );
			break;
		case MIDI_VELOCITY:
			midi_velocity_get_band( col_start_, col_nb_, dst );
			break;
		case DATACUBE: 
			{
				for( INT32 r = 0; r < nb_row_; ++r )
					for( INT32 c = 0; c < col_nb_; ++c )
						band_buf_[r * band_nb_ + c] = datacube_def.get_real( 1, r + 1, c + 1);
			}
				break;
*/
		array::mul_add( data_src, _sample_nb, _data_factor, _data_offset );

		if( _s_data_abs_clamp != DATA_ABS_CLAMP_NO )
		{
			if( _s_data_abs_clamp == DATA_ABS )
				array::abs_to_value( data_src, _sample_nb, _abs_clamp_th );
			else
				array::clamp_to_value( data_src, _sample_nb, _abs_clamp_th );
		}

		if( _s_data_filter_type )
			filter( data_src, _sample_nb, _s_data_filter_type );

		if( _s_process_type == n_def_data::PROCESS_INTERPOLATE && _average_count == 0 )
			type_to_do = n_def_data::PROCESS_DIRECT;
		else
			type_to_do = _s_process_type;

		switch( type_to_do )
		{
		case n_def_data::PROCESS_INTERPOLATE:
			{
//				REAL*	src;
//				REAL*	dst;
				INT32	ia,ib;
				REAL r;
				c = _count_in;
				if( c > count_b_last_ + _average_count )
				{
					count_a_last_ = count_b_last_;
					count_b_last_ = c;
				}
				ia = IMOD( count_a_last_, BUFFER_NB_MAX);
				ib = IMOD( count_b_last_, BUFFER_NB_MAX);
				r = REAL( _count_in-count_b_last_ )/REAL(count_b_last_-count_a_last_ );
				interpolate_v1r_list( dst, _data[index][ia], _data[index][ib], r, _sample_nb );
			}
			break;
		case n_def_data::PROCESS_AVERAGE:
			if( _skip == 0 || IMOD( _count_in, _skip+1) == 0 )
			{
				MEMCPY( dst, data_src, _sample_nb*sizeof(REAL), __FUNCTION__ );
//				REAL r;
				for( INT32 i=_average_count-1; i>0; --i)
				{
					c = IMOD(c-1,BUFFER_NB_MAX);
					REAL*	pdata = _data[index][c]-1;
					REAL*	pdst = dst-1;
					for( INT32 j=_sample_nb; j>0; --j)
						*++pdst += *++pdata;
				}
				if( _average_count > 1)
				{
					REAL over = REAL(1.)/REAL(_average_count );
					REAL*	pdst = dst-1;
					for( INT32 j=_sample_nb; j>0; --j)
						*++pdst *= over;
				}
			}
			break;
		case n_def_data::PROCESS_MAX:
			if( _skip == 0 || IMOD( _count_in, _skip+1) == 0 )
			{
				MEMCPY( dst, data_src, _sample_nb*sizeof(REAL), __FUNCTION__ );
//					REAL	min;
				for( INT32 i=_average_count-1; i>0; --i)
				{
					c = IMOD(c-1,BUFFER_NB_MAX);
					REAL*	pdata = _data[index][c]-1;
					REAL*	pdst = dst;
					for( INT32 j=_sample_nb; j>0; --j)
					{
						*pdst = MAX( *pdst, *++pdata);
						++pdst;
					}
				}
//				min = get_min_of_list_real( dst, sample_nb_ );
//				add_to_list_real( dst, sample_nb_, -min );
			}
			break;
		case n_def_data::PROCESS_MAX_WITH_DECAY:
			if( _skip == 0 || IMOD( _count_in, _skip+1) == 0 )
			{
				MEMCPY( dst, data_src, _sample_nb*sizeof(REAL), __FUNCTION__ );
				REAL r;
//					REAL	min;
				for( INT32 i=_average_count-1; i>0; --i)
				{
					r = REAL(i)/REAL(_average_count );
					c = IMOD(c-1,BUFFER_NB_MAX);
					REAL*	pdata = _data[index][c]-1;
					REAL*	pdst = dst;
					for( INT32 j=_sample_nb; j>0; --j)
					{
						*pdst = MAX( *pdst, *++pdata*r);
						++pdst;
					}
				}
//				min = get_min_of_list_real( dst, sample_nb_ );
//				add_to_list_real( dst, sample_nb_, -min );
			}
			break;
		default:
		case n_def_data::PROCESS_DIRECT:
			MEMCPY( dst, data_src, _sample_nb*sizeof(REAL), __FUNCTION__ );
			break;
		}

		if( (_shape_gain!=.5) || (_shape_bias!=.5) )
		{
			REAL	min;
			REAL	max;
			get_min_max_of_array( min, max, dst, _sample_nb );
			//pow( dst, sample_nb_, snd_exp );
			array::gainbias( dst, _sample_nb, _shape_gain, _shape_bias, min, max);
		}
	}
}


void	c_def_data::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	//REAL	s_u, s_v;
	//REAL	s;
	REAL	s;
	INT32	index;

	REAL CONST *	src_u = src + _src_axe;
	REAL*			dst_a = dst + _dst_axe;

	REAL	o_u = _origin[_src_axe];	//todo axe ?

	if( _b_lissa )
	{
		REAL*	dst_u = dst + IMOD( _dst_axe+1, 3 );
		REAL*	dst_v = dst + IMOD( _dst_axe+2 ,3 );
		REAL	u;
		REAL	v;
		for( ; nb>0; --nb )
		{
			s = (*src_u - o_u)*_src_scale;
			index = IMOD( INT32( s*_sample_nb ), _sample_nb );
			//skew
			cpy_v3( dst, src );

			u = _result[0][index];
			if( _b_lissa_fake )
				v = _result[0][IMOD( index + _lissa_fake_offset, _sample_nb )];
			else
				v = _result[1][index];

			*dst_u = *dst_u * lissa_src_factor_ + u;	// * _strenght + _offset;
			*dst_v = *dst_v * lissa_src_factor_ + v;	// * _strenght + _offset;

			//next one
			src += 3;
			src_u += 3;

			dst += 3;
			dst_u += 3;
			dst_v += 3;
		}
	}
	else
	{
		REAL r;
		for( ; nb>0; --nb )
		{
			//s = (*src_u - o_u)*_src_scale;
			s = (*src_u-o_u)*_src_scale;
			if( _s_data_src == SOUND_WAVE )	// we want sound like signal on an oscilloscope as default
				s = -s;
			index = IMOD( INT32(s*_sample_nb - .000001), _sample_nb );	//avoid going back to start when len correspond to scale

				
			cpy_v3( dst, src );
			r = _result[0][index];

			REAL d = *dst_a;
			//if( _b_force_0  )

			if( _b_force_0 && abs(r) < .01 )
				d = 0;
			else if( _b_range )
			{
				if( r >= 0. )
				{
					if( d >= 0. )
						d = d+r;
					else
						d = d-r;
				}
				else
				{
					if( d >= 0. )
						d = -d+r;
					else
						d = -d-r;	
				}
			}
			else
				d += r;
			//else
			//{
			//	if( !_b_clamp_0 || r>0. )
			//		*dst_a += r;
			//}

			*dst_a = d;

			//next one
			src += 3;
			src_u += 3;

			dst += 3;
			dst_a += 3;
		}
	}
}
