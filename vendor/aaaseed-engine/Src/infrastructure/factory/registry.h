
#ifdef AAA_REGISTRY_H
#error "REGISTRY_H included more than once."
#endif
#define AAA_REGISTRY_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _MAP_
#	include <map>
#endif

class c_registry_master
{
public:
	static bool	b_verbose;
	static void	perform_test();
};

template<class T>
class c_registry final : public c_registry_master
{
private:
	struct less	{ 
				public: 
				//CONST
				bool operator() ( C_PCHAR a, C_PCHAR b ) CONST
					{	return strcmp( a, b ) < 0;	}
				};
	typedef	std::map< C_PCHAR, T, less >	POOL_MAP;

	POOL_MAP _pool;
public:
	FINLINE	T			get( C_PCHAR_C key ) CONST;
	FINLINE	T			get_item( INT32 index ) CONST;
	FINLINE	C_PCHAR_C	get_key( INT32 index ) CONST;	//to be able to perform print for tests
	FINLINE	INT32		size()	CONST			{	return (INT32)_pool.size(); }
public:
	void	clear();
	virtual	~c_registry();

	bool	sign_in(	C_PCHAR_C key,	INT32 CONST len, T val );
	bool	sign_in(	C_PCHAR_C key,	T val );
	bool	sign_out(	C_PCHAR_C key	);
	void	print( bool b_compact = false ) CONST;
};

template<class T>
bool c_registry<T>::sign_in( C_PCHAR_C key, T val )
{
	return sign_in( key, (INT32)strlen(key), val );
}


template<class T>
FINLINE	T	c_registry<T>::get( C_PCHAR_C key ) CONST
{
	if( key )
	{
		auto it = _pool.find( (CHAR *)key );
		if( it != _pool.end() )
			return it->second;
	}
	return T(0);
}

template<class T>
FINLINE	T	c_registry<T>::get_item( INT32 index ) CONST
{
	auto it = _pool.begin();
	advance( it, index );
	if( it != _pool.end() )
		return it->second;
	return T(0);
}

template<class T>
FINLINE	C_PCHAR_C c_registry<T>::get_key( INT32 index ) CONST
{
	auto it = _pool.begin();
	advance( it, index );
	if( it!=_pool.end() )
		return it->first;
	return nullptr;
}

