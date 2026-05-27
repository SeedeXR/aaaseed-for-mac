#include "dir_pool.h"
#include "aaa_dir.h"
#include "infrastructure/param/param_declare.h"


static INT32 CONSTEXPR DIR_POOL_NB_MAX	= 12;

FACTORY_CREATE_V1( c_dir_pool, dir_pool, Directory pool, dir_pool );

#define PARAM_DEF_DIR( letter ) \
		PARAM_DEF_REF_UPDATE_FN(	dir_##letter##_symbol			,c_dir_pool::update_cur	)	\
		PARAM_DEF_REF_UPDATE_FN(	dir_##letter##_path				,c_dir_pool::update_cur	)	\
		PARAM_DEF_STR_LOCKED(		dir_##letter##_path_absolute	)

namespace n_dir_pool
{
	CONSTEXPR INT32 PARAM_NB_MAX	=	DIR_POOL_NB_MAX * 3;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_DIR(	a	)
		PARAM_DEF_DIR(	b	)
		PARAM_DEF_DIR(	c	)
		PARAM_DEF_DIR(	d	)
		PARAM_DEF_DIR(	e	)
		PARAM_DEF_DIR(	f	)
		PARAM_DEF_DIR(	g	)
		PARAM_DEF_DIR(	h	)
		PARAM_DEF_DIR(	i	)
		PARAM_DEF_DIR(	j	)
		PARAM_DEF_DIR(	k	)
		PARAM_DEF_DIR(	l	)
	};
}

void	c_dir_pool::param_init_pt_static()
{
	INT32	h = 0;

	for( INT32 i=0; i<_nb; ++i )
	{
		param_set_pt( h, _symbol[i]			);
		param_set_pt( h, _path[i]			);
		param_set_pt( h, _path_absolute[i]	);
	}

	err_param_init_pt( h );
}


CONSTRUCTOR_CREATE(c_dir_pool)
{
	_index_used		.resize(DIR_POOL_NB_MAX);

	_symbol			.resize(DIR_POOL_NB_MAX);
	_path_absolute	.resize(DIR_POOL_NB_MAX);
	_path			.resize(DIR_POOL_NB_MAX);

	_nb = DIR_POOL_NB_MAX;

	param_init_with( n_dir_pool::param, n_dir_pool::PARAM_NB_MAX ); 
}

c_dir_pool::~c_dir_pool()
{
	_index_used		.clear();

	_path			.clear();
	_path_absolute	.clear();
	_symbol			.clear();

	_nb = 0;
}

void c_dir_pool::update_cur( c_param* param )
{
	cur->update();
}

//todo we can't change add or remove on the fly
AAA_ERR	c_dir_pool::load_do_after( o_str CONST & filename_in )
{
	update();
	return AAA_OK;
}

AAA_ERR c_dir_pool::set_raw( C_PCHAR_C symbol, o_str CONST & path, INT32 index )
{
	INT32 index_free = -42;
	if( index < 0 ) //we need to find it
	{ 
		INT32 len = INT32(strlen(symbol));
		INT32 nb = (INT32)_index_used.size();
		for( INT32 i=0; i<nb; ++i )
		{
			if( _symbol[i].is_str_equal( symbol, len ) )
			{
				index = i;
				break;
			}
			if( index_free < 0 )
			{
				if( _symbol[i].is_empty() )
					index_free = i;
			}
		}
	}
	if( index < 0 && index_free >=0 )
		index = index_free;
	if( index >= 0 )
	{
		 _symbol[index].set( symbol );
		 _path[index].set( path );
		 return AAA_OK;
	}
	return ERR_OUT_OF_BOUND;
}

AAA_ERR c_dir_pool::set( C_PCHAR_C symbol, o_str CONST & path, INT32 index )
{
	AAA_ERR retcode = set_raw( symbol, path, index );
	if( NOERR(retcode) )
		update();
	return retcode;
}

void c_dir_pool::update()
{
#if 0 // debug stuff
	DBG_PRINT_STRING( "dir start : %s", c_file::dir_get_start()	.get() );
	DBG_PRINT_STRING( "dir def   : %s", c_file::dir_get_def()	.get() );
	DBG_PRINT_STRING( "dir cur   : %s", c_file::dir_get_cur()	.get() );
	DBG_PRINT_STRING( "%s()", __FUNCTION__ );
#endif

	_index_used.clear();
	for( INT32 i = 0; i<_nb; ++i )
	{
		o_str& o = _path[i];
		if( o.is_empty() )
		{
			_path_absolute[i].erase();
		}
		else
		{
			AAA_ERR err = c_dir::push_def( o );
			if( NOERR(err) )
			{
				//DBG_PRINT_STRING( "%d -> dir cur now is  : %s", i, c_file::dir_get_cur().get() );
				_path_absolute[i].set( c_dir::get_cur() );
				_index_used.push_back(i);
			}
			c_dir::pop_def();
		}
	}
}

void c_dir_pool::expand_fname( o_str& dst, C_PCHAR_C src )
{
	if( *src == '%' )
	{
		INT32 nb = (INT32)_index_used.size();
		for( INT32 i=0; i<nb; ++i )
		{
			INT32	index	= _index_used[i];
			o_str&	symbol	= _symbol[index];
			INT32	len		= symbol.get_len();

			if( *(src+1+len)=='%' && str_is_equal_nocase( src + 1, symbol.get(), len ) )
			{
				dst.set( _path_absolute[index] );
				dst.add( src + len + 2 );
				return;
			}
		}
		ERR_PRINT_STRING( "%s() Can't find dir_name to substitute in %s", __FUNCTION__, src );
	}
	dst.set( src );
}

void c_dir_pool::expand_fname( o_str& dst, o_str CONST & src )
{
	if( &dst == &src )
	{
		debug_break( "%s() with same src and dst : skipping", __FUNCTION__ );
		return;
	}
	expand_fname( dst, src.get() );	//todo 2024 Sep will do better later
}

void c_dir_pool::compact_fname( char* &dst, o_str CONST & src )
{
	//todo do a test here to be sure src and dst don't overlap
	C_PCHAR pt = src.get();
	for( INT32 i=INT32(_index_used.size())-1; i>=0; --i )
	{
		INT32	index	= _index_used[i];
		INT32	len		= 0;

		if( src.is_starting_with( _path_absolute[index] ) )
			len = _path_absolute[index].get_len();
		else if( src.is_starting_with( _path[index] ) )
			len = _path[index].get_len();

		if( len )
		{
			o_str CONST & symbol = _symbol[index];
			*dst++ = '%';
			strcpy( dst, symbol.get() );
			dst += symbol.get_len();
			*dst++ = '%';
			pt += len;
			break;
		}
	}
	str_add_escape_for_quote( dst, pt );
}

void c_dir_pool::compact_fname( o_str & dst, o_str CONST & src )
{
	if( &dst == &src )
	{
		debug_break( "%s() with same src and dst : skipping", __FUNCTION__ );
		return;
	}

	C_PCHAR pt = src.get();
	for( INT32 i=INT32(_index_used.size())-1; i>=0; --i )
	{
		INT32 index	= _index_used[i];
		INT32 len	= 0;

		if( src.is_starting_with( _path_absolute[index] ) )
			len = _path_absolute[index].get_len();
		else if( src.is_starting_with( _path[index] ) )
			len = _path[index].get_len();

		if( len )
		{
			o_str CONST & symbol = _symbol[index];
			dst.set_char( '%' );
			dst.add( symbol );
			dst.add_char( '%' );
			pt += len;
			break;
		}
	}
	dst.add( pt );	//todo 2024 Sep Should we do escape for quote like the Char version or change the char version
}

c_dir_pool*	c_dir_pool::cur	= nullptr;