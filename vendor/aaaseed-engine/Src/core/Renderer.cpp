
#include "core/Renderer.h"

void core::renderer::release_ptr( core::renderer* p )
{
	if( !p )
	{
		debug_break( "%s() null ptr", __FUNCTION__ );
		return;
	}
	p->release();
	delete p;
}

core::renderer::renderer( void )	{}
core::renderer::~renderer( void )	{}
