
#ifdef AAA_SYSTEMVIEW_AAA_H
#error "SYSTEMVIEW_AAA_H included more than once."
#endif
#define AAA_SYSTEMVIEW_AAA_H 1


#ifndef AAA_SYSTEM_VIEW_H
#	include "SystemView.h"
#endif

namespace core { class AAA_controller; }

/**
* @class system_view_AAA
*
* AAASeed dedicated view
*/
class system_view_AAA final
	: public system_view
{
protected:
	core::AAA_controller*			_controller;

protected:
	C_NO_CPY_MOVE(system_view_AAA)
	/** system_view_AAA class constructor. */
	system_view_AAA(							int32_t	x = X_DEF,	int32_t	y = Y_DEF,	uint32_t sx = SX_DEF,	uint32_t sy = SY_DEF	);
	/** system_view_AAA class destructor */
	virtual ~system_view_AAA( void );

public:		
	/** \! Create, init and return new system_view pointer, do not start thread. */
	static system_view_AAA* create_ptr(			int32_t	x = X_DEF,	int32_t	y = Y_DEF,	uint32_t sx = SX_DEF,	uint32_t sy = SY_DEF	);
	/** \! Create, init and return new system_view pointer once its thread is started. */
	static system_view_AAA* create_ptr_wait(	int32_t	x = X_DEF,	int32_t	y = Y_DEF,	uint32_t sx = SX_DEF,	uint32_t sy = SY_DEF	);

protected:
	/** Thread initialization function, init members inside thread run. */
	virtual void init( void );	
	/** Thread release function, release members inside thread run before exit. */
	virtual void uninit( void );
};

