
#ifdef AAA_IMG_OPENEXR_H
#error "IMG_OPENEXR_H included more than once."
#endif
#define AAA_IMG_OPENEXR_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#if AAA_WIN64()
#	define	AAA_LIB_USE_IMG_OPENEXR() 1
#else
#	define	AAA_LIB_USE_IMG_OPENEXR() (AAA_VSTOOL() < 140)
#endif

#if AAA_LIB_USE_IMG_OPENEXR()

class c_img_2d;
namespace aaa
{
	namespace img
	{
		namespace openexr
		{
			extern void		init();
			extern void		deinit();
			extern AAA_ERR	write(	c_img_2d * image, C_PCHAR_C filename );
			extern AAA_ERR	read(	c_img_2d * image, C_PCHAR_C filename );
		}	//namespace openexr
	}	//namespace img
}	//namespace aaa

#endif
