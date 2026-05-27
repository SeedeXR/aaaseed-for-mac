#include "img.h"
#include "img_net.h"
#include "img_master.h"
#include "obj_ui/com/net_link.h"
//#include "obj_ui/com/net.h"
#include "bind_img_2d.h"
#include "spy.h"

//#include "bind_img.h"
//#include <unordered_map>
//#include "blosc/blosc.h"

//#include "code_utils/compression/TurboRLE/trle.h"  
//#include "code_utils/compression/TurboRLE/ext/mrle.h"

namespace
{
//todo generalize with bdd_line_3d too
//todo check all these values
	INT32 CONSTEXPR COMPRESSION_OFFSET_BYTE_NB = 32;	// 32 is mespotine
	INT32 CONSTEXPR BLK_SIZE_MAX = c_net::BLK_SIZE_MAX - COMPRESSION_OFFSET_BYTE_NB;
	INT32 CONSTEXPR IMG_MEM_HEADER_SIZE = 20;
	INT32 CONSTEXPR IMG_END_HEADER_SIZE = 4;
	UINT8 buf_send[BLK_SIZE_MAX]; //todo adjust to real net data
	//UINT8 buf_comp[BLK_SIZE_MAX * 1024 * 1024 + BLOSC_MAX_OVERHEAD];	//todo cleam
//	UINT8 buf_comp[32 * 256 * 256 * 256 + BLOSC_MAX_OVERHEAD];	//todo clean
	//INT32	tab_encode[256];
	CONSTEXPR INT32 TAB_ENCODE_INT32_SIZE = 256;
	CONSTEXPR INT32 TAB_ENCODE_BYTE_SIZE = TAB_ENCODE_INT32_SIZE * sizeof(INT32);
	INT32*	tab_encode = nullptr;
	UINT8	buf_encode[BLK_SIZE_MAX + COMPRESSION_OFFSET_BYTE_NB + 4	];	//todo clean	// 4 for len info
	bool	tab_decode[256];
	UINT8	buf_decode[BLK_SIZE_MAX + 4];	//todo clean // 4 for len info

	UINT32 CONSTEXPR	BLK_IMG_VERSION	= 1;
	UINT32 CONSTEXPR	BLK_IMG_SIGNATURE = (BLK_IMG_VERSION << 24) | 0x4200;

	enum BLK_IMG_OP : UINT32
	{
		IMG_OP_INVALID	= 0,
		IMG_DEFINE_V0	= 1,	// define the image at bind : carry dst bind, sx, sy, aaa::PIXEL_FORMAT passed
		IMG_BIND		= 2,	// every blk begin by one of thse two code so we work on the right img
		IMG_END			= 3,	// the end of an image and of the BLK
		IMG_MEM			= 4,	// next is a block of memory with infos x_begin, y_begin, size_x, line_nb followed by data
		IMG_BLK_END		= 5		// the end of a BLK if not the end of the img
	//	IMG_BACK
	//	IMG_ERASE
	};
	enum BLK_IMG_COMP : UINT32
	{
		COMP_INVALID	= 0,
		COMP_NO			= 1,
		COMP_MESPOTINE	= 2,
	};
	FINLINE INT32	MAKE_CODE(						BLK_IMG_OP op,	BLK_IMG_COMP comp )	
	{
		return BLK_IMG_SIGNATURE | (op << 16) | comp;
	}
	FINLINE void	GET_OP_COMP(	UINT32 data,	BLK_IMG_OP& op,	BLK_IMG_COMP& comp )
	{
		if( (data & 0xff00ff00) == BLK_IMG_SIGNATURE )
		{	//todo more check here
			op = BLK_IMG_OP((data >> 16) & 0xff);
			comp = BLK_IMG_COMP(data & 0xff);
		}
		else
		{
			op = IMG_OP_INVALID;
			comp = COMP_INVALID;
		}
	}
}

void	c_img_2d::c_init_send()
{
	if( !tab_encode )
	{
		tab_encode = (INT32 *) MALLOC_ALIGNED( TAB_ENCODE_BYTE_SIZE, 32 );	// 32 because of memset
	}
}

void	c_img_2d::c_deinit_send()
{
	FREE_ALIGNED_AND_NULL( tab_encode );
}

namespace
{

/*
	std::unordered_map< INT32, INT32 >	channel_to_bind;
	c_img_2d* get_img_from_channel( INT32 channel_id )
	{
		auto  it = channel_to_bind.find( channel_id );
		return  it != channel_to_bind.end() ? g_bind_img_2d->get_always( it->second ) : nullptr;
	}
*/
#	define OUT_CHAR( p, ch )	*((p)++) = (ch)
#	define IN_CHAR( in, in_ )	( (in)<(in_)?(*in++):-1 )


	INT32 aaa_enc_mespotine( UINT8 CONST * in_beg,  INT32 in_len, UINT8 * CONST out_beg )
	{
		UINT8 CONST *		pi		= in_beg;
		UINT8 CONST * CONST	pi_end	= in_beg + in_len;
		UINT8 *				p_out	= out_beg;
		INT32 c, c_last				= -1;  	// current and last char
		INT32 *				tab	;  		// byte -> saving, was long long but seems too much here
		INT32 run = 0;  // current run leng, was long long but seems too much here

	// Pass 1: determine which chars will compress
		MEMCLEAR( tab_encode, TAB_ENCODE_BYTE_SIZE );
		tab = tab_encode;
		while( pi < pi_end )
		{ 
			c = *pi++;
			if( c == c_last )
				tab[c] += (++run % 255) != 0;
			else
			{
				--tab[c];
				run = 0;
			}
			c_last = c;
		}

		for( INT32 i = 32; i > 0; --i )
		{
			INT32 c = (*tab>0) ? 1 : 0;
			if( *(tab+1)>0 )
				c |= 0x2;
			if( *(tab+2)>0 )
				c |= 0x4;
			if( *(tab+3)>0 )
				c |= 0x8;
			if( *(tab+4)>0 )
				c |= 0x10;
			if( *(tab+5)>0 )
				c |= 0x20;
			if( *(tab+6)>0 )
				c |= 0x40;
			if( *(tab+7)>0 )
				c |= 0x80;
			OUT_CHAR( p_out, c );
			tab += 8;
		}

		tab = tab_encode;
		pi = in_beg;
		c_last = -1;
		run = 0;
		do
		{
			c = pi < pi_end ? (*pi++) : -1;
			if( c == c_last )
				++run;
			else if( run>0 && tab[c_last]>0 )
			{
				OUT_CHAR( p_out, c_last );
				for( ; run>255; run-=255 )
					OUT_CHAR( p_out, 255 );
				OUT_CHAR( p_out, run-1 );
				run = 1;
			}
			else
			{
				for( ; run>0; --run )
					OUT_CHAR( p_out, c_last );
				run = 1;
			}
			c_last = c;
		}
		while( c != -1 );

		return INT32(p_out - out_beg);
	}


	INT32 aaa_dec_mespotine( UINT8 CONST * in_beg, UINT8 * out_beg, INT32 CONST out_len )
	{
		UINT8 CONST * pi = in_beg;	
		INT32 c, c_last = -1;  		// current and last char
		bool* tab = tab_decode ;  	// byte -> savings
		for( INT32 i = 32; i > 0; --i )
		{
			c = *pi++;//_getc(ip,in_);	
#if 1
			*tab     = (c&1)	  != 0;
			*(tab+1) = (c&(1<<1)) != 0;
			*(tab+2) = (c&(1<<2)) != 0;
			*(tab+3) = (c&(1<<3)) != 0;
			*(tab+4) = (c&(1<<4)) != 0;
			*(tab+5) = (c&(1<<5)) != 0;
			*(tab+6) = (c&(1<<6)) != 0;
			*(tab+7) = (c&(1<<7)) != 0;
			tab += 8;
#else
			*tab++ =  c&1		!= 0;
			*tab++ = ((c>>1)&1) != 0;
			*tab++ = ((c>>2)&1) != 0;
			*tab++ = ((c>>3)&1) != 0;
			*tab++ = ((c>>4)&1) != 0;
			*tab++ = ((c>>5)&1) != 0;
			*tab++ = ((c>>6)&1) != 0;
			*tab++ = ((c>>7)&1) != 0;
#endif
		}
		tab = tab_decode;
		UINT8 *			po		= out_beg;
		UINT8 * CONST	po_end	= out_beg + out_len ;
		while( po < po_end )
		{
			c = *pi++;//(c=_getc(ip,in_))!=-1) {
			if( tab[c] )
			{	
				INT32 run;	// current run length
				for( run = 0; (c_last=*pi++)==255; run += 255 );
				run += c_last + 1;
				if( run > 128 )
				{
					MEMSET( po, c, run );
					po += run;
				}
				else
				{
					for( ; run > 0; --run )
						OUT_CHAR( po, c );
				}
			}
			else
				OUT_CHAR( po, c );
		}
		return INT32(pi-in_beg);
	}

}

//void aaa_init_compression()
//{
//static bool b_first = true;
//	if( b_first )
//	{
//		//	Register the filter with the library */
//		ERR_PRINT_STRING( "Blosc version info: %s (%s)\n", BLOSC_VERSION_STRING, BLOSC_VERSION_DATE );
//
//		//	Initialize the Blosc compressor */
//		blosc_init();
//		//tm_init(1,1);
//		b_first = false;
//	}
//}

//static float data[SIZE];
//  static float data_out[SIZE];
//  static float data_dest[SIZE];
//  int isize = SIZE*sizeof(float), osize = SIZE*sizeof(float);
//  int dsize = SIZE*sizeof(float), csize;
//  int i;
//
//  /* Compress with clevel=5 and shuffle active  */
//  csize = blosc_compress (5, 1, sizeof(float), isize, data, data_out, osize );
//  if (csize == 0) {
//    printf("Buffer is uncompressible.  Giving up.\n");
//    return 1;
//  }
//  else if (csize < 0) {
//    printf("Compression error.  Error code: %d\n", csize);
//    return csize;
//  }
//
//  printf("Compression: %d -> %d (%.1fx)\n", isize, csize, (1.*isize) / csize);
//
//  /* Decompress  */
//  dsize = blosc_decompress(data_out, data_dest, dsize);
//  if (dsize < 0) {
//    printf("Decompression error.  Error code: %d\n", dsize);
//    return dsize;
//  }
//
//  printf("Decompression succesful!\n");
//
//  /* After using it, destroy the Blosc environment */
//  blosc_destroy();
//
//  for(i=0;i<SIZE;i++){
//    if(data[i] != data_dest[i]) {
//      printf("Decompressed data differs from original!\n");
//      return -1;
//    }
//  }


AAA_ERR	c_img_2d::send(	st_img_send * st_send )
{
	st_send->_blk_nb = 0;
	st_send->_sent_size = 0;

// img send active ?
	if( !g_img_master->is_net_out_active() )
		return AAA_OK;

// network and link active ?
	if( !net->is_active() )
		return ERR_ANY;

	c_net_link* p_link = net->get_link( st_send->_net_link_index );
	if( !p_link )
	{
		ERR_PRINT_STRING( "%s() link_index %d is out of range or net_link is null", __FUNCTION__, st_send->_net_link_index );
		return ERR_ANY;
	}

	if( !p_link->is_running() )
		return ERR_ANY;

// do image have data ?
	UINT8* data = (UINT8*)get_data();
	if( !data )
	{
		debug_break( "Image have no data allocated yet on CPU to send" );
		return ERR_ANY;
	}

// do we have datat to send
	INT32 sx = get_size_x();
	INT32 sy = get_size_y();
	if( sx<=0 || sy<=0 )
	{
		ERR_PRINT_STRING( "Can't send image with a 0 or negative size" );
		return ERR_DATA_INCOHERENT;
	}

	INT32 xb = 0;	//	begin
	INT32 xe = sx;	//	end
	INT32 yb = 0;	//	begin
	INT32 ye = sy;	//	end
	if( st_send->_b_crop )
	{
		xb =  st_send->_crop.left;
		xe -= st_send->_crop.right;
		yb =  st_send->_crop.bottom;
		ye -= st_send->_crop.top;
		sx = xe - xb;
		sy = ye - yb;
	}

	if( sy<=0 || sx<=0 )
	{
		ERR_PRINT_STRING( "Image have null or negative size to send" );
		return ERR_OUT_OF_BOUND;
	}

// Now we send
	//aaa_init_compression();
	//"blosclz", "lz4", "lz4hc", "snappy", "zlib" and "ztsd".
	//blosc_set_compressor( "lz4" );


//	INT32 size_tot = 0;
//	INT32 size_tot_comp = 0;
//	INT32 size_comp = blosc_compress( 7, BLOSC_NOSHUFFLE, _channel_nb, get_data_size(), data, buf_comp, sizeof(buf_comp) );
//	if( size_comp == 0 )
//	{
//		ERR_PRINT_STRING( "Buffer is uncompressible.  Giving up.");
//		//return 1;
//	}
//	else if ( size_comp < 0 )
//	{
//		ERR_PRINT_STRING( "Compression error.  Error code: %d", size_comp );
//		//return csize;
//	}
//	else
//	{
//		//DBG_PRINT_STRING( "compression %d to %d", pt-buf, size_comp );
//		size_tot_comp += size_comp;
////		size_tot += pt-buf;
//	}



	SPY_PUSH_RANGE( "img::send()", spy::IMG );

//todo we have a problem for line bigger that buffer
/*
	BLK send can be
	4b: len for decode or 0 

*/
	//todo pass it to the fns, other compression
	// get compression
		BLK_IMG_COMP comp = st_send->_b_compress ? COMP_MESPOTINE : COMP_NO;

		INT32 CONST blk_size_max = MIN( BLK_SIZE_MAX, g_img_master->get_net_out_blk_size_max() );

		UINT8* pt;
		bool b_first = true;
		CONSTEXPR INT32 LEN_FIELD_SIZE = sizeof(INT32);
		INT32 byte_per_line = get_byte_per_pixel() * sx;
		INT32 y = yb;
		for(;;)	// this a loop on separate net block wit\h a blk header then a serie of data blk inside
		{	//todo optimize : null line, same lines ....
			pt = c_net::write_value( buf_send, UINT32(0) );	//len for decode or 0 when no compression
			if( b_first )
			{	// first blk send IMG_DEFINE
				b_first = false;
				// this how we start first block
				//	4b: len for decode
				//	4b: Code IMG_DEFINE with version and compression
				//	4b: bind
				//	2b: size_x
				//	2b: size_y
				//	4b: pixel format				
				pt = c_net::write_value( pt, MAKE_CODE( IMG_DEFINE_V0, comp ) );
				//todo add a unique id to lock avoid mixing img at reception
				pt = c_net::write_value( pt, st_send->_bind_dst	);
				pt = c_net::write_value<UINT16>( pt, sx	);
				pt = c_net::write_value<UINT16>( pt, sy	);
				pt = c_net::write_value( pt, get_pixel_format()	);	//todo add an indirect so these number never change
				// aaa::PIXEL_FORMAT don't enforce this for now
				p_link->inc_img_sent_nb();
			}
			else
			{	// then next blks send IMG_BIND only
				// we put less info here that in the first blk but enough to avoid crashes when net blk are received badly
				// this how we start next block
				//	4b: len for decode
				//	4b: Code IMG_BIND with compression
				//	4b: bind
				pt = c_net::write_value( pt, MAKE_CODE( IMG_BIND, comp ) );
				pt = c_net::write_value( pt, st_send->_bind_dst	);
			}
				
			//	line to send
			INT32 len = INT32(pt - buf_send);	//already used
			INT32 line_nb_max_by_block = I_FLOOR( (blk_size_max-len-IMG_END_HEADER_SIZE-IMG_MEM_HEADER_SIZE) / byte_per_line );
			INT32 line_nb = MIN( ye-y, line_nb_max_by_block );
			if( line_nb <= 0 )
				break;	// we are done

		// write IMG_MEM header we send a sub rect here
		//	4b: len for decode
		//	4b: Code IMG_MEM with compression
		//	2b: x begin
		//	2b: y begin
		//	2b: size_x
		//	2b: Line_nb
			pt = c_net::write_value( pt,  MAKE_CODE( IMG_MEM, comp )	);
			//todo add a unique id to lock avoid mixing img at reception
			pt = c_net::write_value<INT16>( pt, 0		);	// we set 0 here because destination is cropped in IMG_DEFINE
			pt = c_net::write_value<INT16>( pt, y-yb	);	//  but wee keep the protocol generic
			pt = c_net::write_value<INT16>( pt, sx		);
			pt = c_net::write_value<INT16>( pt, line_nb	);
		// write all the lines data
			for( ; line_nb>0; --line_nb )
			{
				MEMCPY( pt, data + get_byte_offset(xb,y), byte_per_line, __FUNCTION__ );
				pt += byte_per_line;
				++y;
			}
		//	write the IMG_END or IMG_END_BLK header
			pt = c_net::write_value( pt, MAKE_CODE( ( y >= ye ) ? IMG_END : IMG_BLK_END, comp ) );

		//	SEND BLK
			len = INT32(pt - buf_send);
			if( comp == COMP_MESPOTINE )
			{
				//size_comp = blosc_compress( 5, BLOSC_NOSHUFFLE, _channel_nb, get_data_size(), data, buf_comp, sizeof(buf_comp) );
				c_net::write_value( buf_encode, len - LEN_FIELD_SIZE );	//we need src len to decode;
				// we compress the whole buffer at once
				len = aaa_enc_mespotine( buf_send + LEN_FIELD_SIZE, len - LEN_FIELD_SIZE, buf_encode + LEN_FIELD_SIZE );			
				if( len > 0 )
					p_link->sendto( st_send->_channel_id, c_net::BLK_IMG, buf_encode, len + LEN_FIELD_SIZE );
			}
			else
				p_link->sendto( st_send->_channel_id, c_net::BLK_IMG, buf_send, len );

			// output infos on what is going on 
			++(st_send->_blk_nb);
			st_send->_sent_size += len;

			//DBG_PRINT_STRING( "y is %d ", y );
			if( y >= ye )
				break;
		}

//	if( size_tot > 0 ) 
//		DBG_PRINT_STRING( "compression %d", 100*size_tot_comp/size_tot );

	SPY_POP_RANGE();
	return AAA_OK;
}

AAA_ERR	c_img_2d::receive_mem( UINT8*& pt )
{
	BLK_IMG_OP op;
	BLK_IMG_COMP comp;

	GET_OP_COMP( *(UINT32*)pt, op, comp );
	pt += 4;
	//	pt = c_net::read_value( pt, &op );
	if( op != IMG_MEM )
		return ERR_OUT_OF_BOUND;

	UINT8* data = (UINT8*)get_data();
	if( !data )
	{
		debug_break( "Image have no CPU data allocated yet on CPU to receive" );
		return ERR_ANY;
	}

	INT32 sx = get_size_x();
	INT32 sy = get_size_y();

	INT16 x, y;
	INT16 nbx, nby;

	pt = c_net::read_value( pt, &x, &y	);
	pt = c_net::read_value( pt, &nbx, &nby );

	nby = MIN( nby, sy-y );
	if( nby <= 0 )
		return ERR_OUT_OF_BOUND;

	INT32 byte_src_nb = get_byte_per_pixel() * nbx;
	nbx = MIN( nbx, sx-x );
	if( nbx <= 0 )
		return ERR_OUT_OF_BOUND;

	INT32 byte_nb = get_byte_per_pixel() * nbx;
	for( ; nby>0; --nby )
	{
		MEMCPY( data + get_byte_offset(x,y), pt, byte_nb, __FUNCTION__ );
		pt += byte_src_nb;
		++y;
		if( y >= sy )
			break;
	}
		
	if( g_img_master->is_net_in_update_continuous() )
		set_changed();
	return AAA_OK;
}

//AAA_ERR	c_img_2d::process_blk( INT32 channel_id, UINT8* data, INT32 len )
AAA_ERR	c_img_2d::process_blk( UINT8 CONST * CONST data, INT32 CONST len, INT32 CONST net_link_index )
{
	INT32 len_out;
	UINT8* pt = c_net::read_value( data, &len_out );
	//todo add compression type
	if( len_out )	// len means we have to decompress 
	{
		if( len_out < 0 )	
		{
			ERR_PRINT_STRING( "BLK_IMG got a negative len : %d", len );
			net->get_link(net_link_index)->inc_img_err_nb();
			return ERR_DATA_INCOHERENT;
		}
		if( BLK_SIZE_MAX < len_out )
		{
			ERR_PRINT_STRING( "BLK_IMG with a too big len : %d", len );
			net->get_link(net_link_index)->inc_img_err_nb();
			return ERR_DATA_INCOHERENT;
		}
		aaa_dec_mespotine( pt, buf_decode, len_out  );
		pt = buf_decode;
	}

	c_img_2d* img = nullptr;
	for(;;)
	{
		BLK_IMG_OP op;
		BLK_IMG_COMP comp;
		GET_OP_COMP( *(UINT32*)pt, op, comp );
		pt += 4;
		//pt = c_net::read_value( pt, &op );
		switch( op )
		{
//these start a block
		case IMG_DEFINE_V0:
			{
				INT32 bind;
				//todo receive unique id to lock avoid mixing img at reception
				pt = c_net::read_value( pt, &bind );
//				channel_to_bind[channel_id] = bind;

				INT16  sx, sy;
				pt = c_net::read_value( pt, &sx, &sy );
				aaa::PIXEL_FORMAT pf;
				pt = c_net::read_value( pt, &pf );

				img = g_bind_img_2d->get_always( bind );
				img->init_with_size( sx,sy, pf, __FUNCTION__ );
			}
			break;
		case IMG_BIND:
			{
				INT32 bind;
				//todo receive unique id to lock avoid mixing img at reception
				pt = c_net::read_value( pt, &bind );
				img = g_bind_img_2d->get_always( bind );
			}
			break;
//this is data
		case IMG_MEM:
			{
				if( img )
				{
					pt -= 4;	// go back to op
					//todo receive unique id to lock avoid mixing img at reception
					AAA_ERR ret = img->receive_mem( pt );
					if( ERR(ret) )
					{
						net->get_link(net_link_index)->inc_img_err_nb();
						return ret;
					}
				}
				else
				{
					net->get_link(net_link_index)->inc_img_err_nb();
					return ERR_ANY;
				}
			}
			break;
//these end blk processing
		case IMG_END:		//	end of image and so end of blk
			{
				if( img )
				{
					net->get_link(net_link_index)->inc_img_received_nb();
					//todo receive unique id to lock avoid mixing img at reception
					if( !g_img_master->is_net_in_update_continuous() )
						img->set_changed();
					//	tex_2d_bind( IMG_DST );
					//	img->move_to_gpu( __FUNCTION__ );
				}
			}
		case IMG_BLK_END:	//	end of image blk go to the next
			return AAA_OK;

		case IMG_OP_INVALID:
			ERR_PRINT_STRING( "opcode IMG_OP_INVALID for BLK_IMG" );
			net->get_link(net_link_index)->inc_img_err_nb();
			return ERR_DATA_INCOHERENT;
		default:
			ERR_PRINT_STRING( "Unknown opcode for BLK_IMG : %d", op );
			net->get_link(net_link_index)->inc_img_err_nb();
			return ERR_DATA_INCOHERENT;
		}
	}
	return AAA_OK;
}