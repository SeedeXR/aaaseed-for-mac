
#ifdef AAA_BDD_H
#error "BDD_H included more than once."
#endif
#define AAA_BDD_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_MULTIPLE_H
#	include "draw/multiple.h"
#endif
#ifndef AAA_PARAM_DECLARE_H
#	include "infrastructure/param/param_declare.h"
#endif
//avoid a lot of include in .cpp
#ifndef	AAA_STR_SYMBO_H
#	include "ui/strsymbo.h"
#endif

class c_bdd_empty;
class c_blob;
class c_factory_group_bdd;

typedef	std::vector<c_blob>		BLOBS_CONT;

class	c_bdd : public c_obj_ui
{	
	FACTORY_ABSTRACT_DECLARE( c_bdd, c_obj_ui );
private:
	static	c_bdd*					sta_cur;
	static	c_bdd*					sta_ui;
	
	void print_empty_mess( C_PCHAR_C fn_name );

public:
			static c_factory_group_bdd*	factory_group;
			static bool					gb_allow_clipping;

	FINLINE	static c_bdd*	get_cur()					{	return sta_cur;		}
	FINLINE	static void		set_cur( c_bdd* p_bdd )		{	sta_cur = p_bdd;	}
	FINLINE	static c_bdd*	get_ui()					{	return sta_ui;		}
	FINLINE	static void		set_ui( c_bdd* p_bdd )		{	sta_ui = p_bdd;		}

			static c_bdd_empty*	bdd_empty;	//needed by rendering some global bdd init it first thru this (for now)

			static void		c_init();
//now
/*	static	void	build_ref_to_order();
	static	INT32	get_order_from_ref(	INT32 in );
	static	INT32	get_ref_from_order(	INT32 in );
	static	CHAR*	get_str_from_ref(	INT32 in );
	static	CHAR*	get_str_from_order(	INT32 in );
	static	INT32	build_menu( PT_MENU_FN p_menu_fn, INT32 base_num );
*/
	static	AAA_ERR	build_menu( INT32 menu_id, PT_MENU_FN menu_fn );

protected:
	c_layer*		get_layer() CONST;

//	bool	b_drawable;
public:
//	FINLINE	bool	is_drawable()	{ return b_drawable; }

	static	void	begin_ui();
	static	void	end_ui();
			c_bdd*  update_bdd_target( o_str CONST & name, c_bdd* target );

//	virtual	AAA_ERR	build() = 0;

			INT32	prepare_for_ui_geo( INT32& h );	
			INT32	param_init_pt_no_geo();
			INT32	param_init_pt_geo();
			void	param_init_add_model( INT32& h );

	virtual	void	alloc();
	virtual	void	dealloc();

	virtual	void	cell_draw_obj( REAL CONST size );

	virtual	void	update();

	virtual	void	draw();
	virtual	void	draw_bbox()			{}
protected:
	bool	_b_draw_before;		//todo only used in bdd_tri (so nothing to do here)
	virtual	void	draw_before_low()	{}
public:
	FINLINE	void	draw_before()		{	if(_b_draw_before) draw_before_low(); }
			void	set_draw_before()	{	_b_draw_before = true; }
	virtual	void	draw_after()		{}

	virtual	void	draw_normal_point(	REAL len )	{}
//	virtual	void	draw_normal_edge(	REAL len )	{}
	virtual	void	draw_normal_face(	REAL len )	{}

	virtual	void	restart()			{}
	virtual bool	can_implicit()		{ return false; }

			c_bdd*	get_bdd_prev();

	virtual	INT32	get_point_dataset_nb();
	virtual	bool	set_point_dataset(	INT32 dataset_id );

	virtual	INT32	get_point_nb();
	virtual	void	set_point_nb(		INT32 nb );

	virtual	REAL*	get_points();
	virtual void	erase_points();

	virtual REAL*	get_point_pt(		INT32 CONST index );
	virtual bool	get_point(			REAL* CONST dst, INT32 CONST index );
	virtual bool	set_point(			INT32 CONST index, REAL CONST * CONST src );

	virtual	INT32	get_point_and_id(	REAL* dst, INT32 CONST index );
	virtual	bool	get_points_3d(		REAL* dst, INT32 nb );

			bool	is_normal_draw();
			bool	is_normal_needed();	
	virtual	REAL*	get_normals();

	virtual	INT32	get_segment_nb();
	virtual	REAL*	get_segments();
	virtual	void	get_segment(	REAL* a, REAL* b, INT32 index );

	virtual	void	set_color(		INT32 index,	FP32* col	);
	virtual	void	use_color(		INT32* index,	INT32 nb	);

	virtual	void	set_color_map(	INT32 index,	INT32 bind,	REAL u, REAL v, REAL fu, REAL fv	);
	virtual	void	use_color_map(	INT32* index,	INT32 nb	);

	//todo	extend and really think about this
	virtual	void	transfer_blobs_to( BLOBS_CONT& blobs );

	virtual	bool	get_point_rnd(						REAL* CONST dst,			    	REAL CONST t_in );
	virtual	bool	get_point_and_speed_rnd_time_abs(	REAL* CONST dst, REAL* CONST speed, REAL CONST t_in );
	virtual	bool	get_point_and_speed_rnd_time_rel(	REAL* CONST dst, REAL* CONST speed, REAL CONST t_in );

	virtual	bool	compute_intersection( REAL CONST* origin, REAL CONST* point, REAL* intersection) { return false; } 

	virtual	AAA_ERR	load_data();

	virtual	void	save_obj_file(			o_str CONST & filename );
	virtual	void	save_obj_file(			FILE* CONST file );
	virtual	void	save_obj_file_points(	FILE* CONST file );
	virtual	void	save_obj_file_normals(	FILE* CONST file );

	static	void	save_obj_file_points(	FILE* CONST file, REAL CONST * point,  INT32 nb );
	static	void	save_obj_file_normals(	FILE* CONST file, REAL CONST * normal, INT32 nb );

//todoqq sub class bdd_ui ? or policies
//UI
	static	void	ui_draw_point( REAL CONST * CONST pos, REAL CONST size );
	static	void	ui_register( c_bdd* bdd_add );
	static	void	ui_start_frame();
	static	void	clear_ui_intercept_all();
	static	c_bdd*	ui_start_mouse_down();
	static	bool	ui_do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y );

	static	void	flip_ui_intercept_static();

	virtual	bool	can_ui_intercept()			{	return false; }
	virtual bool	set_ui_intercept( bool b )	{	return false; }
	virtual bool	is_ui_intercept()			{	return false; }
			bool	flip_ui_intercept()			{	return set_ui_intercept( !is_ui_intercept() ); }
	virtual	bool	do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y )
												{	return false; }

//MOUSE
	//	the ui code deal with part of the mouse interaction
	//		each bdd can also do itself
	//	if a bdd want the mouse callbacks below it should answer true to this
	virtual	bool	is_mouse_use();
	//	the bdd can set a start value on down and use the result passed on move and up
	//		or the bdd can do his own cooking inside these fns used just as events trigger
	//		is mouse_down used the event it should return true
	//			cancelling the interaction with this bdd
	//			allowing the event to be passed to a lower level 
	//todo add a mouse id for multitouch/multimouse....
	virtual	bool	mouse_down(	FP32& u_start, FP32& v_start );
	virtual	void	mouse_move(	FP32 u_in, FP32 v_in );		
	virtual	void	mouse_up(	FP32 u_in, FP32 v_in );			
	virtual	bool	mouse_wheel( FP32 factor );					
//	virtual	bool	mouse_cam();									

#define	BDD_BASE_MODEL_PARAM	PARAM_DEF_BASE_STR(	TYPE_BOOL|M_SAVE_NOT,		"use_model",		1, 0,	0, 1,							gstr::current	)
#define	BDD_BASE_MULTIPLE_PARAM	PARAM_DEF_BASE_STR(	TYPE_SYMBOLIC|M_SAVE_NOT,	"use_multiple",		0, 1,	0, PT_NB_STR(gstr::borrow),		gstr::borrow	)
#define	BDD_BASE_REF_PARAM		PARAM_DEF_REF( name_symbo )

#define	BDD_BASE_PARAMS			\
	BDD_BASE_REF_PARAM			\
	BDD_BASE_MODEL_PARAM		\
	BDD_BASE_MULTIPLE_PARAM

#define	BDD_NO_GEO_BASE_PARAMS	\
	BDD_BASE_REF_PARAM

	static	INT32 CONSTEXPR	NO_GEO_PARAM_NB		=	1;
	static	INT32 CONSTEXPR	GEO_PARAM_NB		=	3;
};

//extern	c_bdd*				g_bdd_cur;

class	c_bdd_empty final : public c_bdd
{
	FACTORY_DECLARE(c_bdd_empty,c_bdd);
	virtual	void	param_init_pt() {}
};

class	c_bdd_multiple : public c_bdd
{
	FACTORY_ABSTRACT_DECLARE( c_bdd_multiple, c_bdd );
public:
	static	c_bdd_multiple*	cur;			//check if not a conflict with the bdd one	
private:
	INT32					_render_list_multiple_name;
	
protected:

public:
	virtual	void	draw_multiple();
	virtual	void	draw_single();
	virtual	void	draw();
	virtual	void	draw_w( INT32 iw );

	//COMPILE
			void	multiple_compile_one(	REAL CONST* size = c_multiple::cur->get_size(),
											INT32 axe = c_multiple::cur->get_axe()
										);
	FINLINE	INT32	get_render_list_multiple_name()	{	return	_render_list_multiple_name;	}

};

//
//	TEMPLATE for using get_from_channel 
//
template< class T, INT32 NB_MAX >
class	c_instance_by_channel
{
	typedef T*	P_T;	
	P_T*		_inst_by_channel;
	//	INT32		
public:
	c_instance_by_channel()
	{
		_inst_by_channel = new P_T[NB_MAX];
		for( size_t i = 0; i < NB_MAX; ++i )
			_inst_by_channel[i] = nullptr;
	}
	~c_instance_by_channel()
	{
		SAFE_DELETE_ARRAY( _inst_by_channel );
	}
	void set( INT32 id, P_T o )
	{
		if( 0 < id && id <= NB_MAX )
			_inst_by_channel[ id - 1 ] = o;
	}
	P_T	get( INT32 id )
	{
		if( 0 < id && id <= NB_MAX )
			return _inst_by_channel[ id - 1 ];
		return nullptr;
	}
};

