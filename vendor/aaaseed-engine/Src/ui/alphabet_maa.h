
#ifdef AAA_ALPHABET_MAA_H
#error "ALPHABET_MAA_H included more than once."
#endif
#define AAA_ALPHABET_MAA_H 1


#ifndef AAA_ALPHABET_H
#	include "alphabet.h"
#endif

namespace aaa::alphabet::maa
{
	typedef	void	(*p_fn_draw)();

	extern INT32 		compile_lists();
	extern INT32 		build_vbo();
	extern void			stroke_char( UINT8 CONST* str, INT32 len );
	extern REAL			str_draw_translate_x[];
	extern p_fn_draw	str_draw_array[];

	extern	void		draw_switch_line( bool CONST b_on );
	extern	void		draw_slider_line( REAL CONST in, INT32 CONST mark_nb, bool CONST b_left, bool CONST b_right );
}

