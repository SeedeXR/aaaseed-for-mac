
#ifdef AAA_SYSTEMUTILS_H
#error "SYSTEMUTILS_H included more than once."
#endif
#define AAA_SYSTEMUTILS_H 1


///////////////////////////////////////////////////////////////////////////////////////////////////
//		Includes
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef AAA_NATIVE_SYSTEM_H
#	include "system/Native_System.h"
#endif
#ifndef AAA_NATIVE_TYPES_H
#	include "system/Native_Types.h"
#endif
#if defined (NATIVE_OS_WIN32)
#	include <Shtypes.h>
#endif


#ifndef _SSTREAM_
#	include <sstream>
#endif
#ifndef AAA_SYSTEM_ERROR_H
#	include "system/win32/SystemError.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "code_utils/aaa_str.h"
#endif

class system_window;

namespace sysutils {
	
	//////////////////////////////////////
	//									//
	//				METHOD				//
	//									//
	//////////////////////////////////////

	void init_sys();
	void deinit_sys();

	template<class T>
	static	void	safe_release( T*& p )
	{
		if(p)
		{
			p->Release();
			p = nullptr;
		}
	}


	void utf8_to_unicode( wchar_t* dst, INT32 CONST size, char const* text, INT32 CONST size_text = -1 );

	/**
	* @brief convert char pointer to unicode wchar_t pointer
	* @param p_text text to convert as char pointer
	* @return value as wchar_t pointer that should be deleted (array)
	*/
	wchar_t * utf8_to_unicode( const char* text );
	void free_str_tmp( wchar_t*& pt );

	/**
	* @convert unicode to char
	* @param p_text text to convert as char pointer
	* @return value as char pointer that should be deleted (array)
	*/
	char * toUtf8Char( wchar_t* text );
	void free_str_tmp( char*& pt );

	/**
	* @brief convert numerical type to string
	* @param p_value value to convert
	* @return converted string as std::string
	*/
	template <class T>
	std::string numericalToString( const T & p_value )
	{
		std::string Str_Return;

		std::ostringstream ost;
		ost << p_value;

		Str_Return = ost.str();

		return Str_Return;
	}

	/**
	* @brief convert float to string
	* @param p_floatValue value to convert as float
	* @return converted string as std::string
	*/
	std::string floatToString( const float & p_floatValue );

	/**
	* @brief convert seconds in HH : MM : SS string
	* @param p_seconds seconds to convert as MWMSint32_t
	* @return converted seconds as std::string
	*/
	std::string secondsToHmsString( const int32_t & p_seconds );

	/**
	* @brief make error as text
	* @param err_id windows return code got with windows GetLastError()
	* @Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	*/
	std::string make_error_message( DWORD err_id );

	/**
	* @brief make error as text
	* @Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	*/
	std::string get_error_message();

	/**
	* @brief test file existence
	* @return true if file exists, false otherwise
	*/
	bool doesFileExist( const std::string & p_path );


	/**
	* @brief copy file
	* @return true if file exists, false otherwise
	*/
	bool copyFile(const std::string & p_pathSource, const std::string & p_pathDestination);


	/**
	* @brief remove file name from path
	* @param p_path target path as std::string
	* @return new path as std::string
	*/
	std::string removeFileNameFromPath( const std::string & p_path );


	/**
	* @brief remove extension from file name
	* @param p_name target file name as std::string
	* @return new name as std::string
	*/
	std::string removeExtensionFromFileName( const std::string & p_name );


	/**
	* @brief get file name from path
	* @param p_path target path as std::string
	* @return file name as std::string
	*/
	std::string getFileNameFromPath( const std::string & p_path );

#if defined (NATIVE_OS_WIN32)
	/**
	* @brief convert extensions to filter spec
	* @param p_extensions target extensions list as std::vector<std::string> 
	* @return filter spec as COMDLG_FILTERSPEC
	*/
	COMDLG_FILTERSPEC extensionsToFileterSpec( std::pair<std::vector<std::string>,std::string> p_extensions );
#endif //defined (NATIVE_OS_WIN32)

	/**
	* @brief open file 
	* @note call folder browsing window
	*/
	//todo fix allocation/deallocation
	// the returning value have to be deleted as an array
	//	these two fns implement the next ones
	//LPWSTR do_dialog_file_common(	const LPWSTR target_path,	COMDLG_FILTERSPEC* extensions,								bool b_save,	bool b_folder, bool b_multiple );
	//LPWSTR do_dialog_file_common(	const char* target_path,	const char* extensions,										bool b_save,	bool b_folder, bool b_multiple );

	bool do_file_dialog_load(	o_str& dst,	const LPWSTR target_path,	const LPWSTR message,	COMDLG_FILTERSPEC* extensions,	system_window* parent,		bool b_folder, bool b_multiple	);
	bool do_file_dialog_load(	o_str& dst,	const char* target_path,	C_PCHAR_C message,		const char* extensions,			system_window* parent,		bool b_folder, bool b_multiple	);

	bool do_file_dialog_save(	o_str& dst,	const LPWSTR target_path,	const LPWSTR message,	COMDLG_FILTERSPEC* extensions,	system_window* parent		);
	bool do_file_dialog_save(	o_str& dst,	const char* target_path,	C_PCHAR_C message,		const char* extensions,			system_window* parent		);

	bool do_file_dialog_folder(	o_str& dst,	const LPWSTR target_path,	const LPWSTR message,									system_window* parent		);
	bool do_file_dialog_folder(	o_str& dst,	const char* target_path,	C_PCHAR_C message,										system_window* parent		);

	/**
	* @brief open file browser to easily drop files on GUI
	* @param p_extensions target extensions list as std::vector<std::string> 
	* @param p_targetPath target path to start browsing as std::string (default to empty)
	* @note call folder browsing window
	*/
	void openDropBrowser( std::pair<std::vector<std::string>,std::string> p_extensions, const std::string & p_targetPath="" );


	/**
	* @brief normalize path to fit system requirements
	* @param p_path path to normalize as std::string
	*/
	std::string normalizePath( const std::string & p_path );


	/**
	* @brief get the exe Path
	* @return executable path as std::string
	*/
	std::string ExePath( void );


	/**
	* @brief list target directory files
	* @param p_pathDirectory target directory path as std::string
	* @return files list as std::vector<std::string>
	*/
	std::vector<std::string> listDirectoryFiles( const std::string & p_pathDirectory );


	/**
	* @brief create directory
	* @param p_path new directory path
	* @return true on success, false otherwise
	*/
	bool createDirectory( const std::string & p_path );


	/**
	* @brief open default web browser and go to target url
	* @param p_url target url as std::string
	*/
	void openWebBrowser( const std::string & p_url );


	/**
	* @brief copy string to clipboard
	* @param p_text text to copy as std::string
	*/
	void copyToClipboard( const std::string & p_text );
	/**
	* @brief get text from clipboard
	* @return text as std::string
	*/
	std::string getTextFromClipboard( void );
	
	/** \! Get control key state at the time a key is pressed.*/
	bool getKeyStateCtrl( void );
	/** \! Get alt key state at the time a key is pressed. */
	bool getKeyStateAlt( void );
	/** \! Get shift key state at the time a key is pressed. */
	bool getKeyStateShift( void );

	/** \! Get no special key down (ctrl/alt/shift). */
	bool getModifierState( void );


	/**
	* @brief get control key state at the time the function is called (on OSX this value is cmd key state)
	* @return true if key is pressed
	*/
	bool getActualKeyStateCtrl( void );
	/**
	* @brief get alt key state at the time the function is called
	* @return true if key is pressed
	*/
	bool getActualKeyStateAlt( void );
	/**
	* @brief get shift key state at the time the function is called
	* @return true if key is pressed
	*/
	bool getActualKeyStateShift( void );
	/**
	* @brief get space bar state at the time the function is called
	* @return true if key is pressed
	*/
	bool getActualKeyStateSpace( void );


	/**
	 * @brief set control key state (on OSX this value is cmd key state)
	 * @param p_bState true if key is pressed, false otherwise
	*/
	void setActualKeyStateCtrl( const bool & p_bState );
	/**
	  * @brief set alt key state
	  * @param p_bState true if key is pressed, false otherwise
	  */
	void setActualKeyStateAlt( const bool & p_bState );
	/**
	 * @brief set shift key state
	 * @param p_bState true if key is pressed, false otherwise
	 */
	void setActualKeyStateShift( const bool & p_bState );
	/**
	 * @brief set space key state
	 * @param p_bState true if key is pressed, false otherwise
	 */
	void setActualKeyStateSpace( const bool & p_bState );


	/**
	get monitor nb
	@return int32_t
	*/
	int32_t get_monitor_nb( void );
	/**
	get primary monitor width
	@return int32_t
	*/
	int32_t get_primary_monitor_sx( void );
	/**
	get primary monitor height
	@return int32_t
	*/
	int32_t get_primary_monitor_sy( void );

	/**
	* @brief get main monitor width
	* @return main monitor width as int32_t
	*/
	int32_t getMainMonitorWidth( void );
	/**
	* @brief get main monitor height
	* @return main monitor height as int32_t
	*/
	int32_t getMainMonitorHeight( void );

	/**
	* @brief get virtual screen position x
	* @return virtual screen position x as int32_t
	*/
	int32_t get_virtual_screen_x( void );
	/**
	* @brief get virtual screen position y
	* @return virtual screen position y as int32_t
	*/
	int32_t get_virtual_screen_y( void );

	/**
	* @brief get virtual screen width
	* @return virtual screen width as int32_t
	*/
	int32_t get_virtual_screen_sx( void );
	/**
	* @brief get virtual screen height
	* @return virtual screen height as int32_t
	*/
	int32_t get_virtual_screen_sy( void );


	/**
	* @brief create error window
	* @param p_title window title as std::string
	* @param p_text window text as std::string, text is formated as in printf etc
	* @return true at user validation
	*/
	bool createErrorWindow( const std::string & p_title, const std::string & p_text );


	/**
	* @brief create fatal error window
	* @param p_text window text as std::string, text is formated as in printf etc
	* @note application will terminate when the window is closed 
	*/
	void createFatalErrorWindow( const std::string & p_text );


	/**
	* @brief create error window
	* @param p_title window title as std::string
	* @param p_text window text as std::string, text is formated as in printf etc
	* @return true at user validation, false at user cancelation
	*/
	bool createWarningWindow( const std::string & p_title, const std::string & p_text );


	/**
	* @brief get file extension from path utility function
	* 
	* @param wchar_t * p_pFileName
	* @param wchar_t * p_pExtension
	* 
	* @note used to test loaded file extension type
	*/
	void extensionFromFilename( wchar_t * p_pFileName, wchar_t * p_pExtension );


	/**
	* @brief get file extension from path utility function
	* 
	* @param p_pFileName file name as std::string
	* @param file extension as std::string
	* 
	* @note used to test loaded file extension type
	*/
	std::string extensionFromFilename( std::string p_pFileName );


	// Known folder(s) path helpers.

#if defined(NATIVE_OS_WIN32)
	enum class FOLDER_KNOWN
	{
		DOCUMENTS,
		PROGRAM_FILES_x86,
		PROGRAM_FILES_x64,
		PROGRAM_DATA,
		APP_DATA_LOCAL,
		APP_DATA_ROAMING,
		DESKTOP,
		FOLDER_KNOWN_NB
	};

	bool get_folder_path_known(	char*		CONST	dst,	FOLDER_KNOWN CONST	folder_type );	
	bool get_folder_path_known( wchar_t*	CONST	dst,	FOLDER_KNOWN CONST	folder_type );
	bool get_folder_path_known(	o_str*				dst,	FOLDER_KNOWN CONST 	folder_type );

#endif
}

