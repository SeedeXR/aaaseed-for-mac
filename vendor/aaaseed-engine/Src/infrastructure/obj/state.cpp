#include "state.h"

#if AAA_STATE_COMPILE()

#include "infrastructure/layer/module.h"
#include "infrastructure/param/param_declare.h"

FACTORY_CREATE_V1( c_state_master, state_master, State Master, state_master );

id_unique::c_u32		c_state_master::id_generator;

namespace n_state_master
{
	static	INT32 CONST		BASE_NB_MAX		=	14;
	static	INT32 CONST		MORE_NB_MAX		=	0;
	static	INT32 CONST		GROUP_NB_MAX	=	0;
	static	INT32 CONST		PARAM_NB_MAX	=	BASE_NB_MAX
												+	MORE_NB_MAX
												+	GROUP_NB_MAX;

	static	c_param_def	param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		active				)
		PARAM_DEF_INT32(		state_index,		1, 0,	0, c_state_master::STATE_INDEX_MAX	)
		PARAM_DEF_BOOL_OFF(		continuous			)
		PARAM_DEF_REAL_ZERO(	continuous_value	)
		PARAM_DEF_INT32_LOCKED(	index_begin			)
		PARAM_DEF_INT32_LOCKED(	index_end			)
		PARAM_DEF_BOOL_OFF(		copy_to_next_trig	)
		PARAM_DEF_BOOL_OFF(		insert_trig			)
		PARAM_DEF_BOOL_OFF(		delete_trig			)
		PARAM_DEF_BOOL_OFF(		delete_confirm_trig )
		PARAM_DEF_BOOL_OFF(		a_store_trig		)
		PARAM_DEF_BOOL_OFF(		a_retrieve_trig		)
		PARAM_DEF_BOOL_OFF(		b_store_trig		)
		PARAM_DEF_BOOL_OFF(		b_retrieve_trig		)
	};
}

c_state_master*	c_state_master::cur = nullptr;
c_state_master*	c_state_master::ui = nullptr;

void	c_state_master::param_init_pt()
{				
	INT32	h = 0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _index_ui );
	param_set_pt( h, _b_cont_ui );
	param_set_pt( h, _cont_value_ui );
	param_set_pt( h, _index_begin );
	param_set_pt( h, _index_end );
	param_set_pt( h, _b_copy_to_next_trig );
	param_set_pt( h, _b_insert_trig );
	param_set_pt( h, _b_delete_trig );
	param_set_pt( h, _b_delete_confirm_trig );
	param_set_pt( h, _b_state_a_store_trig );
	param_set_pt( h, _b_state_a_retrieve_trig );
	param_set_pt( h, _b_state_b_store_trig );
	param_set_pt( h, _b_state_b_retrieve_trig );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE(c_state_master)
,_index(0)
,_index_begin(0)
,_index_end(0)
,_cont_value(-2.)
,_b_cont(FALSE)
{
	param_init_with( n_state_master::param, n_state_master::PARAM_NB_MAX );
//	alloc();
}

c_state_master::~c_state_master()
{
	if( cur == this )
		cur = nullptr;
	if( ui == this )
		ui = nullptr;

//	dealloc();
}

INT32	c_state_master::get_new_id()
{
	INT32	id;
	id = id_generator.get();
	id_generator.change();
	return id;
}

INT32	c_state_master::get_id( INT32 index )
{
	if( index < 0 )	//	special states
		return index;
	if( index < INT32(_ids.size()) )	//already defined
		return _ids[index];

#if	AAA_DEBUG()
	if( index-_ids.size()!=0 )
		debug_break();
#endif // DEBUG
	INT32	id = 0;
	while( index >= INT32(_ids.size()) )
	{
		id = get_new_id();
		_ids.push_back(id);
	}
	return id;
}

void	c_state_master::remove_index( INT32 index )
{
	if( _ids.size() < 1 )	//we keep state 0 at least
		return;
	if( index >= INT32(_ids.size()) )
		return;

	//	delete state in the objects
	set_action_delete( index );
	c_module::get_cur()->state_layers_do_action();
	//	delete state id
	_ids.erase( _ids.begin() + index );
	//	see where we are
	_index_end = _ids.size()-1;
	if( index >= _index_end )
		index = _index_end;
	//	retrieve
	set_action_retrieve( index );
	c_module::get_cur()->state_layers_do_action();
	//	set
	set_index( index );
	_index_ui = index;
}

void	c_state_master::insert_index( INT32 index )
{
	if( index >= INT32(_ids.size()) )
		return;

	//	store
	set_action_store( index );
	c_module::get_cur()->state_layers_do_action();
	//	insert state id
	_ids.insert( _ids.begin() + index, get_new_id() );
	_index_end = _ids.size()-1;
	//	store in the new state
	set_action_store( index );
	c_module::get_cur()->state_layers_do_action();
	//	set
	set_index( index );
	_index_ui = index;
}

void	c_state_master::set_index( INT32 index )
{
	_index = index;
	if( _index_begin > index )
		_index_begin = index;
	if( _index_end < index )
		_index_end = index;
}

void	c_state_master::switch_to( INT32 index )
{
	set_action_switch( get_index(), index );
	c_module::get_cur()->state_layers_do_action();
	set_index( index );
}
void	c_state_master::update()
{
	set_cur();
	if( !is_active() )
		return;
	BOOL	b_cont_update_need = FALSE;
	if( _b_cont != _b_cont_ui )
	{
		if( _b_cont_ui )
		{	//	we switch to cont so we store the current state first
			set_action_store( get_index() );
			b_cont_update_need = TRUE;
		}
		else
		{	//	we switch to one by one so we retrieve the current state first
			set_action_retrieve( get_index() );
		}
		c_module::get_cur()->state_layers_do_action();
		_b_cont = _b_cont_ui;
	}
	if( _b_cont )
	{
		if( b_cont_update_need || _cont_value != _cont_value_ui )
		{
			_cont_value = _cont_value_ui;
			INT32	ind;
			REAL	inter;
			ind = I_FLOOR( _cont_value );
			ind = MAX0( ind );
			inter = _cont_value - ind;
			set_action_retrieve_inter( ind, ind+1, inter );
			c_module::get_cur()->state_layers_do_action();
		}
	}
	else
	{
		if( _b_copy_to_next_trig && get_index() < STATE_INDEX_MAX )
		{
			INT32	next_index;
			next_index = get_index()+1;

			set_action_store( get_index() );
			c_module::get_cur()->state_layers_do_action();
			set_action_store( next_index );
			c_module::get_cur()->state_layers_do_action();
			set_index( next_index );

			_index_ui = get_index();
			_b_copy_to_next_trig = false;
		}
		if( get_index() != _index_ui ) 
			switch_to( _index_ui );
		if( _b_insert_trig )
		{
			insert_index( get_index() );
			_b_insert_trig = false;
		}
		if( _b_delete_trig && _b_delete_confirm_trig )
		{
			remove_index( get_index() );
			_b_delete_trig = false;
			_b_delete_confirm_trig = false;
		}
	}
	if( _b_state_a_store_trig )
	{
		set_action_store( -1 );
		c_module::get_cur()->state_layers_do_action();
		_b_state_a_store_trig = false;
	}
	if( _b_state_b_store_trig )
	{
		set_action_store( -2 );
		c_module::get_cur()->state_layers_do_action();
		_b_state_b_store_trig = false;
	}
	if( _b_state_a_retrieve_trig )
	{
		set_action_retrieve( -1 );
		c_module::get_cur()->state_layers_do_action();
		_b_state_a_retrieve_trig = false;
	}
	if( _b_state_b_retrieve_trig )
	{
		set_action_retrieve( -2 );
		c_module::get_cur()->state_layers_do_action();
		_b_state_b_retrieve_trig = false;
	}
}

#endif //AAA_STATE_COMPILE


