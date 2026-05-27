#include "console.h"

#if AAA_OS_WINDOWS()
#	include <Windows.h>
#	include <wincon.h>
#elif AAA_OS_MAC()
#	include <unistd.h>
#	include <cstdio>
#	include <cstring>
#endif

//#using <mscorlib.dll>

namespace {
	//WORD	OriginalColors;
	FP32	color[3];
	bool	b_inverse;

#if AAA_OS_WINDOWS()
	HANDLE	get_console_handle()
	{
		static HANDLE	console_handle = 0;

		if( !console_handle )
		{
			CONSOLE_SCREEN_BUFFER_INFO*	ConsoleInfo = new CONSOLE_SCREEN_BUFFER_INFO();

			console_handle = GetStdHandle( STD_OUTPUT_HANDLE );
			GetConsoleScreenBufferInfo( console_handle, ConsoleInfo );
			//OriginalColors = ConsoleInfo->wAttributes;

			delete ConsoleInfo;

			//	Console::WriteLine(S"Original Colors");
			//	Console::WriteLine(S"Press Enter to Begin");
			//	Console::ReadLine();

			//	SetConsoleTextAttribute(hConsoleHandle, FOREGROUND_BLUE |FOREGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_INTENSITY);
			console::set_text_normal();
		}

		return console_handle;
	}


	FINLINE void set_text( REAL CONST r, REAL CONST g, REAL CONST b, bool CONST b_inv, CONST WORD att )
	{
		color[0] = r;
		color[1] = g;
		color[2] = b;
		b_inverse = b_inv;
		SetConsoleTextAttribute(	get_console_handle(),		att );
	}
#elif AAA_OS_MAC()
	//	On macOS we use ANSI escape sequences on stdout when it's a real
	//	terminal. When stdout is redirected to a file or pipe, we skip the
	//	escapes so log files stay clean.
	bool	is_tty()
	{
		static int cached = -1;
		if( cached < 0 )
			cached = isatty( STDOUT_FILENO ) ? 1 : 0;
		return cached != 0;
	}

	//	ANSI SGR : 30-37 foreground, 40-47 background, 90-97 / 100-107 bright.
	//	"inverse" on Mac maps to a bright-bg + dark-fg variant via SGR 7 (reverse).
	FINLINE void set_text( REAL CONST r, REAL CONST g, REAL CONST b, bool CONST b_inv, C_PCHAR_C ansi )
	{
		color[0] = r;
		color[1] = g;
		color[2] = b;
		b_inverse = b_inv;
		if( is_tty() && ansi && *ansi )
			fputs( ansi, stdout );
	}
#endif
}


void	console::write( C_PCHAR_C buf )
{
#if AAA_OS_WINDOWS()
//todox64 check overflow
	WriteConsoleA( get_console_handle(), buf, (DWORD)strlen( buf ), nullptr, nullptr );
#elif AAA_OS_MAC()
	if( !buf )
		return;
	fputs( buf, stdout );
#endif
}

void	console::get_color_info( FP32* CONST color_dst, bool& inv_dst )
								{
									color_dst[ 0 ] = color[ 0 ];
									color_dst[ 1 ] = color[ 1 ];
									color_dst[ 2 ] = color[ 2 ];
									inv_dst = b_inverse;
								}

#if AAA_OS_WINDOWS()
void	console::set_text_red()					{	set_text( 1, 0, 0,	false,	FOREGROUND_RED	 | FOREGROUND_INTENSITY	);	}
void	console::set_text_green()				{	set_text( 0, 1, 0,	false,	FOREGROUND_GREEN | FOREGROUND_INTENSITY	);	}
void	console::set_text_blue()				{	set_text( 0, 0, 1,	false,	FOREGROUND_BLUE  | FOREGROUND_INTENSITY	);	}
void	console::set_text_yellow()				{	set_text( 1, 1, 0,	false,	FOREGROUND_GREEN | FOREGROUND_RED	| FOREGROUND_INTENSITY	);	}
void	console::set_text_cyan()				{	set_text( 0, 1, 1,	false,	FOREGROUND_GREEN | FOREGROUND_BLUE	| FOREGROUND_INTENSITY	);	}
void	console::set_text_purple()				{	set_text( 1, 0, 1,	false,	FOREGROUND_RED   | FOREGROUND_BLUE	| FOREGROUND_INTENSITY	);	}
void	console::set_text_white()				{	set_text( 1, 1, 1,	false,	FOREGROUND_RED   | FOREGROUND_GREEN	| FOREGROUND_BLUE | FOREGROUND_INTENSITY	);	}
void	console::set_text_green_low()			{	set_text( 0, .5, 0,	false,	FOREGROUND_GREEN );	}

void	console::set_text_red_inverse()			{	set_text( 1, 0, 0,	true,	BACKGROUND_RED	 | BACKGROUND_INTENSITY	);	}
void	console::set_text_green_inverse()		{	set_text( 0, 1, 0,	true,	BACKGROUND_GREEN | BACKGROUND_INTENSITY	);	}
void	console::set_text_blue_inverse()		{	set_text( 0, 0, 1,	true,	BACKGROUND_BLUE  | BACKGROUND_INTENSITY	);	}
void	console::set_text_yellow_inverse()		{	set_text( 1, 1, 0,	true,	BACKGROUND_GREEN | BACKGROUND_RED	| BACKGROUND_INTENSITY	);	}
void	console::set_text_cyan_inverse()		{	set_text( 0, 1, 1,	true,	BACKGROUND_GREEN | BACKGROUND_BLUE	| BACKGROUND_INTENSITY	);	}
void	console::set_text_purple_inverse()		{	set_text( 1, 0, 1,	true,	BACKGROUND_RED   | BACKGROUND_BLUE	| BACKGROUND_INTENSITY	);	}
void	console::set_text_white_inverse()		{	set_text( 1, 1, 1,	true,	BACKGROUND_RED   | BACKGROUND_GREEN	| BACKGROUND_BLUE | BACKGROUND_INTENSITY	);	}
void	console::set_text_green_low_inverse()	{	set_text( 0, .5, 0, true,	BACKGROUND_GREEN );	}
#elif AAA_OS_MAC()
//	ANSI SGR reset is "\x1b[0m". Bright fg colors are 90..97. SGR 7 is reverse video.
void	console::set_text_red()					{	set_text( 1, 0, 0,	false,	"\x1b[0m\x1b[91m" );	}
void	console::set_text_green()				{	set_text( 0, 1, 0,	false,	"\x1b[0m\x1b[92m" );	}
void	console::set_text_blue()				{	set_text( 0, 0, 1,	false,	"\x1b[0m\x1b[94m" );	}
void	console::set_text_yellow()				{	set_text( 1, 1, 0,	false,	"\x1b[0m\x1b[93m" );	}
void	console::set_text_cyan()				{	set_text( 0, 1, 1,	false,	"\x1b[0m\x1b[96m" );	}
void	console::set_text_purple()				{	set_text( 1, 0, 1,	false,	"\x1b[0m\x1b[95m" );	}
void	console::set_text_white()				{	set_text( 1, 1, 1,	false,	"\x1b[0m\x1b[97m" );	}
void	console::set_text_green_low()			{	set_text( 0, .5, 0,	false,	"\x1b[0m\x1b[32m" );	}

void	console::set_text_red_inverse()			{	set_text( 1, 0, 0,	true,	"\x1b[0m\x1b[7m\x1b[91m" );	}
void	console::set_text_green_inverse()		{	set_text( 0, 1, 0,	true,	"\x1b[0m\x1b[7m\x1b[92m" );	}
void	console::set_text_blue_inverse()		{	set_text( 0, 0, 1,	true,	"\x1b[0m\x1b[7m\x1b[94m" );	}
void	console::set_text_yellow_inverse()		{	set_text( 1, 1, 0,	true,	"\x1b[0m\x1b[7m\x1b[93m" );	}
void	console::set_text_cyan_inverse()		{	set_text( 0, 1, 1,	true,	"\x1b[0m\x1b[7m\x1b[96m" );	}
void	console::set_text_purple_inverse()		{	set_text( 1, 0, 1,	true,	"\x1b[0m\x1b[7m\x1b[95m" );	}
void	console::set_text_white_inverse()		{	set_text( 1, 1, 1,	true,	"\x1b[0m\x1b[7m\x1b[97m" );	}
void	console::set_text_green_low_inverse()	{	set_text( 0, .5, 0, true,	"\x1b[0m\x1b[7m\x1b[32m" );	}
#endif

//void	console::set_text_regular()				{	SetConsoleTextAttribute(hConsoleHandle, OriginalColors);	}

void	console::set_text_normal()				{	set_text_green();			}
void	console::set_text_normal_inverse()		{	set_text_green_inverse();	}
