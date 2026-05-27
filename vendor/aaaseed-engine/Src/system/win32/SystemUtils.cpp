#include "system/shared/SystemUtils.h"

#include <fstream>

#if defined(NATIVE_OS_WIN32)
#	ifndef AAA_AAA_OS_H
#		include "aaa_os.h"
#	endif
#	include <Shlwapi.h>
#	include <Shlobj.h>
#endif

#include "system/win32/SystemWindow.h"

#include "err.h"
#include "aaa_mem.h"


namespace sysutils
{

// local dedicated path
#if defined (NATIVE_OS_WIN32)
#define LOCAL_PATH_DEDICATED "\\AAASeed\\"
#else
#define LOCAL_PATH_DEDICATED "/AAASeed/"
#endif

// local dedicated dir
#define LOCAL_DIR_DEDICATED "mwm.dir"

static bool b_sys_init = false;
void init_sys()
{
	if( b_sys_init )
		return;

	// Win32 COM
	HRESULT rtn = ::CoInitializeEx( nullptr, COINIT_MULTITHREADED | COINIT_SPEED_OVER_MEMORY );	//COINIT_DISABLE_OLE1DDE
	if( !SUCCEEDED( rtn ) )
		debug_break( "Can't initialize Win32 COM CoInitializeEx() failed." );
	b_sys_init = true;
}
void deinit_sys()
{
	if( !b_sys_init )
		return;

	// Release Win32 COM.
	::CoUninitialize();
	b_sys_init = false;
}

//-------------------------------------------------------------------------------------------
void utf8_to_unicode( wchar_t* dst, INT32 CONST size_dst, char const* src, INT32 CONST size_src )
{
//	we replace it too because WideCharToMultiByte
//	mbstowcs( rtn, p_text, size );
	MultiByteToWideChar( CP_UTF8, 0, src, size_src, dst, size_dst );
}

//-------------------------------------------------------------------------------------------
wchar_t * utf8_to_unicode( const char* src )
{
	INT32 CONST size_src = int( strlen(src) + 1 );
	INT32 CONST size_needed = MultiByteToWideChar( CP_UTF8, 0, src, size_src, 0, 0 );
	wchar_t* dst = new wchar_t[ size_needed ];
//	we replace it too because WideCharToMultiByte
//	mbstowcs( rtn, p_text, size );
	MultiByteToWideChar( CP_UTF8, 0, src, size_src, dst, size_needed );
	return dst;
}
//todo rename and redo
void free_str_tmp( wchar_t*& pt )
{
	SAFE_DELETE_ARRAY(pt);
}

//-------------------------------------------------------------------------------------------
char * toUtf8Char( wchar_t* text )
{
	const int size = int( wcslen(text) + 1 );
	int		size_needed = WideCharToMultiByte( CP_UTF8, 0, text, size, nullptr, 0, nullptr, nullptr );
	char* rtn = new char[ size_needed ];
//	wcstombs was failing
//		wcstombs( rtn, p_text, size );
	WideCharToMultiByte( CP_UTF8, 0, text, size, rtn, size_needed, nullptr, nullptr );
	return rtn;
}
void free_str_tmp( char*& pt )
{
	SAFE_DELETE_ARRAY(pt);
}

//-------------------------------------------------------------------------------------------
std::string floatToString( const float & p_floatValue )
{
	std::string Str_Return;

	std::ostringstream ost;
	ost << p_floatValue;

	Str_Return = ost.str();

	return Str_Return;
}

//-------------------------------------------------------------------------------------------
std::string secondsToHmsString( const int32_t & p_seconds )
{
	int32_t hours	 = p_seconds / 3600;
	int32_t minutes = (p_seconds % 3600) / 60;
	int32_t seconds = p_seconds % 3600 % 60;

	std::string Str_Return = "";
	if( hours < 10 ) {
		Str_Return += "0";
	}
	Str_Return += sysutils::numericalToString( hours );
	Str_Return += " : ";

	if( minutes < 10 ) {
		Str_Return += "0";
	}
	Str_Return += sysutils::numericalToString( minutes );
	Str_Return += " : ";

	if( seconds < 10 ) {
		Str_Return += "0";
	}
	Str_Return += sysutils::numericalToString( seconds );

	return Str_Return;
}

//-------------------------------------------------------------------------------------------
bool doesFileExist( const std::string & p_path )
{
	bool B_Return = false;

	// Try to open file
	FILE * file = fopen( p_path.c_str(), "r" );
	// File exists
	if( file ) 
	{
		// Update return value
		B_Return = true;
		// Free memory
		fclose( file );
	}

	// Return result
	return B_Return;
}

//------------------------------------------------------------------------------------------
bool copyFile(const std::string& p_pathSource, const std::string& p_pathDestination)
{
	if(doesFileExist(p_pathSource))
	{
		std::ifstream  src(p_pathSource, std::ios::binary);
		std::ofstream  dst(p_pathDestination,   std::ios::binary);

		dst << src.rdbuf();

		return true;
	}
	else 
		return false;
}

//-------------------------------------------------------------------------------------------
std::string removeFileNameFromPath( const std::string & p_path )
{
	std::string Str_Return = std::string( p_path );

	const size_t last_slash_idx = Str_Return.find_last_of("\\/");
	if( std::string::npos != last_slash_idx )
	{
		const size_t length = Str_Return.length();
		Str_Return.erase( last_slash_idx + 1, length );
	}

	return Str_Return;
}

//-------------------------------------------------------------------------------------------
std::string removeExtensionFromFileName( const std::string & p_name )
{
	std::string Str_Return = std::string( p_name );

	const size_t last_dot_idx = Str_Return.find_last_of(".");
	if( std::string::npos != last_dot_idx )
	{
		const size_t length = Str_Return.length();
		Str_Return.erase( last_dot_idx, length );
	}

	return Str_Return;
}

//-------------------------------------------------------------------------------------------
std::string getFileNameFromPath( const std::string & p_path )
{
	std::string Str_Return = std::string( p_path );

	const size_t last_slash_idx = Str_Return.find_last_of("\\/");
	if( std::string::npos != last_slash_idx )
	{
		Str_Return.erase( 0, last_slash_idx + 1 );
	}

	return Str_Return;
}

//	Try to pull a displayable name out of the shell item using one of the non-FILESYSPATH
//	SIGDN values. Used only for diagnostics when SIGDN_FILESYSPATH fails. Returns a heap
//	UTF-8 string (caller frees with free_str_tmp) or nullptr.
static char* get_item_display_name_diag( IShellItem* p_item, SIGDN kind )
{
	if( !p_item )
		return nullptr;
	PWSTR w_name = nullptr;
	HRESULT hr = p_item->GetDisplayName( kind, &w_name );
	if( !SUCCEEDED( hr ) || !w_name )
		return nullptr;
	char* utf8 = toUtf8Char( w_name );
	CoTaskMemFree( w_name );
	return utf8;
}

static PWSTR get_file_name( IShellItem* p_item, LPCWSTR w_ext = nullptr )
{
	PWSTR path_out = nullptr;

	if( !p_item )
	{
		DBG_PRINT_STRING( "%s() p_item is null -- GetResult/GetItemAt returned a null shell item.", __FUNCTION__ );
		return nullptr;
	}

	PWSTR w_path = nullptr;
	HRESULT hr = p_item->GetDisplayName( SIGDN_FILESYSPATH, &w_path );

	if( !SUCCEEDED( hr ) )
	{
		//	Dump HRESULT + best-effort human-readable names so we know WHICH item failed.
		//	Common failures: virtual items (Home, This PC root), cloud-only OneDrive items
		//	with Files On-Demand, Library view items, items from a ZIP/CAB namespace
		//	extension, etc. HRESULT 0x80070490 (ERROR_NOT_FOUND) is the typical code.
		char* normal   = get_item_display_name_diag( p_item, SIGDN_NORMALDISPLAY         );
		char* parsing  = get_item_display_name_diag( p_item, SIGDN_DESKTOPABSOLUTEPARSING );
		char* editing  = get_item_display_name_diag( p_item, SIGDN_DESKTOPABSOLUTEEDITING );
		DBG_PRINT_STRING( "%s() GetDisplayName(SIGDN_FILESYSPATH) failed hr=0x%08lX -- picked item has no filesystem path.", __FUNCTION__, (unsigned long)hr );
		DBG_PRINT_STRING( "%s()   NORMALDISPLAY=\"%s\"  PARSING=\"%s\"  EDITING=\"%s\"", __FUNCTION__,
								normal  ? normal  : "(none)",
								parsing ? parsing : "(none)",
								editing ? editing : "(none)" );
		sysutils::free_str_tmp( normal  );
		sysutils::free_str_tmp( parsing );
		sysutils::free_str_tmp( editing );
		return nullptr;
	}

	if( !w_path )
	{
		DBG_PRINT_STRING( "%s() GetDisplayName succeeded but returned w_path=null -- treating as failure.", __FUNCTION__ );
		return nullptr;
	}

	size_t size = lstrlen( w_path );
	if( size == 0 )
	{
		DBG_PRINT_STRING( "%s() GetDisplayName returned an empty filesystem path.", __FUNCTION__ );
		CoTaskMemFree( w_path );
		return nullptr;
	}

	size_t size_ext = 0;
	if( w_ext )
		size_ext = lstrlen( w_ext );

	path_out = new wchar_t[size + size_ext + 1];
	wcscpy( path_out, w_path );
	CoTaskMemFree( w_path );

	if( w_ext )
	{
		//	if the end of string is not .xxx we add it. Guard size>=size_ext so short
		//	filenames (e.g. "a" with ".jpeg") don't underflow path_out.
		if( size < size_ext || wcscmp( path_out + size - size_ext, w_ext ) )
			wcscpy( path_out + size, w_ext );
	}

#if AAA_DEBUG()
	{
		char* diag = toUtf8Char( path_out );
		DBG_PRINT_STRING( "%s() resolved filesystem path = \"%s\"", __FUNCTION__, diag ? diag : "(conv-fail)" );
		sysutils::free_str_tmp( diag );
	}
#endif

	return path_out;
}

//-------------------------------------------------------------------------------------------
//	The real dialog implementation. MUST be called from an STA thread: IFileOpenDialog
//	and the IShellItem objects it returns are STA-only -- invoking any IShellItem method
//	from the MTA that init_sys() joins this process to yields RPC_E_WRONG_THREAD (0x8001010E).
//	Do not call directly; go through do_dialog_file_all() which hops to an STA worker.
static bool do_dialog_file_all_sta( o_str& dst, const LPWSTR target_path, const LPWSTR message, COMDLG_FILTERSPEC* extensions, system_window* parent, bool b_save, bool b_folder, bool b_multiple )
{
	bool b_retcode = false;
//	LPWSTR path_out = nullptr;

	// Grab target path
	IShellItem* pItemFolder = nullptr;
	if( target_path && *target_path )
	{
		HRESULT hr = ::SHCreateItemFromParsingName( target_path, nullptr, IID_PPV_ARGS( &pItemFolder ) );
		if( !SUCCEEDED( hr ) )
			DBG_PRINT_STRING( "%s() unable to create IShellItem from path.", __FUNCTION__ );
	}

	// Create the File Open Dialog object.
	IFileDialog* pfd = nullptr;
	HRESULT hr;
	if( b_save )
	{
		IFileSaveDialog* pfd_save;
		//todo use smart pointer ?
		hr = ::CoCreateInstance( CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pfd_save ) );
		pfd = pfd_save;
	}
	else
	{
		//todo use smart pointer ?
		IFileOpenDialog* pfd_open;
		hr = ::CoCreateInstance( CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &pfd_open ) );
		pfd = pfd_open;
	}

	// Dialog box created
	if( SUCCEEDED( hr ) )
	{
		// Set the options on the dialog.
		DWORD dw_flags;
		// Before setting, always get the options first in order not to override existing options.
	
		hr = pfd->GetOptions( &dw_flags );
		if( SUCCEEDED( hr ) )
		{
			// In this case, get only files which extensions are specified below.

			//dw_flags |= FOS_NOCHANGEDIR;
			if( b_save )
				dw_flags |= FOS_OVERWRITEPROMPT ;
			else
			{
				if( b_folder )
					dw_flags = FOS_PICKFOLDERS ;
				else
				{
					dw_flags |= FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST;
					if( b_multiple )
					{
					//	debug_break( "have to deal with this" );
						dw_flags |= FOS_ALLOWMULTISELECT;
					}
				}
			}

			if( message )
			{
				hr = pfd->SetTitle( message );
			}

			hr = pfd->SetOptions( dw_flags );
			if( SUCCEEDED( hr ) )
			{
				int32_t ext_nb = 0;
				if( !b_folder && extensions ) // Set the file types to display only. Notice that, this is a 1-based array.
				{
					while( (extensions + ext_nb)->pszName )
						++ext_nb;
					hr = pfd->SetFileTypes( ext_nb, extensions );
					if( ext_nb>0 && SUCCEEDED( hr ) )
					{
						pfd->SetFileTypeIndex( 1 );
						if( !SUCCEEDED( hr ) )
							debug_break( "%s() unable to set file type.", __FUNCTION__ );
					}
				}

				// Set target folder
				if( pItemFolder )
				{
					hr = pfd->SetFolder( pItemFolder );
					if( !SUCCEEDED( hr ) )
						debug_break( "%s() unable to set default folder.", __FUNCTION__ );
				}

				// Show the dialog
				hr = pfd->Show( parent ? parent->get_handle() : nullptr );
				//hr = pfd->Show( nullptr );
				if( hr == HRESULT_FROM_WIN32(ERROR_CANCELLED) )
				{
					DBG_PRINT_STRING( "%s() ERROR_CANCELLED case", __FUNCTION__ );
				}
				else if( !SUCCEEDED( hr ) )
					debug_break( "%s() didn't get the result of file dialog.", __FUNCTION__ );
				else
				{
					// Obtain the result, once the user clicks the 'Open' button.
					if( b_save || !b_multiple )
					{
						LPCWSTR w_ext = nullptr;
						if( b_save && ext_nb>0 )
						{
							UINT index;
							hr = pfd->GetFileTypeIndex( &index );
							if( SUCCEEDED( hr ) )
								w_ext = (extensions + index - 1)->pszSpec + 1;
						}
						// The result is an IShellItem object.
						IShellItem* shell_item = nullptr;
						hr = pfd->GetResult( &shell_item );
						if( SUCCEEDED( hr ) && shell_item )
						{
							LPWSTR path_out = get_file_name( shell_item, w_ext );
							if( path_out && *path_out )
							{
								dst.set( path_out );
								DBG_PRINT_STRING( "%s() dst set to %s ", __FUNCTION__, dst.get() );
								b_retcode = true;
							}
							else
							{
								// GetDisplayName(SIGDN_FILESYSPATH) can fail for virtual/cloud-only items
								// (OneDrive Files On-Demand, Library items, etc.) -- do NOT mark success.
								DBG_PRINT_STRING( "%s() unable to resolve picked item to a filesystem path.", __FUNCTION__ );
							}
							sysutils::free_str_tmp( path_out );
							shell_item->Release();
						}
					}
					else
					{	//return the data here
						// The results are an IShellItemArray object.
						IShellItemArray* shell_item_array = nullptr;
						hr = (reinterpret_cast<IFileOpenDialog*>(pfd))->GetResults( &shell_item_array );
						if( SUCCEEDED( hr ) && shell_item_array )
						{
							DWORD item_count = 0;
							hr = shell_item_array->GetCount( &item_count );
							if( SUCCEEDED( hr ) && item_count > 0 )
							{
								dst.erase();
								DWORD nb_added = 0;
								for( DWORD i = 0; i < item_count; i++ )
								{
									IShellItem* shell_item = nullptr;
									hr = shell_item_array->GetItemAt( i, &shell_item );
									if( SUCCEEDED( hr ) && shell_item )
									{
										LPWSTR path_out = get_file_name( shell_item, nullptr );
										if( path_out && *path_out )
										{
											if( nb_added > 0 )
												dst.add_char( ';' );
											dst.add( path_out );
											++nb_added;
											DBG_PRINT_STRING( "%s() dst now %s ", __FUNCTION__, dst.get() );
										}
										else
										{
											DBG_PRINT_STRING( "%s() skipping item %u -- no filesystem path.", __FUNCTION__, (unsigned)i );
										}
										sysutils::free_str_tmp( path_out );
										shell_item->Release();
									}
								}
								if( nb_added > 0 )
									b_retcode = true;
							}
							shell_item_array->Release();
						}
					}
				}
			}
		}
		if( pItemFolder )
			pItemFolder->Release();
		pfd->Release();
	}

	return b_retcode;
}

//-------------------------------------------------------------------------------------------
//	Arguments trampolined through the STA worker thread that actually runs the dialog.
struct file_dialog_sta_args
{
	o_str*				dst;
	LPWSTR				target_path;
	LPWSTR				message;
	COMDLG_FILTERSPEC*	extensions;
	system_window*		parent;
	bool				b_save;
	bool				b_folder;
	bool				b_multiple;
	bool				result;
};

static DWORD WINAPI file_dialog_sta_thread_proc( LPVOID p_param )
{
	file_dialog_sta_args* args = (file_dialog_sta_args*)p_param;

	//	Join this thread to an STA. init_sys() puts the main thread in MTA (for
	//	DirectShow and friends); we need STA specifically for IFileOpenDialog and
	//	for any IShellItem methods we call on the returned picks.
	HRESULT co_hr = ::CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE );
	bool const b_need_uninit = SUCCEEDED( co_hr );
	if( !b_need_uninit && co_hr != RPC_E_CHANGED_MODE )
	{
		DBG_PRINT_STRING( "%s() CoInitializeEx(STA) failed hr=0x%08lX -- dialog may still work but behavior is undefined.",
				__FUNCTION__, (unsigned long)co_hr );
	}

	args->result = do_dialog_file_all_sta(
			*args->dst, args->target_path, args->message, args->extensions,
			args->parent, args->b_save, args->b_folder, args->b_multiple );

	if( b_need_uninit )
		::CoUninitialize();

	return 0;
}

//	Public entry point: runs the STA implementation on a dedicated STA worker thread,
//	so callers on MTA (our main thread) don't trip RPC_E_WRONG_THREAD on IShellItem.
bool do_dialog_file_all( o_str& dst, const LPWSTR target_path, const LPWSTR message, COMDLG_FILTERSPEC* extensions, system_window* parent, bool b_save, bool b_folder, bool b_multiple )
{
	file_dialog_sta_args args;
	args.dst		= &dst;
	args.target_path= target_path;
	args.message	= message;
	args.extensions	= extensions;
	args.parent		= parent;
	args.b_save		= b_save;
	args.b_folder	= b_folder;
	args.b_multiple	= b_multiple;
	args.result		= false;

	HANDLE h_thread = ::CreateThread( nullptr, 0, file_dialog_sta_thread_proc, &args, 0, nullptr );
	if( !h_thread )
	{
		DBG_PRINT_STRING( "%s() CreateThread failed (GetLastError=0x%08lX) -- falling back to in-caller STA impl.",
				__FUNCTION__, (unsigned long)::GetLastError() );
		//	Last-ditch fallback. Likely to hit RPC_E_WRONG_THREAD but better than silently
		//	doing nothing if the OS denies us a thread.
		return do_dialog_file_all_sta( dst, target_path, message, extensions, parent, b_save, b_folder, b_multiple );
	}

	::WaitForSingleObject( h_thread, INFINITE );
	::CloseHandle( h_thread );

	return args.result;
}

static COMDLG_FILTERSPEC*	new_filter_unicode( C_PCHAR str )
{
	if( !str || ((*str)==0) )
		return nullptr;

	const int32_t max_item = 32;
	COMDLG_FILTERSPEC* filter = new COMDLG_FILTERSPEC[max_item+1];
	int32_t nb = 0;
	C_PCHAR name;
	C_PCHAR ext = nullptr;
	C_PCHAR cur = str;
	for(;;)
	{
		name = cur;
		// find name
		while( *cur )
			++cur;
		++cur;
		if( *cur == 0 )
			break;
		ext = cur;
		while( *cur )
			++cur;
		(filter+nb)->pszName = utf8_to_unicode( name );
		(filter+nb)->pszSpec = utf8_to_unicode( ext );
		if( ++nb >= max_item )
			break;
		++cur;
	}
	(filter+nb)->pszName = nullptr;
	(filter+nb)->pszSpec = nullptr;
	return filter;
}
static void	delete_filter_unicode( COMDLG_FILTERSPEC* filters )
{
	COMDLG_FILTERSPEC* elt = filters;
	while( elt->pszName )
	{
		free_str_tmp( (wchar_t*&) elt->pszName );
		free_str_tmp( (wchar_t*&) elt->pszSpec );
		++elt;
	}
	SAFE_DELETE_ARRAY( filters );
}

bool do_dialog_file_all( o_str& dst, const char* path, C_PCHAR_C message, const char* extensions, system_window* parent, bool b_save, bool b_folder, bool b_multiple )
{
	LPWSTR path_unicode = nullptr;
	if( path && *path )
		path_unicode = utf8_to_unicode( path );
	LPWSTR mess_unicode = nullptr;
	if( message && *message )
		mess_unicode = utf8_to_unicode( message );



	// Convert extensions char to filter spec
	COMDLG_FILTERSPEC* filter = new_filter_unicode( extensions );

	bool b_retcode = do_dialog_file_all( dst, path_unicode, mess_unicode, filter, parent, b_save, b_folder, b_multiple );
	DBG_PRINT_STRING( "%s() call to File dialog returned %s with name %s", __FUNCTION__, b_retcode ? "true" : "false", dst.get() );
	if( filter )
		delete_filter_unicode( filter );

	sysutils::free_str_tmp( path_unicode );	//check if it crash
	sysutils::free_str_tmp( mess_unicode );	//check if it crash

	return b_retcode;
}

//	LOAD
bool do_file_dialog_load( o_str& dst, const LPWSTR target_path, const LPWSTR message, COMDLG_FILTERSPEC* extensions, system_window* parent, bool b_folder, bool b_multiple )
{
	return do_dialog_file_all( dst, target_path, message, extensions, parent, false, b_folder, b_multiple );
}
bool do_file_dialog_load( o_str& dst, const char* target_path, C_PCHAR_C message, const char* extensions, system_window* parent, bool b_folder, bool b_multiple )
{
	return do_dialog_file_all( dst, target_path, message, extensions, parent, false, b_folder, b_multiple );
}

//	SAVE
bool do_file_dialog_save( o_str& dst, const LPWSTR target_path, const LPWSTR message, COMDLG_FILTERSPEC* extensions, system_window* parent )
{
	return do_dialog_file_all( dst, target_path, message, extensions, parent, true, false, false );
}
bool do_file_dialog_save( o_str& dst, const char* target_path, C_PCHAR_C message, const char* extensions, system_window* parent )
{
	return do_dialog_file_all( dst, target_path, message, extensions, parent, true, false, false );
}

//	FOLDER
bool do_file_dialog_folder( o_str& dst, const LPWSTR target_path, const LPWSTR message, system_window* parent )
{
	return do_dialog_file_all( dst, target_path, message, nullptr, parent, false, true, false );
}
bool do_file_dialog_folder( o_str& dst, const char* target_path, C_PCHAR_C message, system_window* parent )
{
	return do_dialog_file_all( dst, target_path, message, nullptr, parent, false, true, false );
}

//=============================================================================================
void openDropBrowser( const std::string & p_targetPath, std::pair<std::vector<std::string>,std::string> p_extensionn )
{
	//todo deal with filter : remove ?
	wchar_t * dir = utf8_to_unicode( p_targetPath.c_str() );

	::ShellExecuteW( nullptr, L"explore", dir, nullptr, nullptr, SW_NORMAL );

	sysutils::free_str_tmp( dir );
}

//-------------------------------------------------------------------------------------------
std::string normalizePath( const std::string & p_path )
{
	std::string S_Return( p_path );

	NATIVE_Uint32_t inc = 0;

	for( NATIVE_Uint32_t i=0; i < p_path.length(); i++ )
	{
		if( p_path.at(i) == '\\' )
		{
			S_Return.insert( i+inc, "\\" );
			++inc;
		}
	}

	return S_Return;
}

//---------------------------------------------------------------------------------------------
std::vector<std::string> listDirectoryFiles( const std::string & p_pathDirectory )
{
	// Return vector
	std::vector<std::string> Vec_Return;


	WIN32_FIND_DATAW ffd;
	WCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError=0;


	// Check that the input path plus 3 is not longer than MAX_PATH
	// Three characters are for the "\*" plus nullptr appended below
	length_of_arg = p_pathDirectory.length();
	if (length_of_arg > (MAX_PATH - 3))
	{
		DBG_PRINT_STRING( "%s() Directory path is too long.", __FUNCTION__ );
		return Vec_Return;
	}


	// Prepare string for use with FindFile functions.  First, copy the
	// string to a buffer, then append '\*' to the directory name
	wchar_t * wpathDirectory = utf8_to_unicode( p_pathDirectory.c_str() );
	wcscpy( szDir, wpathDirectory );
	wcscat( szDir, L"\\*" );
	sysutils::free_str_tmp( wpathDirectory );

	// Find the first file in the directory
	hFind = ::FindFirstFileW( szDir, &ffd );
	// If no file found
	if( INVALID_HANDLE_VALUE == hFind ) 
	{
		DBG_PRINT_STRING( "%s() Directory is empty.", __FUNCTION__ );
		return Vec_Return;
	}

	// List all the files in the directory
	do
	{
		NATIVE_char8_t * wcFileName = toUtf8Char(ffd.cFileName);
		Vec_Return.push_back( std::string( wcFileName ) );
		sysutils::free_str_tmp( wcFileName );
	}
	while( ::FindNextFileW(hFind, &ffd) != 0 );

	// Check if files were missed
	dwError = aaa::system::get_err_last();
	if( dwError != ERROR_NO_MORE_FILES ) 
	{
		DBG_PRINT_STRING( "%s() Files listing failed.", __FUNCTION__ );
	}

	// Release finding engine
	::FindClose( hFind );

	return Vec_Return;
}

//------------------------------------------------------------------------------------------------
bool createDirectory( const std::string & p_path )
{
	wchar_t * wpath = utf8_to_unicode( p_path.c_str() );
	BOOL bReturn = ::CreateDirectoryW( wpath, nullptr); 
	sysutils::free_str_tmp( wpath );
	return (bReturn != 0);
}

//---------------------------------------------------------------------------------------------
void openWebBrowser( const std::string & p_url )
{
	wchar_t * wurl = utf8_to_unicode( p_url.c_str() );
	::ShellExecuteW( nullptr, L"open", wurl, nullptr, nullptr, SW_SHOWNORMAL );
	sysutils::free_str_tmp( wurl );
}

//=============================================================================================
void copyToClipboard( const std::string & p_text )
{
	if( p_text.size() > 0 )
	{
		if( ::OpenClipboard(nullptr) )
		{
			if( ::EmptyClipboard() )
			{
				// Allocate a global memory object for the text. 
				HGLOBAL hglbCopy = ::GlobalAlloc( GMEM_MOVEABLE, (p_text.size()+1) * sizeof(TCHAR) );
				if( hglbCopy != nullptr ) 
				{ 

					// Lock the handle and copy the text to the buffer.  
					LPTSTR lptstrCopy = (LPTSTR)GlobalLock(hglbCopy); 

					wchar_t * unicodeText = utf8_to_unicode( p_text.c_str() );

					MEMCPY( lptstrCopy, unicodeText, (p_text.size()+1) * sizeof(TCHAR), __FUNCTION__ );

					::GlobalUnlock(hglbCopy); 

					sysutils::free_str_tmp( unicodeText );

					// Place the handle on the clipboard. 
					::SetClipboardData( CF_UNICODETEXT, hglbCopy );
				} 		
			}

			// Close the clipboard. 
			::CloseClipboard();
		}
	}
}

//=============================================================================================
std::string getTextFromClipboard( void )
{
	std::string Str_Return = "";

	// Request text
	if( ::IsClipboardFormatAvailable(CF_UNICODETEXT) ) 
	{
		// Open Clipboard
		if( ::OpenClipboard(nullptr) ) 
		{
			// Retrieve datas
			HGLOBAL hglb = ::GetClipboardData(CF_UNICODETEXT); 
			if( hglb != nullptr ) 
			{ 
				// Retrieve text
				LPTSTR lptstr = (LPTSTR)GlobalLock(hglb); 
				if( lptstr != nullptr ) 
				{
					char * text = toUtf8Char( lptstr );

					Str_Return = std::string( text );

					sysutils::free_str_tmp( text );

					::GlobalUnlock( hglb );
				}
			} 
			::CloseClipboard();
		}
	}

	return Str_Return;
}

//--------------------------------------------------------------------------------------------
bool getKeyStateCtrl( void )	{ return (::GetKeyState( VK_CONTROL ) & 0x8000) != 0; }
bool getKeyStateAlt( void )		{ return (::GetKeyState( VK_MENU    ) & 0x8000) != 0; }
bool getKeyStateShift( void )	{ return (::GetKeyState( VK_SHIFT   ) & 0x8000) != 0; }

//=============================================================================================
bool getModifierState( void )
{
	return ( getKeyStateCtrl() || getKeyStateAlt() || getKeyStateShift() );
}


//--------------------------------------------------------------------------------------------
bool getActualKeyStateCtrl(		void )	{	return ( ::GetAsyncKeyState( VK_CONTROL ) & 0x8000 ) != 0;  }
bool getActualKeyStateAlt(		void )	{	return ( ::GetAsyncKeyState( VK_MENU	) & 0x8000 ) != 0;  }
bool getActualKeyStateShift(	void )	{	return ( ::GetAsyncKeyState( VK_SHIFT	) & 0x8000 ) != 0;  }

bool getActualKeyStateSpace(	void )	{	return ( ::GetAsyncKeyState( VK_SPACE	) & 0x8000 ) != 0;  }	

//-------------------------------------------------------------------------------------------
void setActualKeyStateCtrl(  const bool & p_bState ) {} // OSX RESERVED
void setActualKeyStateAlt(	 const bool & p_bState ) {} // OSX RESERVED
void setActualKeyStateShift( const bool & p_bState ) {} // OSX RESERVED
void setActualKeyStateSpace( const bool & p_bState ) {} // OSX RESERVED

//--------------------------------------------------------------------------------------------
int32_t get_monitor_nb( void )				{ return ::GetSystemMetrics( SM_CMONITORS );		}

//--------------------------------------------------------------------------------------------
int32_t get_primary_monitor_sx( void )		{ return ::GetSystemMetrics( SM_CXSCREEN );			}
int32_t get_primary_monitor_sy( void )		{ return ::GetSystemMetrics( SM_CYSCREEN );			}

//--------------------------------------------------------------------------------------------
int32_t get_virtual_screen_x( void )		{ return ::GetSystemMetrics( SM_XVIRTUALSCREEN );	}
int32_t get_virtual_screen_y( void )		{ return ::GetSystemMetrics( SM_YVIRTUALSCREEN );	}

int32_t get_virtual_screen_sx( void )
{
	int32_t sx = get_primary_monitor_sx();
#ifdef SM_CXVIRTUALSCREEN
	int32_t other = GetSystemMetrics( SM_CXVIRTUALSCREEN );
	if( other > 0 )
		sx = other;
#endif
	return sx;
}

int32_t get_virtual_screen_sy( void )
{
	int32_t sy = get_primary_monitor_sy();
#ifdef SM_CYVIRTUALSCREEN
	int32_t other = GetSystemMetrics( SM_CYVIRTUALSCREEN );
	if( other > 0 )
		sy = other;
#endif
	return sy;
}

//--------------------------------------------------------------------------------------------
int32_t getMainMonitorWidth( void )				{ return ::GetSystemMetrics( SM_CXFULLSCREEN );	}
int32_t getMainMonitorHeight( void )			{ return ::GetSystemMetrics( SM_CYFULLSCREEN );	}

//---------------------------------------------------------------------------------------------
bool createErrorWindow( const std::string & p_title, const std::string & p_text )
{
	wchar_t * title = utf8_to_unicode(p_title.c_str());
	wchar_t * text = utf8_to_unicode(p_text.c_str());

	std::wstring strTitle( title );
	std::wstring strText( text );

	sysutils::free_str_tmp( text );
	sysutils::free_str_tmp( title );

	return (::MessageBoxW(nullptr, strText.c_str(), strTitle.c_str(), MB_OK | MB_ICONERROR) == IDOK);
}

//---------------------------------------------------------------------------------------------
void createFatalErrorWindow( const std::string & p_text )
{
	::FatalAppExitW( -1, utf8_to_unicode(p_text.c_str()) );
}

//--------------------------------------------------------------------------------------------
bool createWarningWindow( const std::string & p_title, const std::string & p_text )
{
	wchar_t * title = utf8_to_unicode(p_title.c_str());
	wchar_t * text = utf8_to_unicode(p_text.c_str());

	std::wstring strTitle( title );
	std::wstring strText( text );

	sysutils::free_str_tmp( text );
	sysutils::free_str_tmp( title );

	return (::MessageBoxW(nullptr, strText.c_str(), strTitle.c_str(), MB_OKCANCEL | MB_ICONWARNING) == IDOK);
}

//-------------------------------------------------------------------------------------------------
void extensionFromFilename( wchar_t * p_pFileName, wchar_t * p_pExtension )
{
	int32_t len = lstrlenW( p_pFileName );
	int32_t begin;

	for( begin = len; begin >= 0; --begin )
	{
		if( p_pFileName[begin] == '.' )
		{
			++begin;
			break;
		}
	}

	if( begin <= 0 )
		p_pExtension[0] = '\0';
	else
		lstrcpyW( p_pExtension, &p_pFileName[begin] );
}

//---------------------------------------------------------------------------------------------
std::string extensionFromFilename( std::string p_pFileName )
{
	std::string Str_Return;

	size_t begin = 0;
	bool found = false;

	for( size_t i=p_pFileName.size()-1; i>0 && !found; i-- )
	{
		if( p_pFileName.at(i) == '.' )
		{
			begin = i;
			found = true;
		}
	}

	Str_Return = std::string( p_pFileName.begin()+begin, p_pFileName.end() );

	return Str_Return;
}

std::string ExePath() 
{
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW( nullptr, buffer, MAX_PATH );

	NATIVE_char8_t * buf = toUtf8Char( buffer );

	std::string Str_Return = std::string( buf );
	std::string::size_type pos = Str_Return.find_last_of( "\\/" );

	sysutils::free_str_tmp( buf );

	return Str_Return.substr( 0, pos);
}

} // namespace sysutils


//-----------------------------------------------------------------------------

#if defined(NATIVE_OS_WIN32)
wchar_t * get_folder_path_known_win32(	sysutils::FOLDER_KNOWN CONST p_folder_type )
{
	GUID id = GUID_NULL;
	switch( p_folder_type )
	{
	case sysutils::FOLDER_KNOWN::DOCUMENTS:			id = FOLDERID_Documents;		break;
	case sysutils::FOLDER_KNOWN::PROGRAM_FILES_x86:	id = FOLDERID_ProgramFilesX86;	break;
	case sysutils::FOLDER_KNOWN::PROGRAM_FILES_x64:	id = FOLDERID_ProgramFilesX64;	break;
	case sysutils::FOLDER_KNOWN::PROGRAM_DATA:		id = FOLDERID_ProgramData;		break;
	case sysutils::FOLDER_KNOWN::APP_DATA_LOCAL:	id = FOLDERID_LocalAppData;		break;
	case sysutils::FOLDER_KNOWN::APP_DATA_ROAMING:	id = FOLDERID_RoamingAppData;	break;
	case sysutils::FOLDER_KNOWN::DESKTOP:			id = FOLDERID_Desktop;			break;
	}
	wchar_t * wpath = nullptr;
	if( id != GUID_NULL )
		SHGetKnownFolderPath( id, 0, nullptr, &wpath );
	return wpath;
}

bool sysutils::get_folder_path_known(	char * CONST dst,		FOLDER_KNOWN CONST	folder_type )
{
	wchar_t * wpath = get_folder_path_known_win32( folder_type );
	if( !wpath )
		return false;

	size_t const len = wcslen( wpath );
	wcstombs( dst, wpath, len );
	dst[len] = '\0';
	CoTaskMemFree( wpath );
	return true;
}

bool sysutils::get_folder_path_known(	wchar_t * CONST	dst,	FOLDER_KNOWN CONST	folder_type )
{
	wchar_t * wpath = get_folder_path_known_win32( folder_type );
	if( !wpath )
		return false;

	wcscpy( dst, wpath );
	CoTaskMemFree( wpath );
	return true;
}

bool sysutils::get_folder_path_known(	o_str * dst,			FOLDER_KNOWN CONST	folder_type )
{
	wchar_t * wpath = get_folder_path_known_win32( folder_type );
	if( !wpath )
		return false;

	dst->set( wpath );
	CoTaskMemFree( wpath );
	return true;
}
#endif

//-----------------------------------------------------------------------------
