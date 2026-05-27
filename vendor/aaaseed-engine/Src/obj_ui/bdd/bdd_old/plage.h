
#ifdef AAA_PLAGE_H
#error "PLAGE_H included more than once."
#endif
#define AAA_PLAGE_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

#define	 PLAGE_SEQ_NB_MAX	128
class	c_plage final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_plage,c_obj_active_ui);
private:
	BOOL	b_game;
	BOOL	b_ecoute;
	BOOL	b_play;
	BOOL	b_trig_start;
	BOOL	b_trig_stop;
	INT32	display;
	REAL	delay;
	REAL	time_in;
	REAL	jump_to;
	CHAR*	filename;
	REAL	data[PLAGE_SEQ_NB_MAX][2];
	REAL	time[PLAGE_SEQ_NB_MAX];
	INT32	pod[PLAGE_SEQ_NB_MAX];
	INT32	index_nb;
	INT32	index;
	INT32	trig_count;
	INT32	index_cur;
	INT32	pod_waiting;
	INT32	index_waiting;
	INT32	pod_last;
	INT32	display_last;
	INT32	mode;
	INT32	wait;
	BOOL	b_running;
	BOOL	b_wait;

public:

	virtual	void	param_init_pt();

	void	load_song();
	void	set_free();
	void	set_ecoute();
	void	set_game();
	void	start_stop();
	void	start();
	void	start_game();
	void	stop();

	INT32	get_pod(REAL t);

	virtual	void	update();
};

extern	c_plage*	plage;

