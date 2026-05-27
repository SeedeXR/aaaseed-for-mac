
#ifdef AAA_NODE_H
#error "NODE_H included more than once."
#endif
#define AAA_NODE_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef	AAA_AAA_LIST_H
#	include "aaa_list.h"
#endif

class	c_node;

class	c_node_more final : public c_obj
{
	friend class c_node;
private:
	c_list_pt<c_node>	_branch;
protected:
	//BRANCH	copied from c_obj_ui then changed
	FINLINE	bool		is_branch			();
	FINLINE	INT32		get_branch_nb		();
	FINLINE	c_node*		get_branch			( INT32 index );
	FINLINE	INT32		get_branch_index_of	( c_node* node );
	FINLINE	void		add_branch			( c_node* node );
	FINLINE	void 		insert_branch_before( c_node* node, INT32 index );

	//these are dangerous the object pointed is not freed
	FINLINE	c_node*		remove_branch		( c_node* node );
	FINLINE	c_node*		remove_branch_index	( INT32 index );
	FINLINE	void		remove_branch_all	();

	FINLINE	void		destroy_branch		( c_node* node );
	FINLINE	void		destroy_branch_index( INT32 index );
			void		destroy_branch_all	();
private:
public:
	c_node_more();
	virtual ~c_node_more();
};

class	c_node final : public c_obj
{
public:
			static	INT32	node_free_nb;
	FINLINE	static	c_node*	get_free( c_node* root );
			static	AAA_ERR	store_free( c_node* node );
private:
	REAL			_pos[3];
	c_node_more*	_more;
	c_node*			_root;

	FINLINE	c_node_more*	alloc_more();
	FINLINE	void			dealloc_more();

public:
	//POS
			void			set_pos( REAL x, REAL y, REAL z );
			void			set_pos( REAL* vec );
	FINLINE	REAL*			get_pos()								{	return _pos;	}

	//BRANCH
	FINLINE	bool			is_branch			();
	FINLINE	INT32			get_branch_nb		();
	FINLINE	c_node*			get_branch			( INT32 index );
	FINLINE	INT32			get_branch_index_of	( c_node* node );
			c_node*			add_branch			( c_node* node = nullptr );

			void 			insert_branch_before( c_node* node, INT32 index );

//			c_node*			remove_branch		( c_node* node );
//			c_node*			remove_branch_index	( INT32 index );
//	FINLINE	void			remove_branch_all	()					{	if( _more )	_more->remove_branch_all();		}

			void			destroy_branch		( c_node* node );
			void			destroy_branch_index( INT32 index );
			void			destroy_branch_all	();

protected:
	FINLINE	c_node_more*	get_more()			{	return _more;	}
	FINLINE c_node_more*	get_more_always();
public:
			c_node( c_node* root );
			virtual ~c_node();
};


FINLINE	bool		c_node_more::is_branch			()								{	return _branch.is_item(); } 
FINLINE	INT32		c_node_more::get_branch_nb		()								{	return _branch.get_item_nb(); }
FINLINE	c_node*		c_node_more::get_branch			( INT32 index )					{	return (index<get_branch_nb()) ? _branch.get_item(index) : nullptr; }
FINLINE	INT32		c_node_more::get_branch_index_of( c_node* node )				{	return _branch.found_index(node); }
FINLINE	void		c_node_more::add_branch			( c_node* node )				{	_branch.insert_at_tail( node );	}
FINLINE	void 		c_node_more::insert_branch_before( c_node* node, INT32 index )	{	_branch.insert_before( node, index ); }

//these are dangerous the object pointed is not freed
FINLINE	c_node*		c_node_more::remove_branch		( c_node* node )				{	return _branch.remove_it( node ); }
FINLINE	c_node*		c_node_more::remove_branch_index( INT32 index )					{	return _branch.remove( index ); }
FINLINE	void		c_node_more::remove_branch_all	()								{	_branch.remove_all(); }

FINLINE	void		c_node_more::destroy_branch		( c_node* node )				{	_branch.destroy_it( node );	}
FINLINE	void		c_node_more::destroy_branch_index( INT32 index )				{	_branch.destroy( index );	}


FINLINE	bool			c_node::is_branch			()					{	return _more ? _more->is_branch() : false; }
FINLINE	INT32			c_node::get_branch_nb		()					{	return _more ? _more->get_branch_nb() : 0; }
FINLINE	c_node*			c_node::get_branch			( INT32 index )		{	return _more ? _more->get_branch(index) : nullptr; }
FINLINE	INT32			c_node::get_branch_index_of	( c_node* node )	{	return _more ? _more->get_branch_index_of(node) : -1; }
FINLINE c_node_more*	c_node::get_more_always		()					{	return _more ? _more : alloc_more();	}

void c_list_pt<c_node>::delete_it( c_node* it );