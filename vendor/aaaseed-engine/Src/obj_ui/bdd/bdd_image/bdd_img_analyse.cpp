#include "bdd_img_analyse.h"
#include "image/bind_img_2d.h"
#include "draw/model.h"
#include "draw/rect.h"
#include "draw/line.h"
#include "draw/shape.h"
#include "obj_ui/bdd/bdd_ui_pref.h"
#include "image/convert/color_space.h"
#include "image/img_compo.h"

//todo generalize the common base for different algorithm s
FACTORY_CREATE_PROP_V1( c_bdd_img_analyse, bdd_img_analyse, Image analyse , image_analyse, sub_menu="Image"; );

namespace n_bdd_img_analyse
{
	CONSTEXPR INT32 BASE_NB_MAX			=	c_bdd::GEO_PARAM_NB + 14;
	CONSTEXPR INT32 WHERE_NB_MAX		=	10;
	CONSTEXPR INT32 DRAW_NB_MAX			=	6;
	CONSTEXPR INT32 OUT_NB_MAX			=	6;
	CONSTEXPR INT32 OUT_PIXEL_NB_MAX	=	6;
	CONSTEXPR INT32 MAX_NB_MAX			=	7;
	CONSTEXPR INT32 GROUP_NB_MAX		=	5;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	WHERE_NB_MAX
									+	DRAW_NB_MAX
									+	OUT_NB_MAX
									+	OUT_PIXEL_NB_MAX
									+	MAX_NB_MAX
									+	GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

			PARAM_DEF_BOOL_OFF(				active			)
			PARAM_DEF_BIND_2D_CURRENT_SEL(	image_src		)
			PARAM_DEF_BANK_BIND_2D_OUT(		bind_out		)
			PARAM_DEF_BOOL_OFF(				force_compute	)
			PARAM_DEF_INT32_LOCKED(			process_index	)

			PARAM_DEF_GROUP( Where, WHERE_NB_MAX )
				PARAM_DEF_SYMBO_PSTR_ZERO(	image_component,	aaa::str_compo )
				PARAM_DEF_REAL_ZERO(	begin_u				)
				PARAM_DEF_REAL_ZERO(	begin_v				)
				PARAM_DEF_REAL_ONE(		end_u				)
				PARAM_DEF_REAL_ONE(		end_v				)	
				PARAM_DEF_BOOL_OFF(		by_size				)
				PARAM_DEF_REAL_ONE(		size_u				)
				PARAM_DEF_REAL_ONE(		size_v				)
				PARAM_DEF_BOOL_OFF(		discrete			)
				//PARAM_DEF_INT32(		discrete_nb_u,		0,3,	0,2048*16 )
				PARAM_DEF_INT32(		discrete_skip_line,	0,3,	0,2048*16	)

			PARAM_DEF_GROUP( Draw, DRAW_NB_MAX )
				PARAM_DEF_NONE(			bdd_ui_pref		)
				PARAM_DEF_BOOL_OFF(		draw_texture	)
				PARAM_DEF_BOOL_OFF(		draw_frame		)
				PARAM_DEF_BOOL_ON(		draw_frame_in	)
				PARAM_DEF_BOOL_ON(		draw_corosshair )
				PARAM_DEF_BOOL_ON(		draw_center		)

			PARAM_DEF_BOOL_OFF(			find_rect		)
			PARAM_DEF_REAL_POS_ZERO(	threshold		)
			PARAM_DEF_REAL_POS_ONE(		threshold_max	)
			PARAM_DEF_REAL_ZERO(		coverage_min	)
			PARAM_DEF_REAL_ONE(			coverage_max	)

			PARAM_DEF_REAL_LOCKED(		out_coverage_raw	)
			PARAM_DEF_REAL_LOCKED(		out_coverage		)
			PARAM_DEF_REAL_LOCKED(		out_center_u		)
			PARAM_DEF_REAL_LOCKED(		out_center_v		)
		
			PARAM_DEF_GROUP( Out, OUT_NB_MAX )
				PARAM_DEF_REAL_LOCKED(	out_min_u	)
				PARAM_DEF_REAL_LOCKED(	out_min_v	)
				PARAM_DEF_REAL_LOCKED(	out_max_u	)
				PARAM_DEF_REAL_LOCKED(	out_max_v	)
				PARAM_DEF_REAL_LOCKED(	out_size_u	)
				PARAM_DEF_REAL_LOCKED(	out_size_v	)

			PARAM_DEF_GROUP( Out Pixel, OUT_PIXEL_NB_MAX )
				PARAM_DEF_INT32_LOCKED(	out_min_x	)
				PARAM_DEF_INT32_LOCKED( out_min_y	)
				PARAM_DEF_INT32_LOCKED( out_max_x	)
				PARAM_DEF_INT32_LOCKED( out_max_y	)
				PARAM_DEF_INT32_LOCKED( out_size_x	)
				PARAM_DEF_INT32_LOCKED( out_size_y	)

			PARAM_DEF_GROUP( Max Stuff, MAX_NB_MAX )
				PARAM_DEF_BOOL_OFF(		find_max				)
				PARAM_DEF_REAL(			find_max_skew_y,		0,.1,	-1,1	)
				PARAM_DEF_REAL(			find_max_refine_range,	0,.1,	0,1		)
				PARAM_DEF_REAL_LOCKED(	max_u					)
				PARAM_DEF_REAL_LOCKED(	max_v					)
				PARAM_DEF_REAL_LOCKED(	max_grey				)
				PARAM_DEF_REAL_LOCKED(	max_coverage			)
	};
}

void	c_bdd_img_analyse::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, get_pt_active()			);
	param_set_pt( h, get_src_img_index_ui_pt()	);
	param_set_pt( h, _src_img_index				);
	param_set_pt( h, _b_force_compute_ui		);
	param_set_pt( h, get_process_index_pt()		);

	++h;
		param_set_pt(	h, _compo_src_ui		);
		param_set_pt_2(	h, _anal_begin_ui		);
		param_set_pt_2(	h, _anal_end_ui			);
		param_set_pt(	h, _b_by_size_ui		);
		param_set_pt_2(	h, _anal_size_ui		);
		param_set_pt(	h, _b_anal_discrete_ui	);
		param_set_pt(	h, _anal_step_nb_ui[1]	);

	++h;
		param_attach_obj(	h, c_bdd_ui_pref::cur	);
		param_set_pt(		h, _b_draw_texture		);
		param_set_pt(		h, _b_draw_frame		);
		param_set_pt(		h, _b_draw_frame_sub	);
		param_set_pt(		h, _b_draw_crosshair	);
		param_set_pt(		h, _b_draw_center		);

	param_set_pt(	h, _b_find_rect_ui		);
	param_set_pt(	h, _threshold_min_ui	);
	param_set_pt(	h, _threshold_max_ui	);
	param_set_pt(	h, _coverage_min		);
	param_set_pt(	h, _coverage_max		);

	param_set_pt(	h, _coverage_raw_out	);
	param_set_pt(	h, _coverage_out		);
	param_set_pt_2(	h, _center				);
		
	++h;
		param_set_pt_2(	h, _min			);
		param_set_pt_2(	h, _max			);
		param_set_pt_2(	h, _size		);

	++h;
		param_set_pt_2(	h, _min_pixel	);
		param_set_pt_2(	h, _max_pixel	);
		param_set_pt_2(	h, _size_pixel	);

	++h;
		param_set_pt(	h, _b_find_max				);
		param_set_pt(	h, _find_max_skew_y			);
		param_set_pt(	h, _find_max_refine_range	);
		param_set_pt_2(	h, _max_pos					);
		param_set_pt(	h, _max_max					);
		param_set_pt(	h, _max_coverage			);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_img_analyse)
,_max_max(0)
,_src_img_index(0)
{
	_coverage_raw_out = 0.;
	_coverage_out = 0.;
	clear_v2(  _center );

	clear_v2( _min );
	clear_v2( _max );
	set_v2( _size, REAL(0) );

	clear_v2( _min_pixel );
	clear_v2( _max_pixel );
	set_v2( _size_pixel, 0 );

	clear_v2( _max_pos );
	param_init_with( n_bdd_img_analyse::param, n_bdd_img_analyse::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR(c_bdd_img_analyse)

//todo deal with error
//todo deal with INT16, FP16, FP2 
void c_bdd_img_analyse::update()
{
	if( !is_active() )
		return;

	_src_img_index = g_bind_img_2d->build_index_from_param( get_src_img_index_ui() );
	c_img_2d* img = g_bind_img_2d->get_ready( _src_img_index );
	
	if( !img )
		return;

	if( !process_img_changed(img) && !_b_force_compute_ui )
		return;

	for( INT32 i=0; i<2; ++i )
	{
		REAL b, e;
		b = _anal_begin_ui[i];
		if( _b_by_size_ui )
		{
			e = b + _anal_size_ui[i];
			_anal_end_ui[i] = e;
		}
		else
		{
			e = _anal_end_ui[i];
			_anal_size_ui[i] = e - b;
		}

		CLAMP_REF_01( b );
		CLAMP_REF_01( e );

		if( b <= e )
		{
			_anal_begin[i] = b;
			_anal_size[i] = e - b;
		}
		else
		{
			_anal_begin[i] = e;
			_anal_size[i] = b - e;
		}
	}

	if( !img->is_data_valid( __FUNCTION__ ) )
	{
		err_print( "%s() image don't have valid data on the cpu side", __FUNCTION__ );
		return;
	}
	UINT8*	pt = img->get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !pt )
	{	//	should add a grab here
//		err_print( "%s() image don't have usable data on the cpu side", __FUNCTION__ );
		return;
	}

	bool b_ok;
	if( _b_find_max )
		b_ok = find_max(	img,	_compo_src_ui );
	else
		b_ok = compute(		img,	_compo_src_ui );
	if( !b_ok )
		err_print( "%s() can do with this image and this component", __FUNCTION__ );
}

#undef MIN
#undef MAX

#define FIND_MAX_BEGIN(str) 										\
{																	\
	for( INT32 iy=0; iy<nby; ++iy )									\
	{																\
		INT32	skew = INT32(( iy + oy - (sy>>1) )  * skew_factor);	\
		INT32	line_max = max - skew;								\
		for( INT32 ix=nbx; ix>0; --ix )								\
		{															\
			INT32 val = (str);

#define FIND_MAX_END											\
			pt += pixel_inc;									\
		}														\
		pt += line_stride;										\
	}															\
}

#define FIND_MAX_LOOP( str ) 									\
	FIND_MAX_BEGIN(str)											\
			if( *pt > line_max )								\
			{													\
				++nb;											\
				u += ix;										\
				v += iy;										\
				max_max += *pt + skew;							\
			}													\
	FIND_MAX_END

#define FIND_MAX_LOOP2( str )									\
	FIND_MAX_BEGIN(str)											\
			if( val < line_max )								\
			{													\
			}													\
			else if( val > line_max )							\
			{													\
				max = INT32(val + skew);						\
				u = ix;											\
				v = iy;											\
				nb = 1;											\
			}													\
			else												\
			{													\
				++nb;											\
				u += ix;										\
				v += iy;										\
			}													\
	FIND_MAX_END


//todoq deal with UINT32 directly
//todo line stride and skew_factor are probably wrong
bool c_bdd_img_analyse::find_max( c_img_2d *img, aaa::COMPO src_compo )
{
	UINT8*	pt_start = img->get_data_uint8();
	INT32	oa;
	if( src_compo <= aaa::COMPO::ALPHA )
	{
		if( !img->make_compo_index( src_compo, oa ) )
			return false;
		pt_start += oa;
	}

	INT32	sx = img->get_size_x();
	INT32	sy = img->get_size_y();

	INT32	ox	= INT32(sx * _anal_begin[0]);
	INT32	oy	= INT32(sy * _anal_begin[1]);
	INT32	nbx	= INT32(sx * _anal_size[0] );
	INT32	nby	= INT32(sy * _anal_size[1] );

	INT32	pixel_inc	= img->get_byte_per_pixel();
	INT32	line_stride	= img->get_byte_pitch();

	pt_start += line_stride	* oy;	//skip firsts lines
	pt_start += pixel_inc	* ox;	//skip first colums

	INT32 y_step = 1;
	if( _b_anal_discrete_ui )
	{
		y_step += _anal_step_nb_ui[1];
		INT32 nb = nby / y_step;
		if( nb > 0 )
		{
			nby = nb;
			line_stride *= y_step;
		}
	}
	line_stride -= pixel_inc * nbx;

	REAL	skew_factor = (REAL(256) * _find_max_skew_y ) / sy;

	INT32	nb = 0;
	INT32	max = -256;
	UINT8*	pt = pt_start;
	UINT32	u = 0;
	UINT32	v = 0;

	if( _find_max_refine_range != 0. )
	{
		for( INT32 iy=0; iy<nby; ++iy )
		{
			INT32	skew = INT32( ( iy * y_step + oy - (sy>>1) ) * skew_factor );
			INT32	line_max = max - skew;
			for( INT32 ix=nbx; ix>0; --ix )
			{
				if( *pt > line_max )
				{
					max = *pt + skew;
					line_max = *pt;
				}
				pt += pixel_inc;
			}
			pt += line_stride;
		}
		
		max -= MAX0( INT32( _find_max_refine_range * 256. ) );
		pt = pt_start;
		INT32	max_max = 0;
		if( src_compo <= aaa::COMPO::ALPHA )
			FIND_MAX_LOOP( *pt )
		else
		{
			if( src_compo == aaa::COMPO::MAX_COMPO )
			{
				switch( img->get_channel_nb() )
				{
				case 1:		FIND_MAX_LOOP( *(pt+0) );	break;
				case 2:		FIND_MAX_LOOP( MAX( *(pt+0), *(pt+1) ) );	break;
				case 3:
				case 4:
				default:	FIND_MAX_LOOP( aaa::MAX( *(pt+0), *(pt+1), *(pt+2) ) );	break;
				}
			}
			else if( src_compo == aaa::COMPO::MIN_COMPO )
				switch( img->get_channel_nb() )
				{
				case 1:		FIND_MAX_LOOP( *(pt+0) );	break;
				case 2:		FIND_MAX_LOOP( MIN( *(pt+0), *(pt+1) ) );	break;
				case 3:
				case 4:
				default:	FIND_MAX_LOOP( aaa::MIN( *(pt+0), *(pt+1), *(pt+2) ) );	break;
				}
			else
			{
				INT32 ob;
				if( img->make_compo_index( src_compo, oa, ob ) )
				{
					if( src_compo == aaa::COMPO::GREY )
						FIND_MAX_LOOP( aaa::color::rgb_to_grey( *(pt+oa), *(pt+1), *(pt+ob) ) )
					else
						FIND_MAX_LOOP( *(pt+oa) - *(pt+ob) )
				}
				else
					return false;
			}
		}
		if( nb )
		{
			_max_max = max_max / REAL(nb);
			_max_pos[0] = ( nbx - u/REAL(nb) )	/ nbx;
			_max_pos[1] = ( v/REAL(nb) )		/ nby;
		}
		else
		{
			_max_max = 0;
			_max_pos[0] = 0;
			_max_pos[1] = 0;
		}
	}
	else
	{
		if( src_compo <= aaa::COMPO::ALPHA )
			FIND_MAX_LOOP2( *pt )
		else
		{
			if( src_compo == aaa::COMPO::MAX_COMPO )
			{
				switch( img->get_channel_nb() )
				{
				case 1:		FIND_MAX_LOOP2( *(pt+0) );	break;
				case 2:		FIND_MAX_LOOP2( MAX( *(pt+0), *(pt+1) ) );	break;
				case 3:
				case 4:
				default:	FIND_MAX_LOOP2( aaa::MAX( *(pt+0), *(pt+1), *(pt+2) ) );	break;
				}
			}
			else if( src_compo == aaa::COMPO::MIN_COMPO )
				switch( img->get_channel_nb() )
				{
				case 1:		FIND_MAX_LOOP2( *(pt+0) );	break;
				case 2:		FIND_MAX_LOOP2( MIN( *(pt+0), *(pt+1) ) );	break;
				case 3:
				case 4:
				default:	FIND_MAX_LOOP2( aaa::MIN( *(pt+0), *(pt+1), *(pt+2) ) );	break;
				}
			else
			{	
				INT32 ob;
				if( img->make_compo_index( src_compo, oa, ob ) )
				{
					if( src_compo == aaa::COMPO::GREY )
						FIND_MAX_LOOP2( aaa::color::rgb_to_grey( *(pt+oa), *(pt+1), *(pt+ob) ) )
					else
						FIND_MAX_LOOP2( *(pt+oa) - *(pt+ob) )
				}
				else
					return false;
			}
		}
		if( nb )
		{
			_max_pos[0] = REAL( ( nbx - u/DOUBLE(nb) )	/ nbx );
			_max_pos[1] = REAL( ( v/DOUBLE(nb) )		/ nby );
		}
		_max_max = REAL(max);
	}
	_max_max /= 255.;
	_max_coverage = REAL( DOUBLE(nb) / (nbx * nby) );
	return true;
}

#define FIND_RECT_LOOP( str )\
{\
	for( INT32 iy = 0; iy < nby; ++iy )\
	{\
		for( INT32 ix = 0; ix < nbx; ++ix )\
		{\
			UINT32 val = str ;\
			if( threshold_min_uint < val && val <= threshold_max_uint )\
			{\
				if( ix < min_u )\
					min_u = ix;\
				if( max_u < ix )\
					max_u = ix;\
				if( iy < min_v )\
					min_v = iy;\
				if( max_v < iy )\
					max_v = iy;\
			}\
			pt += pixel_inc;\
		}\
		pt += line_stride;\
	}\
}

#define COMPUTE_LOOP( str )\
{\
	for( INT32 iy = 0; iy < nby; ++iy )\
	{\
		for( INT32 ix = 0; ix < nbx; ++ix )\
		{\
			UINT32 val = str ;\
			if( threshold_min_uint < val && val <= threshold_max_uint )\
			{\
				DOUBLE tmp = DOUBLE( val - threshold_min_uint ) * threshold_size_over;\
				tot += tmp;	\
				tot_x += tmp*ix;\
				tot_y += tmp*iy;\
			}\
			pt += pixel_inc;\
		}\
		pt += line_stride;\
	}\
}

//todoq deal with UINT32 directly
bool c_bdd_img_analyse::compute( c_img_2d *img, aaa::COMPO src_compo )
{
	UINT8*	pt = img->get_data_uint8();
	INT32	oa;
	if( src_compo <= aaa::COMPO::ALPHA )
	{
		if( !img->make_compo_index( src_compo, oa ) )
			return false;
		pt += oa;
	}

	INT32	sx = img->get_size_x();
	INT32	sy = img->get_size_y();

	INT32	xp_begin	= INT32( sx * _anal_begin[0] );
	INT32	yp_begin	= INT32( sy * _anal_begin[1] );
//	INT32	xp_end		= INT32( sx * _anal_end[0]   );
//	INT32	yp_end		= INT32( sy * _anal_end[1]   );
	INT32	nbx			= INT32( sx * _anal_size[0]  );
	INT32	nby			= INT32( sy * _anal_size[1]  );

	INT32	pixel_inc	= img->get_byte_per_pixel();
	INT32	line_stride	= img->get_byte_pitch();

	pt += line_stride * yp_begin;	// skip first lines
	pt += pixel_inc   * xp_begin;	// skip first columns

	INT32 y_step = 1;
	//the whole interest of this is to be faster by doing less iteration
	// but we have to make sure we still compute correctly the result
	if( _b_anal_discrete_ui )
	{
		y_step += _anal_step_nb_ui[1];
		INT32 nb = nby / y_step;
		if( nb > 0 )
		{
			nby = nb;
			line_stride *= y_step;
		}
	}
	line_stride -= pixel_inc * nbx;

	//	analyse done on red if 32 bits or on grey if 8 bits
	DOUBLE	tot		=	0.;
	DOUBLE	tot_x	=	0.;
	DOUBLE	tot_y	=	0.;

	UINT32	threshold_min_uint	= UINT32( _threshold_min_ui	* aaa::img::REAL_NEARLY_256 );
	UINT32	threshold_max_uint	= UINT32( _threshold_max_ui * aaa::img::REAL_NEARLY_256 );
	DOUBLE	threshold_size_over	= 1. / (255. * (_threshold_max_ui-_threshold_min_ui) );

	// no skew here like in find_max
	if( _b_find_rect_ui )
	{
		//todo redo this algo by starting from side and finding the first point from there
		INT32 min_u = nbx;
		INT32 max_u = 0;
		INT32 min_v = nby;
		INT32 max_v = 0;
		if( src_compo <= aaa::COMPO::ALPHA )
			FIND_RECT_LOOP( *pt )
		else if( src_compo == aaa::COMPO::MAX_COMPO )
		{
			switch( img->get_channel_nb() )
			{
			case 1:		FIND_RECT_LOOP( *(pt+0) );	break;
			case 2:		FIND_RECT_LOOP( MAX( *(pt+0), *(pt+1) ) );	break;
			case 3:
			case 4:
			default:	FIND_RECT_LOOP( aaa::MAX( *(pt+0), *(pt+1), *(pt+2) ) );	break;
			}
		}
		else if( src_compo == aaa::COMPO::MIN_COMPO )
		{
			switch( img->get_channel_nb() )
			{
			case 1:		FIND_RECT_LOOP( *(pt+0) );	break;
			case 2:		FIND_RECT_LOOP( MIN( *(pt+0), *(pt+1) ) );	break;
			case 3:
			case 4:
			default:	FIND_RECT_LOOP( aaa::MIN( *(pt+0), *(pt+1), *(pt+2) ) );	break;
			}
		}
		else
		{
			INT32 ob;
			if( img->make_compo_index( src_compo, oa, ob ) )
			{
				if( src_compo == aaa::COMPO::GREY )
					FIND_RECT_LOOP( aaa::color::rgb_to_grey( *(pt+oa), *(pt+1), *(pt+ob) ) )
				else
					FIND_RECT_LOOP( *(pt+oa) - *(pt+ob) )
			}
			else
				return false;
		}

		if( min_u == nbx )	
		{	//we did not find anything
			_min_pixel[0] = 0;
			_min_pixel[1] = 0;
			_max_pixel[0] = 0;
			_max_pixel[1] = 0;
			_size_pixel[0] = 0;
			_size_pixel[1] = 0;
		}
		else
		{	
			_min_pixel[0] = min_u;
			_min_pixel[1] = yp_begin + min_v * y_step;
			_max_pixel[0] = max_u;
			_max_pixel[1] = yp_begin + max_v * y_step;
			_size_pixel[0] = _max_pixel[0] - _min_pixel[0] + 1;
			_size_pixel[1] = _max_pixel[1] - _min_pixel[1] + 1;
		}

		_min[0] = REAL( min_u / DOUBLE(nbx-1) );
		_min[1] = REAL( min_v / DOUBLE(nby-1) );
		_max[0] = REAL( max_u / DOUBLE(nbx-1) );
		_max[1] = REAL( max_v / DOUBLE(nby-1) );
		
		_center[0]	= (_max[0] + _min[0]) * REAL(.5);
		_center[1]	= (_max[1] + _min[1]) * REAL(.5);

		_size[0]	= _max[0] - _min[0];
		_size[1]	= _max[1] - _min[1];
	}
	else
	{
		if( src_compo <= aaa::COMPO::ALPHA )
			COMPUTE_LOOP( *pt )
		else if( src_compo == aaa::COMPO::MAX_COMPO )
		{
			switch( img->get_channel_nb() )
			{
			case 1:		COMPUTE_LOOP( *(pt+0) );	break;
			case 2:		COMPUTE_LOOP( MAX( *(pt+0), *(pt+1) ) );	break;
			case 3:
			case 4:
			default:	COMPUTE_LOOP( aaa::MAX( *(pt+0), *(pt+1), *(pt+2) ) );	break;
			}
		}
		else if( src_compo == aaa::COMPO::MIN_COMPO )
		{
			switch( img->get_channel_nb() )
			{
			case 1:		COMPUTE_LOOP( *(pt+0) );	break;
			case 2:		COMPUTE_LOOP( MIN( *(pt+0), *(pt+1) ) );	break;
			case 3:
			case 4:
			default:	COMPUTE_LOOP( aaa::MIN( *(pt+0), *(pt+1), *(pt+2) ) );	break;
			}
		}
		else
		{
			INT32 ob;
			if( img->make_compo_index( src_compo, oa, ob ) )
			{
				if( src_compo == aaa::COMPO::GREY )
					COMPUTE_LOOP( aaa::color::rgb_to_grey( *(pt+oa), *(pt+1), *(pt+ob) ) )
				else
					COMPUTE_LOOP( MAX0( *(pt+oa) - *(pt+ob) ) )
			}
			else
				return false;
		}
	
		if( tot )
		{
			tot_x /= tot;
			tot_y /= tot;
		}

		_center[0] = REAL( tot_x / DOUBLE(nbx) );
		_center[1] = REAL( tot_y / DOUBLE(nby) );

		tot /= DOUBLE( nbx * nby );
		_coverage_raw_out = REAL(tot);
		//todo generalize a fn for this
		if( _coverage_raw_out <= _coverage_min )
			_coverage_out = 0.;
		else if( _coverage_min != _coverage_max )
			_coverage_out = ( _coverage_raw_out - _coverage_min ) / ( _coverage_max - _coverage_min );
		else
			_coverage_out = 1.;
	}
	return true;
}

void	c_bdd_img_analyse::draw()
{
	if( !is_active() )
		return;
	
	REAL	u;
	REAL	v;
	REAL	size[3];
	INT32	axe;
	c_model::cur->get_size_v3( size );
	axe = c_model::cur->get_axe();

	if( _b_draw_texture )
		draw_rect_axe( size[0],size[1], axe );

	c_bdd_ui_pref::cur->begin();

	if( _b_draw_frame )
	{
		c_bdd_ui_pref::cur->set_drawing( c_bdd_ui_pref::COLOR_FRAME );
		draw_rect_line( size[0], size[1], axe );
	}
	if( _b_draw_frame_sub )
	{
		c_bdd_ui_pref::cur->set_drawing( c_bdd_ui_pref::COLOR_FRAME_SUB );
		draw_rect_line(		size[0] * ( _anal_begin[0] - REAL(.5)),
							size[1] * ( _anal_begin[1] - REAL(.5)),
							size[0] * ( _anal_begin[0] + _anal_size[0] - REAL(.5)),
							size[1] * ( _anal_begin[1] + _anal_size[1] - REAL(.5)),
					c_model::cur->get_axe() );
	}
	if( _b_find_max )
	{
		u = (_anal_begin[0] + _max_pos[0] * _anal_size[0] - REAL(.5)) * size[0];
		v = (_anal_begin[1] + _max_pos[1] * _anal_size[1] - REAL(.5)) * size[1];
		if( _b_draw_center )
		{
			c_bdd_ui_pref::cur->set_drawing( c_bdd_ui_pref::COLOR_OUTPUT );

			draw_plus_line_at(	u, v, 1., axe );
		}
	}
	else
	{
		if( _coverage_out > 0. )
		{
			u = (_anal_begin[0] + _center[0] * _anal_size[0] - REAL(.5)) * size[0];
			v = (_anal_begin[1] + _center[1] * _anal_size[1] - REAL(.5)) * size[1];
			if( _b_draw_crosshair )
			{
				c_bdd_ui_pref::cur->set_drawing( c_bdd_ui_pref::COLOR_CONSTRUCTION );
				draw_line_u( (_anal_begin[0]-REAL(.5)) * size[0],	v,									 _anal_size[0] * size[0], axe );
				draw_line_v( u,										(_anal_begin[1]-REAL(.5)) * size[1], _anal_size[1] * size[1], axe );
			}
			if( _b_draw_center )
			{
				c_bdd_ui_pref::cur->set_drawing( c_bdd_ui_pref::COLOR_OUTPUT );
				draw_plus_line_at(	u, v, MIN( _anal_size[0]*size[0], _anal_size[1]*size[1] )*_coverage_out, axe );
			}
		}
	}

	c_bdd_ui_pref::cur->end();
}

