#include "bdd_lua_flex.h"
#include "language/lua/aaalua_glue.h"
#include "obj_ui/bdd/bdd_simul/bdd_flex.h"


//todo	add fns in factory to deal faster with inheritance
//		in particular
//todo sold get_int32/get_int32_direct get_fp32/get_value_direct<FP32>
namespace aaalua
{
namespace n_bdd_flex
{

#if AAA_USE_FLEX()

bool	b_self_collide = true;
bool	b_fluid = false;
INT32	group_index = 0;
INT32   tag = 0;

// aaa.bdd.flex.create_begin( bdd_ref )
AAALUACALL( create_begin )
{
	LUAAAA_START( L, __FUNCTION__ );
	l.check_arg_nb( 1 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		bdd->begin_create();
		return l.return_true();
	}
	return l.return_nothing();
}

// aaa.bdd.flex.create_end( bdd_ref )
AAALUACALL( create_end )
{
	LUAAAA_START( L, __FUNCTION__ );
	l.check_arg_nb( 1 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		bdd->end_create();
		return l.return_true();
	}
	return l.return_nothing();
}

// nb = aaa.bdd.flex.get_particle_nb( bdd_ref )
AAALUACALL( get_particle_nb )
{
	LUAAAA_START( L, __FUNCTION__ );
	l.check_arg_nb( 1 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
		return l.return_uint32( bdd->get_host_particle_nb() );
	return l.return_nothing();
}

void get_def( c_lua_state& l, INT32 stack_id, INT32 CONST arg_nb  )
{
	b_self_collide =	l.get_bool(					stack_id	);
	b_fluid =			l.get_bool(					++stack_id	);
	group_index =		l.get_value_direct<INT32>(	++stack_id	);
	if( arg_nb > stack_id ) 
		tag =			l.get_value_direct<INT32>(	++stack_id	);
}

// aaa.bdd.flex.set_default( b_self_collide, b_fluid, group_idx )
AAALUACALL( set_default )
{
	LUAAAA_START( L, __FUNCTION__ );
	auto arg_nb = l.get_arg_nb( 3, 4 ); 
	get_def( l, 1, arg_nb );

	return l.return_nothing();
}


// aaa.bdd.flex.append_particle( bdd, x,y,z, vx,xy,xz, ux,uy, mass [,b_self_collide, b_fluid, group_idx [,tag] ]	)
AAALUACALL( append_particle )
{
	LUAAAA_START( L, __FUNCTION__ );
	auto arg_nb = l.get_arg_nb( 10, 13,14 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		FP32 pos[3];			l.get_v3( pos, 2 );
		FP32 vel[3];			l.get_v3( vel, 5 );
		FP32 texcoord[2];		l.get_v2( vel, 8 );
		FP32 mass =				l.get_value_direct<FP32>( 10 );
		if( arg_nb >= 11 )
			get_def( l, 11, arg_nb );

		bdd->append_particle( pos, vel, mass, texcoord, b_self_collide, b_fluid, group_index );
		return l.return_true();
	}
	return l.return_nothing();
}

// aaa.bdd.flex.append_spring( bdd_ref, from, to, stiffness, rest_length )
AAALUACALL( append_spring )
{
	LUAAAA_START( L, __FUNCTION__ );
	l.check_arg_nb( 5 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		INT32	from  =			l.get_value_direct<INT32>(2);
		INT32	to  =			l.get_value_direct<INT32>(3);
		FP32	stiffness  =	l.get_value_direct<FP32>( 4 );
		FP32	rest_length  =	l.get_value_direct<FP32>( 5 );

		bdd->append_spring( from, to, stiffness, rest_length );
		return l.return_true();
	}
	return l.return_nothing();
}

// aaa.bdd.flex.append_triangle( bdd_ref, i1,i2,i3, nx,ny,nz )
AAALUACALL( append_triangle )
{
	LUAAAA_START( L, __FUNCTION__ );
	l.check_arg_nb( 7 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		INT32	i1  =			l.get_value_direct<INT32>(2);
		INT32	i2  =			l.get_value_direct<INT32>(3);
		INT32	i3  =			l.get_value_direct<INT32>(4);
		FP32	norm[3];		l.get_v3( norm, 5 );

		bdd->append_triangle( i1,i2,i3, norm );
		return l.return_true();
	}
	return l.return_nothing();
}



// aaa.bdd.flex.append_box( bdd_ref, x,y,z, rx,ry,rz, vx,vy,vz, nb_x,nb_y,nb_z, radius, mass, b_rigid, stiffness [,b_self_collide, b_fluid, group_idx [,tag] ]	)
AAALUACALL( append_box )
{
static FP32*	fp32_buffer = nullptr;
static INT32	fp32_allocated_nb = 0;

	LUAAAA_START( L, __FUNCTION__ );
	auto arg_nb = l.get_arg_nb( 17, 20,21,22 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		FP32	pos[3];			l.get_v3( pos, 2 );
		FP32	rot[3];			l.get_v3( rot, 5 );		//	carefull here we should be in turn : use macros TURN_TO_RAD/DEG in aaa_math.h
		FP32	velocity[3];	l.get_v3( velocity, 8 );
		INT32	nb[3];			l.get_int32_v3( nb, 11 );
		FP32	spacing		=	l.get_value_direct<FP32>( 14 );	
		FP32	mass		=	l.get_value_direct<FP32>( 15 );
		bool	b_rigid		=	l.get_bool( 16 );
		FP32	stiffness	=	l.get_value_direct<FP32>( 17 );
		if( arg_nb >= 18 )
			get_def( l, 18, arg_nb );

		//unused for now (2024 Nov): was it a test
		FP32* mask = nullptr;
		if( arg_nb >= 22 )
		{
			INT32 grid_size = nb[0] * nb[1];
			if( fp32_allocated_nb < grid_size ) 
			{
				fp32_buffer	= (FP32*) REALLOC( fp32_buffer, grid_size*sizeof(FP32) );
				fp32_allocated_nb = grid_size;
			}
			
			luaL_checktype( L, 22, LUA_TTABLE );
			lua_pushnil(L);
			int idx = 0;
			while( lua_next( L, 22 ) )
			{
				lua_pop(L, 1);
				FP32 f = FP32(lua_tonumber(L, 0));	
				if( idx < grid_size )
					fp32_buffer[idx++] = f;
				else
				{
					//todo we could put a warning here about a bad table size
					break;
				}
			}
		}

		bdd->append_box( pos, rot, velocity, nb, spacing, mass, b_rigid, stiffness, b_self_collide, b_fluid, group_index, tag, fp32_buffer );

		return l.return_true();
	}
	return l.return_nothing();
}

// aaa.bdd.flex.append_sphere( bdd_ref, x,y,z, rx,ry,rz, vx,vy,vz, nb, radius, mass, b_rigid, stiffness [,b_self_collide, b_fluid, group_idx [,tag] ]	)
AAALUACALL( append_sphere )
{
	LUAAAA_START( L, __FUNCTION__ );
	auto arg_nb = l.get_arg_nb( 15, 18,19 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		FP32	pos[3];			l.get_v3( pos, 2 );
		FP32	rot[3];			l.get_v3( rot, 5 );
		FP32	velocity[3];	l.get_v3( velocity, 8 );
		INT32	nb			=	l.get_value_direct<INT32>(11);
		FP32	spacing		=	l.get_value_direct<FP32>( 12 );	
		FP32	mass		=	l.get_value_direct<FP32>( 13 );
		bool	b_rigid		=	l.get_bool( 14 );
		FP32	stiffness	=	l.get_value_direct<FP32>( 15 );
		if( arg_nb >= 16 )
			get_def( l, 16, arg_nb );

		bdd->append_sphere( pos, rot, velocity, nb, spacing, mass, b_rigid, stiffness, b_self_collide, b_fluid, group_index, tag );
		return l.return_true();
	}
	return l.return_nothing();
}

// aaa.bdd.flex.append_cylinder( bdd_ref, x,y,z, rx,ry,rz, vx,vy,vz, nb, radius, mass, b_rigid, stiffness,  [,b_self_collide, b_fluid, group_idx [,tag] ]	)
AAALUACALL( append_cylinder )
{
	LUAAAA_START( L, __FUNCTION__ );
	auto arg_nb = l.get_arg_nb( 16, 19,20 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		FP32	pos[3];			l.get_v3( pos, 2 );
		FP32	rot[3];			l.get_v3( rot, 5 );		
		FP32	velocity[3];	l.get_v3( velocity, 8 );
		INT32	nb  =			l.get_value_direct<INT32>(11);
		INT32	nb_z  =			l.get_value_direct<INT32>(12);
		FP32	spacing		=	l.get_value_direct<FP32>( 13 );	
		FP32	mass		=	l.get_value_direct<FP32>( 14 );
		bool	b_rigid		=	l.get_bool( 15 );
		FP32	stiffness	=	l.get_value_direct<FP32>( 16 );
		if( arg_nb >= 17 )
			get_def( l, 17, arg_nb );

		bdd->append_cylinder( pos, rot, velocity, nb, nb_z, spacing, mass, b_rigid, stiffness, b_self_collide, b_fluid, group_index, tag );
		return l.return_true();
	}
	return l.return_nothing();
}

//todo append_grid()
// aaa.bdd.flex.append_cloth_grid( bdd_ref, x,y,z, sx,sy, mass, axis, pin_mode, nb_u,nb_v,
//			stretch_use, stretch_stiff, stretch_rest,
//			bend_use, bend_stiff, bend_rest,
//			shear_use, shear_stiff, shear_rest
//			 [,b_self_collide, b_fluid, group_idx [,tag] ]	)
AAALUACALL( append_cloth_grid )
{
	LUAAAA_START( L, __FUNCTION__ );
	auto arg_nb = l.get_arg_nb( 20, 23,24 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		FP32	pos[3];					l.get_v3( pos, 2 );
		FP32	size[2];				l.get_v2( size, 5 );
		FP32	mass				=	l.get_value_direct<FP32>( 7 );
		INT32	axis				=	l.get_value_direct<INT32>(8);
		INT32	pin_mode			=	l.get_value_direct<INT32>(9);
		INT32	nb_u  =					l.get_value_direct<INT32>(10);
		INT32	nb_v  =					l.get_value_direct<INT32>(11);

		bool	b_stretch_use		=	l.get_bool( 12 );
		FP32	stretch_stiffness	=	l.get_value_direct<FP32>( 13 );
		FP32	stretch_rest_length	=	l.get_value_direct<FP32>( 14 );

		bool	b_bend_use			=	l.get_bool( 15 );
		FP32	bend_stiffness		=	l.get_value_direct<FP32>( 16 );
		FP32	bend_rest_length	=	l.get_value_direct<FP32>( 17 );

		bool	b_shear_use			=	l.get_bool( 18 );
		FP32	shear_stiffness		=	l.get_value_direct<FP32>( 19 );
		FP32	shear_rest_length	=	l.get_value_direct<FP32>( 20 );

		if( arg_nb >= 21 )
			get_def( l, 21, arg_nb );

		bdd->append_cloth_grid( pos, size, mass, axis, pin_mode, nb_u, nb_v, 
			b_stretch_use,	stretch_stiffness,	stretch_rest_length,
			b_bend_use,		bend_stiffness,		bend_rest_length,
			b_shear_use,	shear_stiffness,	shear_rest_length,
			b_self_collide, b_fluid, group_index );

		return l.return_true();
	}
	return l.return_nothing();
}

// nb = aaa.bdd.flex.get_rigid_nb( bdd_ref )
AAALUACALL( get_rigid_nb )
{
	LUAAAA_START( L, __FUNCTION__ );
	l.check_arg_nb( 1 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
		return l.return_int32( bdd->get_rigid_nb() );
	return l.return_nothing();
}

// nb = aaa.bdd.flex.get_rigid_position( bdd_ref , idx )
AAALUACALL( get_rigid_position )
{
	LUAAAA_START( L, __FUNCTION__ );
	l.check_arg_nb( 2 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		INT32 idx  = l.get_value_direct<INT32>(2);
		FP32 p[3];
		bdd->get_rigid( idx, &p[0] , nullptr );
		return l.return_fp32_v3( &p[0] );
	}
	return l.return_nothing();
}

AAALUACALL( get_rigid_rotation )
{
	LUAAAA_START( L, __FUNCTION__ );
	l.check_arg_nb( 2 );
	if( c_bdd_flex* CONST	bdd = l.get_aaa_obj_of_class<c_bdd_flex>(1) )
	{
		INT32 idx  = l.get_value_direct<INT32>(2);
		FP32 r[4];
		bdd->get_rigid( idx, nullptr, &r[0] );
		return l.return_fp32_v4( &r[0] );
	}
	return l.return_nothing();
}
#endif //#if AAA_USE_FLEX()

void	register_bdd_flex( lua_State* L )
{
#if AAA_USE_FLEX()
	LUAAAA_START( L, __FUNCTION__ );

	l.define_table( "flex" );

	ADD_FN( create_begin		);
	ADD_FN( create_end			);
	ADD_FN( get_particle_nb		);
	ADD_FN( set_default			);		
	ADD_FN( append_particle		);
	ADD_FN( append_spring		);
	ADD_FN( append_triangle		);
	ADD_FN( append_box			);
	ADD_FN( append_sphere		);
	ADD_FN( append_cylinder		);
	ADD_FN( append_cloth_grid	);

	//rigids
	ADD_FN( get_rigid_nb		);
	ADD_FN( get_rigid_position	);
	ADD_FN( get_rigid_rotation	);
	l.pop( 1 );	//pop new table
#endif //#if AAA_USE_FLEX()
}

}	//end namespace n_bdd
}	//end namespace aaalua