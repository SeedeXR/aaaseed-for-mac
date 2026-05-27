#include "image/img.h"
#include "file/aaa_file.h"


#define	IMG_422_PAL_SIZE_X	720
#define	IMG_422_PAL_SIZE_Y	576
#define	IMG_422_NTSC_SIZE_X	720
#define	IMG_422_NTSC_SIZE_Y	486

//todo remove this hack or deal with yuv format
AAA_ERR	c_img_2d::read_yuv( FILE* file )
{
	//todo 2 channel not really handled (2013)
	AAA_ERR	retcode = init_with_size( IMG_422_PAL_SIZE_X, IMG_422_PAL_SIZE_Y, aaa::PIXEL_FORMAT::YUY2, __FUNCTION__ );
	if( NOERR(retcode) )
	{
		UINT8* dst = get_data_uint8();
		INT32 byte_per_line = get_byte_pitch();
		if( dst )
		{
			retcode = AAA_OK;
			dst += ( get_size_y() - 1 ) * byte_per_line;
			for( INT32 y = get_size_y(); y > 0; --y )
			{
				if( ERR( c_file::FREAD_CHECK( dst, byte_per_line, file ) ) )
					goto exit;
				dst -= byte_per_line;
			}
		}
	}
	return retcode;
exit:
	ERR_PRINT_STRING("IMG : Can't read YUV file");
	return c_img_utils::ERR_CANT_READ;
}

AAA_ERR	c_img_2d::write_yuv(  o_str CONST & filename )
{
	AAA_ERR	retcode = c_img_utils::ERR_CANT_WRITE;

	auto file = c_file::FOPEN( filename, "wb" );
	if ( IS_NULL( file ) )		return retcode;

	INT32	byte_per_line = get_byte_pitch();

	UINT8*	dst = get_data_uint8();
	if( dst )
	{
		retcode = AAA_OK;
		dst += ( get_size_y() - 1 ) * byte_per_line;
		for( INT32 y = get_size_y(); y > 0; --y )
		{
			if ( ERR( c_file::FWRITE_CHECK( dst, byte_per_line, file ) ) )
			{
				goto exit;
			}
			dst -= byte_per_line;
		}
	}
	c_file::FCLOSE( file );
	return retcode;
exit:

	return c_img_utils::ERR_CANT_WRITE;
}


#if 0
AAA_ERR	img_yuv_read(c_img_2d* image)
{
	UINT8	buf[IMG_422_PAL_WIDTH*2];
	INT32	x;
	INT32	l;
	UINT8	*dst;
	UINT8	*src;
	REAL	y;
	REAL	u;
	REAL	v;
	REAL	r;
	REAL	b;
	AAA_ERR	retcode = ERR_IMG_CANT_READ;
	INT32	bytes_per_line;


	image->size_x = IMG_422_PAL_WIDTH;
	image->size_y = IMG_422_PAL_HEIGHT;
	image->channel_nb = 2;
	image->type = IMG_TYPE_YUV;

	bytes_per_line = image->size_x * image->channel_nb;

	image->alloc_data( img_get_size_data(image));
	dst = image->data;
	if( dst)
		{
		retcode = AAA_OK;
		for( l=(image->size_y-1)*bytes_per_line; l>=0; l-=bytes_per_line)
			{
			dst = image->data + l;
			if ( myfread( image->file, buf, x=image->size_x*2) )
				{
				src = buf;
				/*
					Initialize YUV tables:

					  Y =  0.29900*R+0.58600*G+0.11400*B
					  U = -0.14740*R-0.28950*G+0.43690*B
					  V =  0.61500*R-0.51500*G-0.10000*B

					U and V, normally -0.5 through 0.5, are normalized to the range 0
					through MaxRGB.  Note that U = 0.493*(B-Y), V = 0.877*(R-Y).
				*/
				/*
					Initialize YUV tables:

					  R = Y          +1.13980*V
					  G = Y-0.39380*U-0.58050*V
					  B = Y+2.02790*U

					U and V, normally -0.5 through 0.5, must be normalized to the range 0
					through MaxRGB.
				*/
				for( x=image->size_x/2; x>0; --x )
					{
#if 0
					DBG_PRINT_STRING( "u %2x, y1 %2x, v %2x, y2 %2x",
						(UINT32) *(src+0),
						(UINT32) *(src+1),
						(UINT32) *(src+2),
						(UINT32) *(src+3)
						);
					DBG_PRINT_STRING( "u %ld, y1 %ld, v %ld, y2 %2ld",
						(INT32) *(src+0)-128,
						(INT32) *(src+1),
						(INT32) *(src+2)-128,
						(INT32) *(src+3)
						);
#endif
					u = (REAL) (*(src+0)-128.);
					v = (REAL) (*(src+2)-128.);
					y = (REAL) (*(src+1) );
					b = u *2.02790;
					r = v *1.13980;

					*dst++ = (INT8) (r+y);
					*dst++ = (INT8) (y -0.39380*u-0.58050*v);
					*dst++ = (INT8) (b+y);
					
					y = (REAL) (*(src+3) );

					*dst++ = (UINT8) (r+y);
					*dst++ = (UINT8) (y -0.39380*u-0.58050*v);
					*dst++ = (UINT8) (b+y);
					
/*
					u = (REAL) (*(src+0)-128.);
					v = (REAL) (*(src+2)-128.);
					y = (REAL) (*(src+1) );
					b = u * 1.4025245;
					r = v * 1.7730496;

					*dst++ = (INT8) (r+y);
					*dst++ = (INT8) ((y*.587 -r*.299 -b*.114) * 1.7035776);
					*dst++ = (INT8) (b+y);
					
					y = (REAL) (*(src+3) );

					*dst++ = (UINT8) (r+y);
					*dst++ = (UINT8) ((y*.587 -r*.299 -b*.114) * 1.7035776);
					*dst++ = (UINT8) (b+y);
*/
					src += 4;
					}
				}
			else
				{
				goto exit;
				}
			}
		}

	return retcode;
exit:
	image->data_dealloc();
	return ERR_IMG_CANT_READ;
}
#endif
