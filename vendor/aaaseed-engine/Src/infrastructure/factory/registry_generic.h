

#ifdef AAA_REGISTRY_GENERIC_H
#error "REGISTRY_GENERIC_H included more than once."
#endif
#define AAA_REGISTRY_GENERIC_H 1

#ifndef AAA_ERR_H
#	include "err.h"
#endif

#define MAP_SERVER_HASH 0

#if	MAP_SERVER_HASH
#	ifndef _HASH_MAP_
#		include <hash_map>
#	endif
#else
#	ifndef _UNORDERED_MAP_
#		include <unordered_map>
#	endif
#endif

/*!
 * \class c_map_server
 * \brief class Map Server
 * perhaps it should not be pointer
 */
template< class T_KEY, class T >
class c_map_server final	// : public c_registry_master
{
private:
/*
struct less	{ 
public: 
		//CONST
		bool operator() ( T_KEY a, T_KEY b ) CONST
		{	return strcmp( a, b ) < 0;	}
	};
	typedef std::map< T_KEY, T, less> asso_map;
*/
#if	MAP_SERVER_HASH
	typedef std::hash_map< T_KEY, T* >			asso_map;
#else
	typedef std::unordered_map< T_KEY, T* >		asso_map;
#endif
	asso_map	_map;
	T_KEY		_key_cached;
	T*			_p_cached;
	T_KEY		_bad_key;
private:
	//	these 3 fns are private because they should not be used because too slow
	FINLINE	typename asso_map::iterator	get_by_index_low( INT32 index );
	FINLINE	T*							get_by_index( INT32 index );
//	FINLINE	T_KEY						get_key_by_index( INT32 index );
public:
	c_map_server( T_KEY bad_key )
		:_bad_key(bad_key)
		,_key_cached(bad_key)
		,_p_cached(nullptr)
	{};
	virtual	~c_map_server()							{	clear();	}

	FINLINE	T*		find( T_KEY key );
	FINLINE	T*		get( T_KEY key );
	FINLINE	INT32	size()							{	return _map.size(); }
	FINLINE	void	clear();	
//	bool	sign_in(	T_KEY key, T* obj );
//	bool	sign_out(	T_KEY key );
//	void	print();
};

template< class T_KEY, class T >
FINLINE	T*	c_map_server<T_KEY,T>::find( T_KEY key )
{
	if( _key_cached == key )
		return _p_cached;
	_key_cached = key;
	typename asso_map::iterator	it = _map.find( key );
	if( it != _map.end() )
	{
		_p_cached = it->second;
		return _p_cached;
	}
	_p_cached = nullptr;
	return nullptr;
}

template< class T_KEY, class T >
FINLINE	T*	c_map_server<T_KEY,T>::get( T_KEY key )
{
	T* pt = find( key );
	if( pt )
		return pt;

	pt = new T;
	if( pt )
	{
		_p_cached = pt;
		_map[key] = pt;
		return pt;
	}
	ERR_PRINT_STRING( "%s() can't allocate.", __FUNCTION__ );
	return nullptr;
}

template< class T_KEY, class T >
FINLINE	typename c_map_server<T_KEY,T>::asso_map::iterator	c_map_server<T_KEY,T>::get_by_index_low( INT32 index )
{
	typename asso_map::iterator	it = _map.begin();
	advance( it, index ); 
	return it;
}

template< class T_KEY, class T >
FINLINE	T*	c_map_server<T_KEY,T>::get_by_index( INT32 index )
{
	typename asso_map::iterator	it = get_by_index_low( index );
	if( it != _map.end() )
		return it->second;
	return nullptr;
}

//template< class T_KEY, class T >
//FINLINE	T_KEY	c_map_server<T_KEY,T>::get_key_by_index( INT32 index )
//{
//	typename asso_map::iterator	it = get_by_index_low( index );
//	if( it != _map.end() )
//		return it->first;
//	return nullptr;	//todo can't reurn a pointeur
//}

template< class T_KEY, class T >
FINLINE	void	c_map_server<T_KEY,T>::clear()
{
	typename asso_map::iterator			it		=	_map.begin();
	typename asso_map::const_iterator	it_end	=	_map.end();
	for( ; it != _map.end(); ++it )
	{
		delete it->second;
	}
	_map.clear();
	_key_cached = _bad_key;
	_p_cached = nullptr;
}

/*!
 * \class c_vector_server
 * \brief class Vector Server
 * \todo deal with a dynamic array size
 */
template< class T >
class c_vector_server final// : public c_registry_master
{
private:
/*
struct less	{ 
public: 
		//CONST
		bool operator() ( T_KEY a, T_KEY b ) CONST
		{	return strcmp( a, b ) < 0;	}
	};
	typedef std::map< T_KEY, T, less> asso_map;
*/
	typedef std::vector< T* >		asso_vec;
	asso_vec	_map;
	INT32		_size;
public:
	c_vector_server()						{	_size = 0;		}
	virtual	~c_vector_server()				{	dealloc_elts();	}		/*! \todo dealloc() */

	FINLINE	T*		find( INT32 key );
	FINLINE	T*		get( INT32 key );
	FINLINE	INT32	size()					{	return (INT32)_map.size(); }

//			void	clear();
			void	dealloc();
			void	dealloc_elts();
			void	clear_elts();
//	bool	sign_in(	T_KEY key, T* obj );
//	bool	sign_out(	T_KEY key );
//	void	print();
};


template< class T >
FINLINE	T*	c_vector_server<T>::find( INT32 key )
{
	if( key < 0 )
		key = 0;
	if( key < _size )
		return _map[key];
	return nullptr;
}

template< class T >
FINLINE	T*	c_vector_server<T>::get( INT32 key )
{
	T* pt = find( key );
	if( pt )
		return pt;

	if( key < 0 )
		key = 0;
	if( key >= _size )
	{
		_size = key + 1;
		_map.resize( _size, nullptr );
	}

	pt = new T;
	if( pt )
	{
		_map[key] = pt;
		return pt;
	}

	ERR_PRINT_STRING( "%s() can't allocate.", __FUNCTION__ );
	return nullptr;
}


template< class T >
void	c_vector_server<T>::dealloc()
{
	for( auto const & p : _map )
		delete p;
	_map.clear();
	_size = 0;
}

template< class T >
void	c_vector_server<T>::dealloc_elts()
{
	for( auto const & p : _map )
		delete p;
	_map.clear();
	_map.resize( _size, nullptr );
}

template< class T >
void	c_vector_server<T>::clear_elts()
{
	for( auto const & p : _map )
	{
		if(p)
			p->clear();
	}
}




