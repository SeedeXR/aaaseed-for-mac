#include "img_jpg.h"
#include "err.h"
#include "image/img.h"


void	img_jpg_print( c_img_2d* image)	{}

namespace
{
	bool b_on = false;
}

AAA_ERR	img_jpg_init()
{
	if ( !b_on )
	{
#if	0
//		if ( InitLib( 0, nullptr, 0) )
		if ( IM_Init( 0 ) )
			b_on = true;
		else
			BOX_ERR( "Can't open Imagery Lib" );
#endif
	}
	return	b_on ? AAA_OK : ERR_ANY ;
}


AAA_ERR	img_jpg_read( c_img_2d* image )
{

#if	0
AAA_ERR		retcode;
HANDLE		hDIBLoad;
INT32		lib_retcode;
HDC			hdc;
HBITMAP		hbitmap;
BITMAP		bitmap;

	if ( NOERR(img_jpg_init()) )
		{
/*		//	load the DIB from the file (Imagery version)
		hDIBLoad = ReadImage( (char *)image->get_filename() );
		if ( !hDIBLoad)
			{
			BOX_ERR( "Error attempting to read DIB" );
			return(false);
			}
*/

		lib_retcode = IM_Load( 100, image->get_filename());
		if ( lib_retcode )
			{
			CHAR	str[256];
			IM_GetMessage( lib_retcode, str, 256);	//todo check this
			//
			BOX_ERR( str );
			}
		else
			{
			hdc = CreateCompatibleDC( nullptr );
			if ( hdc)
				{
				image->type = IMG_TYPE_RGB;
				image->channel_nb = 4;
				image->size_x = IM_SizeX( 100);
				image->size_y = IM_SizeY( 100);
				image->alloc_data();

				if( image->data )
					{
					/*
					bitmap.bmType = 0;
					bitmap.bmWidth = image->size_x;
					bitmap.bmHeight = image->size_y;
					bitmap.bmPlanes = 1;
					bitmap.bmBitsPixel = image->channel_nb * 8;

					bitmap.bmWidthBytes = bitmap.bmWidth * bitmap.bmBitsPixel  / 8;
					bitmap.bmBits = image->data;

					hbitmap = CreateBitmapIndirect( &bitmap);
					*/

					hbitmap = CreateCompatibleBitmap( hdc, IM_SizeX( 100), IM_SizeY( 100) );
					if ( hbitmap)
						{
						if( SelectObject( hdc, hbitmap) )
							{
							RECT	a_rect;

							a_rect.left = 0;
							a_rect.top = 0;
							a_rect.right = IM_SizeX( 100);
							a_rect.bottom = IM_SizeY( 100);
							lib_retcode =  IM_PaintDC( hdc, 100, &a_rect, nullptr, nullptr, 0);
							if ( lib_retcode )
								{
								CHAR	str[256];
								IM_GetMessage(lib_retcode,str,256);
								//
								BOX_ERR( str );
								}
							else
								{
								//retcode = GetDIBits( hdc, hbitmap, 0, IM_SizeY( 100), image->data, &bitmapinfo, DIB_RGB_COLORS)



								img_alpha_force(image, 255);
								}
							}
						else
							{
							BOX_ERR( "Can't SelectObject" );
							}
						}
					else
						{
						BOX_ERR( "Can't create BITMAP" );
						}
					}
				}
			}
		}
#endif
/*
	hDIBInfo=GlobalFree(hDIBInfo);	//	Free old image
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIBLoad);
	if(DeviceBPP<=8&&lpbi->biBitCount>8){
		GlobalUnlock(hDIBLoad);
		hDIBInfo=ColorConvert(hDIBLoad,IMG_RGB_8|dither);	//	Color Convert to 8-bit
		if(!hDIBInfo)
			hDIBInfo=hDIBLoad;	//	Put new DIB in hDIBInfo if failed color convert
	}else{
		GlobalUnlock(hDIBLoad);
		hDIBInfo=hDIBLoad;	//	Put new DIB in hDIBInfo
	}

	bDIBLoaded = true;		//	there is a DIB loaded now

	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIBInfo);
	offBits=(WORD)(lpbi->biSize+lpbi->biClrUsed*sizeof(RGBQUAD));
	SetWindowText(hWnd, achFileName);
	Rectangle.left	 = 0;
	Rectangle.top	 = 0;
	Rectangle.right  = (WORD)lpbi->biWidth;
	Rectangle.bottom = (WORD)lpbi->biHeight;
	GlobalUnlock(hDIBInfo);

	// Compute the size of the window rectangle based on the given
	// client rectangle size and the window style, then size the
	// window.  Do not deal with possibility of more than one menu line.
	//
	AdjustWindowRect (&Rectangle, WS_OVERLAPPEDWINDOW, TRUE);
	SetWindowPos (hWnd, nullptr, 0, 0,
			  Rectangle.right  - Rectangle.left,
			  Rectangle.bottom - Rectangle.top,
			  SWP_NOMOVE | SWP_NOZORDER);

	GetClientRect(hWnd, &Rectangle);

	image->type = IMG_TYPE_RGB;
	//	ski^p magic number
	retcode =  myfread( image->file, buf, IMG_SGI_BUF );
	if ( ERR(retcode) )
		goto exit;
//    img_sgi.image = image;
	
	//	storage format
	switch ( buf[2] )
		{
		case 0:
			img_sgi.b_rle = false;
			break;
		case 1:
			img_sgi.b_rle = true;
			break;
		default:
			ERR_PRINT_STRING( "%s has wrong storage number", image->get_filename() );
			goto exit;
		}

	//	Bytes per Pixel Channel
	switch ( buf[3] )
		{
		case 1:
		case 2:
			image->bits_per_component = buf[3]*8;
			break;
		default:
			ERR_PRINT_STRING( "%s has wrong Bytes per Pixel Channel"  );
			goto exit;
		}
	
	//	Dimension
	i16 = GET_INT16( &buf[4]);
	switch ( i16)
		{
		case 2:
		case 3:
			break;
		default:
			ERR_PRINT_STRING( "%s has a dimension not handled by this program", image->get_filename() );
			break;
		}

	//	X Y and number of Channel
	image->size_x = GET_INT16( &buf[6]);
	image->size_y = GET_INT16( &buf[8]);
	image->channel_nb = GET_INT16( &buf[10]);
	image->byte_per_pixel = image->channel_nb;	//todo use the byte per component
   
	//	max and min for pixvalue
	img_sgi.pixmin = GET_INT32( &buf[12]);
	img_sgi.pixmax = GET_INT32( &buf[16]);
	
	//	colormap
	img_sgi.colormap = GET_INT32( &buf[104]);
	
	//	make the raw reader more generic    
	image->scan_line_aligment = 1;

	c_file::FSEEK_SET( image->file, IMG_SGI_HEADER_LEN );

#if	AAA_DEBUG()
	img_sgi_print(&img_sgi);
#endif
	
	if (img_sgi.rle )
		{
		ERR_PRINT_STRING( "%s is coded using RLE : not implemented yet.", image->get_filename() );
		return	ERR_IMG_CANT_READ_THIS_FORMAT;
		}
	else
		{
		i64 = image->compute_data_size();
		image->alloc_data( i32 );
		if ( image->data )
			{
			retcode =  myfread( image->file, image->data, i32);
			if ( ERR(retcode) )
				goto exit;
			img_merge_channel_to_rgba( image);
			}
		}
*/
	return AAA_OK;
/*
exit:
	return ERR_IMG_CANT_READ;
*/
}

AAA_ERR	img_jpg_write( c_img_2d* image )
{
	BOX_ERR( "Save as JPG non implemented yet !!!" );
	return c_img_utils::ERR_CANT_WRITE;
}
