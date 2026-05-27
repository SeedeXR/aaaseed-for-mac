#include "img_lua.h"
#include "img.h"
#include "language/lua/aaalua_glue.h"
#include "image/bind_img_2d.h"
#include "image/bind_img_3d.h"
#include "draw/seeddraw.h"
#include "draw/tex.h"
#include "image/bind_img.h"


namespace aaalua
{

namespace n_img
{

template < class C_IMG >
INT32 read_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind, bool CONST b_error  )
{
	UINT32 CONST nb_arg			= l.get_arg_nb_min_max( 2, 6 );
	INT32  CONST bind			= l.get_int32( 1 );
	C_PCHAR_C	filename		= l.get_str( 2 );
	bool CONST	b_free			= ( nb_arg >= 3 ) ? l.get_bool(3) : false;
	bool CONST	b_async			= ( nb_arg >= 4 ) ? l.get_bool(4) : false;
	bool CONST	b_force_keep	= ( nb_arg >= 5 ) ? l.get_bool(5) : false;
	bool CONST	b_premultiply	= ( nb_arg >= 6 ) ? l.get_bool(6) : false;

	if( filename )
	{
		AAA_ERR ret = ERR_ANY;
#if 0
		if( b_async )
		{
			C_IMG* img = g_bind->get_always( bind );
			if( img )	
			{
				img->lock();
				img->set_reading( true );
				img->unlock();
			}
			ret = g_bind->ask_texture_async( bind, filename, b_free, b_force_keep, b_premultiply );
		}
		else
			ret = g_bind->load_texture( bind, filename, false, b_free, b_force_keep, b_premultiply );
#else
		ret = g_bind->load_texture( bind, filename, b_async, b_free, b_force_keep, b_premultiply );
#endif
		if( ERR( ret ) )
		{	//todo refine the error handling
			if( b_error )
				l.error_message( "can't read image %s in bind %d", filename, bind );
		}
		else
			return l.return_bool(true);
	}
	return l.return_nothing();
}

AAALUACALL( read )					{	LUAAAA_START( L, __FUNCTION__ );	return read_nd<c_img_2d>( l, g_bind_img_2d, true );		}
AAALUACALL( read_no_error )			{	LUAAAA_START( L, __FUNCTION__ );	return read_nd<c_img_2d>( l, g_bind_img_2d, false );	}

AAALUACALL( read_3d )				{	LUAAAA_START( L, __FUNCTION__ );	return read_nd<c_img_3d>( l, g_bind_img_3d, true );		}
AAALUACALL( read_no_error_3d )		{	LUAAAA_START( L, __FUNCTION__ );	return read_nd<c_img_3d>( l, g_bind_img_3d, false );	}

template < class C_IMG >
INT32 is_ready_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind )
{
	DBG_CHECK_ARG_NB( 1 );
	INT32 CONST bind	= l.get_int32( 1 );

	bool b = false;
	C_IMG* CONST img	= g_bind->get( bind );
	if( img )
	{
		if( img->try_lock() )
		{
			b = !img->is_reading();
			img->unlock(); 
		}
	}
	return l.return_bool( b );
}
AAALUACALL( is_ready )				{	LUAAAA_START( L, __FUNCTION__ );	return is_ready_nd<c_img_2d>( l, g_bind_img_2d );	}
AAALUACALL( is_ready_3d )			{	LUAAAA_START( L, __FUNCTION__ );	return is_ready_nd<c_img_3d>( l, g_bind_img_3d );	}

// return bind index for a given image filename
template < class C_IMG >
INT32 get_bind_index_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind )
{
	DBG_CHECK_ARG_NB( 1 );
	if( C_PCHAR_C filename = l.get_str() )
	{
		INT32 CONST index = g_bind->get_index_from_filename( filename );
		if ( index != -1 ) 
			return l.return_int32( index );
	}
	return l.return_nothing();
}
AAALUACALL( get_bind_index )		{	LUAAAA_START( L, __FUNCTION__ );	return get_bind_index_nd<c_img_2d>( l, g_bind_img_2d );	}
AAALUACALL( get_bind_index_3d )		{	LUAAAA_START( L, __FUNCTION__ );	return get_bind_index_nd<c_img_3d>( l, g_bind_img_3d );	}

AAALUACALL( make_tex_index )
{
	LUAAAA_START( L, __FUNCTION__ );

	DBG_CHECK_ARG_NB( 2 );
	INT32 CONST bank	= l.get_int32( 1 );
	INT32 CONST bind	= l.get_int32( 2 );
	return l.return_int32( g_bind_img_2d->make_tex_index( bank, bind ) );
}

// return filename for a given index
template < class C_IMG >
INT32 get_bind_filename_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind )
{
	DBG_CHECK_ARG_NB( 1 );
	INT32 CONST index	= l.get_int32( 1 );
	//todo will return something even with bad index
	o_str& fname = o_str::push_name();
		fname.compact_fname(  g_bind->get_o_filename_for_index(index) );
		l.push_string( fname );
	o_str::pop_name();
	return 1;
}
AAALUACALL( get_bind_filename )		{	LUAAAA_START( L, __FUNCTION__ );	return get_bind_filename_nd<c_img_2d>( l, g_bind_img_2d );	}
AAALUACALL( get_bind_filename_3d )	{	LUAAAA_START( L, __FUNCTION__ );	return get_bind_filename_nd<c_img_3d>( l, g_bind_img_3d );	}

// return filename for a given index
template < class C_IMG >
INT32 set_bind_filename_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind )
{
	INT32 CONST arg_nb = l.get_arg_nb( 1,2 );
	INT32 CONST index  = l.get_int32( 1 );
	C_PCHAR fname_in = arg_nb == 2 ? l.get_str_or_nil( 2 ) : nullptr;
	if( fname_in && (*fname_in) != 0 )
	{
		o_str& fname = o_str::push_name();	
			fname.expand_fname( fname_in );
			g_bind->set_o_filename_for_index( index, fname );
		o_str::pop_name();
	}
	else
		g_bind->clear_filename_for_index( index );
	return l.return_nothing();
}
AAALUACALL( set_bind_filename )		{	LUAAAA_START( L, __FUNCTION__ );	return set_bind_filename_nd<c_img_2d>( l, g_bind_img_2d );	}
AAALUACALL( set_bind_filename_3d )	{	LUAAAA_START( L, __FUNCTION__ );	return set_bind_filename_nd<c_img_3d>( l, g_bind_img_3d );	}

// return if there an image at this bind
template < class C_IMG >
INT32 exist_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind )
{
	DBG_CHECK_ARG_NB( 1 );
	INT32 CONST index	= l.get_int32( 1 );

	if( index >= 0 )
	{
		C_IMG*	img = g_bind->get_ready( index );
		return l.return_bool( img && !img->is_empty() );
	}
	return l.return_nothing();
}
AAALUACALL( exist )		{	LUAAAA_START( L, __FUNCTION__ );	return exist_nd<c_img_2d>( l, g_bind_img_2d );	}
AAALUACALL( exist_3d )	{	LUAAAA_START( L, __FUNCTION__ );	return exist_nd<c_img_3d>( l, g_bind_img_3d );	}

template < class C_IMG >
INT32 destroy_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind )
{
	DBG_CHECK_ARG_NB( 1 );
	INT32 CONST index	= l.get_int32( 1 );

	if( index >= 0 )
		g_bind->destroy( index );

	return l.return_nothing();
}
AAALUACALL( destroy )		{	LUAAAA_START( L, __FUNCTION__ );	return destroy_nd<c_img_2d>( l, g_bind_img_2d );	}
AAALUACALL( destroy_3d )	{	LUAAAA_START( L, __FUNCTION__ );	return destroy_nd<c_img_3d>( l, g_bind_img_3d );	}

template < class C_IMG, class C_TEX >
INT32 swap_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind, C_TEX* tex )
{
	DBG_CHECK_ARG_NB( 2 );
	INT32 CONST a = l.get_int32( 1 );
	INT32 CONST b = l.get_int32( 2 );
	g_bind->swap( a,b );
	//todo 2025 November Maa added this  tex swap but it does not work
	// without tex->swap and with param "free_when_on_board" at false it seems to functions
	// at true does not function with or without swap
	tex->swap( a,b );	
	//g_bind->refresh(a);
	//g_bind->refresh(b);
	return l.return_nothing();
}
AAALUACALL( swap )		{	LUAAAA_START( L, __FUNCTION__ );	return swap_nd<c_img_2d,c_tex2d>( l, g_bind_img_2d, &tex2d );	}
AAALUACALL( swap_3d )	{	LUAAAA_START( L, __FUNCTION__ );	return swap_nd<c_img_3d,c_tex3d>( l, g_bind_img_3d, &tex3d );	}

template < class C_IMG >
FINLINE	C_IMG* get_img(			c_lua_state& l, c_bind_img<C_IMG>* g_bind, C_IMG*& p_img, INT32 stack_index )
{
	if( l.is_nil(stack_index) )
		return p_img;
	INT32 CONST index = l.get_int32( stack_index );
	if( index < 0 )
		l.error_and_escape( "bind should not be negative, %d was asked", index );
	else
	{
		INT32 CONST index_nb = static_cast<INT32>(g_bind->get_bind_max_nb());
		if( index >= index_nb )
			l.error_and_escape( "bind should inferior to %d, %d was asked", index_nb, index );
		else		
			return g_bind->get_ready( index );	//	was get_image_data which forced load including when on gpu
	}
	return nullptr;
}

template < class C_IMG >
FINLINE	C_IMG* get_img_message(		c_lua_state& l, c_bind_img<C_IMG>* g_bind, C_IMG*& p_img, INT32 stack_index )
{
	if( l.is_nil(stack_index) )
		return p_img;
	INT32 CONST index	= l.get_int32( stack_index );
	if( index >= 0 )
	{ 
		C_IMG * CONST img = g_bind->get_ready( index );	//	was get_image_data which forced load including when on gpu
		if( !img )
			l.error_message( "No image for bind %d", index );
		return img;
	}
	return nullptr;
}

template < class C_IMG >
FINLINE	C_IMG*  get_img_always(	c_lua_state& l, c_bind_img<C_IMG>* g_bind, C_IMG*& p_img, INT32 stack_index )
{
	if( l.is_nil(stack_index) )
		return p_img;
	INT32 CONST index	= l.get_int32( stack_index );
	if( index >= 0 )
		return g_bind->get_always( index );	//	was get_image_data which forced load including when on gpu
	return nullptr;
}


template < class C_IMG >
INT32 set_lua_cur_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind, C_IMG*& p_img )
{
	DBG_CHECK_ARG_NB( 1 );
	INT32 CONST index	= l.get_int32( 1 );
	if( index >= 0 )
	{
		p_img = g_bind->get_ready( index );	//	was get_image_data which forced load including when on gpu
		if( p_img && !p_img->is_empty() )
			return l.return_bool( true );
	}
	p_img = nullptr;
	return l.return_bool( false );
}
AAALUACALL( set_lua_cur )		{	LUAAAA_START( L, __FUNCTION__ );	return set_lua_cur_nd<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur );	}
AAALUACALL( set_lua_cur_3d )	{	LUAAAA_START( L, __FUNCTION__ );	return set_lua_cur_nd<c_img_3d>( l, g_bind_img_3d, c_img_3d::_lua_cur );	}


// return image size for a given bind index
AAALUACALL( get_size )
{
	LUAAAA_START( L, __FUNCTION__ );

	DBG_CHECK_ARG_NB( 1 );
	c_img_2d* CONST img		= get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	l.push_int(	img->get_size_x() );
	l.push_int(	img->get_size_y() );
	return 2;
}
AAALUACALL( get_size_channel )
{
	LUAAAA_START( L, __FUNCTION__ );

	DBG_CHECK_ARG_NB( 1 );
	c_img_2d* CONST	img		= get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	l.push_int(	img->get_size_x()		);
	l.push_int(	img->get_size_y()		);
	l.push_int(	img->get_channel_nb()	);
	return 3;
}
AAALUACALL( get_channel )
{
	LUAAAA_START( L, __FUNCTION__ );

	DBG_CHECK_ARG_NB( 1 );
	c_img_2d* CONST	img		= get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	l.push_int(	img->get_channel_nb() );
	return 1;
}
AAALUACALL( get_size_3d )
{
	LUAAAA_START( L, __FUNCTION__ );

	DBG_CHECK_ARG_NB( 1 );
	c_img_3d* CONST img		= get_img<c_img_3d>( l, g_bind_img_3d, c_img_3d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	l.push_int(	img->get_size_x() );
	l.push_int(	img->get_size_y() );
	l.push_int(	img->get_size_z() );
	return 3;
}
AAALUACALL( get_size_channel_3d )
{
	LUAAAA_START( L, __FUNCTION__ );

	DBG_CHECK_ARG_NB( 1 );
	c_img_3d* CONST img		= get_img<c_img_3d>( l, g_bind_img_3d, c_img_3d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	l.push_int(	img->get_size_x()		);
	l.push_int(	img->get_size_y()		);
	l.push_int(	img->get_size_z()		);
	l.push_int(	img->get_channel_nb()	);
	return 4;
}
AAALUACALL( get_channel_3d )
{
	LUAAAA_START( L, __FUNCTION__ );

	DBG_CHECK_ARG_NB( 1 );
	c_img_3d* CONST img		= get_img<c_img_3d>( l, g_bind_img_3d, c_img_3d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	l.push_int(	img->get_channel_nb() );
	return 1;
}

// set image size for a given bind index
//todo extend with type specification genre i8. i16, fp16, fp32 (voir lua register_array to unify names)
AAALUACALL( set_size_channel )
{
	LUAAAA_START( L, __FUNCTION__ );
	DBG_CHECK_ARG_NB( 4 );
	c_img_2d* CONST img		= get_img_always<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_false();
	INT32 sx	= l.get_int32(2);
	INT32 sy	= l.get_int32(3);
	INT32 ch_nb	= l.get_int32(4);
	//todo extend for i16 fp16 fp32
	AAA_ERR ret = img->init_with_size( sx,sy, aaa::c_pixel_format::make_format_from_channel_type(ch_nb), __FUNCTION__ );

	return l.return_bool( NOERR( ret ) );
}
//todo extend with type specification genre i8. i16, fp16, fp32 (voir lua register_array to unify names)
AAALUACALL( set_size_channel_3d )
{
	LUAAAA_START( L, __FUNCTION__ );
	DBG_CHECK_ARG_NB( 5 );
	c_img_3d* CONST img		= get_img_always<c_img_3d>( l, g_bind_img_3d, c_img_3d::_lua_cur, 1 );
	if( !img )
		return l.return_false();
	INT32 CONST sx		= l.get_int32(2);
	INT32 CONST sy		= l.get_int32(3);
	INT32 CONST sz		= l.get_int32(4);
	INT32 CONST ch_nb	= l.get_int32(5);
	//todo extend for i16 fp16 fp32
	AAA_ERR ret = img->init_with_size( sx,sy,sz, aaa::c_pixel_format::make_format_from_channel_type(ch_nb), __FUNCTION__ );

	return l.return_bool( NOERR( ret ) );
}

// return image size for a given bind index
template < class C_IMG >
INT32 get_format_name_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind, C_IMG*& p_img )
{
	DBG_CHECK_ARG_NB( 1 );
	C_IMG* CONST img	= get_img<C_IMG>( l, g_bind, p_img, 1 );
	if( !img )
		return l.return_nothing();
	return l.return_string( aaa::c_pixel_format::get_name( img->get_pixel_format() ) );
}
AAALUACALL( get_format_name )		{	LUAAAA_START( L, __FUNCTION__ );	return get_format_name_nd<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur );	}
AAALUACALL( get_format_name_3d )	{	LUAAAA_START( L, __FUNCTION__ );	return get_format_name_nd<c_img_3d>( l, g_bind_img_3d, c_img_3d::_lua_cur );	}


template < class C_IMG >
INT32 get_state_unique_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind, C_IMG*& p_img )
{
	DBG_CHECK_ARG_NB( 1 );
	C_IMG* CONST img	= get_img<C_IMG>( l, g_bind, p_img, 1 );
	if( !img )
		return l.return_nothing();
	auto id = img->get_state_unique();
	return l.return_int( id	);
}
AAALUACALL( get_state_unique )		{	LUAAAA_START( L, __FUNCTION__ );	return get_state_unique_nd<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur );	}
AAALUACALL( get_state_unique_3d )	{	LUAAAA_START( L, __FUNCTION__ );	return get_state_unique_nd<c_img_3d>( l, g_bind_img_3d, c_img_3d::_lua_cur );	}

//	input	index flag
template < class C_IMG >
INT32 set_cpu_keep_nd( c_lua_state& l, c_bind_img<C_IMG>* g_bind )
{
	DBG_CHECK_ARG_NB( 2 );
	INT32 CONST index	= l.get_int32( 1 );
	bool  CONST b_keep	= l.get_bool( 2 );
	if( g_bind->set_cpu_keep( index, b_keep ) )
		return l.return_true();
	return l.return_nothing();
}
AAALUACALL( set_cpu_keep )			{	LUAAAA_START( L, __FUNCTION__ );	return set_cpu_keep_nd<c_img_2d>( l, g_bind_img_2d );	}
AAALUACALL( set_cpu_keep_3d )		{	LUAAAA_START( L, __FUNCTION__ );	return set_cpu_keep_nd<c_img_3d>( l, g_bind_img_3d );	}

//	input	index
template < class C_BIND_IMG >
INT32 move_to_gpu_nd( c_lua_state& l, C_BIND_IMG* g_bind )
{
	DBG_CHECK_ARG_NB( 1 );
	INT32 CONST dst		= l.get_int32( 1 );
	g_bind->move_to_gpu( dst, 1 );
	return l.return_nothing();
}
AAALUACALL( move_to_gpu )			{	LUAAAA_START( L, __FUNCTION__ );	return move_to_gpu_nd<c_bind_img_2d>( l, g_bind_img_2d );	}
AAALUACALL( move_to_gpu_3d )		{	LUAAAA_START( L, __FUNCTION__ );	return move_to_gpu_nd<c_bind_img_3d>( l, g_bind_img_3d );	}

AAALUACALL( move_from_gpu )
{
	LUAAAA_START( L, __FUNCTION__ );

	DBG_CHECK_ARG_NB( 1 );
	INT32 CONST bind = l.get_int32( 1 );

	c_img_2d* CONST img = g_bind_img_2d->get_always( bind );
	if( img )
	{
		tex_2d_bind_no_gpu_move( bind );
		img->move_from_gpu( __FUNCTION__, bind );
	}
	return l.return_nothing();
}


 //	input	index
template < class C_BIND_IMG >
INT32 copy_tex_to_tex_nd( c_lua_state& l, C_BIND_IMG* g_bind )
{
	INT32 CONST arg_nb		= l.get_arg_nb_min_max( 2, 3 );
	INT32 CONST src_bind	= l.get_int32( 1 );
	INT32 CONST dst_bind	= l.get_int32( 2 );
	bool b_mipmap_generate = (arg_nb==2) ? true : l.get_bool(3);
	g_bind->copy_tex_to_tex( src_bind, dst_bind, b_mipmap_generate );
	return l.return_nothing();
}
AAALUACALL( copy_tex_to_tex )			{	LUAAAA_START( L, __FUNCTION__ );	return copy_tex_to_tex_nd<c_bind_img_2d>( l, g_bind_img_2d );	}
//todo implement
//AAALUACALL( copy_tex_to_tex_3d )		{	LUAAAA_START( L, __FUNCTION__ );	return copy_tex_to_tex_nd<c_bind_img_3d>( l, g_bind_img_3d );	}

AAALUACALL( copy )
{
	LUAAAA_START( L, __FUNCTION__ );
	UINT32 CONST nb_arg		= l.get_arg_nb( 6 );

	c_img_2d* CONST img_src		= get_img_always<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	c_img_2d* CONST img_dst		= get_img_always<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 2 );

	INT32 CONST sx = l.get_int32(5);
	INT32 CONST sy = l.get_int32(6);
	st_img_conv options( sx,sy, l.get_int32(3),l.get_int32(4) );

	auto pixel_format_src = img_src->get_pixel_format();
	if( NOERR(img_dst->init_with_size( sx,sy, pixel_format_src, __FUNCTION__ )) )
	{
		UINT32 CONST src_pitch = img_src->get_byte_pitch();
		options.src_pixel_format = pixel_format_src;
		options.signature = __FUNCTION__;
		img_dst->copy_from_src( img_src->get_data(), src_pitch, options );
	}
	return l.return_nothing();
}

//todo add async
AAALUACALL( save )
{
	LUAAAA_START( L, __FUNCTION__ );
	UINT32 CONST nb_arg	= l.get_arg_nb( 2, 3 );
	INT32  CONST index	= l.get_int32( 1 );
	C_PCHAR_C filename	= l.get_str( 2 );
	C_PCHAR ext			= ( nb_arg > 2 ) ?  l.get_str( 3 ) : nullptr;

	if( filename )
	{
		AAA_ERR err = AAA_OK;
		o_str& fname = o_str::push_name(filename);
			c_img_utils::FILE_TYPE type = c_img_utils::FILE_TYPE::DEFAULT;
			if( !ext )
			{
				ext = fname.get_ext();
				if( ext )
					type = c_img_utils::get_save_type_from_ext( ext );
			}
			fname.drop_ext();
			err = g_bind_img_2d->save( index, fname.get(), type, true );
		o_str::pop_name();
		return l.return_bool( NOERR(err) );
	}
	return l.return_nothing();
}
// return color component at xy
AAALUACALL( get_component_xy )
{
	LUAAAA_START( L, __FUNCTION__ );
	DBG_CHECK_ARG_NB( 4 );
	c_img_2d* CONST img		= get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	INT32 CONST x		= l.get_int32( 2 );
	INT32 CONST y		= l.get_int32( 3 );
	INT32 CONST what	= l.get_int32( 4 ) - 1;

	return l.return_real( img->get_value_from_xy( x, y, (aaa::COMPO)what ) );
}

AAALUACALL( get_component_max_rect_xy )
{
	LUAAAA_START( L, __FUNCTION__ );
	DBG_CHECK_ARG_NB( 6 );
	c_img_2d* CONST img		= get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	INT32 CONST x_begin	= l.get_int32( 2 );
	INT32 CONST x_end	= l.get_int32( 3 );
	INT32 CONST y_begin	= l.get_int32( 4 );
	INT32 CONST y_end	= l.get_int32( 5 );
	INT32 CONST what	= l.get_int32( 6 ) - 1;

	return l.return_real( img->get_value_max_from_rect( x_begin, x_end, y_begin, y_end, (aaa::COMPO)what ) );
}

// return pixel color at xy
AAALUACALL( get_color_xy )
{
	LUAAAA_START( L, __FUNCTION__ );
	DBG_CHECK_ARG_NB( 3 );
	c_img_2d* CONST img		= get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	INT32 CONST x		= l.get_int32( 2 );
	INT32 CONST y		= l.get_int32( 3 );

	//todoopt	add a c_img_2d::fn to get all the component at once

	FP32 color[4];
	img->get_color4r_from_xy( color, x,y );
	return l.return_fp32_v4( color );
}

// return color component at uv
AAALUACALL( get_component_uv )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32 CONST	nb_arg		=	l.get_arg_nb( 5, 6 );
	c_img_2d* CONST img		=	get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	REAL	CONST u			= l.get_real( 2 );
	REAL	CONST v			= l.get_real( 3 );
	INT32	CONST what		= l.get_int32( 4 ) - 1;
	bool	CONST b_clamped	= l.get_bool( 5 );
	bool	CONST b_linear	= nb_arg >= 6 ? l.get_bool( 6 ) : false;

	//todoopt	add a c_img_2d::fn to get all the component at once	
	return l.return_real( img->get_value_from_uv( u,v, b_clamped, (aaa::COMPO)what, b_linear ) );
}

// return color component at uv on the point of an ellipse
AAALUACALL( get_component_uv_ellipse )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	= l.get_arg_nb( 8, 9 );
	c_img_2d* CONST img		=	get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	REAL	CONST	u			= l.get_real( 2 );
	REAL	CONST	v			= l.get_real( 3 );
	REAL	CONST	ru			= l.get_real( 4 );
	REAL	CONST	rv			= l.get_real( 5 );
	INT32	CONST	nb			= l.get_int32( 6 );
	INT32	CONST	what		= l.get_int32( 7 ) - 1;
	bool	CONST	b_clamped	= l.get_bool( 8 );
	bool	CONST	b_linear	= nb_arg >= 9 ? l.get_bool( 9 ) : false;

	return l.return_real( img->get_value_from_uv_ellipse( u,v, ru,rv, nb, b_clamped, (aaa::COMPO)what, b_linear ) );
}

// return gradient of color component at uv
AAALUACALL( get_gradient_uv )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg		= l.get_arg_nb( 5, 6 );
	c_img_2d*	img				= get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	REAL	CONST	u			= l.get_real( 2 );
	REAL	CONST	v			= l.get_real( 3 );
	INT32	CONST	what		= l.get_int32( 4 ) - 1;
	bool	CONST	b_clamped	= l.get_bool( 5 );
	bool	CONST	b_linear	= nb_arg >= 6 ? l.get_bool( 6 ) : false;

	REAL	vec[2];
	img->get_gradient_from_uv( vec, u,v, b_clamped, (aaa::COMPO)what, b_linear );
	return l.return_real_v2( vec );
}

// return gradient of color component at uv on the point of an ellipse
AAALUACALL( get_gradient_uv_ellipse )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg		= l.get_arg_nb( 8, 9 );
	c_img_2d* CONST img			=	get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();
	REAL	CONST	u			= l.get_real( 2 );
	REAL	CONST	v			= l.get_real( 3 );
	REAL	CONST	ru			= l.get_real( 4 );
	REAL	CONST	rv			= l.get_real( 5 );
	INT32	CONST	nb			= l.get_int32( 6 );
	INT32	CONST	what		= l.get_int32( 7 ) - 1;
	bool	CONST	b_clamped	= l.get_bool( 8 );
	bool	CONST	b_linear	= nb_arg >= 9 ? l.get_bool( 9 ) : false;

	REAL	vec[2];
	img->get_gradient_from_uv_ellipse( vec, u,v, ru,rv, nb, b_clamped, (aaa::COMPO)what, b_linear );
	return l.return_real_v2( vec );
}

// return pixel color for uv
AAALUACALL( get_color_uv )
{
	LUAAAA_START( L, __FUNCTION__ );
	DBG_CHECK_ARG_NB( 4 );
	c_img_2d* CONST img			= get_img<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );		//	was get_image_data which forced load including when on gpu
	if( !img )
		return l.return_nothing();
	REAL	CONST	u			= l.get_real( 2 );
	REAL	CONST	v			= l.get_real( 3 );
	bool	CONST	b_clamped	= l.get_bool( 4 );

	FP32	val[4];
	img->get_color4r_from_uv_nearest( val, u,v, b_clamped );
	return l.return_fp32_v4( val );
}

AAALUACALL( set_color_xy )
{
	LUAAAA_START( L, __FUNCTION__ );
	INT32	CONST	nb_arg	= l.get_arg_nb( 4, 6, 7 );
	c_img_2d* CONST img		= get_img_message<c_img_2d>( l, g_bind_img_2d, c_img_2d::_lua_cur, 1 );
	if( !img )
		return l.return_nothing();

	if( !img->is_data_valid(__FUNCTION__) )
	{
		l.error_message( "img have no data on Cpu" );
		return l.return_nothing();
	}

	INT32 CONST x = l.get_int32( 2 );
	INT32 CONST y = l.get_int32( 3 );
	if( !img->is_valid_xy( x,y ) )
	{
		l.error_message( "%d,%d invalid x,y", x,y );
		return l.return_nothing();
	}

	FP32		col[4];
	switch( nb_arg )
	{
	case 7:		l.get_v4( col, 4 );
				img->set_xy_color4r_low( x,y, col );		// x,y check done already
				break;
	case 6:		l.get_v3( col, 4 );
				img->set_xy_color3r_low( x,y, col );		// x,y check done already
				break;
	case 4:		if( l.is_table(4) )
				{
					l.get_v4_table( col, 4 );
					img->set_xy_color4r_low( x,y, col );	// x,y check done already
				}
				else
				{
					col[2] = col[1] = col[0] = l.get_fp32( 4 );
					img->set_xy_color3r_low( x,y, col );	// x,y check done already
				}
				break;
	default:	break;
	}
		
	return l.return_nothing();
}

namespace{
	thread_local o_str	fname_snap;
};

//	
AAALUACALL( build_snap_filename )
{
	LUAAAA_START( L, __FUNCTION__ );
	::build_snap_filename( fname_snap );
	return l.return_string( fname_snap );
}

#define	ADD_FN_3D( name )	l.add_fn_to_table( #name, name##_3d )

void	register_img( lua_State* L )
{
	LUAAAA_START( L, __FUNCTION__ );

	l.define_table( "img" );
		ADD_FN( make_tex_index				);

		ADD_FN(	read						);
		ADD_FN( read_no_error				);
		ADD_FN(	is_ready					);
		ADD_FN( get_bind_index				);
		ADD_FN( get_bind_filename			);
		ADD_FN( set_bind_filename			);
		ADD_FN( exist						);

		ADD_FN( destroy						);
		ADD_FN( swap						);

		ADD_FN( set_lua_cur					);
		ADD_FN( get_size					);
		ADD_FN( get_size_channel			);
		ADD_FN( get_channel					);
		ADD_FN( set_size_channel			);
		ADD_FN( get_format_name				);
		ADD_FN( get_state_unique			);

		ADD_FN( set_cpu_keep				);
		ADD_FN( move_to_gpu					);
		ADD_FN( move_from_gpu				);
		ADD_FN( copy_tex_to_tex				);
		ADD_FN( copy						);

		ADD_FN( save						);

		ADD_FN( get_component_xy			);
		ADD_FN(	get_component_max_rect_xy	);
		ADD_FN( get_color_xy				);
		ADD_FN( set_color_xy				);

		ADD_FN( get_component_uv			);
		ADD_FN( get_component_uv_ellipse	);
		ADD_FN( get_gradient_uv				);
		ADD_FN( get_gradient_uv_ellipse		);
		ADD_FN( get_color_uv				);

		ADD_FN( build_snap_filename			);
	lua_pop( L, 1 );	//pop table "img"

	l.define_table( "img3d" );
		ADD_FN_3D( read						);
		ADD_FN_3D( read_no_error			);
		ADD_FN_3D( is_ready					);
		ADD_FN_3D( get_bind_index	 		);
		ADD_FN_3D( get_bind_filename		);
		ADD_FN_3D( set_bind_filename		);
		ADD_FN_3D( exist					);

		ADD_FN_3D( destroy					);
		ADD_FN_3D( swap						);

		ADD_FN_3D( set_lua_cur				);
		ADD_FN_3D( get_size					);
		ADD_FN_3D( get_size_channel			);
		ADD_FN_3D( get_channel				);
		ADD_FN_3D( set_size_channel			);
		ADD_FN_3D( get_format_name			);
		ADD_FN_3D( get_state_unique			);

		ADD_FN_3D( set_cpu_keep				);
		ADD_FN_3D( move_to_gpu				);
	lua_pop( L, 1 );	//pop table "img"

}

#undef ADD_FN_3D

}	//	end namespace n_img
}	//	end namespace aaalua
//////