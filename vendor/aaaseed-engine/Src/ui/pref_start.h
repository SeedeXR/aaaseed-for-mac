
#ifdef AAA_PREF_START_H
#error "PREF_START_H included more than once."
#endif
#define AAA_PREF_START_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

// Preference at start
class	c_pref_start final : public c_obj_ui
{
	FACTORY_DECLARE( c_pref_start, c_obj_ui );
public:
	static	c_pref_start*	cur;	//todo singleton
	static	bool			b_keep_system_awake;
	static	bool			b_keep_display_on;
	static	bool			b_license_use;
private:
	// prevent copying or move
	C_NO_CPY_MOVE( c_pref_start )
protected:
public:
	virtual	void	param_init_pt_static();
	virtual	void	prepare_for_ui();

	virtual	AAA_ERR	load_do_before(	o_str CONST & filename );
	virtual	AAA_ERR	load_do_after(	o_str CONST & filename );
	virtual	AAA_ERR	save_do_before(	o_str CONST & filename );

	void	update();
};

