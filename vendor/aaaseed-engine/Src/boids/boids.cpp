#include "boids.h"
#include "boid_universe.h"
#include "boid.h"
#include "obj_ui/bdd/bdd_point/bdd_boid.h"
#include "obj_ui/bdd/bdd_point/bdd_point.h"
#include "draw/model.h"
#include "language/lua/aaalua_util.h"
#include "language/lua/aaalua_wrap.h"
#include "spy.h"
#include "infrastructure/compute_parallel.h"
#if AAA_VSTOOL() >= 141
#	include <execution>
#endif

c_influence::c_influence()		{	clear_v3( _pos );	}
c_influence::~c_influence()		{};

void	c_influence::update()
{
		_radius_center		= (_radius_ext_ui + _radius_int_ui) * REAL(.5);
		_radius_size_half	= (_radius_ext_ui - _radius_int_ui) * REAL(.5);
		axe_build_index( _i_u, _i_v, _i_axe );
}

FINLINE	void c_box_3d::update() NOEXCEPT
{
	scale_v3( _size, _size_ui, _size_ui[3] );

	REAL tmp = REAL(_size[0] * .5);
	if( tmp >= 0.)
	{
		_min[0] = _origin_ui[0] - tmp;
		_max[0] = _origin_ui[0] + tmp;
	}
	else
	{
		_min[0] = _origin_ui[0] + tmp;
		_max[0] = _origin_ui[0] - tmp;
	}
	tmp = REAL(_size[1] * .5);
	if( tmp >= 0.)
	{
		_min[1] = _origin_ui[1] - tmp;
		_max[1] = _origin_ui[1] + tmp;
	}
	else
	{
		_min[1] = _origin_ui[1] + tmp;
		_max[1] = _origin_ui[1] - tmp;
	}
	tmp = REAL(_size[2] * .5);
	if( tmp >= 0.)
	{
		_min[2] = _origin_ui[2] - tmp;
		_max[2] = _origin_ui[2] + tmp;
	}
	else
	{
		_min[2] = _origin_ui[2] + tmp;
		_max[2] = _origin_ui[2] - tmp;
	}
}

C_PCHAR_C	c_boids::repulse_by_other_type_str[REPULSE_TYPE_MAX_NB] =
{
	"OFF",
	"like_self",
	"explicit",
	"like_other",
//	"min_self_other",
//	"max_self_other",
};

C_PCHAR_C	c_boids::lock_method_str[LOCK_METHOD_MAX_NB] =
{
	"Master",
	"No",
	"BY_BOID",
	"BY_CONTACT",
};

C_PCHAR_C	c_boids::interaction_method[INTERACTION_METHOD::INTERACTION_NB] =
{
	"ADD",
	"SWITCH",
	"SMOOTH"
};


bool	c_boids::b_master_living_box_draw_ui;
bool	c_boids::b_master_box_draw_force_ui	;
bool	c_boids::b_master_draw_force_ui		;

REAL	c_boids::master_dist_scale_ui		;
REAL	c_boids::master_dist_repulse_scale_ui;
REAL	c_boids::master_dist_flock_scale_ui;
REAL	c_boids::master_dist_steer_scale_ui;

bool	c_boids::b_master_repulse_allow_ui	;
bool	c_boids::b_master_flock_allow_ui	;
bool	c_boids::b_master_steer_allow_ui	;

bool					c_boids::b_master_parallel_allow_ui	;
bool					c_boids::b_master_parallel_min_nb_force_ui;
INT32					c_boids::master_parallel_min_nb_alive_ui;
c_boids::LOCK_METHOD	c_boids::s_master_parallel_lock_method_ui;

master::NO_ALLOW_FORCE	c_boids::s_master_net_draw_ui		= master::ALLOW;
master::NO_ALLOW_FORCE	c_boids::s_master_repulse_draw_ui	= master::ALLOW;
master::NO_ALLOW_FORCE	c_boids::s_master_flock_draw_ui		= master::ALLOW;
master::NO_ALLOW_FORCE	c_boids::s_master_steer_draw_ui		= master::ALLOW;


namespace {
	c_rand_lin	noise_separation;
	c_rand_lin	rand_create_and_place;
};

c_boids::c_boids( )
	:_i_u(0)
	,_i_v(1)
	,_i_axe(2)
	,_field_in(nullptr)
	,_field_out(nullptr)
	,_field_repulse_out(nullptr)
	,_nb_allocated(0)
	,_contacts(nullptr)
	,_contacts_nb_allocated(0)
	,_contacts_nb(0)
	,_hd_calc(nullptr)
	,_nb_alive(0)
	,_hd_draw(nullptr)
	,_nb_draw(0)
//todo the ui stuff should be inited by the associated bdd or should be consider autonomy ?
	,_test_sucess_ui(0)
	,_test_failed_ui(0)
	,_b_parallel(false)
	,_def_node_repulse(nullptr)
	,_s_repulse_by_other_type_ui(0)
	,_b_repulse_field_ui(false)							//needed because used in param_init_pt 
	,_s_repulse_field_ui(c_def_node::FIELD_TYPE_NO)		//needed because used in param_init_pt 
	,_id_generator(0)

{
	_poids = new c_obj_array_server<c_boid>;
	clear_v3( _center_ui );
}

c_boids::~c_boids()
{
	c_boid_universe::remove_boid( this );
	dealloc();
	delete _poids;
}

void	c_boids::restart()
{
	kill_all();

	_death_nb_ui = 0;
	_birth_nb_ui = 0;
	_id_generator = 0;
	_contacts_nb = 0;
}


bool	c_boids::alloc( UINT32 nb )
{
	if( nb != _nb_allocated )
	{
		dealloc();
		restart();

		_poids->alloc( nb );

		_hd_calc	= new pboid[nb];
		_hd_calc[0]	= nullptr;	//useful mark to invalidate structure
		_hd_draw	= new pboid[nb];
		_hd_draw[0]	= nullptr;	//useful mark to invalidate structure

		INT32 nb_float = nb*3;
		_field_in			= new REAL[nb_float];
		_field_out			= new REAL[nb_float];
		_field_repulse_out	= new REAL[nb_float];

		c_boid*	b;
		for( UINT32 i=0; i<nb; ++i )
		{
			b = _poids->get_pt_from_index(i);
			b->set_boids( this );
			_unused.push_back( b );
		}
		_nb_allocated = nb;

		//todo this was 32 bits code
		nb = MIN( nb, (UINT32)2048 );	// nb > 0x4000 crash the memory
										//	and anyhow this is too much (2M contact max)
		nb = (nb*(nb-1))/2;				// only for i<j

		_contacts = new c_poid_contact[nb];
		_contacts_nb_allocated = _contacts ? nb : 0 ;
	}
	return true;
}

void	c_boids::dealloc()
{
	_living.clear();

	_unused.clear();

	//LOOPER_WHILE()
	//	kill_one( *it++ );
	//_looper.dealloc();
	kill_all();

	SAFE_DELETE_ARRAY( _contacts );

	SAFE_DELETE_ARRAY( _field_repulse_out );
	SAFE_DELETE_ARRAY( _field_out );
	SAFE_DELETE_ARRAY( _field_in );

	SAFE_DELETE_ARRAY( _hd_draw );
	SAFE_DELETE_ARRAY( _hd_calc );

	_poids->dealloc();
}

template< bool B_FIRST >
FINLINE	bool	compute_dist_squared_boxed_if( REAL diff, REAL CONST box_add, REAL& dst, REAL& dist_squared, REAL CONST dist_squared_max )
{
	if( diff > 0. )
	{
		diff -= box_add;
		if( diff >= 0. )
		{
			if CONSTEXPR ( B_FIRST )
				dist_squared	=	diff * diff;
			else
				dist_squared	+=	diff * diff;

			if( dist_squared > dist_squared_max )
				return false;
			dst = diff;
		}
		else
		{
			dst = 0.;
			if CONSTEXPR ( B_FIRST )
				dist_squared	= 0.;
		}
	}
	else
	{
		diff += box_add;
		if( diff <= 0. )
		{
			if CONSTEXPR( B_FIRST )
				dist_squared	=	diff * diff;
			else
				dist_squared	+=	diff * diff;

			if( dist_squared > dist_squared_max )
				return false;
			dst = diff;
		}
		else
		{
			dst = 0.;
			if CONSTEXPR ( B_FIRST )
				dist_squared	= 0.;
		}
	}
	return true;
}

template< INT32 DIM >
FINLINE	bool	c_boids::compute_dist_squared_and_vector_boxed_if_inferior(	REAL CONST * CONST RESTRICT pos_i, REAL CONST * CONST RESTRICT pos_j,
																			REAL CONST * CONST RESTRICT box_i, REAL CONST * CONST RESTRICT box_j,
																			REAL* CONST RESTRICT vec, REAL& dist_squared ) CONST NOEXCEPT
{
	if CONSTEXPR ( DIM == 1 )
		return compute_dist_squared_boxed_if<true>(	pos_i[_i_axe] - pos_j[_i_axe],	box_i[_i_axe] +	box_j[_i_axe],	vec[_i_axe],	dist_squared, _interact_dist_squared );
	if( !compute_dist_squared_boxed_if<true>(		pos_i[_i_u]   - pos_j[_i_u],	box_i[_i_u]   +	box_j[_i_u],	vec[_i_u],		dist_squared, _interact_dist_squared ) )
		return false;
	if( !compute_dist_squared_boxed_if<false>(		pos_i[_i_v]   - pos_j[_i_v],	box_i[_i_v]   +	box_j[_i_v],	vec[_i_v],		dist_squared, _interact_dist_squared ) )
		return false;
	if CONSTEXPR ( DIM == 2 )
	{
		vec[_i_axe] = 0.;
		return true;
	}
	return compute_dist_squared_boxed_if<false>(	pos_i[_i_axe] - pos_j[_i_axe],	box_i[_i_axe] + box_j[_i_axe], vec[_i_axe],		dist_squared, _interact_dist_squared );
}

//#if 0
//template< bool B_FIRST >
//FINLINE	bool	compute_dist_squared_if( REAL CONST diff, REAL& dst, REAL& dist_squared, REAL CONST max )
//{
//	if CONSTEXPR ( B_FIRST )
//		dist_squared	=  diff * diff;
//	else
// 		dist_squared	+= diff * diff;
//
//	if( dist_squared > max )
//		return false;
//	dst = diff;
//	return true;
//}
//
//template< INT32 DIM >
//FINLINE	bool	c_boids::compute_dist_squared_and_vector_if_inferior(	REAL CONST * CONST pos_i, REAL CONST * CONST pos_j,
//																			REAL* CONST vec, REAL& dist_squared )
//{
//	if CONSTEXPR ( DIM == 1 )
//		return compute_dist_squared_if<true>(	pos_i[_i_axe] - pos_j[_i_axe],	vec[_i_axe],	dist_squared, _interact_dist_squared );
//	if( !compute_dist_squared_if<true>(			pos_i[_i_u] - pos_j[_i_u],		vec[_i_u],		dist_squared, _interact_dist_squared ) )
//		return false;
//	if( !compute_dist_squared_if<false>(		pos_i[_i_v] - pos_j[_i_v],		vec[_i_v],		dist_squared, _interact_dist_squared ) )
//		return false;
//	if CONSTEXPR ( DIM == 2 )
//	{
//		vec[_i_axe] = 0.;
//		return true;
//	}
//	return compute_dist_squared_if<false>(		pos_i[_i_axe] - pos_j[_i_axe],	vec[_i_axe],	dist_squared, _interact_dist_squared );
//}
//#else
//template< INT32 DIM >
//FINLINE	bool	compute_dist_squared_and_vector_if_inferior(	REAL CONST * CONST pos_i, REAL CONST * CONST pos_j, REAL * CONST vec,
//																REAL& dist_squared, REAL CONST dist_squared_max )
//{
//	REAL diff;
//	switch( DIM )
//	{
//	case 1:
//		diff = pos_i[_i_axe] - pos_j[_i_axe];
//		dist_squared	= diff * diff;
//		if( dist_squared > dist_squared_max )
//			return false;
//		vec[_i_axe] = diff;
//
//		return true;
//	case 2:
//		diff = pos_i[_i_u] - pos_j[_i_u];
//		dist_squared	= diff * diff;
//		if( dist_squared > dist_squared_max )
//			return false;
//		vec[_i_u] = diff;
//
//		diff = pos_i[_i_v] - pos_j[_i_v];
//		dist_squared	+= diff * diff;
//		if( dist_squared > dist_squared_max )
//			return false;
//		vec[_i_v] = diff;
//
//		vec[_i_axe] = 0.;
//		return true;
//	case 3:
//	default:
//		diff = pos_i[_i_u] - pos_j[_i_u];
//		dist_squared	= diff * diff;
//		if( dist_squared > dist_squared_max )
//			return false;
//		vec[_i_u] = diff;
//
//		diff = pos_i[_i_v] - pos_j[_i_v];
//		dist_squared	+= diff * diff;
//		if( dist_squared > dist_squared_max )
//			return false;
//		vec[_i_v] = diff;
//
//		diff = pos_i[_i_axe] - pos_j[_i_axe];
//		dist_squared	+= diff * diff;
//		if( dist_squared > dist_squared_max )
//			return false;
//		vec[_i_axe] = diff;
//
//		return true;
//	}
//}
//#endif


template< INT32 DIM >
FINLINE REAL	c_boids::get_cos( REAL CONST * CONST a, REAL CONST a_norm_over_one, REAL CONST * CONST b ) CONST
{
	static REAL val_def = 1.;
	switch( DIM )
	{
	case 2:
		{
			//todoopt speed should be normalize only once by poid (use flag)
			REAL	bu = b[_i_u];
			REAL	bv = b[_i_v];
			REAL	n = sum_squared_v2r( bu, bv );	//	square of norm
			if( n >= 0.001f )
			//if( n != 0.f )
				return REAL( ( a[_i_u] * bu + a[_i_v] * bv ) * OVER_ONE(SQRT(n)) * a_norm_over_one );
		}
		break;
	case 3:
		{
			REAL	n = norm_squared_v3r( b ); 
			if( n >= 0.001f )
			//if( n != 0.f )
				return REAL( dot_v3r( a, b ) * OVER_ONE(SQRT(n)) * a_norm_over_one );
		}
		break;
	}
	val_def = -val_def;	// we flip or it will have a bias on one side
	return val_def;
}

//	RETURN true caller should break his loop
//METHOD is not used for now
template< INT32 DIM, c_boids::INTERACTION_METHOD METHOD, bool B_SELF, INT32 S_LOCK >
FINLINE	void	c_boids::update_interaction_one( c_boid* CONST RESTRICT bi, c_boid* CONST RESTRICT bj ) NOEXCEPT
{
	REAL	dist_squared;
	REAL	vec[3];	//	would be the vector with the difference between the 2 poid

	REAL*	pos_i = bi->get_pos();
	REAL*	pos_j = bj->get_pos();

	//	compute vec but return as soon we are not close enough
	if( _b_visibility_with_box_ui )
	{	// compute distance between the box
		if( !compute_dist_squared_and_vector_boxed_if_inferior<DIM>(	pos_i, pos_j, bi->_box_size_half, bj->_box_size_half, vec, dist_squared ) )
			return;	// false;
	}
	else
	{
		REAL diff;
		switch( DIM )
		{
		case 1:
			diff = pos_i[_i_axe] - pos_j[_i_axe];
			dist_squared	= diff * diff;
			if( dist_squared > _interact_dist_squared )
				return;
			vec[_i_axe] = diff;
			break;
		case 2:
			diff = pos_i[_i_u] - pos_j[_i_u];
			dist_squared	= diff * diff;
			if( dist_squared > _interact_dist_squared )
				return;
			vec[_i_u] = diff;

			diff = pos_i[_i_v] - pos_j[_i_v];
			dist_squared	+= diff * diff;
			if( dist_squared > _interact_dist_squared )
				return;
			vec[_i_v] = diff;
			vec[_i_axe] = 0.;
			break;
		case 3:
		default:
			diff = pos_i[_i_u] - pos_j[_i_u];
			dist_squared	= diff * diff;
			if( dist_squared > _interact_dist_squared )
				return;
			vec[_i_u] = diff;

			diff = pos_i[_i_v] - pos_j[_i_v];
			dist_squared	+= diff * diff;
			if( dist_squared > _interact_dist_squared )
				return;
			vec[_i_v] = diff;

			diff = pos_i[_i_axe] - pos_j[_i_axe];
			dist_squared	+= diff * diff;
			if( dist_squared > _interact_dist_squared )
				return;
			vec[_i_axe] = diff;
			break;
		}
//		if( !compute_dist_squared_and_vector_if_inferior		<DIM>(	pos_i, pos_j, vec, dist_squared, _interact_dist_squared ) )
//			return;	// false;
	}

	//ok now we are at proper distance to have interaction 
	if( dist_squared <= _repulse_force_separation_dist_squared )	//noise to separate sticky boids
	{
		switch( DIM )
		{
		case 1:
			dist_squared = noise_separation.get_fp32() * _repulse_force_separation_dist_squared ;
			vec[_i_axe] = dist_squared ;
			dist_squared *= dist_squared ;
			break;
		case 2:
			{
				REAL f = _repulse_force_separation_factor;
				dist_squared = noise_separation.get_fp32() * f ;
				vec[_i_u] = dist_squared ;
				dist_squared *= dist_squared ;
				REAL b = noise_separation.get_fp32() * f ;
				vec[_i_v] = b ;
				dist_squared += b * b ;
			}
			break;
		case 3:
			{
				REAL f = _repulse_force_separation_factor;
				dist_squared = noise_separation.get_fp32() * f ;
				vec[0] = dist_squared ;
				dist_squared *= dist_squared ;
				REAL b = noise_separation.get_fp32() * f ;
				vec[1] = b ;
				dist_squared += b * b ;
				b = noise_separation.get_fp32() * f ;
				vec[2] = b ;
				dist_squared += b * b ;
			}
			break;
		}
	}
	
	bool	b_vi;
	bool	b_vj;
	REAL	norm = -42;
	REAL	norm_over_one;
	//		but later we can scale the space to be sure to be in the 0 1 interval and/or treat it with tables
	//		avoiding this just make the rest more complex
	//		in particular dealing with boid radius
	if( (DIM != 1) && _b_vis )
	{

		//	this computation is a pain in performance
		//		so I should opt with tables
		norm = SQRT( dist_squared );
		norm_over_one = OVER_ONE_AS_REAL(norm);
		//		but later we can scale the space to be sure to be in the 0 1 interval and/or treat it with tables
		//		avoiding this just make the rest more complex
		//		in particular dealing with boid radius

		REAL cos = - get_cos<DIM>( vec, norm_over_one, bi->get_speed() );
		b_vi = cos > _visibility_cos;

		if CONSTEXPR ( B_SELF )
		{
			cos = get_cos<DIM>( vec, norm_over_one, bj->get_speed() );
			b_vj = cos > _visibility_cos;
			if( b_vi )
			{
				bi->inc_seen();
				if( b_vj )
					bj->inc_seen();
			}
			else
			{
				if( b_vj )
					bj->inc_seen();
				else
					return;	// false;	
			}
		}
		else
		{
			if( b_vi )
				bi->inc_seen();
			else
				return;	// false; // nothing to do in this case
		}
		//if all active repulse steer flock use visibility check if no visibility
	}
	else
	{
		b_vi = true;
		bi->inc_seen();

		if CONSTEXPR ( B_SELF )
		{
			b_vj = true;
			bj->inc_seen();
		}
	}

	//	if one of the poid have a radius we should refine the distance computation
	if( _interact_radius_to_substract != 0. )
	{
		if( norm < 0. )
		{
			norm = SQRT( dist_squared );
			norm_over_one = OVER_ONE_AS_REAL(norm);
		}
		REAL	norm_edge = norm - _interact_radius_to_substract;
		if( norm_edge > 0. )	//	ok we repulse from edge instead of center
		{
			scale_v3( vec, norm_edge * norm_over_one );
			norm = norm_edge;
		}
		else	// this time there is contact even penetration
		{	//	this 
			norm = _interact_dist * REAL(.001);
			scale_v3( vec, norm * norm_over_one );
		}
		//norm_over_one = OVER_ONE(norm);
		dist_squared = norm * norm;
		norm_over_one = OVER_ONE_AS_REAL(norm);
	}

//	_set_SSE2_enable( _b_test ? 1 : 0 );
//	dist_squared = SQRT( dist_squared );
//	dist_squared = POW( dist_squared, 2. );

	if( _contact_do_lua_ref != LUA_NOREF )
	{
		boid_info_lua._boid_a = bi;
		boid_info_lua._boid_b = bj;
		if( g_lua_wrap_cur->do_fn_r_pass_ii_final( _contact_do_lua_ref, bi->get_id(), bj->get_id(), 1. ) < 0.5 )
			return; // false;
	}

#if BOID_USE_LOCK()
	if CONSTEXPR ( S_LOCK != LOCK_METHOD_NO )
	{
		if CONSTEXPR ( S_LOCK == LOCK_METHOD_BY_BOID )
			bj->lock();
		else //	if( S_LOCK == LOCK_METHOD_BY_CONTACT ) 
		{
			bi->lock();
			bj->lock();
		}
	}
#endif

//	if( norm < _repulse_dist )
	if( dist_squared < _repulse_dist_squared )
	{
		REAL repulse_dist;

		if( _b_repulse_field )
			repulse_dist = _b_repulse_field_max_ui ? MAX( bi->_dist_repulse, bj->_dist_repulse ) : MIN( bi->_dist_repulse, bj->_dist_repulse );
		else
			repulse_dist = _repulse_dist;

//		if( norm < repulse_dist )
		if( dist_squared < repulse_dist * repulse_dist )
		{
			{
				INT32 ind = ++_contacts_index;	//this id an atomic	
				if( _b_need_contact && ind < _contacts_nb_allocated )
				{
					REAL	norm = SQRT( dist_squared );
					_contacts[ind].set( bi, bj, b_vi, B_SELF ? b_vj : false, norm * ( _b_repulse_field ? OVER_ONE_AS_REAL( repulse_dist ) : _repulse_dist_over_one ) );
				}
			}

			REAL	repulse[3];
			REAL	sca;
			if( norm < 0. )
			{
				norm = SQRT( dist_squared );
				norm_over_one = OVER_ONE_AS_REAL(norm);
			}
			//	we should use bias on reverse of this to avoid a pow
			sca = repulse_dist * norm_over_one;	// this is d/x
			//	this is a repulsion in (d^2/x^2-1)*d which scale correctly with d (distance of influence)
			//	but we divide by x = norm(vec) because we will multiply vec
			//	(d^2/x^2-1)*d/x = (d/x)^3 - d/x
			//	we should aim at ((d/x)^n-1)*d/x
			sca = (sca*sca - REAL(1)) * sca;
			if( _repulse_old_ui != 0. )
			{
				//	this like (d^2/x^2-d/x)/d so it scale badly with d
				//		should disappear
				REAL tmp = (repulse_dist - norm) * norm_over_one * norm_over_one;
				if( _repulse_special_ui != 0. )
				{
					//	1 - dist/repulse_dist_squared don't repulse well when close
					//	in fact this is an aberration : repulsion go to zero
					//	so it can regroup boids under constraint
					//	but sometime this is really nice so "the bug" is kept under _repulse_special influence
					//	I should rework the curve here
					REAL tmp = _b_repulse_field ? OVER_ONE_AS_REAL( repulse_dist ) : _repulse_dist_over_one;
					tmp = interpolate( tmp, 1 - dist_squared * tmp * tmp, _repulse_special_ui );
				}
				sca = interpolate( sca, tmp, _repulse_old_ui );
			}
			scale_v3( repulse, vec, sca * _repulse_factor );

			if( _b_vis_repul )
			{
				if( b_vi )
					bi->add_repulse( repulse );
				if CONSTEXPR ( B_SELF )
				{
					if( b_vj )
						bj->sub_repulse( repulse );
				}
			}
			else
			{
				bi->add_repulse( repulse );
				if CONSTEXPR ( B_SELF )
					bj->sub_repulse( repulse );
			}
		}
	}

	if CONSTEXPR ( B_SELF )
	{
		if( dist_squared < _flock_dist_squared && _flock_dist_squared_interior <= dist_squared )
		{
			if( _b_vis_flock )
			{
				if( b_vi )
					bi->add_flock( pos_j );
				if( b_vj )
					bj->add_flock( pos_i );
			}
			else
			{
				bi->add_flock( pos_j );
				bj->add_flock( pos_i );
			}
		}

		if( dist_squared < _steer_dist_squared )
		{
			if( _b_vis_steer )
			{
				if( b_vi )
					bi->add_steer( bj->get_speed() );
				if( b_vj )
					bj->add_steer( bi->get_speed() );
			}
			else
			{
				bi->add_steer( bj->get_speed() );
				bj->add_steer( bi->get_speed() );
			}
		}
	}

	if( _b_death_by_choking_ui )
	{
		if( bi->_seen_count >= _death_choking_nb_ui )
		{
			//	kill_next_update( bi );
			bi->mark_to_kill();
			//	return true;	//	we go on to avoid asymmetry due to sorting for optimization
		}
		if( bj->_seen_count >= _death_choking_nb_ui )
		{
			bj->mark_to_kill();
			//return true;
		}
	}
//	if( bi->_seen_count >= _nb_seen_max_ui )
//		return true;
//	return false;
		
#if BOID_USE_LOCK()
	if CONSTEXPR ( S_LOCK != LOCK_METHOD_NO )
	{
		if CONSTEXPR ( S_LOCK == LOCK_METHOD_BY_BOID )
			bj->unlock();
		else //	if( S_LOCK == LOCK_METHOD_BY_CONTACT )
		{
			bi->unlock();
			bj->unlock();
		}
	}
#endif

}

//todoq do the parallel version
template< INT32 DIM, c_boids::INTERACTION_METHOD METHOD, INT32 S_LOCK >
void	c_boids::update_interaction_other( c_boids* CONST targ ) NOEXCEPT
{
	INT32		j_start		= targ->_nb_alive;
	c_boid**	hd_j_start	= targ->_hd_calc-1;
	c_boid**	hd_i		= _hd_calc-1;

	for( INT32 i=0; i<_nb_alive; ++i )
	{
		c_boid*	RESTRICT	bi		= *++hd_i;

		INT32				ga		= bi->_ga;
		INT32				gb		= bi->_gb;

		INT32				j		= j_start;
		c_boid**			hd_j	= hd_j_start;
		c_boid*	RESTRICT	bj;
		for( ; j>0; --j )	
		{							
			bj = *++hd_j;
			if( ga - bj->_ga <= _cluster_interact_nb )	// boid are sorted by cluster on pos[_i_u]
				break;
		}
		j_start = j;
		hd_j_start = hd_j-1; 
		//todo store j and pointer for next iteration
		while( j>0 )
		{							
			if( bj->_ga - ga >= _cluster_interact_nb )
				break;
			if( ( (DIM<=1)) || INSIDE_ABS( gb - bj->_gb, _cluster_interact_nb ) )	// boid are sorted by cluster on pos[_i_u]
			{
				update_interaction_one< DIM, METHOD, false, S_LOCK > ( bi, bj );
				if( bi->_seen_count >= _nb_seen_max_ui )
					break;
			}
			bj = *++hd_j;
			--j;
		}
	}	//end	i
}

template< INT32 DIM, c_boids::INTERACTION_METHOD METHOD >
void	c_boids::update_interaction_other( c_boids* CONST targ ) NOEXCEPT
{
	switch( _s_lock_method )
	{
	case LOCK_METHOD_NO:			update_interaction_other< DIM, METHOD, LOCK_METHOD_NO			>( targ );	break;
	case LOCK_METHOD_BY_BOID:		update_interaction_other< DIM, METHOD, LOCK_METHOD_BY_BOID		>( targ );	break;
	case LOCK_METHOD_BY_CONTACT:	update_interaction_other< DIM, METHOD, LOCK_METHOD_BY_CONTACT	>( targ );	break;
	default:		break;
	}
}

//interaction unused skip this call 
//interaction unused skip this call (change next fn too)
//template< INT32 DIM >
//void	c_boids::update_interaction_other( c_boids* CONST targ ) NOEXCEPT
//{
//	switch( _s_interaction_method )
//	{
//	case INTERACTION_ADD:		update_interaction_other< DIM, INTERACTION_ADD		>( targ );	break;
//	case INTERACTION_SWITCH:	update_interaction_other< DIM, INTERACTION_SWITCH	>( targ );	break;
//	case INTERACTION_SNOOTH:	update_interaction_other< DIM, INTERACTION_SNOOTH	>( targ );	break;
//	default:	break;
//	}
//}

void	c_boids::update_interaction_other( c_boids* CONST targ ) NOEXCEPT
{
	SPY_PUSH_RANGE( "Other inter", spy::COL_1 );
		switch( _s_dim_ui )
		{
		case 1:		update_interaction_other< 1, INTERACTION_ADD >( targ ); break;
		case 2:		update_interaction_other< 2, INTERACTION_ADD >( targ ); break;
		case 3:		update_interaction_other< 3, INTERACTION_ADD >( targ ); break;
		default:	break;
		}
	SPY_POP_RANGE();
}

template< INT32 DIM, c_boids::INTERACTION_METHOD METHOD, INT32 S_LOCK >
void	c_boids::update_interaction_self() NOEXCEPT
{
	_test_sucess_ui = 0;
	_test_failed_ui = 0;

	c_boid** hd_i_max = _hd_calc + _nb_alive;
	if( _b_visibility_with_box_ui )	//hack	in fact there is no more optimization
	{
		for( c_boid** hd_i = _hd_calc + 1; hd_i < hd_i_max; ++hd_i )
		{
			c_boid*	RESTRICT bi = *hd_i;
			for( c_boid** hd_j = hd_i-1; hd_j>=_hd_calc; --hd_j )	// we only compare for j<i
			{							// these boids are already initialized by the code just before
				update_interaction_one< DIM, METHOD, true, S_LOCK >( bi, *hd_j );
				if( bi->_seen_count >= _nb_seen_max_ui )
					break;
			}	//end	j
		}	//end	i
	}
	else
	{
		if( _b_parallel )
		{
			INT32 nb = INT32( hd_i_max - _hd_calc );
			// we start at 1 because to protect inside loop from indexing as -1
			PARALLEL_LIB::parallel_for( 1, nb, [&]( INT32 CONST i ) NOEXCEPT
			{
			//for( c_boid** hd_i = _hd_calc + 1; hd_i < hd_i_max; ++hd_i )
			//{
				c_boid*	bi = *(_hd_calc + i);
#if BOID_USE_LOCK()
				if CONSTEXPR ( S_LOCK == LOCK_METHOD_BY_BOID )
					bi->lock();
#endif
					INT32	ga_min = bi->_ga - _cluster_interact_nb;
					c_boid** hd_j = _hd_calc + i - 1;	//this why we start at 1

					if CONSTEXPR ( DIM>1 )
					{
						INT32 gb = bi->_gb;
						INT32 gb_min = gb - _cluster_interact_nb;
						INT32 gb_max = gb + _cluster_interact_nb;
						for( ; hd_j>=_hd_calc; --hd_j )	// we only compare for j<i
						{							// these boids are already initialized by the code just before
							c_boid*	bj = *hd_j;
							if( bj->_ga < ga_min )	// boid are sorted by cluster on one axis
								break;
							if( INSIDE_MIN_MAX( bj->_gb, gb_min, gb_max ) )	// boid are sorted by cluster then on a second axe
							{
								update_interaction_one< DIM, METHOD, true, S_LOCK >( bi, bj );
								if( bi->_seen_count >= _nb_seen_max_ui )
									break;
							}
						}	//end	j
					}
					else
					{
						for( ; hd_j>=_hd_calc; --hd_j )	// we only compare for j<i
						{							// these boids are already initialized by the code just before
							c_boid*	bj = *hd_j;
							if( bj->_ga < ga_min )	// boid are sorted by cluster on one axis
								break;
							update_interaction_one< DIM, METHOD, true, S_LOCK >( bi, bj );
							if( bi->_seen_count >= _nb_seen_max_ui )
								break;
						}	//end	j
					}
#if BOID_USE_LOCK()
				if CONSTEXPR ( S_LOCK == LOCK_METHOD_BY_BOID )
					bi->unlock();
#endif
			}	//end	i
			);
		}
		else
		{
			for( c_boid** hd_i = _hd_calc + 1; hd_i < hd_i_max; ++hd_i )
			{
				c_boid*	bi = *hd_i;
				INT32	ga_min = bi->_ga - _cluster_interact_nb;

				if CONSTEXPR ( DIM>1 )
				{
					INT32 gb = bi->_gb;
					INT32 gb_min = gb - _cluster_interact_nb;
					INT32 gb_max = gb + _cluster_interact_nb;
					for( c_boid** hd_j = hd_i-1; hd_j>=_hd_calc; --hd_j )	// we only compare for j<i
					{							// these boids are already initialized by the code just before
						c_boid*	bj = *hd_j;
						if( bj->_ga < ga_min )	// boid are sorted by cluster on one axis
							break;
						if( INSIDE_MIN_MAX( bj->_gb, gb_min, gb_max ) )	// boid are sorted by cluster then on a second axe
						{
							update_interaction_one< DIM, METHOD, true, S_LOCK >( bi, bj );
							if( bi->_seen_count >= _nb_seen_max_ui )
								break;
						}
					}	//end	j
				}
				else
				{
					for( c_boid** hd_j = hd_i-1; hd_j>=_hd_calc; --hd_j )	// we only compare for j<i
					{							// these boids are already initialized by the code just before
						c_boid*	bj = *hd_j;
						if( bj->_ga < ga_min )	// boid are sorted by cluster on one axis
							break;
						update_interaction_one< DIM, METHOD, true, S_LOCK >( bi, bj );
						if( bi->_seen_count >= _nb_seen_max_ui )
							break;
					}	//end	j
				}
			}	//end	i
		}
	}

	INT32 tmp = _test_failed_ui + _test_sucess_ui;
	if( tmp!=0 )
		_test_score_ui = REAL(100. * REAL(_test_sucess_ui) / tmp);
	else
		_test_score_ui = REAL(0);
}

template< INT32 DIM, c_boids::INTERACTION_METHOD METHOD >
void	c_boids::update_interaction_self() NOEXCEPT
{
	switch( _s_lock_method )
	{
	case LOCK_METHOD_NO:			update_interaction_self< DIM, METHOD, LOCK_METHOD_NO		>();	break;
	case LOCK_METHOD_BY_BOID:		update_interaction_self< DIM, METHOD, LOCK_METHOD_BY_BOID	>();	break;
	case LOCK_METHOD_BY_CONTACT:	update_interaction_self< DIM, METHOD, LOCK_METHOD_BY_CONTACT>();	break;
	default:		break;
	}
}

//interaction unused skip this call (change next fn too)
//template< INT32 DIM >
//void	c_boids::update_interaction_self() NOEXCEPT
//{
//	switch( _s_interaction_method )
//	{
//	case INTERACTION_ADD:		update_interaction_self< DIM, INTERACTION_ADD	>();	break;
//	case INTERACTION_SWITCH:	update_interaction_self< DIM, INTERACTION_SWITCH>();	break;
//	case INTERACTION_SNOOTH:	update_interaction_self< DIM, INTERACTION_SNOOTH>();	break;
//	default:		break;
//	}
//}

void	c_boids::update_interaction_self() NOEXCEPT
{
	SPY_PUSH_RANGE( _b_parallel ? "// Self inter" : "Self inter", spy::COL_3 );
		switch( _s_dim_ui )
		{
		case 1:		update_interaction_self< 1, INTERACTION_ADD >();	break;
		case 2:		update_interaction_self< 2, INTERACTION_ADD >();	break;
		case 3:		update_interaction_self< 3, INTERACTION_ADD >();	break;
		default:	break;
		}
	SPY_POP_RANGE();
}


FINLINE void	c_boids::update_interaction_helper() NOEXCEPT
{
	_repulse_dist_over_one = OVER_ONE_AS_REAL( _repulse_dist );
	//todo power here is strange should be done on the distance not the time ?
	_repulse_factor *= REAL( POW_D( _over_dt, DOUBLE(_repulse_pow_ui) ) );

	_interact_dist = _interact_dist_edge + _interact_radius_to_substract;
	_interact_dist_squared = _interact_dist * _interact_dist;
	_cluster_interact_nb = I_CEIL(_interact_dist * _cluster_by_unit_ui);
	_repulse_force_separation_factor *= _interact_dist * REAL(2);
}

void	c_boids::update_interaction() NOEXCEPT
{
	//	regroup dist constraint
	_repulse_dist					= _repulse_dist_self;
	_repulse_dist_squared			= _repulse_dist * _repulse_dist;

	_flock_dist_squared				= _flock_dist * _flock_dist ;
	_flock_dist_squared_interior	= _flock_dist_squared * _flock_dist_interior_ui * _flock_dist_interior_ui;

	_steer_dist_squared				= _steer_dist * _steer_dist ;

	//	regroup visibility constraint
	_b_vis = _b_visibility_ui 
		&& ( _b_repulse_visibility_ui || _b_flock_visibility_ui || _b_steer_visibility_ui )
		&& ( _visibility_angle_ui<1. );
	_b_vis_repul = _b_vis && _b_repulse_visibility_ui;
	_b_vis_flock = _b_vis && _b_flock_visibility_ui;
	_b_vis_steer = _b_vis && _b_steer_visibility_ui;

	//	initialize and interact with itself
	_repulse_factor = _b_repulse ? _repulse_influence_ui : REAL(0);

	_interact_dist_edge						= _visibility_dist;
	_interact_radius_to_substract			= _interact_internal_radius_ui * REAL(2);
	_interact_dist							= _interact_dist_edge + _interact_radius_to_substract;
	_repulse_force_separation_dist_squared	= _repulse_force_separation_dist_ui * _repulse_force_separation_dist_ui;
	_repulse_force_separation_factor		= _repulse_force_separation_factor_ui;

	boid_info_lua._bdd_a = _bdd_boid;
	boid_info_lua._bdd_b = _bdd_boid;

	if( _b_contact_do_lua_ui )
		_contact_do_lua_ref = g_lua_wrap_cur->get_table_fn_as_ref( "poid", "hook_do_contact" );
	else
		_contact_do_lua_ref = LUA_NOREF;

	if( _b_is_repulse_do_lua_ui )
		g_lua_wrap_cur->do_fn_pass_i( (c_obj_ui*)_bdd_boid, "poid.hook_set_universe", _universe_id_ui );
	if( !_b_is_repulse_do_lua_ui
		|| g_lua_wrap_cur->do_fn_r_pass_ii( (c_obj_ui*)_bdd_boid, "poid", "hook_is_repulse", get_id(), this->get_id(), 1. ) != 0.
		)
	{
		update_interaction_helper();
		update_interaction_self();
	}
	//	then interact with all the other boids in the same universe
	if( _s_repulse_by_other_type_ui != REPULSE_OFF )
	{
		//todo add visibility here too
		//	_repulse_by_other_influence
		//	_repulse_by_other_distance;

		c_boid_universe* universe = c_boid_universe::get_cur();
		if( universe )
		{
			for( auto const & p : *(universe->_boids) )
			{
				if( p != this )
				{
					boid_info_lua._bdd_b = p->_bdd_boid;
					//if( _s_repulse_by_other_type == REPULSE_OFF )
					//	continue;
					if( _b_is_repulse_do_lua_ui
						&& g_lua_wrap_cur->do_fn_r_pass_ii( (c_obj_ui*)_bdd_boid, "poid", "hook_is_repulse", get_id(), p->get_id(), 1. ) == 0.
						)
					{
						continue;
					}
					switch( _s_repulse_by_other_type_ui )
					{
					case REPULSE_SELF:		_repulse_dist	= _repulse_dist_self;
											_repulse_factor	= _repulse_influence_ui;
											break;
					case REPULSE_EXPLICIT:	_repulse_dist	= _repulse_by_other_distance_ui;
											_repulse_factor = _repulse_by_other_influence_ui;
											break;
					case REPULSE_OTHER:		_repulse_dist	= p->_repulse_by_other_distance_ui;
											_repulse_factor = p->_repulse_by_other_influence_ui;
											break;
					}
					_repulse_dist_squared = _repulse_dist * _repulse_dist;

					_interact_dist_edge = _repulse_dist;
					_interact_radius_to_substract = _interact_internal_radius_ui + p->_interact_internal_radius_ui;
				
					_repulse_force_separation_dist_squared = MAX( _repulse_force_separation_dist_ui, p->_repulse_force_separation_dist_ui );
					_repulse_force_separation_dist_squared *= _repulse_force_separation_dist_squared;
					_repulse_force_separation_factor = MAX( _repulse_force_separation_factor_ui, p->_repulse_force_separation_factor_ui );

					update_interaction_helper();
					update_interaction_other( p );
				}
			}
		}
		else
			debug_break( "Boids universe empty, should not can happen even at start of a boid stuff" );
	}
	g_lua_wrap_cur->get_lua_state()->unref( _contact_do_lua_ref );
}

//	the kill is done on the primary structure
//		so the fast struct is still ok til the next frame
//		this way it can be call anytime after the first pass of update
FINLINE	void c_boids::kill_one_low( c_boid* CONST b )
{
	boid_info_lua._bdd = _bdd_boid;
	boid_info_lua._boid = b;
	//todoopt pre cache the fn information 
	g_lua_wrap_cur->do_fn_pass_ii( (c_obj_ui*)_bdd_boid, "poid", "hook_do_kill", get_id(), b->get_id() );

	if( _bdd_point_out )
		_bdd_point_out->delete_dot( b->get_id() );
	//todo	if( _b_net_send )
	//	net_send_kill( _poids.get_index(pt) );
	//if( pt->is_active() )
	//	--_nb_active;
	b->set_dead();
}

//	we can change _unused and _living
//		bacause we call this only when we rely on _hd_calc
void c_boids::kill_one_and_remove( c_boid* CONST b )
{
	kill_one_low( b );
	_unused.push_back( b );
	_living.remove( b );
	//TODO check here
}


void c_boids::kill_all( )
{
	for( auto const & p_boid : _living )
		kill_one_low( p_boid );

	_unused.splice( _unused.end(), _living );
	_nb_alive = 0;
}

void	c_boids::kill_next_update( c_boid* CONST b )		//todo refine needed for kill done thru lua from his interaction or another
{	
	if( !b )
	{
		ERR_PRINT_STRING( "%s() can't kill null boid", __FUNCTION__ );
		return;
	}
	if( _to_kill.insert(b).second == false && _b_verbose_multiple_kill_ui )
	{
		ERR_PRINT_STRING( "%s() try to kill a boid %d/%d already on the kill list", __FUNCTION__, b->get_id()>>24, b->get_id()&0xffffff );
	}
}

//todo refine needed for kill done thru lua
bool	c_boids::kill_by_id( INT32 id )
{
	if( _s_id_generator_ui == c_poid::ID_GENERATOR_FROM_INDEX )
	{
		INT32	nb = _poids->get_nb();
		id &= 0x00ffffff;
		if( id<1 || nb<id )
		{
			ERR_PRINT_STRING( "%s() try to kill boid with id %d when there is only %d boids with ID_FROM_INDEX", __FUNCTION__, id, nb );
			return false;	
		}
		kill_next_update( get_boid_from_id( id ) );
		return true;
	}
	ERR_PRINT_STRING( "%s() unimplemented for id_type %d", __FUNCTION__,_s_id_generator_ui );
	return false;
}

FINLINE	c_boid* c_boids::create_one()	// c_particle* pa, REAL birth_time, bool b_ressucitate )
{
	if( _unused.empty() )
		return nullptr;

	c_boid* b = _unused.front();
	_unused.pop_front();

	REAL	pos[3];
	REAL	speed[3];
	REAL*	size;

	bool	b_force;	//hack
	if( _to_create.empty() )
	{
		cpy_v3( pos, _birth_origin_ui );
		size =_birth_origin_size;
		b_force = false;
	}
	else
	{
		cpy_v3( pos, _to_create.front()._b_pos ? _to_create.front()._pos : _birth_origin_ui );
		cpy_v3( speed, _to_create.front()._speed );
		size = nullptr;
		_to_create.pop_front();
		b_force = true;
	}

	INT32 id;
	switch( _s_id_generator_ui )
	{
	case c_poid::ID_GENERATOR_RAND:			id = (rand_create_and_place.get_uint32() >> 12) + 1;	break;	// +1 is for lua
	case c_poid::ID_GENERATOR_UNIQUE:		id = ++_id_generator;									break;	// preincrement to start at 1 for Lua
	case c_poid::ID_GENERATOR_FROM_INDEX:	
	default:								id = _poids->get_index(b)+1;							break;	// +1 is for lua
	}

	//hack for smart tv. this too brutal mix the 2
	if( b_force )
	{
		cpy_v3( b->get_pos(), pos );
		b->set_pos_to_draw( pos );
		cpy_v3( b->get_speed(), speed );
		b->cpy_to_last();
	}
	else
	{
		REAL vec[4];
		_placer.place_one( id, vec );
		switch( _s_dim_ui )
		{
			case 1:		b->place_at_uvw<1>( pos, size, vec, _i_axe );		break;
			case 2:		b->place_at_uvw<2>( pos, size, vec, _i_u, _i_v );	break;
			case 3:		b->place_at_uvw<3>( pos, size, vec );				break;
			default:	break;
		}
	}
	b->set_alive( (id & 0xffffff) | ( _id_ui << 24 ) );	// we put also _id
	b->_isolation_time = 0;

	_living.push_back( b );


	//todo ?	if it just died
	//	pa->set_life( birth_time, tmp );
	//if( !b_ressucitate )
	//{
	//	++_nb_active;
	//}
	boid_info_lua._bdd = _bdd_boid;
	boid_info_lua._boid = b;
	g_lua_wrap_cur->do_fn_pass_ii( (c_obj_ui*)_bdd_boid, "poid", "hook_do_create", get_id(), b->get_id() );

	if( _bdd_point_out )
		_bdd_point_out->new_dot( b->get_id() );

	return b;
}

void	c_boids::create_at( REAL CONST * CONST pos, REAL CONST * CONST speed )
{
	c_poid_to_create	c;
	if( pos )
	{
		c._b_pos = true;
		cpy_v3( c._pos, pos );
	}
	else
	{
		c._b_pos = false;
		clear_v3( c._pos );
	}
	if( speed )
		cpy_v3( c._speed, speed );
	else
		clear_v3( c._speed );
	_to_create.push_back( c );
}

void	c_boids::clean_dup( CHAR CONST * CONST str )
{
	SPY_PUSH_RANGE( "clean_dup", spy::COL_5 );
		INT32	before;
		INT32	after;

		before = (INT32)_living.size();
		_living.sort();
		_living.unique();
		after = (INT32)_living.size();
		if( before != after )
			ERR_PRINT_STRING( "Boids %d got rid of living %d duplicates. this should not happen %s", _id_ui, before-after, str );

		before = (INT32)_unused.size();
		_unused.sort();
		_unused.unique();
		after = (INT32)_unused.size();
		if( before != after )
			ERR_PRINT_STRING( "Boids %d got rid of unused %d duplicates. this should not happen %s", _id_ui, before-after, str );
	SPY_POP_RANGE();
}


FINLINE	INT32	c_boids::make_cluster( REAL CONST pos )
{
	return I_FLOOR( pos * _cluster_by_unit_ui );
}
//	CONST	REAL CLUSTER_BY_UNIT = 100000.;	//now dynamic

bool c_boids::cluster_less( c_boid CONST * CONST a, c_boid CONST * CONST b ) NOEXCEPT //	{	return a->less( b );	}
{
	INT32 CONST	delta = a->_ga - b->_ga;
	return delta != 0 ? (delta < 0) : (a->_gb < b->_gb) ;
}

void	c_boids::build_array_sorted( REAL dt, bool b_parallel )
{
//___________________________________________________________________________________________________________________________________________
//
//	BUILD POINTER* ARRAY
//		SORTING and PREPARING 
//___________________________________________________________________________________________________________________________________________
	//now we sort then transfer to a simplest data structure possible (array of pointer)
	//	because the interaction ( n squared algo ) require the best parsing speed possible
	//	be careful this structure need to be left intact for other boid in the same universe 
	if( _visibility_dist != 0. )	//todo deal with repulsion from other too
	{
// compute cluster info and adding to _sorter when used
		SPY_PUSH_RANGE( "Cluster", spy::COL_2 );

			if( b_parallel )
			{
				for_each( std::execution::par_unseq, _living.begin(), _living.end(), [&](c_boid* b)
				{
#if	AAA_DEBUG()
					if( _isnan(b->get_pos()[0]) )
						clear_v3( b->get_pos() );
#endif
					b->compute_pos_futur( dt );		//	compute pos_futur eventually useful for speed field
					b->clear_interaction();
					switch( _s_dim_ui )
					{
					case 1:
						b->_ga = make_cluster( b->get_pos()[_i_axe] );
						break;
					case 2:
					case 3:
						b->_ga = make_cluster( b->get_pos()[_i_u] );
						b->_gb = make_cluster( b->get_pos()[_i_v] );
						break;
					}
				}
				);
			}
			else
			{
				for( auto const & b : _living )
				{
#if	AAA_DEBUG()
					if( _isnan(b->get_pos()[0]) )
						clear_v3( b->get_pos() );
#endif
					b->compute_pos_futur( dt );		//	compute pos_futur eventually useful for speed field
					b->clear_interaction();
					switch( _s_dim_ui )
					{
					case 1:
						b->_ga = make_cluster( b->get_pos()[_i_axe] );
						break;
					case 2:
					case 3:
						b->_ga = make_cluster( b->get_pos()[_i_u] );
						b->_gb = make_cluster( b->get_pos()[_i_v] );
						break;
					}
				}
			}
		SPY_POP_RANGE();

//	assign to sorter
		SPY_PUSH_RANGE( "Assign", spy::COL_4 );
#if 0
			//this crash ?
			_sorter.clear();
			_sorter.reserve( _living.size() );
			std::copy( _living.begin(), _living.end(), std::back_inserter(_sorter) );
#else
			_sorter.assign( _living.begin(), _living.end() );
#endif
		SPY_POP_RANGE();

		SPY_PUSH_RANGE( "Sort", spy::COL_6 );
			//	oh yes do it
			//_sorter.sort( cluster_less );	//this was for list
			//sort( _sorter.begin(), _sorter.end(), cluster_less );
			if( b_parallel )
#if AAA_VSTOOL() >= 141
				std::sort( std::execution::par_unseq, _sorter.begin(), _sorter.end(), cluster_less );
#else
				PARALLEL_LIB::parallel_sort( _sorter.begin(), _sorter.end(), cluster_less );
#endif
			else
				sort( _sorter.begin(), _sorter.end(), cluster_less );
		SPY_POP_RANGE();

		//	then we send it back to the simple structure
		c_boid**	hd = _hd_calc-1;
		for( auto const & p_boid : _sorter )
			*++hd = p_boid;
	}
	else			 
	{
		c_boid**	hd = _hd_calc-1;
		for( auto const & b : _living )
		{
			b->compute_pos_futur( dt );	//	compute pos_futur eventually useful for speed field
			b->clear_interaction_minimum();
			*++hd = b;
		}
	}
//	FROM HERE THE ACCES IS READY AND FAST USING _hd_calc
}

void	c_boids::update_movement( INT32 nb ) NOEXCEPT
{
	if( _b_parallel )
		PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT	{ (*(_hd_calc+i))->update_movement(i); }	);
	else
		for( INT32 i=0; i<nb; ++i )											{ (*(_hd_calc+i))->update_movement(i); }
}

void	c_boids::update( REAL CONST dt, bool CONST b_need_contact )
{
	c_boid_universe::set_cur( _universe_id_ui );
	c_boid_universe::add_boid( this );
	c_model::cur->get_axes( _i_u, _i_v, _i_axe );
	_placer.update();	//todo update only when needed

	if( !_b_move_ui )
		return;
	if( dt <= 0. )
	{
		//	DBG_PRINT_STRING( "c_boids::update dt <= 0.");
		return;
	}

	SPY_PUSH_RANGE( "Boids update 1", spy::COL_1 );

	//	we had a bug
	if( _s_check_duplicate_ui > 0 )
		clean_dup( "begin update" );

//___________________________________________________________________________________________________________________________________________
//
//	TIME
//___________________________________________________________________________________________________________________________________________
	_dt = dt;	
	_over_dt = OVER_ONE( dt );
	_over_dt_squared = _over_dt * _over_dt;
#if	AAA_DEBUG()
	//if( _isnan(_over_dt) )
	if( _finite(_over_dt)==0 )
		debug_break();
#endif

	if( _b_curvature_constraint_ui )
	{
		// this is the angle for this dt
		DOUBLE angle = _curvature_turn_by_sec_ui * PI_TIME_2 * dt;	
//		GET_SIN_COS_TURN( _curvature_sin_threshold, _curvature_cos_threshold, angle );
		_curvature_cos_threshold = cos(angle);
		_curvature_sin_threshold = sin(angle);
	}

//___________________________________________________________________________________________________________________________________________
//
//	CHECK CREATION NEEDED AND NB
//___________________________________________________________________________________________________________________________________________
	INT32	nb_to_create;

	//	how many to create
	_birth_nb_ui	+=	_birth_rate_ui * dt;
	_birth_nb_ui	=	MAX0( _birth_nb_ui );
	nb_to_create	=	MAX0( I_FLOOR( _birth_nb_ui ) );
	if( !_to_create.empty() )
		nb_to_create += (INT32)_to_create.size();

	INT32	nb = (INT32)_living.size();	//	this nb include the one who want to die
	//	if we don't reach the minimum
	INT32	nb_min_asked = MIN( _nb_alive_min_ui, _nb_alive_max_ui );
	INT32	nb_max_asked = _nb_alive_max_ui;

	if( nb + nb_to_create < nb_min_asked )
	{	//	we have to add more
		nb_to_create = MAX0( nb_min_asked - nb );		
		_birth_nb_ui = MAX0( _birth_nb_ui );
	}
	SPY_POP_RANGE();
	//	any boids to deal with ?
	if( (nb + nb_to_create) == 0 )
		return;

	SPY_PUSH_RANGE( "Boids update 2", spy::COL_2 );

//___________________________________________________________________________________________________________________________________________
//
//	PREPARE CONTEXT
//___________________________________________________________________________________________________________________________________________
	//	yes prepare all the stuff
	if( _b_death_box_ui )
		_death_box.update();

	_b_need_contact = b_need_contact;

	_b_speed_noise = _speed_noise_factor_ui != 0. ;
	_speed_noise_factor = _speed_noise_factor_ui;


	//	target
	_b_target = _b_target_ui && ( _inf_target._inf != 0. );
	if( _b_target )
		_inf_target.update();

	//	repulse
	REAL repulse_dist = _repulse_dist_ui * c_boids::master_dist_scale_ui * c_boids::master_dist_repulse_scale_ui;
	_b_repulse = c_boids::b_master_repulse_allow_ui &&_b_repulse_ui && ( _repulse_influence_ui != 0. ) && ( repulse_dist > 0. );
	_repulse_dist_self	= _b_repulse ? repulse_dist : REAL(0);

	_def_node_repulse = c_def_node::get_from_field_type( _s_repulse_field_ui );
	_b_repulse_field = (_repulse_dist_self > 0.) && _b_repulse_field_ui && _def_node_repulse;
	//todoq deal later also with repulse other
	if( _b_repulse_field )
	{
		// field repulse scale regular repulse
		REAL zero, one;
		if( _b_repulse_field_inverse_ui )
		{
			zero = _repulse_field_one_ui;
			one =  _repulse_field_zero_ui;
		}
		else
		{
			zero = _repulse_field_zero_ui;
			one =  _repulse_field_one_ui;
		}
		_repulse_field_min		= _repulse_dist_self * zero;
		_repulse_field_factor	= _repulse_dist_self * one - _repulse_field_min;
	}
	
	//	flock
	_flock_dist	= _flock_dist_ui * c_boids::master_dist_scale_ui * c_boids::master_dist_flock_scale_ui;
	_b_flock	= c_boids::b_master_flock_allow_ui &&_b_flock_ui && ( _flock_influence_ui != 0. ) && ( _flock_dist > 0. );
	if( !_b_flock )
		_flock_dist	= 0.;

	//	steer
	_steer_dist	= _steer_dist_ui * c_boids::master_dist_scale_ui * c_boids::master_dist_steer_scale_ui;
	_b_steer	= c_boids::b_master_steer_allow_ui && _b_steer_ui && ( _steer_influence_ui != 0. ) && ( _steer_dist > 0. );
	if( !_b_steer )
		_steer_dist	= 0.;

	_visibility_dist = aaa::MAX( _repulse_dist_self, _flock_dist, _steer_dist );

	_inf_death.update();

	scale_v3( _birth_origin_size, _birth_origin_size_ui, _birth_origin_size_ui[3] );

	// LIVING BOX
	auto& box = _box_living;
	if( box.b_active_ui )
	{
//		bool b_same;
		bool b_box;
		if( box.s_type_all_ui == c_poid::LIVING_BOX_NO )
		{
			box.s_type[0] = box.s_type_ui[0];
			box.s_type[1] = box.s_type_ui[1];
			box.s_type[2] = box.s_type_ui[2];
//			b_same = (box.s_type[0] == box.s_type[1]) && (box.s_type[0] == box.s_type[2]);
//			if( b_same )
//				b_box = box.s_type[0] != c_poid::LIVING_BOX_NO;
//			else
				b_box =		box.s_type[0] != c_poid::LIVING_BOX_NO
						||	box.s_type[1] != c_poid::LIVING_BOX_NO
						||	box.s_type[2] != c_poid::LIVING_BOX_NO;
		}
		else
		{
			auto val = box.s_type_all_ui;
			box.s_type[0] = val;
			box.s_type[1] = val;
			box.s_type[2] = val;
//			b_same = true;
			b_box = val != c_poid::LIVING_BOX_NO;
		}

		if(	b_box )	
		{	//	there is a Box
			box.border_size_over_one = OVER_ONE_AS_REAL( box.border_size_ui );
			scale_v3( box.size, box.size_ui, box.size_ui[3] );
			scale_v3( box.size_half, box.size, .5 );
			//hack check reverse box (size is neg )
			sub_v3( box.min, box.pos_ui, box.size_half );
			add_v3( box.max, box.pos_ui, box.size_half );
			if( box.b_sphere_ui )	//hack not done with center yet do it
			{
				box.radius = aaa::MAX( box.size_ui[0], box.size_ui[1], box.size_ui[2] ) * box.size_ui[3] * REAL(.5);
				box.radius_squared = box.radius * box.radius;
			}

			if(		box.s_type[0] == c_poid::LIVING_BOX_REPULSE
				||	box.s_type[1] == c_poid::LIVING_BOX_REPULSE
				||	box.s_type[2] == c_poid::LIVING_BOX_REPULSE )
			{
				REAL over_dt_box = POW_R( REAL(_over_dt), _box_living.exponent_ui );
#if	AAA_DEBUG()
				//if( _isnan(over_dt_box) )
				if( _finite(over_dt_box)==0 )
					debug_break();
#endif
				box.influence_repulse = box.influence_ui * over_dt_box;
			}
			else
				box.influence_repulse = 0.;

		}
	}
	else
	{
		box.s_type[0] = c_poid::LIVING_BOX_NO;
		box.s_type[1] = c_poid::LIVING_BOX_NO;
		box.s_type[2] = c_poid::LIVING_BOX_NO;
		box.influence_repulse = 0.;
	}
	_visibility_cos = COS_TURN( _visibility_angle_ui/2. );

	_bdd_point_out = _b_feed_bdd_point_ui ? bdd_point_cur : nullptr ;

	c_meca_world::update();

//___________________________________________________________________________________________________________________________________________
//
//	DEATH
//___________________________________________________________________________________________________________________________________________
	//	death from interactions from others
	//todo opt this to avoid a parsing
	if( !_to_kill.empty() )
	{
		if( _s_check_duplicate_ui > 1 )
			clean_dup( "before kill" );
		SPY_PUSH_RANGE( "to_kill", spy::COL_2 );
			for( auto const & p_boid : _to_kill )
				kill_one_and_remove( p_boid );
			_to_kill.clear();
		SPY_POP_RANGE();
		if( _s_check_duplicate_ui > 1 )	
			clean_dup( "after kill" );
	}


	//	how many to kill
	//	this imperfect we should know already how many want to die
	_death_nb_ui += _death_rate_ui * dt;
	INT32	dying_needed = MIN( I_FLOOR( _death_nb_ui ), nb );
	nb = (INT32)_living.size();
	//we want dont pass the maximum
	INT32	futur_nb;

	futur_nb = nb + nb_to_create - dying_needed;
	if( futur_nb > nb_max_asked )
		dying_needed += futur_nb - nb_max_asked;
	//dying_needed = MAX0( dying_needed );

	futur_nb = nb + nb_to_create - dying_needed;
	if( futur_nb < nb_min_asked )
		dying_needed -= nb_min_asked - futur_nb;
	dying_needed = MAX0( dying_needed );

	//now we deal with life and death
	//	the one try to die will be at the beginning of the _living structure
	INT32	sent_to_die = 0;
	//	death	
	if( dying_needed )
	{
		SPY_PUSH_RANGE( "dying_needed", spy::COL_4 );
			for( auto const & b : _living )
			{
				if( b->is_want_to_die() )
				{
					if( --dying_needed <= 0 )	break;
				}
				else
				{
					b->set_want_to_die();
					++sent_to_die;
					if( --dying_needed <= 0 )	break;
				}
			}
		SPY_POP_RANGE();
		if( _s_check_duplicate_ui > 1 )
			clean_dup( "after dying_needed" );
	}

	_death_nb_ui -= sent_to_die;
	_death_nb_ui = MIN( _death_nb_ui, REAL(nb) );
	_death_nb_ui = MAX0( _death_nb_ui );

//___________________________________________________________________________________________________________________________________________
//
//	BIRTH
//___________________________________________________________________________________________________________________________________________
	if( nb_to_create > 0 )
	{
		SPY_PUSH_RANGE( "Create", spy::COL_6 );
			do
			{
				if( !create_one() )
					break;
				_birth_nb_ui = MAX0( --_birth_nb_ui );
			}
			while( --nb_to_create );
		SPY_POP_RANGE();
		if( _s_check_duplicate_ui > 1 )
			clean_dup( "after create" );
	}

	_nb_alive = (INT32)_living.size();

	if( !_to_create.empty() )
	{
		_bdd_boid->err_print( "too many boid to create. skipping %d boids.", _to_create.size() );
		_to_create.clear();
	}

//___________________________________________________________________________________________________________________________________________
//
//	BUILD POINTER* ARRAY
//		SORTING and PREPARING 
//___________________________________________________________________________________________________________________________________________
	_b_parallel = b_master_parallel_allow_ui
					&& _b_parallel_ask_ui
					&& aaa::parallel::is_use()
					&& (b_master_parallel_min_nb_force_ui ? master_parallel_min_nb_alive_ui : _parallel_min_nb_alive) <= _nb_alive;
	if( _b_parallel )
		_s_lock_method = _s_lock_method_ui==LOCK_METHOD_MASTER ? s_master_parallel_lock_method_ui : _s_lock_method_ui;
	else
		_s_lock_method = LOCK_METHOD_NO;
	//now we sort then transfer to a simplest data structure possible (array of pointer)
	//	because the interaction ( n squared algo ) require the best parsing speed possible
	//	be careful this structure need to be left intact for other boid in the same universe 
	build_array_sorted( dt, _b_parallel );

//___________________________________________________________________________________________________________________________________________
//
//	PREPARE FIELDs : force field and repulse field
//___________________________________________________________________________________________________________________________________________
	//	field
	//	b_field = c_def_node::get_cur()->is_deforming() && (((_b_field_speed_ui||b_field_acc_ui) && b_move) || b_field_size ); //
	if( _nb_alive > 0 )
	{
		auto def = c_def_node::get_cur();
		if( def->is_deforming() && _b_field_speed_ui && _field_influence_ui!=0. )
			_field_influence = _field_influence_ui * REAL(_over_dt);
		else
			_field_influence = 0.;

		if( _field_influence != 0. || _b_repulse_field )
		{
			// prepare the source of the field 
			// get position of boids;
			c_meca_obj::get_pos( _field_in, _nb_alive, reinterpret_cast<c_meca_obj**>(_hd_calc), _b_field_predicted_position_ui, _b_parallel );

			if( _field_influence != 0. )
			{
				// get field at this position
				//todoq	we should have a memory zone for this tmp things
				def->get_field( _field_out, _field_in, _nb_alive );		
			}

			if( _b_repulse_field )
			{
				// get field at this position
				//todoq	we should have a memory zone for this tmp things
				def->get_field( _field_repulse_out, _field_in, _nb_alive );
				SPY_PUSH_RANGE( "Repulse Field get", spy::COL_4 );
					c_boid**	hd_i_max = _hd_calc + _nb_alive;
					REAL* pt = _field_repulse_out + _repulse_field_src_axe_ui;
					for( c_boid** hd_i = _hd_calc; hd_i < hd_i_max; ++hd_i, pt+=3 )
					{
						//c_boid*	bi = *hd_i;
						(*hd_i)->_dist_repulse = _repulse_field_min + _repulse_field_factor * REAL(POW( CLAMP_01( *pt ), _repulse_field_power_ui ) );
						//bi->_dist_repulse_squared = bi->_dist_repulse * bi->_dist_repulse; 
					}	//end	hd_i
				SPY_POP_RANGE();
			}
		}
	}
	else
		_field_influence = 0.;

//___________________________________________________________________________________________________________________________________________
//
//	UPDATE INTERACTION
//		JUST ACCUMULATE THE INFOS
//		this is the Bulk of processing time
//___________________________________________________________________________________________________________________________________________
	if( _visibility_dist != 0. )	//todo deal with repulsion too
	{
		_contacts_index = -1;
		update_interaction();
		_contacts_nb = _contacts_index + 1;
	}
	else
		_contacts_nb = 0;
	
//___________________________________________________________________________________________________________________________________________
//
//	UPDATE MOVEMENT
//___________________________________________________________________________________________________________________________________________
	SPY_PUSH_RANGE( "Move", spy::COL_6 );
		_target					= (_b_target && (_inf_target._inf!=0.)) ? &_inf_target : nullptr;
		update_movement(nb);
	SPY_POP_RANGE();

	SPY_PUSH_RANGE( "End", spy::COL_2 );
		clear_v3( _center_ui );
		INT32	count = 0;
		c_boid** hd		= _hd_calc - 1;
		c_boid** hd_dst = _hd_draw - 1;
		for( INT32 i=_nb_alive; i>0; --i )
		{
			c_boid*	b = *++hd;
			if( b->is_to_kill() )
			{
				kill_one_and_remove( b );
				if( _s_check_duplicate_ui > 1 )		{	clean_dup( "after kill_one in loop" );	}
			}
			else
			{
				if( !b->is_want_to_die() )
				{
					++count;
					add_v3( _center_ui, b->get_pos() );
					if( b->is_draw() )
					{
						*++hd_dst = b;
					}
				}
				
				//if( _b_sort_out_by_id )
				//	cpy_v3( dst+3*(b->get_id()%_nb_alive), b->get_pos() );
				//else
				//{
				//	*++dst = b->_pos[0];
				//	*++dst = b->_pos[1];
				//	*++dst = b->_pos[2];
				//}
				
				if( _bdd_point_out )
					_bdd_point_out->set_dot_pos( b->get_id(), b->get_pos(), 0 );

			}
		}
		_nb_draw = INT32( hd_dst - _hd_draw + 1 );
	SPY_POP_RANGE();

	if( count )
		scale_v3( _center_ui, 1./REAL(count) );

	SPY_POP_RANGE();
}

c_boid*	c_boids::get_boid_data_direct( INT32 CONST index ) CONST
{
	return	_poids->get_pt_from_index(index);
}
