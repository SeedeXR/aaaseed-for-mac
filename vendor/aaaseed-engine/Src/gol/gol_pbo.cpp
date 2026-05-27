#include "gol/gol_pbo.h"
#include "aaa_mem.h"
#include "err.h"


namespace GOL	{

		bool	b_pbo_can = false;
static	UINT32	pbo_nb;
		UINT32	pbo_free_nb_out;
		bool	b_pbo_use_several_ui = false;
static	GLuint*	p_pbo_buf;
static	bool*	p_b_pbo_buf_free;
static	UINT64*	p_pbo_buf_size;
		DOUBLE	pbo_size_mb_out = 0.;
static	UINT64	pbo_size = 0;
		bool	b_pbo_free_dealloc_ui;	//	param in texture_master
//		INT32	pbo_max_size = 0;

void	pbo_init()
{
	b_pbo_can = test_version_or_extensions( 2,1, "GL_ARB_pixel_buffer_object", "GL_EXT_pixel_buffer_object" );
	pbo_nb = 0;
	pbo_free_nb_out = 0;
	p_pbo_buf = nullptr;
	p_b_pbo_buf_free = nullptr;
	p_pbo_buf_size = nullptr;
}

void	pbo_deinit()
{
	IF_FREE_AND_NULL( p_pbo_buf );
	IF_FREE_AND_NULL( p_b_pbo_buf_free );
	IF_FREE_AND_NULL( p_pbo_buf_size );
	pbo_nb = 0;
	pbo_free_nb_out = 0;
}

void	pbo_dealloc_buf()
{
	if( b_pbo_can )
	{
		if( !b_pbo_free_dealloc_ui )
			delete_buffers( pbo_nb, p_pbo_buf );
		else
		{
			for( UINT32 i = 0; i < pbo_nb; ++i )
				pbo_dealloc( i );
		}
		pbo_deinit();
	}
}

//todo we never free
void	pbo_alloc_buf( UINT32 CONST nb )
{
	if( b_pbo_can )
	{
		pbo_deinit();
		p_pbo_buf			= ( GLuint* )	MALLOC( nb * sizeof( GLuint ) );
		p_b_pbo_buf_free	= ( bool* )		MALLOC( nb * sizeof( bool ) );
		p_pbo_buf_size		= ( UINT64* )	MALLOC( nb * sizeof( UINT64 ) );
		if( p_pbo_buf && p_b_pbo_buf_free && p_pbo_buf_size )
		{
			for( UINT32 i = 0; i < nb; ++i )
			{
				p_pbo_buf_size[ i ] = 0;
				p_b_pbo_buf_free[ i ] = false;
				p_pbo_buf[ i ] = 0;
			}
			get_error( "cleaning error before pbo gen buffer" );
			gen_buffers( nb, p_pbo_buf );

			INT32 err = get_error( __FUNCTION__ );
			if( err == GL_NO_ERROR )
			{
				pbo_nb = nb;
				for( UINT32 i = 0; i < pbo_nb; ++i )
					p_b_pbo_buf_free[i] = true;
				GOOD_PRINT_STRING( "OGL %d PBO buffers allocated", pbo_nb );
			}
			else
			{
				CHAR str_pb[] = "OGL Can't allocate the PBO buffers";
				debug_break( str_pb );
				ERR_PRINT_STRING( str_pb );
			}	
		}
		else
		{
			debug_break( "can't allocate pbo data needed, will not disbled pbo for this run" );
			b_pbo_can = false;
			pbo_nb = 0;
		}
	}
	pbo_free_nb_out = pbo_nb;
}

void pbo_error_cant( C_PCHAR_C function_name )
{
	debug_break( "%s() when b_pbo_can is false", function_name );
}

static	INT32	pbo_bind_write_cur = 0;
static	INT32	pbo_bind_read_cur = 0;
bool	pbo_do_bind( INT32 CONST index, bool CONST b_write )
{
	if( !b_pbo_can )
	{
		pbo_error_cant( __FUNCTION__ );
		return false;
	}

	INT32	bind = ( index < 0 ) ? 0 : p_pbo_buf[ index ];
	if( b_write )
	{
		if( pbo_bind_write_cur != bind )
		{
			check_error( "before bind_buffer GL_PIXEL_UNPACK_BUFFER" );
			bind_buffer( GL_PIXEL_UNPACK_BUFFER, bind );	//todonow do a bind to avoid interleaved binding
			pbo_bind_write_cur = bind;
			check_error( "after bind_buffer GL_PIXEL_UNPACK_BUFFER" );
		}
	}
	else
	{
		if( pbo_bind_read_cur != bind )
		{
			check_error( "before bind_buffer GL_PIXEL_PACK_BUFFER" );
			bind_buffer( GL_PIXEL_PACK_BUFFER, bind );	//todonow do a bind to avoid interleaved binding
			pbo_bind_read_cur = bind;
			check_error( "after bind_buffer GL_PIXEL_PACK_BUFFER" );
		}
	}
	return true;
}

void	pbo_do_data( UINT64 CONST size, bool CONST b_write )
{
	if( !b_pbo_can )
	{
		pbo_error_cant( __FUNCTION__ );
		return;
	}

	set_buffer_data( b_write ? GL_PIXEL_UNPACK_BUFFER : GL_PIXEL_PACK_BUFFER, size, nullptr, b_write ? GL_DYNAMIC_DRAW : GL_DYNAMIC_READ ); //GL_STREAM_DRAW : GL_STREAM_READ );
	check_error( "after glBufferData" );
}

void*	pbo_do_map( bool CONST b_write )
{
	if( !b_pbo_can )
	{
		pbo_error_cant( __FUNCTION__ );
		return nullptr;
	}

	check_error( "before glMapBuffer" );
		void*	data = map_buffer( b_write ? GL_PIXEL_UNPACK_BUFFER : GL_PIXEL_PACK_BUFFER, b_write ? GL_WRITE_ONLY : GL_READ_ONLY );
	//	PRINT_STRING( "glMapBuffer() return %8x\n", data );
	check_error( "after glMapBuffer" );
	return data;
}

void*	pbo_do_map_range( UINT64 CONST size, bool CONST b_write )
{
	if( !b_pbo_can )
	{
		pbo_error_cant( __FUNCTION__ );
		return nullptr;
	}
			
	check_error( "before glMapBufferRange" );

	GLbitfield access;
	if( b_write )
	{
		access = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
	}
	else
	{
		access = GL_MAP_READ_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
	}
	//			case GL_READ_WRITE:
	//				access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;

	void*	data = (GLubyte*)map_buffer_range( b_write ? GL_PIXEL_UNPACK_BUFFER : GL_PIXEL_PACK_BUFFER, 0, size, access );

	check_error( "after glMapBufferRange" );
	return data;

}
void	pbo_do_unmap( bool CONST b_write )
{
	if( !b_pbo_can )
	{
		pbo_error_cant( __FUNCTION__ );
		return;
	}

	check_error( "before glUnmapBuffer" );
	if( !unmap_buffer( b_write ? GL_PIXEL_UNPACK_BUFFER : GL_PIXEL_PACK_BUFFER ) )
	{
		ERR_PRINT_STRING( "%s() the data store contents have become corrupt during the time the data store was mapped", __FUNCTION__ );
	}
	check_error( "after glUnmapBuffer" );

}

namespace {
	//recalculate pbo size
	//todo avoid to recompute all at every change
	void	pbo_update_size_total()
	{	
		pbo_size = 0;
		for( UINT32 i = 0; i < pbo_nb; ++i )
			pbo_size += p_pbo_buf_size[ i ];
		pbo_size_mb_out = pbo_size / DOUBLE( 1024 * 1024 );
	}
}

void	pbo_dealloc( INT32 CONST index )
{
	if( !b_pbo_can )
	{
		pbo_error_cant( __FUNCTION__ );
		return;
	}

	if( index >= 0 )
	{
		if( !p_b_pbo_buf_free[ index ] )
		{
			if( b_pbo_free_dealloc_ui )
			{
				delete_buffer( p_pbo_buf[ index ] );
				p_pbo_buf_size[ index ] = 0;
				pbo_update_size_total();
			}
			p_b_pbo_buf_free[ index ] = true;
			++pbo_free_nb_out;
		}
	}

}

INT32	pbo_alloc( INT32 CONST index_in, UINT64 CONST size, bool CONST b_write )
{
	if( !b_pbo_can )
	{
		pbo_error_cant( __FUNCTION__ );
		return -1;
	}

	INT32 pbo_buf_index = index_in;
	if( pbo_buf_index < 0 )
	{		
		if( !b_pbo_use_several_ui )
			pbo_buf_index = 0; // if only one we use 0
		else
		{
			for( UINT32 i = 0; i < pbo_nb; ++i )
			{
				if( p_b_pbo_buf_free[i] )
				{
					p_b_pbo_buf_free[i] = false;
					--pbo_free_nb_out;
					pbo_buf_index = i;
					break;
				}
			}
		}
	}
	if( pbo_buf_index < 0 )
	{
		ERR_PRINT_STRING( "PBO no more, can't alloc()" );
		return -1;	//todonow bad no retcode...
	}

	if( b_pbo_free_dealloc_ui && p_pbo_buf[pbo_buf_index] == 0 )
	{
		// have to regen buffer
		check_error( "cleaning error before pbo gen buffer" );
		gen_buffer( p_pbo_buf[pbo_buf_index] );
		check_error( __FUNCTION__ );
	}
	if( size > p_pbo_buf_size[pbo_buf_index] )
	{
		if( pbo_do_bind( pbo_buf_index, b_write ) )
		{
			pbo_do_data( size, b_write );
			p_pbo_buf_size[ pbo_buf_index ] = size;
			GOOD_PRINT_STRING( "OGL PBO %s buffers( %d ) %d bytes allocated", b_write ? "write" : "read", pbo_buf_index, size );
			pbo_do_bind( -1, b_write ); //was true );	//was b_write );
			pbo_update_size_total();
		}
		else
		{
			ERR_PRINT_STRING( "OGL PBO could not bind so can't alloc" );
		}
	}
	return pbo_buf_index;
}

INT32	pbo_move_to( void CONST * CONST src, INT32 CONST size )
{
	if( !b_pbo_can )
	{
		pbo_error_cant( __FUNCTION__ );
		return 0;
	}

	INT32 offset;
	//	offset = b_buf_flip?0:pbo_max_size;
	offset = 0;
	set_buffer_data_sub( GL_PIXEL_UNPACK_BUFFER, offset, size, src );
	return offset;
}

}	//namespace GOL
