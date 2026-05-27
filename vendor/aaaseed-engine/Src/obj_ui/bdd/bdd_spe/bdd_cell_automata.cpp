#include "bdd_cell_automata.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/model.h"
#include "data_2d.h"


FACTORY_CREATE_PROP_V1( c_bdd_cell_automata, bdd_cell_automata, 3D Cell Automata, bdd_cell_automata, sub_menu="Experimental"; );

namespace n_bdd_cell_automata
{
	CONSTEXPR INT32 BASE_NB_MAX		=	c_bdd::GEO_PARAM_NB + 15;
	CONSTEXPR INT32 GROUP_NB_MAX	=	0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_POINT_XYZ(	center	)

		PARAM_DEF_INT32_POS(	nb_u,		2, 16	)
		PARAM_DEF_INT32_POS(	nb_v,		2, 16	)
		PARAM_DEF_INT32_POS(	nb_axe,		2, 16	)

//		{	nullptr,	PARAM_SYMBOLIC,	"generation_per_second",	0, 1,		0, 9,	nullptr, nullptr },
		PARAM_DEF_BOOL_OFF(		start_trig	)
		PARAM_DEF_BOOL_OFF(		stop_trig	)
		PARAM_DEF_BOOL_OFF(		play		)
		PARAM_DEF_BOOL_OFF(		fill_random_trig	)
		PARAM_DEF_BOOL_OFF(		shift_trig	)
		PARAM_DEF_BOOL_OFF(		gene_trig	)

		PARAM_DEF_REAL_ONE(		shift_freq	)
		PARAM_DEF_REAL_ZERO(	phase_offset	)
		PARAM_DEF_REAL_LOCKED(	phase_out	)

//		{	nullptr,	PARAM_INT32,	"birth_min",	0, 1,		0, 8,							nullptr, nullptr },
//		{	nullptr,	PARAM_INT32,	"birth_mwax",	0, 1,		0, 8,							nullptr, nullptr },
//		{	nullptr,	PARAM_INT32,	"death_min",	0, 1,		0, 8,							nullptr, nullptr },
//		{	nullptr,	PARAM_INT32,	"death_max",	0, 1,		0, 8,							nullptr, nullptr },			
	};
}

void	c_bdd_cell_automata::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _center_ui );

	param_set_pt( h, _nb_u_ui );
	param_set_pt( h, _nb_v_ui );
	param_set_pt( h, _nb_axe_ui );

//	param_set_pt( h, generation_per_sec);
	param_set_pt( h, _b_start_trig );
	param_set_pt( h, _b_stop_trig );
	param_set_pt( h, _b_play );
	param_set_pt( h, _b_fill_random_trig );
	param_set_pt( h, _b_shift_trig_ui );
	param_set_pt( h, _b_gene_trig );

	param_set_pt( h, _shift_freq );
	param_set_pt( h, _phase_offset );
	param_set_pt( h, _phase_out );

//	param_set_pt( h, _birth_min );
//	param_set_pt( h, _birth_max );
//	param_set_pt( h, _death_min );
//	param_set_pt( h, _death_max );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_cell_automata )
{
//	nb_u = 0;
//	nb_v = 0;
	_nb_axe_ui = 0;
	_phase = 0.;
	param_init_with( n_bdd_cell_automata::param, n_bdd_cell_automata::PARAM_NB_MAX );
}

c_bdd_cell_automata::~c_bdd_cell_automata()
{
	dealloc();
}

void	c_bdd_cell_automata::alloc()
{
	if( _nb_axe_allocated != _nb_axe_ui )
	{
		if( _nb_axe_allocated <= _nb_axe_ui )
		{
			for( INT32 i = _nb_axe_ui - _nb_axe_allocated; i > 0; --i )
			{
				c_data_2d<REAL>* pt;
				pt = new c_data_2d<REAL>( _nb_u_ui, _nb_v_ui );
				_datas.push_back( pt );
			}
		}
		_nb_axe_allocated = _nb_axe_ui;
	}

	if( _nb_u != _nb_u_ui || _nb_v != _nb_v_ui )
	{
		for( auto const & p : _datas )
		{
			p->alloc( _nb_u_ui, _nb_v_ui );
		}
//		nb_u = nb_u_ui;
//		nb_v = nb_v_ui;
	}

}

void	c_bdd_cell_automata::dealloc()
{
	// todofranz dealloc
	_datas.clear();
}

void c_bdd_cell_automata::shift()
{
	c_data_2d<REAL>*	data;
	data = _datas.back();
	_datas.pop_back();
	_datas.push_front( data );
}


void c_bdd_cell_automata::gene()
{
	//INT32		u, v, w;
	REAL				t;
	c_data_2d<REAL>*	data_2d;

	for( INT32 w = 0; w < _nb_axe_ui; ++w )
	{
		data_2d = _datas[w];
		for( INT32 u = 0; u < _nb_u_ui; ++u )
		{
//			c_tex_anim::cur->bind_this( u);
			for( INT32 v = 0; v < _nb_v_ui; ++v )
			{
//				t = REAL(u+v);		11
				t = SIN_TURN( u * .025 ) * SIN_TURN( v * .025 ) * SIN_TURN( REAL(w) * .06 ) - REAL(.7);
				t = MAX0( t );
				data_2d->set( u, v, t );
			}
//			multiple_index_v_next_hack();
		}
	}
}

static INT32	reseed = 0;
void c_bdd_cell_automata::update()
{
	alloc();
	if( _nb_u_ui * _nb_v_ui * _nb_axe_ui != 0 )
	{
		if( _b_fill_random_trig )
		{
			_datas[0]->fill_random( 1, 300, 435657 + reseed );
			reseed = INT32(reseed + 68768768235);
			_b_fill_random_trig = false;
		}
		if( _delta_t.update() )//|| b_restart_trig )
		{
			_phase = 0.;
//			b_restart_trig = false;
		}
		else
			_phase += REAL(_delta_t.get_dt()) * _shift_freq;
		_phase_out = _phase + _phase_offset;
		_data_index_offset = IMOD( I_FLOOR(_phase_out * _nb_axe_ui), _nb_axe_ui );
		if( _b_play )
			_b_shift_trig_ui = true;
		if( _b_shift_trig_ui )
		{
			shift();
			_b_shift_trig_ui = false;
		}
		if( _b_gene_trig )
		{
			gene();
			_b_gene_trig = false;
		}
		//if( _b_play )
		//	{
		//	}
	}

	update_uvw();
	if(	build_geo_is_needed() )
		build_grid( _center_ui[_i_axe] );
	if( _nb_axe_ui == 1 )
		do_deform_and_normal();
	if( c_multiple::cur )
		c_multiple::cur->set_nb( _nb_u_ui, _nb_v_ui, _nb_axe_ui );
}

void c_bdd_cell_automata::build_grid(REAL z)
{
	REAL*	p_point		= _point;
	REAL*	p_normal	= _normal;
	//INT32	iu, iv;
	REAL	u,v;
	REAL	du,dv;
	REAL	v_start;

	dv = .0;
	du = .0;
	u = _center_ui[_i_u];
	if( _nb_u_ui > 1 )
	{
		du = _size[0]/REAL( _nb_u_ui - 1 );
		if( _i_axe == 0 )
		{
			u -= _size[0];
			du = -du;
		}
		else
			u -= _size[0] * REAL(.5);
	}

	v_start = _center_ui[_i_v];
	if( _nb_v_ui > 1 )
	{
		dv = _size[1] / REAL( _nb_v_ui - 1 );
		v_start -= _size[1] * REAL(.5);
	}

	for( INT32 iu = _nb_u_ui; iu > 0; --iu )
	{
		v = v_start;
		for( INT32 iv = _nb_v_ui; iv > 0; --iv )
		{
			p_point[_i_u] = u;
			p_point[_i_v] = v;
			p_point[_i_axe] = z;
			p_point += 3;

			p_normal[_i_u] = 0.;
			p_normal[_i_v] = 0.;
			p_normal[_i_axe] = 1.;
			p_normal += 3;

			v += dv;
		}
		u += du;
	}

	build_geo_validate();
}

void c_bdd_cell_automata::build_geo()
{
	build_grid( _center_ui[_i_axe]);
}

void c_bdd_cell_automata::draw_multiple()
{
	if( _nb_u_ui * _nb_v_ui > 0 )	//todo move to drawable
		return;	

	REAL*	c = _point_to_draw;

	if( c_multiple::cur->is_align_normal() && _nb_u_ui > 1 && _nb_v_ui > 1 )
	{
		REAL*	nor = _normal_to_draw;
		REAL*	p = _point_to_draw;
		REAL*	n = _point_to_draw + _nb_v_ui * 3;
		REAL	vu[3];
		REAL	vv[3];

//		c_tex_anim::cur->bind_this( nb_u-1);

		sub_v3( vu, n, p);
		sub_v3( vv, c + 3, c);
		c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
		p += 3;	c += 3;	n += 3;	nor += 3;
		c_multiple::cur->next_index_u_hack();

		for( INT32 v = _nb_v_ui - 2; v > 0; --v )
		{
			sub_v3( vu, n, p );
			sub_then_scale_v3r( vv, c + 3, c - 3, .5 );
			c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
			p += 3; c += 3; n += 3;	nor += 3;
			c_multiple::cur->next_index_u_hack();
		}

		sub_v3( vu, n, p );
		sub_v3( vv, c, c-3);
		c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
		p = _point_to_draw;
		c += 3; n += 3; nor += 3;
		c_multiple::cur->next_index_u_hack();

		c_multiple::cur->next_index_v_hack();

		for( INT32 u = _nb_u_ui - 2; u > 0; --u )
		{
//			c_tex_anim::cur->bind_this( u);

			sub_then_scale_v3r( vu, n, p, .5);
			sub_v3( vv, c+3, c);
			c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
			p += 3; c += 3; n += 3; nor += 3;
			c_multiple::cur->next_index_u_hack();

			for( INT32 v = _nb_v_ui - 2; v > 0; --v )
			{
				sub_then_scale_v3r( vu, n, p, .5 );
				sub_then_scale_v3r( vv, c + 3, c - 3, .5 );
				c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
				p += 3; c += 3; n += 3; nor += 3;
				c_multiple::cur->next_index_u_hack();
			}

			sub_then_scale_v3r( vu, n, p, .5 );
			sub_v3( vv, c, c - 3 );
			c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
			p += 3; c += 3; n += 3; nor += 3;
			c_multiple::cur->next_index_u_hack();

			c_multiple::cur->next_index_v_hack();
		}

		n -= 3 * _nb_v_ui;
//		c_tex_anim::cur->bind_this( u);

		sub_v3( vu, n, p );
		sub_v3( vv, c + 3, c);
		c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
		p += 3; c += 3; n += 3; nor += 3;
		c_multiple::cur->next_index_u_hack();

		for( INT32 v = _nb_v_ui - 2; v > 0; --v )
		{
			sub_v3( vu, n, p );
			sub_then_scale_v3r( vv, c+3, c - 3, .5);
			c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
			p += 3; c += 3; n += 3; nor += 3;
			c_multiple::cur->next_index_u_hack();
		}

		sub_v3( vu, n, p );
		sub_v3( vv, c, c - 3);
		c_multiple::cur->draw_one_at_tgn( c, vu, vv, nor );
		c_multiple::cur->next_index_u_hack();

		c_multiple::cur->next_index_v_hack();
	}
	else
	{
		//todoopt should I prefetch like this always ?
		REAL CONST * CONST	size = c_multiple::cur->get_size();
		INT32				axe = c_multiple::cur->get_axe();
		REAL				value;
		REAL				l_size[3];
		c_data_2d<REAL>*	data_2d;

		data_2d = _datas[ IMOD( c_multiple::cur->get_index_w() + _data_index_offset, _nb_axe_ui ) ];
		for( INT32 u = 0; u < _nb_u_ui; ++u )
		{
//			c_tex_anim::cur->bind_this( u);
			for( INT32 v=0; v < _nb_v_ui; ++v )
			{
				value = *data_2d->get_data_pt( u, v );
				if( value != 0 )
				{
					scale_v3( l_size, size, value );
					c_multiple::cur->align_then_draw( c, l_size, axe );
				}
				c += 3;
				c_multiple::cur->next_index_u_hack();
			}
			c_multiple::cur->next_index_v_hack();
		}
	}
}

