#include "bdd_tree.h"
#include "obj_ui/deformer/def_node.h"
#include "math/rand.h"
#include "gol/gol.h"
#include "draw/line.h"
#include "draw/model.h"

//TOCHECK	make sure the rand strategy don't generate identical subtree
//TODO	level/depth as real
//TODO	buffer object for bdd_cone
//TODO	central branch
//TODO	orientation of branch
//TODO	regular trees
//TODO	interpolation from random to regular
//TODO	lua interface
//TODO	draw_leave

FACTORY_CREATE_PROP_V1( c_bdd_tree, bdd_tree, Tree, bdd_tree, sub_menu="AAATest"; );

static	c_rand_lin	rnd;

namespace	n_bdd_tree
{
	CONSTEXPR INT32 BASE_PARAM_NB	=	7 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 TREE_PARAM_NB	=	19;
	CONSTEXPR INT32 GROUP_NB		=	1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	TREE_PARAM_NB
									+	GROUP_NB;
			  	    
	CONSTEXPR INT32 LEVEL_MAX_NB = 4096;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ(		origin				)
		PARAM_DEF_INT32_LOCKED(		node_nb				)
		PARAM_DEF_INT32_LOCKED(		node_free_count		)

		PARAM_DEF_GROUP_CLOSED( tree, TREE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		tree_force_generate		)
			PARAM_DEF_BOOL_OFF(		tree_generate_trig		)
			PARAM_DEF_BOOL_OFF(		tree_trunk				)
			PARAM_DEF_POINT_XYZ(	tree_trunk_move			)
			PARAM_DEF_INT32(		tree_level,				1, 2,	0,LEVEL_MAX_NB )
			PARAM_DEF_INT32_POS(	tree_seed,				0, 42	)
			PARAM_DEF_INT32_POS(	tree_branch_nb,			0, 2	)
			PARAM_DEF_REAL_POS(		tree_branch_nb_extra,	1, 0	)
			PARAM_DEF_REAL_POS(		tree_level_factor,		1, .5	)
			PARAM_DEF_POINT_UVAF(	tree_branch_move		)
			PARAM_DEF_SCALE_UVAF(	tree_branch_size		)

//		PARAM_DEF_REAL_ZERO_ONE(	draw_interpolator	)
		PARAM_DEF_INT32(	draw_level_begin_skip,	1,0,	0,LEVEL_MAX_NB	)
		PARAM_DEF_INT32(	draw_level_end_skip,	1,0,	0,LEVEL_MAX_NB	)

	};
}

void	c_bdd_tree::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _origin );
	param_set_pt( h, _node_nb );
	param_set_pt( h, c_node::node_free_nb );

	++h;
		param_set_pt( h, _b_tree_generate_continuous_ui );
		param_set_pt( h, _b_tree_generate_trig_ui );
		param_set_pt( h, _b_trunk_ui );
		param_set_pt_3( h, _trunk_pos );
		param_set_pt( h, _tree_level );
		param_set_pt( h, _tree_seed );
		param_set_pt( h, _tree_branch_nb );
		param_set_pt( h, _tree_branch_nb_extra );
		param_set_pt( h, _tree_level_factor );
		param_set_pt_4( h, _tree_branch_move_ui );
		param_set_pt_4( h, _tree_branch_size_ui );

	//	param_set_pt( h, _draw_interpolator );
	param_set_pt( h, _draw_level_begin_skip_ui	);
	param_set_pt( h, _draw_level_end_skip_ui	);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_tree )
//,_dataset_id(-1)
,_node_root(nullptr)
,_node_nb(0)
,_size_by_level_nb_allocated(0)
,_size_by_level(nullptr)
{
	param_init_with( n_bdd_tree::param, n_bdd_tree::PARAM_NB_MAX );
}

c_bdd_tree::~c_bdd_tree()
{
//	if( cur == this )
//		cur = nullptr;
	dealloc_by_level();
}

void	c_bdd_tree::dealloc_by_level()
{
	FREE_AND_NULL( _size_by_level );
	_size_by_level_nb_allocated = 0;
}

void	c_bdd_tree::alloc_by_level()
{
	if( _tree_level <= _size_by_level_nb_allocated )
		return;
	_size_by_level = (REAL*) REALLOC( _size_by_level, (_tree_level+2) * 4 * sizeof(REAL) );	//	we use 4 to speed computation and access, +2 because of trunk
	_size_by_level_nb_allocated = _tree_level;
}

AAA_ERR	c_bdd_tree::load_do_after( o_str CONST & filename )
{
	//c_file::push_vfile();	//not saved in the vfile we want to access it as a single file
	//	CHAR	name[AAA_PATH_MAX()];
	//	fname::cpy_and_replace_ext( name, filename, "curve_data" );
	//	CHAR*	pt_num = name + strlen(name) + 1;
	//c_file::pop_vfile();

	_b_tree_generate_trig_ui = true;

	return AAA_OK;
}
/*
AAA_ERR	c_bdd_tree::save_do_after( o_str CONST & filename )
{
	c_file::push_vfile();	//not saved in the vfile we want to access it as a single file
	//	CHAR	name[AAA_PATH_MAX()];
	//	fname::cpy_and_replace_ext( name, filename, "curve_data" );
	//	CHAR*	pt_num = name + strlen(name) + 1;
	c_file::pop_vfile();

	return AAA_OK;
}
*/

void	c_bdd_tree::update()
{
	update_tree();

/*
	if( _b_ui_intercept )
	{
		ui_register( this );
	}
*/
}

static FINLINE	void compute_new_pos( REAL* pos_new, REAL* pos, REAL* move, REAL* size )
{
	pos_new[0] = pos[0] + move[0] + size[0] * rnd.get_fp32();
	pos_new[1] = pos[1] + move[1] + size[1] * rnd.get_fp32();
	pos_new[2] = pos[2] + move[2] + size[2] * rnd.get_fp32();
}

void	c_bdd_tree::add_tree_node( c_node* node, INT32 depth, UINT32 seed, REAL* move, REAL* size )
{
	INT32	nb = _tree_branch_nb;
	if( _tree_branch_nb_extra )
	{
		nb += INT32( rnd.get_fp32_max(_tree_branch_nb_extra + REAL(1)) );	//todo add a random fucntion with min and max
	}
	if( nb<= 0 )
		return;

	++depth;

	REAL	move_new[3];
	REAL	size_new[3];
	if( depth < _tree_level )
	{
		scale_v3( move_new, move, _tree_level_factor );
		scale_v3( size_new, size, _tree_level_factor );
	}

	REAL*	pos = node->get_pos();
	for( INT32 i=0; i<nb; ++i )
	{
		c_node*	node_new = node->add_branch();
		++_node_nb;

		UINT32	local_seed = ( (seed + i) << 6 ) + seed;
		rnd.set_seed( local_seed );
		compute_new_pos( node_new->get_pos(), pos, move, size );

		if( depth < _tree_level )
			add_tree_node( node_new, depth, local_seed + (depth<<8) * 113, move_new, size_new );
	}
}

bool	c_bdd_tree::is_compute_needed()
{
	if( is_diff_v3( _origin_last,			_origin ) )	
		return true;
	if( _b_trunk_last !=					_b_trunk_ui )
		return true;
	if( is_diff_v3( _trunk_pos_last,		_trunk_pos ) )	
		return true;
	if( _tree_seed_last	!=					_tree_seed )
		return true;
	if( _tree_branch_nb_last !=				_tree_branch_nb )
		return true;
	if( _tree_branch_nb_extra_last !=		_tree_branch_nb_extra )
		return true;
	if( _b_tree_branch_nb_random_last !=	_b_tree_branch_nb_random )
		return true;
	if( _tree_level_last !=					_tree_level )
		return true;
	if( _tree_level_factor_last !=			_tree_level_factor )
		return true;
	if( is_diff_v4( _tree_branch_move_last,	_tree_branch_move_ui ) )
		return true;
	if( is_diff_v4( _tree_branch_size_last,	_tree_branch_size_ui ) )
		return true;
	return false;
}
void	c_bdd_tree::validate_compute()
{
	cpy_v3( _origin_last,				_origin );
	_b_trunk_last						= _b_trunk_ui;
	cpy_v3( _trunk_pos_last,			_trunk_pos );
	_tree_seed_last						= _tree_seed;
	_tree_branch_nb_last				= _tree_branch_nb;
	_tree_branch_nb_extra_last			= _tree_branch_nb_extra;
	_b_tree_branch_nb_random_last		= _b_tree_branch_nb_random;
	_tree_level_last					= _tree_level;
	_tree_level_factor_last				= _tree_level_factor;
	cpy_v4( _tree_branch_move_last,		_tree_branch_move );
	cpy_v4( _tree_branch_size_last,		_tree_branch_size );
}

void	c_bdd_tree::update_tree()
{
	INT32 i_u, i_v, i_a;
	c_model::cur->get_axes( i_u, i_v, i_a );

	auto factor = _tree_branch_move_ui[3];
	_tree_branch_move[i_u] = _tree_branch_move_ui[0] * factor;
	_tree_branch_move[i_v] = _tree_branch_move_ui[1] * factor;
	_tree_branch_move[i_a] = _tree_branch_move_ui[2] * factor;

	factor = _tree_branch_size_ui[3];
	_tree_branch_size[i_u] = _tree_branch_size_ui[0] * factor;
	_tree_branch_size[i_v] = _tree_branch_size_ui[1] * factor;
	_tree_branch_size[i_a] = _tree_branch_size_ui[2] * factor;

	if( is_compute_needed() )
		_b_tree_generate_trig_ui = true;

	if( !_b_tree_generate_continuous_ui && !_b_tree_generate_trig_ui )
		return;
	_b_tree_generate_trig_ui = false;

	_node_root.destroy_branch_all();

	_node_root.set_pos( _origin );
	_node_nb = 1;
	c_node*	node = &_node_root;

	if( _b_trunk_ui )
	{
		c_node*	node_new = node->add_branch();
		++_node_nb;
		//UINT32	local_seed = seed + i<<10;
		//rnd.set_seed( local_seed );

		rnd.set_seed( _tree_seed * 12 );
		add_v3( node_new->get_pos(), node->get_pos(), _trunk_pos );
		node = node_new;
	}

	rnd.set_seed( _tree_seed );
	add_tree_node( node, 1, _tree_seed,  _tree_branch_move, _tree_branch_size );
	validate_compute();
}

void	c_bdd_tree::draw_branchs_low( c_node* node, INT32 level, INT32 nb )
{
	++level;
	REAL*	pos = node->get_pos();
	for( INT32 i=0; i<nb; ++i )
	{
		c_node*	node_down = node->get_branch(i);
		if( level >= _draw_level_begin_skip_ui )
		{
			REAL*	pos_down = node_down->get_pos();
			draw_line( pos, pos_down );
		}

		if( level < _draw_level_end )
		{
			INT32 nb_down = node_down->get_branch_nb();
			if( nb_down > 0 )
				draw_branchs_low( node_down, level, nb_down );
		}
	}
}
void	c_bdd_tree::draw_branchs( c_node* node, INT32 level )
{
	INT32	nb = node->get_branch_nb();
	if( nb > 0 )
		draw_branchs_low( node, level, nb );
}



void	c_bdd_tree::get_points_3d_low( REAL*& dst, c_node* node, INT32 nb, INT32& count_down )
{
	cpy_v3( dst, node->get_pos() );
	dst += 3;
	if( --count_down <= 0 )
		return;
	for( INT32 i=0; i<nb; ++i )
	{
		c_node*	node_down = node->get_branch(i);
		INT32 nb_down = node_down->get_branch_nb();
		if( nb_down > 0 )
			get_points_3d_low( dst, node_down, nb_down, count_down );
		else
		{
			cpy_v3( dst, node_down->get_pos() );
			dst += 3;
		}
		if( --count_down < 0 )
			return;
	}
	return;
}
bool	c_bdd_tree::get_points_3d( REAL* dst, INT32 nb )
{
	if( nb > 0 )
		get_points_3d_low( dst, &_node_root, _node_root.get_branch_nb(), nb );
	return true;
}
INT32	c_bdd_tree::get_point_nb()						{	return _node_nb;	}

void	c_bdd_tree::draw_branchs_multiple_low( c_node* node, INT32 level, INT32 nb )
{
	++level;

	REAL*	pos = node->get_pos();
	for( INT32 i=0; i<nb; ++i )
	{
		c_node*	node_down = node->get_branch(i);
		if( level >= _draw_level_begin_skip_ui )
		{
			if( c_multiple::cur->is_align_normal() )
			{
				REAL	nor[3];
				sub_v3( nor, node_down->get_pos(), pos );
				c_multiple::cur->draw_one_at_nor( pos, &_size_by_level[level<<2], nor );
			}
			else
				c_multiple::cur->align_then_draw( pos, &_size_by_level[level<<2] );
		}

		c_multiple::cur->next_index_u();

		if( level < _draw_level_end )
		{
			INT32 nb_down = node_down->get_branch_nb();
			if( nb_down > 0 )
				draw_branchs_multiple_low( node_down, level, nb_down );
		}
	}
}
void	c_bdd_tree::draw_branchs_multiple( c_node* node, INT32 level )
{
	INT32	nb = node->get_branch_nb();
	if( nb<= 0 )
		return;
	draw_branchs_multiple_low( node, level, nb );
}

void	c_bdd_tree::draw_single()
{
	draw_branchs( &_node_root, -1 );
}

void	c_bdd_tree::draw_multiple()
{
	if( !c_multiple::cur )
		return;
		
	_draw_level_end = MAX( 0, _tree_level - _draw_level_end_skip_ui );

	alloc_by_level();
	REAL*	pt = _size_by_level;
	cpy_v3( pt, c_multiple::cur->get_size() );
	if( _b_trunk_ui )
		cpy_v3( pt+4, pt );
	
	INT32 i_u, i_v, i_axe;
	c_multiple::cur->get_axes_index( i_u, i_v, i_axe );
	for( INT32 i=_tree_level; i>0; --i )
	{
		pt[4+i_u]	= pt[i_u] * _tree_level_factor;
		pt[4+i_v]	= pt[i_v] * _tree_level_factor;
		pt[4+i_axe] = pt[i_axe];
		pt += 4;
	}

	c_multiple::cur->set_nb( _node_nb );
	c_multiple::cur->init_index_w();
	draw_branchs_multiple( &_node_root, -1 );
	//this should exist only on draw_multiple()
	
}
