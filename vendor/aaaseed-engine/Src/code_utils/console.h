
#ifdef AAA_CONSOLE_H
#error "CONSOLE_H included more than once."
#endif
#define AAA_CONSOLE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class console
{
public:
	typedef	void (COLOR_FN)();
//	PURE
	static	COLOR_FN	set_text_red;
	static	COLOR_FN	set_text_green;
	static	COLOR_FN	set_text_blue;
//	COMBINE
	static	COLOR_FN	set_text_yellow;
	static	COLOR_FN	set_text_cyan;
	static	COLOR_FN	set_text_purple;
	static	COLOR_FN	set_text_white;
//	LOW
	static	COLOR_FN	set_text_green_low;

//INVERSE
//	PURE
	static	COLOR_FN	set_text_red_inverse;
	static	COLOR_FN	set_text_green_inverse;
	static	COLOR_FN	set_text_blue_inverse;
//	COMBINE
	static	COLOR_FN	set_text_yellow_inverse;
	static	COLOR_FN	set_text_cyan_inverse;
	static	COLOR_FN	set_text_purple_inverse;
	static	COLOR_FN	set_text_white_inverse;
//	LOW
	static	COLOR_FN	set_text_green_low_inverse;

//REST

	static	COLOR_FN	set_text_normal;
	static	COLOR_FN	set_text_normal_inverse;

	static	void		get_color_info( FP32* color_dst, bool& inv_dst );

	static void			write( C_PCHAR_C buf );

};

