#include "file_dlg.h" 
#include "aaa_mem.h" 
#include "system/shared/SystemUtils.h"
#include "file/aaa_dir.h"
#include "aaa/aaa_mutex.h"

#define AAA_LOCAL_DIALOG_2014()	1 

#ifdef	WIN32
#	if AAA_LOCAL_DIALOG_2014()
#		include <shlobj.h>
#		include <stdlib.h>
#	endif	//AAA_LOCAL_DIALOG_2014
#endif

namespace aaa::file
{
	//	we make it big too allow serious multiselection
	o_str	str_file_dialog( 1024*8 );
	//hack using a single buffer we should avoid concurrent use of the dialogs
	aaa::MUTEX	file_dialog_lock;

#ifdef	WIN32
#	if AAA_LOCAL_DIALOG_2014()
		//	HINSTANCE	hinst;
		OPENFILENAMEA ofn;
		OPENFILENAMEA sfn;
		bool	b_get_file_name_open_first = true;
		bool	b_get_file_name_save_first = true;
		char szDirName[AAA_PATH_MAX()];	//	directory string
		char szFileTitle[260];		//	file title string
#	endif	//AAA_LOCAL_DIALOG_2014
#endif	//WIN32

	bool	try_lock()
	{
		bool ret = file_dialog_lock.try_lock();
		if( !ret )
			ERR_PRINT_STRING( "there is already a file dialog running, can't open two at the same time" );
		return ret;
	}
	void	unlock()
	{
		file_dialog_lock.unlock();
	}


	AAA_ERR get_filename_open( o_str & filename, C_PCHAR_C str_message, system_window* parent, C_PCHAR_C str_filter, bool b_multiple )
	{
		if( !try_lock() )
			return ERR_LOCKED;

		c_dir::push_def( nullptr );

			str_file_dialog.convert_to_backslash();

			bool b_retcode = sysutils::do_file_dialog_load( str_file_dialog, str_file_dialog.get(), str_message, str_filter, parent, false, b_multiple );

			// very old school
			//INT32	result;
			//GOOD_PRINT_STRING( "???? %s : ", str_message );
			//result = scanf( "%8192s", str_file_dialog.get() );	//hack 8192 should not be here
			//GOOD_PRINT_STRING( "Trying to read %s", str_file_dialog.get() );
			//if( result == 1 ) 
			//	ret = str_file_dialog.get();	//todo potentialy dangerous ?
	

			if( b_retcode )
			{
				str_file_dialog.convert_to_slash();
				str_file_dialog.sync();
			}	

			c_dir::reset();	//hack GetOpenFileName fuck up the current dir

		c_dir::pop_def();

		filename.set( str_file_dialog );

		unlock();
		DBG_PRINT_STRING( "%s() Open dialog Filename is %s", __FUNCTION__, filename.get() );
		return b_retcode ? (filename.is_empty() ? ERR_STR_EMPTY : AAA_OK) : ERR_CANCEL;
	}

	AAA_ERR get_filename_save( o_str & filename, C_PCHAR_C str_message, system_window* parent, C_PCHAR_C str_filter )
	{
		if( !try_lock() )
			return ERR_LOCKED;

		bool b_retcode = false;
#ifdef	WIN32
		str_file_dialog.convert_to_backslash();
		b_retcode = sysutils::do_file_dialog_save( str_file_dialog, str_file_dialog.get(), str_message, str_filter, parent );
		//str_file_dialog.set( path );
		//sysutils::free_str_tmp( path );
		// ret = str_file_dialog.get();
		str_file_dialog.convert_to_slash();
#else	// of #ifdef	WIN32
		//todo
		INT32	result;
		GOOD_PRINT_STRING("???? %s : ", str_message);
		result = scanf( "%8192s", str_file_dialog.get() );
		GOOD_PRINT_STRING( "Trying to save %s", str_file_dialog.get());
		str_file_dialog.sync();
		b_retcode = (result == 1);
#endif	// of #ifdef	WIN32

		filename.set( str_file_dialog );

		unlock();
		return b_retcode ? (filename.is_empty() ? ERR_OBJ_NULL : AAA_OK) : ERR_CANCEL;
	}

#ifdef	WIN32
#	if	AAA_LOCAL_DIALOG_2014()

	//int MAACALLBACK BrowseForFolderCallback(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData)
	int CALLBACK BrowseForFolderCallback( HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData )
	{
		char szPath[MAX_PATH];

		switch(uMsg)
		{
		case BFFM_INITIALIZED:
			SendMessageA( hwnd, BFFM_SETSELECTION, TRUE, pData );
			break;
		case BFFM_SELCHANGED: 
			if( SHGetPathFromIDListA( (LPITEMIDLIST) lp , szPath) ) 
			{
				SendMessageA( hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szPath );	
			}
			break;
		}
		return 0;
	}
#	endif	//AAA_LOCAL_DIALOG_2014()
#endif //WIN32

	AAA_ERR get_folder( o_str & filename, C_PCHAR_C str_message, system_window* parent )
	{
		if( !try_lock() )
			return ERR_LOCKED;

		bool b_retcode = false;
#ifdef	WIN32
		str_file_dialog.convert_to_backslash();
#	if AAA_LOCAL_DIALOG_2014()
		b_retcode = sysutils::do_file_dialog_folder( str_file_dialog, str_file_dialog.get(), str_message, parent );
		//str_file_dialog.set( path );
		//sysutils::free_str_tmp( path );
		//C_PCHAR_C ret = str_file_dialog.get();
#	else
		BROWSEINFOA		bi;
		LPITEMIDLIST	pidl;

		bi.hwndOwner = get_window_main_handle();
		bi.pidlRoot = nullptr;
		bi.pszDisplayName = nullptr;
		bi.lpszTitle = str_message;
		//	BIF_BROWSEINCLUDEFILES : file too
		bi.ulFlags = BIF_USENEWUI;
		bi.lpfn = BrowseForFolderCallback;
		bi.lParam = (LPARAM)str_file_dialog.get();

		pidl = SHBrowseForFolderA( &bi );
		if(pidl)
		{
			if( SHGetPathFromIDListA( pidl, str_file_dialog.get()) )
			{
				str_file_dialog.sync();
				b_retcode = true;
	//				strcpy(lpszFolder, szPath);
			}
		}
#	endif	//#if AAA_LOCAL_DIALOG_2014()
		str_file_dialog.convert_to_slash();
#else	//WIN32
		ERR_PRINT_STRING( "%s() Unimplemented yet for this OS", __FUNCTION__ );
#endif	//WIN32
		filename.set(str_file_dialog);

		unlock();
		DBG_PRINT_STRING( "%s() Open dialog Filename is %s", __FUNCTION__, filename.get() );
		return b_retcode ? (filename.is_empty() ? ERR_STR_EMPTY : AAA_OK) : ERR_CANCEL;
	}


}	//namespace aaa::file

