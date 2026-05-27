
#ifdef AAA_PICKING_H
#error "PICKING_H included more than once."
#endif
#define AAA_PICKING_H 1


#define	AAA_PICKING()	1

#if	AAA_PICKING()
#	ifndef AAA_OBJ_UI_H
#		include "infrastructure/obj/obj_ui.h"
#	endif
#	ifndef _VECTOR_
#		include <vector>
#	endif
#	ifndef AAA_PICKED_H
#		include "draw/picked.h"
#	endif

	class	c_picking final : public c_obj_active_ui
	{
		FACTORY_DECLARE(c_picking,c_obj_active_ui);
	public:
		static	bool	b_allow_ui;
	private:
		bool	_b_verbose;
		bool	_b_verbose_set_name;
		REAL	_pos_x;
		REAL	_pos_y;
		REAL	_size_x_pixel;
		REAL	_size_y_pixel;
		INT32	_s_show;
		FP32	_color[5];
		INT32	_nb_max_ui;
		INT32	_nb_max;
	//	INT32	type_;
		INT32	_hits;
		INT32	_s_choose;
		UINT32	_hit_min;
		UINT32	_hit_max;

		bool	_b_by_id;
		bool	_b_trig_for_ui;
		bool	_b_stop_when_no_hit;

		UINT32*	_table;
		
	public:

				void	init();
		virtual	void	param_init_pt();
				void	alloc();
				void	dealloc();

		virtual	void	update();

				void	begin();
				void	end();
				void	push_name( INT32 i );
				void	pop_name();
				void	print_hits();
				void	post_hit();
				void	set_pick_ref( OBJ_UI_ID index );

				void	do_matrix();

		FINLINE	bool	is_by_id()				{ return _b_by_id; }
		FINLINE	void	set_by_id( bool b )		{ _b_by_id = b; };

		FINLINE	UINT32	get_pick_ref_min()		{ return _hit_min; }
		FINLINE	UINT32	get_pick_ref_max()		{ return _hit_max; }
		FINLINE	INT32	get_hits_nb()			{ return _hits; }

		FINLINE	void	set_xy( REAL x, REAL y ){ _pos_x = x; _pos_y = y; };
		FINLINE	REAL	get_x()					{ return _pos_x; }
		FINLINE	REAL	get_y()					{ return _pos_y; }

				void	trig_for_ui();
	private:
		static	c_picking*	picking_cur;
		static	c_picking*	picking_ui;
	public:
		static			void		c_init();
		static			void		c_deinit();

		static	FINLINE	bool		is_allow()							{	return b_allow_ui; }
		static	FINLINE	bool		is_cur()							{	return picking_cur != nullptr; }
		static	FINLINE	c_picking*	get_cur()							{	return picking_cur; }
		static	FINLINE	void		clear_cur()							{	picking_cur = nullptr; }

		static	FINLINE	void		cur_do_matrix()						{	if(is_cur())	get_cur()->do_matrix(); }
		static	FINLINE	void		cur_push_name( INT32 index )		{	if(is_cur())	get_cur()->push_name( index ); }
		static	FINLINE	void		cur_pop_name()						{	if(is_cur())	get_cur()->pop_name(); }
		static	FINLINE	void		cur_set_pick_ref( OBJ_UI_ID index )	{	if(is_cur())	get_cur()->set_pick_ref( index ); }

		static	FINLINE	void		cur_begin()							{	get_cur()->begin(); }
		static	FINLINE	void		cur_end()							{	get_cur()->end(); }
		static	FINLINE	UINT32		cur_get_pick_ref_max()				{	return get_cur()->get_pick_ref_max(); }
		static	FINLINE	INT32		cur_get_hits_nb()					{	return get_cur()->get_hits_nb(); }
		static	FINLINE	bool		cur_is_by_id()						{	return get_cur()->is_by_id(); }

		static	FINLINE	c_picking*	get_ui()							{	return picking_ui; }
		static	FINLINE	void		ui_update()							{	get_ui()->update(); }
		static	FINLINE	void		ui_set_focus()						{	get_ui()->set_focus(); }
		static	FINLINE	void		ui_set_trig()						{	get_ui()->trig_for_ui(); }

	};
#else
	class	c_picking final : public c_obj_active_ui
	{
	//	FACTORY_DECLARE(c_picking,c_obj_active_ui);
	public:
		static	FINLINE	bool		is_cur()				{	return	false; }
		static	FINLINE	void		clear_cur()				{}
		static	FINLINE	void		do_matrix_cur()			{}
		static	FINLINE	void		update_ui()				{}
		static	FINLINE	void		set_trig_ui()			{}
		static	FINLINE	c_picking*	get_ui_pt()				{	return nullptr; }
		static	FINLINE	void		begin_cur()				{}
		static	FINLINE	void		end_cur()				{}
		static	FINLINE	UINT32		get_pick_ref_max_cur()	{ return 0; }
		static	FINLINE	INT32		get_hits_nb_cur()		{ return 0; }
	};
#endif	//AAA_PICKING

