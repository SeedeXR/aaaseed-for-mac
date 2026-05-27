
#ifdef AAA_ARCOM88_H
#error "ARCOM88_H included more than once."
#endif
#define AAA_ARCOM88_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_arcom_88 final : public c_obj_active_ui 
{
	FACTORY_DECLARE(c_arcom_88,c_obj_active_ui);
private:
	INT32	base_addr;

	INT32	in;
	INT32	out;
	INT32	out_ui;

	INT32	count;
	INT32	num;

	BOOL	b_out[8];
	BOOL	b_in[8];
public:
			void	enable();
			void	disable();

			void	init();

	virtual	void	update();

	virtual	void	param_init_pt();
	virtual	void	param_init();

			void	set_out_one( INT32 index);
			INT32	get_in_just_one();
};

extern	c_arcom_88*	arcom;

#endif	//  __ARCOM88_H__