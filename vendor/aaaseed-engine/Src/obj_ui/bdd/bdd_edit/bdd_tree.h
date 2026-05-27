
#ifdef AAA_BDD_TREE_H
#error "BDD_TREE_H included more than once."
#endif
#define AAA_BDD_TREE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_NODE_H
#	include "infrastructure/obj/node.h"
#endif

class	c_bdd_tree final : public c_bdd_multiple
{
	FACTORY_DECLARE( c_bdd_tree, c_bdd_multiple );
private:
public:
//	static	c_bdd_tree*	cur;
private:
	c_node	_node_root;
	INT32	_node_nb;

	REAL	_origin[3];
	REAL	_trunk_pos[3];
	INT32	_tree_seed;
	INT32	_tree_branch_nb;
	REAL	_tree_branch_nb_extra;
	bool	_b_tree_branch_nb_random;
	INT32	_tree_level;
	REAL	_tree_level_factor;
	REAL	_tree_branch_move_ui[4];
	REAL	_tree_branch_size_ui[4];
	REAL	_tree_branch_move[3];
	REAL	_tree_branch_size[3];
//	REAL	_draw_interpolator;
	bool	_b_trunk_ui;

	INT32	_draw_level_begin_skip_ui;
	INT32	_draw_level_end_skip_ui;
	INT32	_draw_level_end;

	//	TREE
	bool	_b_tree_generate_continuous_ui;
	bool	_b_tree_generate_trig_ui;


	REAL*	_size_by_level;
	INT32	_size_by_level_nb_allocated;

	REAL	_origin_last[3];
	REAL	_trunk_pos_last[3];
	INT32	_tree_seed_last;
	INT32	_tree_branch_nb_last;
	REAL	_tree_branch_nb_extra_last;
	bool	_b_tree_branch_nb_random_last;
	INT32	_tree_level_last;
	REAL	_tree_level_factor_last;
	REAL	_tree_branch_move_last[3];
	REAL	_tree_branch_size_last[3];
	bool	_b_trunk_last;


private:
			void	add_tree_node( c_node* node, INT32 depth, UINT32 seed, REAL* move, REAL* size );
			void	dealloc_by_level();
			void	alloc_by_level();

			bool	is_compute_needed();
			void	validate_compute();
public:
	virtual	void	param_init_pt();

	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
//	virtual AAA_ERR	save_do_after( o_str CONST & filename );

	virtual	void	draw_single();
	virtual	void	draw_multiple();
	virtual	void	update();

			void	update_tree();
private:
			void	draw_branchs_low(			c_node* node, INT32 level, INT32 nb );
			void	draw_branchs_multiple_low(	c_node* node, INT32 level, INT32 nb );
public:
			void	draw_branchs(			c_node* node, INT32 level );
			void	draw_branchs_multiple(	c_node* node, INT32 level );
private:
			void	get_points_3d_low( REAL*& dst, c_node* node, INT32 nb, INT32& count_down );
public:
	virtual	INT32	get_point_nb();
	virtual	bool	get_points_3d( REAL* dst, INT32 nb );
};
