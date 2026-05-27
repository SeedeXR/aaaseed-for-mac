#include "bdd_lua.h"
#include "bdd_lua_point.h"
#include "language/lua/aaalua_glue.h"
#include "obj_ui/bdd/bdd_point/bdd_blob.h"
#include "obj_ui/bdd/bdd_multitouch/bdd_blob_tracking.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/bdd/bdd_edit/bdd_curve3d.h"
#include "obj_ui/bdd/bdd_edit/bdd_line_3d.h"
#include "obj_ui/bdd/bdd_opencl_part.h"
#include "obj_ui/bdd/bdd_geo/bdd_tube_path.h"
#include "obj_ui/bdd/bdd_edit/bdd_curve_edit.h"
#include "obj_ui/bdd/bdd_edit/bdd_grid_adjustable.h"
#include "obj_ui/bdd/bdd_spe/bdd_boxes.h"
#include "obj_ui/bdd/bdd_point/bdd_boid.h"
#include "obj_ui/bdd/bdd_point/bdd_part.h"
#include "obj_ui/bdd/bdd_point/bdd_voxel.h"
#include "obj_ui/bdd/bdd_spe/bdd_bullet.h"
#include "obj_ui/bdd/bdd_fbx.h"
#include "obj_ui/bdd/bdd_multitouch/bdd_img_contour.h"
#include "time/aaa_time.h"
#include "obj_ui/bdd/bdd_field/bdd_field_gene.h"
#include "obj_ui/deformer/def_distance_field.h"
#include "obj_ui/deformer/def_wall_interaction.h"
#include "bdd_lua_flex.h"


//todo	add fns in factory to deal faster with inheritance
//		in particular
namespace aaalua
{
	c_bdd* bdd_lua_cur = nullptr;

//to put that in a separate namespace and call register_bdd_generic() from register_bdd()
//	is completly artifical and done to contounate a compiler bug in the WoodMetal version
//
//	GENERIC
//
namespace n_bdd_generic
{
	//	bdd = bdd.get_cur()
	AAALUACALL( get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 0 );		
		if( c_bdd* bdd	= c_bdd::get_cur() )
			return l.return_obj_ref( (c_obj_ui*)bdd );
		l.error_message( "no current bdd" );
		return l.return_nothing();
	}
	//	bdd = bdd.get_ui()
	AAALUACALL( get_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 0 );
		if( c_bdd* bdd	= c_bdd::get_ui() )
			return l.return_obj_ref( (c_obj_ui*)bdd );
		l.error_message( "no ui bdd" );
		return l.return_nothing();
	}
	//	set_ui(obj_ref or obj_name)
	AAALUACALL( set_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		if( c_bdd* bdd = (c_bdd*)l.get_aaa_obj_derived_from( 1, "bdd" ) )
			c_bdd::set_ui( bdd );
		return l.return_nothing();
	}
//todo check this is bdd ?
	//	input	(obj_ref or obj_name), force bdd_update
	AAALUACALL( update )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui*	obj		= l.get_aaa_obj( 1 );
		l.pop_all();
		obj->update();
		return l.return_nothing();
	}
	//	input	(obj_ref or obj_name)
	AAALUACALL( restart )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_obj_ui*	obj		= l.get_aaa_obj( 1 );
		l.pop_all();
		obj->restart();
		return l.return_nothing();
	}
	AAALUACALL( set_lua_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	= l.get_arg_nb( 0, 1 );
		bdd_lua_cur = nullptr;
		if( nb_arg == 0 || l.is_nil() )
			return l.return_nothing();
		bdd_lua_cur	= (c_bdd*)l.get_aaa_obj_derived_from( 1, "bdd" );
		return l.return_true();
	}

	void	register_bdd_generic( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		ADD_FN( get_cur					);
		ADD_FN( get_ui					);
		ADD_FN( set_ui					);
		ADD_FN( update					);
		ADD_FN( restart					);

		ADD_FN( set_lua_cur				);
	}
}	//namespace n_bdd_generic

namespace n_bdd
{
	template< class T >
	FINLINE	T*	get_bdd_cur_of_class( c_lua_state& l )
	{
		if( c_bdd* bdd = bdd_lua_cur )
		{
			if( bdd->is_class<T>() )
				return (T*)bdd;
			else
				l.error_and_escape( "object is not from class %.64s but of class %.64s", T::the_factory().get_class_name(), bdd->get_class_name() );
		}
		else
			l.error_and_escape( "No cur bdd, so can return bdd of class", T::the_factory().get_class_name() );
		return nullptr;
	}
	template< class T >
	FINLINE	T*	get_bdd_of_class( c_lua_state& l, bool b_cur, INT32& index )
	{
		if( b_cur )
			return get_bdd_cur_of_class<T>( l );
		return l.get_aaa_obj_of_class<T>( index++ );
	}
	template< class T >
	FINLINE	T*	get_bdd_of_class( c_lua_state& l, bool b_cur )
	{
		if( b_cur )
			return get_bdd_cur_of_class<T>( l );
		return l.get_aaa_obj_of_class<T>( 1 );
	}
	FINLINE	c_bdd*	get_bdd_cur( c_lua_state& l )
	{
		if( c_bdd* bdd = bdd_lua_cur )
			return bdd;
		else
			l.error_and_escape( "No cur bdd." );
		return nullptr;
	}
	FINLINE	c_bdd*	get_bdd( c_lua_state& l, bool b_cur, INT32& index )
	{
		if( b_cur )
			return get_bdd_cur( l );
		return (c_bdd*) l.get_aaa_obj_derived_from( index++, "bdd" );
	}
	FINLINE	c_bdd*	get_bdd( c_lua_state& l, bool b_cur )
	{
		if( b_cur )
			return get_bdd_cur( l );
		return (c_bdd*) l.get_aaa_obj_derived_from( 1, "bdd" );
	}
	FINLINE	c_bdd*	get_bdd_first( c_lua_state& l )
	{
		return (c_bdd*)l.get_aaa_obj_derived_from( 1, "bdd" );
	}

	// aaa.bdd.__do_dev_test( tab, index )
	AAALUACALL( __do_dev_test )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb( 2 );
		INT32	CONST	index	=	l.pop_int32();

		FP32 v[3] = { 12, 21, 42 };
		for( INT32 i=0; i<12; ++i )
		{
			v[0] = FP32(i);
			l.set_field_consecutive_v3( index+i*3, v );
		}
		return l.return_nothing();
	}

	//	no                          bdd.set_color( bdd, index, r, g, b, a )
	//	bdd.set_color( index, r, g, b, a )
	AAALUACALL( set_color )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb( 4, 5 );
		c_bdd*	CONST	bdd		=	get_bdd_cur( l );
		INT32	CONST	index	=	l.get_int32( 1 )-1;
		REAL			col[4];
		if( nb_arg == 5 )
			l.get_v4( col, 2 );
		else
		{
			l.get_v3( col, 2 );
			col[3] = 1;
		}		
		bdd->set_color( index, col );
		return l.return_nothing();
	}
	//	bdd.use_color( id1 [,id2 [,id3] ...] )
	AAALUACALL( use_color )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			nb_arg	=	l.get_arg_nb_min( 1 );
		c_bdd*	CONST	bdd		=	get_bdd_cur( l );
		INT32			data[8];
		nb_arg = MIN( nb_arg, 8 );
		for( INT32 i=1; i<=nb_arg; ++i )
		{
			data[i-1] = l.get_int32( i )-1; 
		}
		bdd->use_color( data, nb_arg );
		return l.return_nothing();
	}

	//	bdd.set_color_map( index, u, v )		--index nil or negative to clear the color map
	AAALUACALL( set_color_map )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg	=	l.get_arg_nb_min_max( 1, 6 );
		c_bdd_voxel*	CONST	bdd		=	get_bdd_cur_of_class<c_bdd_voxel>( l );
		INT32			CONST	index	=	l.get_int32( 1 )-1;
		if( nb_arg==1 || l.is_nil(2) )
			bdd->set_color_map( index, -1, 0,0, 1,1 );
		else
		{
			//INT32	i_arg	=	1;
			INT32	CONST	bind	=	l.get_int32( 2 );
			REAL	CONST	u		=	nb_arg>=3 ? l.get_real(3) : REAL(0);
			REAL	CONST	v		=	nb_arg>=4 ? l.get_real(4) : REAL(.5);
			REAL	CONST	fu		=	nb_arg>=5 ? l.get_real(5) : REAL(1);
			REAL	CONST	fv		=	nb_arg>=6 ? l.get_real(6) : REAL(1);
			bdd->set_color_map( index, bind, u,v, fu,fv );
		}		
		return l.return_nothing();
	}
//todo regroup with use color
	//	bdd.use_color_map( id1 [,id2 [,id3] ...] )
	AAALUACALL( use_color_map )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32				nb_arg	=	l.get_arg_nb_min( 1 );
		c_bdd*		CONST	bdd		=	get_bdd_cur( l );
		INT32				data[8];
		nb_arg = MIN( nb_arg, 8 );
		for( INT32 i=1; i<=nb_arg; ++i )
		{
			data[i-1] = l.get_int32( i )-1; 
		}
		bdd->use_color_map( data, nb_arg );
		return l.return_nothing();
	}

	//	bdd.add_point_line_y( bdd, x, y ,z, dy, nb, img_col )
	AAALUACALL( add_point_line_y )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg	= l.get_arg_nb( 6, 7 );
		INT32			i_arg	= 1;
		c_bdd_voxel*	CONST	bdd		= get_bdd_of_class<c_bdd_voxel>( l, nb_arg==6, i_arg );
		REAL			vec[3];
		l.get_v3( vec, i_arg );
		i_arg += 3;
		REAL			CONST	dy		= l.get_real( i_arg );
		INT32			CONST	nb		= l.get_int32( ++i_arg );
		INT32			CONST	img_col	= l.get_int32( ++i_arg );

		bdd->add_point_line_y( vec, dy, nb, img_col );
		return l.return_nothing();
	}
	//	bdd.set_line_y( bdd, id, b_active, b_forced )
	AAALUACALL( set_line_y )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg		= l.get_arg_nb( 3, 4 );
		INT32			i_arg		= 1;
		c_bdd_voxel*	CONST	bdd			= get_bdd_of_class<c_bdd_voxel>( l, nb_arg==3, i_arg );
		INT32			CONST	id			= l.get_int32( i_arg );
		bool			CONST	b_active	= l.get_bool( ++i_arg );
		bool			CONST	b_forced	= l.get_bool( ++i_arg );

		bdd->set_line_y( id-1, b_active, b_forced );
		return l.return_nothing();
	}
	
	//	bdd.begin_render( bdd )
	AAALUACALL( begin_render )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		c_bdd_voxel*	CONST	bdd		=	get_bdd_of_class<c_bdd_voxel>( l, nb_arg==0 );

		bdd->begin_render();
		return l.return_nothing();
	}
	//	bdd.end_render( bdd )
	AAALUACALL( end_render )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
		c_bdd_voxel*	CONST	bdd		=	get_bdd_of_class<c_bdd_voxel>( l, nb_arg==0 );

		bdd->end_render();
		return l.return_nothing();
	}

	INT32 render_op(	c_lua_state& l,	INT32 nb_buf	)
	{
		c_bdd_voxel::st_render_op	rop;
		c_bdd_voxel*	CONST	bdd		=	get_bdd_cur_of_class<c_bdd_voxel>( l );
		INT32					nb_arg	=	l.get_arg_nb_min_max( 1+nb_buf,1+ nb_buf+16 );
		rop.name.set( l.get_str(1) );
		
		INT32	i_arg = 1;
		for( INT32 i=0; i<nb_buf; ++i )
			rop.buf[i] = l.get_int32( ++i_arg );
		rop.nb_buf = nb_buf;

		nb_arg -= i_arg;
		for( INT32 i=0; i<nb_arg; ++i )
			rop.args[i] = l.get_real( ++i_arg );
		rop.nb_arg = nb_arg;

		if( !bdd->render_op( &rop ) )
			l.error_and_escape( "render_op%d( %.64s, ... ) failed : %.128s", nb_buf, rop.name.get(), rop.err );
		return l.return_nothing();
	}
	//	aaa.bdd.render_op0( opname, [args, ...] )
	AAALUACALL(	render_op0 )		{	LUAAAA_START( L, __FUNCTION__ );	return render_op( l, 0 );	}
	//	aaa.bdd.render_op1( opname, buf1, [args, ...] )
	AAALUACALL(	render_op1 )		{	LUAAAA_START( L, __FUNCTION__ );	return render_op( l, 1 );	}
	//	aaa.bdd.render_op2( opname, buf1, buf2, [args, ...] )
	AAALUACALL(	render_op2 )		{	LUAAAA_START( L, __FUNCTION__ );	return render_op( l, 2 );	}
	//	aaa.bdd.render_op3( opname,	buf1, buf2, buf3, [args, ...] )
	AAALUACALL(	render_op3 )		{	LUAAAA_START( L, __FUNCTION__ );	return render_op( l, 3 );	}

	//	bdd.render_plane_axe( axe, y_begin, y_end )
	AAALUACALL( render_plane_axe )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_bdd_voxel*	CONST	bdd	= get_bdd_cur_of_class<c_bdd_voxel>( l );
		INT32			CONST	axe	= l.get_int32(1);
		REAL			CONST	beg	= l.get_real(2);
		REAL			CONST	end	= l.get_real(3);

		bdd->render_plane_axe( axe-1, beg, end );
		return l.return_nothing();
	}
	//	bdd.render_sphere( x,y,z, size, delta_full, delta_grad )
	AAALUACALL( render_sphere )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 6 );
		c_bdd_voxel*	CONST	bdd		=	get_bdd_cur_of_class<c_bdd_voxel>( l );
		REAL			pos[3];
		l.get_v3( pos, 1 );
		REAL			tmp[3];
		l.get_v3( tmp, 4 );

		bdd->render_sphere( pos, tmp[0], tmp[1], tmp[2] );
		return l.return_nothing();
	}
	//	bdd.render_cylinder_y( x,y,z, size, delta_full, delta_grad )
	AAALUACALL( render_cylinder_y )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 6 );
		c_bdd_voxel*	CONST	bdd		=	get_bdd_cur_of_class<c_bdd_voxel>( l );
		REAL	pos[3];
		l.get_v3( pos, 1 );
		REAL	tmp[3];
		l.get_v3( tmp, 4 );

		bdd->render_cylinder_y( pos, tmp[0], tmp[1], tmp[2] );
		return l.return_nothing();
	}
	//	bdd.render_boule( x,y,z, size )
	//	bdd.render_boule( x,y,z, size, inside_cano )
	AAALUACALL( render_boule )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg	=	l.get_arg_nb( 4, 5 );
		c_bdd_voxel*	CONST	bdd		=	get_bdd_cur_of_class<c_bdd_voxel>( l );
		REAL			pos[3];
		l.get_v3( pos, 1 );

		bdd->render_boule( pos, l.get_real(4), nb_arg==5 ? l.get_real(5) : REAL(1) );
		return l.return_nothing();
	}

	//	bdd.render_cube( x,y,z, size )
	//	bdd.render_cube( x,y,z, size, inside_cano )
	AAALUACALL( render_cube )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg	=	l.get_arg_nb( 4, 5 );
		c_bdd_voxel*	CONST	bdd		=	get_bdd_cur_of_class<c_bdd_voxel>( l );
		REAL			pos[3];
		l.get_v3( pos, 1 );

		bdd->render_cube( pos, l.get_real(4), nb_arg==5 ? l.get_real(5) : REAL(1) );
		return l.return_nothing();
	}
	
	//	bdd.render_segment( xa,ya,za, xb,yb,zb, size )
	//	bdd.render_segment( xa,ya,za, xb,yb,zb, size, inside_cano )
	AAALUACALL( render_segment )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg	=	l.get_arg_nb( 7, 8 );
		c_bdd_voxel*	CONST	bdd		=	get_bdd_cur_of_class<c_bdd_voxel>( l );
		REAL			pos[6];
		l.get_v6( pos, 1 );

		bdd->render_segment( pos, &pos[3], l.get_real(7), nb_arg==8 ? l.get_real(8) : REAL(1) );
		return l.return_nothing();
	}


	//	x,y,z = bdd.get_point_by_id( bdd, id )
	AAALUACALL( get_point_by_id )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bdd_boid*	CONST	bdd	=	l.get_aaa_obj_of_class<c_bdd_boid>( 1 );
		INT32		CONST	id	=	l.get_int32( 2 );

		REAL				vec[3];
		if( bdd->get_point_by_id( id, vec ) )
			return l.return_real_v3( vec );
		return l.return_nothing();
	}

	//	bdd.set_point_by_id( bdd, index, x, y ,z )
	AAALUACALL( set_point_by_id )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 4, 5 );
		c_bdd_boid*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_boid>( 1 );
		INT32		CONST	id		=	l.get_int32( 2 );
		REAL				vec[3];
		vec[0] = l.get_real( 3 );
		vec[1] = l.get_real( 4 );
		vec[2] = nb_arg==5 ? l.get_real( 5 ) : 0;

		if( bdd->set_point_by_id( id, vec ) )
			return l.return_bool( true );
		return l.return_nothing();
	}
	//	bdd.set_box_by_id( bdd, index, x, y ,z )
	AAALUACALL( set_box_by_id )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 4, 5 );
		c_bdd_boid*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_boid>( 1 );
		INT32		CONST	id		=	l.get_int32( 2 );
		REAL		vec[3];
		vec[0] = l.get_real( 3 );
		vec[1] = l.get_real( 4 );
		vec[2] = nb_arg==5 ? l.get_real( 5 ) : 1;

		if( bdd->set_box_by_id( id, vec ) )
			return l.return_bool( true );
		return l.return_nothing();
	}

	AAALUACALL( get_random_point_and_speed )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
		c_bdd*	CONST	bdd		=	get_bdd_first( l );
		REAL	CONST	when	=	nb_arg==2 ? l.get_real( 2 ) : REAL(aaa::time::cur);

		REAL	vec[3];
		REAL	speed[3];
		//if( bdd )
		//{
			bdd->get_point_and_speed_rnd_time_rel( vec, speed, when );
		//}
		//else
		//{
		//	clear_v3( vec );
		//	clear_v3( speed );
		//}
		l.push_real_v3( vec );
		l.push_real_v3( speed );
		return 6;
	}

	
//	BDD_FIELD_GENE
//
//	bdd.add_sphere( bdd, x, y ,z, radius )
	AAALUACALL( add_sphere )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32				CONST	nb_arg	= l.get_arg_nb( 5 );
		c_bdd_field_gene*	CONST	bdd		= l.get_aaa_obj_of_class<c_bdd_field_gene>( 1 );
		REAL						pos[3];
		l.get_v3( pos, 2 );
		bdd->add_sphere( pos, l.get_real(5) );
		return l.return_nothing();
	}


//	DEF_DISTANCE_FIELD
//
	//	bdd.update_fish_length( bdd, len )
	AAALUACALL( update_fish_length )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32					CONST	nb_arg	= l.get_arg_nb( 2 );
		c_def_distance_field*	CONST	bdd		= l.get_aaa_obj_of_class<c_def_distance_field>( 1 );
		bdd->update_fish_length( l.get_real(2) );
		return l.return_nothing();
	}

//	DEF_WALL_INTERACTION
//
	//	bdd.reset_humans( bdd )
	AAALUACALL( reset_walls )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32					CONST	nb_arg	= l.get_arg_nb( 1 );
		c_def_wall_interaction*	CONST	bdd		= l.get_aaa_obj_of_class<c_def_wall_interaction>( 1 );
		bdd->reset_walls();
		return l.return_nothing();
	}
	//	bdd.add_human( bdd, x, y ,z, radius )
	AAALUACALL( add_human )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32					CONST	nb_arg	= l.get_arg_nb( 5 );
		c_def_wall_interaction*	CONST	bdd		= l.get_aaa_obj_of_class<c_def_wall_interaction>( 1 );
		REAL pos[3];
		l.get_v3( pos, 2 );
		bdd->add_human( pos, l.get_real(5) );
		return l.return_nothing();
	}
	//	bdd.end_humans( bdd )
	AAALUACALL( end_humans )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32					CONST	nb_arg	= l.get_arg_nb( 1 );
		c_def_wall_interaction*	CONST	bdd		= l.get_aaa_obj_of_class<c_def_wall_interaction>( 1 );
		bdd->end_humans();
		return l.return_nothing();
	}


//	BDD_PARTICLE
//

	//	bdd.add_particle_at( bdd, x, y ,z )
	AAALUACALL( add_particle_at )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg	=	l.get_arg_nb( 1, 4 );
		c_bdd_particle*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_particle>( 1 );
		if( nb_arg == 4 )
		{
			REAL vec[3];
			l.get_v3( vec, 2 );
			bdd->add_particle_at( vec );
		}
		else
			bdd->add_particle_at();
			
		return l.return_nothing();
	}

//	BDD_UI	bdd_grid_adj, bdd_boxes for now
//
	//	aaa.bdd.clear_ui_intercept_all()
	AAALUACALL( clear_ui_intercept_all )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_bdd::clear_ui_intercept_all();
		return l.return_nothing();
	}

	//	b = aaa.bdd.can_ui_intercept( bdd )
	AAALUACALL( can_ui_intercept )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_bdd*	CONST	bdd		=	get_bdd_first( l );

		return l.return_bool( bdd->can_ui_intercept() );
	}
	//	b = aaa.bdd.set_intercept_ui( bdd, b )
	AAALUACALL( set_ui_intercept )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bdd*	CONST	bdd		=	get_bdd_first( l );
		bool	CONST	b		=	l.get_bool( 2 );

		return l.return_bool( bdd->set_ui_intercept( b ) );
	}
	//	b = aaa.bdd.is_ui_intercept( bdd )
	AAALUACALL( is_ui_intercept )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_bdd*	CONST	bdd		=	get_bdd_first( l );

		return l.return_bool( bdd->is_ui_intercept() );
	}

	//	bdd.set_control_point( bdd, i_u,i_v,	x,y,z )
	//	bdd.set_control_point( bdd, i_u,i_v,	x,y )
	//	bdd.set_control_point( bdd, i_u,		x,y,z )
	//	bdd.set_control_point( bdd, i_u,		x,y )
	AAALUACALL( set_control_point )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg	=	l.get_arg_nb_min_max( 4, 6 );
		c_obj_ui*	obj		=	l.get_aaa_obj( 1 );
		if( obj )
		{
			REAL							vec[3];
			if(	c_bdd_grid_adjustable::is_instance(obj) )
			{
				INT32 CONST	i_u		=	l.get_int32( 2 );
				INT32 CONST	i_v		=	l.get_int32( 3 );
				vec[0]				=	l.get_real( 4 );
				vec[1]				=	l.get_real( 5 );
				vec[2]				=	nb_arg==6 ? l.get_real( 6 ) : 0;

				((c_bdd_grid_adjustable*)obj)->set_control_point( i_u, i_v, vec );	//todo have return code from these
			}
			else
			{
				INT32 CONST	i_u		=	l.get_int32( 2 );
				vec[0]				=	l.get_real( 3 );
				vec[1]				=	l.get_real( 4 );
				vec[2]				=	nb_arg==5 ? l.get_real( 5 ) : 0;
				if(			c_bdd_line_3d	::is_instance(obj) )
					((c_bdd_line_3d*)obj)->set_control_point( i_u, vec );
				else if(	c_bdd_curve_3d	::is_instance(obj) )
					((c_bdd_curve_3d*)obj)->set_control_point( i_u, vec );
			}
		}
		return l.return_nothing();
	}

	//	x,y,z = bdd.get_control_point( bdd, i_u,i_v )
	//	x,y,z = bdd.get_control_point( bdd, i_u )
	AAALUACALL( get_control_point )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_obj_ui*	obj		=	l.get_aaa_obj( 1 );
		if( obj )
		{
			REAL CONST * pt;
			if(	c_bdd_grid_adjustable::is_instance(obj) )
			{
				l.check_arg_nb( 3 );
				INT32 CONST	i_u	= l.get_int32( 2 );
				INT32 CONST	i_v	= l.get_int32( 3 );
				pt = ((c_bdd_grid_adjustable*)obj)->get_control_point( i_u, i_v );
			}
			else
			{
				INT32 CONST	i_u = l.get_int32( 2 );
				if(			c_bdd_line_3d	::is_instance(obj) )
					pt = ((c_bdd_line_3d*)obj)->get_control_point( i_u );
				else if(	c_bdd_curve_3d	::is_instance(obj) )
					pt = ((c_bdd_curve_3d*)obj)->get_control_point( i_u );
			}
			if( pt )
				return l.return_real_v3(pt);
		}
		return l.return_nothing();
	}

	//	u,v = bdd.get_uv_from_xy( obj, x, y )	
	//	u,v = bdd.get_uv_from_xy( obj, x, y, z )	but not implemented yet
	AAALUACALL( get_uv_from_xy )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32					CONST	nb_arg	=	l.get_arg_nb( 3, 4 );
		c_bdd_grid_adjustable*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_grid_adjustable>( 1 );
		REAL							vec[3];
		vec[0]							=	l.get_real( 2 );
		vec[1]							=	l.get_real( 3 );
		//	implemented only with xy for now but in case
		vec[2]							=	nb_arg==4 ? l.get_real( 4 ) : 0;

		REAL u,v;
		if( bdd->get_uv_from_pos( u, v, vec ) )
		{
			return l.return_real_2( u, v );
		}
		return l.return_nothing();
	}

	//		control_id = bdd.get_control_id_from_xy( obj, x, y )
	//	or	control_id = bdd.get_control_id_from_xy( obj, x, y, z )
	//todo make obj,x,y,z input generic
	AAALUACALL( get_control_id_from_xy )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32			CONST	nb_arg	=	l.get_arg_nb( 3, 4 );
		c_bdd_boxes*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_boxes>( 1 );
		REAL					vec[3];
		vec[0]					=	l.get_real( 2 );
		vec[1]					=	l.get_real( 3 );
		//	implemented only with xy for now but in case
		vec[2]					=	nb_arg==4 ? l.get_real( 4 ) : 0;

		INT32 id = bdd->get_control_in( vec[0], vec[1], vec[2] );
		if( id > 0 )
			return l.return_int32( id );
		return l.return_nothing();
	}
//
//	BLOB
//
	AAALUACALL( clear_blob )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_bdd_blob*	CONST	bdd	=	l.get_aaa_obj_of_class<c_bdd_blob>( 1 );

		bdd->clear_blob();
		return l.return_nothing();
	}

	AAALUACALL( add_blob )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 6 );
		c_bdd_blob*	CONST	bdd	=	l.get_aaa_obj_of_class<c_bdd_blob>( 1 );
		INT32		CONST	id	=	l.get_int32( 2 );
		REAL		CONST	x	=	l.get_real( 3 );
		REAL		CONST	y	=	l.get_real( 4 );
		REAL		CONST	sx	=	l.get_real( 5 );
		REAL		CONST	sy	=	l.get_real( 6 );

		bdd->add_blob( id, x, y, sx, sy );
		return l.return_nothing();
	}
	//	b = bdd.get_ghost( obj, index )
	//	specialized for blob_tracking
	AAALUACALL( get_ghost )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bdd_blob_tracking*	bdd		= l.get_aaa_obj_of_class<c_bdd_blob_tracking>( 1 );
		INT32					index	= l.get_int32( 2 );

		return l.return_bool( bdd->get_ghost( index-1 ) );
	}
//
//	CURVE ONLY
//
//	aaa.bdd.get_control_point_nb( bdd, node_id )
//	specialized for bdd_curve_3d and bdd_line_3d
	AAALUACALL( get_control_point_nb )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb(	2 );
		c_obj_ui* obj =	l.get_aaa_obj( 1 );
		if( obj )
		{ 
			INT32	dataset_id	=	l.get_int32( 2 );
			if(			c_bdd_line_3d	::is_instance(obj) )
				return l.return_int32( ((c_bdd_line_3d*)obj)->get_control_point_nb( dataset_id ) );
			else if(	c_bdd_curve_3d	::is_instance(obj) )
				return l.return_int32( ((c_bdd_curve_3d*)obj)->get_control_point_nb( dataset_id ) );
			l.error_message( "object is not from class bdd_curve_3d or bdd_line_3d but of class %.64s", obj->get_class_name() );
		}
		return l.return_nothing();
	}
	//	aaa.bdd.push_control_point_back( bdd, node_id, x, y, z )
	//	specialized for bdd_curve_3d
	AAALUACALL( push_control_point_back )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 5 );
		c_obj_ui* obj =	l.get_aaa_obj( 1 );
		if( obj )
		{ 
			INT32	dataset_id	=	l.get_int32( 2 );
			REAL	src[3];
			l.get_v3(	src, 3 );
			if(			c_bdd_line_3d	::is_instance(obj) )
				((c_bdd_line_3d*)obj)->push_control_point_back( dataset_id, src );
			else if(	c_bdd_curve_3d	::is_instance(obj) )
				((c_bdd_curve_3d*)obj)->push_control_point_back( dataset_id, src );
			else
				l.error_message( "object is not from class bdd_curve_3d or bdd_line_3d but of class %.64s", obj->get_class_name() );
		}
		return l.return_nothing();		
	}

	//	len = aaa.bdd.push_control_point_back_len_max( bdd, node_id, x,y,z, len_max )
	//	specialized for c_bdd_line_3d (2021 Oct)
	AAALUACALL( push_control_point_back_len_max )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 6 );
		c_obj_ui* obj =	l.get_aaa_obj( 1 );
		if( obj )
		{ 
			INT32	dataset_id	=	l.get_int32( 2 );
			REAL	src[3];
			l.get_v3(	src, 3 );
			REAL	len_max = l.get_real( 6 );
			DOUBLE	len;
			if(			c_bdd_line_3d	::is_instance(obj) )
				len = ((c_bdd_line_3d*)obj)->push_control_point_back_len_max( dataset_id, src, len_max );
			else
			{
				l.error_message( "object is not from class bdd_line_3d but of class %.64s", obj->get_class_name() );
				return l.return_nothing();
			}
			return l.return_double( len );	
		}
		return l.return_nothing();
	}

	//	aaa.bdd.pop_control_point_front( bdd, node_id )
	//	specialized for bdd_curve_3d
	AAALUACALL( pop_control_point_front )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb(	2 );
		c_obj_ui* obj =	l.get_aaa_obj( 1 );
		if( obj )
		{ 
			INT32	dataset_id	=	l.get_int32( 2 );
			if(			c_bdd_line_3d	::is_instance(obj) )
				((c_bdd_line_3d*)obj)->pop_control_point_front( dataset_id );
			else if(	c_bdd_curve_3d	::is_instance(obj) )
				((c_bdd_curve_3d*)obj)->pop_control_point_front( dataset_id );
			else
				l.error_message( "object is not from class bdd_curve_3d or bdd_line_3d but of class %.64s", obj->get_class_name() );
		}
		return l.return_nothing();
	}

	//	aaa.bdd.clear_control_points( bdd, node_id )
	//	specialized for bdd_curve_3d
	AAALUACALL( clear_control_points )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb(	2 );
		c_obj_ui* obj =	l.get_aaa_obj( 1 );
		if( obj )
		{ 
			INT32	dataset_id	=	l.get_int32( 2 );
			if(			c_bdd_line_3d	::is_instance(obj) )
				((c_bdd_line_3d*)obj)->clear_control_points( dataset_id );
			else if( 	c_bdd_curve_3d	::is_instance(obj) )
				((c_bdd_curve_3d*)obj)->clear_control_points( dataset_id );
			else
				l.error_message( "object is not from class bdd_curve_3d or bdd_line_3d but of class %.64s", obj->get_class_name() );
		}
		return l.return_nothing();
	}

	//	aaa.bdd.clear_datasets()
	//	specialized for bdd_curve_3d
	//todo generalize
	AAALUACALL( clear_datasets )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb(	1 );
		c_obj_ui* obj =	l.get_aaa_obj( 1 );
		if( obj )
		{ 
			INT32	dataset_id	=	l.get_int32( 2 );
			if(			c_bdd_line_3d	::is_instance(obj) )
				((c_bdd_line_3d*)obj)->clear_datasets();
			else if(	c_bdd_curve_3d	::is_instance(obj) )
				((c_bdd_curve_3d*)obj)->clear_datasets();
			else
				l.error_message( "object is not from class bdd_curve_3d or bdd_line_3d but of class %.64s", obj->get_class_name() );
		}
		return l.return_nothing();
	}

//
//	CURVE / MOCAP
//
	//	x, y, z = bdd.coor_to_world( obj, node_id, x, y, z )
	//	specialized for bdd_mocap and bdd_curve_3d soon
	AAALUACALL( coor_to_world )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb(	5 );
		c_bdd*	bdd		=	get_bdd_first( l );
		INT32	node_id	=	l.get_value_direct<INT32>(2);
		REAL	src[3];
		l.get_v3( src, 3 );

		REAL dst[3];
		if( c_bdd_mocap::is_instance(bdd) )
			((c_bdd_mocap*)bdd)->coor_to_world_one( dst, node_id, src );
		else if( c_bdd_curve_3d::is_instance(bdd) )
			((c_bdd_curve_3d*)bdd)->coor_to_world_one( dst, node_id, src );
		else
			clear_v3( dst );
		return l.return_real_v3( dst );
	}

	//	len = bdd.get_len( obj, node_id )
	//	specialized for bdd_mocap and bdd_curve_3d soon
	AAALUACALL( get_len )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb(	1, 2 );
		c_bdd*	CONST	bdd		=	get_bdd_first( l );

		if( c_bdd_tube_path::is_instance(bdd) )
			return l.return_real( ((c_bdd_tube_path*)bdd)->get_len() );
		else
		{
			if( nb_arg < 2 )
				l.error_and_escape( "not enough arguments : need a node_id as second arg." );
			INT32 node_id	= l.get_int32( 2 );
			if( c_bdd_mocap::is_instance(bdd) )
				return l.return_real( ((c_bdd_mocap*)bdd)	->get_len( node_id ) );
			else if ( c_bdd_line_3d::is_instance(bdd) )
				return l.return_double( ((c_bdd_line_3d*)bdd)	->get_len( node_id ) );
			else if ( c_bdd_curve_3d::is_instance(bdd) )
				return l.return_real( ((c_bdd_curve_3d*)bdd)->get_len( node_id ) );
		}
		return l.return_nothing();
	}
	//	x, y, z = bdd.get_point_s( obj, node_id, s )
	//	specialized for bdd_mocap and bdd_curve_3d
	AAALUACALL( get_point_s )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb(	3 );
		c_bdd*	bdd		=	get_bdd_first( l );
		INT32	node_id	=	l.get_value_direct<INT32>(2);
		REAL	s		=	l.get_value_direct<REAL>(	3 );

		REAL	vec[3];
		if(			c_bdd_mocap		::is_instance(bdd) )
			((c_bdd_mocap*)bdd)		->get_tra( vec, node_id, s );
		else if(	c_bdd_line_3d	::is_instance(bdd) )
			((c_bdd_line_3d*)bdd)	->get_tra( vec, node_id, s );
		else if(	c_bdd_curve_3d	::is_instance(bdd) )
			((c_bdd_curve_3d*)bdd)	->get_tra( vec, node_id, s );
		else
			clear_v3( vec );
		return l.return_real_v3( vec );
	}

	//	table = bdd.get_points_s( obj, node_id, ds, s_begin, s_end, )
	//	specialized for bdd_mocap and bdd_curve_3d
	AAALUACALL( get_points_s )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg = l.get_arg_nb_min_max(	3, 5 );
		c_bdd*	CONST	bdd		=	get_bdd_first( l );
		if(	!c_bdd_mocap::is_instance(bdd) && !c_bdd_line_3d::is_instance(bdd) && !c_bdd_curve_3d::is_instance(bdd) )
			return l.return_nothing();

		INT32	CONST	node_id	=	l.get_value_direct<INT32>(2);
		REAL	CONST	ds		=	l.get_value_direct<REAL>(	3 );
		REAL	CONST	s_begin	=	(nb_arg<4) ? 0 : l.get_value_direct<REAL>(	4 );
		REAL	CONST	s_end	=	(nb_arg<4) ? 1 : l.get_value_direct<REAL>(	5 );

		//	create result table
		if( 0. < ds )
		{
			l.new_table();
			INT32 i = 1;
			REAL vec[3];
			if(	c_bdd_mocap::is_instance(bdd) )
			{
				for( REAL s=s_begin; s <= s_end; s+= ds )
				{
					((c_bdd_mocap*)bdd)		->get_tra( vec, node_id, s );
					l.set_field_consecutive_v3( i, vec );
					i += 3;
				}
			}
			else if( c_bdd_line_3d::is_instance(bdd) )
			{
				for( REAL s=s_begin; s <= s_end; s+= ds )
				{
					((c_bdd_line_3d*)bdd)	->get_tra( vec, node_id, s );
					l.set_field_consecutive_v3( i, vec );
					i += 3;
				}
			}
			else if( c_bdd_curve_3d::is_instance(bdd) )
			{
				for( REAL s=s_begin; s <= s_end; s+= ds )
				{
					((c_bdd_curve_3d*)bdd)	->get_tra( vec, node_id, s );
					l.set_field_consecutive_v3( i, vec );
					i += 3;
				}
			}

			if( i == 0 )
			{
				l.pop( 1 );
				return l.return_nothing();
			}
			else
				return l.return_table();
		}
		else
		{
			l.error_message( "ds (argument 3) should be > 0 and is %f: returning nothing", ds );
			return l.return_nothing();
		}
	}

	//	x, y, z = bdd.get_point_s_curve_fast( obj, node_id, s )
	//	x, y, z = bdd.get_point_s_curve_fast( node_id, s )
	//	specialized for bdd_curve_3d and bdd_line_3d
	AAALUACALL( get_point_s_curve_fast )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg = l.get_arg_nb_min_max( 2, 3 );
		c_obj_ui*	obj		;
		INT32		node_id	;
		REAL		s		;
		if( nb_arg == 3 )
		{
			//obj		=	l.get_aaa_obj_of_class<c_bdd_curve_3d>( 1 );
			obj		=	l.get_aaa_obj( 1 );
			node_id	=	l.get_value_direct<INT32>(2);
			s		=	l.get_value_direct<REAL>(	3 );
		}
		else
		{
			obj		=	get_bdd_cur( l );
			node_id	=	l.get_value_direct<INT32>(1);
			s		=	l.get_value_direct<REAL>(	2 );
		}
		REAL	vec[3];
		if(			c_bdd_line_3d	::is_instance(obj) )
			((c_bdd_line_3d*)obj)->get_tra( vec, node_id, s );
		else if(	c_bdd_curve_3d	::is_instance(obj) )
			((c_bdd_curve_3d*)obj)->get_tra( vec, node_id, s );
		return l.return_real_v3( vec );
	}

	//	tx, ty, tz = bdd.get_tangent_s( obj, node_id, s )
	//	specialized for bdd_mocap and bdd_curve_3d
	AAALUACALL( get_tangent_s )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb(	3 );
		c_bdd*	CONST	bdd		=	get_bdd_first( l );
		INT32	CONST	node_id	=	l.get_value_direct<INT32>(2);
		REAL	CONST	s		=	l.get_value_direct<REAL>(	3 );

		REAL	vec[3];
		if(			c_bdd_mocap::is_instance(bdd) )
			static_cast<c_bdd_mocap*>	(bdd)->get_tangent(	vec, node_id, s );
		else if(	c_bdd_line_3d::is_instance(bdd) )
			static_cast<c_bdd_line_3d*>	(bdd)->get_tangent(	vec, node_id, s );
		else if(	c_bdd_curve_3d::is_instance(bdd) )
			static_cast<c_bdd_curve_3d*>(bdd)->get_tangent(	vec, node_id, s );
		else
			clear_v3(vec);

		return l.return_real_v3( vec );
	}

	//	x, y, z, tx, ty, tz = bdd.get_point_tangent_s( obj, node_id, s )
	//	specialized for bdd_mocap and bdd_curve_3d
	AAALUACALL( get_point_tangent_s )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb(	3 );
		c_bdd*	CONST	bdd		=	get_bdd_first( l );
		INT32	CONST	node_id	=	l.get_value_direct<INT32>(2);
		REAL	CONST	s		=	l.get_value_direct<REAL>(	3 );

		REAL	vec[3];
		REAL	tgn[3];
		if(			c_bdd_mocap::is_instance(bdd)		)
			static_cast<c_bdd_mocap*>	(bdd)->get_point_tangent( vec, tgn, node_id, s );
		else if(	c_bdd_line_3d::is_instance(bdd)		)
			static_cast<c_bdd_line_3d*>	(bdd)->get_point_tangent( vec, tgn, node_id, s );
		else if(	c_bdd_curve_3d::is_instance(bdd)	)
			static_cast<c_bdd_curve_3d*>(bdd)->get_point_tangent( vec, tgn, node_id, s );
		else
		{
			clear_v3( vec );
			clear_v3( tgn );
		}

		l.push_real_v3( vec );
		l.push_real_v3( tgn );
		return 6;
	}

//
//	BDD_TUBE_PATH
//
	//	nb = bdd.get_ring_nb( bdd )
	AAALUACALL( get_ring_nb )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_bdd_tube_path*	bdd	=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );

		return l.return_int32( bdd->get_ring_nb() );
	}
	//	cur, next, inter = bdd.get_ring_from_s()
	AAALUACALL( get_ring_from_s )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bdd_tube_path*	bdd	=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		REAL				s	=	l.get_real( 2 );

		INT32	c;
		INT32	n;
		REAL	inter;
		bdd->get_ring_from_s( s, c, n, inter );
		l.push_int( c );
		l.push_int( n );
		l.push_real( inter );
		return 3;
	}
	//	bdd.init_ring(	bdd, index )
	AAALUACALL( init_ring )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32				CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
		c_bdd_tube_path*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		if( nb_arg >= 2 )
		{
			INT32	index	=	l.get_int32( 2 );
			bdd->init_ring( index );
		}
		else
		{
			bdd->init_ring_all();
		}
		return l.return_nothing();
	}
	// u, v inverted because because the coordinates system of tube is bad ( u is y )
	//	bdd.set_ring_center_offset(	bdd, index, o_u, o_v )	//, o_axe )
	AAALUACALL( set_ring_center_offset )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		c_bdd_tube_path*	bdd		=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				index	=	l.get_int32( 2 );
		REAL				u		=	l.get_real( 3 );
		REAL				v		=	l.get_real( 4 );

		bdd->set_ring_center_offset( index, v, u );
		return l.return_nothing();
	}
	//	ou, ov = bdd.get_ring_center_offset( bdd, index )
	AAALUACALL( get_ring_center_offset )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bdd_tube_path*	bdd		=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				index	=	l.get_int32( 2 );

		REAL o_u, o_v;
		((c_bdd_tube_path*)bdd)->get_ring_center_offset( index, o_v, o_u );
		return l.return_real_2( o_u, o_v );

	}	//	bdd.set_ring_radius(	bdd, index, ru, rv )
	AAALUACALL( set_ring_radius )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		c_bdd_tube_path*	bdd		=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				index	=	l.get_int32( 2 );
		REAL				ru		=	l.get_real( 3 );
		REAL				rv		=	l.get_real( 4 );

		bdd->set_ring_radius( index, rv, ru );
		return l.return_nothing();
	}
	//	ru, rv = bdd.get_ring_radius( bdd, index )
	AAALUACALL( get_ring_radius )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bdd_tube_path*	bdd		=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				index	=	l.get_int32( 2 );

		REAL ru, rv;
		bdd->get_ring_radius( index, rv, ru );
		return l.return_real_2( ru, rv );
	}
	//	bdd.set_ring_angle_offset(	bdd, index, angle_offset )
	AAALUACALL( set_ring_angle_offset )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_bdd_tube_path*	bdd				=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				index			=	l.get_int32( 2 );
		REAL				angle_offset	=	l.get_real( 3 );

		bdd->set_ring_angle_offset( index, angle_offset );
		return l.return_nothing();
	}
	//	bdd.set_ring_angle_range(	bdd, index, angle_offset )
	AAALUACALL( set_ring_angle_range )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_bdd_tube_path*	bdd				=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				index			=	l.get_int32( 2 );
		REAL				angle_range		=	l.get_real( 3 );

		bdd->set_ring_angle_range( index, angle_range );
		return l.return_nothing();
	}
	//	bdd.set_ring_enabled(	bdd, index, b )
	AAALUACALL( set_ring_enabled )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_bdd_tube_path*	bdd				=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				index			=	l.get_int32( 2 );
		bool				b_enabled		=	l.get_bool( 3 );

		bdd->set_ring_enabled( index, b_enabled );
		return l.return_nothing();
	}
	//	bdd.set_ring_marker(	bdd, index, fn_str, id )
	AAALUACALL( set_ring_marker )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32				CONST	nb_arg			=	l.get_arg_nb( 2, 4 );
		c_bdd_tube_path*	CONST	bdd				=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				CONST	index			=	l.get_int32( 2 );
		C_PCHAR_C					str_fn			=	(nb_arg>=3) ? l.get_str_or_nil( 3 ) : nullptr;
		INT32				CONST	id				=	(nb_arg>=4) ? l.get_int32( 4 ) : 0;

		bdd->set_ring_marker( index, str_fn, id );
		return l.return_nothing();
	}
	//	bdd.set_ring_texture(	bdd, index, bind_index )
	AAALUACALL( set_ring_texture )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32				CONST	nb_arg			=	l.get_arg_nb( 2, 3 );
		c_bdd_tube_path*	CONST	bdd				=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				CONST	index			=	l.get_int32( 2 );
		INT32				CONST	bind_index		=	(nb_arg>=3) ? l.get_int32( 3 ) : -1;

		bdd->set_ring_texture( index, bind_index );
		return l.return_nothing();
	}
	//	bdd.set_ring_uv(	bdd, index, u, u_next, v_min, v_max, v_next_min, v_next_max )
	//	bdd.set_ring_uv(	bdd, index, u, u_next, v_min, v_max )
	//	bdd.set_ring_uv(	bdd, index, u, u_next )
	AAALUACALL( set_ring_uv )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32				CONST	nb_arg			=	l.get_arg_nb( 4, 6, 8 );
		c_bdd_tube_path*	CONST	bdd				=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				CONST	index			=	l.get_int32( 2 );
		REAL				CONST	u				=	l.get_real( 3 );
		REAL				CONST	u_next			=	l.get_real( 4 );
		REAL				v_min;
		REAL				v_max;
		if( nb_arg > 4 )
		{
			v_min	= l.get_real( 5 );
			v_max	= l.get_real( 6 );
		}
		else
		{
			v_min	= 0;
			v_max	= 1;
		}
		REAL	v_next_min;
		REAL	v_next_max;
		if( nb_arg > 6 )
		{
			v_next_min	= l.get_real( 7 );
			v_next_max	= l.get_real( 8 );
		}
		else
		{
			v_next_min	= v_min;
			v_next_max	= v_max;
		}
		bdd->set_ring_uv( index, u, u_next, v_min, v_max, v_next_min, v_next_max );
		return l.return_nothing();
	}
	//	x, y, z = bdd.get_ring_center( bdd, index )
	AAALUACALL( get_ring_center )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bdd_tube_path*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				CONST	index	=	l.get_int32( 2 );

		return l.return_real_v3( bdd->get_ring_center( index ) );
	}
	//	x, y, z = bdd.get_ring_normal( bdd, index )
	AAALUACALL( get_ring_normal )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bdd_tube_path*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_tube_path>( 1 );
		INT32				CONST	index	=	l.get_int32( 2 );

		return l.return_real_v3( bdd->get_ring_normal( index ) );
	}
//
//	BDD_CURVE_EDIT
//
	//	val = aaa.bdd.get_curve_val( obj, control_id, phase )
	//	specialized for bdd_curve_edit
	AAALUACALL( get_curve_val )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb(	3 );
		c_bdd_curve_edit*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_curve_edit>( 1 );
		INT32				CONST	ctl_id	=	l.get_int32(	2 );
		REAL				CONST	p		=	l.get_real(	3 );

		REAL val = bdd->control_get_value( ctl_id, p );
		return l.return_real( val );
	}
//
//	DATASET
//
	//	input	(obj_ref or obj_name), dataset_id (0 for all)
	AAALUACALL( update_dataset )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32				CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
		c_bdd_opencl_part*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_opencl_part>( 1 );
		UINT32				CONST	dataset = ( nb_arg == 2 ) ? l.get_int32( 2 ) : 0;

		bdd->update_dataset(dataset);		
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name), dataset_id (0 for all)
	AAALUACALL( draw_dataset )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32				CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
		c_bdd_opencl_part*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_opencl_part>( 1 );
		UINT32				CONST	dataset = ( nb_arg == 2 ) ? l.get_int32( 2 ) : 0;

		bdd->draw_dataset(dataset);
		return l.return_nothing();
	}

	//	input	(obj_ref or obj_name), id (start at one)
	//	set_dataset_float(	bdd, id, v1 [,vn...] )	-- id start at 1
	AAALUACALL( set_dataset_float )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32						nb_arg	=	l.get_arg_nb_min( 3 );
		c_bdd_opencl_part*	CONST	bdd		=	l.get_aaa_obj_of_class<c_bdd_opencl_part>( 1 );
		UINT32				CONST	id		=	l.get_int32( 2 );
		nb_arg -= 2;

		for( INT32 i = 0; i < nb_arg; ++i )
		{
			REAL v = l.get_real( 3 + i );
			bdd->set_dataset_float( id+i-1, v );
		}
		return l.return_nothing();
	}
//
//	BDD_IMG_CONTOUR BDD_BLOB BDD_BLOB_TRACKING
//
//todo find something more elegant to unify these bdd
	AAALUACALL( get_blob_nb )
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(1);

		c_obj_ui*	CONST	obj = l.get_aaa_obj( 1 );
		if( obj )
		{
			if( obj->is_class<c_bdd_img_contour>() )
			{
				c_bdd_img_contour*	CONST	bdd	= (c_bdd_img_contour*)obj;
				return l.return_int( bdd->get_blob_nb() );
			}
			if( obj->is_inherited_from( "bdd_blob_base" ) )
			{
				c_bdd_blob_base*	CONST	bdd	= (c_bdd_blob_base*)obj;
				return l.return_int( bdd->get_blob_nb() );
			}
			l.error_message( "object of class %.64s don't have a %s().", obj->get_class_name(), __FUNCTION__ );
		}
		return l.return_nothing();
	}
	AAALUACALL( get_blob_xy )
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(2);

		c_obj_ui*	CONST	obj = l.get_aaa_obj( 1 );
		if( obj )
		{
			INT32 id =	l.get_int32(2);
			c_blob CONST * blob = nullptr;
			if( obj->is_class<c_bdd_img_contour>() )
			{
				c_bdd_img_contour*	CONST	bdd	= (c_bdd_img_contour*)obj;
				blob = &bdd->get_blob_index( id-1 );

			}
			else if( obj->is_inherited_from( "bdd_blob_base" ) )
			{
				c_bdd_blob_base*	CONST	bdd	= (c_bdd_blob_base*)obj;
				blob = &bdd->get_blob_index( id-1 );
			}
			else
				l.error_message( "object of class %.64s don't have a %s().", obj->get_class_name(), __FUNCTION__ );
			if( blob )
			{
				REAL vec[3];
				blob->get_center( vec );		
				return l.return_real_2( vec[0], vec[1] );
			}
		}
		return l.return_nothing();
	}
	AAALUACALL( get_blob_xy_sxy )
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(2);

		c_obj_ui*	CONST	obj = l.get_aaa_obj( 1 );
		if( obj )
		{
			INT32 id =	l.get_int32(2);
			c_blob CONST * blob = nullptr;
			if( obj->is_class<c_bdd_img_contour>() )
			{
				c_bdd_img_contour*	CONST	bdd	= (c_bdd_img_contour*)obj;
				blob = &bdd->get_blob_index( id-1 );
			}
			else if( obj->is_inherited_from( "bdd_blob_base" ) )
			{
				c_bdd_blob_base*	CONST	bdd	= (c_bdd_blob_base*)obj;
				blob = &bdd->get_blob_index( id-1 );
			}
			else
				l.error_message( "object of class %.64s don't have a %s().", obj->get_class_name(), __FUNCTION__ );
			if( blob )
			{
				REAL vec[3];
				blob->get_center( vec );		
				l.push_real_2( vec[0], vec[1] );
				l.push_real_2( blob->_box.get_size_x(), blob->_box.get_size_y() );
				return 4;
			}
		}
		return l.return_nothing();
	}
	AAALUACALL( get_blob_id_xy )
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(2);

		c_obj_ui*	CONST	obj = l.get_aaa_obj( 1 );
		if( obj )
		{
			INT32 id =	l.get_int32(2);
			c_blob CONST * blob = nullptr;
			if( obj->is_class<c_bdd_img_contour>() )
			{
				c_bdd_img_contour*	CONST	bdd	= (c_bdd_img_contour*)obj;
				auto blob = bdd->get_blob_index( id-1 );
			}
			else if( obj->is_inherited_from( "bdd_blob_base" ) )
			{
				c_bdd_blob_base*	CONST	bdd	= (c_bdd_blob_base*)obj;
				auto blob = bdd->get_blob_index( id-1 );
			}
			else
				l.error_message( "object of class %.64s don't have a %s().", obj->get_class_name(), __FUNCTION__ );
			if( blob )
			{
				l.push_int( blob->get_id() );
				REAL vec[3];
				blob->get_center( vec );		
				l.push_real_2( vec[0], vec[1] );
				return 3;
			}
		}
		return l.return_nothing();
	}
	AAALUACALL( get_blob_id_xy_sxy )
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(2);

		c_obj_ui*	CONST	obj = l.get_aaa_obj( 1 );
		if( obj )
		{
			INT32 id =	l.get_int32(2);
			c_blob CONST * blob = nullptr;
			if( obj->is_class<c_bdd_img_contour>() )
			{
				c_bdd_img_contour*	CONST	bdd	= (c_bdd_img_contour*)obj;
				blob = &bdd->get_blob_index( id-1 );
			}
			else if( obj->is_inherited_from( "bdd_blob_base" ) )
			{
				c_bdd_blob_base*	CONST	bdd	= (c_bdd_blob_base*)obj;
				blob = &bdd->get_blob_index( id-1 );
			}
			else
				l.error_message( "object of class %.64s don't have a %s().", obj->get_class_name(), __FUNCTION__ );
			if( blob )
			{
				l.push_int( blob->get_id() );
				REAL vec[3];
				blob->get_center( vec );		
				l.push_real_2( vec[0], vec[1] );
				l.push_real_2( blob->_box.get_size_x(), blob->_box.get_size_y() );
				return 5;
			}
		}
		return l.return_nothing();
	}

	//	t is an existing table, nb = bdd.get_blob_polyline_to_table( t, bdd, id )
	AAALUACALL( get_blob_polyline_to_table )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_obj_ui* CONST	obj = l.get_aaa_obj( 2 );

		if( obj )
		{
			INT32 id =	l.get_int32(3);
			l.pop(2);
			c_blob CONST * blob;
			if( obj->is_class<c_bdd_img_contour>() )
			{
				c_bdd_img_contour*	CONST	bdd	= (c_bdd_img_contour*)obj;
				blob = &bdd->get_blob_index( id-1 );			
			}
			else if( obj->is_inherited_from( "bdd_blob_base" ) )
			{
				c_bdd_blob_base*	CONST	bdd = (c_bdd_blob_base*)obj;
				blob = &bdd->get_blob_index( id-1 );
			}
			else
				l.error_and_escape( "object of class %.64s don't have a %s().", obj->get_class_name(), __FUNCTION__ );
			INT32 nb;
			if( blob )
			{
				c_polyline_2d CONST * p_polyline = &blob->_polyline;
				if( p_polyline )
				{
					nb = p_polyline->get_point_nb();
					if( nb > 0 )
					{
						for( INT32 i=0; i<nb; ++i )
						{
							st_point_2d CONST &	point = p_polyline->get_point_const( i );
							l.push_int( i+1 );
							l.new_table();
							l.set_field_consecutive_v2( 1, point.x, point.y );
							l.set_table( -3 );
						}
					}
				}
				else
				{
					nb = 0;
					l.error_message( "blob index %d don't have a polyline. %s()", id, __FUNCTION__ );
				}
			}
			l.pop(1);
			return l.return_int(nb);
		}
		l.pop(3);
		return l.return_nothing();
	}

//	BDD_BULLET
//
	// aaa.bdd.set_gravity( bdd, gravity_x, y, z ) -- sets gravity of the active world
	AAALUACALL( set_gravity )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			FP32	grav[3];	
			l.get_v3( grav, 2 );
			bdd->set_gravity( grav );
		}
		return l.return_nothing();
	}

	// ind = aaa.bdd.create_box_shape( bdd, sx, sy, sz ) -- create a btBoxShape and return its index in c_bdd_bullet::_box_shapes
	AAALUACALL( create_box_shape )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			FP32	size[3];
			l.get_v3( size, 2 );
			INT32	id_box = bdd->create_box_shape_id( (btVector3*)size );
			return l.return_int( id_box );
		}
		return l.return_nothing();
	}
	
	// ind = aaa.bdd.create_cylinder_shape( bdd, sx, sy, sz ) -- create a btCylinderShape and return its index in c_bdd_bullet::_cylinder_shapes
	AAALUACALL( create_cylinder_shape )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			FP32	size[3];
			l.get_v3( size, 2 );
			INT32	id_cylinder = bdd->create_cylinder_shape_id( (btVector3*)size );
			return l.return_int( id_cylinder );
		}
		return l.return_nothing();
	}

	// ind = aaa.bdd.create_sphere_shape( bdd, size ) -- create a btSphereShape and return its index in c_bdd_bullet::_sph_shapes
	AAALUACALL( create_sphere_shape )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			FP32	size		= l.get_fp32( 2 );
			INT32	id_sphere 	= bdd->create_sphere_shape_id( size );
			return l.return_int( id_sphere );
		}
		return l.return_nothing();
	}

	/*AAALUACALL( create_heightfield_shape )
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb( 3 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			INT32	nb_u		= l.get_int32(2);
			INT32	nb_v		= l.get_int32(3);
			void*	data		= bdd->generate_heightfield( nb_u, nb_v );
			INT32	id_field	= bdd->create_heightfield_shape_id( data, nb_u, nb_v );
			return l.return_int( id_field );
		}
		return l.return_nothing();
	}*/

	// ind = aaa.bdd.create_transform( bdd, px, py, pz ) -- create a btTransform and return its index in c_bdd_bullet::_transforms 
	AAALUACALL( create_transform )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			FP32	pos[3];	
			l.get_v3( pos, 2 );
			INT32 id_transform 	= 	bdd->create_transform_id( pos );
			return l.return_int( id_transform );
		}
		return l.return_nothing();
	}

	// ind = aaa.bdd.create_rigid_body( bdd, mass, ind_trans, ind_shape, shape_type, [local_inertia_xyz] ) -- create a btRigidBody and return its index in c_bdd_bullet::_rigid_bodies
	AAALUACALL( create_rigid_body )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 5, 8 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			FP32	CONST	mass			= l.get_fp32( 2 );
			INT32	CONST	id_transform	= l.get_int32( 3 );
			INT32	CONST	id_shape		= l.get_int32( 4 );
			INT32	CONST	shape_type		= l.get_int32( 5 );
			FP32	local_inertia[3];
			if( nb_arg == 5 )
				clear_v3( local_inertia );
			else
				l.get_v3( local_inertia, 6 );

			INT32	bid = 0; // BETTER to set -1 ?
			if( shape_type == 1 )
			{
				btSphereShape*	CONST	sph			= bdd->get_sphere_shape( id_shape );
				bid = bdd->create_rigid_body_id( mass, id_transform, local_inertia, sph );
			}
			else if( shape_type == 2 )
			{
				btBoxShape*		CONST	box			= bdd->get_box_shape( id_shape );
				bid = bdd->create_rigid_body_id( mass, id_transform, local_inertia, box );
			}
			else if( shape_type == 3 )
			{
				btCylinderShape* CONST	cylinder	= bdd->get_cylinder_shape( id_shape );
				bid = bdd->create_rigid_body_id( mass, id_transform, local_inertia, cylinder );
			}
			return l.return_int( bid );
		}
		return l.return_nothing();
	}

	// bid = aaa.bdd.add_object_bid( bdd, ind_trans, ind_shape, ind_body, shape_type ) -- create a c_bullet_obj (only rigid for now),
	//																				--	add it to the current dynamic world	and c_bdd_bullet::_objects
	//																				--	returns its bid (bullet id)
	AAALUACALL( add_object_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 6 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			FP32	CONST	mass			= l.get_fp32( 2 );
			INT32	CONST	id_transform	= l.get_int32( 3 );
			INT32	CONST	id_shape		= l.get_int32( 4 );
			INT32	CONST	id_body			= l.get_int32( 5 );
			INT32	CONST	shape_type		= l.get_int32( 6 );
			INT32	bid = 0; // BETTER to set -1 ?
			btTransform* trans = bdd->get_transform( id_transform );
			btRigidBody* body = bdd->get_rigid_body( id_body );
			if( shape_type == 1 )
			{
				btSphereShape*	CONST	sph		= bdd->get_sphere_shape( id_shape );
				bid	= bdd->add_rigid_body_bid( mass, trans, body, sph );
			}
			else if( shape_type == 2 )
			{
				btBoxShape*	CONST	box		= bdd->get_box_shape( id_shape );
				bid	= bdd->add_rigid_body_bid( mass, trans, body, box );
			}
			else if( shape_type == 3 )
			{
				btCylinderShape* CONST	cylinder	= bdd->get_cylinder_shape( id_shape );
				bid	= bdd->add_rigid_body_bid( mass, trans, body, cylinder );
			}
			if( bid > 0 )
				return l.return_int( bid );
		}
		return l.return_nothing();
	}

	// bid = aaa.bdd.delete_obj_bid( bdd, bid ) -- remove obj with bid from c_bdd_bullet::_objects (map) and push_back to c_bdd_bullet::_objects_free
	AAALUACALL( delete_obj_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			INT32 bid = l.get_int32( 2 );
			bid = bdd->delete_obj_bid( bid );
			if( bid > 0 )
				return l.return_int( bid );
		}
		return l.return_nothing();
	}
	
	// aaa.bdd.lock_move_rotate_bid( bdd, bid, axes_xyz, angles_xyz )
	AAALUACALL( lock_move_rotate_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 8 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			INT32	CONST	bid = l.get_int32( 2 );
			FP32	axes[3];
			l.get_v3( axes, 3 );
			FP32	angles[3];
			l.get_v3( angles, 6 );
			bdd->lock_move_rotate( bdd->get_obj_by_bid(bid)->get_rigid_body(), axes, angles );
		}
		return l.return_nothing();
	}
	
	// aaa.bdd.lock_move_rotate_all_bid( bdd, axes_xyz, angles_xyz )
	AAALUACALL( lock_move_rotate_all_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 7 );
		if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
		{
			FP32	axes[3];
			l.get_v3( axes, 2 );
			FP32	angles[3];
			l.get_v3( angles, 5 );
			bdd->lock_all_objects( axes, angles );
		}
		return l.return_nothing();
	}

	namespace
	{
		c_bullet_obj* get_bullet_obj_bid( c_lua_state& l )
		{
			if( c_bdd_bullet* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1) )
			{
				INT32	CONST	bid	= l.get_int32( 2 );
				c_bullet_obj* CONST	obj = bdd->get_obj_by_bid( bid );
				if( obj )
					return obj;
				l.error_message( "not a valid bid : %d", bid );
			}
			return nullptr;
		}
	};

	// _b_is_bid = aaa.bdd.is_bid( bdd, bid )
	AAALUACALL( is_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		return l.return_bool( obj!=nullptr );
	}

	// aaa.bdd.set_mass_bid( bdd, bid, mass )
	AAALUACALL( set_mass_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		if( obj )
		{
			FP32	CONST	mass = l.get_fp32( 3 );
			obj->set_mass( mass );
		}
		return l.return_nothing();
	}
	
	// aaa.bdd.set_pos_bid( bdd, bid, pos )
	AAALUACALL( set_pos_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 5 );
		c_bullet_obj* obj = get_bullet_obj_bid( l );
		if( obj )
		{
			FP32	vec[3];
			l.get_v3( vec, 3 );
			obj->set_pos( vec );
		}
		return l.return_nothing();
	}
	
	// px, py, pz = aaa.bdd.get_pos_bid( bdd, bid )
	AAALUACALL( get_pos_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		if( obj )
		{
			FP32 CONST *	vec = obj->get_pos();
			return l.return_fp32_v3( vec );
		}
		return l.return_nothing();
	}

	// aaa.bdd.set_size_bid( bdd, bid, sx, sy, sz )
	AAALUACALL( set_size_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 5 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		if( obj )
		{
			FP32	vec[3];
			l.get_v3( vec, 3 );
			obj->set_size( vec );
		}
		return l.return_nothing();
	}
	
	// sx, sy, sz = aaa.bdd.get_size_bid( bdd, bid )
	AAALUACALL( get_size_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		if( obj )
		{
			FP32 CONST *	vec = obj->get_size();
			return l.return_fp32_v3( vec );
		}
		return l.return_nothing();
	}

	// aaa.bdd.set_rot_z( bdd, bid, rot_z )
	AAALUACALL( set_rot_z_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		if( obj )
			obj->set_rot_z( l.get_real( 3 ) );
		return l.return_nothing();
	}
	
	// rot_z = aaa.bdd.get_rot_z_bid( bdd, bid )
	AAALUACALL( get_rot_z_bid )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		if( obj )
			return l.return_fp32( obj->get_rot_z() );
		return l.return_nothing();
	}
	
	//	aaa.bdd.set_damping( bbd, bid, damping )
	AAALUACALL( set_damping )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 4 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		if( obj )
		{
			FP32	CONST	lin	= l.get_real( 3 );
			FP32	CONST	ang	= l.get_real( 4 );
			obj->set_damping( lin, ang );
		}
		return l.return_nothing();
	}
	
	//	aaa.bdd.set_dynamic( bbd, bid, bool )
	AAALUACALL( set_dynamic )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		if( obj )	
			obj->set_dynamic( l.get_bool( 3 ) );
		return l.return_nothing();
	}

	// aaa.bdd.set_restitution( bdd, bid, restitution )
	AAALUACALL( set_restitution )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_bullet_obj* CONST	obj = get_bullet_obj_bid( l );
		if( obj )
		{
			FP32	CONST	mass = l.get_fp32( 3 );
			obj->set_restitution( mass );
		}
		return l.return_nothing();
	}

	// aaa.bdd.pick( bdd, from(x, y, 0), to(x, y, z) )
	/*AAALUACALL( pick )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 7 );
		c_bdd_bullet* bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1);
		REAL from[3];
		l.get_v3( from, 2 );
		//printf("getting from\n from[0] : %f, from[1] : %f, from[2] : %f\n", from[0], from[1], from[2]);
		REAL to[3];
		l.get_v3( to, 5 );
		//PRINT_STRING("getting to\n to[0] : %f, to[1] : %f, to[2] : %f\n", to[0], to[1], to[2]);
		if( bdd )
			bdd->pick( from, to );
		return l.return_nothing();
	}*/

	// m0, m1, m2, m3 [...] = aaa.bdd.get_matrixes( bdd )
	/*AAALUACALL( get_matrixes )
	{
		LUAAAA_START(L, __FUNCTION__);
		l.check_arg_nb(1);
		c_bdd_bullet* bdd = l.get_aaa_obj_of_class<c_bdd_bullet>(1);

		if( bdd )
		{
			DOUBLE mproj[16];
			DOUBLE mview[16];
			bdd->get_matrixes( mproj, mview );
			l.push_double(mproj[0]);	l.push_double(mproj[1]);
			l.push_double(mproj[2]);	l.push_double(mproj[3]);
			l.push_double(mproj[4]);	l.push_double(mproj[5]);
			l.push_double(mproj[6]);	l.push_double(mproj[7]);
			l.push_double(mproj[8]);	l.push_double(mproj[9]);
			l.push_double(mproj[10]);	l.push_double(mproj[11]);
			l.push_double(mproj[12]);	l.push_double(mproj[13]);
			l.push_double(mproj[14]);	l.push_double(mproj[15]);

			l.push_double(mview[0]);	l.push_double(mview[1]);
			l.push_double(mview[2]);	l.push_double(mview[3]);
			l.push_double(mview[4]);	l.push_double(mview[5]);
			l.push_double(mview[6]);	l.push_double(mview[7]);
			l.push_double(mview[8]);	l.push_double(mview[9]);
			l.push_double(mview[10]);	l.push_double(mview[11]);
			l.push_double(mview[12]);	l.push_double(mview[13]);
			l.push_double(mview[14]);	l.push_double(mview[15]);
			return 32;
		}
		return l.return_nothing();
	}*/

//	BDD_FBX
//
//unused
	AAALUACALL( update_time )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST nb_arg = l.get_arg_nb( 1, 2 );
		if( c_bdd_fbx* CONST	bdd = get_bdd_of_class<c_bdd_fbx>( l, nb_arg==1 ) )
		{
			FP32	time = l.get_fp32( nb_arg );
			bdd->update_time( time );
		}
		return l.return_nothing();
	}

	void	register_bdd( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );
	
		l.define_table( "bdd" );

			n_bdd_generic::register_bdd_generic( L );

			ADD_FN( __do_dev_test );

			ADD_FN( get_blob_nb					);
			ADD_FN( get_blob_xy					);
			ADD_FN( get_blob_xy_sxy				);
			ADD_FN( get_blob_id_xy				);
			ADD_FN( get_blob_id_xy_sxy			);
			ADD_FN( get_blob_polyline_to_table	);

			register_bdd_point( L );

		//	BDD_BOID
			ADD_FN( set_point_by_id			);
			ADD_FN( get_point_by_id			);
			ADD_FN( set_box_by_id			);

		//	BDD_VOXEL
			ADD_FN( set_color				);
			ADD_FN(	set_color_map			);
//unused	ADD_FN( use_color				);
//unused	ADD_FN( use_color_map			);

			ADD_FN(	add_point_line_y		);
			ADD_FN(	add_point_line_y		);
			ADD_FN(	set_line_y				);
			ADD_FN(	begin_render			);
			ADD_FN(	render_plane_axe		);
			ADD_FN(	render_sphere			);
			ADD_FN(	render_boule			);
			ADD_FN(	render_cube				);
			ADD_FN(	render_segment			);
			ADD_FN( render_cylinder_y		);
			ADD_FN(	end_render				);
			ADD_FN(	render_op0				);
			ADD_FN(	render_op1				);
			ADD_FN(	render_op2				);
			ADD_FN(	render_op3				);

			ADD_FN(	get_random_point_and_speed );
		//	BDD_FIELD
			ADD_FN(	add_sphere );
			ADD_FN(	update_fish_length );
			//	DEF_WALL_INTERACTION
			ADD_FN(	reset_walls );
			ADD_FN(	add_human );
			ADD_FN(	end_humans );

		//	BDD_PART
			ADD_FN(	add_particle_at			);	//todo added for dpool : generalize ?

			ADD_FN( clear_ui_intercept_all	);
			ADD_FN( can_ui_intercept		);
			ADD_FN( set_ui_intercept		);
			ADD_FN( is_ui_intercept			);

			ADD_FN( get_control_point		);
			ADD_FN( set_control_point		);
			ADD_FN( get_uv_from_xy			);
			ADD_FN( get_control_id_from_xy	);

			ADD_FN( clear_blob				);
			ADD_FN( add_blob				);
			ADD_FN( get_ghost				);

			ADD_FN( get_len					);
			ADD_FN( get_point_s				);
			ADD_FN( get_points_s			);
			ADD_FN( get_point_s_curve_fast	);
			ADD_FN( get_tangent_s			);
			ADD_FN( get_point_tangent_s		);
			ADD_FN( coor_to_world			);

			ADD_FN( get_control_point_nb			);
			ADD_FN( push_control_point_back			);
			ADD_FN( push_control_point_back_len_max	);
			ADD_FN( pop_control_point_front			);
			ADD_FN( clear_control_points			);

			ADD_FN(	clear_datasets			);

		//	BDD_TUBE_PATH
			ADD_FN( init_ring				);
			ADD_FN( get_ring_nb				);
			ADD_FN( get_ring_from_s			);
			ADD_FN( set_ring_center_offset	);
			ADD_FN( get_ring_center_offset	);
			ADD_FN( set_ring_radius			);
			ADD_FN( get_ring_radius			);
			ADD_FN( set_ring_angle_offset	);
			ADD_FN( set_ring_angle_range	);
			ADD_FN( set_ring_enabled		);
			ADD_FN( set_ring_marker			);
			ADD_FN( set_ring_texture		);
			ADD_FN( set_ring_uv				);
			ADD_FN( get_ring_center			);
			ADD_FN( get_ring_normal			);

			ADD_FN( get_curve_val			);

			ADD_FN( update_dataset			);
			ADD_FN( draw_dataset			);
			ADD_FN( set_dataset_float		);

		//	BDD_FLEX
			n_bdd_flex::register_bdd_flex( L );

		//	BDD_BULLET
			// bid is bullet id, the index of the c_bullet_obj in c_bdd_bullet::_objects (map)
			ADD_FN( set_gravity					);
			ADD_FN( create_box_shape			);
			ADD_FN( create_cylinder_shape		);
			ADD_FN( create_sphere_shape			);
//			ADD_FN( create_heightfield_shape	);
			ADD_FN( create_transform 			);
			ADD_FN( create_rigid_body			);
			ADD_FN( add_object_bid				);
			ADD_FN( delete_obj_bid				);
			ADD_FN( lock_move_rotate_bid		);
			ADD_FN( lock_move_rotate_all_bid    );
			ADD_FN( set_mass_bid				);
			ADD_FN( set_pos_bid					);
			ADD_FN( get_pos_bid					);
			ADD_FN( set_size_bid				);
			ADD_FN( get_size_bid				);
			ADD_FN( set_rot_z_bid				);
			ADD_FN( get_rot_z_bid				);
			ADD_FN( is_bid						);
//			ADD_FN( pick						);
//			ADD_FN( get_matrixes				);
			ADD_FN( set_damping					);
			ADD_FN( set_dynamic					);
			ADD_FN( set_restitution				);

		//	BDD_FBX
			//unused	ADD_FN( update_time					);

		l.pop( 1 );	//pop new table
	}
}	//end namespace n_bdd
}	//end namespace aaalua
