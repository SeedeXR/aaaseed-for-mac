
#ifdef AAA_SYSTEM_WINDOW_H
#error "SYSTEM_WINDOW_H included more than once."
#endif
#define AAA_SYSTEM_WINDOW_H 1



#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif	
#ifndef AAA_AAA_RECT_H
#	include "aaa/aaa_rect.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

class system_node;


///////////////////////////////////////////////////////////////////////////////////////////////////
//		system_window class
///////////////////////////////////////////////////////////////////////////////////////////////////

/**
* @class system_window
* @brief Windows system window 
*/

class system_window final : public c_obj
{
private:
	HWND					_handle;
	int32_t					_x;
	int32_t					_y;
	uint32_t				_sx;
	uint32_t				_sy;

	aaa::rect::xy_sxy		_before_scale_coor;

//todo rename for aaa
	ATOM					m_Atom;
	HINSTANCE				m_Hinstance;

//	int32_t					m_Cursor;

	RECT					m_WindowedRect;
	bool					m_bsetScaledOnOutput;
	bool					m_bScaledOnOutput;
	LONG					m_Style;
	LONG					m_ExStyle;
	LONG					m_StyleFullscreen;
	LONG					m_ExStyleFullscreen;

	o_str					_o_title;
	bool					_b_fullscreen;

public:
//	int32_t					m_NodalId;
//	SystemNode *			m_pNodeParent;

//	uint32_t				m_minWidth;		//unused for now
//	uint32_t				m_minHeight;	//unused for now
//	uint32_t				m_maxWidth;		//unused for now
//	uint32_t				m_maxHeight;	//unused for now

protected:
	C_NO_CPY_MOVE( system_window )
	/**
	* @brief system_window class constructor
	*
	* @param p_nodalId window node id
	* @param p_pNodeParent parent system node as system_node pointer
	* @param p_x window position on X-axis as int32_t
	* @param p_y window position on Y-axis as int32_t
	* @param p_width widow width as uint32_t
	* @param p_height window height as uint32_t
	* @param p_bsetScaledOnOutput is window scaled on monitor (default to false)
	*/
	system_window
		(	
		int32_t			CONST nodalId, 
		system_node*	CONST pNodeParent, 
		int32_t			CONST x, 
		int32_t			CONST y, 
		uint32_t		CONST sx, 
		uint32_t		CONST sy, 
		bool			CONST bsetScaledOnOutput = false
		);

	/**
	* @brief this avoid duplicate code
	*/
	void set_window_style( LONG CONST style, LONG CONST style_ex, C_PCHAR mess ) CONST;
public:
	//	console utils
	static void				push_console();
	static void				pop_console();
	static void				minimize_console();
	static void				restore_console();
	static system_window*	get_window_main();
	static void				set_monitor_off();
	static void				set_monitor_standby();
	static void				set_monitor_on();	

	/** Create and return new system_window pointer. */
	static system_window* create_ptr
		(	
		int32_t			CONST nodalId, 
		system_node*	CONST pNodeParent, 
		int32_t			CONST x, 
		int32_t			CONST y, 
		uint32_t		CONST sx, 
		uint32_t		CONST sy, 
		bool			CONST bsetScaledOnOutput = false
		);


	/**
	* @brief system_window class destructor
	*/
	virtual ~system_window( void );


	/**
	* @brief initialization function
	* @note virtual function
	*/
	virtual void init( void ); 

	
	///////////////////////////////////////////////////////////////////////////////////////////////
	//		UTILITIES
	///////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief updates max width and height values
	*/
	void update_max_size( void );

	/** 
	* @brief set window active
	*/
	void set_active( void ) CONST;

	/**  
	* @brief set foreground
	*/
	void set_foreground(void) CONST;

	/** 
	* @brief show window
	*/
	void show( void ) CONST;

	/** 
	* @brief hide window
	*/
	void hide( void ) CONST;

	/**
	* @brief minimize window
	*/
	void minimize( void ) CONST;

	/**
	* @brief restore window to its size and position after it was minimized
	*/
	void restore( void ) CONST;

	/**
	* @brief close window
	*/
	void close( void ) CONST;

	/**
	* @brief push window, set the window in bottom of display
	*/
	void push( void ) CONST;

	/**
	* @brief pop window, set the window on top of display
	*/
	void pop( void ) CONST;
 
	/**
	* @brief set_topmost window, set the window on top of all display
	*/
	void set_topmost( void ) CONST;
	void set_notopmost( void ) CONST;

	/**
	* @brief scale the size of the window on output size and align it on top left corner
	* @note this doesn't affect window style
	*/
	void scale_on_output( void );

	/**
	* @brief align window on top left corner
	* @note this doesn't affect window style
	*/
	void fix_on_output( void );

	/**
	* @brief resize window to desired width and height
	* @param sx the new width of the window
	* @param sy the new height of the window
	*/
	void set_size( uint32_t CONST sx, uint32_t CONST sy );
	/**
	* @brief set the width of the window
	* @param sx the new width of the window
	*/
	void set_size_x( uint32_t CONST sx );
	/**
	* @brief set the height of the window
	* @param sy the new height of the window
	*/
	void set_size_y( uint32_t CONST sy );
	
	/**
	* @brief move the mouse pointer inside this window
	*
	* @note The coordinates are relative to the window client area. If the
	* given coordinates are outside the client area, no moving is
	* performed.
	* 
	* @param p_deportX horizontal deport inside window (window zero is top/left)
	* @param p_deportY vertical deport inside window (window zero is top/left)
	*/
	void wrap_mouse( int32_t CONST p_deportX, int32_t CONST p_deportY );


	/**
	* @brief equest window repaint,
	*
	* Ask the windowing system to be repainted. The windowing
	* system will respond and an ExposeEvent will be generated
	* when appropriate.
	* 
	* If the width or the height of the dirty rectangle
	* area is zero, the entire window be repainted.
	* 
	* @param p_x the left side of the dirty rectangle (default to zero)
	* @param p_y the top side of the dirty rectangle (default to zero)
	* @param p_width the width of the dirty rectangle (default to zero)
	* @param p_height the height of the dirty rectangle (default to zero)
	* @see mwm_window::ExposeEvent
	*/
	void repaint( int32_t CONST p_x=0, int32_t CONST p_y=0, uint32_t CONST p_width=0, uint32_t CONST p_height=0 ) CONST;

	
	///////////////////////////////////////////////////////////////////////////////////////////////
	//		GET / SET
	///////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @brief get the size of the window
	* @param p_width the width of the window gets written here
	* @param p_height the height of the window gets written here
	*/
	void get_size( uint32_t & p_width, uint32_t & p_height ) CONST;
	/**
	* @brief get window width
	* @return width as uint32_t
	*/
	uint32_t get_sx( void );
	/**
	* @brief get window height
	* @return height as uint32_t
	*/
	uint32_t get_sy( void );


	
	/**
	* @brief get the position of the window on X-axis
	* @return window position on X-axis as int32_t
	*/
	int32_t get_x( void );
	/**
	* @brief get the y position of the window on Y-axis 
	* @return window position on Y-axis as int32_t
	*/
	int32_t get_y( void );

	bool is_fullscreen() CONST			{ return _b_fullscreen; }
	
	/**
	* @brief set the position of the window
	* @param p_x the new position of the window on X-axis
	* @param p_y the new position of the window on Y-axis
	*/
	void set_position( int32_t CONST & p_x, int32_t CONST & p_y );
	/**
	* @brief set window position on X-axis
	* @param p_x the new position of the window on X-axis
	*/
	void set_x( int32_t CONST & p_x );
	/**
	* @brief set window position on Y-axis
	* @param p_y the new position of the window on X-axis
	*/
	void set_y( int32_t CONST & p_y );

	void set_window_fullscreen(	int32_t CONST x, int32_t CONST y, uint32_t CONST sx, uint32_t CONST sy );
	void set_window(			int32_t CONST x, int32_t CONST y, uint32_t CONST sx, uint32_t CONST sy );

	void save_placement();
	void load_placement();

	///**
	//* @brief convert client coordinates to screen coordinates
	//* @param p_x client coordinate on X-axis
	//* @param p_y client coordinate on Y-axis
	//* @return calculated screen coordinates as glm::ivec2
	//*/
	//glm::ivec2 clientToScreen( int32_t CONST & p_x, int32_t CONST & p_y );

	
	/**
	* @brief set the title of the window (UTF-8)
	* @param p_title the new title of the window, encoded in UTF-8
	*/
	void set_title( C_PCHAR_C title );

	
	/**
	* @brief set the window draggable status
	* @param p_bStatus true if window accepts the dragged files
	*/
	void set_drag_accept_files( bool CONST & p_bStatus ) CONST;

	FINLINE HWND get_handle( void ) CONST			{	return _handle;	}

	//hack
	//static HWND get_handle_def( void );
};

