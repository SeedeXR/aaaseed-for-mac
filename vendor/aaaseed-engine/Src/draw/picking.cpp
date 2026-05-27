#include "draw/picking.h"
#include "gol/gol.h"
#include "gol/gol_color.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/param/param_declare.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif

#if	AAA_PICKING()

CONST CHAR	PICKING_HEADER[] = "# PICKING : ";

bool	c_picking::b_allow_ui = false;
static	INT32	picking_count = 0;
 
FACTORY_CREATE_V1( c_picking, picking, Picking, picking );

CONSTRUCTOR_CREATE( c_picking )
,_nb_max(0)
,_hits(0)
,_table(nullptr)
,_pos_x( 0.)
,_pos_y(0.)
{
	//hack inside the solution to picking_ui definition order
	static	C_PCHAR_C	str_show[3] =
	{
		"No",
		"Zones",
		"Window",
	};

	static	C_PCHAR_C	str_choose[2] =
	{
		"Min",
		"Max"
	};

	CONSTEXPR INT32	BASE_PARAM_NB	= 21;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON(			allow )
		PARAM_DEF_BOOL_OFF(			active )
		PARAM_DEF_BOOL_OFF(			by_id )
		PARAM_DEF_BOOL_ON(			stop_when_no_hit )
		PARAM_DEF_REAL_LOCKED(		position_x )
		PARAM_DEF_REAL_LOCKED(		position_y )
		PARAM_DEF_REAL_POS_ONE(		size_x_pixel )
		PARAM_DEF_REAL_POS_ONE(		size_y_pixel )
		PARAM_DEF_BOOL_OFF(			verbose )
		PARAM_DEF_BOOL_OFF(			verbose_name_set )

		PARAM_DEF_SYMBO_PSTR_ZERO(	show, str_show )
		PARAM_DEF_COLOR_RGBGA(		show )
		PARAM_DEF_INT32_POS(		max, 0, 16 )
		PARAM_DEF_SYMBO_PSTR_ZERO(	choose, str_choose )

		PARAM_DEF_INT32_LOCKED(		hits )
		PARAM_DEF_UINT32_LOCKED(	hit_min )
		PARAM_DEF_UINT32_LOCKED(	hit_max )
	};

	param_init_with( param, PARAM_NB_MAX ); // picking_param, PICKING_PARAM_NB_MAX );
}

void	c_picking::param_init_pt()
{
	INT32	h=0;

	param_set_pt(		h, b_allow_ui );
	param_set_pt(		h, get_pt_active() );
	param_set_pt(		h, _b_by_id );
	param_set_pt(		h, _b_stop_when_no_hit );
	param_set_pt(		h, _pos_x );
	param_set_pt(		h, _pos_y );
	param_set_pt(		h, _size_x_pixel );
	param_set_pt(		h, _size_y_pixel );
	param_set_pt(		h, _b_verbose );
	param_set_pt(		h, _b_verbose_set_name );

	param_set_pt(		h, _s_show );
	param_set_pt_rgbfa( h, _color );
	param_set_pt(		h, _nb_max_ui );
	param_set_pt(		h, _s_choose );

	param_set_pt(		h, _hits );
	param_set_pt(		h, _hit_min );
	param_set_pt(		h, _hit_max );

	err_param_init_pt( h );
}

void	c_picking::dealloc()
{
	SAFE_DELETE_ARRAY( _table );
}

void	c_picking::alloc()
{
	dealloc();
	_table = new UINT32[_nb_max_ui];
	_nb_max = _nb_max_ui;
}

c_picking::~c_picking()
{
	dealloc();
}

void	c_picking::update()
{
	if( is_allow() && is_active() )
	{
		picking_cur = this;
		if( _nb_max_ui != _nb_max )
			alloc();
	}
	else
		clear_cur();

	FP32  xy[2];
	c_mouse::get_cur()->get_xy_render( xy[0], xy[1] );
	_pos_x = REAL(xy[0]);
	_pos_y = REAL(xy[1]);
}

void	c_picking::set_pick_ref( OBJ_UI_ID index )
{
	if( index == 0 )
		debug_break( "%s() zero is a bad pick ref", __FUNCTION__ );
	if( _b_verbose_set_name )
		HEADER_PRINT_STRING( PICKING_HEADER, "%d : set %u name ", picking_count, index );
	glLoadName( index );
}

void	c_picking::push_name(INT32 i)
{
	if( _b_verbose_set_name )
		HEADER_PRINT_STRING( PICKING_HEADER, "%d : push %d name ", picking_count, i );
	glPushName(i);
}

void	c_picking::pop_name()
{
	if( _b_verbose_set_name )
		HEADER_PRINT_STRING( PICKING_HEADER, "%d : pop ", picking_count );
	glPopName();
}

void	c_picking::begin()
{
	if( _s_show == 0 )
	{
		glSelectBuffer( _nb_max, _table );
		glRenderMode( GL_SELECT );
		glInitNames();
		push_name(0);
	}
	else
	{
		GOL::color4(	_color[0] * _color[4],
						_color[1] * _color[4],
						_color[2] * _color[4],
						_color[3]
					);
		GOL::enable_alpha_test();
		GOL::set_alpha_func( GL_GREATER, GLclampf(0.01) );

		GOL::enable_blend();
		GOL::set_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		GOL::set_depth_func( GL_ALWAYS );
	}
}

void	c_picking::end()
{
	if( _s_show != 0 )
		return;

	pop_name();
	_hits = glRenderMode( GL_RENDER );
	++picking_count;
	//transfert hits to picked
	g_picked_def.analyse_hits( _hits, _table );
	if( _b_verbose )
		print_hits();
	post_hit();
}

void	c_picking::print_hits()
{
	if( !_hits )
		return;
	UINT32* pt = _table;
	HEADER_PRINT_STRING( PICKING_HEADER, "%d : %d objects selected", picking_count, _hits );
	for( INT32 i = _hits; i > 0; --i )
	{
		UINT32	nb		= *pt++;
		REAL	z1		= (REAL) *pt++/0x7fffffff;
		REAL	z2		= (REAL) *pt++/0x7fffffff;
		UINT32	index	= *pt++;
		HEADER_PRINT_STRING( PICKING_HEADER, "%d : %d selected with z1 %g z2 %g", picking_count, index, z1, z2 );
		for( INT32 j = nb - 1; j > 0; --j )
		{
			HEADER_PRINT_STRING( PICKING_HEADER, "%d : \t%d selected ", picking_count, *pt++ );
		}
	}
}

void	c_picking::trig_for_ui()
{
	if( !is_allow() )
		return;
	set_active( true );
	set_by_id( true ); 
	_b_trig_for_ui = true;
}

void	c_picking::post_hit()
{
	if( _hits )
	{
		UINT32	nb;
		UINT32	tmp;
		UINT32*	pt;
		
		pt = _table;
		_hit_min = aaa::BIGGEST<UINT32>;
		_hit_max = 0;
		for( INT32 i = _hits; i > 0; --i )
		{
			nb = *pt;
			pt += 3;
			for( INT32 j = nb; j > 0; --j )
			{
				tmp = *pt++;
				if( _hit_min > tmp )
					_hit_min = tmp;
				if( _hit_max < tmp )
					_hit_max = tmp;
			}
		}
		if( _b_trig_for_ui )
		{
			c_obj_ui::OBJ_UI_ID	picked_id	= _s_choose ? _hit_max : _hit_min;
			c_obj_ui*			picked		= c_obj_ui::get_from_id( picked_id );
			if( picked )
			{
				((c_layer*)picked)->become_ui();
				HEADER_PRINT_STRING( PICKING_HEADER, "%d : %u picked ", picking_count, picked_id );
			}
			else
				ERR_PRINT_STRING( "%s%d :%u picked but can't select layer", PICKING_HEADER, picking_count, picked_id );
				
			set_active( false );
			set_by_id( false ); 
			_b_trig_for_ui = false;
		}
	}
	else
	{
		_hit_min = 0;
		_hit_max = 0;
		if( _b_trig_for_ui && _b_stop_when_no_hit )
		{
			set_active( false );
			set_by_id( false ); 
			_b_trig_for_ui = false;
		}
	}
}

void	c_picking::do_matrix()
{
	if( _s_show == 1 )
		return;
	GLint	viewport[4];
	GOL::get_integer( GL_VIEWPORT, viewport );
	gluPickMatrix(	viewport[0] + _pos_x * viewport[2],
					viewport[1] + _pos_y * viewport[3],
					_size_x_pixel, _size_y_pixel,
					viewport );
}



c_picking*	c_picking::picking_ui = nullptr;
c_picking*	c_picking::picking_cur = nullptr;

void c_picking::c_init()
{
	::obj_get( picking_ui );
	picking_cur = picking_ui;
}
void c_picking::c_deinit()
{
	SAFE_DELETE( picking_ui );
}
#endif	//AAA_PICKING()
