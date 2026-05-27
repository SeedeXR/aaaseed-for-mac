
#ifdef AAA_SYSTEM_CONTEXT_H
#error "SYSTEM_CONTEXT_H included more than once."
#endif
#define AAA_SYSTEM_CONTEXT_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_SYSTEM_PIXEL_FORMAT_H
#	ifndef AAA_AAA_OS_H
#		include "aaa_os.h"
#	endif
#	include "system/win32/SystemPixelFormat.h"
#endif


class system_node;
class system_window;


/**
* @class system_context 
* 
* @brief describes Pixel Format and manages OpenGL context
*/
class system_context final : public c_obj
{
	friend class system_node;
//	friend class system_node_control;

protected:
	system_node*			_system_node;
	system_context*			_system_context_master;

	HGLRC					_context;
	HDC						_hdc;
	HWND					_hd_win;

	int32_t					_pixel_format_id;

	system_pixel_format		_pixel_format;

public:
//	uint32_t				m_NodalId;
	bool					_b_sharing;
	bool					_b_valid;

protected:
	/**
	* @brief choose available and compliant context properties
	* @param p_pShareContext parent context as system_context pointer (default to nullptr)
	*/
	bool choose_context( system_context* p_Context = nullptr );
	/**
	* @brief choose available pixel format, as close to desired options as possible depending on hardware
	* @param p_pPfd pixel format descriptor as void pointer
	* @return chosen pixel format ID as int32_t
	*/
	int32_t choose_pixel_format( PIXELFORMATDESCRIPTOR CONST & pfd );


	/**
	* @brief DC attached pixel format may change selected options, 
	* grab and stock updated values
	*/
	void update_format_version( void );



public:
	C_NO_CPY_MOVE( system_context )
	/**
	* @brief system_context class constructor
	*/
	system_context( void );
	/**
	* @brief system_context class destructor
	*/
	virtual ~system_context( void );


	/**
	* @brief initialization function
	* 
	* @param p_nodalId parent node ID as int32_t
	* @param p_pNode parent node as system_node pointer
	* @param p_pContextMaster master/parent context as system_context pointer (default to nullptr)
	*
	* @return true on creation success (HDC + HGLRC), false otherwise
	* 
	* @note after calling that function context is current
	*/
	bool init( int32_t p_nodalId, system_node* p_pNode, system_context* p_pContextMaster = nullptr );


	/**
	* @brief release HGLRC and HDC, nullify members
	*/
	void release( void );


	/**
	* @brief make context current/active
	* @note this should be atomic
	*/
	bool make_current( void );
	/**
	* @brief release context activation
	* @note this should be atomic
	*/
	bool done_current( void );
	/**
	* @brief terminate OpenGL operations and swap buffers if needed
	*/
	virtual void swap_buffers( void );


	/**
	* @brief stock current context as \a p_pContext
	*/
	static void set_context_cur( system_context* p_pContext );
	/**
	* @brief return current context as system_context pointer
	*/
	static const system_context * get_current_context( void );
	/**
	* @brief return number of context created successfully
	*/
	static INT32 get_nb( void );
};


