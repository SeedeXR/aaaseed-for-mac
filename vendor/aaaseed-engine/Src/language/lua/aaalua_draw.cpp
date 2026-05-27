#include "aaalua_draw.h"
#include "aaalua_util.h"
#include "aaalua_glue.h"
		 
#include "draw/color.h"
#include "draw/axe.h"
#include "draw/rect.h"
#include "draw/shape.h"
#include "draw/box.h"
#include "draw/line.h"
#include "draw/geo/sphere.h"
		 
#include "ui/alphabet.h"
#include "image/bind_img.h"

namespace aaalua
{
namespace n_draw
{
	//hack for Garden
	AAALUACALL(	draw_rgba )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 1 );
		c_color*	color	=	l.get_aaa_obj_of_class<c_color>( 1 );
		color->draw();
		return l.return_nothing();
	}
	AAALUACALL(	draw_rgb )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_color*	color	=	l.get_aaa_obj_of_class<c_color>( 1 );
		color->draw_no_alpha();
		return l.return_nothing();
	}

	//	draw_null( x,y,z, size )
	AAALUACALL(	draw_null )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 0, 1, 3, 4 );
		switch( nb_arg )
		{
		case 0:		n_axe::draw_null_3d();					break;
		case 1:		n_axe::draw_null_3d( l.get_fp32(1) );	break;
		case 3:		{
						l.get_v3( g_fp32, 1 );
						n_axe::draw_null_3d( g_fp32 );
					}
					break;
		case 4:		{
						l.get_v4( g_fp32, 1 );
						n_axe::draw_null_3d( g_fp32, g_fp32[3] );
					}
					break;
		}
		return l.return_nothing();
	}

	AAALUACALL(	draw_null_2d )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb_min_max( 0, 5 );
		switch( nb_arg )
		{
		case 0:	n_axe::draw_null_2d();									break;
		case 1:	n_axe::draw_null_2d( l.get_fp32(1) );					break;
		case 2:	n_axe::draw_null_2d( l.get_fp32(1), l.get_int32(2) );	break;
		case 3:
			{
				l.get_v3( g_fp32, 1 );
				n_axe::draw_null_2d( g_fp32 );
			}
			break;
		case 4:
			{
				l.get_v4( g_fp32, 1 );
				n_axe::draw_null_2d( g_fp32, g_fp32[3] );
			}
			break;
		case 5:
			{
				l.get_v4( g_fp32, 1 );
				n_axe::draw_null_2d( g_fp32, g_fp32[3], l.get_int32(5) );
			}
			break;
		}
		return l.return_nothing();
	}

	//	draw_axe_and_plane( planes )
	AAALUACALL(	draw_axe_and_plane )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		n_axe::draw_axe_and_plane( nb_arg==1 ? l.get_int32( 1 ) : 0 );
		return l.return_nothing();
	}

	//	draw_cube( x,y,z, size )
	AAALUACALL(	draw_cube )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 0, 3, 4 );
		if( nb_arg == 0 )
			::draw_cube_cano();
		else
		{
			l.get_v3( g_fp32, 1 );
			::draw_cube_at( nb_arg==3 ? FP32(1.) : l.get_fp32(4), g_fp32 );
		}
		return l.return_nothing();
	}

	//	draw_box( sx,sy,sz,	x,y,z )
	AAALUACALL(	draw_box )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 3, 6 );
	
		if( nb_arg==6 )
		{
			l.get_v6( g_fp32, 1 );
			::draw_box_at( g_fp32, g_fp32+3 );
		}
		else
		{
			l.get_v3( g_fp32, 1 );
			::draw_box( g_fp32 );
		}
		return l.return_nothing();
	}

	//	draw_box_using_transfo(	sx,sy,sz,		x,y,z )
	AAALUACALL(	draw_box_using_transfo )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 3, 6 );

		if( nb_arg==6 )
		{
			l.get_v6( g_fp32, 1 );
			GOL::matrix::translate3v( g_fp32+3 );
		}
		else
			l.get_v3( g_fp32, 1 );

		GOL::matrix::scale3v( g_fp32 );
		::draw_cube_cano();

		return l.return_nothing();
	}

	//	draw_box_line( sx,sy,sz,	x,y,z )
	AAALUACALL(	draw_box_line )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 3, 6 );

		
		if( nb_arg==6 )
		{
			l.get_v6( g_fp32, 1 );
			::draw_box_at_line( g_fp32, g_fp32+3 );
		}
		else
		{
			l.get_v3( g_fp32, 1 );
			::draw_box_line( g_fp32 );
		}
		return l.return_nothing();
	}

	//	draw_box_top_line( sx,sy,sz,	x,y,z )
	AAALUACALL(	draw_box_top_line )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 3, 6 );

		
		if( nb_arg==6 )
		{
			l.get_v6( g_fp32, 1 );
			::draw_box_top_line_at( g_fp32, g_fp32+3 );
		}
		else
		{
			l.get_v3( g_fp32, 1 );
			::draw_box_top_line( g_fp32 );
		}
		return l.return_nothing();
	}

	//	draw_sphere( size, u_nb, v_nb )
	AAALUACALL(	draw_sphere )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb_min_max( 1, 3 );
		REAL	CONST	size = l.get_fp32(1);
		INT32	CONST	nb_u = (nb_arg>=2) ? l.get_int32(2) : 40;
		INT32	CONST	nb_v = (nb_arg>=3) ? l.get_int32(3) : 20;

		::draw_sphere( size, nb_u,nb_v );
		return l.return_nothing();
	}

	//	draw_sphere_at( x,y,z, size, u_nb, v_nb )
	AAALUACALL(	draw_sphere_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb_min_max( 4, 6 );
		l.get_v3( g_fp32, 1 );
		g_fp32[5] = g_fp32[4] = g_fp32[3] = l.get_fp32(4);
		INT32	CONST	nb_u = (nb_arg>=5) ? l.get_int32(5) : 40;
		INT32	CONST	nb_v = (nb_arg>=6) ? l.get_int32(6) : 20;

		::draw_sphere_at( g_fp32+3, g_fp32, nb_u,nb_v );
		return l.return_nothing();
	}

	//	draw_teapot( size, grid_nb )
	AAALUACALL(	draw_teapot )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 1, 2 );
		FP32	CONST	size = l.get_fp32( 1 );
		INT32	CONST	grid_nb = (nb_arg>=2) ? l.get_int32(2) : 10 ;

		g_fp32[2] = g_fp32[1] = g_fp32[0] = size;
		::draw_teapot( nullptr, g_fp32, grid_nb );
		return l.return_nothing();
	}
// CIRCLE
// 
	template<INT32 AXE>
	static FINLINE INT32 draw_circle_axe_low( c_lua_state& l, INT32 prim ) NOEXCEPT
	{
		INT32	CONST	nb_arg	= l.get_arg_nb( 4, 5 );
		l.get_v4( g_fp32, 1 );
		INT32	CONST	nb_arc	= nb_arg==5 ? l.get_int32( 5 ) : 12;
		if CONSTEXPR (AXE == 0)
			draw_circle_x_at( g_fp32, g_fp32[3], prim, nb_arc );
		else if CONSTEXPR (AXE == 1)
			draw_circle_y_at( g_fp32, g_fp32[3], prim, nb_arc );
		else
			draw_circle_z_at( g_fp32, g_fp32[3], prim, nb_arc );
		return l.return_nothing();
	}
// CIRCLE DISK X
	//	draw_circle_axe_x( x,y,z, size, nb_arc )
	AAALUACALL(	draw_circle_axe_x )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_circle_axe_low<0>( l, GL_LINE_LOOP );	}
	//	draw_disk_axe_x( x,y,z, size, nb_arc )
	AAALUACALL(	draw_disk_axe_x )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_circle_axe_low<0>( l, GL_TRIANGLE_FAN );		}
// CIRCLE DISK Y
	//	draw_circle_axe_z( x,y,z, size, nb_arc )
	AAALUACALL(	draw_circle_axe_y )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_circle_axe_low<1>( l, GL_LINE_LOOP );	}
	//	draw_disk_axe_y( x,y,z, size, nb_arc )
	AAALUACALL(	draw_disk_axe_y )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_circle_axe_low<1>( l, GL_TRIANGLE_FAN );		}
// CIRCLE DISK Z
	//	draw_circle_axe_z( x,y,z, size, nb_arc )
	AAALUACALL(	draw_circle_axe_z )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_circle_axe_low<2>( l, GL_LINE_LOOP );	}
	//	draw_disk_axe_z( x,y,z, size, nb_arc )
	AAALUACALL(	draw_disk_axe_z )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_circle_axe_low<2>( l, GL_TRIANGLE_FAN );		}
// CIRCLE DISK ARC Z
	static INT32 draw_circle_arc_axe_z_low( c_lua_state& l, INT32 prim )
	{
		INT32	CONST	nb_arg	= l.get_arg_nb( 6, 7 );
		l.get_v4( g_fp32, 1 );
		FP32	CONST	_angle_begin	= l.get_fp32( 5 );
		FP32	CONST	angle			= l.get_fp32( 6 );
		INT32	CONST	nb_arc			= nb_arg==7 ? l.get_int32( 7 ) : 12;

		draw_circle_arc_z_at( g_fp32, g_fp32[3], _angle_begin, angle, prim, nb_arc );
		return l.return_nothing();
	}
	//	draw_circle_arc_axe_z( x,y,z, size, nb_arc )
	AAALUACALL(	draw_circle_arc_axe_z )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_circle_arc_axe_z_low( l, GL_LINE_STRIP );	}
	//	draw_disk_arc_axe_z( x,y,z, size, nb_arc )
	AAALUACALL(	draw_disk_arc_axe_z )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_circle_arc_axe_z_low( l, GL_TRIANGLE_FAN );	}

// ELLIPSE
//
	template< INT32 AXE>
	static FINLINE INT32 draw_ellipse_axe_low( c_lua_state& l, INT32 prim ) NOEXCEPT
	{
		INT32	CONST	nb_arg	= l.get_arg_nb( 5, 6 );
		l.get_v5( g_fp32, 1 );
		INT32	CONST	nb_arc	= nb_arg==6 ? l.get_int32( 6 ) : 12;
		if CONSTEXPR ( AXE==0 )
			draw_ellipse_axe_x( g_fp32, g_fp32[3],g_fp32[4], prim, nb_arc );
		else if CONSTEXPR ( AXE==1 )
			draw_ellipse_axe_y( g_fp32, g_fp32[3],g_fp32[4], prim, nb_arc );
		else
			draw_ellipse_axe_z( g_fp32, g_fp32[3],g_fp32[4], prim, nb_arc );
		return l.return_nothing();
	}
// ELLIPSE X
// 	//	draw_ellipse_line_axe_x( x,y,z, size_u, size_v, nb_arc )
	AAALUACALL(	draw_ellipse_line_axe_x )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_ellipse_axe_low<0>( l, GL_LINE_LOOP );		}
	//	draw_ellipse_axe_x( x,y,z, size_u, size_v, nb_arc )
	AAALUACALL(	draw_ellipse_axe_x )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_ellipse_axe_low<0>( l, GL_TRIANGLE_FAN );	}
// ELLIPSE Y
	//	draw_ellipse_line_axe_y( x,y,z, size_u, size_v, nb_arc )
	AAALUACALL(	draw_ellipse_line_axe_y )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_ellipse_axe_low<1>( l, GL_LINE_LOOP );		}
	//	draw_ellipse_axe_y( x,y,z, size_u, size_v, nb_arc )
	AAALUACALL(	draw_ellipse_axe_y )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_ellipse_axe_low<1>( l, GL_TRIANGLE_FAN );	}
// ELLIPSE Z 
	//	draw_ellipse_line_axe_z( x,y,z, size_u, size_v, nb_arc )
	AAALUACALL(	draw_ellipse_line_axe_z )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_ellipse_axe_low<2>( l, GL_LINE_LOOP );		}
	//	draw_ellipse_axe_z( x,y,z, size_u, size_v, nb_arc )
	AAALUACALL(	draw_ellipse_axe_z )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_ellipse_axe_low<2>( l, GL_TRIANGLE_FAN );	}
	

	// RECTANGLE
	//	draw_rect_axe_x/y/z( x,y,z, size_u, size_v )
	template< INT32 iu, INT32 iv, INT32 ia, INT32 prim >
	FINLINE static INT32 draw_rect_axe_low( c_lua_state& l )
	{
		INT32 CONST	arg_nb	= l.get_arg_nb_min_max( 3, 6 );
		if( arg_nb == 6 )
		{
			l.get_v6( g_fp32, 1 );
			if( ia==2 )
				::draw_rect_prim_xyz_sxy_rotz( prim, g_fp32 );
			else if( ia==1 )
				::draw_rect_prim_xyz_szx_roty( prim, g_fp32 );
			else
				::draw_rect_prim_xyz_syz_rotx( prim, g_fp32 );
		}
		else
		{
			if( arg_nb == 5 )		
				l.get_v5( g_fp32, 1 );
			else
			{
				l.get_v3( g_fp32, 1 );
				g_fp32[4] = g_fp32[3] = (arg_nb == 4) ? l.get_fp32( 4 ) : FP32(1);
			}
			::draw_rect_axes< iu,iv,ia, prim>( g_fp32, g_fp32[3],g_fp32[4] );
		}
		return l.return_nothing();
	}
	AAALUACALL(	draw_rect_line_axe_x )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_rect_axe_low< 1,2,0, GL_LINE_LOOP	>( l );	}
	AAALUACALL(	draw_rect_axe_x )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_rect_axe_low< 1,2,0, GL_TRIANGLE_FAN>( l );	}
	AAALUACALL(	draw_rect_line_axe_y )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_rect_axe_low< 2,0,1, GL_LINE_LOOP	>( l );	}
	AAALUACALL(	draw_rect_axe_y )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_rect_axe_low< 2,0,1, GL_TRIANGLE_FAN>( l );	}
	AAALUACALL(	draw_rect_line_axe_z )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_rect_axe_low< 0,1,2, GL_LINE_LOOP	>( l );	}
	AAALUACALL(	draw_rect_axe_z )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_rect_axe_low< 0,1,2, GL_TRIANGLE_FAN>( l );	}

	//	draw_rect_uv_axe_x/y/z( x,y,z, size_u, size_v )
	template< INT32 iu, INT32 iv, INT32 ia, INT32 prim >
	FINLINE static INT32 draw_rect_uv_axe_low( c_lua_state& l )
	{
		INT32 CONST arg_nb = l.get_arg_nb_min_max( 3, 6 );
		if( arg_nb == 6  )
		{
			FP32 vec[6];
			l.get_v6( vec, 1 );
			if( ia==2 )
				::draw_rect_uv_xyz_sxy_rotz( vec );
			else if( ia==1 )
				::draw_rect_uv_xyz_szx_roty( vec );
			else
				::draw_rect_uv_xyz_syz_rotx( vec );
		}
		else
		{
			if( arg_nb == 5  )		
				l.get_v5( g_fp32, 1 );
			else
			{
				l.get_v3( g_fp32, 1 );
				g_fp32[1] = g_fp32[0] = (arg_nb == 4) ? l.get_fp32( 4 ) : FP32(1.);
			}
			::draw_rect_uv_axes< iu,iv,ia, prim>( g_fp32, g_fp32[3],g_fp32[4] );
		}
		return l.return_nothing();
	}
	AAALUACALL(	draw_rect_uv_axe_x )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_rect_uv_axe_low< 1,2,0, GL_TRIANGLE_STRIP>( l );	}
	AAALUACALL(	draw_rect_uv_axe_y )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_rect_uv_axe_low< 2,0,1, GL_TRIANGLE_STRIP>( l );	}
	AAALUACALL(	draw_rect_uv_axe_z )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_rect_uv_axe_low< 0,1,2, GL_TRIANGLE_STRIP>( l );	}

	//	draw_rect( min_x, min_y, max_x, max_y )
	AAALUACALL(	draw_rect )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//l.check_arg_nb( 4 );
		l.get_v4( g_fp32, -4 );
		::draw_rect( g_fp32 );
		return l.return_nothing();
	}
	//	draw_rect_cano()
	AAALUACALL(	draw_rect_cano )
	{
		::draw_rect_cano();
		return 0;
	}
	//	draw_rect_size(  [size [,size_v]] )
	AAALUACALL(	draw_rect_size )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST arg_nb = l.get_arg_nb( 0,1,2 );
		switch( arg_nb )
		{
		case 0:	::draw_rect_cano();	break;
		case 1:	::draw_rect_size( l.get_fp32_or_1( 1 ) );	break;
		case 2:
			{			
				l.get_v2( g_fp32, -2 );
				::draw_rect_size( g_fp32 );
			}
			break;
		}		
		return l.return_nothing();
	}
	//	draw_rect_at_z( min_x, min_y, max_x, max_y, z )
	AAALUACALL(	draw_rect_at_z )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 5 );
		l.get_v5( g_fp32, 1 );
		::draw_rect_at_z( g_fp32 );
		return l.return_nothing();
	}
	//	draw_rect_uv_cano()
	AAALUACALL(	draw_rect_uv_cano )
	{
		::draw_rect_uv_cano();
		return 0;
	}
	//	draw_rect_uv( min_x, min_y, max_x, max_y )
	AAALUACALL(	draw_rect_uv )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 4 );
		l.get_v4( g_fp32, -4 );
		::draw_rect_uv( g_fp32 );
		return l.return_nothing();
	}
	//	draw_rect_uv_size( [size [,size_v]] )
	AAALUACALL(	draw_rect_uv_size )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST arg_nb = l.get_arg_nb( 0,1,2 );
		switch( arg_nb )
		{
		case 0:	::draw_rect_uv_cano();					break;
		case 1:	::draw_rect_uv_size( l.get_fp32_or_1( 1 ) );	break;
		case 2:
			{			
				l.get_v2( g_fp32, -2 );
				::draw_rect_uv_size( g_fp32 );
			}
			break;
		}
		return l.return_nothing();
	}
	//	draw_triangle_strip_4xy_uv( x0 y0, x1,y1, x2,y2, x3,y3 )
	AAALUACALL(	draw_triangle_strip_4xy_uv )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 8 );
		l.get_v8( g_fp32, 1 );
		::draw_triangle_strip_4xy_uv( g_fp32 );
		return l.return_nothing();
	}
	//	draw_triangle_strip_4xyz_uv( x0,y0,z0, x1,y1,z1, x2,y2,z2, x3,y3,z3 )
	AAALUACALL(	draw_triangle_strip_4xyz_uv )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 12 );
		l.get_v12( g_fp32, 1 );
		::draw_triangle_strip_4xyz_uv( g_fp32 );
		return l.return_nothing();
	}
	//	draw_rect_uv_at_z( min_x, min_y, max_x, max_y, z )
	AAALUACALL(	draw_rect_uv_at_z )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 5 );
		l.get_v5( g_fp32, 1 );
		::draw_rect_uv_at_z( g_fp32 );
		return l.return_nothing();
	}

	//	draw_rect_line_cano()
	AAALUACALL(	draw_rect_line_cano )
	{
		::draw_rect_line_cano();
		return 0;
	}
	//	draw_rect_line( min_x, min_y, max_x, max_y )
	AAALUACALL(	draw_rect_line )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 4 );
		l.get_v4( g_fp32, -4 );
		::draw_rect_line( g_fp32 );
		return l.return_nothing();
	}
	//	draw_rects_line( min_x, min_y, max_x, max_y [,min_x, min_y, max_x, max_y...] )
	AAALUACALL(	draw_rects_line )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 arg_nb = l.get_arg_nb_multiple<4>();
		INT32 i = 1;
		do
		{
			l.get_v4( g_fp32, i );
			::draw_rect_line( g_fp32 );
			i += 4;
		}
		while( i < arg_nb );
		return l.return_nothing();
	}
	//	draw_rect_line_size( [size [,size_v]] )
	AAALUACALL(	draw_rect_line_size )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST arg_nb = l.get_arg_nb( 0,1,2 );
		switch( arg_nb )
		{
		case 0:	::draw_rect_line_cano();					break;
		case 1:	::draw_rect_line_size( l.get_fp32_or_1( 1 ) );	break;
		case 2:
			{			
				l.get_v2( g_fp32, -2 );
				::draw_rect_line_size( g_fp32 );
			}
			break;
		}		
		return l.return_nothing();
	}

	//	draw_rect_line_at_z( min_x, min_y, max_x, max_y, z )
	AAALUACALL(	draw_rect_line_at_z )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 5 );
		l.get_v5( g_fp32, 1 );
		::draw_rect_line_at_z( g_fp32 );
		return l.return_nothing();
	}

	//	aaa.draw_bind_rect( bind, x_min,y_min, x_max,y_max )
	AAALUACALL(	draw_bind_rect )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 5 );
		l.get_v4( g_fp32, -4 );
		INT32 CONST	dim = GOL::get_texture_dim();
		if( dim != 2 )
			GOL::set_texture_dim_private( 2 );
		if( !l.is_nil(-5) )
			tex_2d_bind( l.get_int32(-5) );
		::draw_rect_uv( g_fp32, c_map::quad_uv_01[0] );
		if( dim != 2 )
			GOL::set_texture_dim( dim );
		return l.return_nothing();
	}

	//	draw_tri_line( x,y,s )
	//	draw_tri_line( x,y, sx,sy )
	AAALUACALL(	draw_tri_line )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	arg_nb = l.get_arg_nb( 3, 4 );
		if( arg_nb==3 )
		{
			l.get_v3( g_fp32, -3 );
			::draw_tri_line( g_fp32[0], g_fp32[1], g_fp32[2] );
		}
		else
		{
			l.get_v4( g_fp32, -4 );
			::draw_tri_line( g_fp32[0], g_fp32[1], g_fp32[2], g_fp32[3] );
		}
		return l.return_nothing();
	}
	//	draw_mul_line( x,y, s )
	//	draw_mul_line( x,y, sx,sy )
	AAALUACALL(	draw_mul_line )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	arg_nb = l.get_arg_nb( 3, 4 );
		if( arg_nb==3 )
		{
			l.get_v3( g_fp32, -3 );
			::draw_mul_line( g_fp32[0], g_fp32[1], g_fp32[2] );
		}
		else
		{
			l.get_v4( g_fp32, -4 );
			::draw_mul_line( g_fp32[0], g_fp32[1], g_fp32[2], g_fp32[3] );
		}
		return l.return_nothing();
	}
	//	draw_plus_line( x,y, s )
	//	draw_plus_line( x,y, sx,sy )
	AAALUACALL(	draw_plus_line )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	arg_nb = l.get_arg_nb( 3, 4 );
		if( arg_nb==3 )
		{
			l.get_v3( g_fp32, -3 );
			::draw_plus_line( g_fp32[0], g_fp32[1], g_fp32[2] );
		}
		else
		{
			l.get_v4( g_fp32, -4 );
			::draw_plus_line( g_fp32[0], g_fp32[1], g_fp32[2], g_fp32[3] );
		}
		return l.return_nothing();
	}

	//	draw_line(	x1,y1,		x2,y2 )
	//	draw_line(	x1,y1,z1,	x2,y2,z2 )
	AAALUACALL(	draw_line )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 4, 6 );
		if( nb_arg == 4 )
		{
			l.get_v4( g_fp32, -4 );
			::draw_line_z( g_fp32 );
		}
		else
		{
			l.get_v6( g_fp32, 1 );
			::draw_line( g_fp32, g_fp32+3 );
		}
		return l.return_nothing();
	}

	//	draw_line_x( x1,x2 )
	AAALUACALL(	draw_line_x )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 2 );
		l.get_v2( g_fp32, 1 );
		::draw_line_x( g_fp32 );
		return l.return_nothing();
	}

	//	draw_line_y( y1,y2 )
	AAALUACALL(	draw_line_y )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 2 );
		l.get_v2( g_fp32, 1 );
		::draw_line_y( g_fp32 );
		return l.return_nothing();
	}

	//	draw_line_uv( u,v, du,dv, axe )
	AAALUACALL(	draw_line_uv )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 4 );
		FP32 CONST u	= l.get_fp32( 1 );
		FP32 CONST v	= l.get_fp32( 2 );
		FP32 CONST du	= l.get_fp32( 3 );
		FP32 CONST dv	= l.get_fp32( 4 );

		::draw_line_uv( u,v, du,dv, 2 );
		return l.return_nothing();
	}

	//	draw_line_vert_nb( nb, x, step, bottom, top )
	AAALUACALL( draw_lines_vert_nb )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 5 );
		::draw_lines_vert_nb( l.get_value_direct<UINT32>(-5),
							l.get_value_direct<REAL>(-4), l.get_value_direct<REAL>(-3), l.get_value_direct<REAL>(-2), l.get_value_direct<REAL>(-1) );
		return l.return_nothing();
	}
	//	draw_lines_hori_nb( nb, y, step, left, right )
	AAALUACALL( draw_lines_hori_nb )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 5 );
		::draw_lines_hori_nb( l.get_value_direct<UINT32>(-5),\
							l.get_value_direct<REAL>(-4), l.get_value_direct<REAL>(-3), l.get_value_direct<REAL>(-2), l.get_value_direct<REAL>(-1) );
		return l.return_nothing();
	}

	static INT32 draw_str_common( c_lua_state& l, INT32 font )
	{
		INT32	CONST	nb_arg = l.get_arg_nb_min( 1 );
		C_PCHAR text;
		for( INT32 i = 1; i <= nb_arg; ++i )
		{
			text = l.get_str( i );
			if( text )
				aaa::alphabet::draw_str( text, font );
			else
				l.error_and_escape( "not a string, try to draw %s", l.get_type_str( l.get_type(i) ) );
		}
		return l.return_nothing();
	}
	//	draw_str( str, ... );
	AAALUACALL( draw_str )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_str_common( l, 1 );	}
	//	draw_str_maa( str, ... );
	AAALUACALL( draw_str_maa )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_str_common( l, 0 );	}

	// str, x,y, sx,sy
	static INT32 draw_str_xy_common( c_lua_state& l, INT32 font )
	{
		INT32	CONST	nb_arg = l.get_arg_nb_min_max( 1, 5 );
		C_PCHAR_C text	= l.get_str( 1 );
		if( text )
		{
			if( nb_arg > 1 )
			{
				REAL	CONST	x = l.get_value_direct<REAL>(2);
				REAL	CONST	y = ( nb_arg > 2 ) ? l.get_value_direct<REAL>(3) : 0;
				if( nb_arg > 3 )
				{
					REAL	CONST	sx = l.get_value_direct<REAL>(4);
					REAL	CONST	sy = ( nb_arg > 4 ) ? l.get_value_direct<REAL>(5) : 1;
					aaa::alphabet::draw_str_xy( text, x,y, sx,sy, font );
				}
				else
					aaa::alphabet::draw_str_xy( text, x,y, 1.,1., font );
			}
			else
			{
				aaa::alphabet::draw_str_xy( text, 0.,0., 1.,1., font );
			}
		}
		else
			l.error_and_escape( "not a string, try to draw %s", l.get_type_str( l.get_type(1) ) );
		return l.return_nothing();
	}

	//	draw_str_xy( str, x,y, sx,sy )
	AAALUACALL( draw_str_xy )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_str_xy_common( l, 1 );	}
	//	draw_str_maa_xy( str, x,y, sx,sy )
	AAALUACALL( draw_str_maa_xy )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_str_xy_common( l, 0 );	}

	// str, x,y,z, sx,sy
	static INT32 draw_str_xyz_common( c_lua_state& l, INT32 font )
	{
		INT32	CONST	nb_arg = l.get_arg_nb_min_max( 4, 6 );
		C_PCHAR_C text	= l.get_str( 1 );
		if( text )
		{
			l.get_v3( g_fp32, 2 );
			if( nb_arg > 4 )
			{
				REAL	CONST	sx = l.get_value_direct<REAL>(5);
				REAL	CONST	sy = ( nb_arg > 4 ) ? l.get_value_direct<REAL>(6) : 1;
				aaa::alphabet::draw_str_v3( text, g_fp32, sx,sy, font );
			}
			else
				aaa::alphabet::draw_str_v3( text, g_fp32, 1.,1., font );
		}
		else
			l.error_and_escape( "not a string, try to draw %s", l.get_type_str( l.get_type(1) ) );
		return l.return_nothing();
	}

	//	draw_str_xzy( str, x,y,z, sx,sy )
	AAALUACALL( draw_str_xyz )		{	LUAAAA_START( L, __FUNCTION__ );	return draw_str_xyz_common( l, 1 );	}
	//	draw_str_maa_xyz( str, x,y,z, sx,sy )
	AAALUACALL( draw_str_maa_xyz )	{	LUAAAA_START( L, __FUNCTION__ );	return draw_str_xyz_common( l, 0 );	}

	//	get_str_translate( str )
	AAALUACALL(	get_str_translate )
	{
		LUAAAA_START( L, __FUNCTION__ );
		// DBG_CHECK_ARG_NB( 1 );
		C_PCHAR_C text = l.get_str( 1 );
		REAL vec[3];
		aaa::alphabet::get_str_translate( vec, text, 1 );
		return l.return_real_v2( vec );
	}

	void	register_draw( lua_State *L )
	{
		LUAAAA_START( L, __FUNCTION__ );

	//	lua_pushnumber(L, 0 );
	//	lua_setglobal(L, "aaa_out" );
		STACK_ENFORCE_STORE( &l );

			ADD_FN( draw_rgb				);
			ADD_FN( draw_rgba				);

			ADD_FN( draw_null				);
			ADD_FN( draw_null_2d			);
			ADD_FN( draw_axe_and_plane		);

			ADD_FN( draw_cube				);
			ADD_FN( draw_box				);
			ADD_FN( draw_box_using_transfo	);
			ADD_FN( draw_box_line			);
			ADD_FN( draw_box_top_line		);
			ADD_FN( draw_sphere				);
			ADD_FN( draw_sphere_xyz			);
			ADD_FN( draw_teapot				);

			ADD_FN( draw_line_x				);
			ADD_FN( draw_line_y				);
			ADD_FN( draw_line				);
			ADD_FN( draw_line_uv			);

			ADD_FN( draw_lines_vert_nb		);
			ADD_FN( draw_lines_hori_nb		);

			ADD_FN( draw_rect_cano			);
			ADD_FN( draw_rect				);
			ADD_FN( draw_rect_size			);
			ADD_FN( draw_rect_at_z			);
			ADD_FN( draw_rect_axe_x			);
			ADD_FN( draw_rect_axe_y			);
			ADD_FN( draw_rect_axe_z			);

			ADD_FN( draw_rect_line_cano		);
			ADD_FN( draw_rect_line			);
			ADD_FN( draw_rects_line			);
			ADD_FN( draw_rect_line_size		);
			ADD_FN( draw_rect_line_at_z		);
			ADD_FN( draw_rect_line_axe_x	);
			ADD_FN( draw_rect_line_axe_y	);
			ADD_FN( draw_rect_line_axe_z	);

			ADD_FN( draw_rect_uv_cano		);
			ADD_FN( draw_rect_uv			);
			ADD_FN( draw_rect_uv_size		);
			ADD_FN( draw_rect_uv_at_z		);
			ADD_FN( draw_rect_uv_axe_x		);
			ADD_FN( draw_rect_uv_axe_y		);
			ADD_FN( draw_rect_uv_axe_z		);

			ADD_FN( draw_triangle_strip_4xy_uv	);
			ADD_FN( draw_triangle_strip_4xyz_uv	);

			ADD_FN( draw_bind_rect			);

			ADD_FN( draw_circle_axe_x		);
			ADD_FN( draw_disk_axe_x			);
			ADD_FN( draw_circle_axe_y		);
			ADD_FN( draw_disk_axe_y			);
			ADD_FN( draw_circle_axe_z		);
			ADD_FN( draw_disk_axe_z			);

			ADD_FN( draw_circle_arc_axe_z	);
			ADD_FN( draw_disk_arc_axe_z		);

			ADD_FN(	draw_ellipse_line_axe_x	);
			ADD_FN(	draw_ellipse_line_axe_y	);
			ADD_FN(	draw_ellipse_line_axe_z	);
			ADD_FN(	draw_ellipse_axe_x		);
			ADD_FN(	draw_ellipse_axe_y		);
			ADD_FN(	draw_ellipse_axe_z		);

			ADD_FN( draw_tri_line			);
			ADD_FN( draw_mul_line			);
			ADD_FN( draw_plus_line			);

			ADD_FN( draw_str				);
			ADD_FN( draw_str_xy				);
			ADD_FN( draw_str_xyz			);
			ADD_FN( draw_str_maa			);
			ADD_FN( draw_str_maa_xy			);
			ADD_FN( draw_str_maa_xyz		);
			ADD_FN( get_str_translate		);

		STACK_ENFORCE_SAME( &l );
	}
}	//end namespace n_draw
}	//end namespace aaalua
