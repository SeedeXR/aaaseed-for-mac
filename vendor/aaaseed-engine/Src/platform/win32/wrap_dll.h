#ifdef AAA_WRAP_DLL_H
#error "WRAP_DLL_H included more than once."
#endif
#define AAA_WRAP_DLL_H 1


// File: wrap_dll.h - generic dll wrapper (platform dependend)
//
// Created: sr@20100421


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#ifndef	_WINDOWS_
#	ifndef	AAA_AAA_OS_H
#		include "aaa_os.h"
#	endif
#	include		<Delayimp.h>		// Delayed load dll
#endif

#define	WRAP_MAA_WAY	1

#ifndef WRAP_SECTION_NAME
#define	WRAP_SECTION_NAME	"AAA_WRAP"
#endif
//#pragma data_seg( WRAP_SECTION_NAME ) 

//=============================================================================
// useful macros...
//
#define INVALID_HINSTANCE		( (HINSTANCE)(INVALID_HANDLE_VALUE) )
#define WRAP_CONCAT(x,y)		x##y
//#pragm
/*
//tried
//#include		"boost\preprocessor\slot\counter.hpp"
//static	INT32 WRAP_CONCAT( cnt_, WRAP_SECTION_NAME ) = 0
static int FUCKING_CNT = 0;
#define ADEFINE			++FUCKING_CNT
*/

//=============================================================================
//
static	int	_unresolved_call( void );	// has to be implemented per wrapper

//=============================================================================
// Infrastructure

#if	WRAP_MAA_WAY

class c_wrap_dll_item;
static	c_wrap_dll_item*	g_wrap_prev;

class c_wrap_dll_item{
public:
	char*				name;
	FARPROC				fn;
	c_wrap_dll_item*	next;

	c_wrap_dll_item( char* name_in, FARPROC fn_in, bool b_first )
	{
		if( b_first )
			g_wrap_prev = nullptr;
		name	=	name_in;
		fn		=	fn_in;
		next	=	nullptr;
		if( g_wrap_prev )
			g_wrap_prev->next = this;
		g_wrap_prev	=	this;
	}
};

extern	UINT32		wrap_dll_init(			c_wrap_dll_item& p_beg	);
extern	UINT32		wrap_dll_deinit(		c_wrap_dll_item& p_beg, LPVOID undef_call );

#else

extern	UINT32		wrap_dll_init(			c_wrap_dll_item* all, LPVOID* beg, LPVOID* wrap_end );
extern	UINT32		wrap_dll_deinit(		LPVOID* wrap_dll, LPVOID undef_call );

#endif	//#if		WRAP_MAA_WAY

extern	void		wrap_unresolved_call(	LPCSTR lib_name );		// just error print
extern	ptrdiff_t	wrap_dll_func(			LPVOID func_name, LPVOID func_addr );

extern	UINT32		wrap_dll_init_linked(	LPCSTR p_name );
extern	UINT32		wrap_dll_deinit_linked(	LPCSTR p_name );


#define XSTR(x) STR(x)
#define STR(x) #x
//__pragma( message ( XSTR( __COUNTER__ ) " -- " XSTR(func)  ) )


#define NAME2(fun,suffix) fun ## "_" ## suffix
#define NAME1(fun,suffix) NAME2(fun,suffix)
#define NAME(fun) NAME1(fun,XSTR( __CNT ))

static int __CNT = __COUNTER__;
//static int __CNT = 0


//#pragma init_seg( user )

#if	WRAP_MAA_WAY
//	opening record
//	__pragma( auto_inline( off ) )
#define DECL_WRAPPER_DLL( wrap_name, dll_name ) \
	__pragma( message ( "   ******** begin "#wrap_name" to wrap "dll_name ) ) \
	static	c_wrap_dll_item		wrap_name##_wrap_begin_( dll_name, (FARPROC) INVALID_HINSTANCE, true );

#define DECL_WRAPPER( func ) \
	c_wrap_dll_item		func##_wrap_one_( #func, (FARPROC) (_unresolved_call), false );

//	closing record
#define DECL_WRAPPER_END( wrap_name ) \
	__pragma( message ("   *************** end "#wrap_name) )
//	static	c_wrap_dll_item		wrap_name##_st_end_( (LPSTR)(INVALID_HINSTANCE), nullptr, false );

#define	WRAP_DLL(			wrap_name )			wrap_dll_init(			wrap_name##_wrap_begin_			)
#define	UNWRAP_DLL(			wrap_name )			wrap_dll_deinit(		wrap_name##_wrap_begin_,		(FARPROC)(_unresolved_call)	)
#define	WRAP_UNRESOLVED(	wrap_name )			wrap_unresolved_call(	wrap_name##_wrap_begin_.name	)

#else	//#if	WRAP_MAA_WAY

#if 1

//	opening record
//	__pragma( auto_inline( off ) )
#define DECL_WRAPPER_DLL( wrap_name, dll_name ) \
	__pragma( message ( "   ******** open section "#wrap_name" for "dll_name" with name "WRAP_SECTION_NAME ) ) \
	__pragma( section( WRAP_SECTION_NAME, read, write ) ) \
	__declspec( allocate( WRAP_SECTION_NAME ) ) c_wrap		wrap_name##_st_beg_( dll_name, (FARPROC) INVALID_HINSTANCE, TRUE );
//	prev_wrap = &(wrap_name##_st_beg_);

//	__declspec( allocate( WRAP_SECTION_NAME ) ) char*			WRAP_CONCAT( wrap_name, _w_beg_  ) = dll_name;	\
//	__declspec( allocate( WRAP_SECTION_NAME ) ) HINSTANCE		WRAP_CONCAT( wrap_name, _h_dll_ ) = INVALID_HINSTANCE;


#define DECL_WRAPPER( func ) \
	__pragma( section( WRAP_SECTION_NAME, read, write ) ) \
	__declspec( allocate( WRAP_SECTION_NAME ) ) char*		WRAP_CONCAT( func, _n_ ) = #func; \
	__declspec( allocate( WRAP_SECTION_NAME ) ) FARPROC		WRAP_CONCAT( func, _p_ ) = (FARPROC) (_unresolved_call);
//	closing record
#define DECL_WRAPPER_END( wrap_name ) \
	__pragma( message ("   *************** close "#wrap_name) )\
	__declspec( allocate( WRAP_SECTION_NAME ) ) c_wrap		wrap_name##_st_end_( (LPSTR)(INVALID_HINSTANCE), nullptr, FALSE );

//__declspec( allocate( WRAP_SECTION_NAME ) ) char*		WRAP_CONCAT( wrap_name, _w_end_ ) = (LPSTR)(INVALID_HINSTANCE);

//	__pragma( section( ".other", read, write ) ) \
//	_declspec( allocate(WRAP_SECTION_NAME) ) HINSTANCE		WRAP_CONCAT( _w_end_bis_, wrap_name) = INVALID_HINSTANCE;
#else	
//	opening record
#define DECL_WRAPPER_DLL( wrap_name, dll_name ) \
	__pragma( message ( "**********************************************************************") ) \
	__pragma( message ( "open section "#wrap_name" for "dll_name" with name "WRAP_SECTION_NAME ) ) \
	__pragma( data_seg( WRAP_SECTION_NAME$##__COUNTER__ ) ) \
	char*			WRAP_CONCAT( wrap_name, _w_beg_ ) = dll_name; \
	HINSTANCE		WRAP_CONCAT( wrap_name, _h_dll_ ) = INVALID_HINSTANCE;  \
	__pragma( data_seg() )

//	function record
#define DECL_WRAPPER( func ) \
	__pragma( data_seg( WRAP_SECTION_NAME$##__COUNTER__ ) ) \
	char*			WRAP_CONCAT( func, _n_ ) = #func; \
	FARPROC			WRAP_CONCAT( func, _p_ ) = (FARPROC) (_unresolved_call); \
	__pragma( data_seg() )
//	closing record
#define DECL_WRAPPER_END( wrap_name ) \
	__pragma( message ("close section "#wrap_name) )\
	__pragma( message ("**********************************************************************") ) \
	__pragma( data_seg( WRAP_SECTION_NAME$##__COUNTER__ ) ) \
	char*			WRAP_CONCAT( wrap_name, _w_end_ ) = (LPSTR)(INVALID_HINSTANCE); \
	__pragma( data_seg() )
//	_declspec( allocate(WRAP_SECTION_NAME) ) HINSTANCE		WRAP_CONCAT( _w_end_bis_, wrap_name) = INVALID_HINSTANCE;
#endif

#define	WRAP_DLL( wrap_name )			wrap_dll_init(	(LPVOID*)	&WRAP_CONCAT( wrap_name, _w_beg_ ), \
	(LPVOID*)	&WRAP_CONCAT( wrap_name, _h_dll_ ), \
	(LPVOID*)	&WRAP_CONCAT( wrap_name, _w_end_ ) )

#define	UNWRAP_DLL( wrap_name )			wrap_dll_deinit((LPVOID*)	&WRAP_CONCAT( wrap_name, _w_beg_ ), (FARPROC)(_unresolved_call) )
#define	WRAP_UNRESOLVED( wrap_name )	wrap_unresolved_call( WRAP_CONCAT( wrap_name, _w_beg_ ) )
#endif	//#if	WRAP_MAA_WAY

//	Directly (without this macro it don't work)
#define WRAP_DELAY_STRING(x)	#x

#define	WRAP_DLL_LINKED( wrap_name )	wrap_dll_init_linked(	WRAP_DELAY_STRING( wrap_name ) )

#define	UNWRAP_DLL_LINKED( wrap_name )	wrap_dll_deinit_linked(	WRAP_DELAY_STRING( wrap_name ) )



//__FUNCTION__
#define	WRAP_CALL2JUMP_OLD( func ) \
{ \
	__asm { \
	jmp	[func##_p_] \
	} \
	wrap_dll_func( (LPVOID)&func##_n_, (LPVOID)&func##_p_ ); /* trick */  \
}

#define	WRAP_CALL2JUMP_MAA( func ) \
{ \
	DECL_WRAPPER( func ) \
	__asm { \
	jmp	[func##_p_] \
	} \
	wrap_dll_func( (LPVOID)&func##_n_, (LPVOID)&func##_p_ ); /* trick */  \
}

#if WRAP_MAA_WAY

#define	WRAP_CALL2JUMP( func )					\
{												\
	extern c_wrap_dll_item	func##_wrap_one_;	\
	__asm { jmp	[func##_wrap_one_.fn] }			\
}												\
	DECL_WRAPPER( func )

#else

#define	WRAP_CALL2JUMP( func )		\
{									\
	extern FARPROC	func##_p_;		\
	extern char*	func##_n_;		\
	__asm { jmp	[func##_p_] }		\
	wrap_dll_func( (LPVOID)&func##_n_, (LPVOID)&func##_p_ ); /* trick */ \
}									\
	DECL_WRAPPER( func )

#endif
//-----------------------------------------------------------------------------





// EOF: wrap_dll.h