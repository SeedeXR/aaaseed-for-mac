#include "img.h"
#include "lib_wrappers/wrap_freeimage.h"
#include "FreeImagePlus.h"
#include "img_master.h"


o_str	c_img_utils::freeimage_version;

namespace aaa
{
namespace img
{
namespace freeimage
{


static	bool	b_dll_freeimage_loaded = false;


//void	FreeImageErrorHandler( FREE_IMAGE_FORMAT fif, const char *message )
//{
//	printf("\n*** ");
//	if(fif != FIF_UNKNOWN)
//	{
//		if (dll_freeimage.FreeImage_GetFormatFromFIF(fif) )
//			printf("%s Format\n", dll_freeimage.FreeImage_GetFormatFromFIF(fif));
//	}
//	printf(message);
//	printf(" ***\n");
//}

AAA_ERR	convert_from_freeimage( c_img_2d* img, fipImage& freeimage, bool b_load_data )
{
	//if ( freeimage.getBitsPerPixel() == 1 )
	//{
	//	// image is 1 bit monochrome convert it to 8 bit
	//	freeimage.convertToGrayscale();
	//}

	INT32			size_x			= freeimage.getWidth();
	INT32			size_y			= freeimage.getHeight();
	INT32			bit_per_pixel	= freeimage.getBitsPerPixel();
	INT32			channel_nb		= ::MAX( bit_per_pixel, 8 ) / 8;
	UINT32			pitch			= freeimage.getScanWidth();
	PIXEL_FORMAT	format_dst		= PIXEL_FORMAT::UNKNOWN;
	PIXEL_FORMAT	format_src		= PIXEL_FORMAT::BGRA_8;
	switch( channel_nb )
	{
	case 1:		format_dst = PIXEL_FORMAT::R_8;
				format_src	= ( bit_per_pixel == 1 ) ? PIXEL_FORMAT::BINARY : PIXEL_FORMAT::R_8;
				break;
	case 2:		format_dst = PIXEL_FORMAT::RG_8;
				format_src	= PIXEL_FORMAT::RG_8;
				break;
	case 3:		format_dst = PIXEL_FORMAT::RGB_8;
				format_src	= PIXEL_FORMAT::BGR_8;
				break;
	case 4:		format_dst = PIXEL_FORMAT::RGBA_8;
				format_src	= PIXEL_FORMAT::BGRA_8;
				break;
	}
	//todoimg centralize
	if( g_img_master->is_read_rgb_add_alpha() )
		format_dst = c_pixel_format::add_alpha( format_dst );
	if( g_img_master->is_read_bgr_keep() )
		format_dst = c_pixel_format::get_format_red_blue_swapped( format_dst );

	if( format_dst != PIXEL_FORMAT::UNKNOWN )
	{
		if( b_load_data )
		{
			//pointer to the image data
			UINT8*	src = (UINT8*)freeimage.accessPixels();
			if( IS_NULL( src ) )
				return ERR_ANY;
			img->copy_from_src( src, pitch, format_src, format_dst, size_x, size_y, false );
			return AAA_OK;
		}
		else
		{
			img->set_size_format( size_x,size_y, format_dst );
			return AAA_OK;
		}
	}
	else
		debug_break( "%s() dst is an unknown format", __FUNCTION__ );
	return ERR_ANY;
}

void	print_format()
{
	if( !b_dll_freeimage_loaded )	
		return;

	INT32	format_nb = dll_freeimage.FreeImage_GetFIFCount();
	for( INT32 i = 0; i < format_nb; ++i )
	{
		DBG_PRINT_STRING( "FreeImage supports %s",	dll_freeimage.FreeImage_GetFormatFromFIF(		(FREE_IMAGE_FORMAT)i ) );
		DBG_PRINT_STRING( "    extension list %s",	dll_freeimage.FreeImage_GetFIFExtensionList(	(FREE_IMAGE_FORMAT)i ) );
		DBG_PRINT_STRING( "    description %s",		dll_freeimage.FreeImage_GetFIFDescription(		(FREE_IMAGE_FORMAT)i ) );
	}
}

AAA_ERR	init()
{
	b_dll_freeimage_loaded = dll_freeimage.init();

	if( b_dll_freeimage_loaded )
	{
		c_img_utils::freeimage_version.set( dll_freeimage.FreeImage_GetVersion() );
		GOOD_PRINT_STRING( "FreeImage version : %s.", c_img_utils::freeimage_version.get() );
		return AAA_OK;
	}
	else
		ERR_PRINT_STRING( "Could not load FreeImage dll" );
	return ERR_ANY;
}

AAA_ERR	deinit()
{
	//FreeImage_DeInitialise();	// should be called for static link

	if( b_dll_freeimage_loaded )
		dll_freeimage.deinit();

	return AAA_OK;
}

AAA_ERR	read( c_img_2d *image, C_PCHAR_C filename, bool b_load_data )
{
	//C_PCHAR		filename	( image->get_filename() );
	//FIBITMAP*			data		( nullptr );
	fipImage			img;

	if( !img.load( filename, b_load_data ? 0 : FIF_LOAD_NOPIXELS ) )
	{
		ERR_PRINT_STRING( "FreeImage can't read image %s", filename );
		return ERR_ANY;
	}
	return convert_from_freeimage( image, img, b_load_data );
}

AAA_ERR	write( c_img_2d *image, C_PCHAR_C filename, c_img_utils::FILE_TYPE save_format )
{
	AAA_ERR				retcode			( AAA_OK );
	INT32				flags			( 0 );
	FREE_IMAGE_FORMAT	format_index	( FIF_UNKNOWN );
	c_img_2d*			img_save		= image;
	c_img_2d*			img_change		= nullptr;

	//fipImage			img;
	//dll_freeimage.FreeImage_SetOutputMessage( FreeImageErrorHandler );
	switch( save_format )
	{
	case c_img_utils::FILE_TYPE::PNG:
		format_index = FIF_PNG;
		if( c_img_utils::save_compression_ui < .1 )
			flags = PNG_Z_NO_COMPRESSION;
		else if( c_img_utils::save_compression_ui < .3 )
			flags = PNG_Z_BEST_SPEED;
		else if( c_img_utils::save_compression_ui < .8 )
			flags = PNG_Z_DEFAULT_COMPRESSION;
		else
			flags = PNG_Z_BEST_COMPRESSION;
		break;
	case c_img_utils::FILE_TYPE::JPG:
		format_index	= FIF_JPEG;
		flags			= (INT32) ( 100 * ( 1.0 - c_img_utils::save_compression_ui ) );
		break;
	case c_img_utils::FILE_TYPE::TIF:
	//todo refine this
		format_index	 = FIF_TIFF;
		if( c_img_utils::save_compression_ui < .1 )
			flags = TIFF_NONE;
		else if( c_img_utils::save_compression_ui < .3 )
			flags = TIFF_PACKBITS;
		else if( c_img_utils::save_compression_ui < .8 )
			flags = TIFF_PACKBITS;
		else
			flags = TIFF_LZW;
		break;
	case c_img_utils::FILE_TYPE::EXR:
		format_index	= FIF_EXR;
		flags			= EXR_NONE;
		break;
	}
	if( format_index == FIF_UNKNOWN )
	{
		ERR_PRINT_STRING( "%s() file %s we don't deal with this format with FreeImage yet.", __FUNCTION__, filename );
		retcode = ERR_ANY;
		goto exit;
	}
 

	FIBITMAP*	bitmap;
	{
		FREE_IMAGE_TYPE type = FIT_UNKNOWN;
		bool b_swap_red_blue = false;
		switch( image->get_pixel_format() )
		{
		case PIXEL_FORMAT::R_8 :
		case PIXEL_FORMAT::R_16 :		type = FIT_UINT16;	break;
	//	case PIXEL_FORMAT::DEPTH_16:
	//	case PIXEL_FORMAT::R_16FP:
		case PIXEL_FORMAT::DEPTH_32:
		case PIXEL_FORMAT::R_32FP:		type = FIT_FLOAT;	break;
		case PIXEL_FORMAT::BGR_8 :
		case PIXEL_FORMAT::BGRA_8 :		b_swap_red_blue = true;
		case PIXEL_FORMAT::RGB_8 :
		case PIXEL_FORMAT::RGBA_8:		type = FIT_BITMAP;	break;
		case PIXEL_FORMAT::BGR_16 :		b_swap_red_blue = true;
		case PIXEL_FORMAT::RGB_16 :		type = FIT_RGB16;	break;
		case PIXEL_FORMAT::BGRA_16 :	b_swap_red_blue = true;
		case PIXEL_FORMAT::RGBA_16:		type = FIT_RGBA16;	break;
		case PIXEL_FORMAT::BGR_32FP:	b_swap_red_blue = true;
		case PIXEL_FORMAT::RGB_32FP:	type = FIT_RGBF;	break;
		case PIXEL_FORMAT::BGRA_32FP:	b_swap_red_blue = true;
		case PIXEL_FORMAT::RGBA_32FP:	type = FIT_RGBAF;	break;
		default:
			retcode = ERR_ANY;
			goto exit;
		}

		if( b_swap_red_blue )
		{
			img_change = c_img_2d::img_init_from_img( image, "FreeImage BGR" );
			if( img_change->copy_from_img( image, false, true ) )
				img_save = img_change;
		}
		if( type == FIT_UNKNOWN )
			bitmap = dll_freeimage.FreeImage_ConvertFromRawBits(
						(BYTE*)img_save->get_data(),
						img_save->get_size_x(), img_save->get_size_y(),
						img_save->get_byte_pitch(), img_save->get_byte_per_pixel() * 8,
						0, 0, 0, true );
		else
			bitmap = dll_freeimage.FreeImage_ConvertFromRawBitsEx(
						false, (BYTE*)img_save->get_data(), 	type,
						img_save->get_size_x(), img_save->get_size_y(),
						img_save->get_byte_pitch(), img_save->get_byte_per_pixel() * 8,
						0, 0, 0, false );
	}
//	img = bitmap;
	
	if( bitmap )
	{
		BOOL		bResult;
	//	if( !image->is_bgr() )
		//FreeImage_GetChannel and FreeImage_SetChannel  to swap bgr to rgb
		//	dll_freeimage.SwapRedBlue32( bitmap );
		if( format_index == FIF_JPEG )
		{
			//img.convertTo24Bits();
			FIBITMAP*	bitmap_24 = dll_freeimage.FreeImage_ConvertTo24Bits( bitmap );
			bResult		= dll_freeimage.FreeImage_Save( format_index, bitmap_24, filename, flags );
			dll_freeimage.FreeImage_Unload( bitmap_24 );
			//bitmap_24	= nullptr;
		}
		else
		{
			bResult = dll_freeimage.FreeImage_Save( format_index, bitmap, filename, flags );
		}
		//bResult = img.save( image->get_filename(), flags );
		dll_freeimage.FreeImage_Unload( bitmap );
		bitmap = nullptr;
		if( !bResult )
		{
			ERR_PRINT_STRING( "FreeImage can't save image %s.", filename );
			retcode = ERR_ANY;
			goto exit;
		}
	}
	else
	{
		ERR_PRINT_STRING( "FreeImage can't allocate bitmap for saving.", filename );
		retcode = ERR_ANY;
		goto exit;
	}
exit:
	SAFE_DELETE( img_change );
	return	retcode;
}

}	//namespace freeimage
}	//namespace img
}	//namespace aaa