
#ifdef AAA_PREF_H
#error "PREF_H included more than once."
#endif
#define AAA_PREF_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

// Preference
class	c_pref final : public c_obj_ui
{
	FACTORY_DECLARE( c_pref, c_obj_ui );
public:
	static	c_pref*	cur;	//todo singleton
private:
	// prevent copying or move
	C_NO_CPY_MOVE( c_pref )

	bool	_b_shift_out;
	bool	_b_ctrl_out;
	bool	_b_alt_out;
protected:
public:
	static void init_masters();
	static void load_masters( o_str & filename );
	static void save_masters( o_str & filename );

	virtual	void	param_init_pt();

			void	update_before();
			void	update_after();

	virtual	AAA_ERR	load_do_after(	o_str CONST & filename );
};



