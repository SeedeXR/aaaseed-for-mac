
#ifdef AAA_SYSTEM_PIXEL_FORMAT_H
#error "SYSTEM_PIXEL_FORMAT_H included more than once."
#endif
#define AAA_SYSTEM_PIXEL_FORMAT_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_NATIVE_SYSTEM_H
#	include "system/Native_System.h"
#endif
#ifndef AAA_NATIVE_TYPES_H
#	include "system/Native_Types.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

static INT32 CONST OPTION_SHIFT_NB = 16;
static INT32 CONST OPTION_SHIFT_MASK = 0xffff;
enum SYSTEM_PIXEL_FORMAT_OPTION : INT32
{
	DoubleBuffer            = 0x0001,
	DepthBuffer             = 0x0002,
	Rgba                    = 0x0004,
	AlphaChannel            = 0x0008,
	AccumBuffer             = 0x0010,
	StencilBuffer           = 0x0020,
	StereoBuffers           = 0x0040,
	DirectRendering         = 0x0080,
	HasOverlay              = 0x0100,
	SampleBuffers           = 0x0200,
	DeprecatedFunctions     = 0x0400,

	SingleBuffer            = DoubleBuffer			<< OPTION_SHIFT_NB,
	NoDepthBuffer           = DepthBuffer			<< OPTION_SHIFT_NB,
	ColorIndex              = Rgba					<< OPTION_SHIFT_NB,
	NoAlphaChannel          = AlphaChannel			<< OPTION_SHIFT_NB,
	NoAccumBuffer           = AccumBuffer			<< OPTION_SHIFT_NB,
	NoStencilBuffer         = StencilBuffer			<< OPTION_SHIFT_NB,
	NoStereoBuffers         = StereoBuffers			<< OPTION_SHIFT_NB,
	IndirectRendering       = DirectRendering		<< OPTION_SHIFT_NB,
	NoOverlay               = HasOverlay			<< OPTION_SHIFT_NB,
	NoSampleBuffers         = SampleBuffers			<< OPTION_SHIFT_NB,
	NoDeprecatedFunctions   = DeprecatedFunctions	<< OPTION_SHIFT_NB
};

typedef CustomFlags<SYSTEM_PIXEL_FORMAT_OPTION> FormatOptions;
NATIVE_DECLARE_OPERATORS_FOR_FLAGS(FormatOptions)


/**
* @class system_pixel_format
*
* @brief pixel format and options used to create OpenGL context
*/
class system_pixel_format final : public c_obj
{
public:

	/*!
	* \enum system_pixel_format::OpenGLContextProfile
	* 
	* This enum describes the OpenGL context profiles that can be
	* specified for contexts implementing OpenGL version 3.2 or
	* higher. These profiles are different from OpenGL ES profiles.
	* 
	* \value NoProfile            OpenGL version is lower than 3.2.
	* \value CoreProfile          Functionality deprecated in OpenGL version 3.0 is not available.
	* \value CompatibilityProfile Functionality from earlier OpenGL versions is available.
	*/
	enum OpenGLContextProfile : INT32
	{
		NoProfile,
		CoreProfile,
		CompatibilityProfile
	};

protected:
	/**
	* @brief Sets the format option to \a p_opt. (convenience method)
	*/
	FINLINE void set_option( FormatOptions p_opt );
	/**
	* @brief Returns true if format option \a p_opt is set; otherwise returns false.
	*/
	FINLINE	bool test_option( FormatOptions p_opt ) const;

public:
	FormatOptions			_options;
	int32_t					_plane;
	int32_t					_depth_size;
	int32_t					_accum_size;
	int32_t					_stencil_size;
	int32_t					_red_size;
	int32_t					_green_size;
	int32_t					_blue_size;
	int32_t					_alpha_size;
	int32_t					_samples_nb;
	int32_t					_swap_interval;
	int32_t					_version_major;
	int32_t					_version_minor;
	bool					_b_debug;

	OpenGLContextProfile	_profile;
public:
	//C_NO_CPY_MOVE( system_pixel_format )
	/**
	* @brief system_pixel_format class constructor
	*/
	system_pixel_format( void );
	/**
	* @brief system_pixel_format class copy constructor
	*/
	system_pixel_format( const system_pixel_format & p_parent );
	/**
	* @brief system_pixel_format class destructor
	*/
	virtual ~system_pixel_format( void );
	/**
	* @brief system_pixel_format class assignation operator
	*/
	system_pixel_format &operator=( const system_pixel_format & p_parent );


	/**
	* @brief comparison operator '==' (are format equals?)
	*/
	friend bool operator==( const system_pixel_format& p_formatFirst, const system_pixel_format& p_formatSecond );
	/**
	* @brief comparison operator '!=' (are format different?)
	*/
	friend bool operator!=( const system_pixel_format& p_formatFirst, const system_pixel_format& p_formatSecond );

	/**
	* @brief Creates a system_pixel_format object that is a copy of the current defaultFormat.
	* 
	* If \a p_options is not 0, the default format is modified by the
	* specified format options. The \a p_options parameter should be
	* GL::FormatOption values OR'ed together.
	* 
	* 
	* The \a p_plane parameter defaults to 0 and is the plane which this
	* format should be associated with. Not all OpenGL implementations
	* supports overlay/underlay rendering planes.
	*/
	void init( FormatOptions p_options, int32_t p_plane = 0 );

	//todo bad comment
	/**
	* @brief convert system PIXELFORMATDESCRIPTOR to system_pixel_format
	* @param p_pfd Win32 pixel format descriptor as PIXELFORMATDESCRIPTOR pointer
	* @return usable pixel format as system_pixel_format
	* @note static function
	*/
	void set_from_pfd( PIXELFORMATDESCRIPTOR CONST & pfd );
	//todo correct comment
	/**
	* @brief convert system system_pixel_format to PIXELFORMATDESCRIPTOR 
	* @param p_sysPixelFormat pixel format descriptor as system_pixel_format pointer
	* @return usable Win32 pixel format descriptor as PIXELFORMATDESCRIPTOR
	* @note static function
	*/
	void convert_to_pfd( PIXELFORMATDESCRIPTOR& pfd ) CONST;
	/**
	* convert system pixel format to system_pixel_format (identified by id)
	* @param p_hdc draw context as HDC
	* @param p_pfi pixel format ID as int32_t
	* @return usable pixel format as system_pixel_format
	* @note static function
	*/
	static void pfiToSystemPixelFormat( HDC p_hdc, int32_t p_pfi, system_pixel_format& fmt );

	static bool is_opengl_overlays( void );


	///////////////////////////////////////////////////////////////////////////////////////////////
	//		GET / SET
	///////////////////////////////////////////////////////////////////////////////////////////////
	
	/**
	* @brief get default pixel format as system_pixel_format
	*/
	static system_pixel_format& get_format_default( void );
	/**
	* @brief set default pixel format as \a p_format
	*/
	//static void set_format_default( const system_pixel_format & p_format );

	/**
	* @brief Set the minimum depth buffer size to \a p_size.
	*/
	void set_depth_buffer_size(int32_t p_size);
	/**
	* @brief Returns the depth buffer size.
	*/
	int32_t  get_depth_buffer_size( void ) const;

	/**
	* @brief Set the preferred red buffer size to \a p_size.
	*/
	void set_red_buffer_size(int32_t p_size);
	/**
	* @brief Returns the red buffer size.
	*/
	int32_t  get_red_buffer_size( void ) const;


	/**
	* @brief Set the preferred green buffer size to \a p_size.
	*/
	void set_green_buffer_size(int32_t p_size);
	/**
	* @brief Returns the green buffer size.
	*/
	int32_t  get_green_buffer_size( void ) const;

	/**
	* @brief Set the preferred blue buffer size to \a p_size.
	*/
	void set_blue_buffer_size(int32_t p_size);
	/**
	* @brief Returns the blue buffer size.
	*/
	int32_t  get_blue_buffer_size( void ) const;

	/**
	* @brief Set the preferred alpha buffer size to \a p_size.
	* This function implicitly enables the alpha channel.
	*/
	void set_apha_buffer_size( int32_t p_size );
	/**
	* @brief Returns the alpha buffer size.
	*/
	int32_t  get_alpha_buffer_size( void ) const;


	/**
	* @brief Set the preferred accumulation buffer size, where \a p_size is the bit depth for each RGBA component.
	*/
	void set_accum_buffer_size( int32_t p_size );
	/**
	* @brief Returns the accumulation buffer size.
	*/
	int32_t  get_accum_buffer_size( void ) const;

	/**
	* @brief Set the preferred stencil buffer size to \a p_size.
	*/
	void set_stencil_buffer_size( int32_t p_size );
	/**
	* @brief Returns the stencil buffer size.
	*/
	int32_t  get_stencil_buffer_size( void ) const;

	/**
	* @brief  Returns true if double buffering is enabled; otherwise returns false. 
	* Double buffering is enabled by default.
	*/
	bool is_buffer_double( void ) const;
	/**
	* @brief If \a p_enable is true sets double buffering; otherwise sets single buffering.
	* 
	* Double buffering is enabled by default.
	* 
	* Double buffering is a technique where graphics are rendered on an
	* off-screen buffer and not directly to the screen. When the drawing
	* has been completed, the program calls a swapBuffers() function to
	* exchange the screen contents with the buffer. The result is
	* flicker-free drawing and often better performance.
	* 
	* Note that single buffered contexts are currently not supported with EGL.
	*/
	void set_buffer_double( bool b_enable );

	/**
	* @brief Returns true if the depth buffer is enabled; otherwise returns false. 
	* The depth buffer is enabled by default.
	*/
	bool is_depth( void ) const;
	/**
	* @brief If \a p_enable is true enables the depth buffer; otherwise disables the depth buffer.
	* 
	* The depth buffer is enabled by default.
	* 
	* The purpose of a depth buffer (or Z-buffering) is to remove hidden
	* surfaces. Pixels are assigned Z values based on the distance to
	* the viewer. A pixel with a high Z value is closer to the viewer
	* than a pixel with a low Z value. This information is used to
	* decide whether to draw a pixel or not.
	*/
	void set_depth( bool b_enable );

	/**
	* @brief Returns true if RGBA color mode is set. Returns false if color index mode is set. 
	* The default color mode is RGBA.
	*/
	bool is_rgba( void ) const;
	/**
	* @brief If \a p_enable is true sets RGBA mode. If \a p_enable is false sets color index mode.
	* 
	* The default color mode is RGBA.
	* 
	* RGBA is the preferred mode for most OpenGL applications. In RGBA
	* color mode you specify colors as red + green + blue + alpha quadruplets.
	* 
	* In color index mode you specify an index into a color lookup table.
	*/
	void set_rgba( bool b_enable );

	/**
	* @brief Returns true if alpha buffer is set, false otherwise. 
	* Alpha is set by default.
	*/
	bool is_alpha( void ) const;
	/**
	* @brief If \a p_enable is true enables the alpha buffer; otherwise disables the alpha buffer.
	* 
	* The alpha buffer is disabled by default.
	* 
	* The alpha buffer is typically used for implementing transparency
	* or translucency. The A in RGBA specifies the transparency of a pixel.
	*/
	void set_alpha( bool b_enable );

	/**
	* @brief Returns true if the accumulation buffer is enabled; otherwise returns false. 
	* The accumulation buffer is disabled by default.
	*/
	bool is_accum( void ) const;
	/*
	* @brief If \a p_enable is true enables the accumulation buffer; otherwise disables the accumulation buffer.
	* 
	* The accumulation buffer is disabled by default.
	* 
	* The accumulation buffer is used to create blur effects and multiple exposures.
	*/
	void set_accum( bool b_enable );

	/**
	* @brief Returns true if the stencil buffer is enabled; otherwise returns false. 
	* The stencil buffer is enabled by default.
	*/
	bool is_stencil( void ) const;
	/**
	* @bruief If \a p_enable is true enables the stencil buffer; otherwise disables the stencil buffer.
	* 
	* The stencil buffer is enabled by default.
	* 
	* The stencil buffer masks certain parts of the drawing area so that
	* masked parts are not drawn on.
	*/
	void set_stencil( bool b_enable );

	/**
	* @brief Returns true if stereo buffering is enabled; otherwise returns false. 
	* Stereo buffering is disabled by default.
	*/
	bool is_stereo( void ) const;
	/*
	* @brief If \a p_enable is true enables stereo buffering; otherwise disables stereo buffering.
	* 
	* Stereo buffering is disabled by default.
	* 
	* Stereo buffering provides extra color buffers to generate left-eye and right-eye images.
	*/
	void set_stereo( bool b_enable );

	/**
	* @brief Returns true if direct rendering is enabled; otherwise returns false.
	* Direct rendering is enabled by default.
	*/
	bool is_direct_rendering( void ) const;
	/*
	* @brief If \a p_enable is true enables direct rendering; otherwise disables direct rendering.
	* 
	* Direct rendering is enabled by default.
	* 
	* Enabling this option will make OpenGL bypass the underlying window
	* system and render directly from hardware to the screen, if this is
	* supported by the system.
	*/
	void set_direct_rendering( bool b_enable );

	/**
	* @brief If \a p_enable is true, a GL context with multisample buffer support is picked; otherwise ignored.
	*/
	bool is_sample_buffers( void ) const;
	/**
	* @brief Returns true if multisample buffer support is enabled; otherwise returns false.
	* The multisample buffer is disabled by default.
	*/
	void set_sample_buffers( bool b_enable );

	/**
	* @brief Returns the number of samples per pixel when multisampling is
	* enabled. By default, the highest number of samples that is available is used.
	*/
	int32_t  get_samples( void ) const;
	/**
	* @brief Set the preferred number of samples per pixel when multisampling
	* is enabled to \a p_numSamples. By default, the highest number of samples available is used.
	*/
	void set_samples(int32_t p_numSamples);
	
	/**
	* @brief Returns the currently set swap interval. -1 is returned if setting
	* the swap interval isn't supported in the system GL implementation.
	*/
	int32_t  get_swap_interval( void ) const;
	/**
	* @brief Set the preferred swap interval. This can be used to sync the GL
	* drawing into a system window to the vertical refresh of the screen.
	* Setting an \a p_interval value of 0 will turn the vertical refresh syncing
	* off, any value higher than 0 will turn the vertical syncing on.
	* 
	* Under Windows and under X11, where the \c{WGL_EXT_swap_control}
	* and \c{GLX_SGI_video_sync} extensions are used, the \a p_interval
	* parameter can be used to set the minimum number of video frames
	* that are displayed before a buffer swap will occur. In effect,
	* setting the \a p_interval to 10, means there will be 10 vertical
	* retraces between every buffer swap.
	* 
	* Under Windows the \c{WGL_EXT_swap_control} extension has to be present,
	* and under X11 the \c{GLX_SGI_video_sync} extension has to be present.
	*/
	void set_swap_interval(int32_t p_interval);

	/**
	* @brief  Returns true if overlay plane is enabled; otherwise returns false.
	* Overlay is disabled by default.
	*/
	bool is_overlay( void ) const;
	/**
	* @brief If \a p_enable is true enables an overlay plane; otherwise disables
	* the overlay plane.
	* 
	* Enabling the overlay plane will cause QGLWidget to create an
	* additional context in an overlay plane. See the QGLWidget
	* documentation for further information.
	*/
	void set_overlay( bool b_enable );

	/**
	* @brief Returns the plane of this format. The default for normal formats
	* is 0, which means the normal plane. The default for overlay
	* formats is 1, which is the first overlay plane.
	*/
	int32_t get_plane( void ) const		{	return _plane;	}
	/**
	* @brief Sets the requested plane to \a p_plane. 0 is the normal plane, 1 is
	* the first overlay plane, 2 is the second overlay plane, etc.; -1,
	* -2, etc. are underlay planes.
	*/
	void set_plane( int32_t p_plane );

	/**
	* @brief Set the OpenGL version to the \a p_major and \a p_minor numbers. If a
	* context compatible with the requested OpenGL version cannot be
	* created, a context compatible with version 1.x is created instead.
	*/
	void set_version( int32_t p_major, int32_t p_minor );
	
	/**
	* @brief Set the OpenGL context profile to \a p_profile.
	*/
	void set_profile( OpenGLContextProfile p_profile );
	/**
	* @brief Returns the OpenGL context profile as OpenGLContextProfile.
	*/
	OpenGLContextProfile get_profile( void ) const;

	void set_debug( bool b );
	bool is_debug() const;

	bool is_depreciated_function() const;
	void set_depreciated_function( bool b );

};


///////////////////////////////////////////////////////////////////////////////////////////////////
//		GET / SET
///////////////////////////////////////////////////////////////////////////////////////////////////

inline int32_t system_pixel_format::get_depth_buffer_size( void ) const		{   return _depth_size;		}
inline int32_t system_pixel_format::get_red_buffer_size( void ) const		{   return _red_size;		}
inline int32_t system_pixel_format::get_green_buffer_size( void ) const		{   return _green_size;		}
inline int32_t system_pixel_format::get_blue_buffer_size( void ) const		{   return _blue_size;		}
inline int32_t system_pixel_format::get_alpha_buffer_size( void ) const		{   return _alpha_size;		}
inline int32_t system_pixel_format::get_accum_buffer_size( void ) const		{   return _accum_size;		}
inline int32_t system_pixel_format::get_stencil_buffer_size( void ) const	{   return _stencil_size;	}

