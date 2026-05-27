#include "boids/meca.h"
#include "obj_ui/deformer/def_node.h"
#include "infrastructure/compute_parallel.h"
#include "math/v.h"

//WORLD
c_meca_world::c_meca_world()
{
//	mass = 1;
	_speed_max_ui = 1.;
	_accel_max_ui = 1.;
}
void		c_meca_world::update()
{
	_mass_over_one = REAL(OVER_ONE(_mass_ui));
	//todo should we keep this : does _time_to_stop is a useful concept and accurate too ?
	_time_to_stop = REAL(_speed_max_ui * OVER_ONE( _accel_max_ui ));	//todo should the _mass interfere

	_b_speed		= is_not_null_v3( _speed );
	_b_offset		= is_not_null_v3( _offset_ui);

	_speed_min_squared = _speed_min_ui * _speed_min_ui;
	_speed_max_squared = _speed_max_ui * _speed_max_ui;
	_accel_max_squared = _accel_max_ui * _accel_max_ui;

	_viscosity_factor = REAL(1.)-_viscosity_ui;
}

//OBJ
void c_meca_obj::init()
{
	clear_v3( _pos				);
	clear_v3( _speed			);

	clear_v3( _pos_futur		);
	clear_v3( _pos_to_draw		);
	clear_v3( _pos_last			);
	clear_v3( _pos_to_draw_last	);
	clear_v3( _speed_last		);
	_speed_squared = 0.;

	clear_v3( _speed_last_used	);
	_speed_last_used_norm = 0.;
}

c_meca_obj::c_meca_obj( c_meca_world* world_in )
{
	_world = world_in;
	if( world_in )
		//todo strange test and message
		debug_break( "%s() empty world ??? old message", __FUNCTION__ );
	init();
}

void	c_meca_obj::do_deform( REAL* pos_tmp_buf, INT32 CONST nb, c_meca_obj * CONST * CONST hd_beg, c_def_node * CONST def, REAL CONST * CONST scale, bool CONST b_parallel )
{
	if( nb > 0 )
	{
		SPY_PUSH_RANGE( b_parallel ? "// c_meca_obj::do_deform" : "c_meca_obj::do_deform", spy::COL_2 );
		if( def->is_deforming() )
		{			
			if( is_all_one_v3( scale ) )
			{	//	copy to buffer no scaling
				if( b_parallel )
				{
					PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT 
					{
						cpy_v3( pos_tmp_buf + i * 3, (*(hd_beg + i))->get_pos() );
					} );
				}
				else
				{
					REAL*					p	= pos_tmp_buf;
					c_meca_obj * CONST *	hd	= hd_beg;
					for( INT32 i=nb; i>0; --i, p += 3, ++hd )
						cpy_v3( p, (*hd)->get_pos() );
				}
			}
			else
			{	//	copy to buffer while scaling
				if( b_parallel )
				{
					PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 i ) NOEXCEPT 
					{
						mul_v3( pos_tmp_buf + i * 3, (*(hd_beg + i))->get_pos(), scale );
					} );
				}
				else
				{
					REAL*					p	= pos_tmp_buf;
					c_meca_obj * CONST *	hd	= hd_beg;
					for( INT32 i=nb; i>0; --i, p += 3, ++hd )
						mul_v3( p, (*hd)->get_pos(), scale );
				}
			}

			//	deform
			def->apply( pos_tmp_buf, nb );

			//	copy from buffer
			if( b_parallel )
			{
				PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT 
				{
					cpy_v3( (*(hd_beg + i))->get_pos_to_draw(), pos_tmp_buf + i * 3 );
				} );
			}
			else
			{
				REAL*					p	= pos_tmp_buf;
				c_meca_obj * CONST *	hd	= hd_beg;
				for( INT32 i=nb; i>0; --i, p += 3, ++hd )
					cpy_v3( (*hd)->get_pos_to_draw(), p );
			}
		}
		else	// or simply scale
		{
			if( is_all_one_v3( scale ) )
			{
				if( b_parallel )
				{
					PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT 
					{
						c_meca_obj* b	= *(hd_beg + i);
						cpy_v3( b->get_pos_to_draw(), b->get_pos() );
					} );
				}
				else
				{
					c_meca_obj * CONST *	hd = hd_beg;
					for( INT32 i=nb; i>0; --i, ++hd )
						cpy_v3( (*hd)->get_pos_to_draw(), (*hd)->get_pos() );
				}
			}
			else
			{
				if( b_parallel )
				{
					PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT 
					{
						c_meca_obj* b	= *(hd_beg + i);
						mul_v3( b->get_pos_to_draw(), b->get_pos(), scale );
					} );
				}
				else
				{
					c_meca_obj * CONST *	hd = hd_beg;
					for( INT32 i=nb; i>0; --i, ++hd )
						mul_v3( (*hd)->get_pos_to_draw(), (*hd)->get_pos(), scale );
				}
			}
		}
		SPY_POP_RANGE();
	}
}

void	c_meca_obj::get_pos( REAL* dst, INT32 nb, c_meca_obj * CONST * hd, bool CONST b_futur, bool CONST b_parallel )
{
	if( nb > 0 )
	{
		if( b_parallel )
		{
			SPY_PUSH_RANGE( "// c_meca_obj::get_pos", spy::COL_2 );
			if( b_futur )
				PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT {	cpy_v3( dst+i*3,	(*(hd+i))->get_pos_futur()	); } );
			else
				PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT {	cpy_v3( dst+i*3,	(*(hd+i))->get_pos()		); } );
		}
		else
		{
			SPY_PUSH_RANGE( "c_meca_obj::get_pos", spy::COL_2 );
			if( b_futur )
				for( ; nb>0; --nb, dst+=3, ++hd )
				{
					cpy_v3(	dst,		(*hd)->get_pos_futur()		);
				}
			else
				for( ; nb>0; --nb, dst+=3, ++hd )
				{
					cpy_v3(	dst,		(*hd)->get_pos()			);
				}
		}
		SPY_POP_RANGE();
	}
}
