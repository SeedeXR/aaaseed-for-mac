
#ifdef AAA_ERR_H
#error "ERR_H included more than once."
#endif
#define AAA_ERR_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_CONSOLE_H
#	include "console.h"
#endif

namespace	aaa
{
namespace	mess
{
	enum TYPE : INT32
	{
	//	PRINT_CURRENT = 0,
		PRINT_NO_SEND = 0,
		PRINT_GOOD,
		PRINT_GOOD_INV,
		PRINT_ERR,
		PRINT_DBG,
		PRINT_UI,
		PRINT_WHITE,
		PRINT_WHITE_INV,
		PRINT_NO_MEDIA,
		PRINT_LUA_GOOD,
		PRINT_LUA_GOOD_INV,
		PRINT_LUA_DBG,
		PRINT_LUA_ERR,
		PRINT_TRACKERS,
		PRINT_TYPE_NB_MAX
	};

	extern  void	init();
	static INT32 CONSTEXPR BLK_PRINT_HEADER_BYTE_NB = 4;

	//	these are THE only external interface to err_mess
	extern	CHAR*	get_tmp();
	extern	bool	b_print_to_console;
	extern	CHAR*	get( INT32 index );
	extern	void	get_color( INT32 index, FP32* color, bool& b_inverse );
	extern	INT32	get_line_nb();
	
	extern	void	print_net_received( INT32 type, INT32 host_id, C_PCHAR_C msg );
	extern	INT32	print_to_tmp( C_PCHAR_C fmt, va_list args );

}	//namespace	mess
}	//namespace	aaa

/*
//	this is needed to do f(...) { g(...); }
template<INT32 count>
struct SVaPassNext
{
	SVaPassNext<count-1> big;
	DWORD dw;
};
template<> struct SVaPassNext<0>{};
//SVaPassNext - is generator of structure of any size at compile time.

class CVaPassNext
{
public:
	SVaPassNext<16> svapassnext;	//	with 32 or 50 the stack is smaller and I trigger exception on DS callback Fns
	CVaPassNext( va_list& args)
	{
		try
		{//to avoid access violation
			MEMCPY( &svapassnext, args, sizeof(svapassnext) );
		}
		catch (...)
		{}
	}
};

#define va_pass(valist) CVaPassNext(valist).svapassnext
//	see .c for an example of using this macro
*/

//todo	this very bad physical linkage
CONSTEXPR	INT32	ERR_MEM_BASE			=	0X100	;	//	MEMORY
CONSTEXPR	INT32	ERR_SOCK_BASE			=	0x200	;	//	SOCKET
CONSTEXPR	INT32	ERR_IMG_BASE			=	0x300	;	//	IMAGE
CONSTEXPR	INT32	ERR_FILE_BASE			=	0x400	;	//	FILE
CONSTEXPR	INT32	ERR_STRUCT_BASE			=	0x500	;	//	STRUCTURE

enum ERR_FILE : INT32
{
	ERR_FWRITE = ERR_FILE_BASE,
	ERR_FREAD,
	ERR_FOPEN,
	ERR_FCLOSE,
	ERR_MKDIR,
	ERR_FILE_NO,
	ERR_FILE_EOF_UNEXPECTED,
	ERR_FILE_FORMAT
};

//todo	use PlaySound on PC
extern	void	BELL();
extern	void	PRINT_STRING_BY_BLOCK(			C_PCHAR_C str,		INT32 len		);
extern	void	PRINT_STRING(					C_PCHAR_C fmt,		...				);
extern	void	PRINT_CR(						);		

extern	void	OBJ_PRINT_STRING_VA(			C_PCHAR_C header,	C_PCHAR_C fmt,	va_list args	);
extern	void	HEADER_PRINT_STRING(			C_PCHAR_C header,	C_PCHAR_C fmt,	...				);
extern	void	HEADER_PRINT_STRING_NO_CR(		C_PCHAR_C header,	C_PCHAR_C s		);
extern	void	HEADER_PRINT_STRING_VA(			C_PCHAR_C header,	C_PCHAR_C fmt,	va_list args	);

extern	void	SET_COLOR_NORMAL();

extern	void	HEADER_PRINT_STRING_VA_GENE( aaa::mess::TYPE type, console::COLOR_FN* fn, C_PCHAR_C header, C_PCHAR_C fmt, va_list args );

//ERR		there is a problem that the user should know
CONSTEXPR	CHAR ERR_HEADER[] = "!!! ERR-> ";
extern	void	SET_COLOR_ERR();
extern	void	SET_COLOR_WARNING();
extern	void	ERR_PRINT_STRING(									C_PCHAR_C fmt,	...				);
extern	void	ERR_PRINT_STRING_VA(								C_PCHAR_C fmt,	va_list args	);
extern	void	ERR_PRINT_STRING_INV(								C_PCHAR_C fmt,	...				);
extern	void	ERR_HEADER_PRINT_STRING(		C_PCHAR_C header,	C_PCHAR_C fmt,	...				);
extern	void	ERR_HEADER_PRINT_STRING_VA(		C_PCHAR_C header,	C_PCHAR_C fmt,	va_list args	);

extern	void	WIN_ERR_PRINT(										C_PCHAR_C msg = ""	);
// some objects have err_print as method, this is the fall back function in case object does not define err_print
extern	void	err_print(									C_PCHAR_C fmt,	...				);
extern	void	verbose_print_string(								C_PCHAR_C fmt,	...				);


//WHITE
extern	void	WHITE_PRINT_STRING(									C_PCHAR_C fmt,	...				);
extern	void	WHITE_PRINT_STRING_INV(								C_PCHAR_C fmt,	...				);

//TIMING	Print process timing information for Modules/Module/Group/Layer 
extern	void	SET_COLOR_TIMING();
extern	void	TIMING_PRINT_STRING(								C_PCHAR_C fmt,	...				);


//NO MEDIA	there is a media missing that the user should know but 
extern	void	SET_COLOR_NO_MEDIA();
extern	void	SET_COLOR_NO_MEDIA_INV();
extern	void	NO_MEDIA_PRINT_STRING(								C_PCHAR_C fmt,	...				);

//VERBOSE	this message are activated thru verbose_something
extern	void	VERBOSE_PRINT_STRING(								C_PCHAR_C fmt,	...				);
extern	void	VERBOSE_PRINT_STRING_INV(							C_PCHAR_C fmt,	...				);

//DEBUG		more info on the debug version
extern	void	SET_COLOR_DBG();
extern	void	DBG_PRINT_STRING_VA(								C_PCHAR_C fmt, va_list args		);
extern	void	DBG_PRINT_STRING(									C_PCHAR_C fmt,	...				);
// dbg_print() is the with-context companion of DBG_PRINT_STRING : it prints "# in <fn> : " using
// get_print_where() then forwards to DBG_PRINT_STRING. c_obj_ui has its own dbg_print() method
// that prefixes "in object <name>,". Use the case-and-name difference to tell them apart :
// upper-case macro DBG_PRINT_STRING for raw output, lower-case dbg_print for call-site / object context.
extern	void	dbg_print(											C_PCHAR_C fmt,	...				);

//GOOD		confirmation of thing going well
extern	void	GOOD_PRINT_STRING_NO_CR(							C_PCHAR_C s		);	
extern	void	GOOD_PRINT_STRING(									C_PCHAR_C fmt,	...				);
extern	void	GOOD_PRINT_STRING_INV(								C_PCHAR_C fmt,	...				);
// some objects have err_print as method, this is the fall back function in case object does not define err_print
extern	void	good_print(									C_PCHAR_C fmt,	...				);

//BAD		something happen but don't have an error status 
extern	void	WARNING_PRINT_STRING(								C_PCHAR_C fmt,	...				);
extern	void	WARNING_PRINT_STRING_INV(							C_PCHAR_C fmt,	...				);
extern	void	WARNING_HEADER_PRINT_STRING_VA(	C_PCHAR_C header,	C_PCHAR_C fmt,	va_list args	);
extern	void	WARNING_HEADER_PRINT_STRING(	C_PCHAR_C header,	C_PCHAR_C fmt,	...				);

//UI		help use the ui
extern	void	SET_COLOR_UI();
extern	void	UI_PRINT_STRING_VA(									C_PCHAR_C fmt, va_list args		);
extern	void	UI_PRINT_STRING(									C_PCHAR_C fmt,	...				);
// some objects have ui_print as method, this is the fall back function in case object does not define err_print
extern	void	ui_print(									C_PCHAR_C fmt,	...				);

//SWITCH
extern	void	SWITCH_PRINT_STRING(			C_PCHAR_C s			);
extern	void	SWITCH_PRINT_STRING(			C_PCHAR_C t,		C_PCHAR_C fmt,	...				);
extern	void	SWITCH_PRINT_STATE(				C_PCHAR_C t,		bool s							);

//LUA
extern	void	LUA_GOOD_PRINT_STRING(								C_PCHAR_C fmt,	...				);
extern	void	LUA_GOOD_PRINT_STRING_INV(							C_PCHAR_C fmt,	...				);
extern	void	LUA_DBG_PRINT_STRING(								C_PCHAR_C fmt,	...				);
extern	void	LUA_ERR_PRINT_STRING(								C_PCHAR_C fmt,	...				);

extern	void	BOX_ERR_FOR_LUA(				C_PCHAR_C str );
extern	void	BOX_WAR_FOR_LUA(				C_PCHAR_C str );
extern	void	BOX_GOOD_FOR_LUA(				C_PCHAR_C str );

extern	INT32	MESSAGE_BOX(	INT32 CONST size_x, void* hd_win,	C_PCHAR_C mess,		C_PCHAR_C title, UINT32 flags, INT32 ret_no_dialog = 0	);
extern	void	BOX_ERR(											C_PCHAR_C fmt,	...				);

extern	void	BOX_TITLE_ERR_VA(				C_PCHAR_C title,	C_PCHAR_C fmt,	va_list args	);
extern	void	BOX_TITLE_ERR(					C_PCHAR_C title,	C_PCHAR_C fmt,	...				);
extern	void	BOX_WAR(											C_PCHAR_C fmt,	...				);
extern	void	BOX_SIZE(	INT32 CONST size_x,	C_PCHAR_C title,	C_PCHAR_C fmt,	...				);
extern	INT32	BOX_DEV(						C_PCHAR_C title,	C_PCHAR_C fmt,	...				);
extern	bool	BOX_LUA(						C_PCHAR_C title,	C_PCHAR_C fmt,	...				);
extern	INT32	BOX_FINGER(						C_PCHAR_C title,	C_PCHAR_C fmt,	...				);
extern	bool	BOX_ASK_WAR(					C_PCHAR_C title,	C_PCHAR_C fmt,	...				);

#if	AAA_DEBUG()
	//todoq remake this
	//#define	DBG_NOT_IMPLEMENTED_FN(s)	PRINT_STRING("%s%s not implemented yet\n\t in %s at line %ld\n",DBG_HEADER,s,__FILE__,__LINE__ )
	FINLINE	void	DBG_NOT_IMPLEMENTED_FN(		C_PCHAR_C s )	{}
#else	//	DEBUG
	FINLINE	void	DBG_NOT_IMPLEMENTED_FN(		C_PCHAR_C s )	{}
#endif	//	DEBUG

extern	void		set_print_where(			C_PCHAR_C str		);
extern	C_PCHAR_C	get_print_where();
extern	void		debug_break_if(				bool b_break,		C_PCHAR_C fmt = nullptr, ... );

extern	bool	b_alert_with_sound_allowed;
extern	void	do_alert_with_sound(			C_PCHAR_C name		);

extern bool		is_message_box();
