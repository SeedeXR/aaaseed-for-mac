#include "err.h"
#include "aaa_def.h"
#include <stdio.h>
#include "aaa_util.h"
#include "infrastructure/layer/layer.h"
#include "obj_ui/com/net.h"
#include "ui/aaa_menu.h"
#include "ui/dialog_dev.h"
#include "math/v_base.h"
#include "file/aaa_dir.h"


#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif
#define AAA_USE_COMMCTRL()	0	//cause problem: Metal and Wood version get an Ordinal 345 at start
#if AAA_USE_COMMCTRL()
#	include <commctrl.h>
#endif

#if AAA_OS_MAC()
#	include <os/log.h>
#	include <unistd.h>
#	include <cstdio>
#	include <cstdarg>
#	include <csignal>
//	Stand-ins for Win32 message-box flags. They are only forwarded into the
//	Mac branch of message_box_low(), which ignores `flags` -- the values
//	themselves don't matter, but the names have to resolve for the shared
//	call sites in BOX_ERR_FOR_LUA / BOX_WAR / BOX_SIZE / etc.
#	ifndef MB_OK
#		define MB_OK               0x00000000u
#	endif
#	ifndef MB_OKCANCEL
#		define MB_OKCANCEL         0x00000001u
#	endif
#	ifndef MB_ICONSTOP
#		define MB_ICONSTOP         0x00000010u
#	endif
#	ifndef MB_ICONWARNING
#		define MB_ICONWARNING      0x00000030u
#	endif
#	ifndef MB_ICONINFORMATION
#		define MB_ICONINFORMATION  0x00000040u
#	endif
#	ifndef MB_TOPMOST
#		define MB_TOPMOST          0x00040000u
#	endif
#	ifndef MB_SETFOREGROUND
#		define MB_SETFOREGROUND    0x00010000u
#	endif
#	ifndef IDOK
#		define IDOK                1
#	endif
#	ifndef IDCANCEL
#		define IDCANCEL            2
#	endif
#endif


//	not useful but for the record
//	#include "vadefs.h"
C_PCHAR_C ERR_GET_STR( AAA_ERR err_code )
{
	C_PCHAR str;
	switch( err_code )
	{
	case AAA_OK:					str = "AAA_OK";					break;
	case ERR_ANY:					str = "ERR_ANY";				break;
	case ERR_UNIMPLEMENTED_YET:		str = "ERR_UNIMPLEMENTED_YET";	break;
	case ERR_NOT_COMPILED:			str = "ERR_NOT_COMPILED";		break;
	case ERR_OBJ_NULL:				str = "ERR_OBJ_NULL";			break;
	case ERR_OUT_OF_BOUND:			str = "ERR_OUT_OF_BOUND";		break;
	case ERR_HACK:					str = "ERR_HACK";				break;
	case ERR_LOCKED:				str = "ERR_LOCKED";				break;
	case ERR_DATA_INCOHERENT:		str = "ERR_DATA_INCOHERENT";	break;
	case ERR_DATA_SKIPPED:			str = "ERR_DATA_SKIPPED";		break;
	case ERR_STR_EMPTY:				str = "ERR_STR_EMPTY";			break;
	case ERR_TYPE_UNUSED:			str = "ERR_TYPE_UNUSED";		break;
	case ERR_CANCEL:				str = "ERR_CANCEL";				break;
	default:						str = "Unknown error code.";	break;
	}
	return str;
}

namespace
{
	//	print to string in a very safe way but if not enough space print to stdout
	INT32	MY_VNSPRINTF( CHAR* str, INT32 str_len, C_PCHAR_C fmt, va_list args )
	{
		va_list args_copy;
		va_copy( args_copy, args );
		INT32 len = vsnprintf( str, str_len, fmt ? fmt : "HUH !!!! got a null string", args_copy );
		if( len >= 0 )
			return len;
		vprintf( fmt, args );
		return vsnprintf( str, str_len, "string too long : printed to stdout only", nullptr );
	}
}

namespace	aaa
{
namespace	mess
{

CONSTEXPR INT32 NB	=	1024;
CONSTEXPR INT32 LEN	=	4096;	// 1024 even 2048 was not enough for OpenCL or lua

//done because standard lock don't function here, no idea why (maa 2013 July)
//hack move to atomic
#define HACK_MAA_USE_ATOMIC	1
#if HACK_MAA_USE_ATOMIC
	static std::atomic<INT32>& 	get_dlg_count()
	{
		static	std::atomic<INT32> lock_dlg = 0;
		return	lock_dlg;
	}
#else
	static	aaa::MUTEX& get_dlg_lock()
	{
		static	aaa::MUTEX lock_dlg;
		return	lock_dlg;
	}
#endif
static MUTEX_RECURSIVE lock_mess;

	
void init()
{
#if AAA_USE_COMMCTRL()
//	InitCommonControls();

	INITCOMMONCONTROLSEX icex = { sizeof(icex), ICC_STANDARD_CLASSES | ICC_WIN95_CLASSES };
	InitCommonControlsEx(&icex);
#endif
}
//	general purpose error string buffer
CHAR*	get_tmp()
{
	static	CHAR	tmp[LEN];
	return tmp;
}

CHAR	buf_net			[LEN];
//	we keep the last MESS_NB lines for the interface
//	all the err_mess should go thru these
INT32	index			=	0;
INT32	line_nb			=	0;
CHAR	buf				[NB][LEN];

struct att_char {
	FP32	color[3];
	bool	b_inv;
};

struct att_char att_line[NB];

FINLINE	INT32	process_index( UINT32 index_in )	{	return IMOD( index_in, NB );				}
FINLINE	CHAR*	get( INT32 index_in )				{	return buf[ process_index( index-index_in ) ];	}
FINLINE	CHAR*	get()								{	return buf[index];								}

void	get_color( INT32 index_in, FP32* color, bool& b_inverse )
{
	index_in = process_index( index-index_in );
	struct att_char& att = att_line[index_in];
	cpy_v3( color, att.color );
	b_inverse = att.b_inv;
}
FINLINE	void	next()
{
	struct att_char& att = att_line[index];
	console::get_color_info( att.color, att.b_inv );
	if( line_nb < NB )
		++line_nb;
	index = process_index( ++index );
}

bool	b_print_to_console = true;
namespace {
	void	output_buf( C_PCHAR_C buf )
	{
		if( b_print_to_console )
		{
#if AAA_OS_WINDOWS()
			console::write( buf );
#elif AAA_OS_MAC()
			//	console::write handles ANSI escapes + isatty gating
			console::write( buf );
#else
			printf( "%s", buf );
#endif
		}
#if AAA_OS_WINDOWS()
		if( IsDebuggerPresent() )
		{
#	if 1	//probably faster and avoid allocation for now
			OutputDebugStringA( buf );
#	else
			//todo refine to avoid allocation/deallocation
			wchar_t* txt = sysutils::utf8_to_unicode( buf );
			OutputDebugStringW( txt );
			sysutils::free_str_tmp( txt );
#	endif
		}
#elif AAA_OS_MAC()
		//	On Mac the system console / Console.app sink is os_log. We tee
		//	to stderr so test runners and detached terminals also see it.
		if( buf && *buf )
		{
			os_log_with_type( OS_LOG_DEFAULT, OS_LOG_TYPE_DEFAULT, "%{public}s", buf );
			fputs( buf, stderr );
		}
#endif
	}
}
void	output_tmp()
{
	output_buf( get_tmp() );
}
void	print()
{
	output_buf( get() );
	next();
}
void	print_with_cr()
{
	output_buf( get() );
	output_buf( "\n" );
	//PRINT_STRING( "%s\n", get() );
	next();
}
void	print_cr()
{
	output_buf( "\n" );
	next();
}
INT32	get_line_nb()
{
	return line_nb;
}

void	send( TYPE type )
{
	static	UINT8	tmp_net[ LEN + BLK_PRINT_HEADER_BYTE_NB ];
	if( net && net->is_active() )
	{
		tmp_net[ 0 ] = 0;
		tmp_net[ 1 ] = (UINT8)net->_host_id;
		tmp_net[ 2 ] = (UINT8)type;
		tmp_net[ 3 ] = 0;	//just to align on 4

		CHAR* tmp = get_tmp();
		INT32 len = (INT32) strlen( tmp ) + 1;
		strcpy( (CHAR*) &tmp_net[BLK_PRINT_HEADER_BYTE_NB], tmp );	//hack make it shorter (more prudent)
		net->sendto( c_net::print_dst, 0, c_net::BLK_PRINT, tmp_net, len + BLK_PRINT_HEADER_BYTE_NB );
	}
}

INT32	print_to_tmp( C_PCHAR_C fmt, va_list args )
{
//		va_list va;
//		va_copy( va, args );
	INT32 len = MY_VNSPRINTF( get_tmp(), LEN-1, fmt, args );
//		va_end( va );
	return len;
}

}	//namespace	mess
}	//namespace	aaa

#define PRINT_TO_TMP()\
{\
	va_list args;\
	va_start( args, fmt );\
		aaa::mess::print_to_tmp( fmt, args );\
	va_end(args);\
}

// not locked -- caller must hold lock_mess if called from multiple threads
// the risk is that the string printed is not the one expected but it should not cause a crash, so we prefer to avoid locking for now (Maa Sep 2017)
void	PRINT_STRING( C_PCHAR_C fmt, ... )
{
	PRINT_TO_TMP();
	aaa::mess::output_tmp();
}

void	PRINT_CR()
{
	aaa::mess::print_cr();
}

void	PRINT_STRING_BY_BLOCK( C_PCHAR str, INT32 len )
{
	if( !str || !*str )
	{
		PRINT_STRING( "!!! Try to print empty string." );
		return;
	}

	INT32	len_out = -1;
	if( len > 80 )		{	len = 80;	}

	CHAR	line[256];
	CHAR*	dst = line-1;
	--str;
	CHAR c;
	do 
	{
		c = *++str;
		*++dst = c;
		if( c==' ' || c==0 )
		{
			//if( dst - line >= len )
			//{
			len_out = INT32(dst - line);
				*dst = 0;
				dst = line-1;
			//}
			//else
			//{
			//	*dst = '\t';
			//	*++dst = '\t';
			//}
		}
		if( len_out >= 0 )
		{
			if( len_out > 0 )
			{
				CHAR* tmp = aaa::mess::get_tmp();
				strncpy( tmp,				"#\t\t\t", 4 );
				strncpy( tmp + 4,			line, len_out );
				strcpy(  tmp + 4 + len_out,	"\n" );
				len_out = 0;
				//va_end(args);

				aaa::mess::output_tmp();
			}
		}
	}
	while( c!=0 );
}


//	make noise
void	BELL()
{
// we avoid printf on windows because of a LuaJit strange problem at linking (Maa Sep 2017)
//	printf( "\a\a\a" );
	putchar('\a');
	putchar('\a');
	putchar('\a');
}

void	PRINT_THIS_NULL( C_PCHAR_C signature )
{
	DBG_PRINT_STRING( "%s() called with a null pointer, returning", signature );
}

// not locked -- caller must hold lock_mess if called from multiple threads
// the risk is that the string printed is not the one expected but it should not cause a crash, so we prefer to avoid locking for now (Maa Sep 2017)
void	OBJ_PRINT_STRING_VA( C_PCHAR_C header, C_PCHAR_C fmt, va_list args )
{
//	va_list va;
//	va_copy( va, args );
	CHAR*	str = aaa::mess::get();
	INT32	len;

	*str = '#';
	*(str+1) = ' ';
	{
		INT32 CONST header_max = aaa::mess::LEN-3;
		INT32 written = snprintf( str+2, header_max, header );
		if( written < 0 )
		{
			written = header_max;
			*(str+2+written) = 0;
		}
		len = 2 + written;
	}
	str += len;
	*str = ' ';
	++str;

	str += MY_VNSPRINTF( str, aaa::mess::LEN-1-len-1, fmt, args );
//	va_end(va);
	aaa::mess::print_with_cr();
}

void	HEADER_PRINT_STRING( C_PCHAR_C header, C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		CHAR*	str = aaa::mess::get();
		INT32	len = 0;
		if( header && *header )
		{
			len = snprintf( str, aaa::mess::LEN-1, header );	//try _snprintf_s
			if( len < 0 )
				len = 0;
			else
				str += len;
		}

		str += MY_VNSPRINTF( str, aaa::mess::LEN-1-len, fmt, args );	//try vsnprintf_s,
	va_end(args);

	aaa::mess::print_with_cr();
}

void	HEADER_PRINT_STRING_NO_CR( C_PCHAR_C h, C_PCHAR_C s )
{
	snprintf( aaa::mess::get(), aaa::mess::LEN -1, "%s%s", h, s );

	aaa::mess::print();
}

void	HEADER_PRINT_STRING_VA( C_PCHAR_C h, C_PCHAR_C fmt, va_list args  )
{
//	va_list va;
//	va_copy( va, args );

	CHAR*	str = aaa::mess::get();
	INT32	len = snprintf( str, aaa::mess::LEN-1, "%s", h );	//try _snprintf_s
	str += len;

	str += MY_VNSPRINTF( str, aaa::mess::LEN-1-len-1, fmt, args );	//try vsnprintf_s,
//	va_end(va);
	aaa::mess::print_with_cr();
}

void	PRINT_STRING_3( C_PCHAR_C h1, C_PCHAR_C h2, C_PCHAR_C mess )
{
	C_PCHAR_C mess_src = mess ? mess : "HUH !!!! got a null string to print";

	CHAR*	str = aaa::mess::get();
	INT32	len = aaa::mess::LEN-1;
	if( h1 )
	{
		if( h2 )
			snprintf( str, len, "%s%s%s", h1, h2, mess_src );	//try _snprintf_s
		else
			snprintf( str, len, "%s%s", h1, mess_src );	//try _snprintf_s
	}
	else
	{
		if( h2 )
			snprintf( str, len, "%s%s", h2, mess_src );	//try _snprintf_s
		else
			snprintf( str, len, "%s", mess_src );	//try _snprintf_s
	}
	aaa::mess::print_with_cr();
}

FINLINE	void	SET_COLOR_NORMAL()			{	console::set_text_normal();			}
FINLINE	void	SET_COLOR_NORMAL_INVERSE()	{	console::set_text_normal_inverse();	}

//ERR
//	it means : there is a problem that the user should know

FINLINE	void	SET_COLOR_ERR()				{	console::set_text_red();			}
FINLINE	void	SET_COLOR_ERR_INVERSE()		{	console::set_text_red_inverse();	}

void	HEADER_PRINT_STRING_VA_GENE( aaa::mess::TYPE type, console::COLOR_FN* fn, C_PCHAR_C header, C_PCHAR_C fmt, va_list args )
{
	aaa::mess::lock_mess.lock();
		aaa::mess::print_to_tmp( fmt, args );

		if( type>0 && c_net::b_print_send )
			aaa::mess::send( type );
		if( fn )
			fn();
		HEADER_PRINT_STRING( header, "%s", aaa::mess::get_tmp() );
		if( fn )
			SET_COLOR_NORMAL();
	aaa::mess::lock_mess.unlock();
}

static	void	ERR_HEADER_PRINT_STRING_VA( bool b_send, bool b_inverse, C_PCHAR_C header, C_PCHAR_C fmt, va_list args )
{
	aaa::mess::lock_mess.lock();
		aaa::mess::print_to_tmp( fmt, args );

		if( b_send && c_net::b_print_send )
			aaa::mess::send( aaa::mess::PRINT_ERR );
		if( b_inverse )	
			SET_COLOR_ERR_INVERSE();
		else
			SET_COLOR_ERR();
		PRINT_STRING_3( ERR_HEADER, header, aaa::mess::get_tmp() );
		SET_COLOR_NORMAL();
	aaa::mess::lock_mess.unlock();
}

void	ERR_PRINT_STRING_VA( C_PCHAR_C fmt, va_list args )
{
	ERR_HEADER_PRINT_STRING_VA( true, false, nullptr, fmt, args );
}

void	ERR_HEADER_PRINT_STRING_VA( C_PCHAR_C header, C_PCHAR_C fmt, va_list args )
{
	ERR_HEADER_PRINT_STRING_VA( true, false, header, fmt, args );
}

void	ERR_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		ERR_HEADER_PRINT_STRING_VA( true, false, nullptr, fmt, args );
	va_end(args);
}

void	ERR_PRINT_STRING_INV( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		ERR_HEADER_PRINT_STRING_VA( true, true, nullptr, fmt, args );
	va_end(args);
}

void	ERR_HEADER_PRINT_STRING( C_PCHAR_C header, C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		ERR_HEADER_PRINT_STRING_VA( true, false, header, fmt, args );
	va_end(args);
}


namespace{
	C_PCHAR str_print_where = "Starting";
};

void	set_print_where( C_PCHAR_C str )
{
	str_print_where = str;
}

C_PCHAR_C get_print_where()
{
	c_layer* layer_cur = c_layer::get_cur();
	if( layer_cur )
		return layer_cur->get_my_filename();	//was get_name_search_str();

	if( str_print_where )
		return str_print_where;

	debug_break( "%s() no layer cur and no str_print_where : this should never happen.", __FUNCTION__ );
	return "Strange place";
}

void	err_print( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
	aaa::mess::lock_mess.lock();
		ERR_PRINT_STRING( "# in %s : ", get_print_where() );
		ERR_HEADER_PRINT_STRING_VA( true, false, nullptr, fmt, args );
	aaa::mess::lock_mess.unlock();
	va_end(args);
}

void	WIN_ERR_PRINT( C_PCHAR_C msg )
{
#if AAA_OS_WINDOWS()
	//Display a message and the last error
	ERR_PRINT_STRING( "%s, Windows error : %s", msg, aaa::system::get_err_message().c_str() );
#elif AAA_OS_MAC()
	//	Mac has no GetLastError equivalent for the generic Windows API surface.
	//	Most call sites are post-Win32 errors that don't fire on Mac. Forward
	//	the message verbatim so log greps still work.
	ERR_PRINT_STRING( "%s (no equivalent system-error code on Mac)", msg );
#endif
}

//WHITE
void	WHITE_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_WHITE, console::set_text_white, nullptr, fmt, args );
	va_end(args);
}

void	WHITE_PRINT_STRING_INV( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_WHITE_INV, console::set_text_white_inverse, nullptr, fmt, args );
	va_end(args);
}

//TIMING
//	Print process timing information for Modules/Module/Group/Layer
CONSTEXPR	CHAR TIMING_HEADER[] = "### TIMING->";
FINLINE	void	SET_COLOR_TIMING()
{
	console::set_text_cyan();
}

void	TIMING_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_NO_SEND, SET_COLOR_TIMING, TIMING_HEADER, fmt, args );
	va_end(args);
}


//NO MEDIA
//	there is a media missing that the user should know but 
CONSTEXPR	CHAR NO_MEDIA_HEADER[] = "# NO MEDIA-> ";

FINLINE	void	SET_COLOR_NO_MEDIA()		{	console::set_text_red();			}
FINLINE	void	SET_COLOR_NO_MEDIA_INV()	{	console::set_text_red_inverse();	}

void	NO_MEDIA_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_NO_MEDIA, SET_COLOR_NO_MEDIA, NO_MEDIA_HEADER, fmt, args );
	va_end(args);
}

//VERBOSE
//	this message are activated thru verbose_something
CONSTEXPR	CHAR VERBOSE_HEADER[] = "### VERBOSE-> ";

static	void	VERBOSE_PRINT_STRING_VA( C_PCHAR_C fmt, va_list args )
{
//	va_list va;
//	va_copy( va, args );
	aaa::mess::print_to_tmp( fmt, args );
//	va_end(va);

	if( c_net::b_print_send )
		aaa::mess::send( aaa::mess::PRINT_DBG );
	HEADER_PRINT_STRING( VERBOSE_HEADER, "%s", aaa::mess::get_tmp() );
}

void	VERBOSE_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	PRINT_TO_TMP();
	HEADER_PRINT_STRING( VERBOSE_HEADER, "%s", aaa::mess::get_tmp() );
}

void	VERBOSE_PRINT_STRING_INV( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_NO_SEND, SET_COLOR_NORMAL_INVERSE, VERBOSE_HEADER, fmt, args );
	va_end(args);
}

void	verbose_print_string( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
		VERBOSE_PRINT_STRING( "# in %s : ", get_print_where() );
		VERBOSE_PRINT_STRING_VA( fmt, args );
	va_end(args);
}

//DEBUG
//	more info on the debug version
CONSTEXPR	CHAR DBG_HEADER[] = "### DBG-> ";
FINLINE	void	SET_COLOR_DBG()
{
	console::set_text_cyan();
}

void	DBG_PRINT_STRING_VA( C_PCHAR_C fmt, va_list args )
{
	HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_DBG, SET_COLOR_DBG, DBG_HEADER, fmt, args );
}

void	DBG_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
		DBG_PRINT_STRING_VA( fmt, args );
	va_end(args);
}

void	dbg_print( C_PCHAR_C fmt, ... )
{
	if( fmt )
	{
		va_list	args;
		va_start( args, fmt );
			DBG_PRINT_STRING( "# in %s : ", get_print_where() );
			HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_DBG, SET_COLOR_DBG, DBG_HEADER, fmt, args );
		va_end(args);
	}
	else
		DBG_PRINT_STRING( "# in %s : ", get_print_where() );
}

//GOOD
#define	GOOD_NEWS !0

#if	GOOD_NEWS
CONSTEXPR	CHAR GOOD_HEADER[] = "# Good NewS ---> ";
CONSTEXPR	CHAR BAD_HEADER[]  = "# Bad NewS ---> ";

//	confirmation of thing going well
void	GOOD_PRINT_STRING_NO_CR( C_PCHAR_C s )
{
	HEADER_PRINT_STRING_NO_CR( GOOD_HEADER, s );
}
 

void	GOOD_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_GOOD, nullptr, GOOD_HEADER, fmt, args );
	va_end(args);
}
void	GOOD_PRINT_STRING_INV( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_GOOD_INV, SET_COLOR_NORMAL_INVERSE, GOOD_HEADER, fmt, args );
	va_end(args);
}
void	good_print( C_PCHAR_C fmt, ... )
{
	if( fmt )
	{
		va_list	args;
		va_start( args, fmt );
		GOOD_PRINT_STRING( "# in %s : ", get_print_where() );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_GOOD, nullptr, GOOD_HEADER, fmt, args );
		va_end(args);
	}
	else
		GOOD_PRINT_STRING( "# in %s : ", get_print_where() );
}


FINLINE	void	SET_COLOR_WARNING()			{	console::set_text_cyan();			}
FINLINE	void	SET_COLOR_WARNING_INV()		{	console::set_text_cyan_inverse();	}

//	something happen but don't have an error status
void WARNING_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_NO_SEND, SET_COLOR_WARNING, BAD_HEADER, fmt, args );
	va_end(args);
}
void WARNING_PRINT_STRING_INV( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_NO_SEND, SET_COLOR_WARNING_INV, BAD_HEADER, fmt, args );
	va_end(args);
}

void WARNING_HEADER_PRINT_STRING_VA( C_PCHAR_C header,	C_PCHAR_C fmt, va_list args )
{
	aaa::mess::print_to_tmp( fmt, args );
	SET_COLOR_WARNING();
		PRINT_STRING_3( BAD_HEADER, header, aaa::mess::get_tmp() );
	SET_COLOR_NORMAL();
}


void WARNING_HEADER_PRINT_STRING( C_PCHAR_C header,	C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		WARNING_HEADER_PRINT_STRING_VA( header, fmt, args );
	va_end(args);
}

#else	//	GOOD_NEWS
void	GOOD_PRINT_STRING_NO_CR( C_PCHAR_C s )	{}
void GOOD_PRINT_STRING( CONST char* fmt, ...)	{}
//	something happen but don't have an error status 
void WARNING_PRINT_STRING( CONST char* fmt, ...)	{}
#endif	//	GOOD_NEWS


//UI
//	more info on the debug version
CONSTEXPR	CHAR UI_HEADER[] = "### User Interface -> ";
FINLINE	void	SET_COLOR_UI()
{
	console::set_text_cyan();
}

void	UI_PRINT_STRING_VA( C_PCHAR_C fmt, va_list args )
{
	HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_UI, SET_COLOR_UI, UI_HEADER, fmt, args );
}

void	UI_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
	UI_PRINT_STRING_VA( fmt, args );
	va_end(args);
}

void	ui_print( C_PCHAR_C fmt, ... )
{
	if( fmt )
	{
		va_list	args;
		va_start( args, fmt );
		UI_PRINT_STRING( "# in %s : ", get_print_where() );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_UI, SET_COLOR_UI, UI_HEADER, fmt, args );
		va_end(args);
	}
	else
		UI_PRINT_STRING( "# in %s : ", get_print_where() );
}

//SWITCH
CONSTEXPR	CHAR SWITCH_HEADER[] = "# Switch ";
void	SWITCH_PRINT_STRING( C_PCHAR_C s )
{
	HEADER_PRINT_STRING( SWITCH_HEADER, s );
}

void	SWITCH_PRINT_STRING( C_PCHAR_C t, C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );	

	CHAR*	str;
	INT32	len;

	str = aaa::mess::get();
	len = snprintf( str, aaa::mess::LEN-1, "%s%s to ", SWITCH_HEADER, t );
	str += len;

	str += MY_VNSPRINTF( str, aaa::mess::LEN-1-len-1, fmt, args );
	va_end(args);

	aaa::mess::print_with_cr();
}

void	SWITCH_PRINT_STATE( C_PCHAR_C t, bool s )
{
	SWITCH_PRINT_STRING( t, s ? "On." : "Off." );
}

CONSTEXPR	CHAR LUA_HEADER[] = "# LUA : ";

void	SET_COLOR_LUA()
{
	console::set_text_yellow();
}
void	SET_COLOR_LUA_INV()
{
	console::set_text_yellow_inverse();
}

void	LUA_GOOD_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_LUA_GOOD, SET_COLOR_LUA, LUA_HEADER, fmt, args );
	va_end(args);
}

void	LUA_GOOD_PRINT_STRING_INV( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_LUA_GOOD_INV, SET_COLOR_LUA_INV, LUA_HEADER, fmt, args );
	va_end(args);
}

void	LUA_DBG_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_LUA_DBG, SET_COLOR_DBG, LUA_HEADER, fmt, args );
	va_end(args);
}


void	LUA_ERR_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_LUA_ERR, SET_COLOR_ERR, LUA_HEADER, fmt, args );
	va_end(args);
}

static	void	FROM_NET_PRINT_STRING( console::COLOR_FN* fn, C_PCHAR_C header, INT32 host_id, C_PCHAR_C msg )
{
	fn();
		HEADER_PRINT_STRING( header, "%d -> %s", host_id, msg );
	SET_COLOR_NORMAL();
}

namespace aaa::mess {
	void	print_net_received(	INT32 type,	INT32 host_id, C_PCHAR_C msg )
	{
		switch( type )
		{
	//	case PRINT_CURRENT:			NET_PRINT_STRING(			host_id, msg );	break;
		case PRINT_GOOD:			FROM_NET_PRINT_STRING(	SET_COLOR_NORMAL,					GOOD_HEADER,			host_id, msg );	break;
		case PRINT_GOOD_INV:		FROM_NET_PRINT_STRING(	SET_COLOR_NORMAL_INVERSE,			GOOD_HEADER,			host_id, msg );	break;
		case PRINT_ERR:				FROM_NET_PRINT_STRING(	SET_COLOR_ERR,						ERR_HEADER,				host_id, msg );	break;
		case PRINT_DBG:				FROM_NET_PRINT_STRING(	SET_COLOR_DBG,						DBG_HEADER,				host_id, msg );	break;
		case PRINT_UI:				FROM_NET_PRINT_STRING(	SET_COLOR_UI,						UI_HEADER,				host_id, msg );	break;
		case PRINT_WHITE:			FROM_NET_PRINT_STRING(	console::set_text_white,			nullptr,				host_id, msg );	break;
		case PRINT_WHITE_INV:		FROM_NET_PRINT_STRING(	console::set_text_white_inverse,	nullptr,				host_id, msg );	break;
		case PRINT_NO_MEDIA:		FROM_NET_PRINT_STRING(	SET_COLOR_NO_MEDIA,					NO_MEDIA_HEADER,		host_id, msg );	break;
		case PRINT_LUA_GOOD:		FROM_NET_PRINT_STRING(	SET_COLOR_LUA,						LUA_HEADER,				host_id, msg );	break;
		case PRINT_LUA_GOOD_INV:	FROM_NET_PRINT_STRING(	SET_COLOR_LUA_INV,					LUA_HEADER,				host_id, msg );	break;
		case PRINT_LUA_ERR:			FROM_NET_PRINT_STRING(	SET_COLOR_ERR,						LUA_HEADER,				host_id, msg );	break;
		case PRINT_LUA_DBG:			FROM_NET_PRINT_STRING(	SET_COLOR_DBG,						LUA_HEADER,				host_id, msg );	break;
		default:					FROM_NET_PRINT_STRING(	SET_COLOR_ERR,						"# Net Unknown : ",	host_id, msg );	break;
		}
	}
}

/*	pass fmt when nil ?
void	BOX_MESS( CONST char * title, UINT flags, CONST char *fmt, ...)
{
	va_list args;
	va_start( args, fmt );

	CHAR*	str = err_mess::get();

	MY_VNSPRINTF( str, err_mess::LEN-1, fmt, args);

	va_end(args);

#ifdef	WIN32
	MessageBox(nullptr, str, title, flags);
#else
	DBG_PRINT_STRING(str);
#endif
	err_mess::next();
}
*/

namespace {
	bool	b_message_box = false;
//	titles for BOX_message
	CONSTEXPR C_PCHAR_C	title_box_error			= "ErrOr";
	CONSTEXPR C_PCHAR_C	title_box_warning		= "Warning";
	CONSTEXPR C_PCHAR_C	title_box_good			= "Good News";

	CONSTEXPR C_PCHAR_C	title_box_error_lua		= "lua Error";
	CONSTEXPR C_PCHAR_C	title_box_warning_lua	= "lua Warning";
	CONSTEXPR C_PCHAR_C	title_box_good_lua		= "lua Good News";
}

bool is_message_box()
{
	return b_message_box;
}


INT32	message_box_low( INT32 CONST size_x, void* hd_win, C_PCHAR_C mess, C_PCHAR_C title, UINT32 flags )
{
#if AAA_OS_WINDOWS()
	if( size_x > 0 )
	{
#	if !AAA_USE_COMMCTRL()
		return n_dialog::do_show( title, mess, "Love it !" );
#	else
		TASKDIALOGCONFIG config = {0};
		config.cbSize = sizeof(config);
		config.hwndParent = (HWND)hd_win;
		config.hInstance = NULL;
		config.dwCommonButtons = TDCBF_OK_BUTTON;
		wchar_t * wc_content = sysutils::utf8_to_unicode( mess );
		wchar_t * wc_title   = sysutils::utf8_to_unicode( title );
			config.pszWindowTitle = wc_title;
			config.pszContent = wc_content;
			config.cxWidth = size_x;

			HRESULT hr = TaskDialogIndirect( &config, NULL, NULL, NULL );
		sysutils::free_str_tmp( wc_title );
		sysutils::free_str_tmp( wc_content );
		return SUCCEEDED(hr) ? IDOK : IDCANCEL;
#	endif
	}
	else
		return MessageBoxA( (HWND)hd_win, mess, title, flags );
#elif AAA_OS_MAC()
	//	No modal popup yet on Mac. Tee FATAL to stderr + os_log so tests and
	//	dev terminals see it. The first port doesn't need a real NSAlert.
	(void)size_x;
	(void)hd_win;
	(void)flags;
	C_PCHAR_C t = title ? title : "";
	C_PCHAR_C m = mess  ? mess  : "";
	fprintf( stderr, "FATAL: [%s] %s\n", t, m );
	os_log_with_type( OS_LOG_DEFAULT, OS_LOG_TYPE_FAULT, "FATAL: [%{public}s] %{public}s", t, m );
	return 0;
#endif
}

INT32	MESSAGE_BOX( INT32 CONST size_x, void* hd_win, C_PCHAR_C mess, C_PCHAR_C title, UINT32 flags, INT32 ret_no_dialog )
{
	b_message_box = true;
	INT32	retcode = ret_no_dialog;	//	default value should be explicit, this can be dangerous 

	C_PCHAR mess_used = mess ? mess : "Null message";
	if( *mess_used==0 )
		mess_used = "Empty message";

	C_PCHAR title_used = title ? title : "Null title";
	if( *title_used==0 )
		title_used = "Empty title";

#if AAA_OS_WINDOWS()
#	if AAA_NEW_DESIGN()
	flags |= MB_SETFOREGROUND;
#	endif
#	if HACK_MAA_USE_ATOMIC
	auto& count = aaa::mess::get_dlg_count();
	if( count.load()==0 )
	{
		menu::detach_all();
		++count;
		//retcode = n_dialog::do_dev( title_used, mess_used );
		retcode = message_box_low( size_x, hd_win, mess_used, title_used, flags );
		--count;
	}
#	else
	aaa::MUTEX& l = aaa::mess::get_dlg_lock();
	if( l.try_lock() )
	{
		//retcode = do_dialog_dev( title_used, mess_used );
		retcode = message_box_low( size_x, hd_win, mess_used, title_used, flags );
		l.unlock();
	}
#	endif
	else
	{
		ERR_PRINT_STRING( "dialog already used will return %d", retcode );
	}
#else
	debug_break( "Dialog not implemented yet" );
#endif	// #if AAA_OS_WINDOWS()
	b_message_box = false;
	return retcode;
}

//void	BOX_ERR_PURE_FOR_C(	 C_PCHAR_C str )	{	MESSAGE_BOX(	   nullptr, str, title_box_error,		MB_OK | MB_ICONSTOP | MB_TOPMOST );			}
void	BOX_WAR_PURE_FOR_C(	 C_PCHAR_C str )	{	MESSAGE_BOX( 0,	   nullptr, str, title_box_warning,		MB_OK | MB_ICONWARNING );					}

void	BOX_ERR_FOR_LUA(	C_PCHAR_C str )		{	MESSAGE_BOX( 0,	   nullptr, str, title_box_error_lua,	MB_OK | MB_ICONSTOP | MB_TOPMOST );			}
void	BOX_WAR_FOR_LUA(	C_PCHAR_C str )		{	MESSAGE_BOX( 0,	   nullptr, str, title_box_warning_lua,	MB_OK | MB_ICONWARNING );					}
void	BOX_GOOD_FOR_LUA(	C_PCHAR_C str )		{	MESSAGE_BOX( 0,	   nullptr, str, title_box_good_lua,	MB_OK | MB_ICONINFORMATION | MB_TOPMOST );	}

void	BOX_TITLE_ERR_VA( C_PCHAR_C title, C_PCHAR_C fmt, va_list args )
{
//	va_list va;
//	va_copy( va, args );
	CHAR*	str = aaa::mess::get();
	MY_VNSPRINTF( str, aaa::mess::LEN-1, fmt ? fmt : "Huh !!! this box called with no message", args );

//	va_end(va);
	ERR_PRINT_STRING( str );
	MESSAGE_BOX( 0, nullptr, str, title, MB_OK | MB_ICONSTOP | MB_TOPMOST );

	aaa::mess::next();
}
void	BOX_TITLE_ERR( C_PCHAR_C title,	C_PCHAR_C fmt,	... )
{
	va_list args;
	va_start( args, fmt );
		BOX_TITLE_ERR_VA( title, fmt, args );
	va_end(args);
}
void	BOX_ERR( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		BOX_TITLE_ERR_VA( title_box_error, fmt, args );
	va_end(args);
}

void	BOX_WAR( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		CHAR*	str = aaa::mess::get();
		MY_VNSPRINTF( str, aaa::mess::LEN-1, fmt, args );
	va_end(args);

	DBG_PRINT_STRING( str );
	BOX_WAR_PURE_FOR_C( str );

	aaa::mess::next();
}

void	BOX_SIZE( INT32 CONST size_x, C_PCHAR_C title, C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		CHAR* str = aaa::mess::get();
		MY_VNSPRINTF( str, aaa::mess::LEN-1, fmt, args );
	va_end(args);

	aaa::mess::print_with_cr();
//	GOOD_PRINT_STRING( str );
	MESSAGE_BOX( size_x, nullptr, str, title, MB_OK | MB_ICONINFORMATION | MB_TOPMOST );

	aaa::mess::next();
}

bool	BOX_ASK_WAR( C_PCHAR_C title, C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		CHAR*	str = aaa::mess::get();
		MY_VNSPRINTF( str, aaa::mess::LEN-1, fmt, args );
	va_end(args);

	//todo the last parameter is OK for lua but this should be refined
	return MESSAGE_BOX( 0, nullptr, str, title, MB_OKCANCEL | MB_ICONWARNING | MB_TOPMOST, IDOK ) == IDOK;
}

INT32	BOX_DEV( C_PCHAR_C title, C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		CHAR*	str = aaa::mess::get();
		MY_VNSPRINTF( str, aaa::mess::LEN-1, fmt, args );
	va_end(args);

	aaa::mess::print_with_cr();
//	GOOD_PRINT_STRING( str );
	INT32 ret = n_dialog::do_dev( title, str );

	aaa::mess::next();

	return ret;
}

bool	BOX_LUA( C_PCHAR_C title, C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		CHAR*	str = aaa::mess::get();
		MY_VNSPRINTF( str, aaa::mess::LEN-1, fmt, args );
	va_end(args);

	aaa::mess::print_with_cr();
//	GOOD_PRINT_STRING( str );
	INT64 ret = n_dialog::do_lua( title, str );

	aaa::mess::next();

	return ret == IDOK;
}

INT32	BOX_FINGER( C_PCHAR_C title, C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
		CHAR*	str = aaa::mess::get();
		MY_VNSPRINTF( str, aaa::mess::LEN-1, fmt, args );
	va_end(args);

	aaa::mess::print_with_cr();
//	GOOD_PRINT_STRING( str );
	INT32 ret = n_dialog::do_finger( title, str );

	aaa::mess::next();

	return ret;
}


void	debug_break_if( bool b_break, CHAR CONST * CONST fmt, ... )
{
	if( fmt )
	{
		PRINT_TO_TMP();
		SET_COLOR_ERR();
			HEADER_PRINT_STRING( ERR_HEADER, aaa::mess::get_tmp() );
		SET_COLOR_NORMAL();
	}
	else
		ERR_PRINT_STRING( "!!! debug_break_if() Try to print empty string." );
#if AAA_DEBUG()
#	if AAA_OS_WINDOWS()
	if( b_break && IsDebuggerPresent() )
		DebugBreak();
#	elif AAA_OS_MAC()
	//	No reliable lldb-attach detection without a sysctl probe ; the
	//	first port skips the trap so unattended test runs aren't killed.
	//	Re-enable with __builtin_debugtrap() once we want a hard-stop here.
	(void)b_break;
#	endif
#endif
}

void	debug_break( CHAR CONST * CONST fmt, ... )
{
	if( fmt )
	{
		PRINT_TO_TMP();
		SET_COLOR_ERR();
			HEADER_PRINT_STRING( ERR_HEADER, aaa::mess::get_tmp() );
		SET_COLOR_NORMAL();
	}
	else
		ERR_PRINT_STRING( "!!! debug_break() Try to print empty string." );
#if AAA_DEBUG()
#	if AAA_OS_WINDOWS()
	if( IsDebuggerPresent() )
		DebugBreak();
#	elif AAA_OS_MAC()
	//	Same rationale as debug_break_if : skip the trap on first port.
	//	__builtin_debugtrap() is available via AppleClang but would crash
	//	unattended test runs ; the surrounding ERR_PRINT_STRING is the
	//	user-visible signal.
#	endif
#endif
}

bool	b_alert_with_sound_allowed = false;	//	initialized by pref infact

void	do_alert_with_sound( C_PCHAR_C name )
{
	if( !b_alert_with_sound_allowed )
		return;

	o_str filename;
	filename.set_fname_absolute( "SoundAlert", c_dir::get_start().get() );
	filename.set_fname_absolute( name, filename.get() );
	filename.add_ext( "wav" );
#if AAA_OS_WINDOWS()
#	if 1
	::PlaySoundA( filename.get(), nullptr, SND_SYNC );
#	else
	wchar_t * txt = sysutils::utf8_to_unicode( filename.get() );
	::PlaySoundW( txt, nullptr, SND_SYNC );
	sysutils::free_str_tmp( txt );
#	endif
#elif AAA_OS_MAC()
	//	No alert-sound playback on Mac in the first port. AVFoundation /
	//	AudioServicesPlaySystemSound is the right path here ; deferred until
	//	the audio subsystem is brought up. The bell character is a cheap
	//	stand-in for now.
	(void)filename;
	putchar('\a');
#endif
}
