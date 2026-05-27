
#ifdef AAA_GOL_ATTRIB_H
#error "GOL_ATTRIB_H included more than once."
#endif
#define AAA_GOL_ATTRIB_H 1

#ifndef	AAA_GOL_BASE_H
#	include "gol_base.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

namespace GOL	{

class c_poper
{
private:
	std::vector< void* >	_data;
	INT32					_nb		{0};
	FINLINE void add( void* CONST pv )
	{
		_data.push_back( pv );
		++_nb;
	}
	FINLINE void* get()
	{
		return _data[--_nb];
	}
public:
	typedef void	(*POP_FN)( void );
	FINLINE void init()
	{
		_nb = 0;
		_data.clear();
	}
						FINLINE void	add_fn(		POP_FN CONST fn )	{	add( (void*)fn );		}
						FINLINE POP_FN	get_fn()						{	return (POP_FN)get();	}


	template< class T >	FINLINE void	add_value(	T* CONST p )	{	add( *(void**)p );	}
	template< class T >	FINLINE void	add_value2(	T* CONST p )	{	add_value(	p+1 );	add_value(p);	}
	template< class T >	FINLINE void	add_value3(	T* CONST p )	{	add_value2(	p+1 );	add_value(p);	}
	template< class T >	FINLINE void	add_value4(	T* CONST p )	{	add_value3(	p+1 );	add_value(p);	}

	template< class T >	FINLINE void	add_value(	T CONST p )		{	add_value(&p);	}

	template< class T > FINLINE T		get_value()					{
																		void* pv = get();
																		return *((T*)&pv);
																	}
	template< class T > FINLINE void	get_value(  T* CONST p )	{	*p = get_value<T>();	}
	template< class T > FINLINE void	get_value2( T* CONST p )	{	get_value(p);	get_value(p+1);	}	
	template< class T > FINLINE void	get_value3( T* CONST p )	{	get_value(p);	get_value2(p+1);	}	
	template< class T > FINLINE void	get_value4( T* CONST p )	{	get_value(p);	get_value3(p+1);	}	

	FINLINE void pop()
	{
		while( _nb > 0 )
		{
			POP_FN fn = get_fn();
			fn();
		}
	}
};

class c_attrib_stack
{
private:
	CONST static INT32 STACK_SIZE = 32;
	c_poper		_array[STACK_SIZE];
	c_poper*	_cur;
	INT32		_nb;
public:
	c_attrib_stack()
	{
		_nb = -1;
		_cur = nullptr;
	}
	FINLINE c_poper* get_poper() CONST	{	return _cur;	}
	FINLINE c_poper* push()
	{
		if( _nb < STACK_SIZE-1 )
		{
			_cur = &_array[++_nb];
			_cur->init();	//todoopt do it in pop
			return _cur;
		}
		ERR_PRINT_STRING( "%s() GOL attrib stack limited to 32 levels, skipping this one", __FUNCTION__ );
		return _cur;
	}
	FINLINE void pop()
	{
		if( _nb >= 0 )
		{
			c_poper* poper = get_poper();
			poper->pop();
			if( --_nb>=0 )
				_cur = &_array[_nb];
			else
				_cur = nullptr;
			return;
		}
		ERR_PRINT_STRING( "%s() GOL already empty, skipping this one", __FUNCTION__ );
	}
};

extern	class c_attrib_stack g_attrib_stack;

FINLINE	c_poper*	push_att()		{	return g_attrib_stack.push();			}
FINLINE	c_poper*	get_att_poper()	{	return g_attrib_stack.get_poper();		}
FINLINE	void		pop_att()		{	return g_attrib_stack.pop();			}

}	//namespace GOL
