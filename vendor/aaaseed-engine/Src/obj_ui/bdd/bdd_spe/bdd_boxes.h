
#ifdef AAA_BDD_BOXES_H
#error "BDD_BOXES_H included more than once."
#endif
#define AAA_BDD_BOXES_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_box_one
{
friend	class	c_bdd_boxes;
public:	
	enum box_type : INT32
		{
			BOX_FIXE = 0,
			BOX_FREE,
			BOX_VERTICAL,
			BOX_VERTICAL_FRAMED,
			BOX_HORIZONTAL,
			BOX_HORIZONTAL_FRAMED,
			BOX_FINGER,
			BOX_POTENTIOMETER,
			BOX_TYPE_MAX
		};
	static	C_PCHAR_C	box_type_str[BOX_TYPE_MAX];
private:
	bool	_b_active;
	bool	_b_enabled;
	bool	_b_manual;

	REAL	_pos[3];
	REAL	_size[3];

	REAL	_pos_ui[3];
	REAL	_size_ui[4];

	REAL	_frame_pos[3];
	REAL	_frame_size[3];
	REAL	_frame_pos_ui[3];
	REAL	_frame_size_ui[4];

	bool	_b_sphere;
	bool	_b_inside;
	bool	_b_selected;
	INT32	_channel_id;
	INT32	_control_id;
	INT32	_s_type;
	o_str	_o_name;
	REAL	_out[3];

	FINLINE	REAL CONST *	get_color_factor()		CONST;
	FINLINE	void			set_color()				CONST;
	FINLINE	REAL CONST *	get_color_factor_back() CONST;
	FINLINE	void			set_color_back()		CONST;

	FINLINE	INT32	get_control_id()		CONST	{ return _control_id; }
	FINLINE	bool	is_active()				CONST	{ return _b_active; }
//	FINLINE	bool	is_enabled()			CONST	{ return _b_enabled; }
	FINLINE	bool	is_selected()			CONST	{ return _b_selected; }
	FINLINE	bool	is_manual()				CONST	{ return _b_manual; }
	FINLINE	bool	is_active_and_enabled()	CONST	{ return _b_active && _b_enabled; }
	FINLINE	REAL *	get_pos()						{ return _pos; }
	FINLINE	REAL *	get_size()						{ return _size; }

			void	draw( REAL* pos, REAL* size, INT32 s_dim );
			void	update( REAL* scale, REAL* box_size );

	template< INT32 s_dim >
	FINLINE	bool	is_in_box( REAL x, REAL y, REAL z ) CONST;
};

class	c_viewport;
class	c_seedcam;

class	c_bdd_boxes final : public c_bdd_multiple
{
	FACTORY_DECLARE( c_bdd_boxes, c_bdd_multiple );

public:
	static	INT32 CONST	BOXES_NB_MAX =	16;
private:
	enum box_type : INT32;

	REAL		_origin[3];
	INT32		_s_dim;
	INT32		_start;
	INT32		_stop;
	INT32		_auto_nb;
	INT32		_s_number_draw;
	REAL		_number_size;
	REAL		_text_size;
	bool		_b_draw_selected_with_line;
	INT32		_nb_selected;

	bool		_b_ui_intercept_ui;
	bool		_b_ui_crop_to_viewport;
	bool		_b_verbose;

	c_viewport*	_viewport;
	c_seedcam*	_cam;

	REAL		_ui_start[3];
	REAL		_ui_start_pos[3];
	INT32		_ui_index;
	bool		_b_vert_trig;
	bool		_b_hori_trig;
	bool		_b_finger_trig;
	bool		_b_frame_trig;
	REAL		_size[3];

	REAL		_build_left;
	REAL		_build_right;
	REAL		_build_bottom;
	REAL		_build_top;

	o_str		_target_name_symbo;
	c_bdd*		_bdd_target;
	REAL		_transfer_radius_factor;
	REAL		_transfer_translate[3];
	REAL		_transfer_scale_ui[4];

	REAL		_box_size_ui[4];
	REAL		_box_size[3];

	c_box_one	_box[BOXES_NB_MAX];
	c_box_one*	_box_selected[BOXES_NB_MAX];
public:
	//todoqq no constructor or destructor

			void	alloc(); 
			void	dealloc();

//			void	build();
			void	build_partiel( INT32 index, INT32 type );
			void	build_hori();
			void	build_vert();
			void	build_finger();
			void	switch_to_framed();

	virtual	void	param_init_pt();

	virtual	void	update();

	FINLINE	void	draw_number();
	virtual	void	draw_single();
	virtual	void	draw_multiple();

	virtual bool	can_implicit() final override { return true; }

			INT32	get_index_in(	REAL x, REAL y, REAL z );
			INT32	get_control_in(	REAL x, REAL y, REAL z );

			void	get_mouse_uv(	FP32& u, FP32& v );
	//todoqq sub class bdd_ui ?

	//
	//	UI_INTERCEPT
	//
	virtual	bool	can_ui_intercept()			CONST	{	return true; }
	virtual bool	set_ui_intercept( bool b )			{	return _b_ui_intercept_ui = b; }
	virtual bool	is_ui_intercept()			CONST	{	return _b_ui_intercept_ui; }

	virtual	bool	do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y );

	virtual	bool	is_mouse_use();
	virtual	bool	mouse_down( FP32& u_start, FP32& v_start );
	virtual	void	mouse_move( FP32 u_in, FP32 v_in );
	virtual	void	mouse_up(	FP32 u_in, FP32 v_in );

	virtual	INT32	get_point_nb() final override;
//	virtual	REAL*	get_points() final override;
	virtual	REAL*	get_point_pt(		INT32 CONST index ) final override;
//	virtual	INT32	get_point_and_id(	REAL* dst, INT32 CONST index ) final override;

	virtual	void	transfer_blobs_to( BLOBS_CONT& blobs );
};
