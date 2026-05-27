
#ifdef AAA_SLICE_H
#error "SLICE_H included more than once."
#endif
#define AAA_SLICE_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_slice final : public c_obj_ui
{
	FACTORY_DECLARE( c_slice, c_obj_ui );
//friend class c_midi;
//friend class c_mackie;
public:	//hack
	bool	_b_but_exclusive[4];
	INT32	_shift_nb[2];
	o_str	_info[5];

public:
	virtual	void	param_init_pt();

	FINLINE	bool	fader_have_shift( INT32 control_index )	CONST { return control_index < _shift_nb[0]; }
	FINLINE	bool	vpot_have_shift(  INT32 control_index )	CONST { return control_index < _shift_nb[1]; }
//			void	make_line_b();

	static	void save( c_slice* pt, INT32 nb, o_str CONST & filename_in );
	static	void load( c_slice* pt, INT32 nb, o_str CONST & filename_in );
};

//extern	void		slices_alloc();
//extern	void		slices_dealloc();
//extern	c_slice*	slices_get_pt( INT32 index );
