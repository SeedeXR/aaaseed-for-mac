#include "param_focus.h"
#include "infrastructure/obj/obj_ui.h"
#include "infrastructure/param/param_draw.h"
#include "ui/flatland.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/param/traxs.h"
#include "obj_ui/demo.h"
#include "ui/pref_start.h"
#include "infrastructure/aaa_window.h"
#include "system/win32/SystemContextMenu.h"


//FOCUS
namespace focus_param
{
	p_param		param			= nullptr;
	c_obj_ui*	obj				= nullptr;
	INT32		line = 2;	//todo check if this is a dirty hack 

	p_param		param_last		= nullptr;
	c_obj_ui*	obj_last		= nullptr;

	p_param		get_param()					{	//	if( focus_param_obj)	focus_param_obj->param_init_pt();//call	//par needed ?
												return focus_param::param;	}
	c_obj_ui*	get_obj()					{	return focus_param::obj;	}
	INT32		get_line()					{	return focus_param::line;	}
	void		set_line( INT32 line )		{	focus_param::line = line;	}

	p_param		menu_param		= nullptr;
	c_obj_ui*	menu_obj		= nullptr;

	p_param		get_menu_param()			{
												if( focus_param::menu_obj )
													focus_param::menu_obj->param_init_pt();
												return focus_param::menu_param;
											}
	c_obj_ui*	get_menu_obj()				{	return focus_param::menu_obj;	}

	p_param		param_list		= nullptr;
	p_param		get_param_list()			{	return focus_param::param_list;	}

	c_obj_ui*	flatland_obj_ui	= nullptr;
	c_obj_ui*	get_flatland_obj_focus()	{	return flatland_obj_ui;		}

	p_param		param_list_out	= nullptr;
	INT32		param_list_nb	= 1;	//hack
	o_str		str_param_list;
}

//		c_connex*	focus_menu_connex		= nullptr;
//	PARAM_UPDATE
//

	

namespace {
	C_PCHAR		action_str_last			= nullptr;
	bool		b_verbose_param_focus	= false;
	c_fifo*		fifo_cur;
	bool		b_update_window_title;
}

//////
////	FIFO HACK
//
//////
////	FIFO variable
//


c_fifo::c_fifo()
:_cur_index(0)
{
}
INT32		c_fifo::get_size()					{	return (INT32)_stack.size();		}
INT32		c_fifo::get_cur_index()				{	return _cur_index;					}
C_PCHAR_C	c_fifo::get_name( UINT32 index )	{	return _stack[index].name.get();	}
void c_fifo::clear()
{
	_stack.clear();
	_cur_index = 0;
}


namespace {
	CONSTEXPR UINT32	MAX_STACK_SIZE = 256;
}

void c_fifo::fill( ST_STACK_ELT&	elt )
{
	elt.param_focus	= focus_param::param;
	elt.line_focus	= focus_param::line;
	elt.index_nb	= focus_param::param_list_nb;
	elt.param_list	= focus_param::param_list;
	elt.obj_ui		= focus_param::flatland_obj_ui;
	elt.name		= focus_param::str_param_list;
}

//remove when too much
void c_fifo::clean()
{
	UINT32	size = UINT32(_stack.size());
	if( size > MAX_STACK_SIZE + 1 )
		debug_break( "%s() fifo size %u exceeds cap %u by more than one : a previous clean was missed", __FUNCTION__, size, MAX_STACK_SIZE );
	while( size > MAX_STACK_SIZE )
	{
		_stack.pop_front();
		--size;
	}
}

void c_fifo::insert_at_end()
{
	//	if( focus_param)
	{
//was but it create a tmp obj
//		ST_STACK_ELT tmp;
//		fill( tmp );
//		_stack.push_back( tmp );
		_stack.emplace_back();
		fill( _stack.back() );
		if( b_verbose_param_focus )
			VERBOSE_PRINT_STRING( "fifo add %s at end", focus_param::str_param_list.get() );
		clean();
	}
}
/*
void c_fifo::insert( UINT32 index )
{
	//	if( focus_param)
	{
		ST_STACK_ELT tmp;
		fill( tmp );
		if( index >= _stack.size() )
		{
			_stack.push_back( tmp );
			if( b_verbose_param_focus )
				VERBOSE_PRINT_STRING( "fifo add %s at end", str_param_list.get() );
		}
		else
		{
			std::deque<ST_STACK_ELT>::iterator	it = _stack.begin();
			advance( it, index );
			_stack.insert( it, tmp );
			if( b_verbose_param_focus )
				VERBOSE_PRINT_STRING( "fifo insert %s at index %d", str_param_list.get(), index );
		}

		clean();
	}
}
*/
void c_fifo::get( UINT32 index )
{
	focus_param::line = _stack[index].line_focus;
	param_focus_set( _stack[index].obj_ui, _stack[index].param_focus );
	if( b_verbose_param_focus )
		VERBOSE_PRINT_STRING("fifo get %s at %d", focus_param::str_param_list.get(), index );
}

void c_fifo::_store()
{
	IF_THIS_NULL_RETURN();

	UINT32	i;
	UINT32	index_max = (UINT32)_stack.size();
	//find the same one
	for( i = 0; i < index_max; ++i )
	{
		if( c_param::is_same_pt( _stack[i].param_list, focus_param::param_list ) )
			break;
	}
	// if none
	if( i == index_max )
	{
		insert_at_end();	// insert( _stack.size() );
		i = (INT32)_stack.size()-1;
	}
	else
	{
		if( b_verbose_param_focus )
			VERBOSE_PRINT_STRING( "fifo found %s at %d", focus_param::str_param_list.get(), i );
		_stack[i].obj_ui	= focus_param::flatland_obj_ui;	//hack
		_stack[i].name		= focus_param::str_param_list;
		get(i);
	}
	_cur_index = i;
	//doing	menu_focus_update();
}

void	c_fifo::remove( c_obj_ui* obj_ui, p_param param )
{
	IF_THIS_NULL_RETURN();

	UINT32	i;
	UINT32	index_max;
	do
	{
		index_max = (UINT32)_stack.size();
		//find the same one
		for( i = 0; i < index_max; ++i )
		{
			ST_STACK_ELT& elt = _stack[i];
			if( elt.obj_ui == obj_ui || elt.param_focus == param || elt.param_list == param )
			{
				_stack.erase( _stack.begin()+i );
				//	we only need to move _cur_index when it pointed to the removed element
				//	AND that element was the last in the stack ; in any other case erase()
				//	leaves _cur_index pointing at a still-valid slot (the next element shifts down).
				if( _cur_index == i && i + 1 == index_max )
					_cur_index = (i > 0) ? i - 1 : 0;
				break;
			}
		}
	}
	while( i<index_max );

	//doing	menu_focus_update();
}

void c_fifo::switch_to( INT32 index )
{
	//put( cur);
	//menu_focus_update();
	//get( cur);
	
	_cur_index = IMOD( index, (INT32)_stack.size() );
	focus_param::line = _stack[_cur_index].line_focus;
	_stack[_cur_index].obj_ui->set_focus();
}

void c_fifo::_push()
{
	if( !_stack.empty() )
	{
		ST_STACK_ELT elt = _stack.front();
		_stack.pop_front();
		_stack.push_back( elt );
		switch_to( _cur_index );
	}
}

void c_fifo::_pop()
{
	if( !_stack.empty() )
	{
		ST_STACK_ELT elt = _stack.back();
		_stack.pop_back();
		_stack.push_front( elt );
		switch_to( _cur_index );
	}
}

void	c_fifo::set_cur( )	
{
	fifo_cur = this;
}

void	c_fifo::store()		{	fifo_cur->_store();	}
void	c_fifo::push()		{	fifo_cur->_push();	}
void	c_fifo::pop()		{	fifo_cur->_pop();	}

//	used in console
void	param_set_action_str_last( C_PCHAR_C action_str )
{
	if( action_str_last != action_str 
		|| focus_param::param_last != focus_param::param 
		|| focus_param::obj_last != focus_param::get_obj()
		)
	{
		focus_param::param_last = focus_param::param;
		focus_param::obj_last = focus_param::get_obj();
		action_str_last = action_str;
		b_update_window_title = true;
	}
}

static	p_param param_last_change;

//todo	should be call only when needed
void	focus_update_start_value()
{
	p_param	param = focus_param::get_param();
	param_last_change = param; 
	if( param && param->is_changeable() )	//par do a is_valid() or is_pointing for this ?
		c_param::start_value = param->update_start_value();
	else
		c_param::start_value = 1.;
	if( c_param::start_value == 0. )
		c_param::step_value = DOUBLE(0.01);
	else
		c_param::step_value = c_param::start_value * 0.01;
}

// extend to a list and have an undo/redo
void	focus_param_undo()
{
	if( param_last_change )
	{
		DOUBLE	store = param_last_change->get_value_as_double();
		param_last_change->set_value_num_from_double( c_param::start_value ); //todo deal with str types
		c_param::start_value = store;
	}
}

//hack done quickly check
void	param_focus_remove( c_obj_ui* obj, p_param param )
{
	if( fifo_cur )
	{
		fifo_cur->remove( obj, param );
		if( focus_param::obj==obj || focus_param::flatland_obj_ui==obj || (param && (focus_param::param==param || focus_param::param_list==param) ) )
		{
			//	fall back to c_pref_start::cur (a c_obj_ui by inheritance). Previous version
			//	took the address of the static cur pointer and cast that to c_obj_ui*, which
			//	produced a non-null but bogus pointer; consumers are all null-safe so the
			//	correct expression is just the pointer itself.
			focus_param::obj				=	c_pref_start::cur;
			focus_param::param				=	nullptr;
			focus_param::flatland_obj_ui	=	c_pref_start::cur;
			focus_param::param_list			=	nullptr;
		}
	}
}

namespace focus_param {
	void	set( c_obj_ui* obj, p_param param )
	{
		//	focus on first one if no default
		if( !param && !obj )
		{
			obj		= 	flatland_obj_ui;
			param	=	param_list;
		}

		//	change it if really different
		if( focus_param::param != param || focus_param::obj != obj )
		{
			focus_param::obj	=	obj;
			focus_param::param	=	param;
			param_set_action_str_last( "Focus" );
		}

		focus_update_start_value();

		//todoq should be done only on triggers
		//	if( focus_param )
		//		focus_param->do_update();
	}
}

//todo	use the line info to optimize refresh
void	focus_menu_set( c_obj_ui* obj, p_param param )
{
	if( focus_param::menu_param == param && focus_param::menu_obj == obj )
		return;

	focus_param::menu_param = param;
//	if( param )
//		DBG_PRINT_STRING( "Yes" );
//	else
//		DBG_PRINT_STRING( "Noooooooooo" );
	if( focus_param::menu_obj != obj )
	{
#if AAA_DEBUG()
		DBG_PRINT_STRING( "%s() focus menu_obj changed to %s", __FUNCTION__, obj ? obj->get_name_str() : "null" );
#endif
		focus_param::menu_obj = obj;
	}
	//hack
//	if( param )
//		focus_menu_connex = param->get_out(0);	//par
	if( b_verbose_param_focus )
	{
		param = focus_param::get_menu_param();
		GOOD_PRINT_STRING( "param_focus_menu is %s", param ? param->get_name().get() : nullptr );
	}
}

//	static to make sure that the last arg is 0 when call from outside
void	param_focus_set( c_obj_ui* obj, p_param p_focus )
{	
//	printf( "param %d data %d more %d\n", sizeof(c_param), sizeof(c_param_data), sizeof(c_param_more) );
	if( !obj )
	{
		debug_break( "%s() no obj", __FUNCTION__ );
		return;
	}

	p_param	param = obj->get_param_header();
	if( !param )
	{
		debug_break( "%s() obj have no param_header", __FUNCTION__ );
		return;
	}

	CHAR* list_name = (CHAR*)(obj->get_name_str());
	if( !list_name )
		debug_break( "%s() list_name is NULL", __FUNCTION__ );

//		if( strcmp( list_name, "Traxs")!=0 )
//			debug_break( "Bug in %s()", __FUNCTION__ );
	if( focus_param::param_list != param || ( list_name && strcmp( focus_param::str_param_list.get(), list_name ) ) )
	{
		//	if( b_fifo_add && !st_focus )	//todoq ce hack est malsain
		//		fifo_cur->put( fifo_cur->get_cur());
		focus_param::flatland_obj_ui	= obj;
		focus_param::param_list			= param;
		focus_param::param_list_out		= param + 1;

		focus_param::str_param_list.set( list_name );	//hack ???

		//hack 	this replace focus_set( obj, nullptr );		//old comment now ?
		focus_param::set( obj, p_focus );
		focus_menu_set( nullptr, nullptr );
		c_fifo::store();
	}
	else
	{
		//hack 	this replace focus_set( obj, nullptr );
		focus_param::set( obj, p_focus ? p_focus : focus_param::param );	//old comment now ?
		//hack DBG_PRINT_STRING( "%s() Can't Focus on this param", __FUNCTION__ );
	}
}



void	focus_param::focus_obj_down()
{
	c_obj_ui* obj = focus_param::get_obj();
	if( obj )
	{
		obj = obj->get_branch(0);
		obj->set_focus();
	}
}

void	focus_param::focus_obj_up()
{
	c_obj_ui* obj = focus_param::get_obj();
	if( obj )
	{
		obj = obj->get_root();
		obj->set_focus();
	}
}

// usefull to go from deformer to deformer for example 
void	focus_param::focus_obj_prev()
{
	c_obj_ui* obj = focus_param::get_obj();
	if( obj )
		obj->branch_prev()->set_focus();
}
void	focus_param::focus_obj_next()
{
	c_obj_ui* obj = focus_param::get_obj();
	if( obj )
		obj->branch_next()->set_focus();
}


//
//	PARAM_SET
//

AAA_ERR	param_list_set_value_to_def( p_param param, INT32 nb )
{
	while( nb-- )
	{
		param->set_value_to_def();
		++param;
	}
	return	AAA_OK;
}


AAA_ERR	param_list_init_value_to_ina( p_param param, INT32 nb )
{
	while( nb-- )
	{
		param->set_value_to_ina();
		++param;
	}
	return	AAA_OK;
}
AAA_ERR	param_list_set_value_to_ina( p_param param, INT32 nb )
{
	while( nb-- )
	{
		//for debug
		/*	
		if( param )
			printf( "%s\n", param->pname );
		else
			printf( "NULL param", param->pname );
		*/
		if( param->is_save() )	//todo test needed so loading will not change param not saved
			param->set_value_to_ina();
//for debug
		/*	
		if( param )
		{
			CHAR str_dbg[2048];
			param->ui_sprint( str_dbg);
			printf( str_dbg );
			//	printf( "%s\n", param->pname );
		}
		else
			printf( "NULL param", param->pname );
		*/
		++param;
	}
	return	AAA_OK;
}

void	param_set_value_to_ina()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
	{
		c_obj_ui* obj = param->get_obj();
		if( obj )
			obj->param_set_to_ina();
		else
			param->set_value_to_ina();
	}
	else
		param_list_set_value_to_ina( focus_param::param_list, focus_param::param_list_nb );
}

void	param_set_value_to_def()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
	{
		c_obj_ui* obj = param->get_obj();
		if( obj )
			obj->param_set_to_def();
		else
			param->set_value_to_def();
	}
	else
		param_list_set_value_to_def( focus_param::param_list, focus_param::param_list_nb );
}

//
//	PARAM_DISPLAY
//

void	enable_param_dbg_display()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
		param->enable_dbg_display();
	else
		for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
			param->enable_dbg_display();
}
void	disable_param_dbg_display()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
		param->disable_dbg_display();
	else
		for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
			param->disable_dbg_display();
}
void	flip_param_dbg_display()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
		param->flip_dbg_display();
	else
	{
		if( focus_param::param_list->is_dbg_display() )
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->disable_dbg_display();
		else
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->enable_dbg_display();
	}
}

//
//	PARAM_EXPAND
//
//todo use fn pointers
void	enable_param_expand()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
		param->enable_expand_all();
	else					
		for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
			param->enable_expand_all();
}
void	disable_param_expand()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
		param->disable_expand_all();
	else
		for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
			param->disable_expand_all();
}
void	flip_param_expand_out()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
		param->flip_expand_out();
	else
	{
		if( focus_param::param_list->is_expand_out() )
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->disable_expand_out();
		else
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->enable_expand_out();
	}
}
void	flip_param_expand_in()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
		param->flip_expand_in();
	else
	{
		if( focus_param::param_list->is_expand_in() )
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->disable_expand_in();
		else
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->enable_expand_in();
	}
}
void	flip_param_expand_list()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
		param->flip_expand_list();
	else
	{
		if( focus_param::param_list->is_expand_list() )
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->disable_expand_list();
		else
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->enable_expand_list();
	}
}
void	flip_param_expand()
{
	 p_param	param = focus_param::get_menu_param();
	if( param )
		param->flip_expand_all();
	else if( focus_param::param_list )
	{	
		if( focus_param::param_list->is_expand() )
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->disable_expand_all();
		else
			for( param = focus_param::param_list; param < focus_param::param_list_out; ++param )
				param->enable_expand_all();
	}
}

void	param_disconnect()
{
	p_param	param = focus_param::get_menu_param();
	if( param )
	{
#if AAA_DEBUG()
		DBG_PRINT_STRING( "%s() will do on param %s", __FUNCTION__, param->get_name().get() );
#endif
		param->disconnect();
	}
	else if( focus_param::param_list == c_module::get_ui()->get_traxs()->get_param_begin() )
		//par this test should be done in a different way
		//	or the disconnect should be passed
		//todonow was g_layerss_ui and what happen when NULL
	{
		if( BOX_ASK_WAR( "Confirmation", "Really disconnect all Global traxs ?") )
			c_module::get_ui()->get_traxs()->disconnect_all();	//todonow idem
	}
}

namespace {
	CHAR title[1024];	// thread_local
};

void	param_update()
{
	if( b_update_window_title
#if AAA_DEMO()
		&& (!demo || !(demo->is_active()))
#endif //#if AAA_DEMO()
		)
	{
		c_param* param = focus_param::get_param();
		if( param )
		{
			INT32	len;
			if( focus_param::str_param_list.is_empty() )
				len = sprintf( title, "%0.64s on ", action_str_last );
			else
				len = sprintf( title, "%0.64s on %0.64s with param ", action_str_last, focus_param::str_param_list.get() );				
			param->sprint( &title[len], 80 );
			len += (INT32) strlen( &title[len] );
//now
#if	AAA_DEBUG()
			if( len >= sizeof(title)-2 )
				debug_break();
#endif
			set_window_main_title( title ); 
			b_update_window_title = false;
		}
	}
}



bool	param_focus_menu_is_on_line( INT32 line )
{
	p_param	param = param_get_from_line( line );
//todo in window mode when menu is unroll we get an offset ??? 
//	if( param )
//		ERR_PRINT_STRING( "param is %64s", param->get_name().get() );
	//todo hack ?
	if( !c_system_context_menu_factory::get_instance()->is_menu_loop_active() )
		focus_menu_set( aaa::param::flat::obj_found, param );
//	if( line>20 )
//		DBG_PRINT_STRING( "%s() line is %d", __FUNCTION__, line );
	return param != nullptr;
}

bool	param_focus_set_on_line( INT32 line )
{
	if( line_drawn < 1 )
	{
		ERR_PRINT_STRING( "uh oh we got no lines here");
		return false;
	}

	if( line > line_drawn )
		return false;

	if( line <= get_int32_param_line_offset() )
		set_real_param_line_offset( REAL( line - 1 ) );
	else if( line >= (c_flatland::get_line_bottom() -2) )	//par should be passed not search for
	//	set_real_param_line_offset( line - (flatland::get_line_bottom()-get_int32_param_line_offset()) );
		set_real_param_line_offset( get_real_param_line_offset() + 1 );

	p_param	param = param_get_from_line( line );
	focus_param::set( aaa::param::flat::obj_found, param );
	focus_param::set_line( line );	// we have to update it here we can rely on draw to do it

	return param != nullptr ;
}

