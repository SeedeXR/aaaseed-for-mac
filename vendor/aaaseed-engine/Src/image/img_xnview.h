
#ifdef AAA_IMG_XNVIEW_H
#error "IMG_XNVIEW_H included more than once."
#endif
#define AAA_IMG_XNVIEW_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_IMG_H
#	include "img.h"
#endif

namespace aaa
{
	namespace img
	{
		namespace gflsdk
		{
			extern AAA_ERR	init();
			extern AAA_ERR	deinit();
			extern AAA_ERR	write(		c_img_2d *image, C_PCHAR_C filename, c_img_utils::FILE_TYPE save_format );
			extern AAA_ERR	read(		c_img_2d *image, C_PCHAR_C filename );
			extern AAA_ERR	read_info(	c_img_2d *image, C_PCHAR_C filename );
			extern void		print_format();
		}	//namespace gflsdk
	}	//namespace img
}	//namespace aaa
