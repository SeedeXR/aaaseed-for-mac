
#ifdef AAA_BDD_ARRAY_H
#error "BDD_ARRAY_H included more than once."
#endif
#define AAA_BDD_ARRAY_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_layer;

//#define	BDD_ARRAY_SRC_MAX_NB	16
//#define	BDD_ARRAY_MAX_NB		256
//#define	BDD_ARRAY_FAST_PROBA	1024

struct	ST_ARRAY_ELT
{
	INT32	layer_index;
	REAL	scale;
	REAL	rotation;
	REAL	dist;
};

class	c_bdd_array final : public c_bdd_multiple 
{
	FACTORY_DECLARE(c_bdd_array,c_bdd_multiple);
public:
	static	CONST	INT32	BDD_ARRAY_SRC_MAX_NB	= 16;
	static	CONST	INT32	BDD_ARRAY_MAX_NB		= 256;
	static	CONST	INT32	BDD_ARRAY_FAST_PROBA	= 1024;
private:
	INT32	draw_mode;
	INT32	seed;
	INT32	i_axe;
	INT32	i_u;
	INT32	i_v;

	REAL	tra_factor;
	REAL	min[3];
	REAL	max[3];
	REAL	scale_factor;
	REAL	scale_min[3];
	REAL	scale_max[3];
	REAL	clip_angle;

	INT32	nb_to_draw;
	INT32	nb_elt;

	bool	b_created_all;
	REAL	proba_x;
	REAL	proba_z;
	REAL	proba[BDD_ARRAY_SRC_MAX_NB];
	ST_ARRAY_ELT	elt[BDD_ARRAY_MAX_NB];
	INT32	proba_fast[BDD_ARRAY_FAST_PROBA];
	REAL	sum_last;

	void	alloc(); 
	void	dealloc();

public:
	virtual	void	param_init_pt();
	virtual	void	param_init();

	virtual	void	update();
	virtual	void	draw_single();
	virtual	void	draw_multiple();
			void	build();

			void	create_elt( INT32 index, INT32 seed );
};


