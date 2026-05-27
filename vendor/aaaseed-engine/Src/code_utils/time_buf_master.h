
#ifdef AAA_TIME_BUF_MASTER_H
#error "TIME_BUF_MASTER_H included more than once."
#endif
#define AAA_TIME_BUF_MASTER_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class c_tbuf;

class	c_tbuf_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_tbuf_master, c_obj_ui );
	friend	class	c_tbuf;
	friend	class	c_bdd_time_watcher;

public:

protected:
//	INT32		max_nb;
//	INT32		free_min_nb;
	DOUBLE		_time_update;
	REAL		_buf_time_len;
//	bool		_b_follow;
	REAL		_draw_time_len;
	c_tbuf**	_p_tbuf;
	bool		_b_need_time_scale;
	REAL		_time_begin;
	REAL		_time_end;
	REAL		_time_factor;
	REAL		_time_offset;
	REAL		_text_size;
	bool		_b_text_always;
public:
		REAL	get_text_size()	CONST	{	return _text_size;		}
		bool	is_text_always() CONST	{	return _b_text_always;	}
private:
//non existent		void	swap_data_path();
//		void	alloc_free( c_tbuf::cont_tmp* p );
		void	deinit();

	c_tbuf * get(			INT32 channel_id );
	c_tbuf * get_always(	INT32 CONST channel_id );

public:
	INT32		_nb_elt;
	INT32		_nb_elt_free;

	virtual	void	param_init_pt();
			void	init();

			void	update();
};

extern	c_tbuf_master*	g_tbuf_master;

