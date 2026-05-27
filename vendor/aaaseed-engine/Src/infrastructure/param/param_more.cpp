#include "param_more.h"
#include "param.h"
#include "infrastructure/obj/obj_ui.h"

namespace{
	UINT32	nb = 0;
	UINT32	nb_created = 0;
	void inc_count()
	{
		++nb;
		++nb_created;
	}
}

UINT32*	c_param_more::get_nb_pt()			{	return &nb;		}
UINT32*	c_param_more::get_nb_created_pt()	{	return &nb_created;	}

c_param_more::c_param_more()
	:_obj_ui_id			( 0			)
	,_flags_and_nb		( 0			)
	,_param_attached	( nullptr	)
	,_comment			( nullptr	)
	,_out				( nullptr	)
	,_in				( nullptr	)
{
	inc_count();
	//DBG_PRINT_STRING( "c_param_more size %d", sizeof(c_param_more) );
	//	printf( "size %d\n", sizeof(c_connex::LIST) );
}

c_param_more::~c_param_more()
{
	disconnect_in();
	disconnect_out();
	SAFE_DELETE( _in );
	SAFE_DELETE( _out );

	SAFE_DELETE( _comment	);

	if( nb > 0 )
		--nb;
	else
		debug_break( "%s() destructor called with param_more count already at zero", __FUNCTION__ );
}

void c_param_more::set_obj(	c_obj_ui* CONST obj )
{
	_obj_ui_id = obj->get_obj_ui_id();
}
//c_obj_ui* c_param_more::get_obj() CONST
//{
//	return c_obj_ui::get_from_id( _obj_ui_id );
//}

// CONNEX
//todo this should be moved to the list structure
void c_param_more::disconnect( c_connex::LIST* CONST ref ) CONST
{
	if( ref )
	{
		INT32 nb = ref->get_item_nb();
		if( nb )
		{
			//we store in a separate structure to avoid destruction modify structure we use to iterate 
			std::vector<c_connex*> cons;
			for(; nb>=1; )
				cons.push_back( ref->get_item(--nb) );
			//now we can remove it
			for( auto const & p : cons )
				c_connex::remove( p );
			//c_connex::remove should have removed it from the lc
			//lc->remove_all();
		}
	}
}

AAA_ERR	c_param_more::add_con( c_connex::LIST*& ref, c_connex * CONST con )
{
	if( !ref )
		//todofranz dealloc
		ref = new c_connex::LIST;
	if( ref )
	{
		ref->insert_at_tail( con );
		return AAA_OK;
	}
	return ERR_ANY;
}

void c_param_more::remove_con( c_connex::LIST* CONST ref,  c_connex * CONST con ) CONST
{
	if( ref )
		ref->remove_it( con );
}


void	c_param_more::attach_param( c_param * CONST par, INT32 CONST nb )
{
	_param_attached = par;
	if( par )
	{
#if	AAA_DEBUG()
		if( nb < 0 )
			debug_break( "%s() negative number", __FUNCTION__ );
#endif
		_flags_and_nb = (nb & NB_MASK) | (_flags_and_nb & ~NB_MASK);

	}
	else
	{
#if	AAA_DEBUG()
		if( nb != 0 )
			debug_break( "%s() number should be zero", __FUNCTION__ );
#endif
		_flags_and_nb &= ~NB_MASK;
	}
}

//todo	this work for trax of param_header get the owner should be interesting for all
// related to the issue with find_obj_by_param
c_obj_ui*	c_param_more::find_obj_owner()
{
	if( is_out() )
	{
		auto* item = _out->get_item(0);
		if( item )
			return item->get_param_obj();
	}
	if( is_in() )
	{
		auto* item = _in->get_item(0);
		if( item )
			return item->get_param_obj();
	}
	return get_obj();
}

c_obj_ui*	c_param_more::get_obj_attached() CONST
{
	return get_param_attached_nb()==1 ? _param_attached->get_obj() : nullptr;
}



void	c_param_more::clear_comment()
{
	if( _comment )
		_comment->erase();
}
void	c_param_more::set_comment( C_PCHAR_C text )
{
	if( text )
	{	//set it
		if( _comment)
			_comment->set( text );
		else
			_comment = new o_str( text );
		_comment->remove_control();
	}
	else if( _comment )	//erase if necessary
	{
		_comment->erase();
	}
}
void		c_param_more::set_comment_int32( INT32 CONST val )
{
	o_str	tmp;	//todo avoid local alloc
	tmp.set( val );
	set_comment( tmp.get() );
}
void		c_param_more::set_comment_real( REAL CONST val )
{
	CHAR	tmp[64];	//todo avoid local alloc
	snprintf( tmp, 64, "%.2f", val );
	set_comment( tmp );
}
void		c_param_more::set_comment_double( DOUBLE CONST val )
{
	CHAR	tmp[64];	//todo avoid local alloc
	snprintf( tmp, 64, "%.2f", val );
	set_comment( tmp );
}


o_str*	c_param_more::get_comment() CONST
{
	return _comment;
}
o_str*	c_param_more::get_comment_always()
{
	if( !_comment )
		_comment = new o_str( );
	return _comment;

}
C_PCHAR_C	c_param_more::get_comment_str()  CONST
{
	return _comment ? _comment->get_if_not_empty() : nullptr;
}