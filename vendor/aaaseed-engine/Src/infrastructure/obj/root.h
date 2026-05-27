
#ifdef AAA_ROOT_H
#error "ROOT_H included more than once."
#endif
#define AAA_ROOT_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_node_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_node_ui, c_obj_ui );
public:
	static	C_PCHAR_C str_name_def;
	virtual	void	param_init_pt() {};

	virtual	void	cell_draw_obj( REAL CONST size );
};

//	c_root should be unique created by c_obj_ui
//		it is the master root
//		then it use _node_global as default father to regroup the globals
//		at some point g_app will take over as default father
class	c_root final : public c_obj_ui
{
	FACTORY_DECLARE( c_root, c_obj_ui );
private:
	c_obj_ui*	_father_cur;	//	this is the default father of obj_ui when created
	c_node_ui*	_node_global;
public:
//todo revisit the father thing to understand and document it
	FINLINE	c_obj_ui*	get_father() CONST					{	return _father_cur; }
	FINLINE	void		set_father( c_obj_ui * CONST f )	{	_father_cur = f; }

	FINLINE	c_node_ui*	get_node_global() CONST		{	return _node_global; }

	virtual	void		param_init_pt()				{}
};

extern	void	do_enter_in_main();

