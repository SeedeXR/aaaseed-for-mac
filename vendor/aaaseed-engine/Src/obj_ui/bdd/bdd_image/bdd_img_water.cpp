#include "bdd_img_water.h"
#include "image/bind_img_2d.h"
#include "math/rand.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_water, bdd_img_water, Image Water, image_water, sub_menu="Image"; );

FINLINE	INT32	make_pixel( REAL uv, INT32 size )
{
	if( uv == 1. )
		return size - 1;
	else
		return IMOD( INT32( uv * (REAL)size ), size );
}

namespace n_bdd_img_water
{
	CONSTEXPR INT32 BASE_PARAM_NB	= c_bdd::GEO_PARAM_NB + 17;
	CONSTEXPR INT32 IN_PARAM_NB		= 6;
	CONSTEXPR INT32 WAVE_PARAM_NB	= 2;
	CONSTEXPR INT32 CLAMP_PARAM_NB	= 2;
	CONSTEXPR INT32 DROP_PARAM_NB	= 3;
	CONSTEXPR INT32 OUT_PARAM_NB	= 2;
	CONSTEXPR INT32 GROUP_NB		= 5;	 
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
										+	IN_PARAM_NB
										+	WAVE_PARAM_NB
										+	CLAMP_PARAM_NB
										+	DROP_PARAM_NB
										+	OUT_PARAM_NB
										+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_IMG_SIZE(			size_x,			8, 128	)
		PARAM_DEF_IMG_SIZE(			size_y,			8, 128	)
		PARAM_DEF_BOOL_OFF(			restart_trig	)
		PARAM_DEF_BOOL_ON(			compute			)

		PARAM_DEF_GROUP(				"IN", IN_PARAM_NB )		
			PARAM_DEF_BOOL_ON(			image_update		)
			PARAM_DEF_BIND_2D_ALONE(	image_src			) //todo we should limit ?
			PARAM_DEF_SYMBO_PSTR(		image_component,	1, INT32(aaa::COMPO::GREY),		aaa::str_compo )\
			PARAM_DEF_BOOL_OFF(			image_clamped		)
			PARAM_DEF_REAL_ZERO(		image_threshold		)
			PARAM_DEF_REAL_ONE(			image_influence		)

		PARAM_DEF_GROUP(				DROP, DROP_PARAM_NB )		
			PARAM_DEF_BOOL_OFF(			drop_active		)
			PARAM_DEF_REAL_ONE(			drop_by_sec		)
			PARAM_DEF_REAL_ONE(			drop_strenght	)

		PARAM_DEF_BOOL_OFF(			method_5x5 )
		PARAM_DEF_REAL_ONE(			convolution_cross		)	
		PARAM_DEF_REAL_INF(			convolution_diag,		0, .5	)
		PARAM_DEF_REAL_INF(			convolution_cross_bis,	0, .25	)
		PARAM_DEF_REAL_INF(			convolution_diag2,		0, .1	)
		PARAM_DEF_REAL_ZERO(		maa_factor )

		PARAM_DEF_GROUP(				Wave, WAVE_PARAM_NB )
//			{	nullptr,	PARAM_REAL,	"wave_speed",	1,10,		0, PARAM_MAX_REAL, nullptr, nullptr },
			PARAM_DEF_REAL_ZERO(		weight_previous				)
			PARAM_DEF_REAL_ONE(			weight_previous_previous	)

		PARAM_DEF_REAL_ZERO(			damping						)
		PARAM_DEF_REAL_ONE_ZERO(		inter						)

		PARAM_DEF_GROUP(				"OUT", OUT_PARAM_NB )
			PARAM_DEF_REAL_LOCKED(		out_field_min	)
			PARAM_DEF_REAL_LOCKED(		out_field_max	)

		PARAM_DEF_GROUP(				CLAMP, CLAMP_PARAM_NB )
			PARAM_DEF_REAL_INF(			clamp_min,		0,-10	)
			PARAM_DEF_REAL_INF(			clamp_max,		0,10	)

		PARAM_DEF_BOOL_OFF(			img_out_float		)
		PARAM_DEF_BOOL_OFF(			img_out_with_alpha	)

		PARAM_DEF_REAL_ONE(			value_factor	)
		PARAM_DEF_BOOL_ON(			gradient		)
		PARAM_DEF_REAL_ONE(			gradient_factor	)
	};
}



CONSTRUCTOR_CREATE( c_bdd_img_water )
{
	init();
	param_init_with( n_bdd_img_water::param, n_bdd_img_water::PARAM_NB_MAX );
}

void c_bdd_img_water::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _grid_size_x_ui	);
	param_set_pt( h, _grid_size_y_ui	);
	param_set_pt( h, _b_restart_trig_ui	);
	param_set_pt( h, _b_compute_ui		);

	++h;
		param_set_pt( h, _b_img_update_ui );
		param_set_pt( h, _img_src_index_ui );
		param_set_pt( h, _s_img_src_compo_ui );
		param_set_pt( h, _b_img_clamped_ui );
		param_set_pt( h, _img_threshold_ui );
		param_set_pt( h, _img_influence_ui );
	++h;
		param_set_pt( h, _b_drop_ui );
		param_set_pt( h, _drop_by_sec_ui );
		param_set_pt( h, _drop_strenght_ui );

	param_set_pt( h, _b_method_complex_ui );
	param_set_pt( h, _conv_factor_ui[0] );
	param_set_pt( h, _conv_factor_ui[1] );
	param_set_pt( h, _conv_factor_ui[2] );
	param_set_pt( h, _conv_factor_ui[3] );
	param_set_pt( h, _maa_factor_ui );

	++h;
//todo		param_set_pt( h, _wave_speed );
		param_set_pt_2( h, _weighting_ui );
	
	param_set_pt( h, _damping_ui );
	param_set_pt( h, _inter_ui );

	++h;
		param_set_pt( h, _out_min );
		param_set_pt( h, _out_max );
	++h;
		param_set_pt( h, _clamp_min_ui );
		param_set_pt( h, _clamp_max_ui );

	param_set_pt( h, _b_img_out_float_ui );
	param_set_pt( h, _b_img_out_with_alpha_ui );

	param_set_pt( h, _value_factor_ui );
	param_set_pt( h, _b_gradient_ui );
	param_set_pt( h, _gradient_factor );

	err_param_init_pt( h );
}

void c_bdd_img_water::init()
{
	_grid_size_x = 0;
	_grid_size_y = 0;

	_data			= nullptr;
	_data_prev		= nullptr;
	_data_prev_prev	= nullptr;

	//todox64 problem ? 
	_out_min = std::numeric_limits<REAL>::max();
	_out_max = std::numeric_limits<REAL>::lowest();
}

c_bdd_img_water::~c_bdd_img_water()
{
	dealloc_water_simulation_data();
}

void	c_bdd_img_water::dealloc_water_simulation_data()
{
	if( _data || _data_prev || _data_prev_prev )
	{
		SAFE_DELETE_ARRAY( _data );
		SAFE_DELETE_ARRAY( _data_prev );
		SAFE_DELETE_ARRAY( _data_prev_prev );

		_grid_size_x = 0;
		_grid_size_y = 0;
	}
}

void	c_bdd_img_water::init_water_simulation_data()
{
	INT32	nb = _grid_size_x * _grid_size_y;
	set_vn( _data,				REAL(0.), nb );
	set_vn( _data_prev,			REAL(0.), nb );
	set_vn( _data_prev_prev,	REAL(0.), nb );
	_drop_left = 0.;
}

bool	c_bdd_img_water::alloc_water_simulation_data( INT32 size_x, INT32 size_y )
{

	dealloc_water_simulation_data();

	size_t nb_elt = size_x * size_y;
	_data = new REAL[ nb_elt ];
	if( !_data )
	{
		ERR_PRINT_STRING( "Def_water : could not allocate water simulation data" );
		dealloc_water_simulation_data();
		return false;
	}

	_data_prev = new REAL[ nb_elt ];
	if( !_data_prev )
	{
		ERR_PRINT_STRING( "Def_water : could not allocate previous water simulation data for t-1" );
		dealloc_water_simulation_data();
		return false;
	}

	_data_prev_prev = new REAL[ nb_elt ];
	if( !_data_prev_prev )
	{
		ERR_PRINT_STRING( "Def_water : could not allocate previous water simulation data for t-2" );
		dealloc_water_simulation_data();
		return false;
	}

	_grid_size_x = size_x;
	_grid_size_y = size_y;

	_b_restart_trig_ui = true;

	return true;
}

//todo make it //
void	c_bdd_img_water::compute()
{
	REAL	damp_factor = REAL(1) - _damping_ui;

	_out_min = std::numeric_limits<REAL>::max();
	_out_max = std::numeric_limits<REAL>::lowest();

	REAL*	w	= _data;
	REAL*	wp	= _data_prev;
	REAL*	wpp	= _data_prev_prev;

	REAL	fp	= _weighting_ui[0];
	REAL	fpp = _weighting_ui[1];

	if( !_b_method_complex_ui )
	{	//	we work on a 3x3 grid here
		//	101
		//	0 0
		//	101

		REAL	tmp = _maa_factor_ui * REAL(.5) / (_conv_factor_ui[0] + _conv_factor_ui[1]);
		REAL	f1 = _conv_factor_ui[0] * tmp;
		REAL	f2 = _conv_factor_ui[1] * tmp;
		//	we skip the first line and first pixel on line
		w	+= _grid_size_x + 1;
		wp	+= _grid_size_x + 1;
		wpp	+= _grid_size_x + 1;

		for( INT32 i = _grid_size_y-2; i>0 ; --i )
		{
			for( INT32 j = _grid_size_x-2; j>0 ; --j )
			{
				REAL* y_prev = wp - _grid_size_x;
				REAL* y_next = wp + _grid_size_x;
				REAL acc;
				acc =	(	*(y_prev)		+ *(wp - 1)			+ *(wp + 1)			+ *(y_next)		)	* f1;
				acc +=	(	*(y_prev - 1)	+ *(y_prev + 1)		+ *(y_next - 1)		+ *(y_next + 1)	)	* f2;
				acc -= *wpp ;
				acc *= damp_factor;

				if( acc > _out_max )
					_out_max = acc;
				if( acc < _out_min )
					_out_min = acc;
				CLAMP_REF( acc, _clamp_min_ui, _clamp_max_ui );
				*w++ = acc;
				++wp;
				++wpp;
			}
			w	+= 2;
			wp	+= 2;
			wpp	+= 2;
		}
	}
	else
	{	//	we work on a 5x5 grid here
		//	 323
		//	31013
		//	20 02
		//	31013
		//	 323		
		
		INT32	skip = (_conv_factor_ui[2]!=0. || _conv_factor_ui[3]!=0.) ? 2 : 1;
		REAL	f = _maa_factor_ui * REAL(.5) / ( _conv_factor_ui[0] + _conv_factor_ui[1] + _conv_factor_ui[2] + REAL(2)*_conv_factor_ui[3] );
	
		//	we skip the first 2 lines
		INT32 step = skip * (_grid_size_x + 1);
		w	+= step;
		wp	+= step;
		wpp	+= step;

		for( INT32 i = _grid_size_y - skip*2; i>0 ; --i )
		{
			for( INT32 j = _grid_size_x - skip*2; j>0 ; --j )
			{
				REAL* y_prev = wp - _grid_size_x;
				REAL* y_next = wp + _grid_size_x;
				REAL acc =
							(	//0
									*(wp - 1)		+	*(wp + 1)
								+	*(y_prev)		+	*(y_next)
							)	* _conv_factor_ui[0]
					
						+ 	(	//1
									*(y_prev - 1)	+	*(y_prev + 1)
								+	*(y_next - 1)	+	*(y_next + 1)
							) * _conv_factor_ui[1];
				//2
				if( _conv_factor_ui[2] != 0. )
				{
					acc +=	(
									*(wp - 2)					+	*(wp + 2)
								+	*(y_prev - _grid_size_x)	+	*(y_next + _grid_size_x) 
								) * _conv_factor_ui[2];
				}
				//3
				if( _conv_factor_ui[3] != 0. )
				{
					acc +=	(
									*(y_prev - _grid_size_x - 1)
								+	*(y_prev - _grid_size_x + 1)
								+	*(y_prev - 2)	+	*(y_prev + 2)
								+	*(y_next - 2)	+	*(y_next + 2)
								+	*(y_next + _grid_size_x - 1)
								+	*(y_next + _grid_size_x + 1)
								) * _conv_factor_ui[3];
				}

				//	size_y = interpolate( size_y / 6. - *(wpp), *(wpp), _maa_factor_ui );
				acc *= f;
				acc -= fp * (*wp) + fpp * (*wpp) ;
				acc *= damp_factor;
				if( acc > _out_max )
					_out_max = acc;
				if( acc < _out_min )
					_out_min = acc;
				CLAMP_REF( acc, _clamp_min_ui, _clamp_max_ui );
				*w++ = _inter_ui * acc + (1-_inter_ui) * *wp;
				++wp;
				++wpp;
			}
			//	we skip beginning and end col or cols
			w	+= skip*2;
			wp	+= skip*2;
			wpp	+= skip*2;
		}
	}

//	printf("Max = %f, Min = %f\n", max_value, min_value );


/*		
	// simulate
//	REAL	delta_x;
//	REAL	delta_y;
//	REAL	dampening;
//	REAL	acceleration;

//	delta_x = 1. / img_->get_size_x();
//	delta_y = 1. / img_->get_size_y();

	REAL	wave_speed_squared;
	REAL	one_half_times_delta_squared;
	delta_t_.update();
	one_half_times_delta_squared = delta_t_.get_dt();
	one_half_times_delta_squared = 0.001;
	wave_speed_squared = wave_speed_; // * wave_speed_;
*/
////	INT32	nb;
//	nb = 0;
//	for( INT32 i = 1; i < grid_size_x_ - 1 ; ++i )
//		for( INT32 j = 1; j < grid_size_y_ - 1; ++j )
//			{
//			REAL	height_x1y1;
//			REAL	height_x0y1;
//			REAL	height_x2y1;
//			REAL	height_x1y0;
//			REAL	height_x1y2;
//			
//			height_x1y1 = 0.0;
//			height_x0y1 = 0.0;
//			height_x2y1 = 0.0;
//			height_x1y0 = 0.0;
//			height_x1y2 = 0.0;
//			height_x1y1 = *(wp);
//			height_x0y1 = *(wp - 1 );
//			height_x2y1 = *(wp + 1 );
//			height_x1y0 = *(wp - grid_size_x_ );
//			height_x1y2 = *(wp + grid_size_x_ );
//			//height_x1y1 = img_->get_value_from_uv( u, v, b_clamped_, (c_img_2d::COMPO)_s_img_src_compo_ui );
//			//height_x0y1 = img_->get_value_from_uv( u - delta_x, v, b_clamped_, (c_img_2d::COMPO)_s_img_src_compo_ui );
//			//height_x2y1 = img_->get_value_from_uv( u + delta_x, v, b_clamped_, (c_img_2d::COMPO)_s_img_src_compo_ui );
//			//height_x1y0 = img_->get_value_from_uv( u, v - delta_y, b_clamped_, (c_img_2d::COMPO)_s_img_src_compo_ui );
//			//height_x1y2 = img_->get_value_from_uv( u, v + delta_y, b_clamped_, (c_img_2d::COMPO)_s_img_src_compo_ui );
//
//			//*w = ( height_x1y1 + height_x0y1 + height_x1y2 + height_x2y1 + height_x1y0 +
//			//	*(wp - grid_size_x_ - 1 ) + *(wp - grid_size_x_ + 1 ) +
//			//	*(wp + grid_size_x_ - 1 ) + *(wp + grid_size_x_ + 1 ) ) / _dampening;
//
////			dampening = _dampening;
//
//			// Compute the acceleration of the point based upon its neighbors
//			acceleration = _dampening * wave_speed_squared * (height_x0y1 + height_x2y1 + height_x1y0 + height_x1y2 - 4.0 * height_x1y1);
//
////			if( acceleration > 0.0 )
////				printf("acceleration = %f\n", acceleration );
//			// Do Verlet integration
//			*( w ) = weighting_[0] * height_x1y1 - weighting_[1] * ( *wpp ) + acceleration * one_half_times_delta_squared;
//		//	*w = 1.0;
//			if( *w > 0.0 )
////				{
//				printf("i = %d, j = %d, w = %f\n", i, j, (REAL)*w );
////				++nb;
////				}
//
//			
//			++w;
//			++wp;
//			++wpp;
////			++nb;
//			}
}

//static	INT32	l_count = 0;
static	c_rand_lin	drop_rand;
//todo extend to other format
//todo this should be img_code
void	c_bdd_img_water::do_drop( REAL* dst )
{
	_drop_left += REAL(_delta_t.get_dt()) * _drop_by_sec_ui;

	while( _drop_left >= 1.  )
	{
		_drop_left -= 1;
		UINT32 offset = IMOD( drop_rand.get_uint32()>>4, _grid_size_x-4 ) + 2 + ( IMOD( drop_rand.get_uint32()>>4, _grid_size_y-4 ) + 2 ) * _grid_size_x;
		REAL val = drop_rand.get_fp32_max(_drop_strenght_ui);
		*(dst + offset) += val;
		//	*(p_w_ + offset) = val;
/*
		*(p_w_ + offset + 1) = val;
		*(p_w_ + offset - 1) = val;
		*(p_w_ + offset + grid_size_x_ ) = val;
		*(p_w_ + offset - grid_size_x_ ) = val;
		val *= _conv_factor_ui;
		*(p_w_ + offset + grid_size_x_+1) = val;
		*(p_w_ + offset + grid_size_x_-1) = val;
		*(p_w_ + offset - grid_size_x_+1) = val;
		*(p_w_ + offset - grid_size_x_-1) = val;
*/	
	}
}

//todo extend to other format
//todo this should be img_code
void	c_bdd_img_water::import_image( c_img_2d* img, REAL* dst )
{
	if( !img || !img->is_valid() )
		return;

//	bool	b_got_one = false;

	REAL du = OVER_ONE_AS_REAL( _grid_size_x );
	REAL dv = OVER_ONE_AS_REAL( _grid_size_y );

	// now copy texture to simulation data
	if( _b_img_update_ui )
	{
		if( _b_method_complex_ui )
		{
			dst += _grid_size_x * 2 + 2;
			INT32 const iu_max = _grid_size_x-3;
			for( INT32 iv = _grid_size_y-3; iv >= 2; --iv )
			{
				REAL v = 1 - (REAL) iv * dv;	// img v are reverse than memory (bottom left)
				for( INT32 iu = 2; iu <= iu_max; ++iu )
				{
					REAL u = (REAL) iu * du;
//todoopt	a call by pixel this is ugly
					REAL tmp = img->get_valid_value_from_uv( u, v, _b_img_clamped_ui, _s_img_src_compo_ui, true );
					tmp -= _img_threshold_ui;
					if( tmp > 0. )
					{
//						b_got_one = true;
						*dst = CLAMP_01( *dst + tmp * _img_influence_ui );
					}
					++dst;
				}
				dst += 4;
			}
		}
		else
		{
			dst += _grid_size_x + 1;
			INT32 const iu_max = _grid_size_x-2;
			for( INT32 iv = _grid_size_y-3; iv >= 2; --iv )
			{
				REAL v = REAL(1) - (REAL) iv * dv;	// img v are reverse than memory (bottom left)		
				for( INT32 iu = 1; iu <= iu_max; ++iu )
				{
					REAL u = (REAL) iu * du;
//todoopt	a call by pixel this is ugly
					REAL tmp = img->get_valid_value_from_uv( u, v, _b_img_clamped_ui, _s_img_src_compo_ui, true );
					tmp -= _img_threshold_ui;
					if( tmp > 0. )
					{
//						b_got_one = true;
						*dst = CLAMP_01( *dst + tmp * _img_influence_ui );
					}
					++dst;
				}
				dst += 2;
			}
		}
	}
}

//todoe extend to other format
//todo this should be img_code
void	c_bdd_img_water::move_to_tex( c_img_2d* img, REAL* src )
{
	if( !img )
		return;

	REAL f_val	= _value_factor_ui;
	REAL f_grad	= _gradient_factor;

	INT32 CONST sx = _grid_size_x;

	//todo we assume for now a stride of 0 between lines
	INT32	CONST compo_per_pixel = _b_img_out_with_alpha_ui ? 4 : 3;

	//we have to do something for the border
	if( _b_img_out_float_ui )
	{
		FP32*	dst = img->get_data_fp32();
		if( !dst )
		{
			ERR_PRINT_STRING( "%s() float dst is Null", __FUNCTION__ );
			return;
		}

		--dst;
		dst += sx * compo_per_pixel;
		src += sx;

		for( INT32 nbv = _grid_size_y - 2; nbv > 0 ; --nbv )
		{
			++src;
			dst += compo_per_pixel;
			for( INT32 nbu = sx - 2; nbu > 0 ; --nbu )
			{	
				REAL c	= *src;
	//			f_grad = 1.0/sqrt(x*x + y*y );
				if( _b_gradient_ui )
				{
					//REAL dx	= *(src - 1)	-	*(src + 1);
					//REAL dy	= *(src - sx)	-	*(src + sx);
					REAL dx	= *(src - 1)	-	c;
					REAL dy	= *(src - sx)	-	c;
					*++dst =	CLAMP_01( dx * f_grad + REAL(.5) );
					*++dst =	CLAMP_01( dy * f_grad + REAL(.5) );
					*++dst =	CLAMP_01( c  * f_val  + REAL(.5) );
				}
				else
				{
					REAL v = CLAMP_01( c * f_val + REAL(.5) );
					*++dst =	v;
					*++dst =	v;
					*++dst =	v;
				}
				if( _b_img_out_with_alpha_ui )
					++dst;
				++src;
			}
			++src;
			dst += compo_per_pixel;
		}
	}
	else
	{
		UINT8*	dst = img->get_data_uint8();
		if( !dst )
		{
			ERR_PRINT_STRING( "%s() uint8 dst is Null", __FUNCTION__ );
			return;
		}
		f_val	*= 256.;
		f_grad	*= 256.;

		--dst;
		dst += sx * compo_per_pixel;
		src += sx;

		for( INT32 nbv = _grid_size_y - 2; nbv > 0 ; --nbv )
		{
			++src;
			dst += compo_per_pixel;
			for( INT32 nbu = sx - 2; nbu > 0 ; --nbu )
			{	
				REAL c	= *src;
	//			f_grad = 1.0/sqrt(x*x + y*y );
				if( _b_gradient_ui )
				{
					//REAL dx	= *(src - 1)	-	*(src + 1);
					//REAL dy	= *(src - sx)	-	*(src + sx);
					REAL dx	= *(src - 1)	-	c;
					REAL dy	= *(src - sx)	-	c;
					*++dst =	UINT8( CLAMP( REAL(dx * f_grad + 128.),	0, 255) );
					*++dst =	UINT8( CLAMP( REAL(dy * f_grad + 128.),	0, 255) );
					*++dst =	UINT8( CLAMP( REAL(c  * f_val  + 128.),	0, 255) );
				}
				else
				{
					UINT8 v = UINT8( CLAMP( INT32(REAL(c * f_val) + 128.),	0, 255 ) );
					*++dst =	v;
					*++dst =	v;
					*++dst =	v;
				}
				if( _b_img_out_with_alpha_ui )
					++dst;
				++src;
			}
			++src;
			dst += compo_per_pixel;
		}
	}

	g_bind_img_2d->do_after_a_compute();
	
/*
image @normalMap(image @hmap)
{			compute( img);

  int w = hmap.width;
  int h = hmap.height;
  
  image @nmap = @image(w, h);
  
  int x, y = 0;
  while( y < h )
  {
	x = 0;
	while( x < w )
	{
	  // Get two vectors passing through pixel
	  float hxp = hmap.Alpha((x + 1)%w, y);
	  float hyp = hmap.Alpha(x, (y + 1)%h);
	  float hxm = hmap.Alpha((x + w - 1)%w, y);
	  float hym = hmap.Alpha(x, (y + h - 1)%h);

	  // Compute the cross product of the two vectors
	  float nx = -2.0*(hyp - hym);
	  float ny = -(hxp - hxm)*2.0;
	  float nz = 2.0*2.0;

	  // Normalize      
	  float norm = 1.0/sqrt(nx*nx + ny*ny + nz*nz);
	  nmap.Pixel(x,y) = pixel(nx*norm, ny*norm, nz*norm);
	  
	  ++x;
	}
	
	++y;
  }
  
  retu
*/
}


void	c_bdd_img_water::update()
{
//	printf( " c_def_water::update()\n" );
	
	_delta_t.update();

//	printf("c_def_water::apply()\n");
	if( !_data || !_data_prev || !_data_prev_prev || _grid_size_x != _grid_size_x_ui || _grid_size_y != _grid_size_y_ui )
	{
		if( !alloc_water_simulation_data( _grid_size_x_ui, _grid_size_y_ui ) )
		{
			return;
		}
	}

	
	//rotate the buffers
	REAL* tmp = _data_prev_prev;
	_data_prev_prev = _data_prev;
	_data_prev = _data;
	_data = tmp;

	if( _b_restart_trig_ui )
	{
		init_water_simulation_data();
		_b_restart_trig_ui = false;
	}

	c_img_2d* img = g_bind_img_2d->get_ready( _img_src_index_ui );
	
	
	if( _b_compute_ui )
	{
		import_image( img, _data_prev );
		if( _b_drop_ui )
			do_drop( _data_prev );
		compute();
	}
	else
	{
		INT32	nb = _grid_size_x * _grid_size_y;
		set_vn( _data,	REAL(0.), nb );
		import_image( img, _data );
		if( _b_drop_ui )
			do_drop( _data );
	}

	// copy to image dst
	//todo deal with the generic case of all format
	aaa::PIXEL_FORMAT pf;
	if( _b_img_out_float_ui )
		pf = _b_img_out_with_alpha_ui ? aaa::PIXEL_FORMAT::RGBA_32FP : aaa::PIXEL_FORMAT::RGB_32FP;
	else
		pf = _b_img_out_with_alpha_ui ? aaa::PIXEL_FORMAT::RGBA_8 : aaa::PIXEL_FORMAT::RGB_8;
	img = g_bind_img_2d->get_img_cur( pf, _grid_size_x, _grid_size_y, true, nullptr, __FUNCTION__ );
	move_to_tex( img, _data );
}
