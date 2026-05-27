#include "img_openexr.h"
#include "img.h"

o_str	c_img_utils::openexr_version;


#if AAA_LIB_USE_IMG_OPENEXR()

//#include "OpenEXR/ImfRgbaFile.h"
//#include "OpenEXR/ImfArray.h"
#include "OpenEXR/ImfInputFile.h"
#include "OpenEXR/ImfOutputFile.h"
#include "OpenEXR/ImfChannelList.h"

#include <lib_use.h>
#if AAA_DEBUG()
//	AAA_LIB_USE64( "Poco/zlibstaticd"				)
	AAA_LIB_USE64( "OpenEXR/Debug/IlmImf-2_4_d"		)
	AAA_LIB_USE64( "OpenEXR/Debug/Iex-2_4_d"		)
	AAA_LIB_USE64( "OpenEXR/Debug/IlmThread-2_4_d"	)
	AAA_LIB_USE64( "OpenEXR/Debug/Half-2_4_d"		)
#else
//	AAA_LIB_USE64( "Poco/zlibstatic"				)
	AAA_LIB_USE64( "OpenEXR/IlmImf-2_4"				)
	AAA_LIB_USE64( "OpenEXR/Iex-2_4"				)
	AAA_LIB_USE64( "OpenEXR/IlmThread-2_4"			)
	AAA_LIB_USE64( "OpenEXR/Half-2_4"				)
#endif

//namespace Imf = OPENEXR_IMF_NAMESPACE;

namespace aaa
{
namespace img
{
namespace openexr
{

void init()
{
	::c_img_utils::openexr_version.set( OPENEXR_VERSION_STRING );
}

void deinit()
{
}

AAA_ERR	write( c_img_2d * image, C_PCHAR_C filename )
{
	AAA_ERR	retcode = c_img_utils::ERR_CANT_WRITE;
	c_img_2d*	img_flip_v = nullptr;

	try
	{

		c_img_2d*	img;
		if( true )
		{
			img_flip_v = c_img_2d::create( __FUNCTION__ );
			img_flip_v->copy_from_img( image, true, false );
			img = img_flip_v;
		}
		else
			img = image;

		INT32	sixe_x	= img->get_size_x();
		INT32	size_y	= img->get_size_y();
		Imf::Header header( sixe_x, size_y );

		//header.dataWindow() = dataWindow;
		auto	type	= Imf::PixelType::HALF;
		bool	b_fp32	= false;
		//todo we should do i16 too
		auto src_type = img->get_data_type();
		switch( src_type )
		{
		case aaa::PIXEL_TYPE::FLOAT_16:
			type = Imf::PixelType::HALF;
			break;
		case aaa::PIXEL_TYPE::FLOAT_32:
			type = Imf::PixelType::FLOAT;
			b_fp32 = true;
			break;
		default:
			retcode = ERR_UNIMPLEMENTED_YET;
			ERR_PRINT_STRING( "AAASeed don't deal yet with saving EXR for pixel type %s", aaa::c_pixel_format::get_pixel_type_name(src_type) );
			goto exit;
		}

		auto	ch_nb = img->get_channel_nb();
		auto	b_bgr = aaa::c_pixel_format::is_bgr( img->get_pixel_format() );
		if( ch_nb >= 1 )
		{
			header.channels().insert( b_bgr ? "B" : "R", Imf::Channel( type ) );
			if ( ch_nb >= 2)
			{
				header.channels().insert( "G", Imf::Channel( type ) );
				if ( ch_nb >= 3 )
				{
					header.channels().insert(  b_bgr ? "R" : "B", Imf::Channel( type ) );
					if ( ch_nb >= 4 )
					{
						header.channels().insert( "A", Imf::Channel( type ) );
					}
				}
			}
		}
		else
		{
			retcode = c_img_utils::ERR_DATA_NO;
			ERR_PRINT_STRING( "%s() channel number is %d", __FUNCTION__, ch_nb );
			goto exit;
		}

		Imf::OutputFile file( filename, header );

		size_t	pitch_x;
		CHAR	*src_1, *src_2, *src_3, *src_4;
		if( b_fp32 )
		{
			pitch_x = sizeof( FP32 ) * ch_nb;
			FP32* pixels = img->get_data_fp32();
			src_1 = reinterpret_cast<CHAR *>(pixels+0);
			src_2 = reinterpret_cast<CHAR *>(pixels+1);
			src_3 = reinterpret_cast<CHAR *>(pixels+2);
			src_4 = reinterpret_cast<CHAR *>(pixels+3);
		}
		else
		{	
			pitch_x = sizeof( FP16 ) * ch_nb;
			FP16* pixels = img->get_data_fp16();
			src_1 = reinterpret_cast<CHAR *>(pixels+0);
			src_2 = reinterpret_cast<CHAR *>(pixels+1);
			src_3 = reinterpret_cast<CHAR *>(pixels+2);
			src_4 = reinterpret_cast<CHAR *>(pixels+3);
		}
		size_t	pitch_y = img->get_byte_pitch();

		Imf::FrameBuffer frame_buffer;
		frame_buffer.insert( b_bgr ? "B" : "R",	// name
								Imf::Slice( type,				// type
											src_1,				// base
											pitch_x,pitch_y,	// xStride, yStride
											1,1, 0.0 )			// x/y sampling, fillValue
							);
		if ( ch_nb >= 2 )
		{
			frame_buffer.insert( "G",			// name
									Imf::Slice( type,				// type
												src_2,				// base
												pitch_x,pitch_y,	// xStride, yStride
												1,1, 0.0 )			// x/y sampling, fillValue
									);
			if ( ch_nb >= 3 )
			{
				frame_buffer.insert( b_bgr ? "R" : "B",	// name
										Imf::Slice( type,				// type
													src_3,				// base
													pitch_x,pitch_y,	// xStride, yStride
													1,1, 0.0 )			// x/y sampling, fillValue
									);
				if ( ch_nb >= 4 )
				{
					frame_buffer.insert( "A",			// name
											Imf::Slice( type,				// type
														src_4,				// base
														pitch_x,pitch_y,	// xStride, yStride
														1,1, 0.0 )			// x/y sampling, fillValue
											);
				}
			}
		}

		file.setFrameBuffer( frame_buffer );
		file.writePixels( size_y );
	}
	catch ( ... )
	{
		ERR_PRINT_STRING( "Error writing EXR image" );
		goto exit;
	}
	retcode = AAA_OK;

exit:
	if( img_flip_v )
		delete img_flip_v;
	if( retcode != AAA_OK )
		ERR_PRINT_STRING( "Can't write EXR file" );
	return  retcode;
}

AAA_ERR	read( c_img_2d * image, C_PCHAR_C filename )
{
	AAA_ERR	retcode = c_img_utils::ERR_CANT_READ;
	void*	pixels = nullptr;

	try
	{
//		Imf::RgbaInputFile file( filename );
		//IMATH_NAMESPACE::Box2i dw = file.dataWindow();
		Imf::InputFile file( filename );
		//Imf::Header				header = file.header();
		auto	dw				= file.header().dataWindow();
		auto const & channels	= file.header().channels();
		size_t	channel_nb		= 0;
		bool	b_fp32			= false;
		bool	b_has_red		= false;
		bool	b_has_green		= false;
		bool	b_has_blue		= false;
		bool	b_has_alpha		= false;
		bool	b_has_y			= false;
		bool	b_has_chroma	= false;

		o_str	channels_name[4];
		for( auto i = channels.begin(), ite = channels.end(); i != ite; ++i )
		{
			C_PCHAR name = "UNKNOWN";
			auto const & channel = i.channel();
			switch( channel.type )
			{
			case Imf::PixelType::UINT:
				name = "UINT32";
				b_fp32 = true;
				break;
			case Imf::PixelType::HALF:
				name = "HALF";
				break;
			case Imf::PixelType::FLOAT:
				name = "FLOAT";
				b_fp32 = true;
				break;
			}

			if( strcmp( i.name(), "R" ) == 0 )
				b_has_red		= true;
			if ( strcmp( i.name(), "G" ) == 0 )
				b_has_green		= true;
			if ( strcmp( i.name(), "B" ) == 0 )
				b_has_blue		= true;
			if ( strcmp( i.name(), "A" ) == 0 )
				b_has_alpha		= true;
			if ( strcmp( i.name(), "Y" ) == 0 )
				b_has_y			= true;
			if ( strcmp( i.name(), "C" ) == 0 )
				b_has_chroma	= true;

			if( channel_nb < 4 )
				channels_name[channel_nb] = i.name();

			GOOD_PRINT_STRING( "Channel %d %s is %s", channel_nb, i.name(), name );
			++channel_nb;
		}
		if( channel_nb > 4 )
			channel_nb /= 2;	// for multiview image

		auto	format = aaa::PIXEL_FORMAT::UNKNOWN;
		if( channel_nb == 4 )
		{
			if( b_has_red && b_has_green && b_has_blue && b_has_alpha )
			{
				channels_name[0] = "R";
				channels_name[1] = "G";
				channels_name[2] = "B";
				channels_name[3] = "A";
			}
			format = b_fp32 ? aaa::PIXEL_FORMAT::RGBA_32FP : aaa::PIXEL_FORMAT::RGBA_16FP;
		}
		else if ( channel_nb == 3 )
		{
			if ( b_has_red && b_has_green && b_has_blue )
			{
				channels_name[0] = "R";
				channels_name[1] = "G";
				channels_name[2] = "B";
			}
			else if ( b_has_y && b_has_chroma && b_has_alpha )
			{
				channels_name[0] = "Y";
				channels_name[1] = "C";
				channels_name[2] = "A";
			}
			format = b_fp32 ? aaa::PIXEL_FORMAT::RGB_32FP : aaa::PIXEL_FORMAT::RGB_16FP;
		}
		else if ( channel_nb == 2 )
		{
			if ( b_has_y && b_has_alpha )
			{
				channels_name[0] = "Y";
				channels_name[1] = "A";
			}
			else if ( b_has_y && b_has_chroma )
			{
				channels_name[0] = "Y";
				channels_name[1] = "C";
			}
			else if ( b_has_red && b_has_green )
			{
				channels_name[0] = "R";
				channels_name[1] = "G";
			}
			else if ( b_has_red && b_has_alpha )
			{
				channels_name[0] = "R";
				channels_name[1] = "A";
			}
			format = b_fp32 ? aaa::PIXEL_FORMAT::RG_32FP : aaa::PIXEL_FORMAT::RG_16FP;
		}
		else if ( channel_nb == 1 )
		{
			format = b_fp32 ? aaa::PIXEL_FORMAT::R_32FP : aaa::PIXEL_FORMAT::R_16FP;
		}

		if( format == aaa::PIXEL_FORMAT::UNKNOWN )
		{
			retcode = c_img_utils::ERR_CANT_READ_THIS_FORMAT;
			goto exit;
		}
		INT32 width		= dw.max.x - dw.min.x + 1;
		INT32 height	= dw.max.y - dw.min.y + 1;
		//INT32 dx		= dw.min.x;
		//INT32 dy		= dw.min.y;

		//img->set_size( width, height, format );
		//img->init_with_size( width, height, format, "OpenEXR image" );

		size_t	pitch_x = sizeof( FP16 ) * channel_nb;
		pixels = MALLOC_ALIGNED( c_img_base::compute_data_size( width, height, format ), 0 );
		if( IS_NULL( pixels ) )
		{
			ERR_PRINT_STRING( "No data in image in %s()", __FUNCTION__ );
			retcode = c_img_utils::ERR_DATA_NO;
			goto exit;
		}

		CHAR	*src_1, *src_2, *src_3, *src_4;
		auto	type = Imf::PixelType::HALF;
		if( b_fp32 )
		{
			pitch_x	= sizeof( FP32 ) * channel_nb;
			type	= Imf::PixelType::FLOAT;
			src_1	= reinterpret_cast<CHAR *>(reinterpret_cast<FP32 *>(pixels) + 0);
			src_2	= reinterpret_cast<CHAR *>(reinterpret_cast<FP32 *>(pixels) + 1);
			src_3	= reinterpret_cast<CHAR *>(reinterpret_cast<FP32 *>(pixels) + 2);
			src_4	= reinterpret_cast<CHAR *>(reinterpret_cast<FP32 *>(pixels) + 3);
		}
		else
		{
			src_1 = reinterpret_cast<CHAR *>(reinterpret_cast<FP16 *>(pixels) + 0);
			src_2 = reinterpret_cast<CHAR *>(reinterpret_cast<FP16 *>(pixels) + 1);
			src_3 = reinterpret_cast<CHAR *>(reinterpret_cast<FP16 *>(pixels) + 2);
			src_4 = reinterpret_cast<CHAR *>(reinterpret_cast<FP16 *>(pixels) + 3);
		}
		size_t	pitch_y = c_img_base::compute_byte_pitch( width, format );

		Imf::FrameBuffer frameBuffer;
		frameBuffer.insert( channels_name[0].get(),	// name
							Imf::Slice( type,		// type
							src_1,					// base
							pitch_x,				// xStride
							pitch_y,				// yStride
							1, 1,					// x/y sampling
							0.0 ) );				// fillValue
		if( channel_nb >= 2 )
		{
			frameBuffer.insert( channels_name[1].get(),	// name
								Imf::Slice( type,		// type
								src_2,					// base
								pitch_x,				// xStride
								pitch_y,				// yStride
								1, 1,					// x/y sampling
								0.0 ) );				// fillValue
			if( channel_nb >= 3 )
			{
				frameBuffer.insert( channels_name[2].get(),	// name
									Imf::Slice( type,		// type
									src_3,					// base
									pitch_x,				// xStride
									pitch_y ,				// yStride
									1, 1,					// x/y sampling
									0.0 ) );				// fillValue
				if( channel_nb >= 4 )
				{
					frameBuffer.insert( channels_name[3].get(),	// name
										Imf::Slice( type,		// type
										src_4,					// base
										pitch_x,				// xStride
										pitch_y,				// yStride
										1, 1,					// x/y sampling
										1.0 ) );				// fillValue
				}
			}
		}

		file.setFrameBuffer( frameBuffer );
		file.readPixels( dw.min.y, dw.max.y );

		// Flip image from EXR
		image->copy_from_src( pixels, static_cast<INT32>(pitch_y), format, format, width, height, true );
		//for ( size_t i = 0; i <= dw.max.y; ++i )
		//{
		//	FP16* pix = (FP16*)pixel + ( dw.max.y - i ) * width * channel_nb;
		//	Imf::FrameBuffer frameBuffer;
		//	frameBuffer.insert( channels_name[0].get(),	// name
		//		Imf::Slice( type,		// type
		//		(char *)(pix),			// base
		//		pitch_x,				// xStride
		//		pitch_y,				// yStride
		//		1, 1,					// x/y sampling
		//		0.0 ) );				// fillValue
		//	if ( channel_nb >= 2 )
		//	{
		//		frameBuffer.insert( channels_name[1].get(),				// name
		//			Imf::Slice( type,					// type
		//			(char *)((FP16*)pix + 1),	// base
		//			pitch_x,							// xStride
		//			pitch_y,							// yStride
		//			1, 1,								// x/y sampling
		//			0.0 ) );							// fillValue
		//		if ( channel_nb >= 3 )
		//		{
		//			frameBuffer.insert( channels_name[2].get(),				// name
		//				Imf::Slice( type,					// type
		//				(char *)((FP16*)pix + 2),	// base
		//				pitch_x,							// xStride
		//				pitch_y,							// yStride
		//				1, 1,								// x/y sampling
		//				0.0 ) );							// fillValue
		//			if ( channel_nb >= 4 )
		//			{
		//				frameBuffer.insert( channels_name[3].get(),				// name
		//					Imf::Slice( type,					// type
		//					(char *)((FP16*)pix + 3),	// base
		//					pitch_x,							// xStride
		//					pitch_y,							// yStride
		//					1, 1,								// x/y sampling
		//					1.0 ) );							// fillValue
		//			}
		//		}
		//	}
		//	file.setFrameBuffer( frameBuffer );
		//	file.readPixels( i );
		//}
	
	}
	catch( ... )
	{
		ERR_PRINT_STRING( "Error reading EXR image" );
		goto exit;
	}
	retcode = AAA_OK;

exit:
	IF_FREE_ALIGNED_AND_NULL( pixels );
	//img->set_empty();
	if( retcode != AAA_OK )
		ERR_PRINT_STRING( "Can't read EXR file" );
	return retcode;
}

}	//namespace openexr
}	//namespace img
}	//namespace aaa

#endif //AAA_LIB_USE_IMG_OPENEXR

