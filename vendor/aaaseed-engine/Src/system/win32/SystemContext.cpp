
#include "SystemContext.h"
#include "gol/gol_os.h"
#include "SystemNode.h"
#include "SystemWindow.h"
#include "SystemError.h"
#include "gol/gol.h"
#include "draw/seeddraw.h"
#include "spy.h"
#include "err.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//		Static
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	INT32			context_nb		{0};
	system_context*	context_current	{};
}

INT32 system_context::get_nb( void )
{
	return context_nb;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		system_context class
///////////////////////////////////////////////////////////////////////////////////////////////////

//=================================================================================================
system_context::system_context( void )

	// Members initialization
	: _system_node				( nullptr )
	, _system_context_master	( nullptr )
//	, m_NodalId					( -1 )

	, _b_sharing				( false )
	, _b_valid					( false )

	, _pixel_format_id			( 0 )
	, _pixel_format				()

	, _context					( nullptr )
	, _hdc						( nullptr )
	, _hd_win					( nullptr )
{}

//=================================================================================================
system_context::~system_context( void )
{
	// do not delete -> shared pointers
	_system_node = nullptr;	
	_system_context_master = nullptr;
}



//=================================================================================================
bool system_context::init( int32_t p_nodalId, system_node* p_pNode, system_context* p_pContextMaster )
{
//	m_NodalId				= p_nodalId;
	_system_node			= p_pNode;
	_system_context_master	= p_pContextMaster;

	_hd_win = _system_node->get_window()->get_handle();

	_pixel_format = system_pixel_format::get_format_default();

	_b_valid = choose_context( _system_context_master );

	return _b_valid;
}



//=================================================================================================
void system_context::release( void )
{
	// Workaround for MATROX driver: make a dummy call to OpenGL to force DLL loading.
	//GOL::do_matrox_hack();

	// Rendering context handle
	if( _context ) 
	{
		wglDeleteContext( _context );
		_context = nullptr;
		--context_nb;
	}
	// Draw context
	if( _hd_win && _hdc ) 
	{
		ReleaseDC( _hd_win, _hdc );
	}

	_hdc				= nullptr;
	_hd_win				= nullptr;
	_pixel_format_id	= 0;
	_b_sharing			= false;
}


//=================================================================================================
namespace
{
	//todo do we really need this now we use bool and not BOOL
	bool equal_logic( bool a, bool b )	{	return (a && b) || (!a && !b);	}
};

//=================================================================================================
bool system_context::choose_context( system_context* p_system_context )
{
	HGLRC ctx = nullptr;

	if( _context )
	{
		debug_break( "already a context" );
		return true;
	}

	// Workaround for MATROX driver: make a cheap call to OpenGL to force loading of DLL.
	//GOL::do_matrox_hack();

	// Initialize OpenGL drawing device
	_hdc = ::GetDC( _hd_win );
	if( _hdc == 0 ) 
	{
		debug_break( "%s() Paint device cannot be null.", __FUNCTION__ );
		goto exit_err;
	}

	// Initialize and choose best fitting Pixel Format
	PIXELFORMATDESCRIPTOR pfd;
		// Pixel format descriptor
	_pixel_format.convert_to_pfd( pfd );
	_pixel_format_id = choose_pixel_format( pfd );
	if( _pixel_format_id == 0 ) 
	{
		debug_break( "%s() choose_pixel_format() failed.", __FUNCTION__ );
		//b_ret = false;
	}
	// If found m_pixelFormat is set in function "choosePixelFormat()"
	//system_pixel_format::pfiToSystemPixelFormat( m_Hdc, m_pixelFormatId, m_pixelFormat );


	// Apply pixel format to DC
	if( !::SetPixelFormat( _hdc, _pixel_format_id, &pfd ) ) 
	{
		debug_break( "%s() SetPixelFormat() failed.", __FUNCTION__ );
		//b_ret = false;
	}

	// we get a context that will use at least to get proc addr or fully used
	GOOD_PRINT_STRING( "Will try to create OpenGl context with wglCreateContext()" );
	ctx = wglCreateContext( _hdc );
	GOOD_PRINT_STRING( "Done wglCreateContext()");
	if( !ctx ) 
	{
		debug_break( "%s() wglCreateContext() failed.", __FUNCTION__ );
		goto exit_err;
	}

	{
		int attribs[16];	//we need 9 for now (4+2+2+1) so 16 is enough
		int* p_att = attribs;
		if( _pixel_format._version_major!=0 )
		{
			*p_att++ = WGL_CONTEXT_MAJOR_VERSION_ARB;
			*p_att++ = _pixel_format._version_major;

			*p_att++ = WGL_CONTEXT_MINOR_VERSION_ARB;
			*p_att++ = _pixel_format._version_minor;
		}

		switch( _pixel_format.get_profile() )
		{
		case system_pixel_format::OpenGLContextProfile::CoreProfile:
			*p_att++ = WGL_CONTEXT_PROFILE_MASK_ARB;
			*p_att++ = WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
			break;
		case system_pixel_format::OpenGLContextProfile::CompatibilityProfile:
			*p_att++ = WGL_CONTEXT_PROFILE_MASK_ARB;
			*p_att++ = WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
			break;
		case system_pixel_format::OpenGLContextProfile::NoProfile:
		default:
			break;
		}
	// Uncomment this for forward compatibility mode
	//WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,

		if( _pixel_format.is_debug() )
		{
			*p_att++ = WGL_CONTEXT_FLAGS_ARB;
			*p_att++ = WGL_CONTEXT_DEBUG_BIT_ARB;
		}

		if( p_att != attribs )
		{
			GOOD_PRINT_STRING( "Will try now to create OpenGl context with wglCreateContextAttribsARB()" );

			// we already have an opengl context to get the address of wglCreateContextAttribsARB
			if( wglMakeCurrent( _hdc, ctx ) )
			{
				wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress( "wglCreateContextAttribsARB" );
				if( wglCreateContextAttribsARB != nullptr )
				{
					*p_att = 0;
					GOOD_PRINT_STRING( "calling wglCreateContextAttribsARB()" );
					_context = wglCreateContextAttribsARB( _hdc, (HGLRC)0, attribs );
					GOOD_PRINT_STRING( "Done wglCreateContextAttribsARB()" );
					if( _context )
						wglDeleteContext( ctx );
					else
						debug_break( "%s() wglCreateContextAttribsARB() failed.", __FUNCTION__ );	
				}
				else
					debug_break( "%s() did not find wglCreateContextAttribsARB()", __FUNCTION__ );
			}		
		}
	}

	if( !_context )
	{
		_context = ctx;
		ctx = nullptr;
	}

	// Create context (GLRC) -> always create it event if master context passed in case of master context release.
	if( _context ) 
	{
		++context_nb;
		// Apply shared display list if needed (pretty much deprecated as I write (OpenGL 4.2 spec))
		// Kept for compatibility and future use
		if( p_system_context ) 
			p_system_context->_b_sharing = _b_sharing = (::wglShareLists( p_system_context->_context, _context ) ) ? true : false;

		// Make context current (has to be activated here to fix DC bound)
		if( !::wglMakeCurrent( _hdc, _context ) )
		{
			//todo make a generic fn for this
			DWORD err = aaa::system::get_err_last();
			if( err != 0 )
				ERR_PRINT_STRING( "Can't wglMakeCurrent error was %s", aaa::system::get_err_message(err).c_str() );
		}

		GOL::OS::init();

		// Stock DC auto updated format
		update_format_version();
		return true;
	}

exit_err:
	if( ctx )
		wglDeleteContext( ctx );
	return false;
}



//=================================================================================================
int32_t system_context::choose_pixel_format( PIXELFORMATDESCRIPTOR CONST & pfd_in )
{
	// Workaround for MATROX driver: make a cheap call to OpenGL to force loading of DLL.
	//GOL::do_matrox_hack();

	// Choose pixel format
	int32_t pfi_chosen = ::ChoosePixelFormat( _hdc, &pfd_in );
	bool b_found;
	if( !pfi_chosen )
	{
		ERR_PRINT_STRING( "%s() ChoosePixelFormat() failed : error was %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
		b_found = false;
	}
	else
	{
		DBG_PRINT_STRING( "%s() ChoosePixelFormat() return %d", __FUNCTION__, pfi_chosen );
		b_found = true;
	}
	
	// GDI function ChoosePixelFormat() does not handle overlay and direct-rendering requests
	PIXELFORMATDESCRIPTOR pfd;
	system_pixel_format fmt;
//	BYTE pmDepth = 0;
	if( b_found ) 
	{
		::DescribePixelFormat( _hdc, pfi_chosen, sizeof(PIXELFORMATDESCRIPTOR), &pfd );
		system_pixel_format fmt;
		fmt.set_from_pfd( pfd );
		if( !(pfd.dwFlags & PFD_SUPPORT_OPENGL) )
		{
			DBG_PRINT_STRING( "No PFD_SUPPORT_OPENGL" );
			b_found = false;
		}
		if(	_pixel_format.is_overlay() && !fmt.is_overlay() )
		{
			DBG_PRINT_STRING( "No overlay in choice" );
			b_found = false;
		}
		if(	!equal_logic( _pixel_format.is_direct_rendering(), fmt.is_direct_rendering() ) )
		{
			DBG_PRINT_STRING( "No direct rendering" );
			b_found = false;
		}
//		if(	!(pfd.dwFlags & PFD_DRAW_TO_BITMAP) || (pfd.cColorBits != pmDepth) )
//		{
//			DBG_PRINT_STRING( "No PFD_DRAW_TO_BITMAP and pfd.cColorBits == pmDepth" );
//			b_found = false;
//		}
		if(	!(pfd.dwFlags & PFD_DRAW_TO_WINDOW ) )
		{
			DBG_PRINT_STRING( "No PFD_DRAW_TO_WINDOW" );
			b_found = false;
		}
		if( !equal_logic( _pixel_format.is_rgba(), fmt.is_rgba() ) )
		{
			DBG_PRINT_STRING( "rgba differenceW" );
			b_found = false;
		}	

		if( !b_found )
			DBG_PRINT_STRING( "Unhappy with Pixel Format chosen, Will try to find ourself" );
	}

	if( !b_found ) 
	{
		int32_t pfi_max = ::DescribePixelFormat( _hdc, 0, 0, nullptr );
		int32_t score_best = -1;
		int32_t pfi_best = -1;

		for( int32_t pfi = 1; pfi <= pfi_max; ++pfi ) 
		{
			::DescribePixelFormat( _hdc, pfi, sizeof(PIXELFORMATDESCRIPTOR), &pfd );
			if( !(pfd.dwFlags & PFD_SUPPORT_OPENGL) )
				continue;
//			if( !(pfd.dwFlags & PFD_DRAW_TO_BITMAP) || (pfd.cColorBits != pmDepth) )
//				continue;
			if( !(pfd.dwFlags & PFD_DRAW_TO_WINDOW) )
				continue;

			system_pixel_format fmt;
			fmt.set_from_pfd( pfd );
			if( _pixel_format.is_overlay() && !fmt.is_overlay() )
				continue;

			//todo computation of score is imperfect
			int32_t score = pfd.cColorBits;
			if( equal_logic( _pixel_format.is_depth(),				fmt.is_depth()				) )
				score += pfd.cDepthBits;
			if( equal_logic( _pixel_format.is_alpha(),				fmt.is_alpha()				) )
				score += pfd.cAlphaBits;
			if( equal_logic( _pixel_format.is_accum(),				fmt.is_accum()				) )
				score += pfd.cAccumBits;
			if( equal_logic( _pixel_format.is_stencil(),			fmt.is_stencil()			) )
				score += pfd.cStencilBits;
			if( equal_logic( _pixel_format.is_buffer_double(),		fmt.is_buffer_double()		) )
				score += 1000;
			if( equal_logic( _pixel_format.is_stereo(),				fmt.is_stereo()				) )
				score += 2000;
			if( equal_logic( _pixel_format.is_direct_rendering(),	fmt.is_direct_rendering()	) )
				score += 4000;
			if( equal_logic( _pixel_format.is_rgba(),				fmt.is_rgba()				) )
				score += 8000;

			//DBG_PRINT_STRING( "\tEvaluating format %d with score %d", pfi, score );
			if( score > score_best ) 
			{
				DBG_PRINT_STRING( "\t\t%d NEW BEST SCORE %d", pfi, score );
				score_best = score;
				pfi_best = pfi;
			}
		}

		if( pfi_best > 0 ) 
		{
			// Stock chosen pfi ID
			pfi_chosen = pfi_best;
			// Stock selected pixel format
			_pixel_format = fmt;
		}
	}

	DBG_PRINT_STRING( "%s() return %d", __FUNCTION__, pfi_chosen );
	return pfi_chosen;
}



//=================================================================================================
void system_context::update_format_version( void )
{
	const GLubyte* s = glGetString(GL_VERSION);
	if( !(s && s[0] >= '0' && s[0] <= '9' && s[1] == '.' && s[2] >= '0' && s[2] <= '9') ) 
	{
		if( !s )
			debug_break( "%s() OpenGL version string is null.", __FUNCTION__ );
		else 
			debug_break( "%s() Unexpected OpenGL version string format.", __FUNCTION__ );
		_pixel_format.set_version( 0, 0 );
		_pixel_format.set_profile( system_pixel_format::NoProfile );
		_pixel_format.set_depreciated_function( TRUE ) ;
		return;
	}

	//Version
	int32_t major = s[0] - '0';
	int32_t minor = s[2] - '0';
	_pixel_format.set_version( major, minor );

	//context flags
	if( major < 3 ) 
	{
		_pixel_format.set_profile(system_pixel_format::NoProfile);
		_pixel_format.set_depreciated_function( TRUE );
	} 
	else 
	{
		GLint value = 0;
		if( major > 3 || minor >= 2 )
			value = GOL::get_integer( GL_CONTEXT_PROFILE_MASK );

		system_pixel_format::OpenGLContextProfile profile;
		switch( value )
		{
		case WGL_CONTEXT_CORE_PROFILE_BIT_ARB:			profile = system_pixel_format::CoreProfile;				break;
		case WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB:	profile = system_pixel_format::CompatibilityProfile;	break;
		default:										profile = system_pixel_format::NoProfile;				break;
		}
		_pixel_format.set_profile( profile );	

		value = GOL::get_integer( GL_CONTEXT_FLAGS );
		_pixel_format.set_depreciated_function(	value & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT	);
		_pixel_format.set_debug(				value & GL_CONTEXT_FLAG_DEBUG_BIT				);
		//todo GOL::CTX deal with part of this: we should remove duplicate code
		//value & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT ;
		//value & GL_CONTEXT_FLAG_NO_ERROR_BIT ;
	}
}


//=================================================================================================
void system_context::set_context_cur( system_context * p_context )
{
	context_current = p_context;
}

//=================================================================================================
const system_context* system_context::get_current_context( void )
{
	return context_current;
}



//=================================================================================================
bool system_context::make_current( void )
{
	bool bret = true;

	HGLRC hgl = _system_context_master ? _system_context_master->_context : _context ;

	if( hgl == ::wglGetCurrentContext() )
	{
//		DBG_PRINT_STRING( "%s() context already current %i \n", __FUNCTION__, (uintptr_t)hgl );			
	}
	else if( ::wglMakeCurrent( _hdc, hgl) ) 
	{
		set_context_cur( this );	
	}
	else 
	{
		debug_break( "%s() wglMakeCurrent() context %i failed %s", __FUNCTION__, (uintptr_t)hgl, aaa::system::get_err_message().c_str() );
		bret = false;
	}

	return bret;
}

//=================================================================================================
bool system_context::done_current( void )
{
	bool BReturn = true;

//todo activate or deactivate dynamically if needed
//AAASeed just have one window one context....

	if( ::wglMakeCurrent( nullptr, nullptr) )
	{
		set_context_cur( nullptr );
	}
	else 
	{
		DBG_PRINT_STRING( "%s() wglMakeCurrent(nullptr,nullptr) failed %s \n", __FUNCTION__, aaa::system::get_err_message().c_str() );
		BReturn = false;
	}

	return BReturn;
}



//=================================================================================================
//todox64 unite with glut path
void system_context::swap_buffers( void )
{
	// Multiple rendering buffers
	if( _pixel_format.is_buffer_double() && draw::is_swap_buffer() ) 
	{
		SPY_PUSH_RANGE( "SWAP", spy::GOL_LOW );
		TBUF_ADD( tbuf::CH_SWAP_BUF, 1., "start" );

#if false // done by Romain Cheminade but look inverted to me (Maa)
		if( !_pixel_format.get_plane() ) 
		{
			//todoqqq ca coincais sous windows le 7 Nov 2019 by Maa
			if( !_pixel_format.is_overlay() )
				SwapBuffers( _hdc );
			else // format has overlay
				wglSwapLayerBuffers( _hdc, WGL_SWAP_MAIN_PLANE );
		}
		else // format has plane
			wglSwapLayerBuffers( _hdc, WGL_SWAP_OVERLAY1 );
#else
		wglSwapLayerBuffers( _hdc, WGL_SWAP_MAIN_PLANE );
#endif
		TBUF_ADD( tbuf::CH_SWAP_BUF, 0., "stop" );
		SPY_POP_RANGE();
	}
	// Single rendering buffers
	else
	{
		// Force OpenGL command termination
		GOL::flush_always();
		GOL::finish_always();
#if AAA_DEBUG()
		if( !_hdc )
			DBG_PRINT_STRING( "%s() HDC invalid.", __FUNCTION__ );
#endif
	}
	draw::speed_render_swap_end();
}
