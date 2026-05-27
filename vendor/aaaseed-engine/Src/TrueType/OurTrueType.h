
#ifdef AAA_OURTRUETYPE_H
#error "OURTRUETYPE_H included more than once."
#endif
#define AAA_OURTRUETYPE_H 1


#define AAA_TRUETYPE1_USE() 0

#if AAA_TRUETYPE1_USE() 
#	define AAA_TRUETYPE2_USE() 0	// at 1 it does not compile
#else
#	define AAA_TRUETYPE2_USE() 0	//can't be 1 if AAA_TRUETYPE1_USE is false
#endif

#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class	c_bind;

namespace aaa::font {

CONSTEXPR	INT32	FONT_MAX_NB = 64 ;

extern	REAL	g_precision;
extern	c_bind*	g_bank_def;

extern	AAA_ERR	init();
extern	void	deinit();

extern	INT32	bind_menu_build();
extern	void	load_from_file(		UINT32 CONST index, o_str CONST & filename_in );
extern	void	load_from_file(		o_str CONST & filename_in );

// used by c_bdd_cell_draw 
extern	REAL	get_height(			INT32 CONST font_bind, bool CONST b_outline );
extern	void	render(				C_PCHAR_C str,		INT32 CONST font_bind, bool CONST b_outline, REAL CONST align_hori, REAL CONST interline = 1. );
extern	void	render(				o_str CONST &  str,	INT32 CONST font_bind, bool CONST b_outline, REAL CONST align_hori, REAL CONST interline = 1. );

// used by c_bdd_tex2d
extern	void	fill_pos( REAL* pt,	C_PCHAR_C str,	INT32 CONST font_bind, bool CONST b_outline, REAL CONST align_hori, REAL CONST interline = 1. );
extern	void	render_char(		UINT8 CONST ch,	INT32 CONST font_bind, bool CONST b_outline );


#if AAA_TRUETYPE1_USE()
	extern	REAL	get_width(		C_PCHAR_C str,	INT32 CONST font_bind, bool CONST b_outline );
	extern	void	render_direct(	C_PCHAR_C str,	INT32 CONST font_bind, bool CONST b_outline, REAL CONST align_hori, REAL CONST interline = 1. );
	//extern	void	font_get_rect( char* str, INT32 font_bind);
#endif	//#if AAA_TRUETYPE1_USE()



} //namespace aaa::font

