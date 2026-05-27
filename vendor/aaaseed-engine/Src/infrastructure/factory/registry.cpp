#include "registry.h"
#include "infrastructure/obj/obj_ui.h"

bool	c_registry_master::b_verbose = false;

/*
template<class T>
c_registry<T>::~c_registry()
{
#if	AAA_DEBUG()
	if( size() != 0.)
		debug_break();
#endif
}
*/

template<class T>
bool c_registry<T>::sign_in( C_PCHAR_C key, INT32 CONST len, T val )
{
	if( !key || *key==0 )	//todo is this acceptable ?
		return false;
	if( b_verbose )
		VERBOSE_PRINT_STRING( "sign in %s ---", key );

	CHAR* l_key = (CHAR *)MALLOC( len + 1 );
	strncpy( l_key, key, len );
	*(l_key + len) = 0;

	bool b_ret = _pool.insert( typename POOL_MAP::value_type( l_key, val ) ).second;
	if( !b_ret )
	{
		debug_break( "pb when signing in registry with key : %s", l_key );
		FREE( l_key );
	}
	return b_ret;
}
template<>
bool c_registry<C_PCHAR_C>::sign_in( C_PCHAR_C key, INT32 CONST len, C_PCHAR_C val )
{
	if( !key || *key==0 )	//todo is this acceptable ?
		return false;
	if( b_verbose )
		VERBOSE_PRINT_STRING( "sign in %s ---", key );

	CHAR* l_key = (CHAR *) MALLOC( len + 1 );
	strncpy( l_key, key, len );
	*(l_key + len) = 0;

	INT32 len_val = (INT32)strlen( val );
	CHAR* l_val = (CHAR *) MALLOC( len_val + 1 );
	strncpy( l_val, val, len_val );
	*(l_val + len_val) = 0;

	bool b_ret = _pool.insert( POOL_MAP::value_type( l_key, l_val ) ).second;
	if( !b_ret )
	{
		debug_break( "pb when signing in registry with key : %s", key );
		FREE( l_val );
		FREE( l_key );
	}
	return b_ret;
}



template<class T>
bool c_registry<T>::sign_out( C_PCHAR_C key )
{
	if( !key || *key==0 )	//todo is this acceptable ?
		return false;
	if( !_pool.empty() )
	{
		auto it = _pool.find(  key );
		if( it!=_pool.end() )
		{
			FREE( (void*) it->first );
			_pool.erase(it);
			return true;
		}
	}
	else
	{
		//we can't do standard error print when this arrive
		//output_debug_string( "%s() empty _pool " );
	}
	return false;
}
template<>
bool c_registry<CONST CHAR *>::sign_out( C_PCHAR_C key )
{
	if( !key || *key==0 )	//todo is this acceptable ?
		return false;
	auto it = _pool.find( (CHAR *)key );
	if( it!=_pool.end() )
	{
		FREE( (void*) it->first );
		FREE( (void*) it->second );
		_pool.erase(it);
		return true;
	}
	return false;
}

template<class T>
void c_registry<T>::clear()
{
	//we do that because we need to free the key and eventually the value for the specialization CHAR *
	//hackqq check and message when not empty
	if( !_pool.empty() )
	{
		DBG_PRINT_STRING( "%s() Cleaning non empty _pool", __FUNCTION__ );
		for( auto const & elt : _pool )
		{
			FREE( (void*) elt.first );
		}
		_pool.clear();
	}
}
template<>
void c_registry<C_PCHAR>::clear()
{
	//we do that because we need to free the key and eventually the value for the specialization CHAR *
	//hackqq check and message when not empty
	if( !_pool.empty() )
	{
		DBG_PRINT_STRING( "%s() Cleaning non empty _pool", __FUNCTION__ );
		for( auto const & elt :_pool )
		{
			FREE( (void *) elt.first );
			FREE( (void *) elt.second );
		}
		_pool.clear();
	}
}

template<class T>
c_registry<T>::~c_registry()
{
	clear();
}

template<>
c_registry<CONST CHAR * CONST>::~c_registry()
{
	clear();
}

//todo regroup the two next in one more fn
template<>
void	c_registry<INT32>::print( bool b_compact ) CONST
{	
	if( b_compact )
	{
		PRINT_STRING( "\t {" );
		bool b_start = true;
		for( auto it=_pool.begin(); it!=_pool.end(); ++it )
		{
			if( !b_start )
				PRINT_STRING( "," );
			PRINT_STRING( " %s = \"%d\"", it->first, it->second );
			if( b_start )
				b_start = false;
		}
		PRINT_STRING( "}\n" );
	}
	else
	{
		PRINT_STRING( "#\t this registry have %d items\n", _pool.size() );
		INT32 count  = 0;
		for( auto it=_pool.begin(); it!=_pool.end(); ++it )
			PRINT_STRING( "\titem %d:\t%s\t-> %d\n", ++count, it->first, it->second );
	}
}

template<>
void c_registry<C_PCHAR_C>::print( bool b_compact ) CONST
{
	if( !_pool.empty() )
	{
		if( b_compact )
		{
			PRINT_STRING( "#\t {" );
			bool b_start = true;
			for( auto const & elt : _pool )
			{
				if( !b_start )
					PRINT_STRING( "," );
				PRINT_STRING( " %s = \"%s\"", elt.first, elt.second );
				if( b_start )
					b_start = false;
			}
			PRINT_STRING( "}\n" );
		}
		else
		{
			PRINT_STRING( "#\t this registry have %d items\n", _pool.size() );
			INT32 count  = 0;
			for( auto const & elt : _pool )
				PRINT_STRING( "#\titem %d: %16s -> \"%s\"\n", ++count, elt.first, elt.second );
		}
	}
}

void	c_registry_master::perform_test()
{
	PRINT_STRING( "#\tAAASeed will now test the registry class\n" );
//force definition too
	c_registry<c_obj_ui*>	reg_obj;
	reg_obj.sign_in( "un",		nullptr	);
	reg_obj.sign_out( "un" );
	c_registry<c_factory_base*>	reg_fac;
	reg_fac.sign_in( "un",		2, nullptr	);
	reg_fac.sign_out( "un" );

	c_registry<INT32>	reg;
	reg.sign_in( "un",		1	);
	reg.sign_in( "deux",	2	);
	reg.sign_in( "trois",	3	);
	reg.sign_in( "quatre",	4	);
	reg.sign_in( "cinq",	5	);
	reg.sign_in( "six",		6	);
	reg.sign_in( "sept",	7	);
	reg.sign_in( "huit",	8	);
	reg.sign_in( "neuf",	9	);
	reg.sign_in( "deux",	10	);
	reg.print();


/*
c_registry<CHAR*>	reg_str;
	reg_str.sign_in( "un",		"1"	);
	reg_str.sign_in( "deux",	"12"	);
	reg_str.sign_in( "trois",	"123"	);
	reg_str.sign_in( "quatre",	"1234"	);
	reg_str.sign_in( "cinq",	"12345"	);
	reg_str.sign_in( "six",		"123456"	);
	reg_str.sign_in( "sept",	"1234567"	);
	reg.print();
*/
	PRINT_STRING( "#\tTEST DONE\n" );
}
