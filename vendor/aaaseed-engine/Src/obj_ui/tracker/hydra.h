
#ifdef AAA_HYDRA_H
#error "HYDRA_H included more than once."
#endif
#define AAA_HYDRA_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if	AAA_TRACKER_HYDRA()
#	if !defined(_SIXENSE_H_)
#		include "sixense.h"
#		include "sixense_utils/controller_manager/controller_manager.hpp"
#	endif
#endif

typedef	struct	st_hydra_controller
{
	REAL	pos[3];
	REAL	rot[9];
	REAL	joy[2];
	REAL	trigger;
	bool	b_buttons[7];

	UINT32	sequence_nb;
	REAL	rot_quat[4];
	bool	b_enabled;
	INT32	control_index;
	bool	b_docked;
	UINT32	which_hand;
} st_hydra_controller;

class	c_hydra final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_hydra, c_obj_active_ui );
public:
#if AAA_TRACKER_HYDRA()
	static	sixenseUtils::ControllerManager*	_controller_manager;
#endif
	static INT32 CONST CTRL_NB = 2;
			o_str	_callback_text;
//	static	CONST	INT32	HYDRA_OBJ_MAX_NB = 1;
private:
			bool	_b_opened;
			bool	_b_open_ui;
			bool	_b_verbose;

			UINT32	_base_index;
			UINT32	_controller_nb;

			bool	_b_filter_active;
			bool	_b_filter_active_ui;
			REAL	_filter_near_range;
			REAL	_filter_near_val;
			REAL	_filter_far_range;
			REAL	_filter_far_val;
			REAL	_filter_near_range_ui;
			REAL	_filter_near_val_ui;
			REAL	_filter_far_range_ui;
			REAL	_filter_far_val_ui;

			bool	_b_tra_in_meter;
			bool	_b_coor_same;
			bool	_b_tra_center_trig[CTRL_NB];
			bool	_b_tra_zero_trig[CTRL_NB];
			REAL	_tra_ui[CTRL_NB][3];
			REAL	_sca[CTRL_NB][3];
			REAL	_sca_ui[CTRL_NB][4];
			REAL	_base_rot_offset[3];

			st_hydra_controller	_ctler[CTRL_NB];

			UINT32	_firmware_revision;
			UINT32	_hardware_revision;
			UINT32	_hemi_tracking;

public:
	virtual	void	param_init_pt();

	static	void	lib_init();
	static	void	lib_deinit();
	static	INT32	enumerate();

			void	init();
			AAA_ERR	open();
			void	close();

	virtual	void	update();
	virtual	void	draw();
};

extern	c_hydra*	g_hydra;


