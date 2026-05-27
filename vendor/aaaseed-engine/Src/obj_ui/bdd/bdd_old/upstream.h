
#ifdef AAA_UPSTREAM_H
#error "UPSTREAM_H included more than once."
#endif
#define AAA_UPSTREAM_H 1


#define	APP_SPECIAL_UPSTREAM()	0

#if APP_SPECIAL_UPSTREAM()

#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif


enum UPSTREAM_PHASE : INT32
{
	UP_ATTENTE = 0,
	UP_DEOOMPTE,
	UP_ESSAI,
	UP_SCORE,
	UP_REMBOBINAGE,
	UP_SEQ_TRANS,
	UP_SCORE_FINAL,
	UP_EVACUATION,
	UP_NB,
};

class	c_phase
{
public:
	BOOL	b_active;
	REAL	len;
	REAL	ease_in;
	REAL	ease_out;
	REAL	time;
	REAL	progression;
	REAL	intensity;
	
public:
	c_phase();
//	~c_phase();

	void	clear();
	void	begin();
	void	end();

	void	update( REAL dt);
};

class	c_upstream final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_upstream,c_obj_active_ui);
private:
	BOOL			b_first;	//todo make it generic for specials
	BOOL			b_on;
	BOOL			b_on_last;

	BOOL			b_restart_trig;
	BOOL			b_prev_trig;
	BOOL			b_next_trig;
	REAL			progression;
	BOOL			b_restart_match_trig;
	BOOL			b_main_restart_trig;

	INT32			score;
	INT32			s_phase_ui;
	INT32			s_phase;

	INT32			essai;
	INT32			manche;

	REAL			rewind;

	BOOL			b_seq_one_by_one;
	INT32			match_index;
	INT32			seq_index;

	INT32			score_in;
	INT32			score_out;
	INT32			score_total;

	c_phase*		phase_last;
	c_phase*		phase_cur;
	c_phase			phases[UP_NB];

	INT32			pal_index;

	BOOL			b_jardin;

	c_delta_t		delta_t;
	REAL			dt;

public:

	void	init();
	void	reset();

	void	phase_begin();
	void	phase_restart();

	void	main_restart();

	void	seq_send();
	void	seq_change();

	BOOL	do_key( UINT8* c, BOOL b_special, INT32* modifiers, INT32* x, INT32* y);

	virtual	void	param_init_pt();
	virtual	void	update();
			void	do_first();
};

extern	c_upstream*	upstream;

#endif	//APP_SPECIAL_UPSTREAM

