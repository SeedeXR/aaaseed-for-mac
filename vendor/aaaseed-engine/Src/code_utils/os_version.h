
#ifdef AAA_OS_VERSION_H
#error "OS_VERSION_H included more than once."
#endif
#define AAA_OS_VERSION_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

namespace n_os_version
{
	extern	o_str	o_vendor;
	extern	o_str	o_name;
	extern	bool	b_windows;
	extern	bool	b_macintosh;
	extern	bool	b_linux;
#ifndef _USING_V110_SDK71_
	//	Point-in-time flags: exactly one is true for a recognised Windows.
	extern	bool	b_xp;
	extern	bool	b_vista;
	extern	bool	b_win7;
	extern	bool	b_win8;
	extern	bool	b_win81;
	extern	bool	b_win10;
	extern	bool	b_win11;
	//	Cumulative flags: monotone -- multiple may be true at once.
	extern	bool	b_win7_or_more;
	extern	bool	b_win8_or_more;
	extern	bool	b_win10_or_more;
#else
	extern	o_str	o_version;
	extern	INT32	version_major;
	extern	INT32	version_minor;
	extern	INT32	build_number;
	extern	o_str	sp_number;
	extern	bool	b_x64;
#endif

#if AAA_OS_MAC()
	//	macOS-specific fields populated by get_info_mac(). These are
	//	declared unconditionally on the Mac build so callers don't need
	//	platform guards to read them. Mirrors fields normally provided
	//	by the _USING_V110_SDK71_ branch on Windows.
	extern	o_str	version_string;		//	"14.3" / "14.3.1"
	extern	INT32	version_major;		//	14
	extern	INT32	version_minor;		//	3
	extern	INT32	version_patch;		//	1 (0 if not present)
	extern	o_str	kernel_version;		//	Darwin kernel "23.3.0"
	extern	o_str	architecture;		//	"arm64"
	extern	bool	b_arm64;
	extern	bool	b_x64;				//	always true on arm64 macOS (64-bit OS)
	extern	INT32	cpu_logical;		//	hw.ncpu
	extern	INT32	cpu_physical;		//	hw.physicalcpu
	extern	INT64	memory_size_bytes;	//	hw.memsize
	extern	o_str	host_name;
	extern	o_str	user_name;
#endif

	extern	void get_info();
};

