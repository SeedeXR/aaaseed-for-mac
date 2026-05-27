#include "obj_ui/deformer/deformer.h"

//todo add a REAL value to control the amount of deformation like in c_def_tocylinder
FACTORY_ABSTRACT_CREATE_V1( c_deformer, deformer, Deformer );

CONSTRUCTOR_ABSTRACT_CREATE(c_deformer)
,_b_add(true)
//,_nb(0)
{	
	set_active( false );	
}

EMPTY_DESTRUCTOR( c_deformer )

INT32	c_deformer::param_init_pt_start()
{
	INT32		h=0;
	//par	this should be done at init only
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, get_name_symbo() );

	return h;
}
