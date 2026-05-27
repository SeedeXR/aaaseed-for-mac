#include "poid.h"
#include "language/lua/aaalua_glue.h"
#include "obj_ui/bdd/lua/bdd_lua.h"
#include "boid.h"
#include "obj_ui/bdd/bdd_point/bdd_boid.h"
#include "obj_ui/bdd/bdd_point/bdd_part.h"

c_boid_info_lua boid_info_lua;

C_PCHAR_C	c_poid::str_living_box_type[LIVING_BOX_TYPE_NB] =
{
	"No",
	"Die",
	"Wrap_and_random",
	"Wrap",
	"Bounce",
	"Repulse"
};

C_PCHAR_C	c_poid::id_generator_str[ID_GENERATOR_NB] =
{
	"Increment",
	"Random",
	"from_index"
};

namespace aaalua
{
namespace poid
{
	C_PCHAR_C	poid_get_pos_help = 
		"() return poid position\n"
		"x,y,z = poid.get_pos()";

	AAALUACALL( poid_get_pos )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//	l.check_arg_nb( 0 );
		return	l.return_real_v3( boid_info_lua._boid->get_pos() );
	}

	FINLINE	INT32	poid_set( c_lua_state& l, REAL* dst )
	{
		INT32 CONST nb_arg = l.get_arg_nb_min_max( 1, 3 );
		if( nb_arg==3 )
			l.get_v3( dst, 1 );
		else if( nb_arg==2 )
		{
			*dst		= l.get_real( 1 );
			*(dst+1)	= l.get_real( 2 );
			*(dst+2)	= 0.;
		}
		else
		{
			*dst		= l.get_real( 1 );
			*(dst+1)	= 0.;
			*(dst+2)	= 0.;
		}
		return 0;
	}

	C_PCHAR_C	poid_set_pos_help = 
		"( x [,y[,z]] ) set poid position\n";
	AAALUACALL( poid_set_pos )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return poid_set( l, boid_info_lua._boid->get_pos() );
	}

	C_PCHAR_C	poid_get_pos_a_help = 
		"() return the a poid position\n"
		"x,y,z = poid.get_pos_a()";
	AAALUACALL( poid_get_pos_a )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return	l.return_real_v3( boid_info_lua._boid_a->get_pos() );
	}

	C_PCHAR_C	poid_get_pos_b_help = 
		"() return the b poid position\n"
		"x,y,z = poid.get_pos_b()";
	AAALUACALL( poid_get_pos_b )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return	l.return_real_v3( boid_info_lua._boid_b->get_pos() );
	}

	C_PCHAR_C	poid_get_pos_to_draw_help = 
		"() return poid position in the camera coordinate system\n"
		"x,y,z = poid.get_pos_to_draw()";
	AAALUACALL( poid_get_pos_to_draw )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//	l.check_arg_nb( 0 );
		return	l.return_real_v3( boid_info_lua._boid->get_pos_to_draw() );
	}

	C_PCHAR_C	poid_get_speed_help = 
		"() return poid speeds\n"
		"x,y,z = poid.get_speed()";
	AAALUACALL( poid_get_speed )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//	l.check_arg_nb( 0 );
		return	l.return_real_v3( boid_info_lua._boid->get_speed() );
	}

	C_PCHAR_C	poid_set_speed_help =	"set poid speed\n"
										"poid.set_speed( x, y, z )";
	AAALUACALL( poid_set_speed )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return poid_set( l, boid_info_lua._boid->get_speed() );
	}

	C_PCHAR_C	poid_get_box_size_help = 
		"() return poid box size\n"
		"sx,sy,sz = poid.get_box_size()";
	AAALUACALL( poid_get_box_size )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return	l.return_real_v3( boid_info_lua._boid->get_box_size() );
	}

	C_PCHAR_C	poid_create_at_help = 
		"( ref_or_name, x, y, z [,speed_x ,speed_y ,speed_z] ) create a new poid";
	AAALUACALL( poid_create_at )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg		= l.get_arg_nb( 4, 7 );	//todo	rely on default position 
		c_bdd_boid*	bdd_boid	= l.get_aaa_obj_of_class<c_bdd_boid>( 1 );
		REAL	pos[3];
		l.get_v3( pos, 2 );
		if( nb_arg == 4 )
			bdd_boid->create_at( pos );	//todo manage to get on id ? but the creation happen later
		else
		{
			REAL	speed[3];
			l.get_v3( speed, 5 );
			bdd_boid->create_at( pos, speed );
		}
		return	0;
	}

	C_PCHAR_C	poid_kill_all_help = 
		"( bdd_ref_or_name ) kill all poid";
	AAALUACALL( poid_kill_all )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_bdd_boid*	bdd_boid	= l.get_aaa_obj_of_class<c_bdd_boid>( 1 );
		l.pop_all();	//hack avoid a bug in the hook check for empty stack
		bdd_boid->kill_all();
		return 0;
	}

	C_PCHAR_C	poid_kill_a_help = 
		"() kill the a poid in interaction";
	AAALUACALL( poid_kill_a )
	{
		LUAAAA_START( L, __FUNCTION__ );
		boid_info_lua._bdd_a->set_killed( boid_info_lua._boid_a );
		return	0;
	}

	C_PCHAR_C	poid_kill_b_help = 
		"() kill the b poid in interaction";
	AAALUACALL( poid_kill_b )
	{
		LUAAAA_START( L, __FUNCTION__ );
		boid_info_lua._bdd_b->set_killed( boid_info_lua._boid_b );
		return	0;
	}

	C_PCHAR_C	poid_kill_by_id_help = 
		"( id ) kill a poid";
//		"poid.kill_by_id( ref_or_name, id )\n";
//2021 Maa seems to use the bdd_cur now
	AAALUACALL( poid_kill_by_id )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_bdd*	bdd	= n_bdd::get_bdd_cur( l );
		INT32	id	= l.get_int32( 1 );
		//todo generalize this (tests and messages)
		if( c_bdd_boid::is_instance(bdd) )
			reinterpret_cast<c_bdd_boid*>(bdd)->kill_by_id( id );
		else if( c_bdd_particle::is_instance(bdd) )
			reinterpret_cast<c_bdd_particle*>(bdd)->kill_by_id( id );
		else
			ERR_PRINT_STRING( "poid.kill_by_id() implemented only for bdd_boid and bdd_particle" );
		return	0;
	}

	INT32 poid_set_fix( c_lua_state& l, c_boid* boid )
	{
		l.check_arg_nb( 1 );
		REAL	r = l.get_real( 1 );
		if( r <.5 )
			boid->clear_fix();
		else
			boid->set_fix();
		return	0;
	}

	C_PCHAR_C	poid_set_fix_a_help = 
		"( value ) set fix poid a in interaction";
	AAALUACALL( poid_set_fix_a )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return poid_set_fix( l, boid_info_lua._boid_a );
	}

	C_PCHAR_C	poid_set_fix_b_help = 
		"( value ) set fix poid b in interaction";
	AAALUACALL( poid_set_fix_b )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return poid_set_fix( l, boid_info_lua._boid_b );
	}


	C_PCHAR_C	poid_split_id_help = 
		"( id ) split the id between the boids id and the poid\n"
		"boids_id, poid_id = poid.poid_split_id( id )";
	//	h8,low24 = bin_split_hi8_low24( int )
	AAALUACALL( poid_split_id )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		UINT32	i = l.get_uint32( 1 );

		l.push_int( (i>>24) & 0xff );
		l.push_int( i & 0xffffff );
		return 2;
	}

	#define	ADD_POID_FN( name )	l.add_fn_to_table( #name, poid_##name,  poid_##name##_help )

	void	register_poid( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table_global_get( "poid" );
			ADD_POID_FN( get_pos );
			ADD_POID_FN( set_pos );

			ADD_POID_FN( get_pos_to_draw );

			ADD_POID_FN( get_speed );
			ADD_POID_FN( set_speed );

			ADD_POID_FN( get_box_size );
			ADD_POID_FN( get_pos_a );
			ADD_POID_FN( get_pos_b );
			ADD_POID_FN( create_at );
			ADD_POID_FN( kill_all );
			ADD_POID_FN( kill_a );
			ADD_POID_FN( kill_b );
			ADD_POID_FN( kill_by_id );
			ADD_POID_FN( set_fix_a );
			ADD_POID_FN( set_fix_b );
			ADD_POID_FN( split_id );
		l.pop( 1 );	//pop table "poid"
	}

}	//end namespace poid
}	//end namespace aaalua
