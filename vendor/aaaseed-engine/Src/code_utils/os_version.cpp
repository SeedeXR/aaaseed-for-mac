
#include "os_version.h"
#include "err.h"

#include <stdio.h>
#include <string.h>
#if AAA_OS_WINDOWS()
#	include <tchar.h>
#endif

#if AAA_OS_WINDOWS()
//define _USING_V110_SDK71_ 1
#	ifndef _USING_V110_SDK71_
#		include "aaa_os.h"
#		include <VersionHelpers.h>
#	endif

#	ifndef AAA_SYSTEMUTILS_H
#		include "system/shared/SystemUtils.h"
#	endif


#	include <lib_use.h>
//	Reg* APIs used below live in advapi32; kernel32 is auto-linked by the
//	CRT so GetVersion/GetModuleHandleW/GetProcAddress need no explicit lib.
AAA_LIB_USE_MESSAGE( "Advapi32" )

typedef	void	(WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef	BOOL	(WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);

//	RtlGetVersion (exported by ntdll.dll) bypasses the GetVersion /
//	GetVersionEx compatibility-manifest shim that caps reported version at
//	Windows 8 when no explicit app manifest is present. Signature returns
//	NTSTATUS (== LONG); pointer type matches RTL_OSVERSIONINFOW which has
//	the same layout as OSVERSIONINFOW so we can reuse POSVERSIONINFOW here
//	without pulling in <winternl.h>.
typedef LONG (WINAPI *RtlGetVersionPtr)( POSVERSIONINFOW );
#elif AAA_OS_MAC()
#	include <sys/sysctl.h>
#	include <sys/utsname.h>
#	include <unistd.h>
#	include <pwd.h>
#endif

namespace	n_os_version
{
	o_str	o_vendor;
	o_str	o_name;
	bool	b_windows		= AAA_OS_WINDOWS() == 1;
	bool	b_macintosh		= AAA_OS_MAC() == 1;
	bool	b_linux			= AAA_OS_LINUX() == 1;
#ifdef _USING_V110_SDK71_
	o_str	o_version;
	INT32	version_major;
	INT32	version_minor;
	INT32	build_number;
	o_str	sp_number;
	bool	b_x64;
#else
	//	Point-in-time flags: exactly one is true for a recognised Windows.
	bool	b_xp;
	bool	b_vista;
	bool	b_win7;
	bool	b_win8;
	bool	b_win81;
	bool	b_win10;
	bool	b_win11;
	//	Cumulative flags: monotone -- multiple may be true at once.
	bool	b_win7_or_more;
	bool	b_win8_or_more;
	bool	b_win10_or_more;
#endif

#if AAA_OS_MAC()
	//	macOS-specific fields populated by get_info_mac().
	o_str	version_string;
	INT32	version_major		= 0;
	INT32	version_minor		= 0;
	INT32	version_patch		= 0;
	o_str	kernel_version;
	o_str	architecture;
	bool	b_arm64				= false;
	bool	b_x64				= false;
	INT32	cpu_logical		= 0;
	INT32	cpu_physical		= 0;
	INT64	memory_size_bytes	= 0;
	o_str	host_name;
	o_str	user_name;
#endif

#if AAA_OS_WINDOWS()
#ifndef _WIN32_WINNT_WIN10
#	define	_WIN32_WINNT_WIN10	0x0a00
#endif

#ifndef VERSIONHELPERAPI
#	define	VERSIONHELPERAPI	bool
#endif //VERSIONHELPERAPI

#ifndef _USING_V110_SDK71_
	VERSIONHELPERAPI
	IsWindowsVersion10OrGreater()
	{
		return IsWindowsVersionOrGreater( HIBYTE(_WIN32_WINNT_WIN10), LOBYTE(_WIN32_WINNT_WIN10), 0 );
	}
#else
	bool IsWindowsVersion10OrGreater() {	return false;	}
#endif


	bool IsWindowsVersionOrGreater( WORD majorVersion, WORD minorVersion, WORD servicePackMajor )
	{
		OSVERSIONINFOEX osvi {};
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		osvi.dwMajorVersion = majorVersion;
		osvi.dwMinorVersion = minorVersion;
		osvi.wServicePackMajor = servicePackMajor;

		DWORDLONG conditionMask = 0;
		VER_SET_CONDITION(conditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
		VER_SET_CONDITION(conditionMask, VER_MINORVERSION, VER_GREATER_EQUAL);
		VER_SET_CONDITION(conditionMask, VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

		return VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, conditionMask) != FALSE;
	}

//todo now (2023 Feb) we don't detect correctly windows 10
//todo we need to detect windows 11 too
	void	get_info_window()
	{
		//{
		//	DWORD dwVersion = 0; 
		//	DWORD dwMajorVersion = 0;
		//	DWORD dwMinorVersion = 0; 
		//	DWORD dwBuild = 0;

		//	dwVersion = GetVersion();
 
		//	// Get the Windows version.
		//	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
		//	dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));

		//	// Get the build number.
		//	if (dwVersion < 0x80000000)              
		//		 dwBuild = (DWORD)(HIWORD(dwVersion));

		//	DBG_PRINT_STRING( "Version is %d.%d (%d)\n", dwMajorVersion, dwMinorVersion, dwBuild );
		//}


		o_vendor	= "Microsoft";
		o_name.erase();

#if 1
		//	Read ProductName from the registry. Defensive against both
		//	RegQueryValueExA failure (leaves buffer uninitialized) and the
		//	documented REG_SZ-may-not-be-null-terminated footgun.
		{
			HKEY hKey;
			LPCSTR keyPath = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion";
			LONG result = RegOpenKeyExA( HKEY_LOCAL_MACHINE, keyPath, 0, KEY_QUERY_VALUE, &hKey );
			if( result == ERROR_SUCCESS )
			{
				char product_name[256];
				DWORD size = sizeof(product_name);
				result = RegQueryValueExA( hKey, "ProductName", NULL, NULL, (LPBYTE)product_name, &size );
				if( result == ERROR_SUCCESS )
				{
					//	REG_SZ is not guaranteed null-terminated by the API.
					//	Clamp to buffer-1 and force a terminator defensively.
					if( size >= sizeof(product_name) )
						size = sizeof(product_name) - 1;
					product_name[size] = 0;
					o_name.set( product_name );
				}
				RegCloseKey( hKey );
			}
		}

		//	Use RtlGetVersion (ntdll) to get the real kernel version --
		//	GetVersion / GetVersionEx lie (cap at 6.2) on Win10+ without an
		//	explicit compatibility manifest. Fall back to GetVersion() if
		//	the symbol is unavailable.
		DWORD dwMajorVersion = 0;
		DWORD dwMinorVersion = 0;
		DWORD dwBuildNumber  = 0;

		HMODULE CONST hNtDll = GetModuleHandleW( L"ntdll.dll" );
		if( hNtDll )
		{
			RtlGetVersionPtr CONST pRtlGetVersion = (RtlGetVersionPtr) GetProcAddress( hNtDll, "RtlGetVersion" );
			if( pRtlGetVersion )
			{
				OSVERSIONINFOW rovi {};
				rovi.dwOSVersionInfoSize = sizeof(rovi);
				if( pRtlGetVersion( &rovi ) == 0 )	//	STATUS_SUCCESS == 0
				{
					dwMajorVersion = rovi.dwMajorVersion;
					dwMinorVersion = rovi.dwMinorVersion;
					dwBuildNumber  = rovi.dwBuildNumber;
				}
			}
		}
		if( dwMajorVersion == 0 )
		{
			//	Fallback: deprecated GetVersion(). May be capped at 6.2 but
			//	still keeps the flags sensible for any modern Windows.
			DWORD CONST dwVersion = GetVersion();
			dwMajorVersion = LOBYTE( LOWORD(dwVersion) );
			dwMinorVersion = HIBYTE( LOWORD(dwVersion) );
			if( dwVersion < 0x80000000 )
				dwBuildNumber = HIWORD( dwVersion );
		}

		//	Derive all b_win* flags from the real kernel version. Point-in-time
		//	flags are set from the version ladder; cumulative "_or_more" flags
		//	are built FROM the point-in-time ones so adding a new Windows
		//	later is a single new branch + one new flag, with the cumulative
		//	flags rippling automatically.
		//	Reset first so get_info() is safely callable more than once.
		b_xp     = false;
		b_vista  = false;
		b_win7   = false;
		b_win8   = false;
		b_win81  = false;
		b_win10  = false;
		b_win11  = false;

		if( dwMajorVersion == 5 )						//	XP (5.1) or Server 2003 / XP-x64 (5.2)
			b_xp = true;
		else if( dwMajorVersion == 6 && dwMinorVersion == 0 )
			b_vista = true;
		else if( dwMajorVersion == 6 && dwMinorVersion == 1 )
			b_win7 = true;
		else if( dwMajorVersion == 6 && dwMinorVersion == 2 )
			b_win8 = true;
		else if( dwMajorVersion == 6 && dwMinorVersion == 3 )
			b_win81 = true;
		else if( dwMajorVersion == 10 && dwBuildNumber < 22000 )
			b_win10 = true;
		else if( dwMajorVersion == 10 && dwBuildNumber >= 22000 )
			b_win11 = true;

		b_win10_or_more = b_win10 || b_win11;
		b_win8_or_more  = b_win8  || b_win81 || b_win10_or_more;
		b_win7_or_more  = b_win7  || b_win8_or_more;

		//	Registry's ProductName can still read "Windows 10 Pro" on in-place
		//	upgrades to Win11 -- override with the correct string.
		if( b_win11 )
			o_name.set( "Windows 11" );

#else
		b_win11			= false;
		b_win10_or_more	= false;
		b_win10			= false;
		b_win81			= false;
		b_win8_or_more	= false;
		b_win8			= false;
		b_win7_or_more	= false;
		b_win7			= false;
		b_vista			= false;
		b_xp			= false;

		{
			OSVERSIONINFO osvi {};
			osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			if( GetVersionEx(&osvi) != 0 )
			{

/*
			HMODULE hMod = GetModuleHandleW( L"ntdll.dll" );
			if( hMod != nullptr )
			{
				RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress( hMod, "RtlGetVersion" );
				if( RtlGetVersion != nullptr )
					{
						RTL_OSVERSIONINFOW osvi {};
						osvi.dwOSVersionInfoSize = sizeof(osvi);

						if (RtlGetVersion(&osvi) == 0)
						{
*/
/*
				std::string getOperatingSystemName()
				{
					if( IsWindowsVersionOrGreater( 10, 0, 0 ) )
						return "Windows 10";
					else if( IsWindowsVersionOrGreater( 6, 3, 0 ) )
						return "Windows 8.1";
					else if (IsWindowsVersionOrGreater(6, 2, 0))
						return "Windows 8";
					else if (IsWindowsVersionOrGreater(6, 1, 0))
						return "Windows 7";
					else if (IsWindowsVersionOrGreater(6, 0, 0))
						return "Windows Vista";
					else if (IsWindowsVersionOrGreater(5, 2, 0))
						return "Windows Server 2003";
					else if (IsWindowsVersionOrGreater(5, 1, 0))
						return "Windows XP";
					else if (IsWindowsVersionOrGreater(5, 0, 0))
						return "Windows 2000";
					else
						return "Unknown";
				}
*/
				{
					DWORD handle;
					DWORD infoSize = GetFileVersionInfoSizeA( "C:\\Windows\\System32\\ntoskrnl.exe", &handle );
					if( infoSize == 0 )
					{
						//std::cerr << "Failed to retrieve file version info size. Error: " << GetLastError() << std::endl;
					}

					std::vector<BYTE> buffer(infoSize);
					if( !GetFileVersionInfoA("C:\\Windows\\System32\\ntoskrnl.exe", handle, infoSize, buffer.data()) )
					{
						//std::cerr << "Failed to retrieve file version info. Error: " << GetLastError() << std::endl;
					}

					VS_FIXEDFILEINFO* fileInfo = nullptr;
					UINT fileInfoLen = 0;
					if( !VerQueryValueA(buffer.data(), "\\", reinterpret_cast<LPVOID*>(&fileInfo), &fileInfoLen) )
					{
						//std::cerr << "Failed to retrieve fixed file info. Error: " << GetLastError() << std::endl;
					}

					WORD majorVersion = HIWORD(fileInfo->dwProductVersionMS);
					WORD minorVersion = LOWORD(fileInfo->dwProductVersionMS);
					WORD buildNumber = HIWORD(fileInfo->dwProductVersionLS);

					//return std::to_string(majorVersion) + "." + std::to_string(minorVersion) + "." + std::to_string(buildNumber);
				}
#if 0
				DWORD dwVersion = GetVersion();

				// Extracting the version information
				DWORD dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));
				DWORD dwMinorVersion = (DWORD)(HIBYTE(LOWORD(dwVersion)));
#endif
				//				if( IsWindowsVersionOrGreater( 11, 0, 0 ) )
				if(		osvi.dwMajorVersion == 11 && osvi.dwMinorVersion == 0 )
				{
					b_win11			= true;
					b_win10_or_more	= true;
					b_win10			= true;
					b_win8_or_more	= true;
					b_win7_or_more	= true;
					o_name	= "Windows 11";
				}
//				else if( IsWindowsVersionOrGreater( 10, 0, 0 ) )
				else if( osvi.dwMajorVersion == 10 && osvi.dwMinorVersion == 0 )
				{
					b_win10_or_more	= true;
					b_win10			= true;
					b_win8_or_more	= true;
					b_win7_or_more	= true;
					o_name	= "Windows 10";
				}
//				else if( IsWindowsVersionOrGreater( 6, 3, 0 ) )
				else if( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 3 )
				{
					b_win8_or_more	= true;
					b_win7_or_more	= true;
					o_name	= "Windows 8.1";
				}
//				else if( IsWindowsVersionOrGreater( 6, 2, 0 ) )
				else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2)
				{
					b_win8_or_more	= true;
					b_win7_or_more	= true;
					o_name	= "Windows 8";
				}
				else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1)
				{
					b_win7_or_more	= true;
					b_win7 = true;
					o_name	= "Windows 7";
				}
				else if(osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
				{
					b_vista = true;
					o_name	= "Windows Vista";
				}
				else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
				{
					o_name	= "Windows Server 2003";
				}
				else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
				{
					b_xp = true;
					o_name	= "Windows XP";
				}
				else if(osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
				{
					o_name	= "Windows 2000";
				}
			}
		}

		if( !o_name.is_empty() )
		{

//todo the #else don't compile and should perhaps be removed
#ifndef _USING_V110_SDK71_


// todo
//	IsWindows10OrGreater returns false unless the application contains a manifest that includes a compatibility section
//		that contains the GUID that designates Windows 10.
			if( IsWindowsVersion10OrGreater() )	// easier API to determine Windows version
			{
				b_win10_or_more	= true;
				b_win10			= true;
				b_win8_or_more	= true;
				b_win7_or_more	= true;
				o_name	= "Windows 10";
			}
// todo
//	IsWindows8Point1OrGreater returns false unless the application contains a manifest that includes a compatibility section
//		that contains the GUID that designates Windows 10.
			else if( IsWindows8Point1OrGreater() )	// easier API to determine Windows version
			{
				b_win81			= true;
				b_win8_or_more	= true;
				b_win7_or_more	= true;
				o_name	= "Windows 8.1";
			}
			else if( IsWindows8OrGreater() )
			{
				b_win8			= true;
				b_win8_or_more	= true;
				b_win7_or_more	= true;
				o_name	= "Windows 8";
			}
			else if( IsWindows7OrGreater() )
			{
				b_win7			= true;
				b_win7_or_more	= true;
				o_name			= "Windows 7";
			}
			else if( IsWindowsVistaOrGreater() )
			{
				b_vista			= true;
				o_name			= "Windows Vista";
			}
			else if( IsWindowsXPOrGreater() )
			{
				b_xp			= true;
				o_name			= "Windows XP";
			}
#else
			OSVERSIONINFOEX	osversion {};
			osversion.dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );
			SYSTEM_INFO		system_info {};

			PGNSI	p_gnsi;
			PGPI	p_gpi;
			BOOL	b_os_version_info_ex;
			DWORD	type;

			CHAR*	name	= nullptr;
			CHAR*	version	= nullptr;

			b_x64 = false;

			if( !( b_os_version_info_ex = GetVersionEx( (OSVERSIONINFO *) &osversion ) ) )
			{
				ERR_PRINT_STRING( "GetVersionEx not supported!!" );
				return;
			}

			// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
			p_gnsi = (PGNSI) GetProcAddress( GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo" );
			if( p_gnsi )
				p_gnsi( &system_info );
			else
				GetSystemInfo(&system_info);

			version_major = osversion.dwMajorVersion;
			version_minor = osversion.dwMinorVersion;

			if( VER_PLATFORM_WIN32_NT == osversion.dwPlatformId && osversion.dwMajorVersion > 4 )
			{
				o_vendor.set( "Microsoft" );

				// Test for the specific product.
				if( osversion.dwMajorVersion == 6 )
				{
					if( osversion.dwMinorVersion == 0 )
					{
						if( osversion.wProductType == VER_NT_WORKSTATION )
							name = "Windows Vista";
						else
							name = "Windows Server 2008";
					}

					if( osversion.dwMinorVersion == 1 || osversion.dwMinorVersion == 2 )
					{
						if( osversion.wProductType == VER_NT_WORKSTATION )
							if( osversion.dwMinorVersion == 1 )
								name = "Windows 7";
							else if( osversion.dwMinorVersion == 2 )
								name = "Windows 8";
						else
							name = "Windows Server 2008 R2";
					}

					p_gpi = (PGPI) GetProcAddress( GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo" );

					p_gpi( osversion.dwMajorVersion, osversion.dwMinorVersion, 0, 0, &type);

					switch( type )
					{
					case PRODUCT_ULTIMATE:						version	=	"Ultimate Edition";									break;
					case PRODUCT_HOME_PREMIUM:					version	=	"Home Premium Edition";								break;
					case PRODUCT_HOME_BASIC:					version	=	"Home Basic Edition";								break;
					case PRODUCT_ENTERPRISE:					version	=	"Enterprise Edition";								break;
					case PRODUCT_BUSINESS:						version	=	"Business Edition";									break;
					case PRODUCT_STARTER:						version	=	"Starter Edition";									break;
					case PRODUCT_CLUSTER_SERVER:				version	=	"Cluster Server Edition";							break;
					case PRODUCT_DATACENTER_SERVER:				version	=	"Datacenter Edition";								break;
					case PRODUCT_DATACENTER_SERVER_CORE:		version	=	"Datacenter Edition (core installation)";			break;
					case PRODUCT_ENTERPRISE_SERVER:				version	=	"Enterprise Edition";								break;
					case PRODUCT_ENTERPRISE_SERVER_CORE:		version	=	"Enterprise Edition (core installation)";			break;
					case PRODUCT_ENTERPRISE_SERVER_IA64:		version	=	"Enterprise Edition for Itanium-based Systems";		break;
					case PRODUCT_SMALLBUSINESS_SERVER:			version	=	"Small Business Server";							break;
					case PRODUCT_SMALLBUSINESS_SERVER_PREMIUM:	version	=	"Small Business Server Premium Edition";			break;
					case PRODUCT_STANDARD_SERVER:				version	=	"Standard Edition";									break;
					case PRODUCT_STANDARD_SERVER_CORE:			version	=	"Standard Edition (core installation)";				break;
					case PRODUCT_WEB_SERVER:					version	=	"Web Server Edition";								break;
					case PRODUCT_PROFESSIONAL:					version =	"Professional";										break;
					case PRODUCT_UNLICENSED:					version =	"Unlicensed";										break;
					}
				}

				if( osversion.dwMajorVersion == 5 && osversion.dwMinorVersion == 2 )
				{
	#ifndef	SM_SERVERR2
		#define SM_SERVERR2		89
	#endif
	#ifndef	VER_SUITE_WH_SERVER
		#define VER_SUITE_WH_SERVER			0x00008000
	#endif
					if( GetSystemMetrics(SM_SERVERR2) )
						name	= "Windows Server 2003 R2";
					else if( osversion.wSuiteMask==VER_SUITE_STORAGE_SERVER )
						name	= "Windows Storage Server 2003";
					else if( osversion.wSuiteMask == VER_SUITE_WH_SERVER )
						name	= "Windows Home Server";
					else if( osversion.wProductType == VER_NT_WORKSTATION && system_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
					{
						name	= "Windows XP Professional x64 Edition";
					}
					else
						name	= "Windows Server 2003";

					// Test for the server type.
					if( osversion.wProductType != VER_NT_WORKSTATION )
					{
						if( system_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
						{
							b_x64 = true;
							if( osversion.wSuiteMask & VER_SUITE_DATACENTER )
								version	=	"Datacenter x64 Edition";
							else if( osversion.wSuiteMask & VER_SUITE_ENTERPRISE )
								version	=	"Enterprise x64 Edition";
							else
								version	=	"Standard x64 Edition";
						}
						else
						{
							if( osversion.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
								version	=	"Compute Cluster Edition";
							else if( osversion.wSuiteMask & VER_SUITE_DATACENTER )
								version	=	"Datacenter Edition";
							else if( osversion.wSuiteMask & VER_SUITE_ENTERPRISE )
								version	=	"Enterprise Edition";
							else if( osversion.wSuiteMask & VER_SUITE_BLADE )
								version	=	"Web Edition";
							else
								version	=	"Standard Edition";
						}
					}
				}

				if( osversion.dwMajorVersion == 5 && osversion.dwMinorVersion == 1 )
				{
					name = "Windows XP";
					if( osversion.wSuiteMask & VER_SUITE_PERSONAL )
						version	=	"Home Edition";
					else
						version	=	"Professional";
				}

				if( osversion.dwMajorVersion == 5 && osversion.dwMinorVersion == 0 )
				{
					name = "Windows 2000 ";

					if( osversion.wProductType == VER_NT_WORKSTATION )
					{
						version	=	"Professional";
					}
					else 
					{
						if( osversion.wSuiteMask & VER_SUITE_DATACENTER )
							version	=	"Datacenter Server";
						else if( osversion.wSuiteMask & VER_SUITE_ENTERPRISE )
							version	=	"Advanced Server";
						else
							version	=	"Server";
					}
				}

				// Include service pack (if any) and build number.
				if( _tcslen( osversion.szCSDVersion ) > 0 )
				{
					sp_number.set( osversion.szCSDVersion );
				}

				build_number = osversion.dwBuildNumber;

				if( osversion.dwMajorVersion >= 6 )
				{
					if( system_info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
					{
						b_x64 = true;
					}
				}
			}
			else
			{
				ERR_PRINT_STRING( "Unknown Windows version" );
			}
			o_name.set( name );
			o_version.set( version );
		}
#endif
#endif
	}
#endif //AAA_OS_WINDOWS() -- get_info_window

#if AAA_OS_MAC()
	//	macOS native query path. Uses sysctlbyname for kernel/product
	//	version + hostname + arch. Apple Silicon only (arm64), macOS 13+.
	//	Sets the same b_* point-in-time / cumulative flags as get_info_window
	//	but only the macOS-relevant ones : everything Windows-specific stays
	//	false. The Mac-specific extra fields live behind AAA_OS_MAC() in
	//	os_version.h.
	void get_info_mac()
	{
		o_vendor.set( "Apple" );
		o_name.erase();

		//	kern.osproductversion : e.g. "14.3" or "14.3.1". On older betas
		//	this can be empty -- fall back to kern.osrelease (Darwin kernel
		//	version) if so.
		char	product_version[64] = { 0 };
		size_t	pv_size = sizeof(product_version);
		if( sysctlbyname( "kern.osproductversion", product_version, &pv_size, nullptr, 0 ) == 0 && pv_size > 0 )
		{
			product_version[ sizeof(product_version) - 1 ] = 0;
			version_string.set( product_version );

			//	Parse "MAJOR.MINOR[.PATCH]" into integers.
			int maj = 0, mn = 0, pt = 0;
			sscanf( product_version, "%d.%d.%d", &maj, &mn, &pt );
			version_major = maj;
			version_minor = mn;
			version_patch = pt;
		}
		else
		{
			version_major = 0;
			version_minor = 0;
			version_patch = 0;
		}

		//	kern.osrelease : Darwin kernel version, e.g. "23.3.0" on macOS 14.
		char	kernel_release[64] = { 0 };
		size_t	kr_size = sizeof(kernel_release);
		if( sysctlbyname( "kern.osrelease", kernel_release, &kr_size, nullptr, 0 ) == 0 && kr_size > 0 )
		{
			kernel_release[ sizeof(kernel_release) - 1 ] = 0;
			kernel_version.set( kernel_release );
		}

		//	hw.machine : "arm64" on Apple Silicon, "x86_64" on Intel.
		char	machine[64] = { 0 };
		size_t	m_size = sizeof(machine);
		if( sysctlbyname( "hw.machine", machine, &m_size, nullptr, 0 ) == 0 && m_size > 0 )
		{
			machine[ sizeof(machine) - 1 ] = 0;
			architecture.set( machine );
			b_arm64 = ( strcmp( machine, "arm64" ) == 0 );
		}
		else
		{
			b_arm64 = false;
		}

		//	CPU counts.
		{
			int		cpu_count = 0;
			size_t	c_size = sizeof(cpu_count);
			if( sysctlbyname( "hw.ncpu", &cpu_count, &c_size, nullptr, 0 ) == 0 )
				cpu_logical = cpu_count;

			int		phys_count = 0;
			size_t	p_size = sizeof(phys_count);
			if( sysctlbyname( "hw.physicalcpu", &phys_count, &p_size, nullptr, 0 ) == 0 )
				cpu_physical = phys_count;
		}

		//	Memory size.
		{
			INT64	mem_size = 0;
			size_t	ms_size = sizeof(mem_size);
			if( sysctlbyname( "hw.memsize", &mem_size, &ms_size, nullptr, 0 ) == 0 )
				memory_size_bytes = mem_size;
		}

		//	Hostname.
		{
			char	hostbuf[256] = { 0 };
			if( gethostname( hostbuf, sizeof(hostbuf) - 1 ) == 0 )
			{
				hostbuf[ sizeof(hostbuf) - 1 ] = 0;
				host_name.set( hostbuf );
			}
		}

		//	User name : prefer getlogin_r ; fall back to passwd lookup.
		{
			char userbuf[256] = { 0 };
			if( getlogin_r( userbuf, sizeof(userbuf) - 1 ) == 0 && userbuf[0] != 0 )
			{
				userbuf[ sizeof(userbuf) - 1 ] = 0;
				user_name.set( userbuf );
			}
			else
			{
				struct passwd* pw = getpwuid( getuid() );
				if( pw && pw->pw_name )
					user_name.set( pw->pw_name );
			}
		}

		//	Build the human-readable name. macOS 13 = Ventura, 14 = Sonoma,
		//	15 = Sequoia. Only label the ones we officially support
		//	(13+), otherwise just emit "macOS <version>".
		char	name_buf[128] = { 0 };
		char const * codename = "";
		switch( version_major )
		{
		case 13:	codename = " Ventura";	break;
		case 14:	codename = " Sonoma";	break;
		case 15:	codename = " Sequoia";	break;
		default:	codename = "";			break;
		}
		snprintf( name_buf, sizeof(name_buf), "macOS %d.%d%s", version_major, version_minor, codename );
		o_name.set( name_buf );

		//	arm64 macOS is always 64-bit.
		b_x64 = true;	//	cohabitation field : "this is a 64-bit OS"

#ifndef _USING_V110_SDK71_
		//	Make sure Windows-only point-in-time / cumulative flags are
		//	cleared on Mac. They are declared extern in os_version.h.
		b_xp     = false;
		b_vista  = false;
		b_win7   = false;
		b_win8   = false;
		b_win81  = false;
		b_win10  = false;
		b_win11  = false;
		b_win7_or_more  = false;
		b_win8_or_more  = false;
		b_win10_or_more = false;
#endif
	}
#endif //AAA_OS_MAC() -- get_info_mac

	void	get_info()
	{
#if AAA_OS_WINDOWS()
		n_os_version::get_info_window();
#elif AAA_OS_MAC()
		n_os_version::get_info_mac();
#endif
	}
}	//namespace	n_os_version

