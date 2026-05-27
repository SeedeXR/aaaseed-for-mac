
#include "image/img.h"
#include "lib_wrappers/wrap_gfl.h"


o_str	c_img_utils::gflsdk_version;
o_str	c_img_utils::gflsdk_libformat;

namespace aaa
{
namespace img
{
namespace gflsdk
{

namespace
{
	bool	b_dll_gfl_loaded = false;
}

AAA_ERR	convert_from_gflsdk( c_img_2d* img, GFL_BITMAP* image, GFL_FILE_INFORMATION* information )
{
	//GFL_BITMAP* image		= gfl_image;
	//GFL_BITMAP*	converted	= nullptr;
	AAA_ERR		retcode		= ERR_ANY;

	//if( information->BitsPerComponent == 1 )
	//{
	//	// Convert to 8 bit monochrome
	//	GFL_ERROR	gfl_error = dll_gfl.gflChangeColorDepth( gfl_image, &converted, GFL_MODE_TO_256GREY, GFL_MODE_NO_DITHER );
	//	if ( gfl_error != GFL_NO_ERROR )
	//	{
	//		ERR_PRINT_STRING( "Error converting to Mono 8 in %s()", __FUNCTION__ );
	//		goto exit;
	//	}
	//	image = converted;
	//}
	UINT8*	src = (UINT8*)image->Data;
	if( IS_NULL( src ) )
	{
		ERR_PRINT_STRING( "No data in GflSDK image" );
		goto exit;
	}

	{
		INT32			size_x		= image->Width;
		INT32			size_y		= image->Height;
		INT32			channel_nb	= image->ComponentsPerPixel;
		PIXEL_FORMAT	format_src	= PIXEL_FORMAT::UNKNOWN;
		PIXEL_FORMAT	format_dst	= PIXEL_FORMAT::RGBA_8;
		switch( channel_nb )
		{
		case 1:		format_dst = PIXEL_FORMAT::R_8;						format_src = ( image->BitsPerComponent == 1 ) ? PIXEL_FORMAT::BINARY : PIXEL_FORMAT::R_8;	break;
		case 3:		format_dst = PIXEL_FORMAT::RGB_8;					format_src = PIXEL_FORMAT::RGB_8;	break;
		case 4:		format_dst = PIXEL_FORMAT::RGBA_8;					format_src = PIXEL_FORMAT::RGBA_8;	break;
		}

		if( format_src != PIXEL_FORMAT::UNKNOWN )
		{
			img->copy_from_src( src, image->BytesPerLine, format_src, format_dst, size_x, size_y, false );
			retcode = AAA_OK;
		}
	}
exit:
	//if( converted )
	//{
	//	dll_gfl.gflFreeBitmap( converted );
	//}
	return retcode;
}

void	print_format()
{
	if( !b_dll_gfl_loaded )
		return;

	UINT32	nb_format = dll_gfl.gflGetNumberOfFormat();
	DBG_PRINT_STRING( "GflSDK supports %d formats.", nb_format );

	GFL_FORMAT_INFORMATION	info;
	for( UINT32 i = 0; i < nb_format; ++i )
	{
		o_str	str;
		dll_gfl.gflGetFormatInformationByIndex( i, &info );
		if ( info.NumberOfExtension > 0 )
		{
			str.add( info.Extension[ 0 ] );
			for( UINT32 j = 1; j < info.NumberOfExtension; ++j )
			{
				str.add( " ," );
				str.add( info.Extension[ j ] );
			}
		}
		DBG_PRINT_STRING( "GflSDK supports %s ( %s )", info.Description, str.get() );
	}
}

AAA_ERR	init()
{
	//gflSetPluginsPathname("..\\plugins\\");
	b_dll_gfl_loaded = dll_gfl.init();

	if( b_dll_gfl_loaded )
	{
		dll_gfl.gflLibraryInit(); 
		dll_gfl.gflEnableLZW( GFL_TRUE );

		c_img_utils::gflsdk_version.set( dll_gfl.gflGetVersion() );
		GOOD_PRINT_STRING( "GflSDK version %s.", c_img_utils::gflsdk_version.get() );

		c_img_utils::gflsdk_libformat.set( dll_gfl.gflGetVersionOfLibformat() );
		GOOD_PRINT_STRING( "GflSDK libformat version %s.", c_img_utils::gflsdk_libformat.get() );

		return AAA_OK;
	}
	else
	{
		ERR_PRINT_STRING( "Could not not GFL dll" );
	}
	return ERR_ANY;
}

AAA_ERR	deinit()
{
	if( b_dll_gfl_loaded )
	{
		dll_gfl.gflLibraryExit();
		dll_gfl.deinit();
		//wrap_gfl_Term();
	}
	return AAA_OK;
}

AAA_ERR	read( c_img_2d *image, C_PCHAR_C filename )
{
	AAA_ERR			retcode = AAA_OK;
	//C_PCHAR	filename = image->get_filename();

	DBG_HEAP_CHECK();

	if( !b_dll_gfl_loaded )
		return ERR_ANY;

	GFL_LOAD_PARAMS	load_params;

	dll_gfl.gflGetDefaultLoadParams( &load_params );
	load_params.Flags			|= GFL_LOAD_FORCE_COLOR_MODEL;
//	load_params.Flags			|= GFL_LOAD_BY_EXTENSION_ONLY;
//	load_params.Flags			|= GFL_LOAD_ONLY_FIRST_FRAME;
	load_params.Origin			= GFL_BOTTOM_LEFT;
//	load_params.Origin			= GFL_TOP_LEFT;
	load_params.ColorModel		= GFL_RGBA;
//	load_params.ColorModel		= GFL_BGR;
//	load_params.ColorModel		= GFL_RGB;
	load_params.LinePadding		= 4;
	load_params.DefaultAlpha	= 255;
	load_params.ImageWanted		= 1;
	//load_params.EpsDpi		= 72; 
	//load_params.EpsWidth		= 1024; 
	//load_params.EpsHeight		= 1024; 

	GFL_ERROR	gfl_error;

	//GFL_FORMAT_INFORMATION	informations;
	//gflGetFormatInformationByName( "pdf", &informations );

	//GFL_FILE_INFORMATION	information;
	//gfl_error =  gflGetFileInformation( filename, -1,  &information );

	GFL_FILE_INFORMATION information;
	gfl_error = dll_gfl.gflGetFileInformation( filename, -1, &information );

	if( gfl_error == GFL_NO_ERROR )
	{
		if( information.ComponentsPerPixel == 1 )
			load_params.ColorModel = GFL_GREY;

		//load_params.FormatIndex =  information.FormatIndex;
		//gflFreeFileInformation( &information );
		GFL_BITMAP* gfl_image = nullptr;
		gfl_error = dll_gfl.gflLoadBitmap( filename, &gfl_image, &load_params, &information );
		if( gfl_error == GFL_NO_ERROR )
		{
			if( gfl_image )
				retcode = convert_from_gflsdk( image, gfl_image, &information );
		}
		else
		{
			ERR_PRINT_STRING( "GflSDK can't read image %s : %s", image->get_filename(), dll_gfl.gflGetErrorString( gfl_error ) );
			retcode = ERR_ANY;
		}
		// free image
		if( gfl_image )
			dll_gfl.gflFreeBitmap( gfl_image );

		dll_gfl.gflFreeFileInformation( &information );	//done when gfl_error provoke crash
	}
	else
	{
		ERR_PRINT_STRING( "GflSDK can't read image information for image %s : %s", image->get_filename(), dll_gfl.gflGetErrorString( gfl_error ) );
		retcode = ERR_ANY;
	}	

	return	retcode;
}

AAA_ERR	read_info( c_img_2d *image, C_PCHAR_C filename )
{
	AAA_ERR			retcode		= AAA_OK;

	if( !b_dll_gfl_loaded )
		return ERR_ANY;

	GFL_ERROR	gfl_error;

	GFL_FILE_INFORMATION	information;
	gfl_error =  dll_gfl.gflGetFileInformation( filename, -1,  &information );

	if( gfl_error == GFL_NO_ERROR )
	{
		INT32			size_x		= information.Width;
		INT32			size_y		= information.Height;
		INT32			channel_nb	= information.ComponentsPerPixel;
		PIXEL_FORMAT	format		= c_pixel_format::make_format_from_channel_type(channel_nb);
		image->set_size_format( size_x,size_y, format );
		dll_gfl.gflFreeFileInformation( &information );
	}
	else
	{
		ERR_PRINT_STRING( "GflSDK can't read image information %s : %s", image->get_filename(), dll_gfl.gflGetErrorString( gfl_error ) );
		retcode = ERR_ANY;
	}
	
	return retcode;
}

// there is gflGetErrorString()
//namespace {
//	CHAR CONST * CONST get_err_str( INT32 error )
//	{
//		CHAR CONST * str;
//		switch( error )
//		{
//		case GFL_NO_ERROR				:	str = "GFL_NO_ERROR";				break;
//
//		case GFL_ERROR_FILE_OPEN        :	str = "GFL_ERROR_FILE_OPEN";		break;
//		case GFL_ERROR_FILE_READ        :	str = "GFL_ERROR_FILE_READ";		break;
//		case GFL_ERROR_FILE_CREATE      :	str = "GFL_ERROR_FILE_CREATE";		break;
//		case GFL_ERROR_FILE_WRITE       :	str = "GFL_ERROR_FILE_WRITE";		break;
//		case GFL_ERROR_NO_MEMORY        :	str = "GFL_ERROR_NO_MEMORY";		break;
//		case GFL_ERROR_UNKNOWN_FORMAT   :	str = "GFL_ERROR_UNKNOWN_FORMAT";	break;
//		case GFL_ERROR_BAD_BITMAP       :	str = "GFL_ERROR_BAD_BITMAP";		break;
//		case GFL_ERROR_BAD_FORMAT_INDEX :	str = "GFL_ERROR_BAD_FORMAT_INDEX";	break;
//		case GFL_ERROR_BAD_PARAMETERS   :	str = "GFL_ERROR_BAD_PARAMETERS";	break;
//
//		case GFL_UNKNOWN_ERROR          :	str = "GFL_UNKNOWN_ERROR";			break;
//		}
//		return str;
//	}
//}

AAA_ERR	write( c_img_2d *image, C_PCHAR_C filename, c_img_utils::FILE_TYPE save_format )
{
	AAA_ERR		retcode = AAA_OK;
	GFL_ERROR	error; 
	GFL_BITMAP*	gfl_bitmap = nullptr;

	DBG_HEAP_CHECK();

	if( !b_dll_gfl_loaded )
		return ERR_ANY;

	GFL_INT32	format_index = 0;
	switch( save_format )
	{
	case c_img_utils::FILE_TYPE::PNG:	format_index = dll_gfl.gflGetFormatIndexByName( "png" );	break;
	case c_img_utils::FILE_TYPE::JPG:	format_index = dll_gfl.gflGetFormatIndexByName( "jpeg" );	break;
	case c_img_utils::FILE_TYPE::TIF:	format_index = dll_gfl.gflGetFormatIndexByName( "tiff" );	break;
	}	
	if( format_index == -1 )
	{
		ERR_PRINT_STRING( "%s() file %s we don't deal with this format in Xnview yet.", __FUNCTION__, filename );
		retcode = c_img_utils::ERR_FILE_TYPE;
		goto exit;
	}
	if( dll_gfl.gflFormatIsWritableByIndex(format_index) == GFL_FALSE )
	{
		ERR_PRINT_STRING( "%s() file %s Xnview can't write in thios format.", __FUNCTION__, filename );
		retcode = c_img_utils::ERR_FILE_TYPE;
		goto exit;
	}

	//todo check with gflFormatIsWritableByIndex()
	GFL_SAVE_PARAMS	save_params;
	dll_gfl.gflGetDefaultSaveParams( &save_params );
	save_params.Flags					|= GFL_SAVE_REPLACE_EXTENSION;
	save_params.Flags					|= GFL_SAVE_ANYWAY;
	save_params.FormatIndex				= format_index;
	save_params.Compression				= 0;
	save_params.Quality					= GFL_INT16( 100 * ( 1.0 - c_img_utils::save_compression_ui ) ); //JPEG Compression 0 - Worst - 100 Best
	save_params.CompressionLevel		= GFL_INT16( 9 * c_img_utils::save_compression_ui ); // PNG Compression 0 min - 9 max
	save_params.OptimizeHuffmanTable	= true;

	GFL_BITMAP_TYPE	bitmap_type;
	//todo refine extend use add alpha and bgr read option 
	{
		auto image_format = image->get_pixel_format();
		switch( image_format )
		{
		case PIXEL_FORMAT::R_8 :
		case PIXEL_FORMAT::R_16 :	bitmap_type = GFL_GREY;	break;
		case PIXEL_FORMAT::RGB_8 :
		case PIXEL_FORMAT::RGB_16 :	bitmap_type = GFL_RGB;	break;
		case PIXEL_FORMAT::RGBA_8:
		case PIXEL_FORMAT::RGBA_16:	bitmap_type = GFL_RGBA;	break;
		default:
			ERR_PRINT_STRING( "GflSDK can't save image %s with this format %s.", filename, c_pixel_format::get_name(image_format) );
			retcode = c_img_utils::ERR_CANT_WRITE;
			goto exit;
		}
	}
	{
		bool b_16 = aaa::c_pixel_format::is_format_i16( image->get_pixel_format() );
		gfl_bitmap = dll_gfl.gflAllockBitmapEx(	bitmap_type,
												image->get_size_x(), image->get_size_y(),
												b_16 ? 16 : 8, image->get_channel_nb(),
												nullptr );
		if( !gfl_bitmap )
		{
			ERR_PRINT_STRING( "GflSDK can't allocate bitmap for saving %s.", filename );
			retcode = ERR_ANY;
			goto exit;
		}

		if( b_16 )
		{
			// we do this to flip vertically
			c_img_2d* img_change = c_img_2d::img_init_from_img( image, "Glf save" );
			c_img_2d* img_src = img_change->copy_from_img( image, true, false ) ? img_change : image;
			MEMCPY( gfl_bitmap->Data, img_src->get_data(), img_src->get_size_y() * img_src->get_byte_pitch(), __FUNCTION__ );
			SAFE_DELETE( img_change );
		}
		else
		{
			MEMCPY( gfl_bitmap->Data, image->get_data(), image->get_size_y() * image->get_byte_pitch(), __FUNCTION__ );

			error = dll_gfl.gflFlipVertical( gfl_bitmap, nullptr );
			if( error != GFL_NO_ERROR )
			{
				ERR_PRINT_STRING( "GflSDK did not Flip Vertically bitmap for saving : %s", dll_gfl.gflGetErrorString(error));
				if( b_16 )
					ERR_PRINT_STRING( "GflSDK probably don't do it in 16 bits per component" );
			}
		}
	}

	dll_gfl.gflBitmapSetComment( gfl_bitmap, "Saved by AAASeed" );

	//todo we should work on the path here : create it so gfl can save
	error = dll_gfl.gflSaveBitmap( (char*)filename, gfl_bitmap, &save_params );
	if( error != GFL_NO_ERROR )
	{
		ERR_PRINT_STRING( "GflSDK error %s : can't save image %s.", dll_gfl.gflGetErrorString(error), filename );
		retcode = ERR_ANY;
		goto exit;
	}

exit:
	if( gfl_bitmap )
	{
		// free image
		dll_gfl.gflFreeBitmap( gfl_bitmap );
	}
	return	retcode;
}

}	//namespace gflsdk
}	//namespace img
}	//namespace aaa