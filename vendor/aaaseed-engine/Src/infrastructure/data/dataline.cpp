#include "infrastructure/data/dataline.h"

void	c_data_elt::clear()
{
	_str = nullptr;
	_b_number = false;
	_d = 0.;
}

c_data_elt::c_data_elt()
{
	clear();
}

c_data_elt::~c_data_elt()
{
	if( _str )
		delete _str;
}

c_data_line::c_data_line()
{
	_size = 0;
//	elt.assign( COL_MAX, nullptr);
/*
	INT32 i;
	for( i=COL_MAX-1; i>=0; --i )
		elt.push_back( nullptr);
*/
}

c_data_line::~c_data_line()
{
	for( INT32 i = _size - 1; i >= 0; --i )
	{
		if( _elt[i] )
			delete _elt[i];
	}
}

void	c_data_line::clear()
{
	for( INT32 i = _size - 1; i >= 0; --i )
	{
		if( _elt[i] )
			_elt[i]->clear();
	}
}

void	c_data_line::set_double( INT32 index, DOUBLE in )
{
	c_data_elt**	hd = get_elt_hd(index);
	if( *hd )
		(*hd)->set_double( in );
	else
	{
		//if( in!= 0 )	//june 2018 Maa remove so we can store 0 and nil differently
		{
			*hd = new c_data_elt;	//todoopt	create constructor
			if( *hd )
				(*hd)->set_double( in );
		}
	}
}

void	c_data_line::set_str( INT32 index, C_PCHAR_C in )
{
	c_data_elt**	hd = get_elt_hd( index );
	if( *hd )
		(*hd)->set_str( in );
	else
	{
		*hd = new c_data_elt;		//todoopt	create constructor
		if( *hd )
			(*hd)->set_str( in );
	}
}

void	c_data_line::set_str_and_double(	INT32 index, C_PCHAR_C str,	DOUBLE d	)
{
	c_data_elt**	hd = get_elt_hd(index);
	if( *hd )
		(*hd)->set_str_and_double( str, d );
	else
	{
		if(  str || d != .0 )
		{
			*hd = new c_data_elt;	//todoopt	create constructor
			if( *hd )
				(*hd)->set_str_and_double( str, d );
		}
	}
}

void	c_data_line::insert( INT32 index )
{
	if( index <_size )
	{
		//	find the place
		iter_elt	it = _elt.begin();
		advance( it, index );
		//for ( INT32 i=index; i>0; --i )
		//	++it;
		//insert it
		_elt.insert( it, new c_data_elt );
		++_size;
	}
	else
	{
		get_elt_hd( index );
	}
}

void	c_data_line::insert_double( INT32 index, DOUBLE in )
{
	insert( index );
	set_double( index, in );
}

void	c_data_line::insert_str( INT32 index, C_PCHAR_C in )
{
	insert (index );
	set_str( index, in );
}
