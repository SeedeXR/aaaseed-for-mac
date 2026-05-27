
#include "core/Controller.h"


void core::Controller::release_ptr( core::Controller * pPtr )
{
	if( !pPtr )
	{
		debug_break( "%s() null ptr", __FUNCTION__ );
		return;
	}

	pPtr->unregister_to_events();
	pPtr->release();
	delete pPtr;
}


core::Controller::Controller( void )	{}
core::Controller::~Controller( void )	{}
