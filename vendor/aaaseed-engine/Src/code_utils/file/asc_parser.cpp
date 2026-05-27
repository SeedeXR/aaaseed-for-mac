#include "file/asc_parser.h"
#include <stdlib.h>

void	c_asc_parser::set_start( CHAR* CONST start )
{
	_cur = start;
	_word = _cur;
}

CHAR*	c_asc_parser::get_rest()	CONST	{	return	(CHAR*)_cur;	}
CHAR*	c_asc_parser::get_word()	CONST	{	return	(CHAR*)_word;	}
bool	c_asc_parser::is_word()		CONST	{	return	_b_word;		}


bool	c_asc_parser::search_word_begin()
{
	while( *_cur )
	{
		if( *_cur > 32 )
		{
			_word = _cur;
			++_cur;
			return true;
		}
		else
		{
			if( *_cur == '\n' )
				break;
		}
		++_cur;
	}
	return false;
}

bool	c_asc_parser::search_word_end()
{
	_b_word = false;
	while( *_cur )
	{
		if( *_cur <= 32 )
		{
			*_cur = 0;
			++_cur;
			return true;
		}
		else
		{
			if( *_cur == '\n' )
				break;
		}
		++_cur;
	}
	return _word!=_cur;
}

bool	c_asc_parser_cstring::search_word_end()
{
	if( *_word == '\"' )
	{
		CHAR*	dst = _cur;

		_b_word = true;
		++_word;
		while( *_cur && *_cur != '\n' )
		{
			if( *_cur == '\"' )
			{
				if( *(_cur-1) == '\\' )
					--dst;
				else
				{
					*dst = 0;
					*_cur = 0;
					++_cur;
					return true;
				}
			}
			*dst = *_cur;
			++dst;
			++_cur;
		}
	}
	else
	{
		_b_word = false;
		while( *_cur )
		{
			if( *_cur <= 32 )
			{
				*_cur = 0;
				++_cur;
				return true;
			}
			++_cur;
		}
	}
	return _word != _cur;
}

CHAR*	c_asc_parser::get_next_word()
{
	if( search_word_begin() )
	{
		if( search_word_end() )
		{
			return _word;
		}
	}
	return nullptr;
}

bool	c_asc_parser::get_next_int( INT32 &i )
{
	if( get_next_word() )
	{
		i = atol( (CHAR*)_word );
		return true;
	}
	return false;
}


bool	c_asc_parser::get_next_double( DOUBLE &r )
{
	if( get_next_word() )
	{
		r = atof( (CHAR*)_word );
		return true;
	}
	return false;
}