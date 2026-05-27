#include "info.h"
#include "aaa_os.h"
#include "draw/aaa_glut.h"
#include "infrastructure/param/param_declare.h"
#include "file/aaa_dir.h"

//	AAA_WINDOWS_SDK_VERSION comes from aaa_sdk_version.generated.h, written
//	to this directory by the GenerateAaaSdkVersionHeader MSBuild Target on
//	every build. Source is MSBuild's $(WindowsTargetPlatformVersion). See
//	the Target definition at the end of AAASeed_by_Maa.vcxproj.
//
//	<ntverp.h>'s VER_PRODUCTBUILD was the obvious candidate but Microsoft
//	froze it at 10011 around 2015 and never updates it, so every SDK from
//	then until now reports 10.0.10011.16384 from that header -- useless.
//	The MSBuild variable is the only reliable source.
//
//	__has_include guard lets this compile cleanly on a fresh clone before
//	the first build has generated the header.
#if __has_include( "aaa_sdk_version.generated.h" )
#	include "aaa_sdk_version.generated.h"
#endif


CONSTEXPR C_PCHAR_C	g_build_str = 
#if AAA_OS_WINDOWS()
#	if AAA_WIN64()
		"Windows x64 Build";
#	elif WIN32
		"Windows x86 Build";
#	endif
#else
	"Unknown non window Build";
#	error Stoping compilation : define non window Build
#endif

FACTORY_CREATE_V1( c_info, info, License Info, info );

namespace aaa	{
namespace file	{
	extern o_str user_name;
	extern o_str net_name;
	extern o_str app_dir;
}	
}

namespace {
	//todo abandon 32 bits version ?
	//todox64 deal with non windows
	bool b_x64 = 
#if AAA_BIT64()
		true;
#else
		false;
#endif
	CONSTEXPR bool b_glut_used	= AAA_GLUT_USE()!=0;
	CONSTEXPR bool b_debug		= AAA_DEBUG()!=0;
}

namespace	n_info
{

	CONSTEXPR INT32	BASE_PARAM_NB	= 20;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	= BASE_PARAM_NB
									+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_STR_LOCKED(	exe_path		)
		PARAM_DEF_STR_LOCKED(	exe_arguments	)
		PARAM_DEF_STR_LOCKED(	exe_start_time	)
		PARAM_DEF_STR_LOCKED(	version			)
		PARAM_DEF_STR_LOCKED(	version_short	)
		PARAM_DEF_STR_LOCKED(	build			)
		PARAM_DEF_STR_LOCKED(	compiler		)
		PARAM_DEF_STR_LOCKED(	toolset			)
		PARAM_DEF_BOOL_LOCKED(	x64				)
		PARAM_DEF_BOOL_LOCKED(	debug			)
		PARAM_DEF_STR_LOCKED(	windows_sdk_version		)
		PARAM_DEF_STR_LOCKED(	kernel32_dll_version	)
		PARAM_DEF_BOOL_LOCKED(	glut_used		)

		PARAM_DEF_STR_LOCKED(	start_dir		)
		PARAM_DEF_STR_LOCKED(	kernel_dir		)
		PARAM_DEF_STR_LOCKED(	net_name		)
		PARAM_DEF_STR_LOCKED(	net_dir			)
		PARAM_DEF_STR_LOCKED(	user_name		)
		PARAM_DEF_STR_LOCKED(	user_dir		)
		PARAM_DEF_STR_LOCKED(	app_dir			)
	};
}

void	c_info::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, _exe_path				);
	param_set_pt( h, _exe_args				);
	param_set_pt( h, _exe_start_time		);
	param_set_pt( h, _version_long			);
	param_set_pt( h, _version_short			);
	param_set_pt( h, _build					);
	param_set_pt( h, _compiler				);
	param_set_pt( h, _toolset				);
	param_set_pt( h, b_x64					);
	param_set_pt( h, b_debug				);
	param_set_pt( h, _sdk_version			);
	param_set_pt( h, _kernel32_version		);
	param_set_pt( h, b_glut_used			);

	param_set_pt( h, c_dir::get_start()		);
	param_set_pt( h, c_dir::get_kernel()	);
	param_set_pt( h, aaa::file::net_name	);
	param_set_pt( h, c_dir::get_net()		);
	param_set_pt( h, aaa::file::user_name	);
	param_set_pt( h, c_dir::get_user()		);
	param_set_pt( h, aaa::file::app_dir		);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE(c_info)
{
	set_name( "Info" );	//	we don't want license too appear too easily
	param_init_with( n_info::param, n_info::PARAM_NB_MAX );

	init();
}

c_info::~c_info()
{
}

void c_info::get_kernel_version()
{
	// Get the system directory (e.g., C:\Windows\System32)
    WCHAR systemDir[MAX_PATH];
    if( GetSystemDirectory(systemDir, MAX_PATH) == 0)
	{
        std::cerr << "Failed to get system directory. Error: " << GetLastError() << std::endl;
        return;
    }

    // Construct the full path to kernel32.dll
    std::wstring dllPath = std::wstring(systemDir) + L"\\kernel32.dll";
	
    // Get version information
    DWORD dummy;
    DWORD versionInfoSize = GetFileVersionInfoSize( dllPath.c_str(), &dummy);
    if( versionInfoSize == 0 )
	{
        std::cerr << "Failed to get version info size. Error: " << GetLastError() << std::endl;
        return;
    }

    std::vector<BYTE> versionInfo(versionInfoSize);
    if( !GetFileVersionInfo( dllPath.c_str(), 0, versionInfoSize, versionInfo.data()) )
	{
        std::cerr << "Failed to get version info. Error: " << GetLastError() << std::endl;
        return;
    }

    VS_FIXEDFILEINFO* fileInfo;
    UINT fileInfoSize;
    if( !VerQueryValue(versionInfo.data(), L"\\", (LPVOID*)&fileInfo, &fileInfoSize) )
	{
        std::cerr << "Failed to query version info. Error: " << GetLastError() << std::endl;
        return;
    }
	_kernel32_version.set( HIWORD(fileInfo->dwProductVersionMS) );
	_kernel32_version.add_char( '.' );
	_kernel32_version.add( LOWORD(fileInfo->dwProductVersionMS) );
	_kernel32_version.add_char( '.' );
	_kernel32_version.add( HIWORD(fileInfo->dwProductVersionLS) );
	_kernel32_version.add_char( '.' );
	_kernel32_version.add( LOWORD(fileInfo->dwProductVersionLS) );
}

void	c_info::init()
{
	_build.set( g_build_str );
	
#if WDK_NTDDI_VERSION < NTDDI_WIN11_GE
#	error "use a more recent windows SDK like 10.0.26100.0"
#endif
	//	See aaa_sdk_version.generated.h comment block near the top of this
	//	file for the rationale.
#ifdef AAA_WINDOWS_SDK_VERSION
	_sdk_version.set( AAA_WINDOWS_SDK_VERSION );
#else
	_sdk_version.set( "Unknown (aaa_sdk_version.generated.h missing -- build with MSBuild)" );
#endif
	get_kernel_version();

	time_t aclock;
	time( &aclock );
	//Get time in seconds
	struct	tm* ti = localtime( &aclock );	//Convert time to struct tm form

	_exe_start_time.add( ti->tm_year + 1900 );
	_exe_start_time.add_char( '_' );
	_exe_start_time.add( ti->tm_mon + 1 );
	_exe_start_time.add_char( '_' );
	_exe_start_time.add( ti->tm_mday );
	_exe_start_time.add_char( '_' );
	_exe_start_time.add( ti->tm_hour );
	_exe_start_time.add( "h_" );
	_exe_start_time.add( ti->tm_min );
	_exe_start_time.add( "m_" );
	_exe_start_time.add( ti->tm_sec );
	_exe_start_time.add_char( 's' );

	update();
}

void	c_info::set_exe( INT32 CONST argc, char** argv )
{
	_exe_path.set( argv[0] );
	_exe_path.convert_to_slash();
	for( INT32 i=1; i<argc; ++i )
	{
		if( i != 1 )
			_exe_args.add_space();
		_exe_args.add( argv[i] );
	}
}

//todo extend using https://www.codeproject.com/Articles/8628/Retrieving-version-information-from-your-local-app
void	c_info::update()
{
	if( _version_long.is_empty() )
	{
		INT32 CONSTEXPR MEM_SIZE = 512;
		CHAR	mem[MEM_SIZE];
		CHAR	str[256];
		LPSTR	lpstrVffInfo = mem; 

		//get version info
		wchar_t	szFullPath[256];
		::GetModuleFileNameW( nullptr, szFullPath, sizeof(szFullPath)); 
		if( ::GetFileVersionInfoW(	szFullPath,		// pointer to filename string
									0,				// ignored
									MEM_SIZE,		// size of buffer
									lpstrVffInfo	// pointer to buffer to receive
									)
								)
		{
			UINT32	byte_nb;
			VS_FIXEDFILEINFO*	version_info = nullptr;
			if( ::VerQueryValueW(	lpstrVffInfo, // address of buffer for version resource
									TEXT("\\"), 
									(LPVOID*) &version_info, 
									&byte_nb
									) 
								)
			{
				if( version_info->dwSignature == 0xfeef04bd )
				{
					_version.set_all(	HIWORD(version_info->dwProductVersionMS),
										LOWORD(version_info->dwProductVersionMS),
										HIWORD(version_info->dwProductVersionLS),
										LOWORD(version_info->dwProductVersionLS)
									);
					_version.sprint_version( str );
					_version_long.set( str );
					_version.sprint_version_short( str );
					_version_short.set( str );
				}
			}
		}
	}
	if( _compiler.is_empty() )
	{
		_compiler.set( AAA_COMPILER_STR() );
#ifdef _MSC_VER
		_compiler.add( " : " );
		_compiler.add( _MSC_FULL_VER );
		_compiler.add( " Language " );
		_compiler.add( (UINT32)_MSVC_LANG );
#endif
	}
	if( _toolset.is_empty() )
	{
		_toolset.set( AAA_VSTOOL() );
	}
}

c_info*	g_info = nullptr;

#if	0
//--------------------------------------------------------------------------- 
// 
// FUNCTION:	GetStringRes (int id INPUT ONLY) 
// 
// COMMENTS:	Load the resource string with the ID given, and return a 
//				pointer to it.  Notice that the buffer is common memory so 
//				the string must be used before this call is made a second time. 
// 
//--------------------------------------------------------------------------- 
 
LPTSTR	GetStringRes (int id ) 
{
	static	TCHAR	buffer[MAX_PATH];

	buffer[0] = 0;
	LoadString (GetModuleHandle nullptr, id, buffer, MAX_PATH);
	return buffer;
}

// 
//  FUNCTION: About(HWND, unsigned, WORD, LONG) 
// 
//  PURPOSE:  Processes messages for "About" dialog box 
//       This version allows greater flexibility over the contents of the 'About' box, 
//       by pulling out values from the 'Version' resource. 
// 
//  MESSAGES: 
//
// WM_INITDIALOG - initialize dialog box
// WM_COMMAND    - Input received
//
//
//LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
void	test()
{
	static	HFONT	hfontDlg;		// Font for dialog text
	static	HFONT	hFinePrint;	// Font for 'fine print' in dialog
	DWORD	dwVerInfoSize;		// Size of version information block
	LPSTR	lpVersion;			// String pointer to 'version' text
	DWORD	dwVerHnd=0;			// An 'ignored' parameter, always '0'
	UINT	uVersionLen;
	WORD	wRootLen;
	BOOL	bRetCode;
	int		i;
	char	szFullPath[256];
	char	szResult[256];
	char	szGetName[256];
	DWORD	dwVersion;
	char	szVersion[40];
	DWORD	dwResult;

//		ShowWindow (hDlg, SW_HIDE); 

	if (PRIMARYLANGID(GetUserDefaultLangID()) == LANG_JAPANESE)
	{
		hfontDlg = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0,
								VARIABLE_PITCH | FF_DONTCARE, "");
		hFinePrint = CreateFont(11, 0, 0, 0, 0, 0, 0, 0, SHIFTJIS_CHARSET, 0, 0, 0,
									VARIABLE_PITCH | FF_DONTCARE, "");
	}
	else
	{
		hfontDlg = CreateFont(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								VARIABLE_PITCH | FF_SWISS, "");
		hFinePrint = CreateFont(11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
								VARIABLE_PITCH | FF_SWISS, "");
	}
	GetModuleFileName ( nullptr, szFullPath, sizeof(szFullPath));

	// Now lets dive in and pull out the version information:
	dwVerInfoSize = GetFileVersionInfoSize(szFullPath, &dwVerHnd );
	if (dwVerInfoSize)
	{
		LPSTR	lpstrVffInfo;
		HANDLE	hMem;
		hMem = GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpstrVffInfo  = (char *)GlobalLock(hMem);
		GetFileVersionInfo(szFullPath, dwVerHnd, dwVerInfoSize, lpstrVffInfo);
		// The below 'hex' value looks a little confusing, but
		// essentially what it is, is the hexadecimal representation
		// of a couple different values that represent the language
		// and character set that we are wanting string values for.
		// 040904E4 is a very common one, because it means:
		//   US English, Windows MultiLingual character set
		// Or to pull it all apart:
		// 04------        = SUBLANG_ENGLISH_USA
		// --09----        = LANG_ENGLISH
		// --11----        = LANG_JAPANESE
		// ----04E4 = 1252 = Codepage for Windows:Multilingual

//		lstrcpy(szGetName, GetStringRes(IDS_VER_INFO_LANG));
//		wRootLen = (WORD) lstrlen(szGetName); // Save this position

		// Set the title of the dialog:
		lstrcat (szGetName, "ProductName");

		bRetCode = VerQueryValue((LPVOID)lpstrVffInfo,
					TEXT("\\StringFileInfo\\040904E4\\FileDescription"),
					//(LPSTR)szGetName,
					(LPVOID *)&lpVersion,
					(UINT *)&uVersionLen );

		// Notice order of version and string...
		if (PRIMARYLANGID(GetUserDefaultLangID()) == LANG_JAPANESE)
		{
			lstrcpy(szResult, lpVersion);
			lstrcat(szResult, "    o [ W        ");
		}
		else
		{
			lstrcpy(szResult, "About ");
			lstrcat(szResult, lpVersion);
		}

		// -----------------------------------------------------

/*	SetWindowText (hDlg, szResult);

		// Walk through the dialog items that we want to replace:
		for (i = DLG_VERFIRST; i <= DLG_VERLAST; ++i )
		{
			GetDlgItemText(hDlg, i, szResult, sizeof(szResult));
			szGetName[wRootLen] = (char)0;
			lstrcat (szGetName, szResult);
			uVersionLen   = 0;
			lpVersion     = nullptr;
			bRetCode      =  VerQueryValue((LPVOID)lpstrVffInfo,
									(LPSTR)szGetName,
									(LPVOID)&lpVersion,
									(UINT *)&uVersionLen);

			if ( bRetCode && uVersionLen && lpVersion)
			{
				// Replace dialog item text with version info
				lstrcpy(szResult, lpVersion);
				SetDlgItemText(hDlg, i, szResult);
			}
			else
			{
				dwResult = GetLastError();

				wsprintf(szResult, GetStringRes(IDS_VERSION_ERROR), dwResult);
				SetDlgItemText (hDlg, i, szResult);
			}
			SendMessage (GetDlgItem (hDlg, i), WM_SETFONT,
							(ULONG_PTR)((i==DLG_VERLAST)?hFinePrint:hfontDlg),
							TRUE);
		}	// for (i = DLG_VERFIRST; i <= DLG_VERLAST; ++i )
*/

		GlobalUnlock(hMem);
		GlobalFree(hMem);
	}
	else
	{
		// No version information available.
	} // if (dwVerInfoSize)

/*
	SendMessage( GetDlgItem (hDlg, IDC_LABEL), WM_SETFONT,
					(WPARAM)hfontDlg,(LPARAM)TRUE);

	// We are  using GetVersion rather then GetVersionEx
	// because earlier versions of Windows NT and Win32s
	// didn't include GetVersionEx:
	dwVersion = GetVersion();

	if (dwVersion < 0x80000000)
	{
		// Windows NT
		wsprintf (szVersion, "Microsoft Windows NT %u.%u (Build: %u)",
					(DWORD)(LOBYTE(LOWORD(dwVersion))),
					(DWORD)(HIBYTE(LOWORD(dwVersion))),
					(DWORD)(HIWORD(dwVersion)) );
	}
	else if (LOBYTE(LOWORD(dwVersion))<4)
	{
		// Win32s
		wsprintf (szVersion, "Microsoft Win32s %u.%u (Build: %u)",
				(DWORD)(LOBYTE(LOWORD(dwVersion))),
				(DWORD)(HIBYTE(LOWORD(dwVersion))),
				(DWORD)(HIWORD(dwVersion) & ~0x8000) );
	}
	else
	{
		// Windows 95
		wsprintf (szVersion, "Microsoft Windows 95 %u.%u",
				(DWORD)(LOBYTE(LOWORD(dwVersion))),
				(DWORD)(HIBYTE(LOWORD(dwVersion))) );
	}

		SetWindowText (GetDlgItem(hDlg, IDC_OSVERSION), szVersion); 
		ShowWindow (hDlg, SW_SHOW); 
*/

}
#endif	//
