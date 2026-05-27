
#include "img_utils.h" 
#if AAA_LIB_USE_IMG_FREEIMAGE()
#	include "img_freeimage.h"
#endif
#if AAA_LIB_USE_IMG_GFLSDK()
#	include "img_xnview.h"
#endif
#include "Commdlg.h"
#include "obj_ui/print_master.h"
#include "file/aaa_dir.h" 
#include "Winspool.h"

#if __has_include( <filesystem> )
#	include <filesystem>
#endif



c_img_utils::FILE_TYPE	c_img_utils::save_format_ui;
REAL					c_img_utils::save_compression_ui;
bool					c_img_utils::b_save_compo_as_white_with_alpha_ui;
bool					c_img_utils::b_save_as_tga_grey_ui;
bool					c_img_utils::b_lib_print_format_trig_ui;

C_PCHAR_C		c_img_utils::str_file_type	[INT32(FILE_TYPE::MAX_NB)] =
{
	ext_tga,
	ext_png,
	ext_jpg,
	ext_tif,
	ext_exr,
	ext_yuv,
	ext_cpp,
	ext_dds,
	ext_aaatc,
	ext_sgi,
	"default"
};


C_PCHAR_C		c_img_utils::str_file_type_3d	[INT32(FILE_TYPE_3D::MAX_NB)] =
{
	"vtk",
	"default"
};

C_PCHAR_C		c_img_utils::str_lib_name		[INT32(LIBRARY	::MAX_NB)] =
{
	"GflSDK",
	"FreeImage",
	//	"Image Magick",
};

c_img_utils::LIBRARY	c_img_utils::s_lib = c_img_utils::LIBRARY::GFLSDK;

namespace
{
#if AAA_LIB_USE_IMG_FREEIMAGE()
	bool	b_freeimage_ok	= false;
#endif
#if AAA_LIB_USE_IMG_GFLSDK()
	bool	b_gflsdk_ok		= false;
#endif
}

void	img_libs_init()
{
	DBG_PRINT_STRING( "%s() Begin", __FUNCTION__ );
#if	AAA_LIB_USE_IMG_FREEIMAGE()
	b_freeimage_ok = aaa::img::freeimage::init() == AAA_OK;
#endif
#if	AAA_LIB_USE_IMG_GFLSDK()
	b_gflsdk_ok = aaa::img::gflsdk::init() == AAA_OK;
#endif
#if	AAA_LIB_USE_IMG_OPENEXR()
	aaa::img::openexr::init();
#endif
//	img_openexr_init() == AAA_OK;
	DBG_PRINT_STRING( "%s() Done", __FUNCTION__ );
}

void	img_libs_deinit()
{
#if	AAA_LIB_USE_IMG_FREEIMAGE()
	aaa::img::freeimage::deinit();
#endif
#if	AAA_LIB_USE_IMG_GFLSDK()
	aaa::img::gflsdk::deinit();
#endif
#if	AAA_LIB_USE_IMG_OPENEXR()
	aaa::img::openexr::deinit();
#endif
}

void	img_lib_error_mess()
{
	if ( c_img_utils::s_lib >= c_img_utils::LIBRARY::GFLSDK && c_img_utils::s_lib < c_img_utils::LIBRARY::MAX_NB )
		ERR_PRINT_STRING( "Image Library %s not used in this AAASeed version", c_img_utils::str_lib_name[ (INT32)c_img_utils::s_lib ] );
	else
		ERR_PRINT_STRING( "image Library %d not defined in this AAASeed version", (INT32)c_img_utils::s_lib );
}

void	img_lib_print_formats()
{
	switch( c_img_utils::s_lib )
	{
#if	AAA_LIB_USE_IMG_FREEIMAGE()
	case c_img_utils::LIBRARY::FREEIMAGE:
		aaa::img::freeimage::print_format();
		break;
#endif
#if	AAA_LIB_USE_IMG_GFLSDK()
	case c_img_utils::LIBRARY::GFLSDK:
		aaa::img::gflsdk::print_format( );
		break;
#endif
	default:
		img_lib_error_mess();
		break;
	}
}

AAA_ERR	write_with_lib( c_img_2d* img, o_str CONST & filename, c_img_utils::FILE_TYPE save_format, c_img_utils::LIBRARY s_lib )
{
	AAA_ERR	retcode = ERR_ANY;
	o_str& u8filename = o_str::push_name(filename);

		if( c_file::b_use_filesystem )
		{
			std::filesystem::path p = std::filesystem::path( filename.get() );	// path is C++20 was u8path
			u8filename.set( p.string().c_str() );
		}

		switch( s_lib )
		{
#if	AAA_LIB_USE_IMG_FREEIMAGE()
		case c_img_utils::LIBRARY::FREEIMAGE:
			if( b_freeimage_ok )
				retcode = aaa::img::freeimage::write( img, u8filename.get(), save_format );
			break;
#endif
#if	AAA_LIB_USE_IMG_GFLSDK()
		case c_img_utils::LIBRARY::GFLSDK:
			if( b_gflsdk_ok )
				retcode = aaa::img::gflsdk::write( img, u8filename.get(), save_format );
			break;
#endif
		default:
			img_lib_error_mess();
			break;
		}

	o_str::pop_name();
	return retcode;
}

////////
////////
//todo check if these lib are thread safe
AAA_ERR	img_read_with_lib( c_img_2d* img, o_str CONST & filename, bool b_load_data )
{
	AAA_ERR		retcode = ERR_ANY;

	o_str& u8filename = o_str::push_name(filename);
		if( c_file::b_use_filesystem )
		{
			std::filesystem::path p = std::filesystem::path( filename.get() );	// path is C++20 was u8path
			u8filename.set( p.string().c_str() );
		}

		switch( c_img_utils::s_lib )
		{
#if	AAA_LIB_USE_IMG_FREEIMAGE()
		case c_img_utils::LIBRARY::FREEIMAGE:
			if( b_freeimage_ok )
				retcode = aaa::img::freeimage::read( img, u8filename.get(), b_load_data );
			break;
#endif
#if	AAA_LIB_USE_IMG_GFLSDK()
		case c_img_utils::LIBRARY::GFLSDK:
			if( b_gflsdk_ok )
			{
				if( b_load_data )
					retcode = aaa::img::gflsdk::read( img, u8filename.get() );
				else
					retcode = aaa::img::gflsdk::read_info( img, u8filename.get() );				
			}
			break;
#endif
		default:
			img_lib_error_mess();
			break;
		}

	o_str::pop_name();
	return retcode;
}

AAA_ERR	c_img_2d::write_cpp( o_str CONST & filename )
{
	if( !check_data_valid( __FUNCTION__ ) )
		return c_img_utils::ERR_DATA_NO;
	auto file = c_file::FOPEN( filename, "wt" );
	if( IS_NULL( file ) )
		return c_img_utils::ERR_CANT_READ;

	INT32	nb_x	= get_size_x();
	INT32	nb_y	= get_size_y();
	INT32	ch_nb	= get_channel_nb();
	UINT8*	pt		= get_data_uint8();

	fprintf( file, "#define\tIMG_X\t%d\n", nb_x );
	fprintf( file, "#define\tIMG_Y\t%d\n", nb_y );
	fprintf( file, "#define\tIMG_CHANNEL\t%d\n", ch_nb );

	fprintf( file, "INT8 image[IMG_X*IMG_Y*IMG_CHANNEL] =\n" );
	fprintf( file, "\t{" );

	for( INT32 iy = 0; iy < nb_y; ++iy )
	{
		fprintf( file, "\n\t//line %d", iy );
		pt = get_data_uint8() + get_byte_offset( 0, iy );
		//todo deal with alignment too here
		for( INT32 ix = 0; ix < nb_x; ++ix )
		{
			if( ( ix & 0x0f ) == 0 )
				fprintf( file, "\n\t" );
			switch( ch_nb )
			{
			case 4:
				fprintf( file, "0x%02x,0x%02x,0x%02x,0x%02x,", ((INT32)*pt) & 0xff, ((INT32)*(pt+1)) & 0xff, ((INT32)*(pt+2)) & 0xff, ((INT32)*(pt+3)) & 0xff );
				pt += 4;
				break;		
			case 3:
				fprintf( file, "0x%02x,0x%02x,0x%02x,0xff",    ((INT32)*pt) & 0xff, ((INT32)*(pt+1)) & 0xff, ((INT32)*(pt+2)) & 0xff );
				pt += 3;
				break;
			case 2:
				fprintf( file, "0x%02x,0x%02x,0x00,0xff",		((INT32)*pt) & 0xff, ((INT32)*(pt+1)) & 0xff );
				pt += 3;
				break;
			case 1:
				{
					INT32 v = (INT32)*pt++;
					fprintf( file, "0x%02x,0x%02x,0x%02x,0xff",		v, v, v );
				}
				break;
			default:
				ERR_PRINT_STRING( "%s() not implemented with this channel nb %d", __FUNCTION__, ch_nb );
				print_info();
				break;
			}
		}
	}
	fprintf( file, "\n\t};\n\n" );
	c_file::FCLOSE( file );
	return AAA_OK;
}


AAA_ERR		c_img_2d::write( o_str CONST & filename, c_img_utils::FILE_TYPE file_type_asked )
{
	//if( str_is_equal_nocase( license_get_user(), "Miguel" ) )
	//{
	//	ERR_PRINT_STRING( "Rejected : wrong privileges, a topic for Maa." );
	//	return ERR_ANY;
	//}
	if( !check_data_valid( __FUNCTION__ ) )
	{
		ERR_PRINT_STRING( "IMG : Can't save because no data : %s", filename.get() );
		return c_img_utils::ERR_DATA_NO;
	}

	AAA_ERR	retcode = AAA_OK;
	o_str& locname = o_str::push_name();
	if( filename.is_empty() )
	{
		locname.set( "AAA_NoName" );
		ERR_PRINT_STRING( "%s() try to save image without a filename :", __FUNCTION__ );
		ERR_PRINT_STRING( "\tsaving using \"%s\"", locname.get() );
	}
	else
		locname.set( filename );

	//	get the file type to save to
	auto format = get_pixel_format();
	c_img_utils::FILE_TYPE	file_type;
	c_img_utils::LIBRARY	s_lib = c_img_utils::s_lib;

	if( file_type_asked == c_img_utils::FILE_TYPE::DEFAULT )
		file_type = c_img_utils::save_format_ui;
	else
		file_type = file_type_asked; 


	if( aaa::c_pixel_format::is_format_fp16( format ) || aaa::c_pixel_format::is_format_fp32( format ) )
		file_type = c_img_utils::FILE_TYPE::EXR;
	else if( aaa::PIXEL_FORMAT::YUY2 == format )
		file_type = c_img_utils::FILE_TYPE::YUV;
	else if( c_img_utils::save_format_ui == c_img_utils::FILE_TYPE::TGA && aaa::c_pixel_format::is_format_i16( format ) )
		file_type = c_img_utils::FILE_TYPE::PNG;
	else
	{
		switch( file_type )
		{
			case c_img_utils::FILE_TYPE::EXR:
			{
				auto src_type = get_data_type();
				switch( src_type )
				{
				case aaa::PIXEL_TYPE::FLOAT_16:
				case aaa::PIXEL_TYPE::FLOAT_32:	break;
				default:
					file_type = c_img_utils::FILE_TYPE::PNG;
					break;
				}
				break;
			}
		}
	}

 #if AAA_LIB_USE_IMG_GFLSDK()
//	if( aaa::c_pixel_format::is_format_i16( format ) )
//		s_lib = c_img_utils::LIBRARY::GFLSDK;
#endif
//ERR_PRINT_STRING( "IMG : %s, Can't save this type of data.", aaa::c_pixel_format::get_name(format) );
//				print_info();

	if( c_img_utils::b_save_compo_as_white_with_alpha_ui && has_alpha() )
		transform_compo_to_white_with_alpha( 0 );

	C_PCHAR_C	ext = c_img_utils::str_file_type[ static_cast<INT32>(file_type) ];
	locname.add_ext( ext );

	//	set the new file name
	if( file_type != c_img_utils::FILE_TYPE::CPP )
		set_filename( locname );

	o_str& dir_name = o_str::push_name();
		dir_name.set_dir_name( locname );
		if( !dir_name.is_empty() )
		{
			retcode = c_dir::make( dir_name );
			if( ERR( retcode ) )
				debug_break( "%s() can't create dir %s", __FUNCTION__, dir_name.get() );
		}
	o_str::pop_name();
	if( ERR( retcode ) )
		goto exit;

	switch( file_type )
	{
	case c_img_utils::FILE_TYPE::TGA:
		retcode = write_tga( locname, c_img_utils::save_compression_ui>.5, (aaa::c_pixel_format::get_channel_nb(format)==1) || c_img_utils::b_save_as_tga_grey_ui );
		break;
	case c_img_utils::FILE_TYPE::EXR:
#if AAA_LIB_USE_IMG_OPENEXR()
											retcode = aaa::img::openexr::write(	this,	locname.get() );				break;
#endif
	case c_img_utils::FILE_TYPE::JPG:
	case c_img_utils::FILE_TYPE::TIF:
	case c_img_utils::FILE_TYPE::PNG:		retcode = write_with_lib(	this,	locname,	file_type, s_lib );	break;
	case c_img_utils::FILE_TYPE::YUV:		retcode = write_yuv(				locname );						break;
	case c_img_utils::FILE_TYPE::CPP:		retcode = write_cpp(				locname );						break;

	default:
		ERR_PRINT_STRING( "IMG : Can't save this type of file : %s", locname.get() );
		print_info();
		retcode = c_img_utils::ERR_FILE_TYPE;
		break;
	}
	if( ERR( retcode ) ) // || ferror( file ) )
		goto exit;
	//	close the file
	set_changed();
	//c_file::FCLOSE( file );
	//file = nullptr;
	//	time_modification = c_file::get_mdate( file );
	c_file::push_vfile();
		set_time_modification( c_file::get_mdate( locname ) );
	c_file::pop_vfile();
	o_str::pop_name();

	return  AAA_OK;
exit:
	//if ( IS_NULL( file ) )
	//{
	//	ERR_PRINT_STRING( "IMG write : Can't open %s", locname.get() );
	//}
	//else
	{
		ERR_PRINT_STRING( "%s() : Can't write %s ", __FUNCTION__, locname.get() );
		//c_file::FCLOSE( file );
		//	_file = nullptr;
	}
	o_str::pop_name();
	return  c_img_utils::ERR_CANT_WRITE;
}

// ============================================================================
// print snapshot on default printer (if one)
// mode 0:just print 1:Show PrintDialog 2:ShowSetupDialog

//todo this don't print (Maa 2017 May)
AAA_ERR	img_print( c_img_2d* img ) // SC0804
{
	UINT8* src_start = img->get_data_valid_rgb_uint8( __FUNCTION__ );
	if ( !src_start )
		return c_img_utils::ERR_DATA_NO;

	CHAR		tampon[MAX_PATH];
	AAA_ERR		retcode	= ERR_ANY;
	ULONG		size	= MAX_PATH;
	void*		pImage	= nullptr;
	HDC			pdc		= nullptr;	// printer hdc
	PRINTDLGA	prd		= { 0 };
	HANDLE		bm		= nullptr;
	HDC			hMemDC	= nullptr;
	LPBITMAPINFO pBmi	= nullptr;

	//ZeroMemory(&prd, sizeof(prd) ); // pour le free final

	// === print dialog === Appelle une Common Dialog Box d'impression.

	if ( g_print_master->get_dialog_mode() == 1 || g_print_master->get_dialog_mode() == 2 )  // Show PrintDialogBox
	{
		prd.lStructSize			= sizeof( PRINTDLG );
		prd.hDevMode			= nullptr;
		prd.hDevNames			= nullptr;
		prd.Flags				= PD_RETURNDC;		// we will get the DC after PrintDlg() call
		if ( g_print_master->get_dialog_mode() == 2 )
			prd.Flags			|= PD_PRINTSETUP;		// ajout flag mode setup
		//prd.Flags				= PD_RETURNDC|PD_PRINTSETUP;		// we will get the DC after PrintDlg() call
		//prd.hwndOwner			= hwnd;
		prd.hwndOwner			= nullptr;	// ou trouver le hwnd d'AAASeed ?
		prd.hDC					= nullptr;
		prd.nFromPage			= 1;
		prd.nToPage				= 1;
		prd.nMinPage			= 0;
		prd.nMaxPage			= 0;
		prd.nCopies				= 1;
		prd.hInstance			= nullptr;
		prd.lCustData			= 0L;
		prd.lpfnPrintHook		= nullptr;
		prd.lpfnSetupHook		= nullptr;
		prd.lpPrintTemplateName = nullptr;
		prd.lpSetupTemplateName = nullptr;
		prd.hPrintTemplate		= nullptr;
		prd.hSetupTemplate		= nullptr;

		if ( PrintDlgA( &prd ) == 0 ) return ERR_ANY; // cancel
		pdc = prd.hDC;
	}
	// --- recuperation de l'imprimante par defaut
	if ( IS_NULL( pdc ) )
	{
		::GetDefaultPrinterA( tampon, &size );
		pdc = ::CreateDCA( 0, tampon, 0, 0 ); // --- Cr�er un HDC pour cette imprimante:
	}
	// --- Afficher un message d'erreur quand il n'y a pas d'imprimante par d�faut:
	if ( !pdc )
	{
		ERR_PRINT_STRING( "%s() : No Default Printer Found", __FUNCTION__ );
		goto exit;
	}

	// === Formatage de l'image pour impression:

	// --- Allouer la m�moire n�cessaire pour BITMAPINFO:
	pBmi = (LPBITMAPINFO)LocalAlloc( GMEM_FIXED, sizeof( BITMAPINFO ) );
	if ( !pBmi )	{ goto exit; }

	// --- Initialiser les membres de BITMAPINFO:
	pBmi->bmiHeader.biSize			= sizeof( BITMAPINFOHEADER );
	pBmi->bmiHeader.biWidth			= img->get_size_x();
	pBmi->bmiHeader.biHeight		= img->get_size_y();
	pBmi->bmiHeader.biPlanes		= 1;
	pBmi->bmiHeader.biBitCount		= 24;
	pBmi->bmiHeader.biCompression	= BI_RGB;
	pBmi->bmiHeader.biSizeImage		= 0;
	pBmi->bmiHeader.biXPelsPerMeter = 0;
	pBmi->bmiHeader.biYPelsPerMeter = 0;
	pBmi->bmiHeader.biClrUsed		= 0;
	pBmi->bmiHeader.biClrImportant	= 0;

	hMemDC = CreateCompatibleDC( 0 );
	if ( !hMemDC )
		goto exit;

	bm = CreateDIBSection( hMemDC, pBmi, DIB_RGB_COLORS, (void **)&pImage, nullptr, 0 );
	if ( !bm )
		goto exit;
	if ( !pImage )
		goto exit;

	// --- injecter l'image dans le buffer

	// BGRA --> RGB
	//todotex check and extend to all modes
	//todo use c_pixel_format::get_format_bgr_flipped( img->get_pixel_format() );
	aaa::PIXEL_FORMAT	pixel_format; 
	switch( img->get_pixel_format() )
	{
	case aaa::PIXEL_FORMAT::R_8:	pixel_format = aaa::PIXEL_FORMAT::R_8;		break;
	case aaa::PIXEL_FORMAT::RGB_8:	pixel_format = aaa::PIXEL_FORMAT::BGR_8;	break;
	case aaa::PIXEL_FORMAT::RGBA_8:	pixel_format = aaa::PIXEL_FORMAT::BGRA_8;	break;
	case aaa::PIXEL_FORMAT::BGR_8:	pixel_format = aaa::PIXEL_FORMAT::RGB_8;	break;
	case aaa::PIXEL_FORMAT::BGRA_8:	pixel_format = aaa::PIXEL_FORMAT::RGBA_8;	break;
	default:
		ERR_PRINT_STRING( "%s() don't know how to convert to BGR" );
		goto exit;
		//break;
	}

	{	
		//create an img object, attach the bitmap, and convert
		st_img_conv	options( img->get_size_x(), img->get_size_y() );
		c_img_2d*	img_bgr = c_img_2d::create( __FUNCTION__ );
		if( img_bgr )
		{
			img_bgr->init_from_mem( options.sx, options.sy, aaa::PIXEL_FORMAT::RGB_8, reinterpret_cast<UINT8*>(pImage), pBmi->bmiHeader.biSizeImage );
			//img_bgr.copy_src_to_rgb8( src_start, img->get_pitch(), pixel_format, options, "Print converted to BGR" );
			options.signature = "Print converted to BGR";
			options.src_pixel_format = pixel_format;
			img_bgr->copy_from_src( src_start, img->get_byte_pitch(), options );
			delete img_bgr;
		}
	}

	{
		// --- Obtenir la largeur et la hauteur de la zone imprimable en points:
		INT32 dxPage = ::GetDeviceCaps( pdc, HORZRES );
		INT32 dyPage = ::GetDeviceCaps( pdc, VERTRES );

		// --- D�clarer une structure DOCINFO et initialiser ses membres:
		DOCINFO di {};
		di.cbSize = sizeof( DOCINFO );
		di.lpszDocName = L"AAASeed";

		// --- Lancer le document et la page:
		::StartDocW( pdc, &di );
		::StartPage( pdc );

		// === Impression de l'image:

		// --- calcul du ratio � appliquer pour centrer l'image sur la page
		{
			REAL	ratio = MIN( (float)dxPage * img->get_size_x_over(), (float)dyPage * img->get_size_y_over() );  // recup le plus petit ratio
			REAL	size_x = img->get_size_x() * g_print_master->get_scale_x() * ratio;
			REAL	size_y = img->get_size_y() * g_print_master->get_scale_y() * ratio;
			//   ratio = MIN( ratio, ratio * g_print_master->get_scale() ); // force scale [0-1] => pas de debordement de l'image

			{
				REAL decX = (dxPage - size_x) * REAL(.5); // dec calcule pour centrer l'image
				REAL decY = (dyPage - size_y) * REAL(.5);
	
				decX *= REAL(1) + g_print_master->get_justify_x();
				decY *= REAL(1) + g_print_master->get_justify_y();

				// --- Dessiner notre bitmap sur le papier:

				::StretchDIBits( pdc, I_FLOOR(decX),I_FLOOR(decY), I_FLOOR(size_x),I_FLOOR(size_y), 0,0, img->get_size_x(), img->get_size_y(), pImage, pBmi, DIB_RGB_COLORS, SRCCOPY );
			}
		}
	}

	// --- Terminer la page et le document:
	::EndPage( pdc );
	::EndDoc( pdc );

	retcode = AAA_OK;
exit:
	// --- Lib�ration M�moire.

	::DeleteDC( pdc );			// --- release printer DC

	// --- release blocs allocated by PrintDlg(&prd)
	if( prd.hDevMode )
		::GlobalFree( prd.hDevMode );
	if( prd.hDevNames )
		::GlobalFree( prd.hDevNames );

	//todo because of exit and init check value here before calling and init better
	::DeleteObject( bm );		// --- release bitmap
	::DeleteDC( hMemDC );		// --- release memDC 
	::LocalFree( pBmi );		// --- release BITMAPINFO: // and so pImage

	return retcode;
}

void	c_img_utils::update()
{
	if( b_lib_print_format_trig_ui )
	{
		b_lib_print_format_trig_ui = false;
		img_lib_print_formats();
	}
}


c_img_utils::FILE_TYPE c_img_utils::get_save_type_from_ext( C_PCHAR_C ext )
{
	FILE_TYPE type;
	if( ext )
	{
		if( str_is_equal_nocase( ext, ext_tga) )
			type = FILE_TYPE::TGA;
		else if ( str_is_equal_nocase( ext, ext_png ) )
			type = FILE_TYPE::PNG;
		else if ( str_is_equal_nocase( ext, ext_jpg ) )
			type = FILE_TYPE::JPG;
		else if ( str_is_equal_nocase( ext, ext_exr ) )
			type = FILE_TYPE::EXR;
		else if ( str_is_equal_nocase( ext, ext_dds ) )
			type = FILE_TYPE::DDS;
		else if ( str_is_equal_nocase( ext, ext_aaatc ) )
			type = FILE_TYPE::AAATC;
		else if ( str_is_equal_nocase( ext, ext_yuv ) )
			type = FILE_TYPE::YUV;
		else if ( str_is_equal_nocase( ext, ext_sgi ) || str_is_equal_nocase( ext, ext_rgb ) )
			type = FILE_TYPE::SGI;
		else if ( str_is_equal_nocase( ext, ext_tif ) || str_is_equal_nocase( ext, ext_tiff ) )
			type = FILE_TYPE::TIF;
		else if ( str_is_equal_nocase( ext, ext_cpp ) )
			type = FILE_TYPE::CPP;
		else
			type = FILE_TYPE::DEFAULT;
	}
	else
		type = FILE_TYPE::DEFAULT;
	return type;
}

