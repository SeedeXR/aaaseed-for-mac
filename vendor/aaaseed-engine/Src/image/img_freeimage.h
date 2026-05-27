
#ifdef AAA_IMG_FREEIMAGE_H
#error "IMG_FREEIMAGE_H included more than once."
#endif
#define AAA_IMG_FREEIMAGE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_IMG_H
#	include "image/img.h"
#endif

namespace aaa
{
	namespace img
	{
		namespace freeimage
		{
			extern AAA_ERR	init();
			extern AAA_ERR	deinit();
			extern AAA_ERR	write( c_img_2d * image, C_PCHAR_C filename, c_img_utils::FILE_TYPE save_format );
			extern AAA_ERR	read(  c_img_2d * image, C_PCHAR_C filename, bool b_load_data = true );
			extern void		print_format();
		}	//namespace freeimage
	}	//namespace img
}	//namespace aaa


