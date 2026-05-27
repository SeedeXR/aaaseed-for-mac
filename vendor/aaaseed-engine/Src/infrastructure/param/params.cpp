#include "params.h"
#include "infrastructure/obj/obj_ui.h"
#include "infrastructure/seedfile.h"
#include "spy.h"


p_param	c_params::alloc_param( INT32 CONST nb )
{
	if( _param_array )
	{
		debug_break( "%s() params already allocated", __FUNCTION__ );
	}
	_param_array = new c_param[nb+1];	// header is 0 and explain the +1
	if( _param_array )
	{
		_param_nb_allocated = nb;	//we don't count the header
		_param_nb_used = nb; 
	}
	else
	{
		BOX_ERR( "%s() can't alloc %d c_param", __FUNCTION__, nb );
		_param_nb_allocated = 0;
		_param_nb_used = 0;
	}
	return _param_array;
}

void	c_params::dealloc_params()		//par  should be called by the c_obj_ui destructor not by subclass
{								//todonownow
	remove_refs();

	SAFE_DELETE_ARRAY( _param_array );

	_param_nb_allocated = 0;
	_param_nb_used = 0;
#if AAA_STATE_COMPILE()
	if( _state_map )
		obj_delete( _state_map );
#endif //AAA_STATE_COMPILE
}

c_params::c_params()
	:_param_array		(	nullptr	)
	,_param_nb_allocated(	0		)
	,_param_nb_used		(	0		)
#if AAA_STATE_COMPILE()
	,_state_map(nullptr)
#endif //AAA_STATE_COMPILE
{
	//DBG_PRINT_STRING( "c_params size %d", sizeof(c_params) );
}

c_params::~c_params()
{
	if( c_obj_ui::b_aaa_exiting && c_obj_ui::b_aaa_exiting_fast )
		return;

	dealloc_params();
}



void	c_params::init_for_obj( c_obj_ui* obj, INT32 nb )
{
	SPY_PUSH_RANGE( "init_for_obj", spy::INFRA );
	c_factory_base* factory = obj->get_factory();

	if( nb<0 )
	{
		debug_break( "%s() we pass a negative or null nb of params.", __FUNCTION__ );
	}
	else
	{
		c_param* param_array = alloc_param(nb);
		if( param_array )
		{
			auto & param_header = param_array[0];
			param_header.set_id( 0 );
			param_header.attach_param( &_param_array[1], get_param_nb_allocated() );	//param
			//	param_set_type( *param_header, TYPE_GROUP );
			param_header.flip_expand_all();
			param_header.set_obj(obj);

			//link c_param to their corresponding c_param_data			
			for( INT32 id=1; id<=nb; ++id )
			{
				c_param * CONST param = param_array + id;
				param->set_id(id);

				c_param_data * CONST data = factory->get_param_data(id-1);
				param->set_data( data );
				//	the following flags are moved to the c_param where they can be changed and accessed quickly
				//		without growing the c_param structure
				//		this why these changes are made here
				//		all the transfer from c_param_def to data should be made here
				if( data->is_group() )
				{
					if( INT32 nb_is_group = INT32(data->get_def()); nb_is_group > 0 )	
					{
						INT32 id_group_end = id + nb_is_group;
						if( id_group_end >= nb || param_array[id_group_end].is_last() )	// we check last and last in enclosing group
							param->set_last();
						param_array[id_group_end].set_last();
						param->set_expand_list( !data->is_group_closed() );	//this type serve for init only
					}
				}
				if( data->is_lock() )
					param->set_lock();
#if AAA_STATE_COMPILE()
				if( data->is_state_sensitive() )
					param->set_state_sensitive();
#endif	//AAA_STATE_COMPILE
			}
			param_array[nb].set_last();
		}
		else
		{
			debug_break( "this should not happen" );
		}
	}
	SPY_POP_RANGE();
}

c_param* c_params::get( INT32 index	) CONST
{
#if AAA_DEBUG()
	if( index < 0 || _param_nb_allocated <= index )
	{
		//todo param regroup critical message
		debug_break( "%s() param index %d out of range, a crash will happen", __FUNCTION__, index );
		return nullptr;
	}
#endif
	return _param_array + index + 1;
}


void	c_params::set_pt_null_vn(		INT32& h,	INT32 nb	) CONST
{
	c_param* p = get(h);
	h += nb;
	p->set_pt_null();
	for( ; --nb>0; )
		(++p)->set_pt_null();
}

//todo this is really slow
// we need an acceleration structure by obj except if param list can be dynamic which will complexify the game
// passing an o_str or the len would make it a lot faster
c_param*	c_params::get_param_by_name( C_PCHAR_C name_in ) CONST
{
	INT32 len = (INT32) strlen( name_in );
	INT32 nb = get_param_nb_allocated();
	for( INT32 i=0; i < nb; ++i )
	{
		c_param* param = get( i );
		if( param->get_name().is_equal_nocase( name_in, len ) )
			return param;
	}
	return nullptr;
}

INT32	c_params::get_param_index_by_name( C_PCHAR_C name_in ) CONST
{
	INT32 len = (INT32) strlen( name_in );
	INT32 nb = get_param_nb_allocated();
	for( INT32 i=0; i < nb; ++i )
	{
		c_param* param = get( i );
		if( param->get_name().is_equal_nocase( name_in, len ) )
			return i;
	}
	return -1;
}

//bool	c_params::is_param_belong( c_param CONST * param ) CONST
//{
//	if( _param_array<=param && param<=_param_array+get_param_nb_allocated() )
//		return true;
//	return false;
//}

INT32	c_params::get_param_index( c_param CONST * param ) CONST
{
	c_param* first = get(0);
	if( first && first<=param && param<=get(get_param_nb_allocated()-1) )
		return INT32( param - first );
	return -1;
}

//void		c_params::set_param_header_low(	c_param* param	)
//{
//	*_param_array = *param;	//todo
//}
//void	c_params::set_param_header( c_param* param, c_obj_ui* obj ) //param
//{
//	if( _param_array )
//	{
//		_param_array[0].attach_param( param, get_param_nb_allocated() );	//param
//		//	param_set_type( *param_header, TYPE_GROUP );
//		_param_array[0].flip_expand_all();
//		_param_array[0].set_obj(obj);
//	}
//	else
//	{
//		debug_break( "this should not happen" );
//	}
//}

void	c_params::set_param_nb_used( INT32 nb )
{	
	if( _param_array )
	{
		_param_nb_used = nb;	//param check we don't overshoot the all check we still free the rest
		//this is the param header infact
		_param_array->attach_param( _param_array+1, _param_nb_used ); //param
	}
}

void	c_params::swap_param( INT32 id_a, INT32  id_b ) CONST
{
	if( id_a==0 || id_b==0 )
	{
		debug_break( "%s() can't swap param with index 0, this is a header", __FUNCTION__ );
		return;
	}

	c_param* pa = get( id_a - 1 );
	c_param* pb = get( id_b - 1 );
	//todo other way do it direct from header and check
	auto obj = pa->get_obj_owner();
	auto factory = obj->get_factory();

	bool doa = pa->is_data_owner();
	bool dob = pb->is_data_owner();
	auto data_a = pa->_data;
	auto data_b = pb->_data;
	bool la = pa->is_last();
	bool lb = pb->is_last();

	SWAP( pa->_pt,		pb->_pt		);

	SWAP( pa->_flags,	pb->_flags	);
	pa->set_last( lb );
	pb->set_last( la );
	pa->set_id( id_a );
	pb->set_id( id_b );

	pa->_data = dob ? data_b : factory->get_param_data(id_a-1); 
	pb->_data = doa ? data_a : factory->get_param_data(id_b-1); 

//	pa->set_data_owner( db );
//	pb->set_data_owner( da );
	SWAP( pa->_more,	pb->_more	);
	//todoqq what we do with the flags and data
}

void	c_params::swap_param_expand( INT32 id_a, INT32  id_b ) CONST
{
	if( id_a==0 || id_b==0 )
	{
		debug_break( "%s() can't swap param with index 0, this is a header", __FUNCTION__ );
		return;
	}

	c_param* pa = get( id_a - 1 );
	c_param* pb = get( id_b - 1 );
	bool b_a = pa->is_expand_list();
	bool b_b = pb->is_expand_list();
	
	pb->set_expand_list( b_a );
	pa->set_expand_list( b_b );
}

CHAR*	c_params::param_write_to_mem( CHAR* dst, INT32 nb ) CONST
{
	c_param* param = get_param_begin();
	while( nb-- )
	{
		dst = param->write_to_mem( dst );
		++param;
	}
	return dst;
}

CHAR CONST * c_params::param_read_from_mem( CHAR CONST * src, INT32 nb )
{
	c_param* param = get_param_begin();
	while( nb-- )
	{
		src = param->read_from_mem( src );
		++param;
	}
	return src;
}

//todo remove ref on header ?
void	c_params::remove_refs()
{
	c_param* param = get_param_begin();
	INT32 nb = get_param_nb_allocated();
	while( nb-- )
	{
		if( param->is_referenced() )
			c_obj_ui::remove_param_ref( param );
		++param;
	}
}

void	c_params::cpy_from( c_params* src, INT32 index_begin, INT32 nb ) CONST
{
	if( nb < 0 )
		nb = get_param_nb_allocated();
	c_param* param_src	= src->get( index_begin );
	c_param* param_dst	= get( index_begin );

	while( nb-- )
	{
		param_dst->cpy_value_from_param_type_same( param_src );
		++param_dst;
		++param_src;
	}
}

void	c_params::cpy_from_skip( c_params* src, INT32 nb_skip_at_begin, INT32 nb_skip_at_end ) CONST
{
	cpy_from( src, nb_skip_at_begin, get_param_nb_allocated() - nb_skip_at_begin - nb_skip_at_end );
}

#if AAA_STATE_COMPILE()
class	c_param_state_base
{
public:
	//	c_param_state();
	//	virtual	~c_param_state();

	virtual	void	store( c_param* p, INT32 id ) = 0;
	virtual	void	retrieve( c_param* p, INT32 id ) = 0;
	virtual	void	retrieve_inter( c_param* p, INT32 a_ind, INT32 b_ind, REAL inter ) = 0;

	virtual	void	remove( INT32 id ) = 0;
};

template <class T>
class	c_param_state: public c_param_state_base
{
private:
	std::map<INT32,T> _param_state;
public:
	//	c_param_state();
	//	virtual	~c_param_state();

	virtual	void	store( c_param* p, INT32 id );
	virtual	void	retrieve( c_param* p, INT32 id );
	virtual	void	retrieve_inter( c_param* p, INT32 a_ind, INT32 b_ind, REAL inter );

	virtual	void	remove( INT32 id )	{ _param_state.erase(id); }
};

template <class T>
void	c_param_state<T>::store( c_param* p, INT32 id )
{
	_param_state[id] = *((T*)(p->get_pt()));
}

template <class T>
void	c_param_state<T>::retrieve( c_param* p, INT32 id )
{
	auto it = _param_state.find(id);
	if( it != _param_state.end() )
		*((T*)(p->get_pt_changeable())) = it->second;
	else
		p->set_value_to_ina();
}

template <class T>
void	c_param_state<T>::retrieve_inter( c_param* p, INT32 a_ind, INT32 b_ind, REAL inter )
{
	std::map<INT32,T>::iterator it;

	T a;
	it = _param_state.find(a_ind);
	if( it != _param_state.end() )
		a = it->second;
	else
		a = p->get_ina();

	T b;
	it = _param_state.find(b_ind);
	if( it != _param_state.end() )
		b = it->second;
	else
		b = p->get_ina();
	*((T*)(p->get_pt_changeable())) = interpolate( a, b, inter );
}

//	if BOOL is INT32 we dont need this
/*
template <>
void	c_param_state<BOOL>::retrieve_param_inter( c_param* p, INT32 a_ind, INT32 b_ind, REAL inter )
{
	map<INT32,BOOL>::iterator it;
	if( inter < 0. )
		it = _param_state.find(a_ind);
	else
		it = _param_state.find(b_ind);
	if( it != _param_state.end() )
		*((BOOL*)(p->get_pt())) = it->second;
	else
		p->set_value_to_ina();
};
*/

template <>
void	c_param_state<INT32>::retrieve_inter( c_param* p, INT32 a_ind, INT32 b_ind, REAL inter )
{
	std::map<INT32,INT32>::iterator it;
	if( inter < 1. )	//we switch only at the next state
		it = _param_state.find(a_ind);
	else
		it = _param_state.find(b_ind);
	if( it != _param_state.end() )
		p->set_int32( it->second );
	else
		p->set_value_to_ina();
}

template <>
void	c_param_state<UINT32>::retrieve_inter( c_param* p, INT32 a_ind, INT32 b_ind, REAL inter )
{
	std::map<INT32,UINT32>::iterator it;
	if( inter < 1. )	//we switch only at the next state
		it = _param_state.find(a_ind);
	else
		it = _param_state.find(b_ind);
	if( it != _param_state.end() )
		p->set_uint32( it->second );
	else
		p->set_value_to_ina();
}

FINLINE	void	c_params::store_state( c_param* param )
{
	// do we need to store this param ?
	if( !param->need_state_store() || !param->is_type_number() )	//we don't do it on text for now
		return;

	if( param->is_ina() )
	{	//	We don't save it 
		if( _state_map )	//	but eventually we have to remove
		{
			auto it = _state_map->find(param);
			if( it != _state_map->end() )
				it->second->remove( c_state_master::get_cur()->_act_dst );
		}
		return;
	}

	//	if we have a map try to use it
	if( _state_map )
	{
		auto it = _state_map->find(param);
		if( it != _state_map->end() )
		{
			it->second->store( param, c_state_master::get_cur()->_act_dst );
			return;
		}
	}

	//we need a param_state
	c_param_state_base* param_state;
	switch( param->get_type_internal() )
	{
	//todo use type_internal
	case TYPE_INTERNAL_BOOL:	param_state = new c_param_state<bool>;		break;
	case TYPE_INTERNAL_INT32:	param_state = new c_param_state<INT32>;		break;
	case TYPE_INTERNAL_UINT32:	param_state = new c_param_state<UINT32>;	break;
	case TYPE_INTERNAL_FP32:	param_state = new c_param_state<FP32>;		break;
	case TYPE_INTERNAL_DOUBLE:	param_state = new c_param_state<DOUBLE>;	break;
	case TYPE_INTERNAL_STRING:	//param_state = new c_param_state<o_str>;
								debug_break( "%s() not for this param type", __FUNCTION__ );
								//hack we should do something here
								return;
	case TYPE_INTERNAL_NONE:
	default:					debug_break( "%s() not for this param type", __FUNCTION__ );
								return;
	}

	param_state->store( param, c_state_master::get_cur()->_act_dst );
	if(!_state_map )
	{
		_state_map = new std::map<c_param*,c_param_state_base*>;
	}
	(*_state_map)[param] = param_state;
}


FINLINE	void	c_params::store_state()
{
//	store_state( _param_header );	param_header are never an independent param sometime a proxy

	c_param* param = get_param_pt();
	INT32	nb = get_param_nb_allocated();
	while( nb-- )
	{
		store_state( param );
		++param;
	}
}

FINLINE	void	c_params::retrieve_state()
{
	if( _state_map )
	{
		INT32	id = c_state_master::get_cur()->_act_src;
		for( auto const & elt : *_state_map )
			elt.second->retrieve( elt.first, id );
	}
}

FINLINE	void	c_params::retrieve_state_inter()
{
	if( _state_map )
	{

		INT32	a =		c_state_master::get_cur()->_act_src;
		INT32	b =		c_state_master::get_cur()->_act_src_bis;
		REAL	inter =	c_state_master::get_cur()->_act_inter;
		for( auto const & elt : *_state_map )
			elt.second->retrieve_inter( elt.first, a, b, inter );
	}
}

FINLINE	void	c_params::remove_state()
{
	if( _state_map )
	{
		INT32	id = c_state_master::get_cur()->_act_del;
		for( auto const & elt : *_state_map )
			elt.second->remove(id);
	}
}

void	c_params::state_do_action()
{
	switch( c_state_master::get_cur()->_action )
	{
	case c_state_master::ACT_STORE:				store_state();						break;
	case c_state_master::ACT_SWITCH:			store_state(); retrieve_state();	break;
	case c_state_master::ACT_RETRIEVE:			retrieve_state();					break;
	case c_state_master::ACT_RETRIEVE_INTER:	retrieve_state_inter();				break;
	case c_state_master::ACT_DELETE:			remove_state();						break;
	default:									debug_break();						break;
	}
}
#endif //AAA_STATE_COMPILE
