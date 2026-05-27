
#ifdef AAA_BDD_DEPTINA_H
#error "BDD_DEPTINA_H included more than once."
#endif
#define AAA_BDD_DEPTINA_H 1


#ifndef	AAA_BDD_H
#include "obj_ui/bdd/util/bdd.h"
#endif



class	c_bdd_deptina final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_deptina, c_bdd );
public:

protected:
	INT32		_bind_src_ui;
	INT32		_bind_mask_ui;

	UINT32		_nb_u;
	UINT32		_nb_v;
	UINT32		_nb_zone;

	UINT32		_img_index_last;
	UINT32		_sx;
	UINT32		_sy;

	UINT32		_nb;

	//INT32*		_color_index;

	FP32*	_color_max;

	bool		_b_crop_ui;
	UINT32		_left_ui;
	UINT32		_top_ui;
	UINT32		_right_ui;
	UINT32		_bottom_ui;

	UINT32		_nb_u_ui;
	UINT32		_nb_v_ui;
private:
	void		alloc( UINT32 nb_u, UINT32 nb_v );
public:
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();

};


