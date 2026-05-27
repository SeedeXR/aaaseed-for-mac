#include "file_virtual.h"
#include "infrastructure/layer/layers.h"
#include "spy.h"
#include "file_io.h"
#include "obj_ui/bdd/util/bdd.h"
#include "obj_ui/bdd/util/factory_group_bdd.h"

//to we assume one vfile at a time only for now
namespace {
	//todo make it dynamic
	INT32 bdd_factory_array_len = 0;
	CONSTEXPR INT32 BDD_FACTORIES_NB = c_layers::LAYER_NB_MAX;
	//we use a vector before but we want to avoid overweight
	c_factory_base** bdd_factories[BDD_FACTORIES_NB];	// LAYER_NB_MAX to cover files fx_a to fx_z
	INT32 bdd_factories_count[BDD_FACTORIES_NB];

	c_file_io						reader;
	std::deque<bool>				stack_active;

	void init_bdd_factories()
	{
		bdd_factory_array_len = 64;
		for( INT32 i=0; i<BDD_FACTORIES_NB; ++i )
			bdd_factories[i] = (c_factory_base**) MALLOC( sizeof(c_factory_base*) * (bdd_factory_array_len+1) );	// +1 for nullptr at the end
	}
	void open_bdd_factories()
	{
		if( !bdd_factory_array_len )
			init_bdd_factories();
		for( INT32 i=0; i<BDD_FACTORIES_NB; ++i )
			bdd_factories_count[i] = 0;
	}
	void close_bdd_factories()
	{
		for( INT32 i=0; i<BDD_FACTORIES_NB; ++i )
			*(bdd_factories[i] + bdd_factories_count[i]) = nullptr;
	}
}

c_file_virtual::V_FILE_MAP		file_map;

bool	c_file_virtual::b_filename_relative	= false;	//	first version was not perfectly relative
bool	c_file_virtual::b_reading			= false;
bool	c_file_virtual::b_active			= false;
o_str	c_file_virtual::dirname;
FILE*	c_file_virtual::file_cur			= nullptr;
	 
c_factory_base ** c_file_virtual::get_bdd_factories( INT32 CONST index )
{
	return bdd_factories[index];
}
static c_file_virtual::st_vfile	st_vfile_null = { nullptr, 0 };

c_file_virtual::st_vfile CONST & c_file_virtual::get_data( C_PCHAR_C key )
{
	V_FILE_MAP::const_iterator it;
	if( b_filename_relative )
	{
		o_str& name = o_str::push_name();
			name.set_fname_relative( key, dirname );
			it = file_map.find( (C_PCHAR)name.get() );
		o_str::pop_name();
	}
	else
		it = file_map.find( (C_PCHAR)key );
		
	if( it != file_map.end() )
		return it->second;
	return st_vfile_null;
}

void c_file_virtual::open_vfile_save( o_str CONST & filename, C_PCHAR_C mode )
{
	SPY_PUSH_RANGE2( "vfile load :", spy::FILE_HIGH, filename );

		file_cur = c_file::FOPEN( filename, mode );
		if( file_cur )
		{
			set_active( true );
			dirname.set_dir_name( filename );
			fprintf( file_cur, "//AAA::set_filename_relative\n" );
		}
		else
		{
			ERR_PRINT_STRING( "can't open virtual file %s to save, using separate file instead.", filename );
			set_active( false );
		}

	SPY_POP_RANGE2();
}

void	c_file_virtual::open_vfile_load( o_str CONST & filename, C_PCHAR_C mode )
{
	SPY_PUSH_RANGE2( "vfile load :", spy::FILE_HIGH, filename );

		file_map.clear();
		b_filename_relative = false;
		open_bdd_factories();

		if( ERR(reader.read_file( filename )) )
		{
			ERR_PRINT_STRING( "no virtual file %s, using separate file instead.", filename.get() );
			set_active( false );
			file_cur = nullptr;
		}
		else
		//build the file_map
		//if( reader.move_before( "//AAA::") )
		{
			dirname.set_dir_name( filename );
			while( reader.move_after("//AAA::") )
			{
				C_PCHAR	keyword = reader.get_next_str();
				if( strcmp( keyword, "filename" )==0 )
				{
					C_PCHAR_C name = (C_PCHAR_C) reader.get_data_pt( 0 );	//start of name
					if( reader.skip_until_control() )
					{
						*reader.get_data_pt( 1 ) = 0;	//0 at the end and skip it
						//DBG_PRINT_STRING( "file %s", name );
						if( reader.skip_space_or_less() )
						{
							auto found = file_map.find(name);
							if( found != file_map.end() )
							{
								debug_break( "duplicate \"%.512s\" in vfile dir \"%.512s\" this should not happen: skipping this one", name, dirname.get() );
							}
							else
							{
								UINT8 * CONST data = reader.get_data_pt( 0 );	//start of data
								reader.move_before( "//AAA::" );
								UINT8 * CONST end = reader.get_data_pt( 0 );
								*(end-1) = 0;	//0 at the end
								//DBG_PRINT_STRING( "data %s", data );
								file_map[name] = { data, UINT32(end - data) };	//todo with no cast message was conversion from 'int' to 'UINT32' requires a narrowing conversion
								if( name[0] == 'f' && name[1] == 'x' && name[2] == '_' && name[4] == '.' )
								{
									C_PCHAR_C ext = fname::get_ext( name );
									c_factory_base * CONST pf = c_bdd::factory_group->get_from_file_extension(ext);
									if( pf )	// yep this is a bdd
									{
										INT32 index = name[3] - 97;
										INT32 count = bdd_factories_count[index];
										if( count < bdd_factory_array_len )
										{
											*(bdd_factories[index] + count) = pf;
											bdd_factories_count[index] = ++count;
										}
										else
										{
											ERR_PRINT_STRING( "virtual file system support only 64 bdds by layers:" );
											ERR_PRINT_STRING( "ignoring %s in %512d", pf->get_class_name(), filename );
										}
										//bdd_factories[index].push_back( pf );
									}
								}
							}
						}
					}
				}
				else if( strcmp( keyword, "set_filename_relative" )==0 )
				{
					b_filename_relative = true;
				}
				else
					ERR_PRINT_STRING( "%s() found unknown keyword %s ", __FUNCTION__, keyword );
			}
			set_active( true );
			file_cur = nullptr;
		}

		close_bdd_factories();

	SPY_POP_RANGE2();
}

void	c_file_virtual::close_vfile()
{
	if( is_active() )
	{
		if( file_cur )
		{
			fclose( file_cur );
			file_cur = nullptr;
		}
		set_active( false );
	}
#if AAA_DEBUG()
	else
		debug_break( "No vfile to close" );
#endif
}

void	c_file_virtual::push_vfile()
{
#if AAA_DEBUG()
	INT32 CONST depth = (INT32)stack_active.size();
	if( depth >= 12 )
		debug_break( "%s() suspiciously deep stack (%d)", __FUNCTION__, depth );
#endif
	stack_active.push_back( is_active() );
	set_active( false );
}

void	c_file_virtual::pop_vfile()
{
	if( !stack_active.empty() )
	{
		set_active( stack_active.back() );
		stack_active.pop_back();
	}
	else
		debug_break( "pop_vfile() called with empty stack" );
}
