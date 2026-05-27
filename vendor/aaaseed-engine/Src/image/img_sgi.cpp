//#include "img_sgi.h"
#include "err.h"
#include "image/img.h"
#include "file/aaa_file.h"


//  should add a TARGET switch
//#define	GET_INT16(p)	( ((*(p)))<<8) + *((p)+1) )
//#define	GET_INT32(p)	( ((*(p)))<<24) + (*((p)+1)<<16) + (*((p)+2)<<8) + *((p)+3) )

#define	GET_INT16(p)	( ((UINT16)(*(p)&0xff)<<8) + (UINT16)(*((p)+1)&0xff) )
#define	GET_INT32(p)	( ((*(p)&0xff)<<24) + ((*((p)+1)&0xff)<<16) + ((*((p)+2)&0xff)<<8) + (*((p)+3)&0xff) )

typedef struct ST_IMG_SGI
{
//	c_img_2d	image;
	bool	b_rle;
	INT32	pixmin;
	INT32	pixmax;
	INT32	colormap;
}	ST_IMG_SGI;

CONST size_t	IMG_SGI_BUF			= 108;
CONST size_t	IMG_SGI_HEADER_LEN	= 512;

void	img_sgi_print( ST_IMG_SGI *img_sgi )
{
	if( img_sgi->b_rle )
		PRINT_STRING("\tRLE");
	else
		PRINT_STRING("\tVERBATIM");
	VERBOSE_PRINT_STRING(" PixMin/Max %ld/%ld, ColorMap %ld", img_sgi->pixmin, img_sgi->pixmax, img_sgi->colormap );
}

AAA_ERR	c_img_2d::read_sgi( FILE* file, o_str CONST & filename )
{
	ST_IMG_SGI	img_sgi;
	INT32		retcode;
	UINT8		buf[IMG_SGI_BUF];
	INT16		i16;
	
	//_type = TYPE_RGB;
	//	skip magic number
	retcode = c_file::FREAD_CHECK( buf, IMG_SGI_BUF, file );
	if( ERR(retcode) )
		goto exit;
//	img_sgi.image = image;
	
	//	storage format
	switch( buf[2] )
	{
	case 0:		img_sgi.b_rle = false;	break;
	case 1:		img_sgi.b_rle = true;	break;
	default:	ERR_PRINT_STRING( "%s has wrong storage number", filename.get() );
				goto exit;
	}

	//	Bytes per Pixel Channel
	switch ( buf[3] )
	{
	case 1:
	case 2:
		//_bits_per_component = buf[3]*8;
		break;
	default:
		ERR_PRINT_STRING( "%s has wrong Bytes per Pixel Channel", filename.get() );
		goto exit;
	}
	
	//	Dimension
	i16 = GET_INT16( &buf[4] );
	switch ( i16)
	{
	case 2:
	case 3:
		break;
	default:
		ERR_PRINT_STRING( "%s has a dimension not handled by this program", filename.get() );
		break;
	}

	//	X Y and number of Channel
	{
		INT32 channel_nb = GET_INT16( &buf[10] );	//todo use the byte per component
		//hack
		aaa::PIXEL_FORMAT	format = aaa::c_pixel_format::make_format_from_channel_type(channel_nb);
		set_size_format( GET_INT16(&buf[6]), GET_INT16(&buf[8]), format );
		//set_size( GET_INT16(&buf[6]), GET_INT16(&buf[8]), GET_INT16(&buf[10]) );

		//	max and min for pixvalue
		img_sgi.pixmin = GET_INT32( &buf[12]);
		img_sgi.pixmax = GET_INT32( &buf[16]);

		//	colormap
		img_sgi.colormap = GET_INT32( &buf[104]);

		c_file::FSEEK_SET( file, IMG_SGI_HEADER_LEN );

#if	AAA_DEBUG()
		img_sgi_print( &img_sgi );
#endif
	
		if( img_sgi.b_rle )
		{
			ERR_PRINT_STRING( "%s is coded using RLE : not implemented yet.", get_filename() );
			return	c_img_utils::ERR_CANT_READ_THIS_FORMAT;
		}
		else
		{
			UINT64 size = compute_data_size();
			c_img_base::alloc_data( size, format, __FUNCTION__ );
			if( check_data_valid( __FUNCTION__ ) )
			{
				retcode = c_file::FREAD_CHECK( get_data_uint8(), size, file );	//todo this is ok only if alignment if file is like opengl here 
				if( ERR(retcode) )
					goto exit;
				merge_channel_to_rgba();
			}
		}
	}
	return AAA_OK;

exit:
	return c_img_utils::ERR_CANT_READ;
}

//AAA_ERR	img_sgi_write(c_img_2d* image)
//{
//	ERR_PRINT_STRING( "Save as SGI non implemented yet !!!");
//	return c_img_2d::ERR_CANT_WRITE;
//}
