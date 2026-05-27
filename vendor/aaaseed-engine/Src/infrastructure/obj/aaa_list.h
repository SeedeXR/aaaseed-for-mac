
#ifdef AAA_AAA_LIST_H
#error "AAA_LIST_H included more than once."
#endif
#define AAA_AAA_LIST_H 1


#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef _DEQUE_
#	include <deque>
#endif


template <class T>
class c_list_pt
{
protected:
	typedef std::vector<T*> CONTAINER_PTS;
#define	CONTAINER_PTS_IT		typename std::vector<T*>::iterator
//#define	CONTAINER_PTS_IT_CONST	typename std::vector<T CONST *>::iterator
//	typedef CONTAINER_PTS::iterator CONTAINER_PTS_IT;
//	typedef std::vector<T CONST * CONST > CONTAINER_CONST;
	CONTAINER_PTS	_pts;

	//private and undefined to avoid bad =
	//c_list& operator=(CONST c_list& rhs);
public:
//	c_list_pt() {};
//	virtual ~c_list_pt();
	/*
	c_list(CONST c_list& rhs);
	CONST T&	operator[] (INT32 index) CONST;
	*/
	C_NO_CPY_MOVE( c_list_pt )
	C_DEF_CTOR_DTOR( c_list_pt )

	FINLINE	void insert_before( T* CONST item, INT32 CONST index )
			{
				//_pt.insert( &(_pt[index]), item); }
				//	more portable
				auto it = _pts.begin();
				advance( it, index );
				_pts.insert( it, item );
			}
	FINLINE	void		insert_after	(T* CONST item, INT32 CONST index )	{	insert_before( item, index+1 ); }
	FINLINE	void		insert_at_head	(T* CONST item)						{	_pts.insert( _pts.begin(), item ); }
	FINLINE	void		insert_at_tail	(T* CONST item)						{	_pts.push_back( item ); }
					
	//	for	void	connector_draw();
	FINLINE	CONTAINER_PTS const &	get_container()							{	return _pts; }
	FINLINE	INT32		found_index( T* CONST to_found )
			{
				INT32 i = 0;
				for( auto const & pt : _pts )
				{
					if( pt == to_found )
						return i;
					++i;
				}
				return -1;
			}
	FINLINE	bool		found_iterator( CONTAINER_PTS_IT& it, T CONST * CONST to_found )
			{
				auto it_end	= _pts.cend();
				for( it	= _pts.begin(); it != it_end; ++it )
				{
					if( *it == to_found )
						return true;
				}
				return false;
			}

	FINLINE	T*			remove_it( T* CONST p )		{
														CONTAINER_PTS_IT it;
														if( found_iterator( it, p ) )
															_pts.erase( it );
														else
															//hack
															ERR_PRINT_STRING( "can't remove non found element" );
														return p;
													}

	FINLINE	T*			remove( INT32 CONST index )	{
														CONTAINER_PTS_IT it = _pts.begin();
														advance( it, index );
														T* p = *it;
														_pts.erase( it );
														return p;
													}
	FINLINE	void		remove_all()				{	_pts.clear(); }

			void		delete_it( T* CONST p )			{	delete p;						}
	FINLINE	void		destroy_it( T* CONST p  )		{	delete_it( remove_it( p ) );	}
	FINLINE	void		destroy( INT32 CONST index  )	{	delete_it( remove( index ) );	}
			void		destroy_all()
			{
				IF_THIS_NULL_RETURN()
				if( !_pts.empty() )
				{
					for( auto const & pt : _pts )
						delete_it( pt );
					remove_all();
				}
			}

//inline void	LIST::delete_it( c_connex* p )
//{
//	c_connex::remove(p);
//}
//
			void		destroy_all_hack_connex()
			{
				IF_THIS_NULL_RETURN();
				if( !_pts.empty() )
				{
					//done this way because the fn_del have to remove vector element
					while( !_pts.empty() )
						T::remove( _pts.back() );
					remove_all();
				}
			}

//	FINLINE	T*			operator[]( INT32 index )		{	return	*( _pt+(index_max?IMOD( index, index_max):0) ); }
	FINLINE	bool		is_item()				CONST	{	return !_pts.empty(); }
	FINLINE	T*			get_item( INT32 index )	CONST	{	return _pts[index]; }
	FINLINE	INT32		get_item_nb()			CONST	{	return INT32(_pts.size()); }
//	FINLINE	INT32		get_len()				CONST	{	return get_item_nb(); }

			T*			get_prev( T CONST * CONST in );
			T*			get_next( T CONST * CONST in );

			void		find_by_class_name(							C_PCHAR_C class_name,	CONTAINER_PTS& vec )	CONST;
			T*			find_by_class_name(							C_PCHAR_C class_name	)						CONST;
			T*			find_by_name_symbo(							o_str CONST & o_sym		)						CONST;
			T*			find_down_by_name_symbo(					o_str CONST & o_sym		)						CONST;
			T*			find_down_by_class_name(					C_PCHAR_C class_name	)						CONST;
			T*			find_down_by_class_and_name_symbo(			C_PCHAR_C class_name,					o_str CONST & o_sym	)	CONST;
			T*			find_down_by_class_and_name_symbo_except(	C_PCHAR_C class_name,					o_str CONST & o_sym,	T* CONST o_no )	CONST;
			T*			find_down_by_class_start_and_name_symbo(	C_PCHAR_C class_name,	INT32 CONST nb,	o_str CONST & o_sym	)	CONST;
			T*			find_by_my_filename(						C_PCHAR_C class_name	) CONST;
};



template <class T>
void	c_list_pt<T>::find_by_class_name( C_PCHAR_C class_name, CONTAINER_PTS& vec ) CONST 
{
	vec.clear();
	for( auto const & pt : _pts )
	{
		if( pt->is_class_name( class_name ) )
			vec.push_back( pt );
	}
}

template <class T>
T*	c_list_pt<T>::find_by_class_name( C_PCHAR_C class_name ) CONST
{
	for( auto const & pt : _pts )
	{
		if( pt->is_class_name( class_name ) )
			return pt;
	}
	return nullptr;
}

template <class T>
T*	c_list_pt<T>::find_by_name_symbo( o_str CONST & o_sym ) CONST
{
	for( auto const & pt : _pts )
	{
		if( pt->is_name_symbo( o_sym ) )
			return pt;
	}
	return nullptr;
}

template <class T>
T*	c_list_pt<T>::find_down_by_name_symbo( o_str CONST & o_sym ) CONST
{
	for( auto const & pt : _pts )
	{	
		if( T* found = pt->find_down_by_name_symbo( o_sym ) )
			return found;
	}
	return nullptr;
}

template <class T>
T*	c_list_pt<T>::find_down_by_class_name( C_PCHAR_C class_name ) CONST
{
	for( auto const & pt : _pts )
	{
		if( T* found = pt->find_down_by_class_name( class_name ) )
			return found;
	}
	return nullptr;
}

template <class T>
T*	c_list_pt<T>::find_down_by_class_and_name_symbo( C_PCHAR_C class_name, o_str CONST & o_sym ) CONST
{
	for( auto const & pt : _pts )
	{
		if( T* found = pt->find_down_by_class_and_name_symbo( class_name, o_sym ) )
			return found;
	}
	return nullptr;
}

template <class T>
T*	c_list_pt<T>::find_down_by_class_and_name_symbo_except( C_PCHAR_C class_name, o_str CONST & o_sym, T*CONST o_no ) CONST
{
	for( auto const & pt : _pts )
	{
		if( pt != o_no )
		{
			if( T* found = pt->find_down_by_class_and_name_symbo_except( class_name, o_sym, o_no ) )
				return found;
		}
	}
	return nullptr;
}

template <class T>
T*	c_list_pt<T>::find_down_by_class_start_and_name_symbo( C_PCHAR_C class_name, INT32 CONST nb, o_str CONST & o_sym ) CONST
{
	for( auto const & pt : _pts )
	{
		if( T* found = pt->find_down_by_class_start_and_name_symbo( class_name, nb, o_sym ) )
			return found;
	}
	return nullptr;
}

template <class T>
T*	c_list_pt<T>::find_by_my_filename( C_PCHAR_C name ) CONST
{
	for( auto const & pt : _pts )
	{
		if( strcmp( pt->get_my_filename(), name ) == 0  )
			return pt;
	}
	return nullptr;
}

template <class T>
T*	c_list_pt<T>::get_prev( T CONST * CONST in )
{
	CONTAINER_PTS_IT it;
	if( found_iterator( it, in ) )
	{
		if( it == _pts.begin() )
			it = _pts.end();
		--it;
		return *it;
	}
	return nullptr;
}

template <class T>
T*	c_list_pt<T>::get_next( T CONST * CONST in )
{
	CONTAINER_PTS_IT it;
	if( found_iterator( it, in ) )
	{
		++it;
		if( it == _pts.end() )
			it = _pts.begin();
		return *it;
	}
	return nullptr;
}

//	
//	with active flag
//
template <class T>
class c_list_pt_active final : public c_list_pt<T>
{
private:
	std::deque<bool>	_actives;
	//private and undefined to avoid bad =
	//c_list& operator=(CONST c_list& rhs);
public:
	//	c_list_active();
	//	virtual ~c_list_active();

	FINLINE			bool	is_item_active( INT32 CONST index)	CONST	{ return _actives[index]; }
	FINLINE			bool*	get_item_active_pt( INT32 CONST index )		{ return &_actives[index]; }

	FINLINE			void 	insert_before( T* item, INT32 CONST index )
							{
								c_list_pt<T>::insert_before( item, index );

								auto pb = _actives.begin();
								advance( pb, index );
								_actives.insert( pb, true );
							}
	FINLINE			void	insert_at_tail( T* CONST item )
							{
								c_list_pt<T>::insert_at_tail( item );
								_actives.push_back( true );
							}

	FINLINE			T*		remove( INT32 CONST index )
							{
								auto pb = _actives.begin();
								advance( pb, index );
								_actives.erase( pb );

								return c_list_pt<T>::remove( index );
							}
	//hack we don't remove the active BOOL in the queue
//	FINLINE			T*		remove_it( T* it )			{	_pt.erase( &it); return it; }
	FINLINE			T*		remove_it( T* CONST it )
							{
								INT32	index = c_list_pt<T>::found_index( it );
								if( index < 0 )
								{
									ERR_PRINT_STRING( "can't remove non found element" );
									return nullptr;
								}
								return remove( index );
							}
	FINLINE			void	remove_all()					{
																_actives.clear();
																c_list_pt<T>::remove_all();
																//_pt.clear(); //did not compile
															}

	FINLINE			void	destroy_it( T* CONST it )		{	delete remove_it( it );	}
	FINLINE			void	destroy( INT32 CONST index )	{	delete remove( index ); }
					void	destroy_all()					{	_actives.clear(); c_list_pt<T>::destroy_all(); }

/*
	FINLINE	T*	find_by_class_name( C_PCHAR_C class_name )
	{
		return c_list_pt<T>::find_by_class_name( class_name );
	}
	FINLINE	T*	find_down_by_name_symbo( C_PCHAR_C name_symbo )
	{
		return c_list_pt<T>::find_down_by_name_symbo( name_symbo );
	}
*/
	/*
		c_list(CONST c_list& rhs);
		CONST T&	operator[] (INT32 index) CONST;
	*/
};

