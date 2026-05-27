#include "aaa_def.h"
#include "list_fix.h"
#include "strnum.h"
#include "ui/aaa_menu.h"
#include "infrastructure/param/param.h"

FACTORY_CREATE_V1( c_list_fix, list_fix, Bind list, list_fix );

//hack this should be dynamic
static	CONST INT32		LIST_FIX_MAX_NB	= 2048;
static	CHAR			list_fix_param_name[LIST_FIX_MAX_NB][10];
static	c_param			list_fix_param[LIST_FIX_MAX_NB];

//todo clear up the order of initialization

void	list_fix_init()
{
	CHAR	str[10];
	INT32	nb;

	strcpy( str, "Bind_");
	for( INT32 i = 0; i < LIST_FIX_MAX_NB; i++ )
	{
		if( i<100 )
			nb = 2;
		else if( i<1000 )
			nb = 3;
		else
			nb = 4;
		strnum::make( str+5, nb, i);
		*(str+5+nb) = 0;
		strcpy( list_fix_param_name[i], str);
		//hack because of PARAM_FILENAME now the list can't be edited 
		list_fix_param[i].set( NULL, PARAM_FILENAME, list_fix_param_name[i], 0, 0, 0, 0 );
	}
}


void	c_list_fix::alloc( INT32 nb, INT32 menu_sub_nb )
{
	if( nb < 0 || nb > LIST_FIX_MAX_NB )
	{
		BOX_ERR( "Can't allocate %d item in a list_fix.\nYou should exit.", nb);
	}
	else
	{
//		DBG_PRINT_STRING( "c_list_fix::alloc()");
//		cont.reserve(nb);
//		for( INT32 i=0; i<nb; i++)
//			cont.push_back( &str_array[i] );
		_cont.resize(nb);
		index_max_ = nb;
	}
	menu_sub_nb_ = menu_sub_nb;
	_menu_sub.resize( menu_sub_nb_, 0);
}

void	c_list_fix::dealloc()
{
//	if ( str_array)
//		delete [] str_array;
//	str_array = NULL;
	_cont.clear();
	_menu_sub.clear();
	obj_delete( menu_sub_name_ );
}

CONSTRUCTOR_CREATE(c_list_fix)
{
	if( _factory->is_obj_first() )
		list_fix_init();

//	str_array = NULL;
	menu_id_ = 0;
	index_cur_ = 0;
}

c_list_fix::~c_list_fix()
{
	dealloc();
}

//hack just done to follow the constructor
void	c_list_fix::set(	CHAR*	name,
							INT32	nb,
							INT32	menu_sub_nb,
							bool	b_draw_number,
							bool	b_file_relative
						)
{
	set_name( name);
	alloc( nb, menu_sub_nb);
	set_param( list_fix_param, index_max_ );

	menu_sub_item_max_ = index_max_/menu_sub_nb_;
	if( menu_sub_nb_*menu_sub_item_max_ != index_max_ )	//todo deal with odd number case
	{
		BOX_ERR( "c_list_fix::c_list_fix can't deal with these numbers" );
		debug_break( "c_list_fix::c_list_fix can't deal with these numbers" );
	}
	if( menu_sub_nb_ > 1 )
	{
		menu_sub_name_ = new c_list_fix;
		menu_sub_name_->set( "Sub Menu Bind", menu_sub_nb );
	}
	else
		menu_sub_name_ = NULL;

	_b_draw_number = b_draw_number;
	_b_file_relative = b_file_relative;
}

void	c_list_fix::param_init_pt()
{
	for( INT32 i = 0; i < index_max_; i++ )
	{
		list_fix_param[i].set_pt( &_cont[i] );
	}
}

//todoqq replace the %.64s by a real fn
INT32	c_list_fix::menu_build( INT32 base_id, PT_MENU_FN menu_fn )
{
	_base_id = base_id;

	if( menu_id_ )
	{
		CONSTEXPR CHAR * mess = "c_list_fix::menu_build(): menu already built";
		debug_break( mess );
		BOX_ERR( mess );
	}
	else
	{
		CHAR	title[256];
		INT32	index = 0;

		if( menu_sub_nb_ > 1)
		{
			for( INT32 j = 0; j < menu_sub_nb_; j++ )
			{
				_menu_sub[j] = menu::create(menu_fn);
				for( INT32 i = 0; i < menu_sub_item_max_; i++ )
				{
					if( _b_draw_number )
						sprintf( title, "%2d   %.64s", i, get_str(index) );	//todo in all these fns add... when too long
					else
						sprintf( title, "%.64s", get_str(index) );
					menu::add_item( title, _base_id+index );
					index++;
				}
			}
			menu_id_ = menu::create(menu_fn);
			menu::set_cur( menu_id_ );
			for( INT32 j = 0; j < menu_sub_nb_; j++ )
			{
				if( menu_sub_name_ )
				{
					if( _b_draw_number )
						sprintf( title, "%2d   %.64s", j, menu_sub_name_->get_str(j) );
					else
						sprintf( title, "%.64s", menu_sub_name_->get_str(j) );
				}
				else
					sprintf( title, "Bank %d", j);
				menu::add_menu_sub( title, _menu_sub[j]);
			}
		}
		else
		{
			menu_id_ = menu::create(menu_fn);
			menu::set_cur( menu_id_ );
			for( INT32 i = 0; i < menu_sub_item_max_; i++ )
			{
				menu::add_item( get_str(index), _base_id+index );
				index++;
			}
		}
	}
	return menu_id_;
}

FINLINE	INT32	c_list_fix::get_menu_id(INT32 index)
{
	if( menu_sub_nb_ == 1 )
		return menu_id_;
	else
		return _menu_sub[index/menu_sub_item_max_];
}

FINLINE	INT32	c_list_fix::get_menu_index(INT32 index)
{
	if( menu_sub_nb_ == 1 )
		return index;
	else
		return IMOD(index,menu_sub_item_max_ );
}

void	c_list_fix::menu_item_set( CHAR* title, INT32 index, BOOL flag )
{
#if	!MENU_LOCKED
	if( menu_id_ && !b_menu_on )
	{
		CHAR	str[256];
		menu::set_cur( get_menu_id(index) );
/*		strcpy( err_str, flag?"->\t":"  \t" );
		fname::get_pure_name( err_str+3, get_str(index) );
		menu::change_item( index+1, err_str, base_id+index );
*/
		if( _b_draw_number )
			sprintf( str, "%2d   %.64s", IMOD(index, menu_sub_item_max_ ), title );
		else
			sprintf( str, "%.64s", title );
		menu::change_item( get_menu_index(index)+1, str, _base_id+index, flag?1:0 );
	}
#endif
}

void	c_list_fix::menu_item_set_before_after( INT32 index, CHAR* before, CHAR* after, BOOL flag )
{
	if( menu_id_ && !b_menu_on )
	{
		CHAR	str[256];
		sprintf( str, "%.64s%.64s%.64s", before, get_str(index), after );
		menu_item_set( str, index, flag );
	}
}

void	c_list_fix::menu_item_make_active( INT32 index, BOOL flag )
{
	if( menu_id_ && !b_menu_on )
	{
		CHAR	str[256];

		if( _b_file_relative )
			fname::get_rel_name( str, get_str(index) );
		else
			sprintf( str, "%.64s", get_str(index) );
		menu_item_set( str, index, flag );
	}
}


void	c_list_fix::set_item( INT32 index, CHAR* str )
{
	index = IMOD( index, index_max_ );
	_cont[index].set( str );
	menu_item_make_active( index, index == index_cur_ );
}

void	c_list_fix::swap_item( INT32 dst, INT32 src )
{
	SWAP( _cont[src], _cont[dst] );
/*
	o_str*	pstr = cont[src];
	cont.erase( cont.begin() + src );
	if( dst <= src )
		cont.insert( cont.begin()+dst, pstr );
	else
		cont.insert( cont.begin()+dst, pstr );
*/
	menu_item_make_active( src, index_cur_==src );
	menu_item_make_active( dst, index_cur_==dst );
	//	menu_item_make_active( index, index == index_cur );
}

//todoqq	it's ok but a real Pb for the img_bind
AAA_ERR	c_list_fix::load_do_after( C_PCHAR_C filename_in)
{	
#if	!MENU_LOCKED
	if( menu_id_ )	//avoid crash when menu not build before loading
	{
		//	load menu name from file if there is one
		if( menu_sub_name_ && filename_in )
		{
			CHAR	filename[_MAX_PATH];
			CHAR	str[256];

			fname::add_ext( filename, filename_in, "name" );
			menu_sub_name_->load_from_file(filename);
			menu::set_cur( menu_id_ );
			for( INT32 i = 0; i < menu_sub_nb_; i++ )
			{
				CHAR* tmp_str = menu_sub_name_?menu_sub_name_->get_str(i):"";
				if( _b_draw_number )
				{
					sprintf( str, "%2d   %.64s", i, tmp_str );
					tmp_str = str;
				}
				menu::change_menu_sub( i+1, tmp_str, _menu_sub[i]);
			}
		}
		for( INT32 i = 0; i < index_max_; i++ )
			menu_item_make_active( i, FALSE);
	}
	set_index_cur( index_cur_ );
#endif
	return	AAA_OK;
}

void	c_list_fix::set_index_cur( INT32 index)
{
	menu_item_make_active( index_cur_, FALSE );
	index_cur_ = index;
	menu_item_make_active( index_cur_, TRUE );
}

INT32	c_list_fix::find( CHAR* pat)
{
	for( INT32 i = 0; i < index_max_; i++ )
	{
		if( str_is_equal_bothnocase( get_str(i), pat ) )	
			return	i;
	}
	return	-1;
}

