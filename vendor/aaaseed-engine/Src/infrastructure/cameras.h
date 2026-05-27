
#ifdef AAA_CAMERAS_H
#error "CAMERAS_H included more than once."
#endif
#define AAA_CAMERAS_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_OBJ_UI_ARRAY_H
#	include "infrastructure/obj/obj_ui_array.h"
#endif


class	c_seedcam;

class	c_cameras final : public c_obj_ui
{
	FACTORY_DECLARE( c_cameras, c_obj_ui );
private:
	c_obj_ui_array_pt<c_seedcam>	_array_pt;
private:
	FINLINE bool		check_index( CHAR* str, INT32 index );
public:
	//constructor linked
			void		set_nb( INT32 nb );
			void		alloc(  INT32 nb ); 
			void		dealloc();

	virtual	void		update();
	virtual	void		draw();
	//todofocus 2023 May removed virtual (was the only one)
			void		set_focus();
	virtual	void		param_init_pt();

	FINLINE	INT32		get_nb()	{ return _array_pt.get_nb(); }
//			bool		move( c_trax* to_move, INT32 inc );

private:
			c_seedcam*	load_one( INT32 index, o_str CONST & filename );
public:
			c_seedcam*	get_always( INT32 index );
			c_seedcam*	get( INT32 index );
			c_seedcam*	get_free();

	virtual	AAA_ERR		load_do_after(	o_str CONST &  filename_in );
	virtual	AAA_ERR		save_do_after(	o_str CONST & filename_in );	
};

