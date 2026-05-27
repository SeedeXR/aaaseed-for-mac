#include "seed_user.h"
#include "aaa_str.h"
#include "file/aaa_dir.h"
#include "infrastructure/namer.h"
#include "obj_ui/com/net.h"


namespace aaa
{
namespace file
{
o_str user_name;
o_str net_name;
o_str app_dir;

namespace {
	thread_local  o_str	buf;

	CONSTEXPR CHAR	kernel_default_name[]	= "AAAKernel";
	CONSTEXPR CHAR	user_dirname[]			= "AAAUser";
	CONSTEXPR CHAR	username_filename[]		= "AAAWho.txt";
//	CONSTEXPR CHAR	user_name_def[]			= "Guest";
}

C_PCHAR_C get_user_pref_filename()
{
	buf.set( c_dir::get_user() );
	buf.add_slash();
	buf.add( "default" );
	return buf.get();
}

AAA_ERR read_vector_str( o_str CONST & path, std::vector<o_str> & dst )
{
	AAA_ERR ret;
	if( c_file::is_exist( path ) )
	{
		FILE* f = c_file::FOPEN( path, "rt" );
		if( f )
		{
			o_str& o = o_str::push_name();
				ret = o.read_file( path );
				if( ERR(ret) )
					BOX_ERR( "Can't read the existing file \"%s\".", path.get() );
				else
					o.split( dst, '\n' );
				c_file::FCLOSE(f);
			 o_str::pop_name();
		}
		else
		{
			BOX_ERR( "Can't open the existing file \"%s\".", path.get() );
			ret = ERR_FOPEN;
		}
	}
	else
	{
		BOX_ERR( "No file \"%s\".", path.get() );
		ret = ERR_FILE_NO;
	}
	return ret;
}

bool make_aaawho_path( o_str& path )
{
	bool b_found;
	o_str& base = o_str::push_name();

		base.set( user_dirname );
		base.add_slash();
		base.add( username_filename );

		path.set( "../" );
		path.add( base );
		b_found = c_file::is_exist( path );
		if( !b_found )
		{
			path.set( kernel_default_name );
			path.add_slash();
			path.add( "Dev" );
			path.add_slash();
			path.add( base );
			b_found = c_file::is_exist( path );
		}

	o_str::pop_name();
	return b_found;
}

AAA_ERR	read_user_pref()
{
	AAA_ERR	retcode;

	// STARTING DIRECTORY
	// where AAASeed start is the starting directory
	c_dir::set_cur_as_start();
	GOOD_PRINT_STRING( "\"%s\" is now Starting directory", c_dir::get_start().get() );

	// just to check
	INT32 CONST fname_index_begin = o_str::get_stack_index();
	// KERNEL DIRECTORY
	{
		o_str& dir_kernel = o_str::push_name();

			dir_kernel.set( c_dir::get_start() );
			dir_kernel.add_slash();
			dir_kernel.add( kernel_default_name );
			
			retcode = c_dir::set_def( dir_kernel );
			//retcode = c_dir::push_def( dir_kernel );
			c_dir::set_cur_as_kernel();
			//c_dir::pop_def();
			if( NOERR( retcode ) )
			{
				GOOD_PRINT_STRING( "\"%s\" is now kernel directory", c_dir::get_kernel().get() );
				c_namer::set_dir_only(		(c_obj_ui*)c_obj_ui::get_the_root(), c_dir::get_def() );
				c_namer::set_namer_start(	(c_obj_ui*)c_obj_ui::get_the_root() );
			}			
			else
				BOX_ERR( "Could not set kernel directory to \"%s\"", dir_kernel.get() );
				
		o_str::pop_name();
	}
	
	if( NOERR(retcode) )
	{
		o_str& aaawho_path			= o_str::push_name();
		o_str& app_path_relative	= o_str::push_name();
	//	o_str& path_user = o_str::push_name();
		c_dir::push_def( c_dir::get_start() );

			//find AAAWho.txt and read the 3 lines 
			retcode = make_aaawho_path( aaawho_path ) ? AAA_OK : ERR_FILE_NO;
			if( NOERR(retcode) )
			{
				GOOD_PRINT_STRING( "\"%s\" found and will be used", aaawho_path.get() );

				std::vector<o_str> words;
				retcode = read_vector_str( aaawho_path, words );
				if( NOERR(retcode) )
				{
					if( words.size()!=3 )
					{
						BOX_ERR( "We expect 3 lines for this file \"%s\":\n  user_name\n  net_name\n  APP_dir_path_from_AAAUser", aaawho_path.get() );
						retcode = ERR_FILE_FORMAT;
					}
					else
					{
						user_name.set( words[0] );
						GOOD_PRINT_STRING_INV( "User name is \"%s\" ",		user_name.get() );

						net_name.set( words[1] );
						GOOD_PRINT_STRING_INV( "Net name is \"%s\"",		net_name.get() );

						app_path_relative.set( words[2] );
						GOOD_PRINT_STRING_INV( "User APPs dir is \"%s\"",	app_path_relative.get() );
					}
				}
			}
			else
			{
				BOX_ERR( "No \"%s/%s\" file in the starting directory or up one level.", user_dirname, username_filename );
			}

			//	build data location filename
			if( NOERR( retcode ) )
			{
				o_str& path_aaauser_dir = o_str::push_name();
				// make AAAUser dir
				path_aaauser_dir.set_dir_name( aaawho_path.get() );

					retcode = c_dir::push_def( path_aaauser_dir );
						if( NOERR(retcode) )
						{
							path_aaauser_dir.set( c_dir::get_cur() );
							GOOD_PRINT_STRING_INV( "AAAUser dir is \"%s\"",	path_aaauser_dir.get() );
						}
						else
							BOX_ERR( "No \"%s\" folder in the starting directory or up one level.", path_aaauser_dir.get() );

						//user folder
						retcode = c_dir::push_def( user_name );
							if( NOERR(retcode) )
							{
								c_dir::set_cur_as_user();
								GOOD_PRINT_STRING( "\"%s\" is now user folder", c_dir::get_user().get() );
							}
							else
								BOX_ERR( "No \"%s\" folder in the AAAUser directory.", user_name.get() );
						c_dir::pop_def();	//user_name
						
						if( NOERR(retcode) )
						{
							// deal with net
							obj_get( net );
							if( net )
							{
								retcode = c_dir::push_def( net_name );
									if( NOERR(retcode) )
									{
										o_str& path = o_str::push_name();
											c_dir::set_cur_as_net();
											GOOD_PRINT_STRING( "\"%s\" is now net folder", c_dir::get_net().get() );

											path.set( "pref" );
											path.add_ext( "net" );
											GOOD_PRINT_STRING( "Net file is \"%s\"", path.get() );

											retcode = net->load_from_file( path );
											if( NOERR(retcode) )
											{
												// retrieve langroup and machine name
												net->get_host();
											}
											else
											{
												BOX_ERR( "Can't read \"%s\"", path.get() );
												retcode = AAA_OK;	// we can continue
											}
										o_str::pop_name();
									}
									else
									{
										BOX_ERR( "No \"%s\" folder in the AAAUser directory.", net_name.get() );
										retcode = AAA_OK;	// we can continue
									}
								c_dir::pop_def(); // net_name
							}
							else
							{	
								BOX_ERR( "Can't create net object" );
								retcode = ERR_OBJ_NULL;
							}
						}
					c_dir::pop_def();	//path_aaauser_dir

					// we consider it is relative to the start folder 
					if( NOERR(retcode) )
					{
						retcode = c_dir::push_def( app_path_relative );
							if( NOERR(retcode) )
							{
								c_dir::update_cur();
								app_dir.set( c_dir::get_cur() );
								GOOD_PRINT_STRING( "\"%s\" is now APPs folder", app_dir.get() );
							}
							else
							{
								BOX_ERR( "No \"%s\" folder in the AAAUser directory.", app_path_relative.get() );
								retcode = AAA_OK;
							}
						c_dir::pop_def();	//app_path_relative
					}

				 o_str::pop_name();	// path_aaauser_dir
			}

			//		aaa::user_pref_filename.add_slash();
			//		aaa::user_pref_filename.add( "default" );
			//		aaa::user_pref_filename.add_ext( c_pref_start::the_factory().get_file_ext().get() );

		c_dir::pop_def();	// start folder
		o_str::pop_name();	//	app_path_relative
		o_str::pop_name();	//	aaawho_path
	}
	
	if( NOERR(retcode) )
		GOOD_PRINT_STRING( "user_pref_filename is \"%s\"", get_user_pref_filename() );

	INT32 CONST fname_index_end = o_str::get_stack_index();
	if( fname_index_begin != fname_index_end )
		debug_break( "%s() begin with o_str depth stack %d but end with %d", __FUNCTION__, fname_index_begin, fname_index_end );

	return retcode;
}

}	//namespace file
}	//namespace aaa
