#include "aaa_file_lua.h"
#include "aaa_dir_lua.h"
#include "file/file_io.h"
#include "system/shared/SystemUtils.h"
#include "WinIoCtl.h"
#include "aaa_dir.h"
#include "ui/file_dlg.h"
#include "system/win32/SystemWindow.h"
#include "infrastructure/seedfile.h"


namespace aaalua
{
namespace n_file
{
	c_files_async*	files[16] = {	nullptr,nullptr,nullptr,nullptr,
									nullptr,nullptr,nullptr,nullptr,
									nullptr,nullptr,nullptr,nullptr,
									nullptr,nullptr,nullptr,nullptr		};

	thread_local o_str o_buf;

	AAALUACALL( read_async )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		INT32		slot		= l.get_int32( -2 );
		C_PCHAR_C	filename	= l.get_str();
		if( filename )
		{
			c_files_async* file = files[slot-1];
			if( !file )
			{
				file = new c_files_async;
				files[slot-1] = file;
			}
			if( file && filename )
			{
				//AAA_ERR ret = ERR_ANY;
				file->load_async( slot, filename );
			}
		}
		else
			l.error_message( "filename name not a string" );
		return l.return_nothing();
	}

	//	input	filename
	AAALUACALL(	is_exist )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	filename	= l.get_str();

		if( filename && *filename )
			return l.return_bool( c_file::is_exist(filename) );

		l.error_message( "filename name not a string" );
		return	l.return_nothing();
	}

	//todo return err_str
	//	input	filename
	AAALUACALL(	create )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	filename	= l.get_str();

		bool	b_ok = false;
		if( filename && *filename )
		{
			if( c_file::is_exist(filename) )
				b_ok = true;
			else
			{
				FILE* file = c_file::FOPEN( filename, "wb" );
				if( file )
				{
					c_file::FCLOSE( file );
					b_ok = true;
				}
			}
		}
		else
			l.error_message( "filename name not a string" );
		return l.return_bool( b_ok );
	}

	//	input	filename
	AAALUACALL(	read_text )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	filename	= l.get_str();

		if( filename && *filename )
		{
			if( NOERR( o_buf.read_file(filename) ) )
				return l.return_string( o_buf );
		}
		else
			l.error_message( "filename name not a string" );
		return l.return_nil();
	}

	//	input	filename
	AAALUACALL(	get_modification_date )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	filename	= l.get_str();

		if( filename && *filename )
		{
			return l.return_int64( c_file::get_mdate(filename) );
		}
		else
			l.error_message( "filename name not a string" );
		return	l.return_nothing();
	}

	// remove
	AAALUACALL( remove )
	{
		LUAAAA_START( L, __FUNCTION__ )
		l.check_arg_nb( 1 );
		
		C_PCHAR err_str = nullptr;
		C_PCHAR_C filename	= l.get_str();
		if( filename && *filename )
		{
			wchar_t* filename_w = sysutils::utf8_to_unicode( filename );

			if( ::DeleteFileW(filename_w) == 0 )
			{
				// Error during move		 
				err_str = "Can't delete";
				ERR_PRINT_STRING( "%s file %ls : error %s", err_str, filename_w, aaa::system::get_err_message().c_str() );
			}
			else
				GOOD_PRINT_STRING( "%ls removed", filename_w );

			sysutils::free_str_tmp( filename_w );
		}
		else
			l.error_message( "filename name not a string" );
		if( err_str )
			return l.return_string( err_str );
		else
			return l.return_nothing();
	}

	// move
	AAALUACALL( move )
	{
		LUAAAA_START( L, __FUNCTION__ )
		l.check_arg_nb( 2 );

		C_PCHAR err_str = nullptr;
		C_PCHAR_C filename_1 = l.get_str( 1 );
		C_PCHAR_C filename_2 = l.get_str( 2 );
		if( filename_1 && *filename_1 )
		{
			if( filename_2 && *filename_2 )
			{
				wchar_t* filename_src = sysutils::utf8_to_unicode( filename_1 );
				wchar_t* filename_dst = sysutils::utf8_to_unicode( filename_2 );

				if( ::MoveFileW( filename_src, filename_dst ) == 0 )
				{
					// Error during move
					err_str = "Can't move";
					ERR_PRINT_STRING( "%s %ls to file %ls : error %s", err_str, filename_src, filename_dst, aaa::system::get_err_message().c_str() );
				}
				else
					GOOD_PRINT_STRING( "%ls copied to %ls", filename_src, filename_dst );

				sysutils::free_str_tmp( filename_dst );
				sysutils::free_str_tmp( filename_src );
			}
			else
				l.error_message("filename destination name not a string");
		}
		else
			l.error_message("filename source name not a string");

		if( err_str )
			return l.return_string( err_str );
		else
			return l.return_nothing();
	}

	//	CopyFile
	AAALUACALL(	copy )
	{
		LUAAAA_START( L, __FUNCTION__ )

		HANDLE	file_src = INVALID_HANDLE_VALUE;
		HANDLE	file_dst = INVALID_HANDLE_VALUE;
		C_PCHAR	err_str = nullptr;

		INT32 CONST arg_nb = l.get_arg_nb( 2, 3 );
		C_PCHAR_C filename_1 = l.get_str( 1 );
		C_PCHAR_C filename_2 = l.get_str( 2 );
		if( filename_1 && *filename_1 )
		{
			if( filename_2 && *filename_2 )
			{
				wchar_t*	filename_src	= sysutils::utf8_to_unicode( filename_1 );
				wchar_t*	filename_dst	= sysutils::utf8_to_unicode( filename_2 );
				bool CONST	b_overwrite		= (arg_nb >= 3) ? l.get_bool(3) : false;

				bool	b_copy = true;

				file_src = ::CreateFileW( filename_src, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
				if( file_src == INVALID_HANDLE_VALUE )
				{
					err_str = "Could not open source file";
					ERR_PRINT_STRING( "%s %ls, %s", err_str, filename_src, aaa::system::get_err_message().c_str() );		
					goto goto_exit;
				}

				file_dst = ::CreateFileW( filename_dst, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr );
				if( !b_overwrite && file_dst != INVALID_HANDLE_VALUE )
				{
					// File exist
					// Check last modified time
					FILETIME	ft_write_src, ft_write_dst;

					// Retrieve the files times for the source file.
					if( !::GetFileTime( file_src, nullptr, nullptr, &ft_write_src ) )
					{
						err_str = "Can't get write time for source file";
						ERR_PRINT_STRING( "%s %ls", err_str, filename_src );
						goto goto_exit;
					}
					// Retrieve the files times for the dst file.
					if( !::GetFileTime( file_dst, nullptr, nullptr, &ft_write_dst ) )
					{
						err_str = "Can't get write time for destination file";
						ERR_PRINT_STRING( "%s %ls", err_str, filename_dst );
						goto goto_exit;
					}
					if( ::CompareFileTime( &ft_write_src, &ft_write_dst ) == 0 )
					{
						// Files Times are identical
						// Check size
						LARGE_INTEGER size_src, size_dst;
						if( !GetFileSizeEx( file_src, &size_src ) || !GetFileSizeEx( file_dst, &size_dst ) )
						{
							err_str = "Can't get files sizes";
							ERR_PRINT_STRING( err_str );
							goto goto_exit;
						}
						else
						{
							if( size_src.QuadPart == size_dst.QuadPart )
							{
								// File Sizes are identical
								b_copy = false;
							}
						}
					}
					::CloseHandle( file_dst );
					file_dst = INVALID_HANDLE_VALUE;
				}
				else
				{
					if( file_dst != INVALID_HANDLE_VALUE )
						::CloseHandle( file_dst );
					// file does not exist, need to check directory before trying to copy
					// directory structure has to be created before
					o_buf.set_dir_name( l.get_str(2) );
					if( ERR( c_dir::make( o_buf ) ) )
					{
						err_str = "Can't create destination dir";
						ERR_PRINT_STRING( "%s %s", err_str, o_buf.get() );
						goto goto_exit;
					}
				}
				::CloseHandle( file_src ) ;
				file_src = INVALID_HANDLE_VALUE;

				if( b_copy )
				{
					if( ::CopyFileW( filename_src, filename_dst, false ) == 0 )
					{
						// Error during copy
						err_str = "Can't copy";
						ERR_PRINT_STRING( "%s %ls to file %ls : error %s", err_str, filename_src, filename_dst, aaa::system::get_err_message().c_str() );
						goto goto_exit;
					}
					else
					{
						//GOOD_PRINT_STRING( "%ls copied to %ls", filename_src, filename_dst );
					}
				}
goto_exit:
				if( file_src != INVALID_HANDLE_VALUE )
					::CloseHandle( file_src );
				if( file_dst != INVALID_HANDLE_VALUE )
					::CloseHandle( file_dst );

				sysutils::free_str_tmp( filename_dst );
				sysutils::free_str_tmp( filename_src );
			}
			else
				l.error_message("filename destination name not a string");
		}
		else
			l.error_message("filename source name not a string");

		if( err_str )
			return l.return_string( err_str );
		else
			return l.return_nothing();
	}

	//	input	filename_old filename_new
	AAALUACALL(	rename )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		C_PCHAR filename_old	= l.get_str(-2);
		C_PCHAR filename_new	= l.get_str(-1);

		if( filename_old && *filename_old )
		{
			if( filename_old && *filename_old )
			{
				INT32 result = ::rename( filename_old, filename_new );
				if( result!=0 )
				{
					C_PCHAR err_str = "Can't rename file or dir";
					ERR_PRINT_STRING( "%s %s to %s : error %s", err_str, filename_old, filename_new, aaa::system::get_err_message().c_str() );
					l.return_string( err_str );
				}
			}
			else
				l.error_message("filename destination name not a string");
		}
		else
			l.error_message("filename source name not a string");
		return	l.return_nothing();
	}

	//	input	filename
	AAALUACALL(	get_ext )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR filename	= l.get_str();

		if( filename ) //&& *filename )
		{
			filename = fname::get_ext(filename);
			if( filename && *filename )
				return l.return_string( filename );
		}


		return	l.return_nothing();
	}
	//	input	filename
	AAALUACALL(	get_file_name )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	filename	= l.get_str();

		if( filename ) //&& *filename )
			return l.return_string( fname::get_fname( filename ) );
		else
			l.error_message("filename name not a string");
		return	l.return_nothing();
	}


	//	input	filename
	AAALUACALL(	get_dir_name )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	filename	= l.get_str();

		if( filename ) // && *filename )
		{
			o_buf.set_dir_name( filename );
			return l.return_string( o_buf );
		}
		return	l.return_nothing();
	}
	//	input	filename
	AAALUACALL(	get_name_pure )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	filename	= l.get_str();

		if( filename )	//	&& *filename )
		{
			o_buf.set_fname_pure( filename );
			return l.return_string( o_buf );
		}
		return	l.return_nothing();
	}

	bool get_disks_property( HANDLE device_handle, PSTORAGE_DEVICE_DESCRIPTOR device_desc )
	{
		STORAGE_PROPERTY_QUERY	query;		// input param for query
		DWORD					out_length;	// IOCTL output length
		BOOL					b_result;	// IOCTL return val

		// specify the query type
		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;

		// Query using IOCTL_STORAGE_QUERY_PROPERTY 
		b_result = ::DeviceIoControl( device_handle,		// device handle
				IOCTL_STORAGE_QUERY_PROPERTY,			// info of device property
				&query, sizeof(STORAGE_PROPERTY_QUERY),	// input data buffer
				device_desc, device_desc->Size,			// output data buffer
				&out_length,							// out's length
				nullptr);			

		return b_result != 0;
	}

	enum LUA_DRIVE_TYPE : INT32
	{
		LUA_DRIVE_UNKNOWN = 0, 
		LUA_DRIVE_CD_ROM,
		LUA_DRIVE_USB_DRIVE,
		LUA_DRIVE_USB_KEY,
		LUA_DRIVE_FIXED,
		LUA_DRIVE_REMOVABLE,
		LUA_DRIVE_NETWORK,
		LUA_DRIVE_RAM,
		LUA_DRIVE_TYPE_MAX_NB,
	};

	static C_PCHAR	LUA_DRIVE_TYPE_STR[ LUA_DRIVE_TYPE_MAX_NB ] =
	{
		"unknown",
		"cd_rom",
		"usb_drive",
		"usb_key",
		"fixed",
		"removable",
		"network",
		"ram"
	};	

	AAALUACALL(	list_drive )
	{
		LUAAAA_START( L, __FUNCTION__ )
		l.check_arg_nb_escape_no( 0 );

		INT32			index = 1;
	
		CHAR			drive_volume[256];
		HANDLE			device_handle;		// used to determine if drive is USB

		CHAR			drive_buffer[1024];
		::GetLogicalDriveStringsA( 1024, drive_buffer );
		CHAR*			pch = drive_buffer;
		UINT32			drive_type;
		LUA_DRIVE_TYPE	lua_drive_type = LUA_DRIVE_UNKNOWN; // unknown by default
		l.new_table();
		while( *pch )
		{
			bool	b_usb = false;
			l.push_int( index );
		
			l.new_table();
				// drive letter
				l.push_string( "drive" );
				l.push_string( pch );
				l.set_table( -3 );

			//GOOD_PRINT_STRING("Drive found %s, ", pch );
			drive_type = ::GetDriveTypeA( pch );

			switch ( drive_type ) 
			{
				case DRIVE_UNKNOWN: // The drive type cannot be determined.
				case DRIVE_NO_ROOT_DIR: // The root directory does not exist.
				//	GOOD_PRINT_STRING( "Unknown Drive Type" );
					break;
				case DRIVE_CDROM: // The drive is a CD-ROM drive.
				//	GOOD_PRINT_STRING( "CD-ROM Drive" );
					lua_drive_type = LUA_DRIVE_CD_ROM;
					break;
				case DRIVE_REMOVABLE: // The drive can be removed from the drive.
				case DRIVE_FIXED: // The disk cannot be removed from the drive.

					if( drive_type == DRIVE_FIXED )
						lua_drive_type = LUA_DRIVE_FIXED;
					else
						lua_drive_type = LUA_DRIVE_REMOVABLE;				

					sprintf( drive_volume, "\\\\?\\%c:", pch[0] );		// weird formatting, but needed to get it working, MSDN says that \\?\ extend the path to 32k char, it change the string to unicode.
					device_handle = CreateFileA( drive_volume, 0, 0, nullptr, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, nullptr);

					if( device_handle != INVALID_HANDLE_VALUE )
					{
						PSTORAGE_DEVICE_DESCRIPTOR	device_descriptor;
						device_descriptor = (PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];

						device_descriptor->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

						if( get_disks_property( device_handle, device_descriptor ) )
						{
							if( device_descriptor->BusType == BusTypeUsb )
							{
								b_usb = true;
								if( drive_type == DRIVE_FIXED )
									lua_drive_type = LUA_DRIVE_USB_DRIVE;
								else
									lua_drive_type = LUA_DRIVE_USB_KEY;
							}

							C_PCHAR str;
							switch( device_descriptor->BusType )
							{
							case BusTypeScsi:				str = "Scsi";				break;
							case BusTypeAtapi:				str = "Atapi";				break;
							case BusTypeAta:				str = "Ata";				break;
							case BusType1394:				str = "1394";				break;
							case BusTypeSsa:				str = "Ssa";				break;
							case BusTypeFibre:				str = "Fibre";				break;
							case BusTypeUsb:				str = "Usb";				break;
							case BusTypeRAID:				str = "RAID";				break;
							case BusTypeiScsi:				str = "iScsi";				break;
							case BusTypeSas:				str = "Sas";				break;
							case BusTypeSata:				str = "Sata";				break;
							case BusTypeSd:					str = "Sd";					break;
							case BusTypeMmc:				str = "Mmc";				break;
							case BusTypeVirtual:			str = "Virtual";			break;
							case BusTypeFileBackedVirtual:	str = "FileBackedVirtual";	break;
#ifdef BusTypeSpaces
							case BusTypeSpaces:				str = "Spaces";				break;
							case BusTypeNvme:				str = "Nvme";				break;
#endif
							case BusTypeUnknown:
							default:						str = "Unknown";			break;
							}
							l.push_string( "bus_type" );
							l.push_string( str );
							l.set_table( -3 );

							if( device_descriptor->VendorIdOffset )
							{
								CHAR* str = ((CHAR*)device_descriptor) + device_descriptor->VendorIdOffset;
								l.push_string( "vendor_id" );
								l.push_string( str );
								l.set_table( -3 );
							}
							if( device_descriptor->ProductIdOffset )
							{
								CHAR* str = ((CHAR*)device_descriptor) + device_descriptor->ProductIdOffset;
								l.push_string( "product_id" );
								l.push_string( str );
								l.set_table( -3 );
							}
							if( device_descriptor->ProductRevisionOffset )
							{
								CHAR* str = ((CHAR*)device_descriptor) + device_descriptor->ProductRevisionOffset;
								l.push_string( "product_revision" );
								l.push_string( str );
								l.set_table( -3 );
							}
							if( device_descriptor->SerialNumberOffset )
							{
								CHAR* str = ((CHAR*)device_descriptor) + device_descriptor->SerialNumberOffset;
								l.push_string( "serial_number" );
								l.push_string( str );
								l.set_table( -3 );
							}
						}
						delete device_descriptor;
						CloseHandle( device_handle );
					}
					break;
				case DRIVE_REMOTE: // The drive is a remote (network) drive.
					//GOOD_PRINT_STRING( "Network Drive" );
					lua_drive_type = LUA_DRIVE_NETWORK;
					break;
				case DRIVE_RAMDISK: // The drive is a RAM disk.
					//GOOD_PRINT_STRING( "RAM Drive" );
					lua_drive_type = LUA_DRIVE_RAM;
					break;
			}

			// Drive Type
			l.push_string( "type" );
			l.push_string( LUA_DRIVE_TYPE_STR[ lua_drive_type ] );
			l.set_table( -3 );

			// USB Drive or USB Key
			l.push_string( "b_usb" );
			l.push_bool( b_usb );
			l.set_table( -3 );

			l.set_table( -3 );

			index++;
			pch = &pch[strlen(pch) + 1];
		}
		return 1;
	}

		//	input	title filter
	//	return	filename
	//todo add multiple option
	AAALUACALL(	do_dialog_open )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg = l.get_arg_nb( 1, 2, 3 );
		C_PCHAR_C	title		= l.get_str( 1 );
		C_PCHAR		filter		= nb_arg>=2 ? l.get_str( 2 ) : nullptr;
		bool		b_multiple	= nb_arg>=3 ? l.get_bool( 3 ) : false;

		if( str_is_equal_nocase( filter, "image" ) )
			filter = aaa::file::get_filter_from_type_io( aaa::file::TYPE_IO_TEXTURE_2D );
		else if( str_is_equal_nocase( filter, "video" ) )
			filter = aaa::file::get_filter_from_type_io( aaa::file::TYPE_IO_VIDEO );

		o_str tmp;
		auto err = aaa::file::get_filename_open( tmp, title, system_window::get_window_main(), filter, b_multiple );
		if( NOERR(err) )
		{
			if( b_multiple )
			{
				tmp.get_line_nb();
				//DBG_PRINT_STRING( "count is %d", tmp.count_char() );
				std::vector<o_str> files;
				tmp.split( files, ';' );
				INT32 nb = INT32( files.size() );
				DBG_PRINT_STRING( "count is %d", nb );
				if( nb )
				{	
					l.new_table();	//	create result table
					for( INT32 i=0; i<nb; ++i )						
						l.set_field( i+1, files[i].get() );
					return l.return_table();
				}
				else
					return l.return_nil();
			}
			else
				return l.return_string_or_nil( tmp );
		}

		if( err != ERR_CANCEL )
			l.error_message( "can't get filename for opening, error is %s.", ERR_GET_STR(err) );
		return l.return_nothing();
	}

	//	input	title filter
	//	return	filename
	AAALUACALL(	do_dialog_save )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 CONST	nb_arg = l.get_arg_nb( 1, 2 );
		C_PCHAR_C	title  = l.get_str( 1 );
		C_PCHAR		filter = nb_arg >= 2 ? l.get_str( 2 ) : nullptr;

		if( str_is_equal_nocase( filter, "image" ) )
			filter = aaa::file::get_filter_from_type_io( aaa::file::TYPE_IO_SNAPSHOT );
		else if( str_is_equal_nocase( filter, "video" ) )
			filter = aaa::file::get_filter_from_type_io( aaa::file::TYPE_IO_VIDEO );

		o_str	tmp;
		AAA_ERR	err = aaa::file::get_filename_save(tmp, title, system_window::get_window_main(), filter);
		if( NOERR(err) )
			return l.return_string_or_nil( tmp );

		if( err != ERR_CANCEL )
			l.error_message( "can't get filename for saving" );
		return l.return_nothing();
	}

	//	input	title
	//	return	folder name
	AAALUACALL(	do_dialog_folder )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C	title	= l.get_str();

//todo return the error message
		o_str	tmp;
		if( NOERR( aaa::file::get_folder( tmp, title, system_window::get_window_main() ) ) )
			return l.return_string_or_nil( tmp );

		l.error_message( "folder_dialog Canceled or can't get folder" );
		return l.return_nothing();
	}


	void	register_file( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "file" );
			ADD_FN(	read_async				);
			ADD_FN( is_exist				);
			ADD_FN( create					);
			ADD_FN( read_text				);
			ADD_FN( get_modification_date	);
			ADD_FN( remove					);
			ADD_FN( move					);
			ADD_FN( copy					);
			ADD_FN( rename					);
			ADD_FN( get_dir_name			);
			ADD_FN( get_file_name			);
			ADD_FN( get_name_pure			);
			ADD_FN( get_ext					);
			ADD_FN( list_drive				);

			ADD_FN( do_dialog_open			);
			ADD_FN( do_dialog_save			);
			ADD_FN( do_dialog_folder		);
		l.pop( 1 );	//pop new table
	}
}	//end namespace n_file
}	//end namespace aaalua
