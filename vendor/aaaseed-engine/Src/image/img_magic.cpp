#include "image/img.h"
#include <Magick++.h>
//#include <iostream>
//using namespace std;
//using namespace Magick;

AAA_ERR	img_magick_init()
{
	//todonow, linking not working in Release Mode
//	Magick::InitializeMagick( "" );
	return AAA_OK;
}

AAA_ERR convert_from_image_magic( Magick::Image* magick_image, c_img* img)
{
INT32	size_x = magick_image->columns();
INT32	size_y = magick_image->rows();
INT32	channel_nb = 4;
Magick::PixelPacket*	pp;

	img->init_with_image_size( size_x, size_y, channel_nb);
//	pp = GetImagePixels( magick_image, 0, 0, size_x, size_y);
	pp = magick_image->getPixels( 0, 0, size_x, size_y);
	if( pp )
	{
		UINT8*	dst;
		INT32	pixel_count;
		INT32	line_index;
		bool const b_matte = magick_image->matte();

		dst = img->get_data();
		pixel_count = 0;
		line_index = size_y;

		for( INT32 i = size_x * size_y; i > 0; --i )
		{
			if( --pixel_count < 0)
			{
				line_index--;
				dst = img->get_data() + size_x * 4 * line_index;
				pixel_count = size_x - 1;
			}
			*dst++ = pp->red;
			*dst++ = pp->green;
			*dst++ = pp->blue;
			if( b_matte )
			{
				if( img->get_file_type() == IMG_FILE_TYPE_TGA )
					*dst++ = 255 - pp->opacity;
				else
					*dst++ = UINT8(pp->opacity);
			}
			else
				*dst++ = 255;
			pp++;
		}
	}
	img->set_changed(true);
	img->clear_empty();
	return( AAA_OK);
}

AAA_ERR	img_magick_read( c_img *image)
{
AAA_ERR		retcode = ERR_ANY;
std::string filename( image->get_filename() );
Magick::Image magick_image;

	mem::DBG_CHECK_HEAP();
	try
		{
		magick_image.read( filename);
		}
	catch(...)
		{
		ERR_PRINT_STRING( "ImageMagick can't read image %s.", image->get_filename());
		goto exit;
		}
	// Ensure that there are no other references to this image.
	magick_image.modifyImage();
	// Set the image type to TrueColor DirectClass representation.
	magick_image.type( Magick::TrueColorType);
	if ( magick_image.isValid() )
		{
		retcode = convert_from_image_magic( &magick_image, image);
		}
	else
		goto exit;
exit:
	return	retcode;
}

/* old version for image magick 4.2 ..something
AAA_ERR c_img::convert_from_image_magic( Image *magick_image)
{
register int	i;
register int	j;

register	RunlengthPacket	*p;
register	UINT8			*dst;
register	INT32			pixel_count;
register	INT32			line_index;

	type = IMG_TYPE_RGB;
	size_x = magick_image->columns;
	size_y = magick_image->rows;
	channel_nb = 4;
	byte_per_pixel = channel_nb;
	data_alloc();

	//	No interlacing:  RGBRGBRGBRGBRGBRGB...
	p = magick_image->pixels;
	dst = data;
	pixel_count = 0;
	line_index = size_y;

	for ( i=magick_image->packets; i>0 ; i-- )
		{
		for (j=0; j <= ((int) p->length); ++j)
			{
			if( --pixel_count < 0)
				{
				line_index--;
				dst = data + size_x * byte_per_pixel * line_index;
				pixel_count = size_x - 1;
				}
			*dst++ = p->red;
			*dst++ = p->green;
			*dst++ = p->blue;
			if (magick_image->matte)
				{
				if( file_type == IMG_FILE_TYPE_TGA )
					*dst++ = 255 - p->index;
				else
					*dst++ = UINT8(p->index);
				}
			else
				*dst++ = 255;
			}
		p++;
		}
	set_changed(true);
	empty = false;
	return( AAA_OK);
}
*/
