#include "node.h"
#include "math/v_base.h"

//NODE_MORE
//
//	NODE data is kept as minimal as possible, so basic node will be fast anbd light
//		when a NODE became more complex it use a MODE_MORE to extend is data

c_node_more::c_node_more()
//:_branch(nullptr)
{
}

c_node_more::~c_node_more()
{
//	SAFE_DELETE( _branch );
}

void c_node_more::destroy_branch_all()
{
	_branch.destroy_all();
}

void c_list_pt<c_node>::delete_it( c_node* it )
{
	c_node::store_free(it);
}

//
//	NODE
//

static	std::vector<c_node*>	node_stock;
INT32	c_node::node_free_nb = 0;
FINLINE	c_node*	c_node::get_free( c_node* root )
{
	c_node*	ret;
	if( node_stock.empty() )
		ret = new c_node(root);
	else
	{
		ret = node_stock.back();
		node_stock.pop_back();
		--node_free_nb;
	}
	ret->_root = root;
	return ret; 
}

AAA_ERR	c_node::store_free( c_node* node )
{
	node->destroy_branch_all();
	node_stock.push_back( node );
	++node_free_nb;
	return AAA_OK;
}

c_node_more*	c_node::alloc_more()
{
	return _more = new c_node_more;
}

void	c_node::dealloc_more()
{
	destroy_branch_all();
	SAFE_DELETE( _more );
}

c_node::c_node(c_node* root)
	:_more	(	nullptr	)
	,_root	(	root	)
{
	clear_v3( _pos );
}

c_node::~c_node()
{
	if( get_more() )
		dealloc_more();
}

//POS
void	c_node::set_pos( REAL x, REAL y, REAL z )	{	set_v3( _pos, x, y, z );	}
void	c_node::set_pos( REAL* vec )				{	cpy_v3( _pos, vec );	}

//BRANCH
c_node*		c_node::add_branch( c_node* node )
{
	c_node_more* more = get_more_always();	//todo makeit secure
	if( node )
		node->_root = this;
	else
		node = get_free( this );
	more->add_branch( node );
	return node;
}

void 		c_node::insert_branch_before( c_node* node, INT32 index )
{
	c_node_more* more = get_more_always();
	node->_root = this;
	more->insert_branch_before( node, index );
}

/*
c_node*	c_node::remove_branch( c_node* node )
{
	if( _more )
		return _more->remove_branch(node);
	BOX_ERR("Can't remove branch because no branch table in c_node" );
	return nullptr;
}

c_node*	c_node::remove_branch_index( INT32 index )
{
	if( _more )
		return _more->remove_branch_index( index );
	BOX_ERR("Can't remove branch because no branch table in c_node" );
	return nullptr;
}
*/

void	c_node::destroy_branch( c_node* node )
{
	if( _more )
		_more->destroy_branch( node );
	else
		BOX_ERR( "Can't destroy branch because no branch table in c_node" );
}

void	c_node::destroy_branch_index( INT32 index )
{
	if( _more )
		_more->destroy_branch_index( index );
	else
		BOX_ERR( "Can't destroy branch because no branch table in c_node" );
}

void	c_node::destroy_branch_all()
{
	if( _more )
		_more->destroy_branch_all();
}
