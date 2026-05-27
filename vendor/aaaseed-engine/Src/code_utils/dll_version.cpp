
#include "dll_version.h"
		 
#include "psapi.h"
#include "err.h"
#include "system/shared/SystemUtils.h"
#include "aaa_mem.h"


#include <lib_use.h>
AAA_LIB_USE_MESSAGE( "version" )


c_module_details::c_module_details()
{
}

c_module_details::~c_module_details()
{
}

void c_module_details::init()
{
	_h_process	= HANDLE(0);
	_h_module	= HMODULE(0);
	_error		= 0;
	_file_size	= 0;
	_lang_id	= 0;
	_char_set	= 0;
	_o_comments.erase();			
	_o_company_name.erase();
	_o_file_description.erase();
	_o_file_version.erase();
	_o_internal_name.erase();
	_o_legal_copyright.erase();
	_o_legal_trademarks.erase();
	_o_original_filename.erase();
	_o_private_build.erase();
	_o_product_name.erase();
	_o_product_version.erase();
	_o_special_build.erase();
}

bool c_module_details::set( HANDLE CONST process_id, HMODULE CONST module_id )
{
	init();

	_h_process	= process_id;
	_h_module	= module_id;

	wchar_t		buffer[ MAX_PATH ];
	wchar_t*	unicode_version_info = nullptr;
	
	//	CFileStatus		FileStatus;
	DWORD result;
	DWORD version_handle = 0;
	DWORD version_size = GetFileVersionInfoSizeW( buffer, &version_handle );

	//Get the module details based on the handle passed
	MEMCLEAR( buffer, MAX_PATH );
	result = ::GetModuleBaseNameW( _h_process, _h_module, buffer, MAX_PATH - 1 );
	if( result == 0 )
	{
		_error = aaa::system::get_err_last();
		goto goto_exit;
	}
	else
	{
		_o_base_name.set( buffer );
	}

	// Module Filename Full path
	MEMCLEAR( buffer, MAX_PATH );
	result = ::GetModuleFileNameExW( _h_process, _h_module, buffer, MAX_PATH-1 );
	if( result == 0 )
	{
		_error = aaa::system::get_err_last();
		goto goto_exit;
	}
	else
	{
		_o_full_file_name.set( buffer );
		//		 _o_full_file_name.MakeUpper();
	}
	_o_module_name.set( _o_base_name );

	// Get the file details
	//	CFile::GetStatus( _o_full_file_name,FileStatus);
	//	m_lFileSize = FileStatus.m_size;
	//	m_FileDate = FileStatus.m_mtime;

	//	TRACE(GetModuleName());
	//	TRACE("\n");

	// Get the version information
	version_size = GetFileVersionInfoSizeW( buffer, &version_handle );
	if( version_size == 0 )
	{
		_error = aaa::system::get_err_last();
		goto goto_exit;
	}
	else
	{
		unicode_version_info = (wchar_t *)MALLOC( version_size );
		if( !unicode_version_info )
		{
			_error = aaa::system::get_err_last();
			goto goto_exit;
		}
		else
		{
			if( ::GetFileVersionInfoW( buffer, version_handle, version_size, unicode_version_info ) == FALSE )
			{
				_error = aaa::system::get_err_last();
				goto goto_exit;
			}
			else
			{
				void*	fixed_pointer = nullptr;
				UINT	fixed_length = 0;
				if( ::VerQueryValueW( unicode_version_info, L"\\VarFileInfo\\Translation", &fixed_pointer, &fixed_length ) == FALSE )
				{
					_error = aaa::system::get_err_last();
					goto goto_exit;
				}
				else
				{
					TRANSLATIONS* translations = (TRANSLATIONS *)fixed_pointer;

					_lang_id  = translations[0].m_wLangID;
					_char_set = translations[0].m_wCharSet;

					//version_info = sysutils::toUtf8Char( unicode_version_info );
					GetVersionString( _o_comments,			unicode_version_info, L"Comments"			);
					GetVersionString( _o_company_name,		unicode_version_info, L"CompanyName"		);
					GetVersionString( _o_file_description,	unicode_version_info, L"FileDescription"	);
					GetVersionString( _o_file_version,		unicode_version_info, L"FileVersion"		);
					GetVersionString( _o_internal_name,		unicode_version_info, L"InternalName"		);
					GetVersionString( _o_legal_copyright,	unicode_version_info, L"LegalCopyright"		);
					GetVersionString( _o_legal_trademarks,	unicode_version_info, L"LegalTrademarks"	);
					GetVersionString( _o_original_filename,	unicode_version_info, L"OriginalFilename"	);
					GetVersionString( _o_private_build,		unicode_version_info, L"PrivateBuild"		);
					GetVersionString( _o_product_name,		unicode_version_info, L"ProductName"		);
					GetVersionString( _o_product_version,	unicode_version_info, L"ProductVersion"		);
					GetVersionString( _o_special_build,		unicode_version_info, L"SpecialBuild"		);
				}
			}
		}
	}
goto_exit:
	IF_FREE_AND_NULL( unicode_version_info );
	if( _error != 0 )
	{
		ERR_PRINT_STRING( "c_module_details Failure for file %s", _o_module_name.get() );
		ERR_PRINT_STRING( "\t%s", aaa::system::get_err_message(_error).c_str() );
		return false;
	}
	//SAFE_DELETE_ARRAY( version_info );
	return true;
}

void	c_module_details::GetVersionString( o_str& return_str, wchar_t CONST * CONST version_info, wchar_t CONST * CONST key )
{
	void*	fixed_pointer = nullptr;
	UINT	fixed_length = 0;
	wchar_t	query[ 64 ];
//	o_str	return_str;
	return_str.set( "" );

	wsprintfW( query, L"\\StringFileInfo\\%04x%04x\\%s", _lang_id, _char_set, key );
	if( ::VerQueryValueW( version_info, query, &fixed_pointer, &fixed_length ) == TRUE )
	{
		if( fixed_length > 0 )
		{
			// uFixedLength includes the trailing "\0"
			return_str.set( (wchar_t*)fixed_pointer );
			//return(CString((char *)pFixedPointer,((int)uFixedLength)-1));
		}
	}
	else
	{
	//	_error = aaa::system::get_err_last();
	}
	//return;
}

namespace aaa
{
	void	print_dll_version()
	{
		HMODULE		h_modules[ 1024 ];
		//int			handle_count = 0;
		int			result = 0;
		DWORD		dw_needed = 0;
		HANDLE		h_process;
		DWORD		proc_id = GetCurrentProcessId();

		h_process = OpenProcess( PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, proc_id );
		result = EnumProcessModules( h_process, h_modules, sizeof(h_modules), &dw_needed );
		if( result != 0 )
		{
			int	handle_count = ( dw_needed/sizeof(HMODULE) );
			c_module_details* p_module = new c_module_details;
			C_PCHAR str;
			for( INT32 i = 0; i < handle_count; ++i ) 
			{
				p_module->set( h_process, h_modules[i] );		

				str = p_module->GetInternalName();
				if( *str || *(p_module->GetFullFileName()) )
					GOOD_PRINT_STRING( "name %s\tfile %s",		str, p_module->GetFullFileName() );
				str = p_module->GetProductVersion();
				if( *str )
					GOOD_PRINT_STRING( "\tProduct version\t%s",	str );
				str = p_module->GetFileVersion();
				if( *str )
					GOOD_PRINT_STRING( "\tFile version\t%s",	str );
				str = p_module->GetCompanyName();
				if( *str ) 
					GOOD_PRINT_STRING( "\t%s",					str );
				str = p_module->GetLegalCopyright();
				if( *str ) 
					GOOD_PRINT_STRING( "\t%s",					str );

			}
			SAFE_DELETE( p_module );
		}
		CloseHandle( h_process );
	}
}
