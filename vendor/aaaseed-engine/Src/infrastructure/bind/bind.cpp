#include "aaa_def.h"
#include "infrastructure/obj/root.h"
#include "infrastructure/bind/bind.h"
#include "strnum.h"
#include "ui/aaa_menu.h"
#include "infrastructure/param/param_declare.h"
#include "file/aaa_dir.h"


FACTORY_ABSTRACT_CREATE_V1( c_bind_abstract, bind_abstract, Bind Abstract);

//hack this should be dynamic
namespace
{
	CONSTEXPR INT32 BIND_MAX_NB	= 32 * 1024 * 2;
	CONSTEXPR INT32 MENU_SUB_MAX_NB	= 64;
	static	c_param_def*	bind_param;
}

//todo clear up the order of initialization
//todo if bind get too big the param struct is too much (menu too)
//			do something else

static	void	c_bind_init()
{
	bind_param = new c_param_def[BIND_MAX_NB];
	CHAR str_base[] = "Bind_xxxxxxx";
	c_param_def	bind_param_ref( TYPE_FILENAME,	"", 0, 0,	0, 0 );	// real type and other details will be forced later before creating param in c_bind_abstract::set()

	if( BIND_MAX_NB > 10000000 )
		ERR_PRINT_STRING( "c_bind_init() BIND_MAX_NB should not be more than 10000000.");
		
	for( UINT32 i = 0; i < BIND_MAX_NB; ++i )
	{
		INT32		nb;
		if ( i < 100 )
			nb = 2;
		else if ( i < 1000 )
			nb = 3;
		else if ( i < 10000 )
			nb = 4;
		else if ( i < 100000 )
			nb = 5;
		else if ( i < 1000000 )
			nb = 6;
		else
			nb = 7;
		strnum::make( str_base + 5, nb, i );
		*( str_base + 5 + nb ) = 0;
		bind_param[i] = bind_param_ref;
		bind_param[i].set_name( str_base );
	}
}

static	void	c_bind_deinit()
{
	SAFE_DELETE_ARRAY( bind_param );
}

void	c_bind_abstract::alloc( INT32 CONST nb, INT32 CONST menu_sub_nb )
{
	if( nb < 0 || nb > BIND_MAX_NB )
		box_err( "Can't allocate %d item in a bind.\nMaximum is %d for now\nYou should exit.", nb, BIND_MAX_NB );
	else
	{
		//		DBG_PRINT_STRING( "c_list_fix::alloc()");
		//		cont.reserve(nb);
		//		for( INT32 i=0; i<nb; ++i )
		//			cont.push_back( &str_array[i] );
		_cont.resize( nb );
		_index_nb = nb;
	}
	_menu_sub_nb = MIN( menu_sub_nb, MENU_SUB_MAX_NB );
	_menu_sub.resize( _menu_sub_nb, 0 );
}

void	c_bind_abstract::dealloc()
{
	//	if ( str_array)
	//		delete [] str_array;
	//	str_array = nullptr;
	_cont.clear();
	_menu_sub.clear();
	obj_delete( _menu_sub_name );
}

CONSTRUCTOR_ABSTRACT_CREATE(c_bind_abstract)
,_index_cur					{0}
,_index_for_next_load_save	{-42}
,_index_nb					{0}
,_base_id					{0}
,_b_draw_number				{false}
,_b_draw_number_continuous	{false}
,_b_file_relative			{false}
,_menu_sub_name				{nullptr}
,_menu_id					{0}
,_menu_sub_item_max			{0}
,_menu_sub_nb				{0}
{
	if( is_obj_first() )
		c_bind_init();
	//	str_array = nullptr;
}

c_bind_abstract::~c_bind_abstract()
{
	dealloc();
	if( is_obj_first() )
		c_bind_deinit();
}

//hack just done to follow the constructor
AAA_ERR	c_bind_abstract::set(	INT32 CONST			nb,
								C_PCHAR_C			name,
								C_PCHAR_C			ext,
								aaa::file::TYPE_IO	type_io,
								INT32 CONST			menu_sub_nb,
								bool CONST			b_draw_number,
								bool CONST			b_draw_number_continuous,
								bool CONST			b_file_relative
						)
{
	set_name( name );
	set_fname_ext( ext );
	
	alloc( nb, menu_sub_nb );
	// now that the bind_param arwe allocated and have the right names we can force the type
	param_init_with( bind_param, nb );
	//we have to do this after and on Param because of what is stored in the factoiry object 
	if( type_io != aaa::file::TYPE_IO_NONE )	//create param_data stuff so avoid if possible
	{
		//TYPE CONST type = (type_io == aaa::file::TYPE_IO_NONE) ? TYPE_STR : TYPE_FILENAME;
		TYPE CONST type = TYPE_FILENAME;
		REAL value_def = REAL(type_io);
		for( INT32 i=0; i<nb; ++i )
		{
			c_param * CONST param = get_param(i);
			if( type != TYPE_FILENAME )
				param->set_type( type );
			if( value_def != 0. ) 
				param->set_def( value_def );
		}
	}

	_menu_sub_item_max = nb / menu_sub_nb;
	if( menu_sub_nb*_menu_sub_item_max != nb )	//todoqqq deal with odd number case
	{
		box_err( "c_bind_abstract::set can't deal with these numbers" );
		debug_break( "c_bind_abstract::set can't deal with these numbers" );
	}
	if( _menu_sub_nb > 1 )
	{
		_menu_sub_name = obj_get( _menu_sub_name );
		_menu_sub_name->set_root( this );	//hack c_bind belongs to node_bind by c_bind constructor

		o_str& sub_name = o_str::push_name( "Sub Menu " );
			sub_name.add( get_name_str() );
			_menu_sub_name->set( menu_sub_nb, sub_name.get(), nullptr, aaa::file::TYPE_IO_NONE );
		o_str::pop_name();
	}
	else
		_menu_sub_name = nullptr;

	_b_draw_number = b_draw_number;
	_b_draw_number_continuous = b_draw_number_continuous;
	_b_file_relative = b_file_relative;
	
	return AAA_OK;	//hack
}

void	c_bind_abstract::param_init_pt()
{
	INT32	h = 0;
	while( h < _index_nb )
		param_set_pt( h, &_cont[h]);
	err_param_init_pt( h );
}

//todoqq replace the %.64s by a real fn
INT32	c_bind_abstract::menu_build( INT32 CONST base_id, PT_MENU_FN menu_fn )
{
	_base_id = base_id;

	if( _menu_id )
	{
		CONSTEXPR C_PCHAR_C mess = "c_bind_abstract::menu_build(): menu already built";
		debug_break( mess );
		box_err( mess );
	}
	else
	{
		CHAR	title[256];
		INT32	index = 0;

		if( _menu_sub_nb > 1 )
		{
			for( INT32 j = 0; j < _menu_sub_nb; ++j )
			{
				_menu_sub[j] = menu::create(menu_fn);
				for( INT32 i = 0; i < _menu_sub_item_max; ++i )
				{
					if( _b_draw_number )
						sprintf( title, "%2d   %.64s", _b_draw_number_continuous?index:i, get_str(index) );	//todo in all these fns add... when too long
					else
						sprintf( title, "%.64s", get_str(index) );
					menu::add_item( title, _base_id+index );
					++index;
				}
			}
			_menu_id = menu::create(menu_fn);
			menu::set_cur( _menu_id, __FUNCTION__ );
			for( INT32 j = 0; j < _menu_sub_nb; ++j )
			{
				if( _menu_sub_name )
				{
					if( _b_draw_number )
						sprintf( title, "%2d   %.64s", j, _menu_sub_name->get_str(j) );
					else
						sprintf( title, "%.64s", _menu_sub_name->get_str(j) );
				}
				else
					sprintf( title, "Bank %d", j);
				menu::add_menu_sub( title, _menu_sub[j]);
			}
		}
		else
		{
			_menu_id = menu::create(menu_fn);
			menu::set_cur( _menu_id, __FUNCTION__ );
			for( INT32 i = 0; i < _menu_sub_item_max; ++i )
			{
				menu::add_item( get_str(index), _base_id+index );
				++index;
			}
		}
	}
	return _menu_id;
}

FINLINE	INT32	c_bind_abstract::get_menu_id(INT32 index)
{
	if( _menu_sub_nb == 1 )
		return _menu_id;

	index = MIN( index / _menu_sub_item_max, MENU_SUB_MAX_NB-1 );
	return _menu_sub[index];
}

FINLINE	INT32	c_bind_abstract::get_menu_index( INT32 CONST index )
{
	if( _menu_sub_nb == 1 )
		return index;

	return IMOD( index, _menu_sub_item_max );
}

void	c_bind_abstract::menu_item_set( INT32 CONST index, C_PCHAR_C title, bool CONST flag )
{
#if	!AAA_MENU_LOCKED()
	if( _menu_id && !menu::is_on() )
	{
		CHAR	str[256];
		menu::set_cur( get_menu_id( index ), __FUNCTION__ );
		/*
			strcpy( err_str, flag?"->\t":"  \t" );
			fname::cpy_fname_pure( err_str+3, get_str(index) );
			menu::change_item( index+1, err_str, base_id+index );
		*/
		if( _b_draw_number )
			sprintf( str, "%2d   %.64s", _b_draw_number_continuous?index:IMOD( index, _menu_sub_item_max ), title );
		else
			sprintf( str, "%.64s", title );
		menu::change_item( get_menu_index( index ) +1 , str, _base_id + index, flag ? 1 : 0 );
	}
#endif
}

void	c_bind_abstract::menu_item_set_before_after( INT32 CONST index,  C_PCHAR_C before,  C_PCHAR_C after, bool CONST flag )
{
	if( _menu_id && !menu::is_on() )
	{
		CHAR	str[256];
		sprintf( str, "%.64s%.64s%.64s", before ? before : "", get_str(index), after ? after : "" );
		menu_item_set( index, str, flag );
	}
}

void	c_bind_abstract::menu_item_make_active( INT32 CONST index, bool CONST flag )
{
	if( _menu_id && !menu::is_on() )
	{
		CHAR	str[256];
		if ( _b_file_relative )
			fname::cpy_fname_relative( str, get_str(index), c_dir::get_def().get() );
		else
			sprintf( str, "%.64s", get_str(index) );
		menu_item_set( index, str, flag );
	}
}

void	c_bind_abstract::set_item( INT32 index, o_str CONST & str )
{
	index = IMOD( index, _index_nb );
	_cont[index].set( str );
	menu_item_make_active( index, index == _index_cur );
}

void	c_bind_abstract::clear_item( INT32 index )
{
	index = IMOD( index, _index_nb );
	_cont[index].erase();
	menu_item_make_active( index, index == _index_cur );
}

void	c_bind_abstract::swap_item( INT32 CONST dst, INT32 CONST src )
{
	SWAP( _cont[src], _cont[dst] );
	/*
	o_str*	pstr;
	pstr = cont[src];
	cont.erase( cont.begin() + src );
	if( dst <= src )
		cont.insert( cont.begin() + dst, pstr );
	else
		cont.insert( cont.begin() + dst, pstr );
	*/
	menu_item_make_active( src, _index_cur == src );
	menu_item_make_active( dst, _index_cur == dst );
	//	menu_item_make_active( index, index == index_cur );
}

bool	c_bind_abstract::swap_item( c_param CONST * CONST param, INT32 CONST inc )
{
	INT32 src = get_param_index( param );
	if( src >= 0 )
	{
		INT32 dst = src + inc;
		if( 0 <= dst && dst < get_elt_nb() )
		{
			swap_item( src, dst );
			return true;
		}
	}
	return false;
}

//todoqq	it's ok but a real Pb for the img_bind
AAA_ERR	c_bind_abstract::load_do_after( o_str CONST & filename_in )
{	
#if	!AAA_MENU_LOCKED()
	if( _menu_id )	//avoid crash when menu not build before loading
	{
		//INT32	i;
		//	load menu name from file if there is one
		if( _menu_sub_name && !filename_in.is_empty() )
		{
			o_str& filename = o_str::push_name( filename_in );
				filename.add_ext( "name" );
				_menu_sub_name->load_from_file( filename );
			o_str::pop_name();

			CHAR	str[256];
			menu::set_cur( _menu_id, __FUNCTION__ );
			for( INT32 i = 0; i < _menu_sub_nb; ++i )
			{
				C_PCHAR tmp_str = _menu_sub_name ? _menu_sub_name->get_str(i) : "";
				if( _b_draw_number )
				{
					sprintf( str, "%2d   %.64s", i, tmp_str );
					tmp_str = str;
				}
				menu::change_menu_sub( i+1, tmp_str, _menu_sub[i]);
			}
		}
		for( INT32 i = 0; i < _index_nb; ++i )
			menu_item_make_active( i, false );
	}
	set_index_cur( _index_cur );
#endif
	return	AAA_OK;
}

void	c_bind_abstract::set_index_cur( INT32 CONST index )
{
	menu_item_make_active( _index_cur, false );
	_index_cur = index;
	menu_item_make_active( _index_cur, true );
}

INT32	c_bind_abstract::find( C_PCHAR_C pat ) CONST
{
	for( INT32 i = 0; i < _index_nb; ++i )
	{
		if( str_is_equal_bothnocase( get_str(i), pat ) )	//todo use o_str
			return	i;
	}
	return	-1;
}
void	c_bind_abstract::set_index_for_next_load_save( INT32 CONST index )
{
	_index_for_next_load_save = index;
}
INT32	c_bind_abstract::get_reset_index_for_next_load_save()
{
	INT32 index = _index_for_next_load_save;
	_index_for_next_load_save = -42;
	return index;
}


FACTORY_CREATE_V1( c_bind, bind, Bind, bind );

CONSTRUCTOR_CREATE( c_bind )
{
}

c_node_ui* & node_bind()
{
	static c_node_ui* node_bind_trick = nullptr;
	if( !node_bind_trick )
		obj_new( node_bind_trick )->set_name( "NODE_BIND" );
	return node_bind_trick;
}

void delete_node_bind()
{
	auto& node = node_bind();
	delete node;
	node = nullptr;
}

c_bind::~c_bind()
{
	if( is_obj_first() )
		delete_node_bind();
}



c_bind* c_bind::get_new( C_PCHAR_C name_symbo )
{
	c_bind*	bind = nullptr;
	node_bind()->obj_get( bind );
	bind->set_name_symbo( name_symbo );
	return bind;
}
