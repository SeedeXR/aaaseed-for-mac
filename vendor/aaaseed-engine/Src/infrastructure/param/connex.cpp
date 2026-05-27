#include "connex.h"
#include "infrastructure/param/trax.h"
#include "draw/seeddraw.h"
#include "draw/line.h"


static	c_connex::LIST	trax_connex;

bool	c_connex::b_verbose;

c_param*	c_connex::get_in_param()
{
	return _b_trax_out ? _param						: _trax->get_param_header() ;
}
c_param*	c_connex::get_out_param()
{
	return _b_trax_out ? _trax->get_param_header()	: _param ;
}

AAA_ERR		c_connex::plug()
{
	if( b_verbose)
		VERBOSE_PRINT_STRING( "%s()", __FUNCTION__ );
	if( get_out_obj()->add_in_connex( this ) >= 0 )
	{
		if( get_in_obj()->add_out_connex( this ) >= 0 )
		{
			if( _b_trax_out )
				_param->add_out( this );
			else
				_param->add_in( this );
			trax_connex.insert_at_tail( this );
			return AAA_OK;
		}
		else
			BOX_ERR( "c_connex::plug() Could not connect out" );
		get_out_obj()->remove_in_connex( this );
	}
	else
		BOX_ERR( "c_connex::plug() Could not connect in" );
	return ERR_ANY;
}

c_connex::c_connex( c_obj_ui* o_in,	p_param p_in,
					c_obj_ui* o_out, p_param p_out )
{
	if( p_in )
	{
		_b_trax_out = true;
		_param = p_in;
		_param_obj = o_in;
		_trax = (c_trax *)o_out;
	}
	else
	{
		_b_trax_out = false;
		_param = p_out;
		_param_obj = o_out;
		_trax = (c_trax *)o_in;
	}
	plug();	//todo check and throw error
}

c_connex::~c_connex()
{
#if AAA_DEBUG()
	if( !this )
		debug_break( "Can't remove NULL connection");
	else
#endif
//it crash because it modified struct we iterate on at a calling level (in trax)
	{ 
	//	unplug();
	//	trax_connex.remove_it(this);
	}
}

void		c_connex::unplug()	//	make sure it symetrical to plug()
{
	if( b_verbose)
		VERBOSE_PRINT_STRING( "%s()", __FUNCTION__ );

	trax_connex.remove_it(this);
	get_out_obj()->remove_in_connex( this );
	get_in_obj()->remove_out_connex( this );
	if( _b_trax_out )
		_param->remove_out( this );
	else
		_param->remove_in( this );
}


//STATIC
AAA_ERR c_connex::add(	c_obj_ui* o_in,	p_param p_in,
						c_obj_ui* o_out, p_param p_out )
{
#if	AAA_DEBUG()
	if( !o_in || !o_out )
		debug_break( "%s() try to add a fucked up connection", __FUNCTION__ );
#endif
	try
	{
		c_connex*	con = new c_connex( o_in, p_in, o_out, p_out );
	}
	catch( std::bad_alloc e )
	{
		BOX_ERR( "Can't allocate new connex because :\n%256s", e.what() );
		//todo we are alredy in limit condition add something but the dialog should break too
		return ERR_MEM_BASE;
	}
	return AAA_OK;
}

AAA_ERR	c_connex::remove( c_connex* con )
{
	if( con )
	{
		con->unplug();
		delete con;	//recycle
		return AAA_OK;
	}
	else
		BOX_ERR( "Can't remove NULL connection");
	return ERR_ANY;
}

void	c_connex::draw()
{
	INT32 rf = draw::get_frame();
	for( auto const & con : trax_connex.get_container() )
	{
		c_obj_ui* CONST in = con->get_in_obj();
		if(	in->get_cell_render_frame() == rf )	//in->is_active() )
		{
			c_obj_ui* CONST	out = con->get_out_obj();
			if( out->get_cell_render_frame() == rf )
				draw_line( in->get_cell_pos(), out->get_cell_pos() );
		}
	}
}

/*
AAA_ERR		c_connex::plug()
{
	if(this)
	{
		if( get_out_param()->add_in( this ) >= 0)
		{
			if( get_in_param()->add_out( this ) >= 0 )
			{
				trax_connex.insert_at_tail( this);
				return AAA_OK;
			}
			else
		BOX_ERR( "c_connex::plug() Could not connect out");
	//	get_out_param()->remove_in( this );
		}
		else
			BOX_ERR( "c_connex::plug() Could not connect in");
	}
	else
		BOX_ERR( "c_connex::plug() Can't add NULL connection");
	return ERR_ANY;
}
*/

//inline void	LIST::delete_it( c_connex* p )
//{
//	c_connex::remove(p);
//}
//
//inline void	LIST::destroy_all()
//{
//	IF_THIS_NULL_RETURN();
//	if( !_pt.empty() )
//	{
//		//done this way because the fn_del have to remove vector element
//		while( !_pt.empty() )
//			c_connex::remove( _pt.back() );
//		remove_all();
//	}
//}
