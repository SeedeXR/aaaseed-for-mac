
#ifdef AAA_STATE_H
#error "STATE_H included more than once."
#endif
#define AAA_STATE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#if AAA_STATE_COMPILE()
#	ifndef	AAA_OBJ_UI_H
#		include "obj_ui.h"
#	endif
#	ifndef AAA_ID_UNIQUE_H
#		include "id_unique.h"
#	endif
#	ifndef _VECTOR_
#		include <vector>
#	endif

class	c_state_master final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_state_master, c_obj_active_ui );
	friend c_params;
private:
	static	c_state_master*			ui;
	static	c_state_master*			cur;
	static	id_unique::c_u32		id_generator;
public:
					void			set_cur()		{	cur = this;	}
			static	void			set_cur_null()	{	cur = nullptr;	}
	FINLINE	static	c_state_master*	get_cur()		{	return cur;	}

	FINLINE	static	c_state_master*	get_ui()		{	return ui;	}
					void			set_ui()		{	ui = this;	}

	FINLINE	static	bool			is_state_ref()	{	return cur==nullptr || cur->_index==0; }

	enum ACTION : INT32
	{
		ACT_STORE,			//void	c_module::store_state_layers( INT32 dst )	
		ACT_SWITCH,			//void	c_module::switch_state_layers_from_to( INT32 prev, INT32 next )
		ACT_RETRIEVE,		//void	c_module::retrieve_state_layers( INT32 src )
		ACT_RETRIEVE_INTER,	//void	c_module::retrieve_state_layers( INT32 a_ind, INT32 b_ind, REAL inter )
		ACT_DELETE,
		ACT_MAX_NB
	};

private:
	std::vector<INT32>	_ids;
	INT32				_index;

	INT32				get_new_id();
	INT32				get_id( INT32 index );
protected:
	INT32	_index_ui;
	bool	_b_copy_to_next_trig;

	bool	_b_state_a_store_trig;
	bool	_b_state_a_retrieve_trig;
	bool	_b_state_b_store_trig;
	bool	_b_state_b_retrieve_trig;

	bool	_b_delete_trig;
	bool	_b_delete_confirm_trig;
	bool	_b_insert_trig;

	INT32	_index_begin;
	INT32	_index_end;

	bool	_b_cont_ui;
	bool	_b_cont;
	REAL	_cont_value;
	REAL	_cont_value_ui;
private:
	ACTION	_action;
	INT32	_act_src;
	INT32	_act_src_bis;
	INT32	_act_dst;
	REAL	_act_inter;
	INT32	_act_del;
public:
	void	set_action_store( INT32 dst )										{ _action=ACT_STORE;			_act_dst=get_id(dst);		}
	void	set_action_switch( INT32 prev, INT32 next )							{ _action=ACT_SWITCH;			_act_dst=get_id(prev); _act_src=get_id(next); }
	void	set_action_retrieve( INT32 src )									{ _action=ACT_RETRIEVE;			_act_src=get_id(src);		}
	void	set_action_retrieve_inter( INT32 src_a, INT32 src_b, REAL inter )	{ _action=ACT_RETRIEVE_INTER;	_act_src=get_id(src_a); _act_src_bis=get_id(src_b); _act_inter=inter; }
	void	set_action_delete( INT32 index )									{ _action=ACT_DELETE;			_act_del=get_id(index);		}

	static	CONST	INT32	STATE_INDEX_MAX = 999;

	virtual	void	param_init_pt();
	virtual	void	update();
	//	virtual	void	prepare_for_ui();
	//			void	init();

			void	switch_to( INT32 index );

			void	set_index( INT32 index );
			INT32	get_index()				{ return _index; }
			INT32	get_index_begin()		{ return _index_begin; }
			INT32	get_index_end()			{ return _index_end; }

			void	insert_index( INT32 index );
			void	remove_index( INT32 index );
	//	AAA_ERR	alloc(); 
	//	void	dealloc();

public:
};
#endif //AAA_STATE_COMPILE

