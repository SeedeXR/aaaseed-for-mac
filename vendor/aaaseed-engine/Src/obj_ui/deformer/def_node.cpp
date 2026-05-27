#include "def_node.h"
#include "math/v.h"
#include "spy.h"
#include "infrastructure/compute_parallel.h"

FACTORY_ABSTRACT_CREATE_V1(	c_node_list_ui<c_deformer>, node_list_ui<deformer>, Node List );
FACTORY_CREATE_PROP_V1(		c_def_node, def_node, Deformer Node, dfl, param_hook="def"; );


namespace {
	REAL*		buf			= nullptr;
	REAL*		buf2		= nullptr;
	REAL*		buf_stride	= nullptr;
	INT32		buf_nb		= 0;
}

c_def_node*	c_def_node::cur			= nullptr;
c_def_node*	c_def_node::prev		= nullptr;
c_def_node*	c_def_node::ui			= nullptr;

C_PCHAR_C	c_def_node::field_type_str[FIELD_TYPE_MAX_NB] =
{
	"No",
	"Current",
	"Previous"
};

void c_def_node::c_init()
{
//todo check that no more def don't provoke crash
//	ui = prev = cur = def = new c_def_node;
}


c_def_node*	c_def_node::get_from_field_type( FIELD_TYPE field_type )
{
	switch( field_type )
	{
	default:					ERR_PRINT_STRING( "Unknown field type %d", field_type );
	case FIELD_TYPE_NO:			return nullptr;
	case FIELD_TYPE_CURRENT:	return get_cur();
	case FIELD_TYPE_PREVIOUS:	return get_prev();
	}
}

//o_str	c_node_list_ui<c_deformer>::base_name = o_str("def");	
//o_str	c_node_list_ui<c_deformer>::class_name;	

CONSTRUCTOR_CREATE(c_def_node)
{
	build_param();
}
//todo dealloc buffer on last instance
c_def_node::~c_def_node()
{
	if( cur  == this )
		cur	 = nullptr;
	if( prev == this )
		prev = nullptr;
	if( ui   == this )
		ui   = nullptr;
	//hack	don't really do the job
	remove_and_destroy_all();
//todo make it faster
//	remove_branch_all();
	if( is_obj_first() )
	{
		FREE_ALIGNED_AND_NULL( buf );
		buf_nb		= 0;
		buf2		= nullptr;
		buf_stride	= nullptr;
//		obj_delete( def	);
	}
}

INT32	c_def_node::get_param_extra_nb()
{
	return 2;
}
c_param_def* c_def_node::build_param_extra( c_param_def* param )
{
	//	initialize active param
	param++->set_all( TYPE_BOOL, "src_fixed", 1., 0., 0., 1. );	//par
	param++->set_all( TYPE_BOOL, "parallel",  0., 1., 0., 1. );	//par
	return param;
}
void	c_def_node::param_init_pt_extra( INT32& h )
{
	param_set_pt( h, _b_src_fixed );
	param_set_pt( h, _b_parallel_ui );
}

void	c_def_node::update()
{
	if( is_active() )
	{
		_defs.clear();
		INT32 nb = get_branch_nb();
		for( INT32 i=0; i<nb; ++i )
		{
			c_deformer* def = (c_deformer*)get_branch_active(i);
			if( def )
			{
				if( def->is_active() )
				{
					def->update();
					if( def->is_deforming() )
						_defs.push_back(def); 
				}
			}
		}
		set_deforming( !_defs.empty() );
	}
	else
	{
		set_deforming( false );
	}
	set_cur( this );
}

void	c_def_node::alloc_data( INT32 nb )
{
	if( buf_nb >= nb )
		return;

	INT32	nb_byte = 3 * nb * 3 * sizeof(REAL);	// 3 buffers of nb point (3 REAL)
	buf = (REAL*) REALLOC_ALIGNED( buf, nb_byte );
	if( !buf )
	{
		err_print( "deformer Can't allocate %d Bytes", nb_byte );
		buf_nb = 0;
		buf2 = nullptr;
		buf_stride = nullptr;
		return;
	}

	buf_nb = nb;
	buf2 = buf + 3 * nb; 
	buf_stride = buf2 + 3 * nb;
}

void	c_def_node::apply( REAL* dst, REAL CONST * src, INT32 nb )
{
	if( !is_deforming() )
		return;
	if( is_src_fixed() )
	{
		alloc_data( nb );
		if( !buf  )
			return;
	}

	SPY_PUSH_RANGE( "apply", spy::COL_4 );

		auto CONST it_end	= _defs.end() ;
		if( nb>=256 && _b_parallel_ui && aaa::parallel::is_use() )
		{
			INT32 threads_nb = INT32( aaa::parallel::get_thread_nb_def() ) / 2;
			INT32 CONST nb_by_thread = nb / threads_nb;
			PARALLEL_LIB::parallel_for( 0, threads_nb, [&]( std::size_t CONST i ) NOEXCEPT
			{
				auto p_src = src + i * nb_by_thread * 3;
				auto p_dst = dst + i * nb_by_thread * 3;
				INT32 nbt = nb_by_thread;
				if ( i == ( threads_nb - 1 ) )
					nbt = nb - nb_by_thread * ( threads_nb - 1 );

				auto it	= _defs.begin();
				if( is_src_fixed() )
				{
					if( it != it_end )
					{
						auto p_buf = buf + i * nb_by_thread * 3;
						(*it)->apply( p_src, p_dst, nbt );
						while( ++it != it_end )
						{
							(*it)->apply( p_src, p_buf, nbt );
							sub_then_add_to_array_v3r( p_dst, p_buf, p_src, nbt );
						}
					}
				}
				else
				{		
					(*it)->apply( p_src, p_dst, nbt );		
					while( ++it != it_end )
						(*it)->apply( p_dst, p_dst, nbt );	
				}
			} );
		}
		else
		{
			auto it	= _defs.begin();
			if( is_src_fixed() )
			{
				if( it != it_end )
				{
					(*it)->apply( src, dst, nb );
					while( ++it != it_end )
					{
						(*it)->apply( src, buf, nb );
						sub_then_add_to_array_v3r( dst, buf, src, nb );
					}
				}
			}
			else
			{		
				(*it)->apply( src, dst, nb );		
				while( ++it != it_end )
					(*it)->apply( dst, dst, nb );	
			}
		}

	SPY_POP_RANGE();
}


void	c_def_node::apply( REAL* dst, INT32 nb )
{
	if( !is_deforming() )	//todo	put all those inline
		return;

	SPY_PUSH_RANGE( "apply dst", spy::COL_4 );

		if( is_src_fixed() )
		{
			alloc_data( nb );
			if( buf )
			{
				//	optimize this case
				MEMCPY( buf2, dst, (size_t) (sizeof(REAL) * 3 * nb), __FUNCTION__ );
				apply( dst, buf2, nb );
			}
		}
		else
		{
			for( auto const & b : _defs )
				b->apply( dst, dst, nb );
		}

	SPY_POP_RANGE();
}

void	c_def_node::apply( REAL* dst, INT32 dst_stride, REAL* src, INT32 src_stride, INT32 nb )
{
	if( !is_deforming() )
		return;

	SPY_PUSH_RANGE( "apply stride", spy::COL_4 );

		alloc_data( nb );
		if( buf_stride )
		{
			//fill the buffer
			cpy_from_with_stride_v3r( buf_stride, src, src_stride, nb );	//todo used already in apply when src_fixxed
			//then apply
			apply( buf_stride, nb );
			//then put it back in place
			cpy_to_with_stride_v3r( dst, dst_stride, buf_stride, nb );
		}

	SPY_POP_RANGE();
}

void	c_def_node::get_field( REAL* dst, REAL CONST * src, INT32 nb )
{
	if( !is_deforming() )
		return;

	SPY_PUSH_RANGE( "get_field", spy::COL_4 );

		if( is_src_fixed() )
		{
			apply( dst, src, nb );
		}
		else
		{
			MEMCPY( dst, src, (size_t) (sizeof(REAL) * 3 * nb), __FUNCTION__ );
			apply( dst, nb );
		}
		sub_array_v3r( dst, src, nb );

	SPY_POP_RANGE();
}
