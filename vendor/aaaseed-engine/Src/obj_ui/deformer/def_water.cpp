#include "obj_ui/deformer/def_water.h"
#include "image/bind_img_2d.h"
#include "ui/strsymbo.h"
#include "math/rand.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_water, def_water, Deformer Water, def );

FINLINE	INT32	make_pixel( REAL uv, INT32 size )
{
	if( uv == 1. )	{	return size - 1;	}
	else			{	return IMOD( INT32( uv * (REAL)size ), size );	}
}

namespace n_def_water
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 23 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 ORIGIN_PARAM_NB	= 3;
	CONSTEXPR INT32 WAVE_PARAM_NB	= 3;
	CONSTEXPR INT32 CLAMP_PARAM_NB	= 2;
	CONSTEXPR INT32 DROP_PARAM_NB	= 3;
	CONSTEXPR INT32 OUT_PARAM_NB	= 2;
	CONSTEXPR INT32 GROUP_NB		= 5;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	ORIGIN_PARAM_NB
									+	WAVE_PARAM_NB
									+	CLAMP_PARAM_NB
									+	DROP_PARAM_NB
									+	OUT_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_BIND_2D_ALONE(	Image_src			)	//todo we should limit ?
		PARAM_DEF_SYMBO_PSTR(		image_component,	1, INT32(aaa::COMPO::RED),		aaa::str_compo )

	//todo regroup with PARAM_DEF_IMG_SIZE_FORMAT ?
		PARAM_DEF_IMG_SIZE(			size_x,				1, 128	)
		PARAM_DEF_IMG_SIZE(			size_y,				1, 128	)
		PARAM_DEF_BIND_2D_ALONE(	image_dst		)
		PARAM_DEF_REAL_ZERO(		maa_factor		)
		PARAM_DEF_REAL_ONE(			gradient_factor	)
		PARAM_DEF_REAL_ONE(			value_factor	)
		PARAM_DEF_BOOL_OFF(			5x5				)
		PARAM_DEF_REAL_ZERO(		seuil			)
		PARAM_DEF_REAL_ONE(			water_strenght	)
		PARAM_DEF_REAL_ONE(			dampening		)
		PARAM_DEF_REAL_INF(			convolution_adjustement,		0., .5	)
		PARAM_DEF_GROUP( Wave, WAVE_PARAM_NB )
			PARAM_DEF_REAL_POS(		wave_speed,		1, 10	)
			PARAM_DEF_REAL_INF(		weighting_x,	1, 0.99	)
			PARAM_DEF_REAL_ZERO(		weighting_y		)

		PARAM_DEF_BOOL_OFF(			Gradient		)
//		PARAM_DEF_REAL_ZERO(		Normal			)
		PARAM_DEF_AXE_X(			axe_src			)
		PARAM_DEF_AXE_Y(			axe_dst			)
		PARAM_DEF_REAL_ONE(			scale			)
		PARAM_DEF_REAL_ZERO(		strenght		)
		PARAM_DEF_REAL_ZERO(		offset			)
		PARAM_DEF_BOOL_OFF(			clamped			)
		PARAM_DEF_BOOL_ON(			update_img		)
		PARAM_DEF_BOOL_OFF(			restart_trig	)
		PARAM_DEF_GROUP( Origin, ORIGIN_PARAM_NB )
			PARAM_DEF_POINT_XYZ(	origin	)
		PARAM_DEF_BOOL_OFF(			swap )

		PARAM_DEF_GROUP( CLAMP, CLAMP_PARAM_NB )
			PARAM_DEF_REAL_INF(			clamp_min,		0, -10	)
			PARAM_DEF_REAL_INF(			clamp_max,		0, 10	)
		PARAM_DEF_GROUP( DROP, DROP_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			drop )
			PARAM_DEF_REAL_ONE(			drop_by_sec		)
			PARAM_DEF_REAL_ONE(			water_strenght	)
		PARAM_DEF_GROUP( OUT_FIELD, OUT_PARAM_NB )
			PARAM_DEF_REAL_LOCKED(		out_field_min	)
			PARAM_DEF_REAL_LOCKED(		out_field_max	)
		};
}


CONSTRUCTOR_CREATE( c_def_water )
{
	init_name_with( "Def Water" );
	param_init_with( n_def_water::param, n_def_water::PARAM_NB_MAX );
	init();
}

void c_def_water::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _img_index_ui );
	param_set_pt( h, _s_compo_ui );
	param_set_pt( h, grid_size_x_ui_ );
	param_set_pt( h, grid_size_y_ui_ );
	param_set_pt( h, img_dst_index_ );
	param_set_pt( h, maa_factor_ );
	param_set_pt( h, gradient_factor_ );
	param_set_pt( h, value_factor_ );
	param_set_pt( h, _b_method_complex );
	param_set_pt( h, seuil_ );
	param_set_pt( h, water_strenght_ );
	param_set_pt( h, dampening_ );
	param_set_pt( h, conv_factor_ );

	++h;
		param_set_pt( h, wave_speed_ );
		param_set_pt( h, weighting_[0] );
		param_set_pt( h, weighting_[1] );

	param_set_pt( h, _b_gradient_ui );
//	param_set_pt( h, normal_ );
	
	param_set_pt( h, i_src_axe_ );
	param_set_pt( h, i_dst_axe_ );
	param_set_pt( h, scale_ );
	param_set_pt( h, _strenght_ui );
	param_set_pt( h, _offset );
	param_set_pt( h, _b_clamped_ui );
	param_set_pt( h, _b_update_img );
	param_set_pt( h, _b_restart_trig_ui );

	++h;
		param_set_pt_3( h, _origin );

	param_set_pt( h, _b_swap );

	++h;
		param_set_pt( h, clamp_min_ );
		param_set_pt( h, clamp_max_ );

	++h;
		param_set_pt( h, _b_drop );
		param_set_pt( h, drop_by_sec_ );
		param_set_pt( h, drop_strenght_ );

	++h;
		param_set_pt( h, out_min_ );
		param_set_pt( h, out_max_ );

	err_param_init_pt( h );
}

void c_def_water::init()
{
	grid_size_x_ = 0;
	grid_size_y_ = 0;
	data_ = nullptr;
	data_prev_ = nullptr;
	data_prev_prev_ = nullptr;
	p_w_ = nullptr;
	p_wp_ = nullptr;
	p_wpp_ = nullptr;
	out_min_ = std::numeric_limits<REAL>::max();
	out_max_ = std::numeric_limits<REAL>::lowest();
}

c_def_water::~c_def_water()
{
}

void	c_def_water::dealloc_water_simulation_data()
{
	SAFE_DELETE_ARRAY( data_ );
	SAFE_DELETE_ARRAY( data_prev_ );
	SAFE_DELETE_ARRAY( data_prev_prev_ );

	p_w_ = nullptr;
	p_wp_ = nullptr;
	p_wpp_ = nullptr;

	grid_size_x_ = 0;
	grid_size_y_ = 0;
}

void	c_def_water::init_water_simulation_data()
{
	INT32	nb;
	nb = grid_size_x_ * grid_size_y_;
	set_vn( data_,				REAL(0.), nb );
	set_vn( data_prev_,			REAL(0.), nb );
	set_vn( data_prev_prev_,	REAL(0.), nb );
}

bool	c_def_water::alloc_water_simulation_data( INT32 size_x, INT32 size_y )
{
	INT32 nb = size_x * size_y;
	if( data_ || data_prev_ || data_prev_prev_ )
		dealloc_water_simulation_data();

	data_ = new REAL[nb];
	if( !data_ )
	{
		err_print( "Def_water : could not allocate water simulation data" );
		dealloc_water_simulation_data();
		return false;
	}

	data_prev_ = new REAL[nb];
	if( !data_prev_ )
	{
		err_print( "Def_water : could not allocate previous water simulation data t-1" );
		dealloc_water_simulation_data();
		return false;
	}

	data_prev_prev_ = new REAL[nb];
	if( !data_prev_prev_ )
	{
		err_print( "Def_water : could not allocate previous water simulation data t-2" );
		dealloc_water_simulation_data();
		return false;
	}

	grid_size_x_ = size_x;
	grid_size_y_ = size_y;

	p_w_ = data_;
	p_wp_ = data_prev_;
	p_wpp_ = data_prev_prev_;

	init_water_simulation_data();

	return true;
}

void	c_def_water::update()
{
//	printf( " c_def_water::update()\n" );
	img_ = g_bind_img_2d->get_ready( _img_index_ui );
	
	if( img_ )
	{
	//	prepare the axes
		axe_build_index( i_src_u_, i_src_v_, i_src_axe_ );
	//	scale_v3r( size, size_ui, size_ui[3] );
		delta_t_.update();
		set_deforming( true );
	}
	else
	{
		set_deforming( false );
	}
}

void	c_def_water::compute()
{
	REAL*	w;
	REAL*	wp;
	REAL*	wpp;
	REAL	damp_factor;

	damp_factor = REAL( 1. - ( 1. / dampening_ ) );

	out_min_ = std::numeric_limits<REAL>::max();
	out_max_ = std::numeric_limits<REAL>::lowest();

	REAL	height;
	REAL*	y_prev;
	REAL*	y_next;

	w = p_w_;
	wp = p_wp_;
	wpp = p_wpp_;

	if( _b_method_complex )
	{
		w += 2* grid_size_x_;
		wp += 2* grid_size_x_;
		wpp += 2* grid_size_x_;
		for( INT32 i = 2; i < grid_size_y_ - 2 ; ++i )
		{
			w += 2;
			wp += 2;
			wpp += 2;
			for( INT32 j = 2; j < grid_size_x_ - 2; ++j )
			{
				y_prev = wp - grid_size_x_;
				y_next = wp + grid_size_x_;
				height = *(wp - 1 ) + *(wp - 2 ) + *(wp + 1 ) + *(wp + 2 ) 
						+ *(y_prev) + *(y_prev - grid_size_x_ )  + *(y_next) + *(y_next + grid_size_x_ ) 
						+ *(y_prev - 1 ) + *(y_prev + 1 ) + *(y_next - 1) + *(y_next + 1);
				height = REAL(height / 6. - *(wpp));

				height *= damp_factor;
				if( height > out_max_ )		out_max_ = height;
				if( height < out_min_ )		out_min_ = height;
				CLAMP_REF( height, clamp_min_, clamp_max_ );
				*(w) = height;

				++w;
				++wp;
				++wpp;
			}
			w += 2;
			wp += 2;
			wpp += 2;
		}
	}
	else
	{
		w += grid_size_x_;
		wp += grid_size_x_;
		wpp += grid_size_x_;

		REAL	f;
		f = REAL( 1. / ( 4. + 4.*conv_factor_) );

		for( INT32 i = 1; i < grid_size_y_ - 1 ; ++i )
		{
			++w;
			++wp;
			++wpp;
			for( INT32 j = 1; j < grid_size_x_ - 1; ++j )
			{
				height =	*(wp - 1 )	+ *(wp + 1 )	+ *(wp - grid_size_x_ )		+ *(wp + grid_size_x_ )
								+ (		*(wp - grid_size_x_ - 1)
									+	*(wp - grid_size_x_ + 1)
									+	*(wp + grid_size_x_ - 1)
									+	*(wp + grid_size_x_ + 1)
								  ) * conv_factor_;

				height =  height * f  - (*wp);

				height *= maa_factor_;

				height = REAL( (1.+weighting_[0]) * (*wp) - weighting_[0] * (1+weighting_[1]) * (*wpp) + height );

				height  *= damp_factor;

				if( height > out_max_ )		out_max_ = height;
				if( height < out_min_ )		out_min_ = height;

				CLAMP_REF( height, clamp_min_, clamp_max_ );

				*(w) = height;

				++w;
				++wp;
				++wpp;
			}
			++w;
			++wp;
			++wpp;
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
//			//height_x1y1 = img_->get_value_from_uv( u, v, b_clamped_, (c_img_2d::COMPO)s_compo_ );
//			//height_x0y1 = img_->get_value_from_uv( u - delta_x, v, b_clamped_, (c_img_2d::COMPO)s_compo_ );
//			//height_x2y1 = img_->get_value_from_uv( u + delta_x, v, b_clamped_, (c_img_2d::COMPO)s_compo_ );
//			//height_x1y0 = img_->get_value_from_uv( u, v - delta_y, b_clamped_, (c_img_2d::COMPO)s_compo_ );
//			//height_x1y2 = img_->get_value_from_uv( u, v + delta_y, b_clamped_, (c_img_2d::COMPO)s_compo_ );
//
//			//*w = ( height_x1y1 + height_x0y1 + height_x1y2 + height_x2y1 + height_x1y0 +
//			//	*(wp - grid_size_x_ - 1 ) + *(wp - grid_size_x_ + 1 ) +
//			//	*(wp + grid_size_x_ - 1 ) + *(wp + grid_size_x_ + 1 ) ) / dampening_;
//
////			dampening = dampening_;
//
//			// Compute the acceleration of the point based upon its neighbors
//			acceleration = dampening_ * wave_speed_squared * (height_x0y1 + height_x2y1 + height_x1y0 + height_x1y2 - 4.0 * height_x1y1);
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
static	c_rand_lin	a_rand;
static	UINT32		_drop_off = 0;
static	REAL		_drop_t = 0.;

void	c_def_water::import_image()
{
	REAL	u;
	REAL	v;
	REAL*	w;
	REAL	dx,dy;
	bool	b_got_one = false;	//todo just for debug ?

	dx = REAL(1. / grid_size_x_ );
	dy = REAL(1. / grid_size_y_ );


	w = p_w_;
	// now copy texture to simulation data
	REAL	tmp;
	if( _b_update_img )
	{
		w += grid_size_x_;
		for( INT32 j = grid_size_y_-2; j>0; --j )
		{
			v = (REAL) j * dy;
			++w;
			for( INT32 i = grid_size_x_-2; i>0; --i )
			{
				u = (REAL) i * dx;
//todoopt	a call by pixel this is ugly
				tmp = img_->get_value_from_uv( u, v, _b_clamped_ui, _s_compo_ui );
				if( tmp > seuil_ )
				{
					b_got_one = true;
					*w = CLAMP( *w + tmp * water_strenght_ , 0, 1 );
				}
				++w;
			}
			++w;
		}
	}
	
	if( _b_drop )
	{
		if( _drop_t > 0. )
		{
			_drop_t -= REAL( delta_t_.get_dt()*1. );
			*(p_w_ + _drop_off) = drop_strenght_*SIN_TURN(_drop_t);
		}
		else
		{
			tmp = REAL(delta_t_.get_dt()) * drop_by_sec_;
			if(	 a_rand.get_fp32_01() < tmp )
			{
				UINT32	offset;
				REAL	val;
				offset = grid_size_x_* 3 + IMOD( a_rand.get_uint32()>>4, grid_size_x_ *(grid_size_y_-6) );
				val = a_rand.get_fp32_max( drop_strenght_ );
				_drop_t = .5;
				_drop_off = offset;
				//*(p_w_ + offset) = val;

/*				*(p_w_ + offset + 1) = val;
				*(p_w_ + offset - 1) = val;
				*(p_w_ + offset + grid_size_x_ ) = val;
				*(p_w_ + offset - grid_size_x_ ) = val;
				val *= conv_factor_;
				*(p_w_ + offset + grid_size_x_+1) = val;
				*(p_w_ + offset + grid_size_x_-1) = val;
				*(p_w_ + offset - grid_size_x_+1) = val;
				*(p_w_ + offset - grid_size_x_-1) = val;
*/			}
		}
	}
}


void	c_def_water::move_to_tex()
{
	img_dst_ = g_bind_img_2d->get_always( img_dst_index_ );
	if( img_dst_ )
	{
		REAL*	w;
		UINT8*	data;
		REAL	x, y, c;
		REAL	f_grad;
		REAL	f_val;

		f_val = REAL( value_factor_ * 127.99 );
		f_grad = REAL( gradient_factor_ * 127.99 * .5 );

		img_dst_->init_with_size( grid_size_x_, grid_size_y_, aaa::PIXEL_FORMAT::RGB_8, __FUNCTION__ );

		data = img_dst_->get_data_uint8();
		if( data )
		{
			data += grid_size_x_ * 3;

			w = p_w_;
			w += grid_size_x_;

			for( INT32 i = grid_size_y_ - 2; i>0 ; --i )
			{
				++w;
				data += 3;
				for( INT32 j = grid_size_x_ - 2; j>0 ; --j )
				{
					c = *w;
					x = *(w + 1) - c;
					y = *(w + grid_size_x_ ) - c;

	//				f_grad = 1.0/sqrt(x*x + y*y );

					*(data) =	UINT32( CLAMP( REAL(x * f_grad) + 128., 0, 255) );
					*(data+1) =	UINT32( CLAMP( REAL(y * f_grad) + 128., 0, 255) );
					*(data+2) =	UINT32( CLAMP( REAL(c * f_val)  + 128., 0, 255) );

					++w;
					data += 3;
				}
				++w;
				data += 3;
			}
		}
	}
	
/*
image @normalMap(image @hmap)
{
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

return
*/
}

void	c_def_water::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL	u;
	REAL	v;
	REAL	o_u;
	REAL	o_v;

	if( !img_ )	{ return; }	//todo un peu rapide non

//	printf("c_def_water::apply()\n");
	if( !data_ || !data_prev_ || !data_prev_prev_ || grid_size_x_ != grid_size_x_ui_ || grid_size_y_ != grid_size_y_ui_ )
	{
		// first time here, alloc memory
		if( !alloc_water_simulation_data( grid_size_x_ui_, grid_size_y_ui_ ) )
		{
			return;
		}
	}

	if( _b_restart_trig_ui )
	{
		init_water_simulation_data();
		_b_restart_trig_ui = false;
	}

	compute();
	import_image();

	// copy to image dst
	move_to_tex();

	if( _b_swap )
	{
		REAL* tmp;
		tmp = p_wpp_;
		p_wpp_ = p_wp_;
		p_wp_ = p_w_;
		p_w_ = tmp;
	}
	else
	{
		//save previous data
		MEMCPY( data_prev_prev_,	data_prev_, grid_size_x_ * grid_size_y_ * sizeof(REAL), __FUNCTION__ );
		MEMCPY( data_prev_,			data_,		grid_size_x_ * grid_size_y_ * sizeof(REAL), __FUNCTION__ );
	}

	o_u = _origin[i_src_u_];
	o_v = _origin[i_src_v_];

	if( _b_gradient_ui )
	{
		//INT32		i_dst_u;
		//INT32		i_dst_v;
		//axe_build_index( i_dst_u, i_dst_v, i_dst_axe);
		//for(; nb > 0; --nb )
		//	{
		//	REAL r[3];

		//	cpy_v3( dst, src);

		//	u = ( *(src + i_src_u) - o_u ) * scale;
		//	v = ( *(src + i_src_v) - o_v ) * scale;
		//	img->get_gradient_from_uv_linear_v2r( r, u, v, b_clamped, (c_img_2d::COMPO)s_compo );

		//	*(dst + i_dst_u) += r[0] * strenght;
		//	*(dst + i_dst_v) += r[1] * strenght;
		//	if( normal != 0. )
		//		{		
		//		*(dst + i_dst_u) += r[1] * strenght* normal;
		//		*(dst + i_dst_v) += -r[0] * strenght * normal;
		//		}
		//	//next one
		//	src += 3;
		//	dst += 3;
		//	}
	}
	else
	{
		if( _strenght_ui == 0.)
		{
			MEMCPY( dst, src, nb * 3 * sizeof(REAL), __FUNCTION__ );
		}
		else
		{
			for(; nb > 0; --nb )
			{
				REAL r;

				cpy_v3( dst, src);

				u = ( *(src + i_src_u_ ) - o_u ) * scale_;
				v = ( *(src + i_src_v_ ) - o_v ) * scale_;
				INT32 iu = make_pixel( REAL(u / 8.0 + 0.5), grid_size_x_ );
				INT32 iv = make_pixel( REAL(v / 8.0 + 0.5), grid_size_y_ );
	//			if( v > 0 )
	//				printf( "iu = %d, iv = %d\n", iu, iv);
				r =(REAL) *( p_w_ + iu + iv * grid_size_x_ ) / 255;     // img->get_value_from_uv( u, v, b_clamped, (c_img_2d::COMPO)s_compo);

				r = r * _strenght_ui + _offset;
				*(dst + i_dst_axe_ ) += r;

				//next one
				src += 3;
				dst += 3;
			}
		}
	}
}
