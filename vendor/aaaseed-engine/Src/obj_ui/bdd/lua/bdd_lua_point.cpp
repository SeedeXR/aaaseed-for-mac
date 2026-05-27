#include "bdd_lua_point.h"
#include "bdd_lua.h"
#include "../util/bdd_uv.h"
#include "language/lua/aaalua_glue.h"

namespace aaalua
{
namespace n_bdd
{

//	nb = bdd.get_point_dataset_nb( obj )
//	nb = bdd.get_point_dataset_nb(     )
AAALUACALL( get_point_dataset_nb )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==0 );

	INT32 nb = bdd->get_point_dataset_nb();
	return l.return_int32( nb );
}

//	bdd.set_point_dataset( bdd, id )
//	bdd.set_point_dataset(      id )
AAALUACALL( set_point_dataset )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
	INT32			i_arg	=	1;
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==1, i_arg );
	INT32	CONST	index	=	l.get_int32( i_arg );

	if( bdd->set_point_dataset( index ) )
		return l.return_true();
	return l.return_false();
}

//	nb = bdd.get_point_nb( obj )
AAALUACALL( get_point_nb )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==0 );

	INT32 nb = bdd->get_point_nb();
	return l.return_int32( nb );
}

//	x, y, z = bdd.get_point( obj, index )
AAALUACALL( get_point )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
	INT32			i_arg	=	1;
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==1, i_arg );
	INT32	CONST	id		=	l.get_int32( i_arg );
	if( id < 1 )
		l.error_and_escape( "get_point(id) require id >= 0 here id is %d", id );

	REAL	vec[3];
	if( bdd->get_point( vec, id-1 ) )
		return l.return_real_v3( vec );
	//	clear_v3( vec );
	return l.return_nothing();
}

//	table of {x, y, z} = bdd.get_points( obj )
AAALUACALL( get_points )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==0 );
	// Maybe check bdd type only for bdd_img_cv_stereo_point
	INT32 nb = bdd->get_point_nb();

	if( nb<= 0 )	return l.return_nothing();

	lua_newtable(L);
	UINT32	k = 1;
	for( INT32 i = 0; i < nb; ++i )
	{
		REAL	vec[3];
		if( bdd->get_point( vec, i ) )
		{
			// if z value == REAL_BIG_VALUE then it is discarded
			if( vec[2] != REAL_BIG_VALUE )
			{
				lua_newtable( L );
				// put position in table
				l.set_field_consecutive_v3( 1, vec );
				// we have now { vec[0], vec[1], vec[2] } at the top of the stack

				// add this table to the main table 
				lua_rawseti(	L, -2, k );

				++k;
			}
		}
	}
	return l.return_table();
}

//	b = bdd.get_points_to_table( table, table_id, obj, index_point, point_u_nb ,point_u_stride  )
//	b = bdd.get_points_to_table( table, table_id, obj, point_index, point_u_nb, point_u_stride	[,point_v_nb ,point_v_stride] )
AAALUACALL( get_points_to_table )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg				=	l.get_arg_nb( 6, 8 );
	if( !l.is_table(1) )
		l.error_and_escape( "get_points_to_table() require a table as first argumet : we got %s", l.get_type_str(l.get_type(1)) );
	INT32			table_id			=	l.get_int32( 2 );

	c_bdd*	CONST	bdd					=	(c_bdd *) l.get_aaa_obj_derived_from( 3, "bdd" );
	INT32			point_index_start	=	l.get_int32( 4 );
	INT32	CONST	point_u_nb			=	l.get_int32( 5 );
	INT32	CONST	point_u_stride		=	l.get_int32( 6 ); 
	INT32	CONST	point_v_nb			=	(nb_arg>=7) ? l.get_int32( 7 ) : 1; 
	INT32	CONST	point_v_stride		=	(nb_arg>=7) ? l.get_int32( 8 ) : 0; 

	// Maybe check bdd type only for bdd_img_cv_stereo_point
	INT32 nb = bdd->get_point_nb();

	if( nb<= 0 )
		return l.return_false();

	l.pop( nb_arg - 1 );
//	l.print_stack_non_empty( "af" );

	bool b = true;
	for( INT32 nb_v = point_v_nb; nb_v>0; --nb_v )
	{
		INT32 index_point = point_index_start;
		for( INT32 nb_u = point_u_nb; nb_u>0; --nb_u )
		{
			FP32*	vec = bdd->get_point_pt( index_point ) ;
			if( vec )
				l.set_field_consecutive_v3( table_id, vec );
			else
			{
				l.set_field_consecutive_v3( table_id, zero_v4fp32 );
				b = false;
			}
			table_id += 3;
			index_point += point_u_stride;
		}
		point_index_start += point_v_stride;
	}
	return l.return_bool( b );
}

//	table of {x, y, z, id} = bdd.get_points_with_id( obj )
AAALUACALL( get_points_with_id )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==0 );
	// Maybe check bdd type only for bdd_img_cv_stereo_point
	INT32	CONST	nb = bdd->get_point_nb();

	if( nb<= 0 )
		return l.return_nothing();

	lua_newtable(L);
	UINT32	k = 1;
	for( INT32 i = 0; i < nb; ++i )
	{
		REAL	vec[3];
		INT32 id = bdd->get_point_and_id( vec, i );
		// if z value == REAL_BIG_VALUE then it is discarded
		if( vec[2] != REAL_BIG_VALUE )
		{
			lua_newtable( L );
			// put position and id in table
			l.set_field_consecutive_v3( 1, vec );
			lua_pushinteger( L, id );		lua_rawseti( L, -2, 4 );
			// we have now { vec[0], vec[1], vec[2], id } at the top of the stack

			// add this table to the main table 
			lua_rawseti(	L, -2, k );
			++k;
		}
	}
	return l.return_table();
}

//	id, x, y, z = bdd.get_id_and_point( obj, index )
AAALUACALL( get_id_and_point )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==1 );
	INT32			id		=	l.get_int32( nb_arg );
	REAL			vec[3];
	id = bdd->get_point_and_id( vec, id-1 );
	l.push_int( id );
	l.push_real_v3( vec );
	return 4;
}

//	x, y, z, id = bdd.get_point_and_id( obj, index )
AAALUACALL( get_point_and_id )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==1 );
	INT32			id		=	l.get_int32( nb_arg );
	REAL			vec[3];
	id = bdd->get_point_and_id( vec, id-1 );
	l.push_real_v3( vec );
	l.push_int( id );
	return 4;
}

//	x, y, z, id = bdd.get_point_and_id_local( obj, index )
AAALUACALL( get_point_and_id_local )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==1 );
	INT32			id		=	l.get_int32( nb_arg );
	REAL			vec[3];
	id = bdd->get_point_and_id( vec, id-1 );
	l.push_real_v3( vec );
	l.push_int( id & 0xffffff );
	return 4;
}

//	bdd.set_point( bdd, index, x, y ,z )
AAALUACALL( set_point )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 4, 5 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==4 );
	INT32	CONST	index	=	l.get_int32( 2 );
	REAL			vec[3];
	vec[0] = l.get_real( 3 );
	vec[1] = l.get_real( 4 );
	vec[2] = nb_arg==5 ? l.get_real( 5 ) :REAL(0);

	if( bdd->set_point( index-1, vec ) )
		return l.return_true();
	return l.return_nothing();
}

//	bdd.erase_points( bdd )
//	bdd.erase_points()
AAALUACALL( erase_points )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==0 );

	bdd->erase_points();
	return l.return_nothing();
}

//	x,y,z = bdd.get_point_from_uv( obj, u,v )
AAALUACALL( get_point_from_uv )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32		CONST	nb_arg	=	l.get_arg_nb( 3 );
	c_bdd_uv*	CONST	bdd		=	(c_bdd_uv*)	l.get_aaa_obj_derived_from( 1, "bdd" );
	FP32		CONST	u		=	l.get_value_direct<FP32>( 2 );
	FP32		CONST	v		=	l.get_value_direct<FP32>( 3 );

	REAL		vec[3];
	bdd->get_point_from_uv( vec, u,v );
	l.push_real_v3( vec );
	return 3;
}

//
//	SEGMENT
//
//	nb = bdd.get_segment_nb( obj )
AAALUACALL( get_segment_nb )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 0, 1 );
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==0 );

	INT32 CONST	nb = bdd->get_segment_nb();
	return l.return_int32( nb );
}

//	xa,ya,za,	xb,yb,zb	= bdd.get_segment( obj, index )
AAALUACALL( get_segment )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
	INT32			i_arg	=	1;
	c_bdd*	CONST	bdd		=	get_bdd( l, nb_arg==1, i_arg );
	INT32	CONST	id		=	l.get_int32( i_arg );
	if( id < 1 )
		l.error_and_escape( "get_segment(id) require id >= 0 here id is %d", id );

	REAL	vec0[3];
	REAL	vec1[3];
	bdd->get_segment( vec0, vec1, id );
	//else
	//	clear_v3( vec );
	l.push_real_v3( vec1 );
	l.push_real_v3( vec0 );
	return 6;
	//		return l.return_real_v6( vec );
}

void	register_bdd_point( lua_State* L )
{
	LUAAAA_START( L, __FUNCTION__ );

	ADD_FN(	get_point_dataset_nb	);
	ADD_FN(	set_point_dataset		);

	ADD_FN( get_point_nb			);
	ADD_FN( get_point				);
	ADD_FN( get_points				);
	ADD_FN(	get_points_to_table		);
	ADD_FN( get_points_with_id		);
	ADD_FN( get_id_and_point		);
	ADD_FN( get_point_and_id		);
	ADD_FN( get_point_and_id_local	);
	ADD_FN( get_point_from_uv		);

	ADD_FN( set_point				);

	ADD_FN(	erase_points			);

	ADD_FN( get_segment_nb			);
	ADD_FN( get_segment				);
}

}	//end namespace n_bdd
}	//end namespace aaalua