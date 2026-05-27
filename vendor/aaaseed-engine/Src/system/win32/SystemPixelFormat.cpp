
#include "SystemPixelFormat.h"
#include "gol/gol_os.h"
#include "aaa_mem.h"

namespace
{
	//infact this is not a defaut format but the default structure we use to define the format we want
	//todo call it format_asked ?
	system_pixel_format default_format; //<! Default pixel format
}

//todo call it get_format_asked ?
system_pixel_format& system_pixel_format::get_format_default( void )
{
	return default_format;
}

//=================================================================================================
system_pixel_format::system_pixel_format( void )

	// Members initialization
	: _options			( (Rgba | DirectRendering | DeprecatedFunctions) )
	, _plane			( 0 )
	, _depth_size		( -1 )
	, _accum_size		( -1 )
	, _stencil_size		( -1 )
	, _red_size			( -1 ) 
	, _green_size		( -1 )
	, _blue_size		( -1 ) 
	, _alpha_size		( -1 )
	, _samples_nb		( -1 )
	, _swap_interval	( -1 )
	, _version_major	( GOL::VERSION_MAJOR_REQUIRED )
	, _version_minor	( GOL::VERSION_MINOR_REQUIRED )
	, _b_debug			( false ) 
	, _profile			( CompatibilityProfile )
{}

//=================================================================================================
system_pixel_format::system_pixel_format( system_pixel_format CONST & src )
	// Members init
	: _options			( src._options			)
	, _plane			( src._plane			)
	, _depth_size		( src._depth_size		)
	, _accum_size		( src._accum_size		)
	, _stencil_size		( src._stencil_size		)
	, _red_size			( src._red_size			)
	, _green_size		( src._green_size		)
	, _blue_size		( src._blue_size		)
	, _alpha_size		( src._alpha_size		)
	, _samples_nb		( src._samples_nb		)
	, _swap_interval	( src._swap_interval	)
	, _version_major	( src._version_major	)
	, _version_minor	( src._version_minor	)
	, _b_debug			( src._b_debug			)
	, _profile			( src._profile			)
{
}

//=================================================================================================
system_pixel_format & system_pixel_format::operator=( system_pixel_format CONST & src )
{
	_options			= src._options			;
	_plane				= src._plane			;
	_depth_size			= src._depth_size		;
	_accum_size			= src._accum_size		;
	_stencil_size		= src._stencil_size		;
	_red_size			= src._red_size			;
	_green_size			= src._green_size		;
	_blue_size			= src._blue_size		;
	_alpha_size			= src._alpha_size		;
	_samples_nb			= src._samples_nb		;
	_swap_interval		= src._swap_interval	;
	_version_major		= src._version_major	;
	_version_minor		= src._version_minor	;
	_b_debug			= src._b_debug			;
	_profile			= src._profile			;

	return *this;
}

//=================================================================================================
system_pixel_format::~system_pixel_format( void )
{}

//=================================================================================================
void system_pixel_format::init( FormatOptions p_options, int32_t p_plane )
{
	FormatOptions newOpts = p_options;
	_options = system_pixel_format::get_format_default()._options;
	_options |= (newOpts & OPTION_SHIFT_MASK);
	_options &= ~(newOpts >> OPTION_SHIFT_NB);
	set_plane( p_plane );
}



//=================================================================================================
void system_pixel_format::set_from_pfd( PIXELFORMATDESCRIPTOR CONST & pfd )
{
	// Double buffering
	set_buffer_double( pfd.dwFlags & PFD_DOUBLEBUFFER);
	
	// Depth 
	set_depth( pfd.cDepthBits>0 );
	if( is_depth() )
		set_depth_buffer_size( pfd.cDepthBits );

	// RGBA 
	set_rgba( pfd.iPixelType == PFD_TYPE_RGBA );
	set_red_buffer_size( pfd.cRedBits );
	set_green_buffer_size( pfd.cGreenBits );
	set_blue_buffer_size( pfd.cBlueBits );
	set_alpha( pfd.cAlphaBits>0 );

	if( is_alpha() )
		set_apha_buffer_size( pfd.cAlphaBits );


	set_accum( pfd.cAccumBits>0 );
	if( is_accum() )
		set_accum_buffer_size( pfd.cAccumRedBits );

	set_stencil( pfd.cStencilBits>0 );
	if( is_stencil() )
		set_stencil_buffer_size( pfd.cStencilBits );

	set_stereo( (pfd.dwFlags & PFD_STEREO)!=0 );

	set_direct_rendering( (pfd.dwFlags & PFD_GENERIC_ACCELERATED) || !(pfd.dwFlags & PFD_GENERIC_FORMAT) );

	set_overlay( (pfd.bReserved & 0x0f) != 0 );
}

void system_pixel_format::convert_to_pfd( PIXELFORMATDESCRIPTOR& pfd ) CONST
{
	// Pixel format descriptor
	memset( &pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	// Draw flags
	pfd.dwFlags  = PFD_SUPPORT_OPENGL;
	//p->dwFlags |= PFD_DRAW_TO_BITMAP;
	pfd.dwFlags |= PFD_DRAW_TO_WINDOW;

	// Direct Rendering
	if( !is_direct_rendering() )
		pfd.dwFlags |= PFD_GENERIC_FORMAT;

	// Double Buffering
	if( is_buffer_double() )
		pfd.dwFlags |= PFD_DOUBLEBUFFER;

	// Stereo
	if( is_stereo() )
		pfd.dwFlags |= PFD_STEREO;

	// Depth
	if( is_depth() )
		pfd.cDepthBits = (get_depth_buffer_size() == -1 ? 32 : get_depth_buffer_size());
	else
		pfd.dwFlags |= PFD_DEPTH_DONTCARE;

	// RGBA
	if( is_rgba() ) 
	{
		pfd.iPixelType = PFD_TYPE_RGBA;

		if( get_red_buffer_size() != -1 )
			pfd.cRedBits = get_red_buffer_size();
		if( get_green_buffer_size() != -1 )
			pfd.cGreenBits = get_green_buffer_size();
		if( get_blue_buffer_size() != -1 )
			pfd.cBlueBits = get_blue_buffer_size();

		pfd.cColorBits = 32;
	} 
	else 
	{
		pfd.iPixelType = PFD_TYPE_COLORINDEX;
		pfd.cColorBits = 8;
	}

	// Alpha
	if( is_alpha() )
		pfd.cAlphaBits = get_alpha_buffer_size() == -1 ? 8 : get_alpha_buffer_size();

	// Accum buffer
	if( is_accum() ) 
	{
		BYTE value = (get_accum_buffer_size() == -1 ? 16 : get_accum_buffer_size());
		pfd.cAccumRedBits = pfd.cAccumGreenBits = pfd.cAccumBlueBits = pfd.cAccumAlphaBits = value;
	}

	// Stencil
	if( is_stencil() )
		pfd.cStencilBits = (get_stencil_buffer_size() == -1 ? 8 : get_stencil_buffer_size());

	// iLayerType is ignored, backward compatibility only
	pfd.iLayerType = PFD_MAIN_PLANE;
}



//=================================================================================================
void system_pixel_format::pfiToSystemPixelFormat( HDC hdc, int32_t pfi, system_pixel_format& fmt )
{
	// Attributes
	int32_t * iAttributes = (int32_t*)CALLOC(  14, sizeof(int32_t) );
	memset( iAttributes, 0, sizeof(int32_t)*14 );

	int32_t * iValues = (int32_t*)CALLOC( 14, sizeof(int32_t) );
	memset( iValues, 0, sizeof(int32_t)*14 );

	// Inc
	int32_t i = 0;

	bool has_sample_buffers = false; //GLExtensions::glExtensions() & GLExtensions::SampleBuffers;

	iAttributes[ i++ ] = WGL_DOUBLE_BUFFER_ARB;		// 0
	iAttributes[ i++ ] = WGL_DEPTH_BITS_ARB;		// 1
	iAttributes[ i++ ] = WGL_PIXEL_TYPE_ARB;		// 2
	iAttributes[ i++ ] = WGL_RED_BITS_ARB;			// 3
	iAttributes[ i++ ] = WGL_GREEN_BITS_ARB;		// 4
	iAttributes[ i++ ] = WGL_BLUE_BITS_ARB;			// 5
	iAttributes[ i++ ] = WGL_ALPHA_BITS_ARB;		// 6
	iAttributes[ i++ ] = WGL_ACCUM_BITS_ARB;		// 7
	iAttributes[ i++ ] = WGL_STENCIL_BITS_ARB;		// 8
	iAttributes[ i++ ] = WGL_STEREO_ARB;			// 9
	iAttributes[ i++ ] = WGL_ACCELERATION_ARB;		// 10
	iAttributes[ i++ ] = WGL_NUMBER_OVERLAYS_ARB;	// 11

	if( has_sample_buffers ) 
	{
		iAttributes[i++] = WGL_SAMPLE_BUFFERS_ARB;	// 12
		iAttributes[i++] = WGL_SAMPLES_ARB;			// 13
	}

	//  Prototype for wglGetPixelFormatAttribivARB()
	//typedef bool (WINAPI* PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC hdc,int iPixelFormat,int iLayerPlane,UINT nAttributes,const int *piAttributes,int *piValues);
	//PFNWGLGETPIXELFORMATATTRIBIVARBPROC cb_wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)wglGetProcAddress("wglGetPixelFormatAttribivARB");

	// It seems we have a hard time to retrieve "wglGetPixelFormatAttribivARB" callback !!!

	if( wglGetPixelFormatAttribivARB && wglGetPixelFormatAttribivARB( hdc, pfi, 0, i, iAttributes, iValues) )
	{
		fmt.set_buffer_double( iValues[0]!=0 );
		fmt.set_depth( iValues[1]!=0 );

		if( fmt.is_depth() )
			fmt.set_depth_buffer_size( iValues[1] );

		fmt.set_rgba(iValues[2] == WGL_TYPE_RGBA_ARB);
		fmt.set_red_buffer_size( iValues[3] );
		fmt.set_green_buffer_size( iValues[4] );
		fmt.set_blue_buffer_size( iValues[5] );
		fmt.set_alpha( iValues[6]!=0 );
		if( fmt.is_alpha() )
			fmt.set_apha_buffer_size( iValues[6] );

		fmt.set_accum( iValues[7]!=0 );
		if( fmt.is_accum() )
			fmt.set_accum_buffer_size( iValues[7] );

		fmt.set_stencil( iValues[8]!=0 );
		if( fmt.is_stencil() )
			fmt.set_stencil_buffer_size(iValues[8]);

		fmt.set_stereo( iValues[9]!=0 );

		fmt.set_direct_rendering( iValues[10] == WGL_FULL_ACCELERATION_ARB );

		fmt.set_overlay( iValues[11]!=0 );

		if( has_sample_buffers ) 
		{
			fmt.set_sample_buffers( iValues[12]!=0 );
			if( fmt.is_sample_buffers() )
				fmt.set_samples( iValues[13] );
		}
	}

	// Free memory
	FREE( iAttributes );
	FREE( iValues );
}



//=================================================================================================
bool system_pixel_format::is_opengl_overlays( void )
{
	// workaround for matrox driver:
	// make a cheap call to opengl to force loading of DLL
	//GOL::do_matrox_hack();

	static bool hasOl = false;


	// Check once by Pixel Format
	static bool checkDone = false;
	if( !checkDone ) 
	{
		checkDone = true;

		HDC display_dc = ::GetDC(0);
		
		int32_t pfiMax = ::DescribePixelFormat(display_dc, 0, 0, nullptr);
		
		PIXELFORMATDESCRIPTOR pfd;
		for( int32_t pfi = 1; pfi <= pfiMax; pfi++ ) 
		{
			::DescribePixelFormat( display_dc, pfi, sizeof(PIXELFORMATDESCRIPTOR), &pfd );

			if( (pfd.bReserved & 0x0f) && (pfd.dwFlags & PFD_SUPPORT_OPENGL) ) 
			{
				// format has overlays/underlays
				LAYERPLANEDESCRIPTOR lpd;
				::wglDescribeLayerPlane( display_dc, pfi, 1, sizeof(LAYERPLANEDESCRIPTOR), &lpd );

				if( lpd.dwFlags & LPD_SUPPORT_OPENGL ) 
				{
					hasOl = true;
					break;
				}
			}
		}
		::ReleaseDC( nullptr, display_dc );
	}

	return hasOl;
}



///////////////////////////////////////////////////////////////////////////////////////////////////
//		GET / SET
///////////////////////////////////////////////////////////////////////////////////////////////////


//=================================================================================================
void system_pixel_format::set_depth_buffer_size( int32_t p_size )
{
	if( p_size > 0 ) 
	{
		_depth_size = p_size;
		set_depth( (p_size>0) );
	}
}

//=================================================================================================
void system_pixel_format::set_red_buffer_size( int32_t p_size )
{
	if( p_size > 0 ) {
		_red_size = p_size;
	}
}

//=================================================================================================
void system_pixel_format::set_green_buffer_size( int32_t p_size )
{
	if( p_size > 0 ) {
		_green_size = p_size;
	}
}

//=================================================================================================
void system_pixel_format::set_blue_buffer_size( int32_t p_size )
{
	if( p_size > 0 ) {
		_blue_size = p_size;
	}
}

//=================================================================================================
void system_pixel_format::set_apha_buffer_size( int32_t p_size )
{
	if( p_size > 0 ) 
	{
		_alpha_size = p_size;
		set_alpha( (p_size>0) );
	}
	
}

//=================================================================================================
void system_pixel_format::set_accum_buffer_size( int32_t p_size )
{
	if( p_size > 0 )
	{
		_accum_size = p_size;
		set_accum( (p_size>0) );
	}
	
}

//=================================================================================================
void system_pixel_format::set_stencil_buffer_size( int32_t p_size)
{
	if( p_size > 0 )
	{
		_stencil_size = p_size;
		set_stencil( p_size > 0 );
	}
	
}

//=================================================================================================
FINLINE	void system_pixel_format::set_option( FormatOptions p_opt )
{
	FormatOptions options_on = p_opt & OPTION_SHIFT_MASK;
	if( options_on ) {
		_options |= options_on;
	}
	else {
	   _options &= (~(p_opt >> OPTION_SHIFT_NB)) & OPTION_SHIFT_MASK;
	}
}
FINLINE bool system_pixel_format::test_option( FormatOptions p_opt ) const
{
	bool b;
	FormatOptions options_on = p_opt & OPTION_SHIFT_MASK;
	if( options_on )
	   b = (_options & options_on) != 0;
	else
	   b = (_options & ((p_opt >> OPTION_SHIFT_NB) & OPTION_SHIFT_MASK) ) == 0;

	return b;
}


//=================================================================================================
void system_pixel_format::set_buffer_double( bool b_enable )
{
	if( b_enable != is_buffer_double() )
		set_option( b_enable ? DoubleBuffer : SingleBuffer );
}
void system_pixel_format::set_depth( bool b_enable )
{
	if( b_enable != is_depth() )
		set_option( b_enable ? DepthBuffer : NoDepthBuffer );
}
void system_pixel_format::set_rgba( bool b_enable )
{
	if( b_enable != is_rgba() )
		set_option( b_enable ? Rgba : ColorIndex );
}
void system_pixel_format::set_alpha( bool b_enable )
{
	if( b_enable != is_alpha() )
		set_option( b_enable ? AlphaChannel : NoAlphaChannel );
}
void system_pixel_format::set_accum( bool b_enable )
{
	if( b_enable != is_accum() )
		set_option( b_enable ? AccumBuffer : NoAccumBuffer );
}
void system_pixel_format::set_stencil( bool b_enable )
{
	if( b_enable != is_stencil() )
		set_option( b_enable ? StencilBuffer: NoStencilBuffer );
}
void system_pixel_format::set_stereo( bool b_enable )
{
	if( b_enable != is_stereo() )
		set_option( b_enable ? StereoBuffers : NoStereoBuffers );
}
void system_pixel_format::set_direct_rendering( bool b_enable )
{
	if( b_enable != is_direct_rendering() )
		set_option( b_enable ? DirectRendering : IndirectRendering );
}
void system_pixel_format::set_sample_buffers( bool b_enable )
{
	if( b_enable != is_sample_buffers() )
		set_option( b_enable ? SampleBuffers : NoSampleBuffers );
}
void system_pixel_format::set_overlay( bool b_enable )
{
	if( b_enable != is_opengl_overlays() )
		set_option( b_enable ? HasOverlay : NoOverlay );
}
void system_pixel_format::set_depreciated_function( bool b_enable )
{
	if( b_enable != is_opengl_overlays() )
		set_option( b_enable ? DeprecatedFunctions : NoDeprecatedFunctions );
}

//=================================================================================================
void system_pixel_format::set_samples( int32_t p_numSamples )
{
	if( p_numSamples > 0 && p_numSamples != _samples_nb ) 
	{
		_samples_nb = p_numSamples;
		set_sample_buffers( (p_numSamples > 0) );
	}
}

//=================================================================================================
void system_pixel_format::set_swap_interval( int32_t p_interval )
{
	_swap_interval = p_interval;
}

//=================================================================================================
void system_pixel_format::set_plane( int32_t p_plane )
{
	_plane = p_plane;
}

bool	system_pixel_format::is_buffer_double( void ) const			{	return test_option(DoubleBuffer);			}
bool	system_pixel_format::is_depth( void ) const					{	return test_option(DepthBuffer);			}
bool	system_pixel_format::is_rgba( void ) const					{	return test_option(Rgba);					}
bool	system_pixel_format::is_alpha( void ) const					{	return test_option(AlphaChannel);			}
bool	system_pixel_format::is_accum( void ) const					{	return test_option(AccumBuffer);			}
bool	system_pixel_format::is_stencil( void ) const				{	return test_option(StencilBuffer);			}
bool	system_pixel_format::is_stereo( void ) const				{	return test_option(StereoBuffers);			}
bool	system_pixel_format::is_direct_rendering( void ) const		{	return test_option(DirectRendering);		}
bool	system_pixel_format::is_overlay( void ) const				{	return test_option(HasOverlay);				}
bool	system_pixel_format::is_sample_buffers( void ) const		{	return test_option(SampleBuffers);			}
bool	system_pixel_format::is_depreciated_function( void ) const	{	return test_option(DeprecatedFunctions);	}

//=================================================================================================
inline int32_t system_pixel_format::get_samples( void ) const
{
   return _samples_nb;
}

//=================================================================================================
inline int32_t system_pixel_format::get_swap_interval( void ) const
{
	return _swap_interval;
}

//=================================================================================================
void system_pixel_format::set_version( int32_t major, int32_t minor )
{
//	if( major > 1 && minor > 0 )
//	if( major > 1 )
//	{
		_version_major = major;
		_version_minor = minor;
//	}
}

//=================================================================================================
void system_pixel_format::set_debug( bool b )	{	_b_debug = b;		}
bool system_pixel_format::is_debug() const		{	return _b_debug;	}

//=================================================================================================
void system_pixel_format::set_profile( OpenGLContextProfile p_profile )
{
	//detach();
	_profile = p_profile;
}

//=================================================================================================
system_pixel_format::OpenGLContextProfile system_pixel_format::get_profile( void ) const
{
	return _profile;
}
