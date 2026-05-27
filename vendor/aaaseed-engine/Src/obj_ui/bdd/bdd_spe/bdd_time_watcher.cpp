#include "bdd_time_watcher.h"
#include "draw/model.h"
#include "spy.h"
#include "obj_ui/deformer/def_node.h"
#include "gol/gol_draw.h"
#include "infrastructure/layer/layer.h"
#include "ui/alphabet.h"
#include "gol/gol_matrix.h"
#include "time_buf_master.h"
#include "time_buf.h"


FACTORY_CREATE_PROP_V1( c_bdd_time_watcher, bdd_time_watcher, Time Watcher, bdd_time_watcher, sub_menu="Internal"; );

namespace n_bdd_time_watcher
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 11 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 MARKER_PARAM_NB	= 10;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	MARKER_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BASE_STR(			TYPE_BOOL|M_SAVE_NOT,	"use_color",	1, 0,	0, 1,	gstr::current )
		PARAM_DEF_NONE(				time_buffer_master )
		PARAM_DEF_SYMBO_PSTR_ZERO(	channel,	tbuf::str_ch_name	)
		PARAM_DEF_POINT_XYZ(		center		)
		PARAM_DEF_REAL_ONE(			size_factor	)
		PARAM_DEF_BOOL_OFF(			draw_box	)
		PARAM_DEF_BOOL_ON(			draw_line	)
		PARAM_DEF_BOOL_OFF(			draw_point	)
		PARAM_DEF_BOOL_OFF(			draw_text	)

		PARAM_DEF_GROUP_CLOSED( Marker, MARKER_PARAM_NB )	
			PARAM_DEF_BOOL_OFF(		draw_marker )
			PARAM_DEF_REAL(			marker_per_sec,			50, 60,	.00001, PARAM_MAX_REAL )
			PARAM_DEF_REAL_ZERO(	marker_time_offset		)
			PARAM_DEF_REAL_ZERO(	marker_space_offset		)
			PARAM_DEF_REAL_ONE(		marker_space_size		)
			PARAM_DEF_COLOR_RGBGA(	marker					)
	};
}

void	c_bdd_time_watcher::param_init_pt()
{
	INT32	h = param_init_pt_geo();
	if( c_layer* p_layer = get_layer() )
	{
		param_attach_obj_no_inc( h, (c_obj_ui*)p_layer->get_color_for_bdd() );
		param_set_pt( h, p_layer->get_b_color_pt() );
	}
	else
	{
		param_attach_obj( h, nullptr );
	}
	param_attach_obj( h, g_tbuf_master );

	param_set_pt( h, _channel_id );
	param_set_pt_3( h, _center );
	param_set_pt( h, _size_factor_ui );

	param_set_pt( h, _b_draw_box_ui );
	param_set_pt( h, _b_draw_line_ui );
	param_set_pt( h, _b_draw_point_ui );
	param_set_pt( h, _b_draw_text_ui );

	++h;
		param_set_pt( h, _b_draw_marker_ui );
		param_set_pt( h, _marker_per_sec );
		param_set_pt( h, _marker_time_offset );
		param_set_pt( h, _marker_space_offset );
		param_set_pt( h, _marker_space_size );

		param_set_pt_4( h, _marker_color_ui );
		param_set_pt( h, _marker_color[3] );

	err_param_init_pt(h);
}

FINLINE	void c_bdd_time_watcher::point_init()
{ 
	_point = nullptr;
	_point_deformed = nullptr;
	_point_nb_allocated = 0;
	_sample_nb = 0;
}

FINLINE	void c_bdd_time_watcher::point_alloc( INT32 nb_in )
{
	if( _point_nb_allocated < nb_in )
	{
		_point = (REAL *) REALLOC_ALIGNED( _point, 2* nb_in*3*sizeof(REAL) );
		if( _point )
		{
			_point_nb_allocated = nb_in;
			_point_deformed = _point + _point_nb_allocated * 3;
		}
		else
		{
			err_print( "Can't alloc point" );
			point_dealloc();
		}
	}
}

void c_bdd_time_watcher::point_dealloc()
{ 
	if( _point )
	{
		FREE_ALIGNED_AND_NULL( _point );
		point_init();
	}
}

void c_bdd_time_watcher::init()
{
	point_init();
	param_init_with( n_bdd_time_watcher::param, n_bdd_time_watcher::PARAM_NB_MAX ); // bdd_time_watcher_param, BDD_TIME_WATCHER_PARAM_NB_MAX);
}

CONSTRUCTOR_CREATE(c_bdd_time_watcher)
{
	init();
}

c_bdd_time_watcher::~c_bdd_time_watcher()
{
	point_dealloc();
}


void	c_bdd_time_watcher::update_low()
{
	c_tbuf::cont_data *	pdata = _p_tbuf->get_data();
	_b_const = _p_tbuf->is_const();

	_sample_nb = (INT32)pdata->size();

	if( _sample_nb == 0 )	//todo	should make some update needed (eg for marker)
		return;

	if( _b_const )
		_point_nb = _sample_nb * 2 - 1;
	else
		_point_nb = _sample_nb;
	point_alloc( _point_nb+_sample_nb );

	c_model::cur->get_axes( _i_u, _i_v, _i_axe );
	c_model::cur->get_size_v3( _size );

	REAL				time_begin;
	REAL				time_end;
	c_tbuf::iter_data	it;
	REAL*				p_point;
	REAL*				p_point_base;
	REAL				tmp;
	c_tdata*			src;
	REAL				time_factor;
	REAL				time_offset;
	INT32				point_offset;
	
	if( g_tbuf_master->_b_need_time_scale )
	{
		g_tbuf_master->_b_need_time_scale = false;

		//it = pdata->begin();
		//time_begin = (*it)->get_time();

		it = pdata->end();
		--it;
		time_end = REAL((*it)->get_time());

		time_begin = time_end - g_tbuf_master->_draw_time_len;

		if( time_begin != time_end )
			time_factor = _size[0] / ( time_end - time_begin );
		else
			time_factor = _size[0];
		time_offset = -_size[0] * REAL(.5) + _center[_i_u];

		g_tbuf_master->_time_begin = time_begin;
		g_tbuf_master->_time_end = time_end;
		g_tbuf_master->_time_factor = time_factor;
		g_tbuf_master->_time_offset = time_offset;
	}
	else
	{
		time_begin = g_tbuf_master->_time_begin;
		time_end = g_tbuf_master->_time_end;
		time_factor = g_tbuf_master->_time_factor;	//	* _size[0] not sure
		time_offset = g_tbuf_master->_time_offset;
	}

	_size_factor = _size_factor_ui*_size[1];
	p_point = _point;
	point_offset = _b_const ? 6 : 3 ;
	for( it = pdata->begin(); it != pdata->end(); ++it )
	{
		src = *it;

/*		//this not optimum
		if ( tmp < time_begin )
			time_begin = tmp;
		if ( tmp > time_end )
			time_begin = tmp;
*/
		tmp = REAL(src->get_time()) - time_begin;
		tmp *= time_factor;
		p_point[_i_u] = (tmp + time_offset) * _size[0];

		tmp = src->get_value();
		p_point[_i_v] = tmp*_size_factor + _center[_i_v];

		p_point[_i_axe] = _center[_i_axe];

		p_point += point_offset;
	}
	
	if( _b_const)
	{
		p_point = _point;
		for( INT32 i=_point_nb/2; i>0; --i )
		{
			p_point[_i_u+3]		= p_point[_i_u+6];	//	time given by next one
			p_point[_i_axe+3]	= p_point[_i_axe];	//	value given by previous one
			p_point[_i_v+3]		= p_point[_i_v];	//	position constant
			p_point += 6;
		}
	}
	p_point = _point;
	p_point_base = p_point + _point_nb*3;
	for( INT32 i=_sample_nb; i>0; --i )
	{
		cpy_v3( p_point_base, p_point);
		p_point_base[_i_v] = _center[_i_v];
		p_point += point_offset;
		p_point_base += 3;
	}
	
	if( _b_draw_marker_ui )
	{	//todo _marker_color is never used
		scale_v3( _marker_color, _marker_color_ui, _marker_color_ui[3] );
	}

	auto def = c_def_node::get_cur();
	if( def->is_deforming() )
	{
		def->apply( _point_deformed, _point, _point_nb+_sample_nb );
		_point_to_draw = _point_deformed;
	}
	else
		_point_to_draw = _point;
}

void	c_bdd_time_watcher::update()
{
	_p_tbuf = g_tbuf_master->get(_channel_id);
	if( _p_tbuf )
		update_low();
}

void	c_bdd_time_watcher::draw_point()
{
	INT32	i;
	REAL*	pt = _point_to_draw;
	INT32	point_offset;

	if( _b_const )
	{
		i = _point_nb / 2 + 1;
		point_offset = 6;
	}
	else
	{
		i = _point_nb;
		point_offset = 3;
	}
	if( i>1 )
	{
		GOL::begin( GL_POINTS );
		while( i-->0 )
		{
			GOL::vertex3v( pt);
			pt += point_offset;
		}
		GOL::end();
	}
}

void	c_bdd_time_watcher::draw_line()
{
	INT32	i;
	REAL*	pt = _point_to_draw;

	i = _point_nb;
	if( i>1 )
	{
		GOL::begin( GL_LINE_STRIP );
		while( i-->0 )
		{
			GOL::vertex3v( pt);
			pt += 3;
		}
		GOL::end();
	}
}
/*
			cpy_v3( vec, pt);
			vec[i_axe] = center[i_axe]-1;
			GOL::vertex3v( vec);
			GOL::vertex3v( pt);
			pt += 3;
			GOL::vertex3v( pt);
			cpy_v3( vec, pt);
			vec[i_axe] = center[i_axe]-1;
			GOL::vertex3v( vec);
*/
void	c_bdd_time_watcher::draw_box()
{
	INT32	i;
	REAL*	pt = _point_to_draw;
	REAL*	pt_base = pt + _point_nb*3;

	i = _point_nb;
	if( i>1 )
	{
		if( _b_const )
		{
			i = _sample_nb-1;
			GOL::begin( GL_TRIANGLES );
			while( i-->0 )
			{
				if( pt_base[_i_v] == pt[_i_v] )
				{
					pt_base += 3;
					pt += 6;
				}
				else
				{
					GOL::vertex3v( pt );
					GOL::vertex3v( pt_base );
					pt += 3;
					GOL::vertex3v( pt );

					GOL::vertex3v( pt );
					GOL::vertex3v( pt_base );
					pt_base += 3;
					GOL::vertex3v( pt_base );

					pt += 3;
				}
			}
			GOL::end();
		}
		else
		{
			GOL::begin( GL_TRIANGLE_STRIP );
			while( i-->0 )
			{
				GOL::vertex3v( pt_base );
				pt_base += 3;
				GOL::vertex3v( pt );
				pt += 3;
			}
			GOL::end();
		}
	}
}

void	c_bdd_time_watcher::draw_text()
{
	INT32	i;
	REAL*	pt = _point_to_draw;
	INT32	point_offset;

	if( _b_const )
	{
		i = _point_nb / 2 + 1;
		point_offset = 6;
	}
	else
	{
		i = _point_nb;
		point_offset = 3;
	}

	c_tbuf::cont_data*	pdata = _p_tbuf->get_data();
	c_tbuf::iter_data	it = pdata->begin();	//it != pdata->end(); ++it )

/*		//this not optimum
		if ( tmp < time_begin )
			time_begin = tmp;
		if ( tmp > time_end )
			time_begin = tmp;
*/
	o_str	o;
	if( i>1 )
	{
		REAL sx = REAL(.01) * g_tbuf_master->get_text_size();
		REAL sy = REAL(.03) * g_tbuf_master->get_text_size();
		while( i-->0 )
		{
			c_tdata*	src  = *it;
			o.erase();
			o.set( src->get_text()->get() );
			o.add_space();
			c_obj_ui CONST * obj = src->get_obj_ui();
			if( obj )
				o.add( obj->get_my_filename() );
			
			if( o.get_len() > 0 )
			{
				GOL::matrix::push();
					GOL::matrix::translate3v( pt );
					GOL::matrix::rotate_z_deg( 45. );
					GOL::matrix::scale( sx, sy );
					GOL::matrix::translate_y( -.5 );
					aaa::alphabet::draw_str( o.get(), 0 );
				GOL::matrix::pop();
			}
			pt += point_offset;
			++it;
		}
	}
}

void	c_bdd_time_watcher::draw_marker()
{
	REAL	time_begin	= g_tbuf_master->_time_begin;
	REAL	time_end	= g_tbuf_master->_time_end;
	REAL	time_step	= OVER_ONE_AS_FP32(_marker_per_sec);
	REAL	time		= time_end + FMOD( _marker_time_offset, time_step);

	REAL	time_factor = g_tbuf_master->_time_factor;
	REAL	time_offset = g_tbuf_master->_time_offset;

	REAL	vec[3];
	REAL	bottom = _center[_i_axe] + _marker_space_offset;
	REAL	top = bottom + _marker_space_size;

	vec[_i_v] = _center[_i_v];

	REAL	time_cur;

//	GOL::push_attrib( GL_CURRENT_BIT );
//	GOL::color4v( marker_color);

	INT32 i = 0;
	do
	{
		time_cur = time - i*time_step;
		++i;

		GOL::begin( GL_LINE_STRIP );
			vec[_i_u] = (time_cur-time_begin)*time_factor + time_offset;
			vec[_i_axe] = bottom;
			GOL::vertex3v( vec);
			vec[_i_axe] = top;
			GOL::vertex3v( vec);
		GOL::end();

//		time -= time_step;
	}
	while( time_begin <= time_cur );

//	GOL::pop_attrib();
}

//__int64 rt_cycle_last;

void	c_bdd_time_watcher::draw()
{
	if( !_p_tbuf )
		return;
/*
__int64 rt_cycle;
__asm 
{
	pushfd
	pushad
	rdtsc
	mov DWORD PTR [rt_cycle + 0], EAX
	mov DWORD PTR [rt_cycle + 4], EDX
	popad
	popfd
}
__int64 rt_cycle_dif;
	rt_cycle_dif = rt_cycle - rt_cycle_last;

//	fTicks = float(rt_cycle_dif);
	printf( "%#g\n", float(rt_cycle_dif));
	rt_cycle_last = rt_cycle_dif;
*/
	if( _sample_nb != 0 )
	{
		if( _b_draw_marker_ui )
			draw_marker();
		if( _b_draw_box_ui )
			draw_box();
		if( _b_draw_line_ui )
			draw_line();
		if( _b_draw_point_ui )
			draw_point();
		if( _b_draw_text_ui && (g_tbuf_master->is_text_always() || !g_tbuf_master->is_active()) )
			draw_text();

	}
}

//todo	draw_marker
//todo	draw_block	(block at 0 and block at 1
//todo	draw_legend
//todo	integrate color cycle and extend 
