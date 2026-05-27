
#ifdef AAA_FONT_H
#error "FONT_H included more than once."
#endif
#define AAA_FONT_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace GOL
{
	extern	void	make_raster_font();
	extern	void	draw_string( C_PCHAR_C s );
//	extern	void	hack_draw_string( C_PCHAR_C s );
}	//namespace GOL
