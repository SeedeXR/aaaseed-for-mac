
#include "img.h"
#include "image/convert/img_convert_macros.h"
#include "media/video/texture_flux_master.h"
#include "math/v.h"
#include "gol/gol_color.h"

using namespace aaa;
	////////
	////////
//todo	make it fit better in the memory scheme
void	c_img_2d::merge_channel_to_rgba()
{
	UINT8* src = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !src )
		return;

	//todo add rg and bgr mode
	//todo code is probably buggy
	if( _pixel_format != PIXEL_FORMAT::R_8 && _pixel_format != PIXEL_FORMAT::RGB_8 && _pixel_format != PIXEL_FORMAT::RGBA_8 )
	{
		debug_break( "%s() %d channel unimplemented", __FUNCTION__, _channel_nb );
		print_err_unsupported_format( __FUNCTION__ );
		return;
	}

	// todo deal with pitch
	INT32	size		= get_pixel_nb_to_process();
	//INT32	size		= i32 * 4;
	UINT8*	new_data	= (UINT8*)MALLOC_ALIGNED_SIGNATURE( size * 4, 0, __FUNCTION__ );
	if( new_data )
	{
		//INT32*	cur		= (INT32*) new_data - 1;
		UINT32	sx			= get_size_x();
		UINT32	pitch		= sx * 4;
		UINT32	src_pitch	= get_byte_pitch();
		UINT8*	dst			= new_data;
		if( _channel_nb == 4 )
		{
			for( INT32 i = get_size_y(); i > 0; --i )
			{
				UINT8*	r = src - 1;
				UINT8*	g = r + size;
				UINT8*	b = g + size;
				UINT8*	a = b + size;

				INT32	i32 = sx;
				INT32*	cur = (INT32*)dst - 1;
				while( i32-- )
				{
#ifdef	WIN32
					*++cur = (*++a << 24) + (*++b << 16) + (*++g << 8) + *++r;
#else
					*++cur = (*++r << 24) + (*++g << 16) + (*++b << 8) + *++a;
#endif
				}
				src += src_pitch;
				dst += pitch;
			}
		}
		else if( _channel_nb == 3 )
		{
			for( INT32 i = get_size_y(); i > 0; --i )
			{
				UINT8*	r = src - 1;
				UINT8*	g = r + size;
				UINT8*	b = g + size;

				INT32	i32 = sx;
				INT32*	cur = (INT32*)dst - 1;
				while ( i32-- )
				{
#ifdef	WIN32
					*++cur = *++r + (*++b << 16) + (*++g << 8) + (0xff << 24);
#else
					*++cur = ( *++r << 24 ) + ( *++g << 16 ) + ( *++b << 8 ) + 0xff;
#endif
				}
				src += src_pitch;
				dst += pitch;
			}
		}
		else if( _channel_nb == 1 )
		{
			for( INT32 i = get_size_y(); i > 0; --i )
			{
				INT32	i32 = sx;
				INT32*	cur = (INT32*)dst - 1;
				UINT8*	bw = src;
				while( i32-- )
				{
					*++cur = (*bw << 24) + (*bw << 16) + (*bw << 8);
					++bw;
				}
				src += src_pitch;
				dst += pitch;
			}
		}
		else
		{
			print_err_unsupported_channel_nb( __FUNCTION__ );
		}
		change_data( new_data, size*4, true );
	}
}

////////
////////
//todo nearly unused
//todo generalize (offset/factor) ?
//todo optimize ?
//todo more format
void	c_img_2d::inverse()
{
	UINT8* src = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !src )
		return;

//	UINT8*	a	= data;
//	INT32	i32	= get_pixel_nb_to_process_aligned_x() + 1;
	// todo deal with pitch
	UINT32	sx = get_size_x();
//	UINT32	pitch = sx * 4;
	UINT32	src_pitch = get_byte_pitch();

	for( INT32 i = get_size_y(); i > 0; --i )
	{

		INT32	i32 = sx;
		UINT8*	a = src;
		switch( _channel_nb )
		{
		case 1:
			while( --i32 >= 0 )
			{
				*a = 255 - *a;	++a;
			}
			break;
		case 2:
			while( --i32 >= 0 )
			{
				*a = 255 - *a;	++a;
				*a = 255 - *a;	++a;
			}
			break;
		case 3:
			while( --i32 >= 0 )
			{
				*a = 255 - *a;	++a;
				*a = 255 - *a;	++a;
				*a = 255 - *a;	++a;
			}
			break;
		case 4:
			while( --i32 >= 0 )
			{
				*a = 255 - *a;	++a;
				*a = 255 - *a;	++a;
				*a = 255 - *a;	a += 2;
			}
			break;
		}
		src += src_pitch;
	}

	set_changed();
}

////////
////////
//todo nearly unused
//todo generalize (offset/factor) ?
//todo optimize ?
//todo more format
void	c_img_2d::compo_inverse( UINT32 CONST index )
{
	UINT8* src = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !src )
		return;

	if( index < ( UINT32 )_channel_nb )
	{

		// todo deal with pitch
		UINT32	sx = get_size_x();
		UINT32	src_pitch = get_byte_pitch();
		for ( INT32 i = get_size_y(); i > 0; --i )
		{
			INT32	i32 = sx;
			UINT8*	a = src + index;

			switch( _channel_nb )
			{
			case 1:	while ( --i32 >= 0 )	{ *a = 255 - *a;	++a; }		break;
			case 2:	while ( --i32 >= 0 )	{ *a = 255 - *a;	a += 2; }	break;
			case 3:	while ( --i32 >= 0 )	{ *a = 255 - *a;	a += 3; }	break;
			case 4:	while ( --i32 >= 0 )	{ *a = 255 - *a;	a += 4; }	break;
			}
			src += src_pitch;
		//default:
		//	debug_break( "%s() %d channel unimplemented", __FUNCTION__, _channel_nb );
		//	print_err_unsupported_channel_nb( __FUNCTION__ );
		//	return;
		}
		set_changed();
	}
	else
	{
		DBG_PRINT_STRING( "This image don't have enough channel" );
	}
}

////////
////////
void	c_img_2d::alpha_inverse()
{
	compo_inverse( 3 );
}

template< typename T, INT32 ch_nb >
void c_img_2d::fill_compo_loop( UINT8* CONST dst, INT32 CONST sx, INT32 CONST sy, T CONST v )
{
	for( INT32 iy=0; iy<sy; ++iy )
	{
		INT32	nb_x = sx;
		T* pt = (T*)(dst + get_byte_offset( 0,iy ) );
		*pt = v;
		switch( ch_nb )
		{
		case 1:	while( nb_x-- )	{ *++pt = v; };			break;
		case 2:	while( nb_x-- )	{ pt += 2; *pt = v;	}	break;
		case 3:	while( nb_x-- )	{ pt += 3; *pt = v;	}	break;
		case 4:	while( nb_x-- )	{ pt += 4; *pt = v;	}	break;
		default:	print_err_unsupported_channel_nb( __FUNCTION__ );	break;
		}
	}
}

////////
////////
void	c_img_2d::fill_compo( INT32 CONST compo_index, FP32 CONST val )
{
	UINT8* data = (UINT8*)get_data();
	if( !data )
		return;
	INT32 sx = get_size_x();
	INT32 sy = get_size_y();
	if( sx <= 0 || sy <= 0 )
		return;
	//todo bgr ?
	if( c_pixel_format::is_format_rgb( _pixel_format ) )
	{
		auto type = get_data_type();
		switch( type )
		{
		case aaa::PIXEL_TYPE::UINT_8:
			{
				UINT8	v	=  aaa::img::c_compo::to_uint8( val );
				data += compo_index;
				switch( _channel_nb )
				{
				case 1:		fill_compo_loop<UINT8,1>( data, sx, sy, v );		break;
				case 2 :	fill_compo_loop<UINT8,2>( data, sx, sy, v );		break;
				case 3 :	fill_compo_loop<UINT8,3>( data, sx, sy, v );		break;
				case 4 :	fill_compo_loop<UINT8,4>( data, sx, sy, v );		break;
				default:	print_err_unsupported_channel_nb( __FUNCTION__ );	return;
				}
			}
			break;
		case aaa::PIXEL_TYPE::UINT_16:
			{
				UINT16	v	=  aaa::img::c_compo::to_uint16( val );
				data += compo_index * 2;
				switch( _channel_nb )
				{
				case 1:		fill_compo_loop<UINT16,1>( data, sx, sy, v );		break;
				case 2 :	fill_compo_loop<UINT16,2>( data, sx, sy, v );		break;
				case 3 :	fill_compo_loop<UINT16,3>( data, sx, sy, v );		break;
				case 4 :	fill_compo_loop<UINT16,4>( data, sx, sy, v );		break;
				default:	print_err_unsupported_channel_nb( __FUNCTION__ );	return;
				}
			}
			break;
		case aaa::PIXEL_TYPE::FLOAT_16:
			{
				FP16	v	=  aaa::img::c_compo::to_fp16( val );
				data += compo_index * 2;
				switch( _channel_nb )
				{
				case 1:		fill_compo_loop<FP16,1>( data, sx, sy, v );		break;
				case 2 :	fill_compo_loop<FP16,2>( data, sx, sy, v );		break;
				case 3 :	fill_compo_loop<FP16,3>( data, sx, sy, v );		break;
				case 4 :	fill_compo_loop<FP16,4>( data, sx, sy, v );		break;
				default:	print_err_unsupported_channel_nb( __FUNCTION__ );	return;
				}
			}
			break;
		case aaa::PIXEL_TYPE::FLOAT_32:
			{
				data += compo_index * 4;
				switch( _channel_nb )
				{
				case 1:		fill_compo_loop<FP32,1>( data, sx, sy, val );	break;
				case 2 :	fill_compo_loop<FP32,2>( data, sx, sy, val );	break;
				case 3 :	fill_compo_loop<FP32,3>( data, sx, sy, val );	break;
				case 4 :	fill_compo_loop<FP32,4>( data, sx, sy, val );	break;
				default:	print_err_unsupported_channel_nb( __FUNCTION__ );	return;
				}
			}
			break;
		default:
			print_err_unsupported_type( __FUNCTION__ );
			return;
		}
	}
	else
	{
		//todo
		print_err_unsupported_format( __FUNCTION__ );
		return;
	}
	set_changed();
}

// fill_alpha_sse3 removed : dead code, the SSE3 path was slower than the scalar / stripe parallel path in
// fill_alpha below on modern CPUs.

////////
////////
void	c_img_2d::fill_alpha( FP32 CONST alpha )
{
	UINT8* data = (UINT8*)get_data();
	if( !data )
		return;

	INT32 alpha_index = c_pixel_format::get_compo_index( _pixel_format, aaa::COMPO::ALPHA );
	if( alpha_index < 0 )
	{
		DBG_PRINT_STRING( "%s() This image don't have alpha channel", __FUNCTION__ );
		return;
	}

	INT32 sx = get_size_x();
	INT32 sy = get_size_y();
	if( sx <= 0 || sy <= 0 )
		return;

	auto type = get_data_type();
	switch( type )
	{
	case aaa::PIXEL_TYPE::UINT_8:
		{
			UINT8	val		= aaa::img::c_compo::to_uint8( alpha );
			UINT32	sx		= get_size_x();
			UINT32	pitch	= get_byte_pitch();
			data += alpha_index;
			imgcon::call_by_line_striped( get_size_y(), [&](INT32 j) NOEXCEPT
			{
				UINT8*	RESTRICT a = data + (j * pitch);
				INT32	pixel_nb = sx;
				while( pixel_nb-- )
				{
					*a = val;
					a += 4;
				}
			});
		}
		break;
	case aaa::PIXEL_TYPE::UINT_16:
	case aaa::PIXEL_TYPE::FLOAT_16:
	case aaa::PIXEL_TYPE::FLOAT_32:
		fill_compo( alpha_index, alpha );
		break;
	default:
		print_err_unsupported_type( __FUNCTION__ );
		return;
	}
	set_changed();

}

////////
////////
//todo move it with generic function ad handle more cases only used 
void		c_img_2d::transform_compo_to_white_with_alpha( UINT32 CONST index )
{
	UINT8* src = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !src )
		return;

	if( _channel_nb != 4 )
	{
		DBG_PRINT_STRING( "%s() This image don't have alpha channel", __FUNCTION__ );
		return;
	}

	//UINT32*	p_rgba	= ((UINT32*)src) - 1;
	//UINT8*	p_comp	= src + index;
//	INT32	i32		= get_pixel_nb_to_process_aligned_x() + 1;
	UINT32	sx = get_size_x();
	UINT32	pitch = get_byte_pitch();
	imgcon::call_by_line_striped( get_size_y(), [&](INT32 j) NOEXCEPT
	{
		UINT32* RESTRICT p_rgba = (UINT32*)(src + (j * pitch));
		INT32 pixel_nb = sx;
		while( pixel_nb-- )
			*p_rgba++ = PACK_RGBA( 0xff,0xff,0xff, GET_BYTE_INDEX( *p_rgba, index ) );
	});
	set_changed();
}

////////
////////
void	c_img_2d::fill_alpha_from_rgb()
{
	UINT8* dst = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !dst )
		return;

	if( _channel_nb != 4 )
	{
		DBG_PRINT_STRING( "%s() This image don't have alpha channel", __FUNCTION__ );
		return;
	}

	UINT32	sx		= get_size_x();
	UINT32	pitch	= get_byte_pitch();
	for( INT32 i = get_size_y(); i > 0; --i )
	{

	////	__m128i vk0 = _mm_set1_epi8( 0 );   // constant vector of all 0s for use with _mm_unpacklo_epi8/_mm_unpackhi_epi8
	//	__m128i v = _mm_stream_load_si128( (__m128i *)dst );
	//	CONST __m128i mask_rg	= _mm_set_epi8( 0xff, 13, 0xff, 12, 0xff, 9, 0xff, 8, 0xff, 5, 0xff, 4, 0xff, 1, 0xff, 0 );
	//	CONST __m128i mask_b	= _mm_set_epi8( 0xff, 0xff, 0xff, 14, 0xff, 0xff, 10, 0xff, 0xff, 0xff, 6, 0xff, 0xff, 0xff, 0xff, 2 );
	//	CONST __m128i mask_sum	= _mm_set_epi8( 15, 14, 7, 6, 13, 12, 5, 4, 11, 10, 3, 2, 9, 8, 1, 0 );

	//	__m128i	vk0 = _mm_shuffle_epi8( v, mask_rg );
	//	__m128i	vk1 = _mm_shuffle_epi8( v, mask_b );
	//	//__m128i vl = _mm_unpacklo_epi8( vk0, vk1 ); // unpack to two vectors of 16 bit values
	//	//__m128i vh = _mm_unpackhi_epi8( vk0, vk1 );
	//	__m128i b	= _mm_hadd_epi16 ( vk0,  vk1 );
	//	__m128i	vk2 = _mm_shuffle_epi8( b, mask_sum );
	//	__m128i b2	= _mm_hadd_epi16( vk2, vk2 );
	//	__m128i b3 = _mm_packus_epi16( b2, b2 );

	//	CONST __m128i mask_sum2 = _mm_set_epi8( 3, 0xff, 0xff, 0xff, 2, 0xff, 0xff, 0xff, 1, 0xff, 0xff, 0xff, 0, 0xff, 0xff, 0xff );
	//	CONST __m128i	mask1 = _mm_set_epi8( 0xff, 11, 10, 9, 0xff, 8, 7, 6, 0xff, 5, 4, 3, 0xff, 2, 1, 0 );
	//	__m128i	vk3 = _mm_shuffle_epi8( b3, mask_sum2 );
	//	__m128i	vk4 = _mm_shuffle_epi8( v, mask1 );
	//	__m128i res = _mm_or_si128( vk3, vk4 );
	//	_mm_store_si128( (__m128i *)dst, res );
		INT32	i32 = sx;
		UINT32*	a = (UINT32*)dst;
		while( i32-- )
		{
			UINT32 rgba = *a;
#ifdef	WIN32
			UINT32 sum = ( rgba & 0xff ) + ( ( rgba>>8 ) & 0xff ) + ( ( rgba>>16 ) & 0xff );
			if( sum > 255 )
				sum = 255;
			*a = (rgba & 0xffffff) | (sum << 24);
#else
			UINT32 sum = ((rgba >> 24) & 0xff) + ((rgba >> 16) & 0xff) + ((rgba >> 8) & 0xff);
			*a = (rgba & 0xffffff00) | (sum / 3);
#endif
			++a;
		}
		dst += pitch;
	}

//		INT32	i32	= get_pixel_nb_to_process_aligned_x() + 1;
//		UINT32*	a	= (UINT32*)dst;
//		while( --i32 )
//		{
//			UINT32 rgba = *a;
//#ifdef	WIN32
//			UINT32 sum = ( rgba & 0xff ) + ( ( rgba>>8 ) & 0xff ) + ( ( rgba>>16 ) & 0xff );
//			if( sum > 255 )
//				sum = 255;
//			*a = ( rgba & 0xffffff ) | ( sum<<24 );
//#else
//			UINT32 sum = ( ( rgba>>24 ) & 0xff ) + ( ( rgba>>16 ) & 0xff ) + ( ( rgba>>8 ) & 0xff );
//			*a = ( rgba & 0xffffff00 ) | ( sum/3 );
//#endif
//			++a;
//		}
	set_changed();
}

////////
////////
void c_img_2d::fill_rgba8_sse3( FP32 CONST * CONST color )
{
//	DBG_HEAP_IS_CORRUPT();

	UINT8	CONST	r8		= aaa::img::c_compo::to_uint8( color[0] );
	UINT8	CONST	g8		= aaa::img::c_compo::to_uint8( color[1] );
	UINT8	CONST	b8		= aaa::img::c_compo::to_uint8( color[2] );
	UINT8	CONST	a8		= aaa::img::c_compo::to_uint8( color[3] );
	__m128i	CONST	mask	= _mm_set_epi8( a8, b8, g8, r8, a8, b8, g8, r8, a8, b8, g8, r8, a8, b8, g8, r8 );

	UINT32	CONST	sx			= get_size_x();
	INT32	CONST	sy			= get_size_y();
	UINT32	CONST	pitch		= get_byte_pitch();
	UINT8*	CONST	dst			= get_data_uint8();

	CONSTEXPR INT32 PIXEL_PER_LOOP = 32;
	CONSTEXPR INT32 BYTE_PER_LOOP = PIXEL_PER_LOOP * 4;
	INT32 CONST PIXEL_BLK_LOOP = sx / PIXEL_PER_LOOP;
	INT32 CONST PIXEL_4_LEFT = ( sx % PIXEL_PER_LOOP ) >> 2;
	INT32 CONST PIXEL_1_LEFT = sx % 4;

	imgcon::call_by_line_striped( get_size_y(), [&](INT32 CONST y) NOEXCEPT
	{
		UINT8*	RESTRICT p_dst = dst + y * pitch;
		// Increment by 8 * 16 bytes at a time:
		for( INT32 nb = PIXEL_BLK_LOOP; nb > 0; --nb )
		{
			_mm_prefetch( (CHAR*)p_dst + 256, _MM_HINT_NTA );
			_mm_storeu_si128( (__m128i *)(p_dst +  0), mask );
			_mm_storeu_si128( (__m128i *)(p_dst + 16), mask );
			_mm_storeu_si128( (__m128i *)(p_dst + 32), mask );
			_mm_storeu_si128( (__m128i *)(p_dst + 48), mask );
			_mm_storeu_si128( (__m128i *)(p_dst + 64), mask );
			_mm_storeu_si128( (__m128i *)(p_dst + 80), mask );
			_mm_storeu_si128( (__m128i *)(p_dst + 96), mask );
			_mm_storeu_si128( (__m128i *)(p_dst + 112), mask );
			p_dst	+= BYTE_PER_LOOP;
		}
		switch ( PIXEL_4_LEFT )
		{
		case 7: _mm_storeu_si128( (__m128i *)(p_dst + 96),	mask );
		case 6: _mm_storeu_si128( (__m128i *)(p_dst + 80),	mask );
		case 5: _mm_storeu_si128( (__m128i *)(p_dst + 64),	mask );
		case 4: _mm_storeu_si128( (__m128i *)(p_dst + 48),	mask );
		case 3: _mm_storeu_si128( (__m128i *)(p_dst + 32),	mask );
		case 2: _mm_storeu_si128( (__m128i *)(p_dst + 16),	mask );
		case 1: _mm_storeu_si128( (__m128i *)(p_dst +  0),	mask );
		}

		p_dst	+= 16 * PIXEL_4_LEFT;
		for( INT32 nb = PIXEL_1_LEFT; nb > 0; --nb )
		{
			p_dst[0] = r8;
			p_dst[1] = g8;
			p_dst[2] = b8;
			p_dst[3] = a8;
			p_dst += 4;
		}
	});
	// Memory fence to make sure the stores are good:
	//_mm_mfence();
//	DBG_HEAP_IS_CORRUPT();
}

void	c_img_2d::fill_rgba( FP32 CONST * CONST color )
{
	if( !check_valid(__FUNCTION__) )
		return;

	FP32 col[4];
	aaa::PIXEL_FORMAT	format =_pixel_format;
	if( c_pixel_format::is_bgr(format) )
	{
			col[0] = color[2];
			col[1] = color[1];
			col[2] = color[0];
			col[3] = color[3];
	}
	else
		cpy_v4( col, color );

	INT32 pitch = get_byte_pitch();
		
	switch( get_data_type() )
	{
	case aaa::PIXEL_TYPE::UINT_8:
		{
			UINT8* dst = get_data_uint8();

			switch( format )
			{
			case PIXEL_FORMAT::R_8:			fill_channel_1<UINT8>	( dst,	pitch,	aaa::img::c_compo::to_uint8(col[0])	);	break;
			case PIXEL_FORMAT::RG_8:		fill_channel_2<UINT8>	( dst,	pitch,	aaa::img::c_compo::to_uint8(col[0]),
																					aaa::img::c_compo::to_uint8(col[1])	);	break;
			case PIXEL_FORMAT::RGB_8:
			case PIXEL_FORMAT::BGR_8:		fill_channel_3<UINT8>	( dst,	pitch,	aaa::img::c_compo::to_uint8(col[0]),
																					aaa::img::c_compo::to_uint8(col[1]),
																					aaa::img::c_compo::to_uint8(col[2])	);	break;
			case PIXEL_FORMAT::RGBA_8:
			case PIXEL_FORMAT::BGRA_8:
				{
					if( texture_flux_master->is_convert_to_rgb_sse3() )
					{
						fill_rgba8_sse3( col );
					}
					else
					{
						UINT32	rgba;
						rgba = PACK_RGBA(	aaa::img::c_compo::to_uint8(col[0]),
											aaa::img::c_compo::to_uint8(col[1]),	
											aaa::img::c_compo::to_uint8(col[2]),
											aaa::img::c_compo::to_uint8(col[3])
											);
					//	INT32	i32 = get_pixel_nb_to_process_aligned_x() + 1;
					//	UINT32*	a	= ((UINT32*)dst) - 1;
					//	while( --i32 )
					//	{
					//		*++a = rgba;
					//	}
						UINT32	sx		= get_size_x();
						UINT32	pitch	= get_byte_pitch();
						for ( INT32 i = get_size_y(); i > 0; --i )
						{
							INT32	i32 = sx;
							UINT32*	a = (UINT32*)dst - 1;
							while( i32-- )
								*++a = rgba;
							dst += pitch;
						}
					}
					set_changed();
				}
				break;
			default:
				print_err_unsupported_format( __FUNCTION__ );
				break;
			}
		}
		break;
	case aaa::PIXEL_TYPE::UINT_16:
		{
			pitch /= 2;
			UINT16* dst = get_data_uint16();

			switch( format )
			{
			case PIXEL_FORMAT::R_16:		fill_channel_1<UINT16>	( dst,	pitch,	aaa::img::c_compo::to_uint16( col[0] )	);	break;
			case PIXEL_FORMAT::RG_16:		fill_channel_2<UINT16>	( dst,	pitch,	aaa::img::c_compo::to_uint16( col[0] ),
																					aaa::img::c_compo::to_uint16( col[1] )	);	break;
			case PIXEL_FORMAT::RGB_16:		  
			case PIXEL_FORMAT::BGR_16:		fill_channel_3<UINT16>	( dst,	pitch,	aaa::img::c_compo::to_uint16( col[0] ),
																					aaa::img::c_compo::to_uint16( col[1] ),
																					aaa::img::c_compo::to_uint16( col[2] )	);	break;
			case PIXEL_FORMAT::RGBA_16:
			case PIXEL_FORMAT::BGRA_16:		fill_channel_4<UINT16>	( dst,	pitch,	aaa::img::c_compo::to_uint16( col[0] ),
																					aaa::img::c_compo::to_uint16( col[1] ),
																					aaa::img::c_compo::to_uint16( col[2] ),
																					aaa::img::c_compo::to_uint16( col[3] )	);	break;
			default:
				print_err_unsupported_format( __FUNCTION__ );
				break;
			}
		}
		break;
	case aaa::PIXEL_TYPE::FLOAT_16:
		{
			pitch /= 2;
			FP16* dst = get_data_fp16();

			switch( format )
			{
			case PIXEL_FORMAT::DEPTH_16:
			case PIXEL_FORMAT::R_16FP:		fill_channel_1<FP16>	( dst,	pitch,	aaa::img::c_compo::to_fp16( col[0] )	);	break;
			case PIXEL_FORMAT::RG_16FP:		fill_channel_2<FP16>	( dst,	pitch,	aaa::img::c_compo::to_fp16( col[0] ),
																					aaa::img::c_compo::to_fp16( col[1] )	);	break;
			case PIXEL_FORMAT::RGB_16FP:
			case PIXEL_FORMAT::BGR_16FP:	fill_channel_3<FP16>	( dst,	pitch,	aaa::img::c_compo::to_fp16( col[0] ),
																					aaa::img::c_compo::to_fp16( col[1] ),
																					aaa::img::c_compo::to_fp16( col[2] )	);	break;
			case PIXEL_FORMAT::RGBA_16FP:
			case PIXEL_FORMAT::BGRA_16FP:	fill_channel_4<FP16>	( dst,	pitch,	aaa::img::c_compo::to_fp16( col[0] ),
																					aaa::img::c_compo::to_fp16( col[1] ),
																					aaa::img::c_compo::to_fp16( col[2] ),
																					aaa::img::c_compo::to_fp16( col[3] )	);	break;
			default:
				print_err_unsupported_format( __FUNCTION__ );
				break;
			}
		}
		break;
	case aaa::PIXEL_TYPE::FLOAT_32:
		{
			pitch /= 4;
			FP32* dst = get_data_fp32();

			switch( format )
			{
			case PIXEL_FORMAT::DEPTH_32:
			case PIXEL_FORMAT::R_32FP:		fill_channel_1<FP32>	( dst,	pitch,	col[0]	);	break;
			case PIXEL_FORMAT::RG_32FP:		fill_channel_2<FP32>	( dst,	pitch,	col[0],
																					col[1]	);	break;
			case PIXEL_FORMAT::RGB_32FP:												 
			case PIXEL_FORMAT::BGR_32FP:	fill_channel_3<FP32>	( dst,	pitch,	col[0],
																					col[1],
																					col[2]	);	break;
			case PIXEL_FORMAT::RGBA_32FP:												
			case PIXEL_FORMAT::BGRA_32FP:	fill_channel_4<FP32>	( dst,	pitch,	col[0],
																					col[1],
																					col[2],
																					col[3]	);	break;
			default:
				print_err_unsupported_format( __FUNCTION__ );
				break;
			}
		}
		break;
	default:
		print_err_unsupported_type( __FUNCTION__ );
		break;
	}
}


void	c_img_2d::premultiply_alpha()
{
	UINT8* p8 = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !p8 )
		return;

	if( _channel_nb != 4 )
	{
		DBG_PRINT_STRING( "%s() This image doesn't have alpha channel", __FUNCTION__ );
		return;
	}

	UINT32 CONST sx    = get_size_x();
	UINT32 CONST pitch = get_byte_pitch();
	CONSTEXPR FLOAT inv255 = 1.f / 255.f;
	imgcon::call_by_line_striped( get_size_y(), [&](INT32 CONST i) NOEXCEPT
	{
		UINT32 * RESTRICT p32 = (UINT32*)(p8 + (i * pitch));
		INT32	pixel_nb = sx;

		while( pixel_nb-- )
		{
			UINT32 u32 = *p32;
			INT32 a = GET_BYTE_3(u32);
			*p32++ = PACK_RGBA(	static_cast<UINT8>( static_cast<float>( GET_BYTE_0(u32) * a ) * inv255 ),
								static_cast<UINT8>( static_cast<float>( GET_BYTE_1(u32) * a ) * inv255 ),
								static_cast<UINT8>( static_cast<float>( GET_BYTE_2(u32) * a ) * inv255 ),
								a	);
		}
	});

	set_changed();

}

////////
////////
//todo deal with alignment and odd/even size_x for src data
//todo	deal seriously with the file extension pb

#define	USE_FLOAT	0
#if USE_FLOAT
#define	TYPE_USED	DOUBLE
#else
#define	TYPE_USED	INT32
#define	SHIFT	16
#endif


////	we write YUYV and it should be
////			UYUV but this way it's ready
////	for the VM machine
////	we also do a ^ 0x80 with the U and V
////todo	this is too much use stride
////todo  deal with alignment
//AAA_ERR	c_img_2d::rgb_to_yuv()
//{
//	AAA_ERR	retcode = ERR_ANY;
//
//	GOOD_PRINT_STRING( "IMG : Converting from RGB to YUV : " );
//						//get_type_str( TYPE_RGB ),
//						//	get_type_str( TYPE_YUV )
//						//	 );
//
//	IF_THIS_NULL()
//	{
//		retcode = c_img_utils::ERR_NULL;
//		goto exit;
//	}
//
//	UINT8* dst = get_data_valid_rgb_uint8( __FUNCTION__ );
//	if( dst )
//	{
//		register	TYPE_USED	f;
//		register	TYPE_USED	r0;
//		register	TYPE_USED	g0;
//		register	TYPE_USED	b0;
//		TYPE_USED	r1;
//		TYPE_USED	g1;
//		TYPE_USED	b1;
//
//		UINT8*	src = dst;
//		if( _channel_nb == 4 )	{	src = dst;		}
//		else					{	src = dst-1;	}
//
//		retcode = AAA_OK;
//		//todo deal with x alignment
//		for( INT32 l = get_size_y(); l > 0; --l )
//		{
//			// todoquick, 2 for with same variable ???
////			for( x = _size_x >> 1; x > 0; --x )
////			{
//				/*
//				these info come from the Fast company
//				and they deal with the UYV use in the screen Machine
//						Y =  0.256635*R +0.503971*G +0.098218*B	+ 16
//						U = -0.148135*R -0.290902*G +0.439036*B + 128
//						V =  0.439327*R -0.367673*G -0.071655*B + 128
//				the variables are in these ranges
//						0  <= RGB <= 255
//						16 <=  Y  <= 235
//						16 <= UV  <= 240
//				en contradiction avec la doc de la Video Machine
//				*/
//			for( INT32 x = get_size_x() / 2; x > 0; --x )
//			{
//				if( _channel_nb == 4 )
//				{
//					r0 = ( TYPE_USED ) *src;
//					g0 = ( TYPE_USED ) *++src;
//					b0 = ( TYPE_USED ) *++src;
//					src += 2;
//					r1 = ( TYPE_USED ) *src;
//					g1 = ( TYPE_USED ) *++src;
//					b1 = ( TYPE_USED ) *++src;
//					src += 2;
//				}
//				else if( _channel_nb == 3 )
//				{
//					r0 = ( TYPE_USED ) *++src;
//					g0 = ( TYPE_USED ) *++src;
//					b0 = ( TYPE_USED ) *++src;
//					r1 = ( TYPE_USED ) *++src;
//					g1 = ( TYPE_USED ) *++src;
//					b1 = ( TYPE_USED ) *++src;
//				}
//				else 
//				{
//					b0 = g0 = r0 = ( TYPE_USED ) *++src;
//					b1 = g1 = r1 = ( TYPE_USED ) *++src;
//				}
//
//#if	USE_FLOAT
//				//	we do Y0 first
//				f =  0.256635*r0 +0.503971*g0 +0.098218*b0	+ 16.;
//				f = CLAMP( f, 16, 235 );
//				*( dst+0 ) = ( UINT8 ) f;
//				//	then Y1
//				f =  0.256635*r1 +0.503971*g1 +0.098218*b1	+ 16.;
//				f = CLAMP( f, 16, 235 );
//				*( dst+2 ) = ( UINT8 ) f;
//				//	then we add the 2 pixels to calculate U and V
//				//		add we will use scale down factor
//				r0 += r1;
//				g0 += g1;
//				b0 += b1;
//				//	then U
//				f = -0.0740675*r0 - 0.145451*g0 + 0.219518*b0 + 128.;
//				f = CLAMP( f, 16, 240 );
//				*( dst+1 ) = ( UINT8 ) f ^ 0x80;
//				//	then V
//				f =  0.2196635*r0 - 0.1838365*g0 - 0.0358275*b0 + 128.;
//				f = CLAMP( f, 16, 240 );
//				*( dst+3 ) = ( UINT8 ) f ^ 0x80;
//#else
//				//	we do Y0 first
//				f =  15743*r0 + 30915*g0 + 6025*b0 + 65536;
//				f = CLAMP( f, TYPE_USED( 1L<<SHIFT ), TYPE_USED( 205L<<SHIFT ) );
//				*( dst+0 ) = ( UINT8 ) ( f>>SHIFT );
//				//	then Y1
//				f =  15743*r1 + 30915*g1 + 6025*b1 + 65536;
//				f = CLAMP( f, TYPE_USED( 1L<<SHIFT ), TYPE_USED( 205L<<SHIFT ) );
//				*( dst+2 ) = ( UINT8 ) ( f>>SHIFT );
//				//	then we add the 2 pixels to calculate U and V
//				//		add we will use scale down factor
//				r0 += r1;
//				g0 += g1;
//				b0 += b1;
//				//	then U
//				f = -4854L*r0 -9532L*g0 + 14386L*b0 + ( 128L<<SHIFT );
//				f = CLAMP( f, TYPE_USED( 16L<<SHIFT ), TYPE_USED( 240L<<SHIFT ) );
//				*( dst+1 ) = ( UINT8 ) ( f>>SHIFT ) ^ 0x80;
//				//	then V
//				f =  14395L*r0 - 12048L*g0 - 2348L*b0 + ( 128L<<SHIFT );
//				f = CLAMP( f, TYPE_USED( 16L<<SHIFT ), TYPE_USED( 240L<<SHIFT ) );
//				*( dst+3 ) = ( UINT8 ) ( f>>SHIFT ) ^ 0x80;
//
//#endif
//				dst += 4;
//			}
////			}
//		}
//		set_size( get_size_x(), get_size_y(), PIXEL_FORMAT::YUY2 );
//	//	_type = TYPE_YUV;
//	//	_channel_nb = 2;
//		set_changed();
//	}
//	else
//	{
//		retcode = c_img_utils::ERR_DATA_NO;
//		goto exit;
//	}
//exit:
//	if( ERR( retcode ) )
//	{
//		ERR_PRINT_STRING( "%s() : Conversion Failed", __FUNCTION__ );
//	}
//	else
//	{
//		GOOD_PRINT_STRING( "Done" );
//	}
//
//	return retcode;
//}
//
//	////////
//	////////
////todo deal with alignment and odd/even size_x for src data
////todo	deal seriously with the file extension pb
//AAA_ERR	c_img_2d::to_yuv()
//{
//	AAA_ERR	retcode;
//	if( this )
//	{
//		switch( _pixel_format )
//		{
//		case PIXEL_FORMAT::RGBA :
//			retcode = rgb_to_yuv();
//			break;
//		case PIXEL_FORMAT::YUY2:
//			retcode = AAA_OK;
//			break;
//		default:
//			ERR_PRINT_STRING( "IMG : No converter from %s to YUV", c_pixel_format::force_type_str[ (INT32)_pixel_format ]
//					//	get_type_str( _type ),
//					//	get_type_str( TYPE_YUV )
//						);
//			print_err_unsupported_type( __FUNCTION__ );
//			retcode = c_img_utils::ERR_CANT_CONVERT;
//			break;
//		}
//	}
//	else
//	{
//		retcode = c_img_2d::ERR_NULL;
//	}
//	return retcode;
//}



FINLINE	REAL UV_TO_PIXEL( REAL CONST uv, INT32 CONST size ) { return uv * ( REAL )size; }

FINLINE	INT32	make_pixel( REAL CONST uv, INT32 CONST size )
{
	if( uv == 1. )
		return size - 1;
	return IMOD( INT32( UV_TO_PIXEL(uv,size) ), size );
}
FINLINE	INT32	make_pixel_clamped( REAL CONST uv, INT32 CONST size )
{
	if( uv >= 1. )
		return size - 1;
	if( uv <= 0. )
		return 0;
	return INT32( UV_TO_PIXEL( uv, size ) );
}

//	cn = center next
FINLINE	void	make_pixel_uv_cn_clamped( REAL& uv, INT32& c, INT32& n, INT32 CONST size )
{
	// we make sure nothing happen outside
	if( uv <= 0. )
	{
		n = c = 0;
		uv = 0;
	}
	else if( uv >= 1. )
	{
		n = c = size-1;
		uv = 0;
	}
	else
	{
		uv = UV_TO_PIXEL( uv, size );
		c = I_FLOOR( uv );
		uv -= c;
		// we treat last pixel different
		if( c==(size-1) )
			n = c;
		else
			n = c + 1;
	}
}

//	cn = center next
FINLINE	void	make_pixel_uv_cn( REAL& uv, INT32& c, INT32& n, INT32 CONST size )
{
	if( uv == 1. )
	{
		uv = 0;
		c = size - 2;
		n = size - 1; 
	}
	else
	{
		uv = UV_TO_PIXEL( uv, size );
		c = I_FLOOR( uv );
		uv -= c;
		c = IMOD( c, size );
		n = MIN( c+1, size-1 );	//todo check this
	}
}

//	cn = center next
FINLINE	void	make_pixel_cn_clamped( REAL uv, INT32& c, INT32& n, INT32 CONST size )
{
	// we make sure nothing happen outside
	if( uv <= 0. )
		c = n = 0;
	else if( uv >= 1. )
		c = n = size-1;
	else
	{
		c = I_FLOOR( UV_TO_PIXEL( uv, size ) );
		// we treat last pixel different
		if( c==(size-1) )
			n = c;
		else
			n = c + 1;
	}
}

//	cn = center next
FINLINE	void	make_pixel_cn( REAL CONST uv, INT32& c, INT32& n, INT32 CONST size )
{
	if( uv == 1. )
	{
//hack for const look weird			uv = 0;
		c = size - 2;
		n = size - 1; 
	}
	else
	{
		c = I_FLOOR( UV_TO_PIXEL( uv, size ) );
		c = IMOD( c, size );
		n = MIN( c+1, size-1 );	//todo check this
	}
}

/*
//	pcn = previous center next
FINLINE	bool	make_pixel_uv_pcn_clamped( REAL& uv, INT32& p, INT32& c, INT32& n, INT32 size )
{
	// we make sure nothing happen outside
	if( uv <= 0. )
		return false;
	else if( uv >= 1. )
		return false;
	else
	{
		uv = UV_TO_PIXEL( uv, size );
		c = I_FLOOR( uv );
		uv -= c;
		// we treat first and last pixel different
		if( c==0 )
		{
			p = 0;
			n = 1;
			return true;
		}
		else if( c==(size-1) )
		{
			p = c - 1;
			n = c;
			return true;
		}
		else
		{
			p = c - 1;
			n = c + 1;
			return true;
		}
	}
}
*/

//	PCN = Previous Center Next
FINLINE	void	make_pixel_uv_pcn(	REAL& uv, INT32& p, INT32& c, INT32& n, INT32 CONST size )
{
	if( uv == 1. )
	{
		uv = 0;
		p = size - 2;
		c = size - 1;
		n = size - 1; 
	}
	else
	{
		uv = UV_TO_PIXEL( uv, size );
		c = I_FLOOR( uv );
		uv -= c;
		c = IMOD( c, size );
		p = MAX0( c - 1 );
		n = MIN( c+1, size-1 );
	}
}

////////
////////
//todo deal with alignment and odd/even size_x for src data
FINLINE	void	c_img_2d::uv_to_pixel( INT32& iu, INT32& iv, REAL CONST u, REAL CONST v, bool CONST b_clamped ) CONST
{
	if( b_clamped )	//todo optimize
	{
		iu = make_pixel_clamped( u, get_size_x() );
		iv = make_pixel_clamped( v, get_size_y() );
	}
	else
	{
		iu = make_pixel( u, get_size_x() );
		iv = make_pixel( v, get_size_y() );
	}
}
////////
////////	return true	if there is a valid position
FINLINE	void	c_img_2d::uv_to_pixels_with_uv(	INT32& cu, INT32& cv,
												INT32& nu, INT32& nv,
												REAL& u, REAL& v, bool CONST b_clamped ) CONST
{
	if( b_clamped )	//todo optimize
	{
		make_pixel_uv_cn_clamped( u, cu, nu, get_size_x() );
		make_pixel_uv_cn_clamped( v, cv, nv, get_size_y() );
	}
	else
	{
		make_pixel_uv_cn( u, cu, nu, get_size_x() );
		make_pixel_uv_cn( v, cv, nv, get_size_y() );
	}
}

////////
////////	return true	if there is a valid position
FINLINE	void	c_img_2d::uv_to_pixels(	INT32& cu, INT32& cv,
										INT32& nu, INT32& nv,
										REAL CONST u, REAL CONST v, bool CONST b_clamped ) CONST
{
	if( b_clamped )	//todo optimize
	{
		make_pixel_cn_clamped( u, cu, nu, get_size_x() ); 
		make_pixel_cn_clamped( v, cv, nv, get_size_y() );
	}
	else
	{
		make_pixel_cn( u, cu, nu, get_size_x() );
		make_pixel_cn( v, cv, nv, get_size_y() );
	}
}

template< typename T >
FP32 get_val( T* pt, INT64 offset, aaa::PIXEL_FORMAT CONST & pf, COMPO what )
{
	if( what <= aaa::COMPO::ALPHA )
	{
		INT32 a;
		if( aaa::c_pixel_format::make_compo_index( pf, what, a ) )
		{
			pt += offset;
			return aaa::img::c_compo::to_fp32( *(pt+a) );
		}
		else
			return 1.;
	}

	if( what >= aaa::COMPO::RED_MINUS_GREEN )
	{	// all differences
		INT32 a,b;
		if( aaa::c_pixel_format::make_compo_index( pf, what, a, b ) )
		{
			pt += offset;
			return aaa::img::c_compo::sub_to_fp32( *(pt+a), *(pt+b) );
		}
		else
			return 1.;
	}

	//todo add bgr format
	switch( pf )
	{
	case PIXEL_FORMAT::R_8:			case PIXEL_FORMAT::R_16:		case PIXEL_FORMAT::R_16FP:	case PIXEL_FORMAT::R_32FP:
	case PIXEL_FORMAT::DEPTH_16:	case PIXEL_FORMAT::DEPTH_24:	case PIXEL_FORMAT::DEPTH_32:
		pt += offset;
		switch( what )
		{
		case COMPO::GREY:	
		case COMPO::MAX_COMPO:
		case COMPO::MIN_COMPO:	return aaa::img::c_compo::to_fp32( *pt );
		default:				return FP32(1);
		}
		break;
	case PIXEL_FORMAT::RG_8:	case PIXEL_FORMAT::RG_16:	case PIXEL_FORMAT::RG_16FP:	case PIXEL_FORMAT::RG_32FP:
		pt += offset;
		switch( what )
		{
		case COMPO::MAX_COMPO:	return aaa::img::c_compo::to_fp32( MAX( *pt, *(pt+1) ) );
		case COMPO::MIN_COMPO:	return aaa::img::c_compo::to_fp32( MIN( *pt, *(pt+1) ) );
		default:				return FP32(1);
		}
		break;
	case PIXEL_FORMAT::RGB_8:	case PIXEL_FORMAT::RGB_16:	case PIXEL_FORMAT::RGB_16FP:	case PIXEL_FORMAT::RGB_32FP:
	case PIXEL_FORMAT::RGBA_8:	case PIXEL_FORMAT::RGBA_16:	case PIXEL_FORMAT::RGBA_16FP:	case PIXEL_FORMAT::RGBA_32FP:
	case PIXEL_FORMAT::BGR_8:	case PIXEL_FORMAT::BGR_16:	case PIXEL_FORMAT::BGR_16FP:	case PIXEL_FORMAT::BGR_32FP:
	case PIXEL_FORMAT::BGRA_8:	case PIXEL_FORMAT::BGRA_16:	case PIXEL_FORMAT::BGRA_16FP:	case PIXEL_FORMAT::BGRA_32FP:
		pt += offset;
		//todoq precompute byteperline
		switch( what )
		{
		case COMPO::GREY:
			{
				INT32 a,b;
				if( aaa::c_pixel_format::make_compo_index( pf, what, a, b ) )
					return aaa::img::c_compo::to_fp32( T( aaa::color::rgb_to_grey( *(pt+a), *(pt+1), *(pt+b) ) ) );
				else
					return FP32(1);
			}
			break;
		case COMPO::MAX_COMPO:	return aaa::img::c_compo::to_fp32( aaa::MAX( *pt, *(pt+1), *(pt+2) ) );
		case COMPO::MIN_COMPO:	return aaa::img::c_compo::to_fp32( aaa::MIN( *pt, *(pt+1), *(pt+2) ) );
		default:				return FP32(1);
		}
		break;
	default:
//		print_err_unsupported_format( __FUNCTION__ );
		return FP32(1);
	}
}

//
//	GET FROM X Y
//
template < aaa::PIXEL_TYPE type >
FINLINE FP32	c_img_2d::get_valid_value_from_xy_low( INT32 CONST x, INT32 CONST y, COMPO CONST what ) CONST
{
	INT64 offset = get_byte_offset( x, y );
	switch( type )
	{
	case aaa::PIXEL_TYPE::UINT_8:	return get_val<UINT8 >( get_data_uint8 (), offset,    _pixel_format, what );
	case aaa::PIXEL_TYPE::UINT_16:	return get_val<UINT16>( get_data_uint16(), offset>>1, _pixel_format, what );
	case aaa::PIXEL_TYPE::FLOAT_16:	return get_val<FP16  >( get_data_fp16(),   offset>>1, _pixel_format, what );
	case aaa::PIXEL_TYPE::FLOAT_32:	return get_val<FP32  >( get_data_fp32(),   offset>>2, _pixel_format, what );
	default:
		print_err_unsupported_type( __FUNCTION__ );
		return 1.;
	}
}
FINLINE FP32	c_img_2d::get_valid_value_from_xy( INT32 CONST x, INT32 CONST y, COMPO CONST what ) CONST
{
	//using namespace aaa;
	//	make sure than the img stay on cpu
	if( !aaa::c_pixel_format::is_format_rgb( _pixel_format ) )
	{
		print_err_unimplemented( __FUNCTION__, "rgb" );
		return 1.;
	}

	switch( get_data_type() )
	{
	case aaa::PIXEL_TYPE::UINT_8:	return get_valid_value_from_xy_low< aaa::PIXEL_TYPE::UINT_8   >( x,y, what );
	case aaa::PIXEL_TYPE::UINT_16:	return get_valid_value_from_xy_low< aaa::PIXEL_TYPE::UINT_16  >( x,y, what );
	case aaa::PIXEL_TYPE::FLOAT_16:	return get_valid_value_from_xy_low< aaa::PIXEL_TYPE::FLOAT_16 >( x,y, what );
	case aaa::PIXEL_TYPE::FLOAT_32:	return get_valid_value_from_xy_low< aaa::PIXEL_TYPE::FLOAT_32 >( x,y, what );
	default:
		print_err_unimplemented( __FUNCTION__, "i8, i16, fp16, fp32" );
		return 1.;
	}
}
FP32	c_img_2d::get_value_from_xy(			INT32 CONST x,	INT32 CONST y,		COMPO CONST what	)
{
	if( is_valid_xy( x,y ) )	// check bounds
	{
		set_cpu_keep( true );
		return get_valid_value_from_xy( x,y, what );
	}
	return 1.;
}

template< typename T >
FINLINE void	get_color3r_from_pt( FP32* color, T CONST * src, UINT64 CONST offset, INT32 CONST channel_nb )
{
	if( src )
	{
		src += offset;
		switch( channel_nb )
		{
		case 1:
			{
				FP32	r = aaa::img::c_compo::to_fp32( *src );
				*color	 = r;
				*++color = r;
				*++color = r;
			}
			break;
		case 2:
			*color	 = aaa::img::c_compo::to_fp32( *src		);
			*++color = aaa::img::c_compo::to_fp32( *++src	);
			*++color = 0.;
			break;
		case 3:
		case 4:
			*color	 = aaa::img::c_compo::to_fp32( *src		);
			*++color = aaa::img::c_compo::to_fp32( *++src	);
			*++color = aaa::img::c_compo::to_fp32( *++src	);
			break;
		default:	//todo
			ERR_PRINT_STRING( "%s() not implemented with a channel nb of %d", __FUNCTION__, channel_nb );
			clear_v3( color );
			break;
		}
	}
	else
	{
		set_v3( color, FP32(1.) );
	}
}

void	c_img_2d::get_color3r_from_xy_low(		FP32* CONST color,	INT32 CONST x, INT32 CONST y ) CONST
{
	if( !aaa::c_pixel_format::is_format_rgb( _pixel_format ) )
	{
		print_err_unsupported_format( __FUNCTION__ );
		clear_v3( color );
		return;
	}

	switch( get_data_type() )
	{
	case aaa::PIXEL_TYPE::UINT_8:
		get_color3r_from_pt( color, get_data_uint8(),	get_byte_offset(x,y),		_channel_nb );
		return;
	case aaa::PIXEL_TYPE::UINT_16:
		get_color3r_from_pt( color, get_data_uint16(),	get_byte_offset(x,y)>>1,	_channel_nb );
		return;
	case aaa::PIXEL_TYPE::FLOAT_16:
		get_color3r_from_pt( color, get_data_fp16(),	get_byte_offset(x,y)>>1,	_channel_nb );
		return;
	case aaa::PIXEL_TYPE::FLOAT_32:
		get_color3r_from_pt( color, get_data_fp32(),	get_byte_offset(x,y)>>2,	_channel_nb );
		return;
	default:
		clear_v3( color );
		print_err_unsupported_type( __FUNCTION__ );
		break;
	}
//	case TYPE_YUV:	//todo
	return;
}

void	c_img_2d::get_color3r_from_xy(		FP32* color,	INT32 CONST x, INT32 CONST y )
{
	// check bounds
	if( is_valid_xy( x,y ) )
	{
		get_color3r_from_xy_low( color, x,y );
		return;
	}

	clear_v3( color );
	return;
}

void	c_img_2d::get_color4r_from_xy_low(		FP32* color,	INT32 CONST x, INT32 CONST y ) CONST
{
	switch( _pixel_format )
	{
//	8 bits
	case aaa::PIXEL_FORMAT::R_8:
		{
		FP32	r = aaa::img::c_compo::to_fp32( *( (UINT8*)get_data() + get_byte_offset(x,y) ) );
		*color	 = r;
		*++color = r;
		*++color = r;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RG_8:
		{
		UINT8*	p = (UINT8*)get_data() + get_byte_offset( x,y );
		*color	 = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = 0.;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RGB_8:
		{
		UINT8*	p = (UINT8*)get_data() + get_byte_offset( x,y );
		*color	 = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::BGR_8:
		{
		UINT8*	p = (UINT8*)get_data() + get_byte_offset( x,y );
		*color	 = aaa::img::c_compo::to_fp32(*(p+2) );
		*++color = aaa::img::c_compo::to_fp32(*(p+1) );
		*++color = aaa::img::c_compo::to_fp32(*p );
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RGBA_8:
		{
		UINT8*	p = (UINT8*)get_data() + get_byte_offset( x,y );
		*color	 = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		}
		break;
	case aaa::PIXEL_FORMAT::BGRA_8:
		{
		UINT8*	p = (UINT8*)get_data() + get_byte_offset( x,y );
		*color	 = aaa::img::c_compo::to_fp32(*(p+2));
		*++color = aaa::img::c_compo::to_fp32(*(p+1));
		*++color = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*(p+3));
		}
		break;
//	16 bits
	case aaa::PIXEL_FORMAT::R_16:
		{
		FP32	r = aaa::img::c_compo::to_fp32( *( (UINT16*) ( (UINT8*)get_data() + get_byte_offset(x,y) ) ) );
		*color	 = r;
		*++color = r;
		*++color = r;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RG_16:
		{
		UINT16*	p = (UINT16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = 0.;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RGB_16:
		{
		UINT16*	p = (UINT16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::BGR_16:
		{
		UINT16*	p = (UINT16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*(p+2) );
		*++color = aaa::img::c_compo::to_fp32(*(p+1) );
		*++color = aaa::img::c_compo::to_fp32(*p );
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RGBA_16:
		{
		UINT16*	p = (UINT16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		}
		break;
	case aaa::PIXEL_FORMAT::BGRA_16:
		{
		UINT16*	p = (UINT16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*(p+2));
		*++color = aaa::img::c_compo::to_fp32(*(p+1));
		*++color = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*(p+3));
		}
		break;
//	float 16 bits
	case aaa::PIXEL_FORMAT::DEPTH_16:
	case aaa::PIXEL_FORMAT::R_16FP:
		{
		FP32	r = aaa::img::c_compo::to_fp32( *( (FP16*) ( (UINT8*)get_data() + get_byte_offset( x,y ) ) ) );
		*color	 = r;
		*++color = r;
		*++color = r;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RG_16FP:
		{
		FP16*	p = (FP16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = 0.;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RGB_16FP:
		{
		FP16*	p = (FP16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::BGR_16FP:
		{
		FP16*	p = (FP16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*(p+2) );
		*++color = aaa::img::c_compo::to_fp32(*(p+1) );
		*++color = aaa::img::c_compo::to_fp32(*p );
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RGBA_16FP:
		{
		FP16*	p = (FP16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		*++color = aaa::img::c_compo::to_fp32(*++p);
		}
		break;
	case aaa::PIXEL_FORMAT::BGRA_16FP:
		{
		FP16*	p = (FP16*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = aaa::img::c_compo::to_fp32(*(p+2));
		*++color = aaa::img::c_compo::to_fp32(*(p+1));
		*++color = aaa::img::c_compo::to_fp32(*p);
		*++color = aaa::img::c_compo::to_fp32(*(p+3));		}
		break;
//	float 32 bits
	case aaa::PIXEL_FORMAT::DEPTH_32:
	case aaa::PIXEL_FORMAT::R_32FP:
		{
		FP32	r = *( (FP32*) ((UINT8*)get_data() + get_byte_offset( x,y ) ) );
		*color	 = r;
		*++color = r;
		*++color = r;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RG_32FP:
		{
		FP32*	p = (FP32*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = *p;
		*++color = *++p;
		*++color = 0.;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RGB_32FP:
		{
		FP32*	p = (FP32*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = *p;
		*++color = *++p;
		*++color = *++p;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::BGR_32FP:
		{
		FP32*	p = (FP32*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = *(p+2);
		*++color = *(p+1);
		*++color = *p;
		*++color = 1.;
		}
		break;
	case aaa::PIXEL_FORMAT::RGBA_32FP:
		{
		FP32*	p = (FP32*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = *p;
		*++color = *++p;
		*++color = *++p;
		*++color = *++p;
		}
		break;
	case aaa::PIXEL_FORMAT::BGRA_32FP:
		{
		FP32*	p = (FP32*) ((UINT8*)get_data() + get_byte_offset( x,y ));
		*color	 = *(p+2);
		*++color = *(p+1);
		*++color = *p;
		*++color = *(p+3);		}
		break;
	default:
		clear_v4( color );
		print_err_unsupported_format( __FUNCTION__ );
		break;
	}
}

void	c_img_2d::get_color4r_from_xy(		FP32* color,	INT32 CONST x, INT32 CONST y )
{
	// check bounds
	if( is_valid() && is_valid_xy( x,y ) )
//	if( is_valid_xy( x,y ) )
	{
		get_color4r_from_xy_low( color, x,y );
		return;
	}

	clear_v4( color );
	return;
}

void	c_img_2d::set_xy_color3r_low(	INT32 CONST x, INT32 CONST y, FP32 CONST * CONST color )
{
	//todo bgr ?
	if( c_pixel_format::is_format_rgb( _pixel_format ) )
	{
		auto type = get_data_type();
		switch( type )
		{
		case aaa::PIXEL_TYPE::UINT_8:
			{
				UINT8*	dst = (UINT8*)get_data() + get_byte_offset( x,y );
				switch( _channel_nb )
				{
				case 1:	*dst	= aaa::img::c_compo::to_uint8( color[0] );	set_changed();	break;
				case 2:	*dst	= aaa::img::c_compo::to_uint8( color[0] );
						*++dst	= aaa::img::c_compo::to_uint8( color[1] );	set_changed();	break;
				case 4:
				case 3:	*dst	= aaa::img::c_compo::to_uint8( color[0] );
						*++dst	= aaa::img::c_compo::to_uint8( color[1] );
						*++dst	= aaa::img::c_compo::to_uint8( color[2] );	set_changed();	break;
				default:
					print_err_unsupported_channel_nb( __FUNCTION__ );
					break;
				}

			}
			break;
		case aaa::PIXEL_TYPE::UINT_16:
			{
				UINT16*	dst = (UINT16*)get_data() + (get_byte_offset( x,y )>>1);
				switch( _channel_nb )
				{
				case 1:	*dst	= aaa::img::c_compo::to_uint16( color[0] );	set_changed();	break;
				case 2:	*dst	= aaa::img::c_compo::to_uint16( color[0] );
						*++dst	= aaa::img::c_compo::to_uint16( color[1] );	set_changed();	break;
				case 4:
				case 3:	*dst	= aaa::img::c_compo::to_uint16( color[0] );
						*++dst	= aaa::img::c_compo::to_uint16( color[1] );
						*++dst	= aaa::img::c_compo::to_uint16( color[2] );	set_changed();	break;
				default:
					print_err_unsupported_channel_nb( __FUNCTION__ );
					break;
				}

			}
			break;
		case aaa::PIXEL_TYPE::FLOAT_16:
			{
				FP16*	dst = (FP16*)get_data() + (get_byte_offset( x,y )>>1);
				switch( _channel_nb )
				{
				case 1:	*dst	= aaa::img::c_compo::to_fp16( color[0] );	set_changed();	break;
				case 2:	*dst	= aaa::img::c_compo::to_fp16( color[0] );
						*++dst	= aaa::img::c_compo::to_fp16( color[1] );	set_changed();	break;
				case 4:
				case 3:	*dst	= aaa::img::c_compo::to_fp16( color[0] );
						*++dst	= aaa::img::c_compo::to_fp16( color[1] );
						*++dst	= aaa::img::c_compo::to_fp16( color[2] );	set_changed();	break;
				default:
					print_err_unsupported_channel_nb( __FUNCTION__ );
					break;
				}
			}
			break;
		case aaa::PIXEL_TYPE::FLOAT_32:
			{
				FP32*	dst = (FP32*)get_data() + (get_byte_offset( x,y )>>2);
				switch( _channel_nb )
				{
				case 1:	*dst	= color[0];	set_changed();	break;
				case 2:	*dst	= color[0];
						*++dst	= color[1];	set_changed();	break;
				case 4:
				case 3:	*dst	= color[0];
						*++dst	= color[1];
						*++dst	= color[2];	set_changed();	break;
				default:
					print_err_unsupported_channel_nb( __FUNCTION__ );
					break;
				}
			}
			break;
		default:
			print_err_unsupported_type( __FUNCTION__ );
			break;
		}
	}
	else
	{
		//todo
		print_err_unsupported_format( __FUNCTION__ );
	}
}
void	c_img_2d::set_xy_color3r(	INT32 CONST x, INT32 CONST y, FP32 CONST * CONST color )
{
	// check bounds
	if( is_valid_xy( x,y ) )
	{
		set_xy_color3r_low(	x,y, color );
	}
}

void	c_img_2d::set_xy_color4r_low(	INT32 CONST x, INT32 CONST y, FP32 CONST * CONST color )
{
	//todo bgr ?
	if( c_pixel_format::is_format_rgb( _pixel_format ) )
	{
		auto type = get_data_type();
		switch( type )
		{
		case aaa::PIXEL_TYPE::UINT_8:
			{
				UINT8*	dst = (UINT8*)get_data() + get_byte_offset( x,y );
				switch( _channel_nb )
				{
				case 1:	*dst	= aaa::img::c_compo::to_uint8( color[0] );
						set_changed();
						break;
				case 2:	*dst	= aaa::img::c_compo::to_uint8( color[0] );
						*++dst	= aaa::img::c_compo::to_uint8( color[1] );
						set_changed();
						break;
				case 3:	*dst	= aaa::img::c_compo::to_uint8( color[0] );
						*++dst	= aaa::img::c_compo::to_uint8( color[1] );
						*++dst	= aaa::img::c_compo::to_uint8( color[2] );
						set_changed();
						break;
				case 4:	*dst	= aaa::img::c_compo::to_uint8( color[0] );
						*++dst	= aaa::img::c_compo::to_uint8( color[1] );
						*++dst	= aaa::img::c_compo::to_uint8( color[2] );
						*++dst	= aaa::img::c_compo::to_uint8( color[3] );
						set_changed();
						break;
				default:
					print_err_unsupported_channel_nb( __FUNCTION__ );
					break;
				}
			}
			break;
		case aaa::PIXEL_TYPE::UINT_16:
			{
				UINT16*	dst = (UINT16*)( (UINT8*)get_data() + get_byte_offset( x,y ) );
				switch( _channel_nb )
				{
				case 1:	*dst	= aaa::img::c_compo::to_uint16( color[0] );
						set_changed();
						break;
				case 2:	*dst	= aaa::img::c_compo::to_uint16( color[0] );
						*++dst	= aaa::img::c_compo::to_uint16( color[1] );
						set_changed();
						break;
				case 3:	*dst	= aaa::img::c_compo::to_uint16( color[0] );
						*++dst	= aaa::img::c_compo::to_uint16( color[1] );
						*++dst	= aaa::img::c_compo::to_uint16( color[2] );
						set_changed();
						break;
				case 4:	*dst	= aaa::img::c_compo::to_uint16( color[0] );
						*++dst	= aaa::img::c_compo::to_uint16( color[1] );
						*++dst	= aaa::img::c_compo::to_uint16( color[2] );
						*++dst	= aaa::img::c_compo::to_uint16( color[3] );
						set_changed();
						break;
				default:
					print_err_unsupported_channel_nb( __FUNCTION__ );
					break;
				}
			}
			break;
		case aaa::PIXEL_TYPE::FLOAT_16:
			{
				FP16*	dst = (FP16*)( (UINT8*)get_data() + get_byte_offset( x,y ) );
				switch( _channel_nb )
				{
				case 1:	*dst	= aaa::img::c_compo::to_fp16( color[0] );
						set_changed();
						break;
				case 2:	*dst	= aaa::img::c_compo::to_fp16( color[0] );
						*++dst	= aaa::img::c_compo::to_fp16( color[1] );
						set_changed();
						break;
				case 3:	*dst	= aaa::img::c_compo::to_fp16( color[0] );
						*++dst	= aaa::img::c_compo::to_fp16( color[1] );
						*++dst	= aaa::img::c_compo::to_fp16( color[2] );
						set_changed();
						break;
				case 4:	*dst	= aaa::img::c_compo::to_fp16( color[0] );
						*++dst	= aaa::img::c_compo::to_fp16( color[1] );
						*++dst	= aaa::img::c_compo::to_fp16( color[2] );
						*++dst	= aaa::img::c_compo::to_fp16( color[3] );
						set_changed();
						break;
				default:
					print_err_unsupported_channel_nb( __FUNCTION__ );
					break;
				}
			}
			break;
		case aaa::PIXEL_TYPE::FLOAT_32:
			{
				FP32*	dst = (FP32*)( (UINT8*)get_data() + get_byte_offset( x,y ) );
				switch( _channel_nb )
				{
				case 1:	*dst	= color[0];	set_changed();
						break;
				case 2:	*dst	= color[0];
						*++dst	= color[1];	set_changed();
						break;
				case 3:	*dst	= color[0];
						*++dst	= color[1];
						*++dst	= color[2];	set_changed();
						break;
				case 4:	*dst	= color[0];
						*++dst	= color[1];
						*++dst	= color[2];
						*++dst	= color[3];	set_changed();\
						break;
				default:
					print_err_unsupported_channel_nb( __FUNCTION__ );
					break;
				}
			}
			break;
		default:
			print_err_unsupported_type( __FUNCTION__ );
			break;
		}
	}
	else
	{
		//todo
		print_err_unsupported_format( __FUNCTION__ );
	}
}

void	c_img_2d::set_xy_color4r(	INT32 CONST x, INT32 CONST y, FP32 CONST * CONST color )
{
	// check bounds
	if( is_valid_xy( x,y ) )
	{
		set_xy_color4r_low(	x,y, color );
	}
}

/*
////////
////////	return true	if there is a valid position
FINLINE	bool	c_img_2d::uv_to_pixels( 	INT32& pu, INT32& pv,
c_img_2dINT32& cu, INT32& cv,
										INT32& nu, INT32& nv,
										REAL& u, REAL& v, bool b_clamped )
{
	if( b_clamped )	//todo optimize
	{
		if( make_pixel_uv_pcn_clamped( u, pu, cu, nu, get_size_x() ) && make_pixel_uv_pcn_clamped( v, pv, cv, nv, get_size_y() ) )
			return true;
		else
			return false;
	}
	else
	{
		make_pixel_uv_pcn( u, pu, cu, nu, get_size_x() );
		make_pixel_uv_pcn( v, pv, cv, nv, get_size_y() );
		return true;
	}
}
*/

//
//	GET from u v
//

void	c_img_2d::get_valid_color3r_from_uv_linear(	FP32* color,	REAL u,	REAL v,	bool CONST b_clamped ) CONST
{
	INT32	cx, cy; 
	INT32	nx, ny;
	//	cx,ny  ---  nx,ny
	//	        |
	//	cx,cy  ---  nx,cy
	uv_to_pixels_with_uv( cx,cy, nx,ny, u,v, b_clamped );

	FP32 col[4][4];
	get_color3r_from_xy_low( col[0], cx,cy );
	get_color3r_from_xy_low( col[1], nx,cy );
	interpolate_v3( col[0], col[1], u );

	get_color3r_from_xy_low( col[2], cx,ny );
	get_color3r_from_xy_low( col[3], nx,ny );
	interpolate_v3( col[2], col[3], u );

	interpolate_v3( col[0], col[2], v );
	cpy_v3( color, col[0] );
}
void	c_img_2d::get_valid_color4r_from_uv_linear(	FP32* color,	REAL u,	REAL v,	bool CONST b_clamped ) CONST
{
	INT32	cx, cy; 
	INT32	nx, ny;
	//	cx,ny  ---  nx,ny
	//	        |
	//	cx,cy  ---  nx,cy
	uv_to_pixels_with_uv( cx, cy, nx, ny, u, v, b_clamped );

	FP32 col[4][4];
	get_color4r_from_xy_low( col[0], cx,cy );
	get_color4r_from_xy_low( col[1], nx,cy );
	interpolate_v4( col[0], col[1], u );

	get_color4r_from_xy_low( col[2], cx,ny );
	get_color4r_from_xy_low( col[3], nx,ny );
	interpolate_v4( col[2], col[3], u );

	interpolate_v4( col[0], col[2], v );
	cpy_v4( color, col[0] );
}

//todo deal with alignment and odd/even size_x for src data
//
FINLINE FP32	c_img_2d::get_valid_value_from_uv_linear( REAL u, REAL v, bool CONST b_clamped, COMPO CONST what ) CONST
{
	INT32	cx, cy; 
	INT32	nx, ny;
	//	cx,ny  ---  nx,ny
	//	        |
	//	cx,cy  ---  nx,cy
	uv_to_pixels_with_uv( cx,cy, nx,ny, u,v, b_clamped );

	FP32	a, b;
	a = interpolate( get_valid_value_from_xy( cx,cy, what ), get_valid_value_from_xy( nx,cy, what ), u );
	b = interpolate( get_valid_value_from_xy( cx,ny, what ), get_valid_value_from_xy( nx,ny, what ), u );

	return interpolate( a, b, v );
}

//todo deal with alignment and odd/even size_x for src data
FINLINE FP32	c_img_2d::get_valid_value_from_uv_nearest( REAL CONST u, REAL CONST v, bool CONST b_clamped, COMPO CONST what ) CONST
{
	INT32	x, y;
	uv_to_pixel( x,y, u,v, b_clamped );
	return get_valid_value_from_xy( x,y, what );
}

FP32	c_img_2d::get_valid_value_from_uv( REAL CONST u, REAL CONST v, bool CONST b_clamped, aaa::COMPO CONST what,	bool CONST b_linear ) CONST
{
	return	b_linear
			? get_valid_value_from_uv_linear(  u,v, b_clamped, what )
			: get_valid_value_from_uv_nearest( u,v, b_clamped, what );
}

void	c_img_2d::get_color3r_from_uv_linear( FP32* color, REAL CONST u, REAL CONST v, bool CONST b_clamped )
{
	if( is_valid() )	//	we need max speed here so no check_valid()
	{
		get_valid_color3r_from_uv_linear( color, u,v, b_clamped );
		return;
	}
	clear_v3( color );
	return;
}
void	c_img_2d::get_color4r_from_uv_linear( FP32* color, REAL CONST u, REAL CONST v, bool CONST b_clamped )
{
	if( is_valid() )	//	we need max speed here so no check_valid()
	{
		get_valid_color4r_from_uv_linear( color, u,v, b_clamped );
		return;
	}
	clear_v4( color );
	return;
}

FINLINE	FP32	c_img_2d::get_value_max_from_line_low( INT32 CONST x_begin, INT32 CONST x_end, INT32 CONST y, CONST COMPO what )
{
	//todo do with float
	//todo do bgr mode
	//todo check
	set_cpu_keep( true );	//	make sure than the img stay on cpu
	UINT8 imax = 0;
	if( c_pixel_format::is_format_rgb_i8( _pixel_format ) )
	{
		//todoq precompute byteperline
			//	validity check is done on caller
			UINT8*	p = get_data_uint8() + get_byte_offset( x_begin, y );
			INT32	i = x_end - x_begin + 1;
			
			if( _channel_nb == 1 )
			{
				switch( what )
				{
				case COMPO::RED:		case COMPO::GREEN:		case COMPO::BLUE:
				case COMPO::ALPHA:		case COMPO::GREY:
				case COMPO::MAX_COMPO:	case COMPO::MIN_COMPO:
					--p;
					do
					{
						if( imax < *++p )
							imax = *p;
					}
					while( --i > 0 );
					break;
				}
			}
			else
			{
				switch( what )
				{
				case COMPO::RED:
				case COMPO::GREEN:
				case COMPO::BLUE:
				case COMPO::ALPHA:
					if( what == COMPO::ALPHA && _channel_nb != 4 )
					{
						imax = 255;
						break;
					}
					p = p + INT32(what);
					do
					{
						if( imax < *p )
							imax = *p;
						p += _channel_nb;
					}
					while( --i > 0 );
					break;
				case COMPO::GREY:
					do
					{
						INT32 tmp = aaa::color::rgb_to_grey( *p, *(p+1), *(p+2) );
						if( imax < tmp )
							imax = tmp;
						p += _channel_nb;
					}
					while( --i > 0 );
					break;
				case COMPO::MAX_COMPO:
					if( _channel_nb== 2 )
					{
						do
						{
							INT32 tmp = MAX( *p, *(p+1) );
							if( imax < tmp )
								imax = tmp;
							p += 2;
						}
						while( --i > 0 );
					}
					else
					{
						do
						{
							INT32 tmp = aaa::MAX( *p, *(p+1), *(p+2) );
							if( imax < tmp )
								imax = tmp;
							p += _channel_nb;
						}
						while( --i > 0 );
					}
					break;
				case COMPO::MIN_COMPO:
					if( _channel_nb== 2 )
					{
						do
						{
							INT32 tmp = MIN( *p, *(p+1) );
							if( imax < tmp )
								imax = tmp;
							p += 2;
						}
						while( --i > 0 );
					}
					else
					{
						do
						{
							INT32 tmp = aaa::MIN( *p, *(p+1), *(p+2) );
							if( imax < tmp )
								imax = tmp;
							p += _channel_nb;
						}
						while( --i > 0 );
					}
					break;
				}
			}
		}
//		break;
//	case TYPE_YUV:
//	default:
	else
	{
		print_err_unsupported_format( __FUNCTION__ );
	}
	return aaa::img::c_compo::to_fp32(imax);
}

FP32	c_img_2d::get_value_max_from_rect( INT32 x_begin, INT32 x_end, INT32 y_begin, INT32 y_end, CONST COMPO what )
{
	if( !get_data_valid_rgb_uint8( __FUNCTION__ ) )
		return 0.;

	INT32 t = get_size_x()-1;
	CLAMP_REF( x_begin,	0, t );
	CLAMP_REF( x_end,	0, t );
	FORCE_MIN_MAX( x_begin, x_end );
	t = get_size_y()-1;
	CLAMP_REF( y_begin,	0, t );
	CLAMP_REF( y_end,	0, t );
	FORCE_MIN_MAX( y_begin, y_end );

	FP32 max = 0.;
	for( INT32 y=y_begin; y<=y_end; ++y )
	{
		//todo really compute a grey
		max = MAX( max, get_value_max_from_line_low( x_begin, x_end, y, what ) );
	}
	return max;
}

void	c_img_2d::get_valid_gradient_from_uv_nearest( REAL* dst, REAL CONST u, REAL CONST v, bool CONST b_clamped, CONST COMPO what ) CONST
{
	INT32	cx, cy;
	INT32	nx, ny;

	uv_to_pixels( cx, cy, nx, ny, u, v, b_clamped );

#if TRUE
	REAL a = get_valid_value_from_xy( nx,ny, what ) - get_valid_value_from_xy( cx,cy, what );
	REAL b = get_valid_value_from_xy( nx,cy, what ) - get_valid_value_from_xy( cx,ny, what );
		
	*dst		= ( a + b ) * REAL(.5);
	*( dst+1 )	= ( a - b ) * REAL(.5);
#else
	REAL	c;	//	v	e
	REAL	e;	//	c	u
	c = get_valid_value_from_xy( cx,cy, what );
	u = get_valid_value_from_xy( nx,cy, what );
	v = get_valid_value_from_xy( cx,ny, what );
	e = get_valid_value_from_xy( nx,ny, what );

	*dst		= (u - c + e - v) * REAL(.5);
	*( dst+1 )	= (v - c + e - u) * REAL(.5);
#endif
}

void	c_img_2d::get_valid_gradient_from_uv_linear( REAL* dst, REAL CONST u, REAL CONST v, bool CONST b_clamped, CONST COMPO what ) CONST
{
	FP32	d;
	d = FP32(.5) * get_size_x_over();
	REAL	pu = get_valid_value_from_uv_linear( u-d, v, b_clamped, what );
	REAL	nu = get_valid_value_from_uv_linear( u+d, v, b_clamped, what );
	d = REAL(.5) * get_size_y_over();
	REAL	pv = get_valid_value_from_uv_linear( u, v-d, b_clamped, what );
	REAL	nv = get_valid_value_from_uv_linear( u, v+d, b_clamped, what );
	*dst		= nu-pu;
	*( dst+1 )	= nv-pv;
}

/*
void		c_img_2d::get_gradient_from_uv_linear( REAL* dst, REAL u, REAL v, bool b_clamped, COMPO what )
{
	if( this && is_data_valid() )
	{
		INT32	px, py; 
		INT32	cx, cy; 
		INT32	nx, ny;

		//	a0		a1		a2
		//	b0	c0	b1	c1	b2
		//	a3		a4		a5
		//			b4	c2	b5
		//			a7		a8
		REAL	a[9];

		if( uv_to_pixels( px, py, cx, cy, nx, ny, u, v, b_clamped ) )
		{
			a[0] = get_value_from_xy( px, ny, what );
			a[1] = get_value_from_xy( cx, ny, what );
			a[2] = get_value_from_xy( nx, ny, what );

			a[3] = get_value_from_xy( px, cy, what );
			a[4] = get_value_from_xy( cx, cy, what );
			a[5] = get_value_from_xy( nx, cy, what );

			//a[6] = get_value_from_xy( px, py, what );
			a[7] = get_value_from_xy( cx, py, what );
			a[8] = get_value_from_xy( nx, py, what );

			a[0] = interpolate( a[3], a[0], v );
			a[1] = interpolate( a[4], a[1], v );
			a[2] = interpolate( a[5], a[2], v );

			a[4] = interpolate( a[7], a[4], v );
			a[5] = interpolate( a[8], a[5], v );

			a[0] = interpolate( a[0], a[1], u );
			a[1] = interpolate( a[1], a[2], u );
			a[2] = interpolate( a[4], a[5], u );

			*dst	= a[1] - a[0];
			*( dst+1 )= a[1] - a[2];
		}
		else
		{
			*dst		= 0.;
			*( dst+1 )	= 0.;
		}
	}
	else
	{
		*dst		= 0;
		*( dst+1 )	= 0;
	}
}

*/
void	c_img_2d::get_valid_color3r_from_uv_nearest( FP32* color, REAL CONST u, REAL CONST v, bool CONST b_clamped ) CONST
{
	if( c_pixel_format::is_format_rgb( _pixel_format ) )
	{
		INT32	iu, iv;
		uv_to_pixel( iu,iv, u,v, b_clamped );
		get_color3r_from_xy_low( color,	iu,iv );
	}
	else
		print_err_unsupported_format( __FUNCTION__ );
}

//	kind of hack low level for voronoi
void	c_img_2d::get_valid_color4r_from_uv_nearest( FP32* color, REAL CONST u, REAL CONST v, bool CONST b_clamped  ) CONST
{
	if( c_pixel_format::is_format_rgb( _pixel_format ) )
	{
		INT32	iu, iv;
		uv_to_pixel( iu,iv, u,v, b_clamped );
		get_color4r_from_xy_low( color, iu,iv );
	}
	else
	{
		print_err_unsupported_format( __FUNCTION__ );
	}
}

void	c_img_2d::get_color3r_from_uv_nearest( FP32 *color, REAL CONST u, REAL CONST v, bool CONST b_clamped  )
{
	if( !is_ok() || !is_data_valid(__FUNCTION__) )	//todoqq reload if data cleared
		return;
	set_cpu_keep( true );	//	make sure than the img stay on cpu
	get_valid_color3r_from_uv_nearest( color, u,v, b_clamped  );
}
void	c_img_2d::get_color4r_from_uv_nearest( FP32 *color, REAL CONST u, REAL CONST v, bool CONST b_clamped  )
{
	if( !is_ok() || !is_data_valid(__FUNCTION__) )	//todoqq reload if data cleared
		return;
	set_cpu_keep( true );	//	make sure than the img stay on cpu
	get_valid_color4r_from_uv_nearest( color, u,v, b_clamped  );
}

UINT8*	c_img_2d::get_color4ubv_from_uv( REAL CONST u, REAL CONST v )
{
	if( is_ok() && is_data_valid(__FUNCTION__) )	//todoqq reload if data cleared
	{
		set_cpu_keep( true );	//	make sure than the img stay on cpu
		if( c_pixel_format::is_format_rgb_i8( _pixel_format ) )
		{
			INT32	iu; 
			INT32	iv; 
			uv_to_pixel( iu,iv, u,v, false );
			//todoq precompute byteperline
			//	checked by caller
			return get_data_uint8() + get_byte_offset( iu,iv );
		}
		else
			print_err_unsupported_format( __FUNCTION__ );
	}
	return nullptr;
}

//hack clean it
//	kind of hack low level for voronoi
#include "gol/gol.h"

void	c_img_2d::mul_valid_and_set_color4v_from_uv( FP32 CONST * CONST color, REAL CONST u, REAL CONST v, bool CONST b_clamped  )
{
	if( c_pixel_format::is_format_rgb_i8( _pixel_format ) )
//	{
//	case TYPE_RGB:
	{
		INT32	iu, iv;
		uv_to_pixel( iu,iv, u,v, b_clamped );

		UINT8*	p = get_data_rgb_uint8();
		if( !p )
		{
			print_err_unimplemented( __FUNCTION__, "rgb 8bits" );
			return;
		}

		p += get_byte_offset( iu,iv );
		switch( _channel_nb )
		{
		case 1:
			{
			FP32	r = aaa::img::c_compo::to_fp32(*p);
			GOL::color4(	r * color[0],
							r * color[1],
							r * color[2],
							color[3]
				);
			}
			break;
		case 2:
			GOL::color4(	aaa::img::c_compo::to_fp32(p[0]) * color[0],
							aaa::img::c_compo::to_fp32(p[1]) * color[1],
							color[2],
							color[3]
						);
			break;
		case 3:
			GOL::color4(	aaa::img::c_compo::to_fp32(p[0]) * color[0],
							aaa::img::c_compo::to_fp32(p[1]) * color[1],
							aaa::img::c_compo::to_fp32(p[2]) * color[2],
							color[3]
						);
			break;
		case 4:
			GOL::color4(	aaa::img::c_compo::to_fp32(p[0]) * color[0],
							aaa::img::c_compo::to_fp32(p[1]) * color[1],
							aaa::img::c_compo::to_fp32(p[2]) * color[2],
							aaa::img::c_compo::to_fp32(p[3]) * color[3]
						);
			break;
		default:
			GOL::color4v( color );
			print_err_unsupported_channel_nb( __FUNCTION__ );
			break;
		}
	}
	else
	{
		GOL::color4v( color );
		print_err_unsupported_format( __FUNCTION__ );
	}
}	

// ELLIPSE
FP32	c_img_2d::get_valid_value_from_uv_ellipse( REAL CONST u, REAL CONST v, REAL CONST ru, REAL CONST rv, INT32 nb, bool CONST b_clamped, CONST COMPO what, bool CONST b_linear ) CONST
{
	REAL	a = 0;
	FP32	da = OVER_ONE_AS_FP32( nb );
	FP32	dst = 0;
	for( ; nb>0; --nb )
	{
		REAL	s,c;
		GET_SIN_COS_TURN( s, c, a );
		dst += get_valid_value_from_uv( u + c*ru, v + s*rv, b_clamped, what, b_linear );
		a += da;
	}
	return dst * da;
}
void	c_img_2d::get_valid_color3r_from_uv_ellipse(	FP32* color,	REAL CONST u,	REAL CONST v,	 REAL CONST ru, REAL CONST rv, INT32 nb, bool CONST b_clamped, bool CONST b_linear ) CONST
{
	REAL	a = 0;
	FP32	da = OVER_ONE_AS_FP32( nb );
	clear_v3( color );
	for( ; nb>0; --nb )
	{
		REAL	s,c;
		GET_SIN_COS_TURN( s, c, a );
		FP32	col[3];
		get_valid_color3r_from_uv( col, u + c * ru, v + s * rv, b_clamped, b_linear );
		add_v3( color, col );
		a += da;
	}
	scale_v3( color, da );
}
void	c_img_2d::get_valid_color4r_from_uv_ellipse(	FP32* color,	REAL CONST u,	REAL CONST v,	 REAL CONST ru, REAL CONST rv, INT32 nb, bool CONST b_clamped, bool CONST b_linear ) CONST
{
	REAL	a = 0;
	FP32	da = OVER_ONE_AS_FP32( nb );
	clear_v4( color );
	for( ; nb>0; --nb )
	{
		REAL	s,c;
		GET_SIN_COS_TURN( s, c, a );
		FP32	col[4];
		get_valid_color4r_from_uv( col, u + c * ru, v + s * rv, b_clamped, b_linear );
		add_v4( color, col );
		a += da;
	}
	scale_v4( color, da );
}
void	c_img_2d::get_valid_gradient_from_uv_ellipse( REAL* dst, REAL CONST u, REAL CONST v, REAL CONST ru, REAL CONST rv, INT32 nb, bool CONST b_clamped, CONST COMPO what, bool CONST b_linear ) CONST
{
	REAL	a = 0;
	REAL	da = OVER_ONE_AS_REAL( nb );
	clear_v2( dst );
	for( ; nb>0; --nb )
	{
		REAL	s,c;
		GET_SIN_COS_TURN( s, c, a );
		REAL	grad[2];
		get_valid_gradient_from_uv( grad, u + c * ru, v + s * rv, b_clamped, what, b_linear );
		add_v2( dst, grad );
		a += da;
	}
	scale_v2( dst, da );
}


bool c_img_2d::find_rect_with_value( aaa::rect::lbrt_real& rect, FP32 min, FP32 max, bool b_outside, COMPO what )
{
	UINT8* data8 = get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !data8 )
		return false;

	INT32 ch_nb = get_channel_nb();
	//todo do 2 channels and bgr mode
	if( ch_nb != 4 && ch_nb != 3 && ch_nb != 1 )
	{
		ERR_PRINT_STRING( "%s() : implemented only for RGBA, RGB and R", __FUNCTION__ );
		return false;
	}

	INT32	sx			=	get_size_x();
	INT32	line_stride	=	get_byte_pitch();
	INT32	sy			=	get_size_y();

	UINT8*	pt_line = data8;
	UINT8*	pt;

	rect.left   = REAL(sx);
	rect.right  = REAL(0);
	rect.bottom = REAL(sy);
	rect.top    = REAL(0);
	if( ch_nb == 1 )
	{
		for( INT32 iy=0; iy<sy; ++iy )
		{
			pt = pt_line;
			for( INT32 ix=0; ix<sx; ++ix )
			{
				FP32 val = aaa::img::c_compo::to_fp32( *pt );
				bool b = b_outside ? OUTSIDE_MIN_MAX( val, min, max ) : INSIDE_MIN_MAX( val, min, max );
				if( b )
				{
					if( rect.left   > REAL(ix) )
						rect.left   = REAL(ix);
					if( rect.right  < REAL(ix) )
						rect.right  = REAL(ix);
					if( rect.bottom > REAL(iy) )
						rect.bottom = REAL(iy);
					if( rect.top    < REAL(iy) )
						rect.top    = REAL(iy);
				}
				++pt;
			}
			pt_line += line_stride;
		}
	}
	else
	{
		for( INT32 iy=0; iy<sy; ++iy )
		{
			pt = pt_line;
			for( INT32 ix=0; ix<sx; ++ix )
			{
				FP32 val;
				switch( what )
				{
				case COMPO::RED:
				case COMPO::GREEN:
				case COMPO::BLUE:		val = aaa::img::c_compo::to_fp32( *( pt + INT32(what) ) );									break;
				case COMPO::ALPHA:		val = ( ch_nb == 4 ) ? aaa::img::c_compo::to_fp32(*( pt + 3 )) : 1 ;							break;
				case COMPO::GREY:	//todo really compute a grey
										val = aaa::img::c_compo::to_fp32( (UINT8)aaa::color::rgb_to_grey( *pt, *(pt+1), *(pt+2) ) );	break;
				case COMPO::MAX_COMPO:	val = aaa::img::c_compo::to_fp32( aaa::MAX( *pt, *(pt+1), *(pt+2) ) );						break;
				case COMPO::MIN_COMPO:	val = aaa::img::c_compo::to_fp32( aaa::MIN( *pt, *(pt+1), *(pt+2) ) );						break;
				default:				val = 1.;																						break;
				}
				bool b = b_outside ? OUTSIDE_MIN_MAX( val, min, max ) : INSIDE_MIN_MAX( val, min, max );
				if( b )
				{
					if( rect.left   > REAL(ix) )
						rect.left   = REAL(ix);
					if( rect.right  < REAL(ix) )
						rect.right  = REAL(ix);
					if( rect.bottom > REAL(iy) )
						rect.bottom = REAL(iy);
					if( rect.top    < REAL(iy) )
						rect.top    = REAL(iy);
				}
				pt += ch_nb;
			}
			pt_line += line_stride;
		}
	}
	--sx;
	rect.left	/= sx;
	rect.right	/= sx;
	--sy;
	rect.bottom	/= sy;
	rect.top	/= sy;
	return rect.left < rect.right;	//we got one point
}

