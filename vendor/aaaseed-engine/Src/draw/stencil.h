
#ifdef AAA_STENCIL_H
#error "STENCIL_H included more than once."
#endif
#define AAA_STENCIL_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_GOL_H
#	include "gol/gol.h"
#endif

class	c_stencil final : public c_obj_ui
{
	FACTORY_DECLARE( c_stencil, c_obj_ui );
public:
	static	c_stencil*	def;
	static	c_stencil*	cur;
	static	c_stencil*	ui;

private:
	bool	_b_stencil_clear_ui;
	INT32	stencil_clear_value_;
	bool	_b_stencil_ui;
	INT32	_s_stencil_test;
	INT32	_stencil_ref;
	INT32	_stencil_mask;
	INT32	_s_stencil_op_fail;
	INT32	_s_stencil_op_depth_fail;
	INT32	_s_stencil_op_depth_pass;

public:
	virtual	void	param_init_pt();
	virtual	void	update();

	FINLINE	void	update_state()	CONST //todo strategies in layer should be clearer
					{	
						GOL::set_stencil( _b_stencil_ui );
					}
};

