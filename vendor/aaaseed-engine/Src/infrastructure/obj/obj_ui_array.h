
#ifdef AAA_OBJ_UI_ARRAY_H
#error "OBJ_UI_ARRAY_H included more than once."
#endif
#define AAA_OBJ_UI_ARRAY_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif


template <class T>
class	c_obj_ui_array_pt final			//	: public c_obj
{
private:
	INT32			_i_begin	{1};			//todo change this scheme to optimize update
	INT32			_i_end		{0};
	INT32			_nb			{0};
	std::vector<T*>	_vec;

public:
	FINLINE	INT32	get_nb()					CONST	{	return _nb;			}	//todo 	we could use vector but speed ?
	FINLINE	INT32	get_index_begin()			CONST	{	return _i_begin;	}
	FINLINE	INT32	get_index_end()				CONST	{	return _i_end;		}

	FINLINE bool	check_index( INT32 index )	CONST	{	return 0 <= index && index < _nb;	}
	//todo	perhaps we should protect these access 
	FINLINE	T*		get(	INT32 index )		CONST	{	return _vec[index];		}
//not ok with index strategy
//	FINLINE	T**		get_hd( INT32 index )				{	return &_vec[index];	}

private:
	FINLINE	void	init_limit()
	{
		_i_begin = _nb+1;
		_i_end = 0;
	}
	FINLINE	void	check_limit( INT32 index )
	{
		UPDATE_MIN_MAX_SAFE( _i_begin, _i_end, index );
	}
	FINLINE void	update_limit()
	{
		INT32 i = 0;
		for( ; i < _nb; ++i )
		{
			if( get( i ) )
			{
				_i_begin = i;
				break;
			}
		}
		if( i == _nb )
			init_limit();
		else
		{
			i = _nb - 1;
			for( ; i >= 0; --i )
			{
				if( get( i ) )
				{
					_i_end = i;
					break;
				}
			}
		}
	}
public:
	FINLINE void	set_nb( INT32 nb )
	{
		if( _nb != nb )
		{	//	done once we should later handle more case
			if( _nb == 0 )
			{
				_vec.assign( nb, nullptr );
				_nb = nb;
				init_limit();
			}
			else
				debug_break( "%s() set_nb should not be called this way for now", __FUNCTION__ );
		}
	}

	c_obj_ui_array_pt( INT32 nb=0 )
	{
		set_nb( nb );
	}
	virtual ~c_obj_ui_array_pt()
	{
	}

	void	delete_objs()
	{
		for( INT32 i=_i_begin; i<=_i_end; ++i )
			SAFE_DELETE( _vec[i] );
		init_limit();
	}

	INT32	get_index_free() CONST	//todo extend when not enough
	{	//todo use _i_begin/end to optimize
		for( INT32 i = 0; i < _nb; ++i )
		{
			if( _vec[i] == nullptr )
				return i;
		}
		return -1;
	}
	FINLINE	void	set( INT32 i, T* p )
	{	
		_vec[i] = p;
		check_limit( i );
	}

	FINLINE void	swap( INT32 index_1, INT32 index_2 )
	{
		SWAP( _vec[index_1], _vec[index_2] );
		update_limit();	//	we could optimize this
	}
	FINLINE	void	delete_obj( INT32 i )	//todo refine using i value
	{
		SAFE_DELETE( _vec[i] );
		update_limit();
	}

	typedef void (T::*void_method_void) (void);
	template< void_method_void fn >
	void	do_method()
	{
		for( INT32 i=_i_begin; i<=_i_end; ++i )
		{
			T* pt = _vec[i];
			if( pt )
				(pt->*fn)();
		}
	}

	typedef void (T::*void_method_bool_const)( bool CONST );
	template< void_method_bool_const fn >
	void	do_method_bool_const( bool CONST b_in )
	{
		for( INT32 i=_i_begin; i<=_i_end; ++i )
		{
			T* pt = _vec[i];
			if( pt )
				(pt->*fn)( b_in );
		}
	}

	typedef void (T::*void_method_int32_const)( INT32 CONST );
	template< void_method_int32_const fn >
	void	do_method_index()
	{
		for( INT32 i=_i_begin; i<=_i_end; ++i )
		{
			T* pt = _vec[i];
			if( pt )
				(pt->*fn)( i );
		}
	}



/*
	//maa this drive me crazy and I don't even really know if a virtual fn could be called here
	typedef void (T::*void_method_void)();

	template< void_method_void fn >
	void	do_method()
	{
		for( INT32 i=_i_begin; i<=_i_end; ++i )
		{
			T* pt = _vec[i];
			if( pt )
				pt->fn();
		}
	}

	typedef void (T::*int_method_void)();
	template< int_method_void fn >
	void	do_int_method()
	{
		for( INT32 i=_i_begin; i<=_i_end; ++i )
		{
			T* pt = _vec[i];
			if( pt )
				pt->fn();
		}
	}

	typedef void (T::*void_method_int)( INT32 );
	template< void_method_int fn >
	void	do_method_int( INT32 val )
	{
		for( INT32 i=_i_begin; i<=_i_end; ++i )
		{
			T* pt = _vec[i];
			if( pt )
				pt->fn( val );
		}
	}
*/

};

