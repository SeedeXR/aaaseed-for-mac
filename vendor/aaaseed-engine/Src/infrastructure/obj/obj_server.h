
#ifdef AAA_OBJ_SERVER_H
#error "OBJ_SERVER_H included more than once."
#endif
#define AAA_OBJ_SERVER_H 1


#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif

template < class T >
class c_obj_server final : public c_obj
{
	typedef	std::deque<T*>	T_CONT;

	T_CONT				_free;
//	T_CONT				_used;
	mutable aaa::MUTEX	_lock		{};
//	mutable aaa::MUTEX	_lock_used	{};
	INT32				_nb			{0};
	INT32				_max;
public:
	c_obj_server( INT32 max=0 ) : _max(max)	{}
	virtual ~c_obj_server()					{	dealloc();	}
	T* get()
	{
		T* pt = nullptr;
		std::lock_guard<aaa::MUTEX> guard(_lock);
		if( _free.empty() )
		{
			if( _max==0 || _nb < _max )
			{				
				pt = new T;
				++_nb;
			}
		}
		else
		{
			pt = _free.front();
			_free.pop_front();
		}
		return pt;
	}
	void store( T* pt )
	{
		if( pt )
		{
			std::lock_guard<aaa::MUTEX> guard(_lock);
			_free.push_back( pt );
		}
	}
	void dealloc()
	{
		std::lock_guard<aaa::MUTEX> guard(_lock);
		for( auto const & elt : _free )
			delete elt;
		_free.clear();
	}

};

template< class T >
class c_obj_array_server final : public c_obj
{
	T*		_data;
	INT32	_nb;
public:
	c_obj_array_server()
		:	_data(nullptr)
		,	_nb(0)
	{}
	~c_obj_array_server()
	{
		dealloc();
	}
	void	dealloc()
	{
		SAFE_DELETE_ARRAY( _data );
		_nb = 0;
	}
	BOOL	alloc( INT32 nb )
	{
		_data = new T[nb];
		if( _data )
		{
			_nb = nb;
			return TRUE;
		}
		_nb = 0;
		return FALSE;
	}
	//this index bizness is here because of the network stuff
	FINLINE	INT32	get_nb()			CONST	{	return _nb;					}		
	FINLINE	INT32	get_index( T* p )	CONST	{	return INT32( p - _data );	}
	FINLINE	T*		get_pt_from_index( INT32 CONST index )	CONST
	{
		return ( 0 <= index && index < _nb ) ? &_data[index] : nullptr;
	}
};

