
#ifdef AAA_SYSTEM_NODE_H
#error "SYSTEM_NODE_H included more than once."
#endif
#define AAA_SYSTEM_NODE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#ifndef _STRING_
#	include <string>
#endif

#ifndef AAA_NATIVE_THREAD_H
#	include "thread/win32/Native_Thread.h"
#endif


class system_context;
class system_window;

class system_node_control;

class system_event_listener;
class system_event_handler;
class system_event_reader;


/**
* @class system_node 
*
* @brief base system node class
*
* @note extends NativeT::Thread class
* @note 1 node = 1 thread
*/
class system_node 
	: public NativeT::Thread
{
	friend class system_window;
	friend class system_context;
	friend class system_event_handler;

protected:
	system_window*			_system_window;
	system_context*			_context;
	system_event_handler*	_event_handler;
	system_event_reader*	_event_reader;
	system_event_listener*	_event_listener;

	INT32					_sx;
	INT32					_sy;

	INT32					_x;
	INT32					_y;
	
	system_node*			_node_master;

public:
	static	bool			b_sleep_message;
	static	INT32			sleep_message_millisec;

protected:
	C_NO_CPY_MOVE( system_node )
	/**
	* @brief system_node class constructor
	* 
	* @param p_x node window position on X-axis as INT32 
	* @param p_y node window position on Y-axis as INT32
	* @param p_windowWidth node window width as INT32
	* @param p_windowHeight node window height as INT32
	* @param p_pNodeMaster parent node as system_node pointer (default to nullptr)
	*/
	system_node(							INT32 CONST x,	INT32 CONST y,	INT32 CONST sx,	INT32 CONST sy,	system_node* CONST pNodeMaster = nullptr	);

	/**
	* @brief system_node class destructor
	*/
	virtual ~system_node( void );


public:
	/** \! Create, init and return new system_view pointer immediately, do not start thread. 
	* Example :
	*	SystemNode * ptr = system_node::create_ptr_wait();*/
	static system_node * create_ptr(		INT32 CONST x,	INT32 CONST y,	INT32 CONST sx,	INT32 CONST sy,	system_node* CONST pNodeMaster = nullptr	);
	/** \! Create, init and return new system_view pointer once its thread is started. 
	* Example :
	*	SystemNode * ptr = system_node::create_ptr_wait();*/
	static system_node * create_ptr_wait(	INT32 CONST x,	INT32 CONST y,	INT32 CONST sx,	INT32 CONST sy,	system_node* CONST pNodeMaster = nullptr	);
	/** \! Release and delete pointer class members once its thread is stopped. 
	* Example :
	*	system_view::release_ptr_wait( ptr );
	*/
	static void release_ptr_wait( system_node* pPtr );


	/**
	* @brief initialization function
	* @note virtual function
	*/
	virtual void init( void );
	/**
	* @brief release function
	* @note virtual function
	*/
	virtual void uninit( void );


	/** Thread run. */
	virtual void run( void );
	/** Thread stop
	* Stops the thread immediately by sending WM_QUIT message
	*/
	void stop( void );


	///////////////////////////////////////////////////////////////////////////////////////////////
	//		WINDOW UTILITIES
	///////////////////////////////////////////////////////////////////////////////////////////////

	///**
	//* @brief convert client coordinates to screen coordinates
	//* @param p_x client coordinate on X-axis
	//* @param p_y client coordinate on Y-axis
	//* @return calculated screen coordinates as glm::ivec2
	//*/
	//glm::ivec2 clientToScreen( const INT32 & p_x, const INT32 & p_y );


	void show_window( void ) CONST;
	void hide_window( void ) CONST;
	void set_window_foreground( void ) CONST;


	/**
	* @brief scale the size of the system window on output size and align it on top left corner
	* @note this doesn't affect window style
	*/
	void scale_window_on_output( void ) CONST;
	/**
	 * @brief scale system window on it's actual display 
	 * (only control window will use this so we only set the window position, not the size)
	 */
	void scale_window_on_display( void ) CONST;
	
	
	/**
	 * @brief minimize system window
	 */
	void minimize_window( void ) CONST;


	/**
	* @brief get the system window position on X-axis
	* @return window position on X-axis as INT32
	*/
	INT32 get_window_x( void ) CONST;
	/**
	* @brief get the system window position on Y-axis 
	* @return window position on Y-axis as INT32
	*/
	INT32 get_window_y( void ) CONST;
	/**
	* @brief set system window position
	* @param p_x position on X-axis as INT32
	* @param p_y position on Y-axis as INT32
	*/
	void set_window_xy( INT32 CONST & p_x, INT32 CONST & p_y ) CONST;
	/**
	* @brief set system window position on X-axis
	* @param p_x position on X-axis as INT32
	*/
	void set_window_x( INT32 CONST & p_x ) CONST;
	/**
	* @brief set system window position on Y-axis
	* @param p_y position on Y-axis as INT32
	*/
	void set_window_y( INT32 CONST & p_y ) CONST;


	/**
	* @brief get system window width
	* @return width as INT32
	*/
	INT32 get_window_sx( void ) CONST ;
	/**
	* @brief get system window height
	* @return height as INT32
	*/
	INT32 get_window_sy( void ) CONST ;


	void set_window(			INT32 CONST x, INT32 CONST y,	INT32 CONST sx, INT32 CONST sy ) CONST;
	void set_window_fullscreen(	INT32 CONST x, INT32 CONST y,	INT32 CONST sx, INT32 CONST sy ) CONST;
	/**
	* @brief set system window size
	* @param sx the new width of the window
	* @param sy the new height of the window
	*/
	void set_window_size(										INT32 CONST sx, INT32 CONST sy ) CONST;
	/**
	* @brief set system window width
	* @param p_width the new width of the window
	*/
	void set_window_sx( INT32 CONST & p_width ) CONST;
	/**
	* @brief set system window height
	* @param p_height the new height of the window
	*/
	void set_window_sy( INT32 CONST & p_height ) CONST;
	/**
	* @brief set system window title (UTF-8)
	* @param p_title the new title of the window, encoded in UTF-8
	*/
	void set_window_title( C_PCHAR_C title ) CONST;


	/**
	* @brief set system window draggeable status
	* @param p_bStatus true if window accepts the dragged files
	*/
	void setWindowDragAcceptFiles( bool CONST & p_bStatus ) CONST;

	//	unused
	//void set_window_cursor( cursor::State CONST & cursor_type );


	///////////////////////////////////////////////////////////////////////////////////////////////
	//		CONTEXT UTILITIES
	///////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief make context current
	* @return true on success, false otherwise
	*/
	bool make_context_current( void ) CONST;
	/**
	* @brief release context
	* @return true on success, false otherwise
	*/
	bool release_context_current( void ) CONST;


	/**
	* @brief swap OpenGL context buffers
	*/
	void swap_buffers( void ) CONST;

	void init_multitouch( void ) CONST;


	///////////////////////////////////////////////////////////////////////////////////////////////
	//		GET / SET
	///////////////////////////////////////////////////////////////////////////////////////////////
	
	/**
	* @brief get event reader
	* @return reader as system_event_reader pointer
	*/
	FINLINE system_event_reader*	get_event_reader( void ) CONST		{	return _event_reader;	}

	/**
	* @brief get event listener
	* @return listener as system_event_listener pointer
	*/
	FINLINE system_event_listener*	get_event_listener( void ) CONST	{	return _event_listener;	}

	/**
	* @brief get system window
	* @return window as system_window pointer
	*/
	FINLINE system_window*			get_window( void ) CONST			{	return _system_window;	}

	/**
	* @brief get OpenGL context
	* @return context as system_context pointer
	*/
	FINLINE system_context*			get_context( void ) CONST			{	return _context;		}
};

