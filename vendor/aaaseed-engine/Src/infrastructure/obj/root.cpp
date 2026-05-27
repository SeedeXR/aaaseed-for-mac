#include "infrastructure/obj/root.h"
#include "infrastructure/namer.h"
#include "gol/gol_matrix.h"
#include "draw/shape.h"

FACTORY_CREATE_V1( c_node_ui, node_ui, Node for obj_ui, node_ui );

C_PCHAR_C c_node_ui::str_name_def = "Node_ui";

CONSTRUCTOR_CREATE(c_node_ui)
{
	set_name( str_name_def );
	param_init_with( (c_param_def*)nullptr, 0 );
}
EMPTY_DESTRUCTOR(c_node_ui)

void c_node_ui::cell_draw_obj( REAL CONST size )
{
	GOL::matrix::scale_z( .125 );
		draw_circle_x( GL_LINE_LOOP, 6, size*REAL(.5) );
		draw_circle_y( GL_LINE_LOOP, 6, size*REAL(.5) );
		draw_circle_z( GL_LINE_LOOP, 6, size*REAL(.5) );
	GOL::matrix::scale_z( 8. );
}


FACTORY_CREATE_V1( c_root, root, Root of all obj, root );

namespace {
	bool	b_single_root_check = false;
}

//todo totally unclear I have to describe
CONSTRUCTOR_CREATE(c_root)
	,_father_cur	(	nullptr	)
	,_node_global	(	nullptr	)
{
	if( b_single_root_check )	//	this is overkill should never happen
		debug_break( "problem in %s() c_root should not be instanced twice", __FUNCTION__ );

	set_name( "Root" );
	param_init_with( (c_param_def*)nullptr, 0 );
	set_root( nullptr );

//	c_root will be the root
	set_father( this );
	//factory not initialized can't
	//	(c_node_ui*) c_factory_base::create_by_cid( "node_ui" );
	_node_global = new c_node_ui;
	_node_global->set_name( "node_global" );
	//	node_global is the root for now
	set_father( _node_global );
	b_single_root_check = true;
}
c_root::~c_root()
{
//	obj_delete( _node_global );
}


void	do_enter_in_main()
{
	auto the_root = c_obj_ui::get_the_root();
	c_namer::set_dir_and_file( the_root->get_node_global(), "pref", "default" );
	c_namer::set_namer_start( the_root->get_node_global() );
#if AAA_DEBUG()
	DBG_PRINT_STRING( "checking orphan at the beginning of main()" );
#endif
	c_obj_ui::assign_orphan();
	the_root->set_father( the_root->get_node_global() );
}