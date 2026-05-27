
#ifdef AAA_BDD_CHAT_H
#error "BDD_CHAT_H included more than once."
#endif
#define AAA_BDD_CHAT_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif

class	c_layers;
class	c_bdd_tex2d;

class	c_chat_mess
{
friend class c_bdd_chat;
	INT32							_line_nb;
	c_bdd_tex2d*					_pseudo;
	std::deque<c_bdd_tex2d*>		_lines;
public:
	c_chat_mess();
	~c_chat_mess();

	c_bdd_tex2d*	get_line( INT32 index );
};

class	c_bdd_chat final : public c_bdd_multiple 
{
	FACTORY_DECLARE(c_bdd_chat,c_bdd_multiple);
private:
	INT32	_s_axe;
	INT32	_i_u;
	INT32	_i_v;

	INT32	_datagrid_id;
	INT32	_pseudo_id;
	INT32	message_id;

	INT32	_line_min;
	INT32	_line_speed_max;
	INT32	_line_nb;
	INT32	_line_ready;
	REAL	_line_offset;
	REAL	_line_inc;
	INT32	_line_drawn;

	REAL	_speed_min;
	REAL	_speed_max;

	std::deque<c_chat_mess*>	_messages;
	INT32	_mess_nb;
	INT32	_mess_index;

	REAL	_pos_pseudo;
	REAL	_pos_pseudo_ui;
	REAL	_pos_pseudo_last;

	REAL	_pos_pseudo_after;
	REAL	_pos_pseudo_after_ui;
	REAL	_pos_pseudo_after_last;

	REAL	_pos_left;
	REAL	_pos_left_ui;
	REAL	_pos_left_last;

	REAL	_pos_right;
	REAL	_pos_right_ui;
	REAL	_pos_right_last;

//	REAL	_pos_len;

	INT32	_s_type;
	INT32	_s_type_last;
	INT32	_s_font;
	INT32	_s_font_last;
	REAL	_font_size_over_one;

	INT32	_s_font_pseudo;
	INT32	_s_font_pseudo_last;
	REAL	_font_pseudo_size_over_one;

	REAL	_origin[3];
	REAL	_origin_ui[3];

	REAL	_font_size[3];
	REAL	_font_size_last[3];
	REAL	_font_size_ui[4];

	REAL	_size[3];
	REAL	_size_last[3];

	c_layers*	_layers_pseudo;
	c_layers*	_layers_text;

	o_str	_pseudo_prefix;
	o_str	_pseudo_postfix;

	c_delta_t	_delta_t;

	bool	_b_first;
	o_str	_ostr;	//here to avoid reallocation
	bool	_b_debug;

			void	init();
			void	deinit();

public:	
	virtual	void	param_init_pt();

			void	update_chat_mess( c_chat_mess* chat_mess, C_PCHAR_C pseudo, C_PCHAR_C mess );
			void	draw_chat_mess( c_chat_mess* pcm);

			void	mess_alloc( INT32 mess_nb_in);
			void	mess_restart();
			void	mess_update();
			void	mess_pop();

	virtual	void	update();
			void	draw_one_line( INT32 i);
	virtual	void	draw_multiple();
	virtual	void	draw_single();
};
