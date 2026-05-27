
#ifdef AAA_SYSTEMEVENT_H
#error "SYSTEM_EVENT_H included more than once."
#endif
#define AAA_SYSTEMEVENT_H 1


//////////////////////////////////////////////////
//                                              //
//  CAUTION :                                   //
//  this header is shared between target        //
//  system compilations (Windows, OSX, LINUX)   //
//                                              //
//////////////////////////////////////////////////

#ifndef	AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_SYSTEMKEYBOARD_H
#	include "system/win32/SystemKeyboard.h"
#endif
#ifndef AAA_SYSTEMMOUSE_H
#	include "system/shared/SystemMouse.h"
#endif



namespace evt
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	//	System event types enum
	///////////////////////////////////////////////////////////////////////////////////////////////

	enum SYSTEM_EVENT_TYPE : INT32
	{
		NO_EVENT,
		KEY_DOWN,
		KEY_UP,
		BUTTON_DOWN,
		BUTTON_UP,
		BUTTON_DOUBLE,
		TEXT_INPUT,
		WIN_EXPOSE,
		MOTION,
		PASSIVE_MOTION,
		MOUSE_ENTER,
		MOUSE_LEAVE,
		WIN_MOVE,
		WIN_RESIZE,
		WIN_CLOSE,
		WIN_FOCUS_GOT,
		WIN_FOCUS_LOST,
		WIN_SHOW,
		WIN_HIDE,
		MENU,
		DISPLAY_CHANGE,
		DEVICE_CHANGE,
		DROP_FILES,
		NATIVE_EVENT
	};

	extern CHAR CONST & get_name( SYSTEM_EVENT_TYPE type );


	///////////////////////////////////////////////////////////////////////////////////////////////
	//	EventType structure
	///////////////////////////////////////////////////////////////////////////////////////////////

	typedef struct 
	{
		// Shared
		SYSTEM_EVENT_TYPE	_type;
	} ST_EVENT_TYPE;



	///////////////////////////////////////////////////////////////////////////////////////////////
	//	System Events
	///////////////////////////////////////////////////////////////////////////////////////////////

	struct ST_EVENT_ANY
	{
		ST_EVENT_TYPE	type;
		INT32			x, y, a, b, c;
	};


	struct ST_EVENT_KEY
	{
		ST_EVENT_TYPE		type;
		keyboard::SYMBOL	symbol;
		keyboard::MODIFIER	keymod;
		bool				b_repeat;
		bool				b_special;
	};


	struct ST_EVENT_BUTTON
	{
		ST_EVENT_TYPE		type;
		INT32				x, y;
		mouse::BUTTON		button;
		mouse::BUTTON_MASK	buttons;
		//system::keyboard::KeyMod keymod;
	};


	struct ST_EVENT_TEXT
	{
		ST_EVENT_TYPE		type;
		INT32				unicode;
		keyboard::MODIFIER	keymod;
	};


	struct ST_EVENT_WIN_EXPOSE
	{
		ST_EVENT_TYPE		type;
		INT32				x, y;
		INT32				sx, sy;
	};

	struct ST_EVENT_MOTION
	{
		ST_EVENT_TYPE		type;
		INT32				x, y;
		mouse::BUTTON_MASK	buttons;
		keyboard::MODIFIER	keymod;
	};

	struct ST_EVENT_MOUSE_OVER
	{
		ST_EVENT_TYPE		type;
		INT32				x, y;
	};

	struct ST_EVENT_WIN_MOVE
	{
		ST_EVENT_TYPE		type;	
		INT32				x, y;
	};

	struct ST_EVENT_WIN_RESIZE
	{
		ST_EVENT_TYPE		type;	
		INT32				sx, sy;
	};

	typedef ST_EVENT_ANY ST_EVENT_WIN_CLOSE;
	typedef ST_EVENT_ANY ST_EVENT_WIN_FOCUS;
	typedef ST_EVENT_ANY ST_EVENT_WIN_SHOW;

	struct ST_EVENT_DROP_FILE
	{
		ST_EVENT_TYPE		type;
		wchar_t**			filesname;
		INT32				numberOfFiles;
		INT32				x, y;
	};

	enum SYSTEM_EVENT_MENU_SUBTYPE : INT32
	{
		MENU_DEFAULT,
		MENU_LOOP_ENTER,
		MENU_LOOP_EXIT,
	};
	struct ST_EVENT_MENU
	{
		ST_EVENT_TYPE				type;
		SYSTEM_EVENT_MENU_SUBTYPE	subtype;
		INT32						menu_id;
		UINT32						choice;
	};

	struct ST_EVENT_DISPLAY_CHANGE
	{
		ST_EVENT_TYPE		type;
	};

	struct ST_EVENT_DEVICE_CHANGE
	{
		ST_EVENT_TYPE		type;
		bool				b_arrival;
		bool				b_remove;
		bool				b_video;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	//	Event type union
	///////////////////////////////////////////////////////////////////////////////////////////////

	union EVENT
	{
		ST_EVENT_TYPE			type;

		// System
		ST_EVENT_ANY			any;
		ST_EVENT_KEY			key;
		ST_EVENT_BUTTON			button;
		ST_EVENT_TEXT			text;
		ST_EVENT_WIN_CLOSE		close;
		ST_EVENT_WIN_EXPOSE		expose;
		ST_EVENT_WIN_FOCUS		focus;
		ST_EVENT_MOTION			motion;
		ST_EVENT_MOUSE_OVER		mouseover;
		ST_EVENT_WIN_MOVE		move;
		ST_EVENT_WIN_RESIZE		resize;
		ST_EVENT_WIN_SHOW		show;
		ST_EVENT_DROP_FILE		filedrop;
		ST_EVENT_MENU			menu;
		ST_EVENT_DISPLAY_CHANGE	display_change;
		ST_EVENT_DEVICE_CHANGE	device_change;
	};
} // namespace evt

