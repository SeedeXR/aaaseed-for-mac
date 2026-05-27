
#ifdef AAA_AICHI_H
#error "AICHI_H included more than once."
#endif
#define AAA_AICHI_H 1


#define	APP_SPECIAL_AICHI()		0

#if APP_SPECIAL_AICHI()

#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif
#ifndef _MAP_
#	include <map>
#endif

class c_bdd_boid;

class	c_com
{
public:
	INT32	index_gb;
	INT32	index_jp;
	o_str	name;
	o_str	fname;
};

//vector<c_com*>	all_com;

class	c_plan
{
public:
	o_str			name;
	REAL			in;
	REAL			out;
	vector<c_com*>	coms;
};

class	c_seq
{
public:
	o_str	name;
	vector<c_plan*>	plans;
};

class	c_com_on
{
public:
	c_com*	com;
	INT32	index;
	REAL	age;
	REAL	val;
	REAL	x;
	REAL	y;
};

class	c_aichi_contact
{
public:
	REAL	time_left;
	REAL	weight_step;
};

#define	WORD_BOID_NB	12	
#define	COM_BOID_MAX_NB	32

class	c_aichi_point
{
public:
	REAL	size_x;
	REAL	weight;
	REAL	growth;
	REAL	growth_color;
	REAL	phase_left;
	BOOL	b_touchable;
	map< INT32, c_aichi_contact* >	contacts;

	c_aichi_point();
	~c_aichi_point()	{};

	void				reset();
	void				erase_contact();
	void				add_contact( INT32 id, REAL time_left_in, REAL  weight_step );
	void				del_contact( INT32 id );
	c_aichi_contact*	find_contact( INT32 id );
};

class	c_aichi final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_aichi, c_obj_active_ui );
private:
	BOOL				b_running;
	BOOL				b_running_ui;
	REAL				b_tc_active_ui;
	REAL				b_tc_active;
	REAL				tc;
	INT32				seq_cur_index;
	INT32				plan_cur_index;
	c_seq*				seq_cur;
	c_plan*				plan_cur;
	vector<c_seq*>		seqs;
	deque<c_com_on*>	coms_on;
	c_delta_t			delta_t;
	REAL				speed_min_trig;
	BOOL				b_video_done;

	REAL	dt;

	INT32	seq_nb;
	INT32	data_datagrid_id;
	BOOL	b_first;
	BOOL	b_video_on;
	REAL	transition_video_time_length;
	REAL	transition_choice_time_length;
	REAL	transition_phase;
	BOOL	b_transition;
	BOOL	b_transition_force;

	BOOL	b_send_dot_on_trig;
	BOOL	b_send_dot_off_trig;
	BOOL	b_send_dot_trig;
	BOOL	b_send_film_on_trig;
	BOOL	b_send_film_off_trig;
	BOOL	b_send_com_on_trig;

//	BOOL	b_scramble;
//	REAL	scramble_time;
//	REAL	scramble_time_left;

	BOOL	b_film_net_send;
	REAL	film_x;
	REAL	film_y;
	REAL	film_x_last;
	REAL	film_y_last;

	BOOL	b_com_active;
	BOOL	b_com_net_send;
	INT32	com_datagrid_id;
	INT32	com_img_bind_start;
	INT32	com_emiter_datagrid_id;
	o_str	com_fname_last;
	INT32	com_on_nb;
	REAL	com_life_time;
	REAL	com_emiter_size_y;
	REAL	com_emiter_size;
	REAL	com_emiter_size_x_gb;
	REAL	com_emiter_size_x_jp;

	BOOL	b_word_active;
	INT32	word_datagrid_id;
	REAL	word_text_size_factor;
	REAL	word_size_factor;
	REAL	word_size_y;
	BOOL	b_word_reset_trig;
	REAL	word_choice_threshold;
	INT32	s_word_choice;
	BOOL	b_word_net_send;


	INT32	contact_on_word_nb;
	INT32	contact_on_com_nb;
	INT32	contact_free_nb;
	REAL	contact_on_dist;
	REAL	contact_off_dist;
	REAL	contact_time;
	REAL	contact_step;
	REAL	word_decay_speed;
	REAL	word_growth_speed;

	c_aichi_point	word_aichi_boid[WORD_BOID_NB];
	c_aichi_point	com_aichi_boid[COM_BOID_MAX_NB];

	REAL	video_offset_y;
	REAL	video_size[3];

			void	deinit();
public:
	BOOL	b_dot_flip_y;

public:

			void	init();
			void	reset();

/*
	void	data_init();
	void	menu_init();
	void	menu_add_circ(INT32 i);
	void	menu_add_dep_circ( INT32 dep_in, INT32 sub_dep);
	void	menu_add_dep(INT32 i);
	void	menu_add();
	void	menu_do( INT32 value);
*/
			BOOL	do_key( UINT8* c, BOOL b_special, INT32* modifiers, INT32* x, INT32* y );

	virtual	void	param_init_pt();
	virtual	void	update();
			void	do_first();

			void	com_parse_file();
			void	com_build_filename( CHAR* dst, CHAR* src );
			void	com_on_send( BOOL b_english, REAL x, REAL y );
			void	com_on_add( c_com* com, INT32 index, BOOL b_english, REAL x, REAL y );
			void	com_on_update();

			void	words_update();
			void	dot_delete( INT32 id );
			void	dot_clear();
			void	contact_check( c_bdd_boid* bdd_boid, c_aichi_point* p_aichi_point_in, REAL size_factor, REAL size_y );
			void	contact_update( INT32 index );
			void	contact_do( INT32 index, REAL x, REAL y );

			void	word_on_send( INT32 index, REAL x, REAL y, REAL p );

			void	film_send( REAL x, REAL y );
};

extern	c_aichi*	aichi;

#endif	// APP_SPECIAL_AICHI

