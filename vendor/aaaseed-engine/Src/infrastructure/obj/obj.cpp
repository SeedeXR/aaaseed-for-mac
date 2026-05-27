#include "obj.h"
#include "aaa_mem.h"
#include "err.h"
#include <list>

UINT32	c_obj::obj_nb_created = 0;
UINT32	c_obj::obj_nb = 0;

c_obj::c_obj()
{	
	++obj_nb;
	++obj_nb_created;
#if AAA_OBJ_USE_ID()
	_obj_id = obj_nb_created;
#endif
}

c_obj::~c_obj()
{
#if AAA_DEBUG()
	if( obj_nb == 0 )
		debug_break( "the count of obj is already 0 when trying to destruct one" );
	else
#endif
		--obj_nb;
//	DBG_HEAP_CHECK();
}
