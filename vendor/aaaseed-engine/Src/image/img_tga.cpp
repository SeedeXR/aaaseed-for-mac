#include "img.h"
#include "img_master.h"
#include "file/file_io.h"
#include "infrastructure/obj/obj_server.h"
#include "spy.h"

//todo writing need to be thread safe and is not now

namespace {
	typedef struct ST_IMG_TGA_HEADER {
		unsigned char	numid;
		unsigned char	maptyp;
		unsigned char	imgtyp;
		UINT16			maporig;
		UINT16			mapsize;
		unsigned char	mapbits;
		UINT16			xorig;
		UINT16			yorig;
		unsigned char	pixsize;
		unsigned char	imgdes;
	} ST_IMG_TGA_HEADER;

	void	print_header( ST_IMG_TGA_HEADER* t )
	{
		GOOD_PRINT_STRING( "numid %ld",					t->numid				);
		GOOD_PRINT_STRING( "maptype %ld",				t->maptyp				);
		GOOD_PRINT_STRING( "imgtyp %ld",				t->imgtyp				);
		GOOD_PRINT_STRING( "maporig %ld mapsize %ld",	t->maporig, t->mapsize	);
		GOOD_PRINT_STRING( "mapbits %ld",				t->mapbits				);
		GOOD_PRINT_STRING( "xorig yorig %ld %ld",		t->xorig, t->yorig		);
		GOOD_PRINT_STRING( "pixsize %ld",				t->pixsize				);
		GOOD_PRINT_STRING( "imgdes %ld",				t->imgdes				);
	}

	//#define	FLIPY	0x20
//hack fixed size buffer a really bad idea
	INT32 CONST	TGA_BUF_SIZE = 2048 * 2048 * 4;
	UINT8		tga_line_buf[ TGA_BUF_SIZE ];	//hack use it even  better (write only when full or finish)
	//todo	we don't check out_uint8, so we don't get errors
	UINT8*		pt_tga;

//todo this should move away
FINLINE	void	out_uint8( FILE* of, UINT8 CONST c )	{ c_file::FPUTC( c, of ); }

FINLINE	void	tga_buf_init()	{	pt_tga = tga_line_buf - 1;	}
FINLINE	void	tga_buf_write( FILE* CONST file )
{
	INT32 CONST len = INT32(pt_tga + 1 - tga_line_buf);
	if( len )
	{
		c_file::FWRITE( tga_line_buf, len, 1, file );
		tga_buf_init();
	}
}

FINLINE	void	outchar(	UINT8 CONST c	)		{ *++pt_tga = c; }
FINLINE	void	outshort(	UINT16 CONST i	)
{
#ifdef	WIN32
	*++pt_tga = i & 0xff ;
	*++pt_tga = (i>>8) & 0xff ;
#else
	*++pt_tga = i & 0xff ;
	*++pt_tga = (i>>8) & 0xff ;
#endif
}

//
//	HEADER
//
AAA_ERR	img_tga_write_header( c_img_2d CONST * CONST image, FILE* CONST file, INT32 CONST type )
{
	//FILE*				of = image->get_file();
	ST_IMG_TGA_HEADER	header;

	header.numid	= 0;
	header.maptyp	= 0;
	header.imgtyp	= type;
	header.maporig	= 0;
	header.mapsize	= 0;
	header.mapbits	= 0;
	header.xorig	= 0;
	header.yorig	= 0;
	if( type == 10 || type == 2 )	header.pixsize = ((INT16)image->get_channel_nb())*8;
	else							header.pixsize = (INT16)8;
	header.imgdes	= 0;

	tga_buf_init();
	outchar(	header.numid	);
	outchar(	header.maptyp	);
	outchar(	header.imgtyp	);
	outshort(	header.maporig	);
	outshort(	header.mapsize	);
	outchar(	header.mapbits	);
	outshort(	header.xorig	);
	outshort(	header.yorig	);
	outshort(	(UINT16)image->get_size_x() );
	outshort(	(UINT16)image->get_size_y() );
	outchar(	header.pixsize	);
	outchar(	header.imgdes	);
	c_file::FWRITE( tga_line_buf, pt_tga + 1 - tga_line_buf, 1, file );

	return	AAA_OK;
}

FINLINE	void	puttargarow24( FILE* CONST of, UINT8 CONST * r, UINT8 CONST * g, UINT8 CONST * b, INT32 n )
{
	INT32 CONST write_count = n * 3;
	if( write_count < sizeof(tga_line_buf) )
	{
		--r;	--g;	--b;	++n;	//	to use later preinc instead of postinc
		UINT8* pt = tga_line_buf - 1;	//	to use later preinc instead of postinc
		while( --n )	{	*++pt = *++b;	*++pt = *++g;	*++pt = *++r;	}
		c_file::FWRITE( tga_line_buf, write_count, 1, of );
	}
	else
		debug_break( "%s() buffer too small", __FUNCTION__ );
}


FINLINE	void	puttargarow32( FILE* CONST of, UINT8 CONST * r, UINT8 CONST * g, UINT8 CONST * b, UINT8 CONST * a, INT32 n )
{
	INT32 CONST write_count = n * 4;
	if( write_count < sizeof(tga_line_buf) )
	{
		--r;	--g;	--b;	--a;	++n;	//	to use later preinc instead of postinc
		UINT8* pt = tga_line_buf - 1;			//	to use later preinc instead of postinc
		while( --n )	{	*++pt = *++b;	*++pt = *++g;	*++pt = *++r;	*++pt = *++a;	}
		c_file::FWRITE( tga_line_buf, write_count, 1, of );
	}
	else
		debug_break( "%s() buffer too small", __FUNCTION__ );
}

FINLINE	void	put_row24_from_24( FILE* CONST of, UINT8 CONST * d, INT32 n )
{
	INT32 CONST write_count = n*3;
	if( write_count < sizeof(tga_line_buf) )
	{
		UINT8* pt = tga_line_buf - 1;
		while( --n>=0 )
		{
			*++pt = *(d+2);
			*++pt = *(d+1);
			*++pt = *d;
			d += 3;
		}
		c_file::FWRITE( tga_line_buf, write_count, 1, of );
	}
	else
		debug_break( "%s() buffer too small", __FUNCTION__ );
}

FINLINE	void	put_row24_from_32( FILE* CONST of, UINT8 CONST * d, INT32 n )
{
	INT32 write_count = n*3;
	if( write_count < sizeof(tga_line_buf) )
	{
		UINT8* pt = tga_line_buf - 1;
		++n;	//	to use preinc instead of postinc
		while( --n )
		{
			*++pt = *(d+2);
			*++pt = *(d+1);
			*++pt = *d;
			d += 4;
		}
		c_file::FWRITE( tga_line_buf, write_count, 1, of );
	}
	else
		debug_break( "%s() buffer too small", __FUNCTION__ );
}

//
//	ROW8
//
FINLINE	void	put_row8_low( FILE* CONST of, UINT8 CONST * & d, INT32 n, INT32 CONST step )
{
	INT32	write_count = n;
	UINT8*	pt = tga_line_buf - 1;
	while( --n >= 0 )
	{
		*++pt = *d;
		d += step;
	}
	c_file::FWRITE( tga_line_buf, write_count, 1, of );
}

FINLINE	void	put_row8( FILE* CONST of, UINT8 CONST * d, INT32 n, INT32 CONST step )
{
	while( n > sizeof(tga_line_buf) )
	{
		put_row8_low( of, d, sizeof(tga_line_buf), step );
		n -= sizeof(tga_line_buf);
	}
	put_row8_low( of, d, n, step );
}

FINLINE	void	put_row8_mem( UINT8 CONST * src, INT32 n )
{
	while( --n >= 0 )
	{
		*++pt_tga = *src;
		src += 4;
	}
}

namespace {
	INT32			nb_comp = 0;
	UINT32			data_last;
	UINT8			u8_data_last;
	INT32			nb_last_line = 0;
	UINT8 CONST*	marker_last_line;
}

FINLINE	void	compressed_init()
{
	nb_comp = 0;
	nb_last_line = 0;
}

//todo this implementation should compress fully across lines (including several)
//todo make it work with padding etc....
//todo	best solution for both use seek()
//
//	COMPRESSED 8
//
FINLINE	void	put_compressed_raw_8( FILE* CONST of, UINT8 CONST * p, INT32 n, INT32 CONST step )
{
	out_uint8( of, n-1 );
	if( nb_last_line )
	{
		if ( nb_last_line > n )
			nb_last_line = n;
		put_row8( of, marker_last_line, nb_last_line, step );
		n -= nb_last_line;
		nb_last_line = 0;
	}
	put_row8( of, p, n, step );
}

FINLINE	void	put_compressed_rlc_8( FILE* CONST of, UINT32 CONST d, INT32 CONST n )
{
	UINT8*	pt = tga_line_buf - 1;
	*++pt = 0x80 + (n-1);
	*++pt = d;
	c_file::FWRITE( tga_line_buf, 2, 1, of );
}
//
//	COMPRESSED 8
//
FINLINE	void	put_compressed_leftover_row8( FILE* CONST of, INT32 CONST step )
{
	if( nb_comp < 0 )	{	put_compressed_rlc_8( of, u8_data_last, -nb_comp );			}
	else				{	put_compressed_raw_8( of, marker_last_line, nb_comp, step );	}
}

//todoopt	preincrement instead of postincrement
FINLINE	void	put_compressed_row8( FILE* CONST of, UINT8 CONST * p, INT32 n, INT32 CONST step )
{
	UINT8			data;
	UINT8 CONST *	marker = p;

	while( --n >= 0 )
	{
		if( nb_comp == 0 )
		{
			marker = p;
			u8_data_last = *p;
			p += step;
			nb_comp = -1;
		}
		else
		{
			data = *p;
			p += step;
			if( nb_comp < 0 )	// search for repetition
			{
				if( data == u8_data_last )
				{
					--nb_comp;
					if( nb_comp == -128 )
					{	// save rlc
						put_compressed_rlc_8( of, u8_data_last, -nb_comp );
						nb_comp = 0;
						nb_last_line = 0;
					}
				}
				else
				{
					if( nb_comp == -1 )
					{
						nb_comp = 2;
					}
					else
					{	// save rlc
						put_compressed_rlc_8( of, u8_data_last, -nb_comp );
						marker = p-step;
						nb_comp = -1;
						nb_last_line = 0;
					}
					u8_data_last = data;
				}
			}
			else	// search for different
			{
				if( data != u8_data_last )
				{
					++nb_comp;
					u8_data_last = data;
					if( nb_comp == 128 )
					{	// save raw
						put_compressed_raw_8( of, marker, nb_comp, step );
						nb_comp = 0;
					}
				}
				else
				{
					//	write nb-1 raw
					put_compressed_raw_8( of, marker, nb_comp-1, step );
					nb_comp = -2;
				}
			}
		}
	}
	nb_last_line = (nb_comp>-2 )? ((nb_comp>0)?nb_comp:-nb_comp) : 0;	//	we need to include -1
	marker_last_line = marker;
}

//
//	COMPRESSED 24
//
FINLINE	void	put_compressed_raw_24_from_24( FILE* CONST of, UINT8 CONST * p, INT32 n )
{
	out_uint8( of, n-1 );
	if( nb_last_line )
	{
		if( nb_last_line > n )	{	nb_last_line = n;	}

		put_row24_from_24( of, marker_last_line, nb_last_line );
		n -= nb_last_line;
		nb_last_line = 0;
	}
	put_row24_from_24( of, p, n );
}

FINLINE void	put_compressed_rlc_24_from_24( FILE* CONST of, UINT32 CONST d, INT32 CONST n )
{
	UINT8*	pt = tga_line_buf - 1;
	*++pt = 0x80 + (n-1);
	*++pt = d>>8;
	*++pt = d>>16;
	*++pt = d>>24;
	c_file::FWRITE( tga_line_buf, 4, 1, of );
}

FINLINE	void	put_compressed_leftover_row24_from_24( FILE* CONST of )
{
	if( nb_comp<0 )	{	put_compressed_rlc_24_from_24( of, data_last,		-nb_comp );	}
	else			{	put_compressed_raw_24_from_24( of, marker_last_line, nb_comp );	}
}

//todoopt	preincrement instead of postincrement
FINLINE	void	put_compressed_row24_from_24( FILE* CONST of, UINT8 CONST * p, INT32	n )
{
	UINT32	data;
	UINT8 CONST	* marker;

	marker = p;
	while( n-- > 0)
	{
		if( nb_comp == 0)
		{
			marker = p;
			data_last = *p++ << 24 ;
			data_last += *p++ << 16 ;
			data_last += *p++ << 8;
			nb_comp = -1;
		}
		else
		{
			data = *p++ << 24 ;
			data += *p++ << 16 ;
			data += *p++ << 8;
			if( nb_comp < 0 )	// search for repetition
			{
				if( data == data_last )
				{
					--nb_comp;
					if( nb_comp == -128 )
					{	// save rlc
						put_compressed_rlc_24_from_24( of, data_last, -nb_comp );
						nb_comp = 0;
						nb_last_line = 0;
					}
				}
				else
				{
					if( nb_comp == -1 )
					{
						nb_comp = 2;
					}
					else
					{	// save rlc
						put_compressed_rlc_24_from_24( of, data_last, -nb_comp );
						marker = p-3;
						nb_comp = -1;
						nb_last_line = 0;
					}
					data_last = data;
				}
			}
			else	// search for different
			{
				if( data != data_last )
				{
					++nb_comp;
					data_last = data;
					if( nb_comp == 128 )
					{	// save raw
						put_compressed_raw_24_from_24( of, marker, nb_comp );
						nb_comp = 0;
					}
				}
				else
				{
					//	write nb-1 raw
					put_compressed_raw_24_from_24( of, marker, nb_comp-1 );
					nb_comp = -2;
				}
			}
		}
	}
	nb_last_line = (nb_comp>-2) ? ((nb_comp>0)?nb_comp:-nb_comp) : 0;	//	we need to include -1
	marker_last_line = marker;
}

//
//	COMPRESSED 32
//
FINLINE	void	put_row32_from_32_low( FILE* CONST of, UINT8 CONST *& r, INT32 n )
{
	INT32	write_count = n*4;
	//todo check the change here
	//	done because it was call with -5
	//   while(n--)
	UINT8*	pt = tga_line_buf - 1;
	while( --n >= 0 )
	{
		*++pt = *(r+2);
		*++pt = *(r+1);
		*++pt = *r;
		*++pt = *(r+3);
		r += 4;
	}
	c_file::FWRITE( tga_line_buf, write_count, 1, of );
}

FINLINE	void	put_row32_from_32( FILE* CONST of, UINT8 CONST * r, INT32 n )
{
	while( n*4 > sizeof(tga_line_buf) )
	{
		put_row32_from_32_low( of, r, sizeof(tga_line_buf)/4 );
		n -= sizeof(tga_line_buf)/4;
	}
	return put_row32_from_32_low( of, r, n );
}

FINLINE	void	put_row32_from_32_mem( UINT8 CONST * r, INT32 n )
{
	while( --n >= 0 )
	{
		*++pt_tga = *(r+2);
		*++pt_tga = *(r+1);
		*++pt_tga = *r;
		*++pt_tga = *(r+3);
		r += 4;
	}
}

FINLINE	void	put_compressed_raw_32_from_32( FILE* CONST of, UINT8 CONST * p, INT32 n )
{
	out_uint8( of, n-1 );
	if( nb_last_line )
	{
		if( nb_last_line > n )	{	nb_last_line = n;	}

		put_row32_from_32( of, marker_last_line, nb_last_line );
		n -= nb_last_line;
		nb_last_line = 0;
	}
	put_row32_from_32( of, p, n );
}

FINLINE	void	put_compressed_rlc_32_from_32( FILE* CONST of, UINT32 CONST d, INT32 CONST n )
{
	UINT8*	pt = tga_line_buf - 1;
	*++pt = 0x80 + (n-1);
	*++pt = d>>8;
	*++pt = d>>16;
	*++pt = d>>24;
	*++pt = d;
	c_file::FWRITE( tga_line_buf, 5, 1, of );
}

//changes here
FINLINE	void	put_compressed_leftover_row32_from_32( FILE* CONST of )
{
	if ( nb_comp < 0 )
	{
		put_compressed_rlc_32_from_32( of, data_last, -nb_comp );
		nb_comp = 0;
		nb_last_line = 0;
	}
	else
	{
		nb_last_line = 0;
		put_compressed_raw_32_from_32( of, marker_last_line, nb_comp );
		nb_comp = 0;
	}
}

//todoopt	preincrement instead of postincrement
FINLINE	void	put_compressed_row32_from_32( FILE* CONST of, UINT8 CONST * p, INT32 n )
{
	UINT32			data;
	UINT8 CONST	*	marker = p;

	while( --n >= 0 )
	{
		if( nb_comp == 0 )
		{
			marker = p;
			data_last = *p++ << 24 ;
			data_last += *p++ << 16 ;
			data_last += *p++ << 8;
			data_last += *p++;
			nb_comp = -1;
		}
		else
		{
			data = *p++ << 24 ;
			data += *p++ << 16 ;
			data += *p++ << 8;
			data += *p++;
			if( nb_comp < 0 )	// search for repetition
			{
				if( data == data_last)
				{
					--nb_comp;
					if( nb_comp == -128 )
					{	// save rlc
						put_compressed_rlc_32_from_32( of, data_last, -nb_comp );
						nb_comp = 0;
						nb_last_line = 0;
					}
				}
				else
				{
					if( nb_comp == -1 )
					{
						nb_comp = 2;
					}
					else
					{	// save rlc
						put_compressed_rlc_32_from_32( of, data_last, -nb_comp );
						marker = p-4;
						nb_comp = -1;
						nb_last_line = 0;
					}
					data_last = data;
				}
			}
			else	// search for different
			{
				if( data != data_last )
				{
					++nb_comp;
					data_last = data;
					if( nb_comp == 128 )
					{	// save raw
						put_compressed_raw_32_from_32( of, marker, nb_comp );
						nb_comp = 0;
					}
				}
				else
				{
					//	write nb-1 raw
					put_compressed_raw_32_from_32( of, marker, nb_comp-1 );
					nb_comp = -2;
				}
			}
		}
	}
	nb_last_line = (nb_comp-2) ? ((nb_comp>0)?nb_comp:-nb_comp) : 0;	//	we need to include -1
	marker_last_line = marker;
}

//
//	COMPRESSED 32 in MEM
//
FINLINE	void	put_compressed_raw_32_from_32_mem( UINT8 CONST * p, INT32 n )
{
	*++pt_tga = n - 1;
	if( nb_last_line )
	{
		if( nb_last_line > n )	{	nb_last_line = n;	}

		put_row32_from_32_mem( marker_last_line, nb_last_line );
		n -= nb_last_line;
		nb_last_line = 0;
	}
	put_row32_from_32_mem( p, n );
}

FINLINE	void	put_compressed_rlc_32_from_32_mem( UINT32 CONST d, INT32 CONST n )
{
	*++pt_tga = 0x80 + (n-1);
	*++pt_tga = d>>8;
	*++pt_tga = d>>16;
	*++pt_tga = d>>24;
	*++pt_tga = d;
}

FINLINE	void	put_compressed_leftover_row32_from_32_mem()
{
	if( nb_comp<0 )
	{
		put_compressed_rlc_32_from_32_mem( data_last, -nb_comp );
		nb_comp = 0;
		nb_last_line = 0;
	}
	else
	{
		nb_last_line = 0;
		put_compressed_raw_32_from_32_mem( marker_last_line, nb_comp );
		nb_comp = 0;
	}
}

//todoopt	preincrement instead of postincrement
FINLINE	void	put_compressed_row32_from_32_mem( UINT8 CONST * p, INT32 n )
{
	UINT32			data;
	UINT8 CONST *	marker = p;
	while( --n >= 0 )
	{
		if( nb_comp == 0 )
		{
			marker = p;
			data_last = *p << 24 ;
			data_last += *++p << 16 ;
			data_last += *++p << 8;
			data_last += *++p;
			++p;
			nb_comp = -1;
		}
		else
		{
			data = *p << 24 ;
			data += *++p << 16 ;
			data += *++p << 8;
			data += *++p;
			++p;
			if( nb_comp < 0 )	// search for repetition
			{
				if( data == data_last )
				{
					--nb_comp;
					if( nb_comp == -128 )
					{	// save rlc
						put_compressed_rlc_32_from_32_mem( data_last, -nb_comp );
						nb_comp = 0;
						nb_last_line = 0;
					}
				}
				else
				{
					if( nb_comp == -1)
					{
						nb_comp = 2;
					}
					else
					{	// save rlc
						put_compressed_rlc_32_from_32_mem( data_last, -nb_comp );
						marker = p-4;
						nb_comp = -1;
						nb_last_line = 0;
					}
					data_last = data;
				}
			}
			else	// search for different
			{
				if( data != data_last)
				{
					++nb_comp;
					data_last = data;
					if ( nb_comp == 128 )
					{	// save raw
						put_compressed_raw_32_from_32_mem( marker, nb_comp );
						nb_comp = 0;
					}
				}
				else
				{
					//	write nb-1 raw
					put_compressed_raw_32_from_32_mem( marker, nb_comp-1 );
					nb_comp = -2;
				}
			}
		}
	}
	nb_last_line = (nb_comp>-2) ? ((nb_comp>0)?nb_comp:-nb_comp) : 0;	//	we need to include -1
	marker_last_line = marker;
}

AAA_ERR	tga_write_core_compressed( c_img_2d CONST * CONST img, FILE* CONST file, bool CONST b_grey )
{
	UINT8 CONST *	src = img->get_data_uint8();
	if( !src )
		return ERR_MEM_BASE;

	INT32 CONST sx = img->get_size_x();
	INT32 CONST sy = img->get_size_y();

	compressed_init();

	marker_last_line = src;
	INT32 CONST channel_nb = img->get_channel_nb();
	//FILE*	file = img->get_file();
	if( channel_nb == 3 )
	{
		if( b_grey )
		{
			for( INT32 y = 0; y < sy; ++y )
				put_compressed_row8( file,  src + sx * 3 * y, sx, 3 );
			put_compressed_leftover_row8( file, 3 );
		}
		else
		{
			for( INT32 y = 0; y < sy; ++y )
				put_compressed_row24_from_24( file, src + sx * 3 * y, sx );
			put_compressed_leftover_row24_from_24( file );
		}
		return	AAA_OK;
	}
	else if( channel_nb == 4 )
	{
		tga_buf_init();
		if( b_grey )
		{
			for( INT32 y = 0; y < sy; ++y )
				put_compressed_row8( file,  src + sx * 4 * y, sx, 4 );
			put_compressed_leftover_row8( file, 4 );
		}
		else
		{
			if( (sx*sy*5) > sizeof(tga_line_buf) )	//	5 is here to estimate a worst case compression
			{
				for( INT32 y = 0; y < sy; ++y )
					put_compressed_row32_from_32( file, src + sx * 4 * y, sx );
				put_compressed_leftover_row32_from_32( file );
			}
			else
			{
				for( INT32 y = 0; y < sy; ++y )
					put_compressed_row32_from_32_mem( src + sx * 4 * y, sx );
				put_compressed_leftover_row32_from_32_mem();
				tga_buf_write( file );
			}
		}
		return	AAA_OK;
	}
	else if( channel_nb == 1 )
	{
		tga_buf_init();
		if( b_grey )
		{
			for( INT32 y = 0; y < sy; ++y )
				put_compressed_row8( file,  src + sx * 1 * y, sx, 1 );
			put_compressed_leftover_row8( file, 1 );
		}
		else
		{
			return ERR_UNIMPLEMENTED_YET;
		}
		return	AAA_OK;
	}
	return ERR_UNIMPLEMENTED_YET;
}

AAA_ERR	tga_write_core( c_img_2d CONST * CONST img, FILE* CONST file, bool CONST b_grey )
{
#if	AAA_DEBUG()
	DBG_PRINT_STRING( "thru tga_write_core(%d,%d)", img->get_size_x(), img->get_size_y() );
#endif
	UINT8 CONST *	src = img->get_data_uint8();
	if( !src )
		return ERR_MEM_BASE;

	INT32 CONST	sx = img->get_size_x();
	INT32		sy = img->get_size_y();
	//FILE*	file = img->get_file();
	INT32 CONST	channel_nb = img->get_channel_nb();
	if( channel_nb == 3 )
	{
		INT32 CONST step_y = sx * 3L;
		if( b_grey )
		{
			for( INT32 y = 0; y < sy; ++y )
				put_row8( file, img->get_data_uint8() + step_y * y, sx, 3 );
		}
		else
		{
			for( INT32 y = 0; y < sy; ++y )
				put_row24_from_24( file, img->get_data_uint8() + step_y * y, sx );
		}
		return	AAA_OK;
	}
	else if( channel_nb == 4 )
	{
		INT32 CONST step_y = sx * 4L;
		tga_buf_init();
		if( b_grey )
		{
			if( sx*sy > sizeof(tga_line_buf) )
			{
				for( ; sy>0; --sy )
				{
					put_row8_mem( src, sx );
					tga_buf_write( file );
					src += step_y;
				}
			}
			else
			{
				for( ; sy>0; --sy )
				{
					put_row8_mem( src, sx );
					src += step_y;
				}
				tga_buf_write( file );
			}
		}
		else
		{
			if( sx*sy*4 > sizeof(tga_line_buf) )
			{
				for( ; sy>0; --sy )
				{
					put_row32_from_32_mem( src, sx );
					tga_buf_write( file );
					src += step_y;
				}
			}
			else
			{
				for( ; sy>0; --sy )
				{
					put_row32_from_32_mem( src, sx );
					src += step_y;
				}
				tga_buf_write( file );
			}
		}
		return	AAA_OK;
	}
	else if( channel_nb==1 )
	{
		INT32 CONST step_y = sx;
		if( b_grey )
		{
			for( INT32 y = 0; y < sy; ++y )			{	put_row8( file, img->get_data_uint8() + step_y * y, sx, 1 ); }
		}
		else
		{
			return ERR_UNIMPLEMENTED_YET;
			//todo
			//for( INT32 y = 0; y < sy; ++y )			{	put_row24_from_24( file, img->get_data_uint8() + step_y * y, sx ); }
		}
		return	AAA_OK;

	}
	else
	{
		return ERR_UNIMPLEMENTED_YET;
	} 
	return ERR_ANY;
}
}	//end anonymous namespace

AAA_ERR	c_img_2d::write_tga( o_str CONST & filename, bool CONST b_compressed, bool CONST b_grey )
{
	AAA_ERR	retcode = AAA_OK;

	//FILE* file = get_file();
	//	open file
	FILE* CONST file = c_file::FOPEN( filename, "wb" );
	if( file == nullptr )
	{
		ERR_PRINT_STRING( "IMG : Can't create file : %s", filename.get() );
		return c_img_utils::ERR_CANT_WRITE;
	}

//	#define	R	0
//	#define	G	1
//	#define	B	2
//	#define	A	3
	//if( _format.is_channel_split() )	//2006 unused so untested
	//{
	//	UINT8*	buf[4];
	//	for ( INT32 i = 0; i < _channel_nb; ++i )
	//	{
	//		buf[i] = (UINT8 *)MALLOC_ALIGNED( get_size_x(), 0 );
	//	}
	//	if( buf[0] && buf[1] && buf[2] && buf[3] )
	//	{
	//		img_tga_write_header( this, 2 );
	//		for( INT32 y = 0; y < get_size_y(); ++y )
	//		{
	//			if( _channel_nb < 3 )
	//			{
	//				//img_get_row8(image->file,buf[R],y,0);
	//				puttargarow24( file, buf[R], buf[R], buf[R], get_size_x() );
	//			}
	//			else
	//			{
	//				//img_get_row8(image->file,buf[R],y,R);
	//				//img_get_row8(image->file,buf[R],y,G);
	//				//img_get_row8(image->file,buf[R],y,B);
	//				if( _channel_nb == 3 )
	//				{
	//					puttargarow24( file, buf[R], buf[G], buf[B], get_size_x() );
	//				}
	//				else
	//				{
	//					//img_get_row8(image,buf[R],y,A);
	//					puttargarow32( file, buf[R], buf[G], buf[B], buf[A], get_size_x() );
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		retcode = ERR_MEM_BASE;
	//	}
	//	for ( INT32 i = 0; i < _channel_nb; ++i )
	//	{
	//		IF_FREE_ALIGNED_AND_NULL( buf[i] );
	//	}
	//}
	//else
	{
		if( b_compressed )
		{
			INT32 CONST channel_nb = get_channel_nb();
			img_tga_write_header( this, file, b_grey ? 11 : 10 );
			INT32 len;
			len = c_file::FTELL( file );
			tga_write_core_compressed( this, file, b_grey );
			len = c_file::FTELL( file ) - len;
			//check that the non compressed is not bigger
			if( len <= ( get_pixel_nb_to_process() * channel_nb ) )
				goto exit;
			else
				c_file::FSEEK_SET( file, 0L );
		}
		img_tga_write_header( this, file, b_grey ? 3 : 2 );
		tga_write_core( this, file, b_grey );
	}
exit:
	c_file::FCLOSE( file );
	return  retcode;
}



/*
static	INT32	flipcode;
static	INT32	outx, outy;
static	INT32	size_x, size_y;
static	UINT8	*rbuf, *gbuf, *bbuf;
static	INT32	pixsize;
*/

/*
void	outpixel( INT32 r, INT32 g, INT32 b)
{
	rbuf[outx] = (UINT8) r;
	gbuf[outx] = (UINT8) g;
	bbuf[outx] = (UINT8) b;
	++outx;
	if(outx == size_x)
		{
		//putfliprow(image,rbuf,outy,0,flipcode);
		//putfliprow(image,gbuf,outy,1,flipcode);
		//putfliprow(image,bbuf,outy,2,flipcode);
		outx = 0;
		++outy;
		}
}
*/

namespace {
struct st_tga_reader
{
	st_tga_reader( c_file_io& reader_in ) : reader(reader_in) {}
	c_file_io&	reader;
	INT32		flip_code;
	UINT8		buf4[4];
	UINT8*		dst;
	INT32		ch_nb;
	INT32		size_x;
	INT32		size_y;
	INT32		pitch;
	c_img_2d*	image;
};

template < INT32 CH_NB, INT32 PIXSIZE >
FINLINE AAA_ERR	img_tga_get_pixels_one( st_tga_reader & st )
{
	UINT8 * CONST dst = st.buf4;
	switch( PIXSIZE )
	{
	case 32:
		{
			UINT8 CONST * src = st.reader.get_data_pt( 4 );
			if( !src )
				return ERR_FREAD;
			*(dst)		= *(src+2);
			*(dst+1)	= *(src+1);
			*(dst+2)	= *(src);
			if( CH_NB == 4 )
				*(dst+3) = *(src+3);
		}
		return AAA_OK;
	case 24:
		{
			UINT8 CONST * src = st.reader.get_data_pt( 3 );
			if( !src )
				return ERR_FREAD;
			*(dst)		= *(src + 2);
			*(dst+1)	= *(src + 1);
			*(dst+2)	= *(src);
			if( CH_NB == 4 )
				*(dst+3) = 0xff;
		}
		return AAA_OK;
	case 16:
	case 15:
		{
			UINT16 pix;
			if( ERR( st.reader.read_uint16( pix ) ) )
				return ERR_FREAD;
			*(dst)		= ((pix >> 10) & 0x1f) << 3;
			*(dst+1)	= ((pix >> 5) & 0x1f) << 3;
			*(dst+2)	= ((pix >> 0) & 0x1f) << 3;
			if( CH_NB == 4 )
				*(dst+3) = (pix&0x8000) ? 0xff : 0 ;
		}
		return AAA_OK;
	case 8:
		{
			//	todo check error code
			UINT8 src;
			if( ERR( st.reader.read_uint8( src ) ) )
				return ERR_FREAD;
			if( CH_NB == 1 )
				*dst	=	src;
			else
			{
				*(dst)		= src;
				*(dst+1)	= src;
				*(dst+2)	= src;
				if( CH_NB == 4 )
					*(dst+3) = 0xff;
			}
		}
		return AAA_OK;
	default:
		return ERR_FREAD;
	}
}

template < INT32 CH_NB, INT32 PIXSIZE >
FINLINE AAA_ERR	img_tga_get_pixels( st_tga_reader CONST & st, UINT8* dst, INT32 n )
{
	AAA_ERR		retcode = ERR_FREAD;

	switch( PIXSIZE )
	{
	case 32:
		{
			UINT8 CONST * src = st.reader.get_data_pt( 4*n );
			if( !src )
				goto exit;
			--dst;
			do
			{
				*++dst = *(src+2);
				*++dst = *(src+1);
				*++dst = *(src);
				if( CH_NB == 4 )
					*++dst = *(src+3);
				src += 4;
			}
			while(--n);
		}
		break;
	case 24:
		{
			UINT8 CONST * src = st.reader.get_data_pt( 3*n );
			if( !src )
				goto exit;
			if( CH_NB == 3 )
			{
				MEMCPY( dst, src ,3*n, __FUNCTION__ );
			}
			else
			{
				--dst;
				do
				{
					*++dst = *(src+2);
					*++dst = *(src+1);
					*++dst = *(src);
					if( CH_NB == 4 )
						*++dst = 0xff;
					src += 3;
				}
				while(--n);
			}
		}
		break;
	case 16:
	case 15:
		--dst;
		do
		{
			//	todo check error code
			UINT16 pix;
			if( ERR(st.reader.read_uint16( pix ) ) )
				goto exit;
			*++dst =	((pix>>10)&0x1f)	<<	3;
			*++dst =	((pix>>5)&0x1f)		<<	3;
			*++dst =	((pix>>0)&0x1f)		<<	3;
			if( CH_NB == 4 )
				*++dst = (pix&0x8000) ? 0xff : 0 ;
		}
		while(--n);
		break;
	case 8:
		{
			UINT8 CONST * src = st.reader.get_data_pt( n );
			if( !src )
				goto exit;
			--dst;
			--src;
			if( CH_NB == 1 )
			{
				do
					*++dst = *++src;
				while(--n);
			}
			else
			{
				do
				{
					*++dst = *++src;
					*++dst = *src;
					*++dst = *src;
					if( CH_NB == 4 )
						*++dst = 0xff;
				}
				while(--n);
			}
		}
		break;
	}
	retcode = AAA_OK;
exit:
	return retcode;
}

template < INT32 CH_NB_DST, INT32 PIXSIZE >
AAA_ERR	readrle( st_tga_reader & st )
{
	bool CONST b_check  = false;
	if( b_check )
		DBG_HEAP_IS_CORRUPT();
			
	SPY_PUSH_RANGE( "tga::readrle", spy::IMG_LOW );

	AAA_ERR	retcode = ERR_FREAD;

	INT32		outx		= 0;
	INT32		outy		= 0;
	INT32		pitch		= st.pitch;
	INT32 CONST	size_x		= st.size_x;
	UINT8*		data_dst	= st.dst;
	INT32 CONST	size_y		= st.size_y;

	if( st.flip_code == 2 )
	{
		data_dst += pitch * (size_y-1);
		pitch = -pitch;
	}
	UINT8* dst = data_dst + outy * pitch;

	c_file_io&		reader = st.reader;
	UINT8* CONST	buf		= st.buf4;
	while( outy < size_y )
	{
		UINT8	token;
		if(ERR(reader.read_uint8(token)))
			goto exit;
		if( (token & 0x80) != 0x80 )
		{	//	pixels are all different		
			for( INT32 nbpix = token + 1; nbpix ; --nbpix )
			{
				//todo we should do that outside loop even if several loops
				if( outx >= size_x )
				{
					outx = 0;
					if( ++outy >= size_y )
						goto overshoot;
					dst = data_dst + outy * pitch;
					if( b_check )
						DBG_HEAP_IS_CORRUPT();
				}
				if( ERR( (img_tga_get_pixels_one<CH_NB_DST,PIXSIZE>(st)) ) )
					goto exit;
				switch( CH_NB_DST )
				{
					case 1:	*dst++ = buf[0];	break;
					case 3:	*dst++ = buf[0];	*dst++ = buf[1];	*dst++ = buf[2];	break;
					case 4:					
#if AAA_ENDIAN_LITTLE()
						*(INT32*)dst = *(INT32*)buf;
						dst += 4;
#else
						*dst++ = buf[0];	*dst++ = buf[1];	*dst++ = buf[2];	*dst++ = buf[3];
#endif
						break;
				}
				++outx;
			}
			if( b_check )
				DBG_HEAP_IS_CORRUPT();
		}
		else
		{	//  pixels are all the same
			//	Galaad trouve ca joli le vert
			if( ERR( (img_tga_get_pixels_one<CH_NB_DST,PIXSIZE>(st)) ) )
				goto exit;
			for( INT32 nbpix = (token & 0x7f) + 1; nbpix ; --nbpix )
			{
				//todo we should do that outside loop even if several loops
				if( outx >= size_x )
				{
					outx = 0;
					if( ++outy >= size_y )
						goto overshoot;
					dst = data_dst + outy * pitch;
					if( b_check )
						DBG_HEAP_IS_CORRUPT();
				}
				switch( CH_NB_DST )
				{
					case 1:	*dst++ = buf[0];	break;
					case 3:	*dst++ = buf[0];	*dst++ = buf[1];	*dst++ = buf[2];	break;
					case 4:					
#if AAA_ENDIAN_LITTLE()
						*(INT32*)dst = *(INT32*)buf;
						dst += 4;
#else
						*dst++ = buf[0];	*dst++ = buf[1];	*dst++ = buf[2];	*dst++ = buf[3];
#endif
						break;
				}
				++outx;
			}
			if( b_check )
				DBG_HEAP_IS_CORRUPT();
		}
		if( outx >= size_x )
		{
			outx = 0;
			++outy;
			dst = data_dst + outy * pitch;
			if( b_check )
				DBG_HEAP_IS_CORRUPT();
		}
	}
	retcode = AAA_OK;
exit:
	SPY_POP_RANGE();
	return retcode;
overshoot:
	SPY_POP_RANGE();
	BOX_ERR( "Error reading Tga file: %s\nThis file is probably corrupt, AAASeed have to stop reading it.", st.image->get_filename() );
	return AAA_OK;
}

template < INT32 CH_NB_DST, INT32 PIXSIZE >
AAA_ERR	readuncompressed( st_tga_reader CONST & st )
{
	SPY_PUSH_RANGE( "tga::readuncompressed", spy::IMG_LOW );

	UINT8*	p = st.dst;

	AAA_ERR	retcode	= ERR_FREAD;
//	INT32	ch_nb	= image->get_channel_nb();
	INT32 CONST	size_x	= st.size_x;
	INT32 CONST	size_y	= st.size_y;
	INT32		pitch	= st.pitch;

	aaa::PIXEL_FORMAT	src_format;
	bool				b_force_alpha;
	switch( PIXSIZE )
	{
	case 32:	src_format = aaa::PIXEL_FORMAT::BGRA_8;		b_force_alpha = false;	break;
	case 24:	src_format = aaa::PIXEL_FORMAT::BGR_8;		b_force_alpha = true;	break;
	case 8:		src_format = aaa::PIXEL_FORMAT::R_8;		b_force_alpha = true;	break;
	case 16:
	case 15:
	default:	src_format = aaa::PIXEL_FORMAT::UNKNOWN;	b_force_alpha = true;	break;
	}

	if( src_format != aaa::PIXEL_FORMAT::UNKNOWN )
	{
		st_img_conv options( size_x,size_y );
		options.b_force_alpha	= b_force_alpha && (st.ch_nb == 4);
		options.b_flip_vert		= st.flip_code == 2;

		UINT8 CONST * src = st.reader.get_cur();
		options.src_pixel_format = src_format;
		options.signature = __FUNCTION__;
		retcode = st.image->copy_from_src( src, aaa::c_pixel_format::get_byte_per_pixel(src_format)*size_x, options ) ? AAA_OK : ERR_ANY;
	}
	else
	{
		if( CH_NB_DST == 3 && PIXSIZE == 24 && size_x*3 == pitch )
		{
			size_t CONST	size = 3 * size_x * size_y;
			UINT8 CONST *	src = st.reader.get_data_pt( size );
			MEMCPY( p, src, size, __FUNCTION__ );
		}
		else
		{
			if( st.flip_code == 2 )
			{
				p += pitch * (size_y-1);
				pitch = -pitch;
			}
			for( INT32 y = 0; y < size_y; ++y )
			{
				if( ERR( (img_tga_get_pixels<CH_NB_DST,PIXSIZE>( st, p, size_x )) ) )
					goto exit;
				p += pitch;
			}
		}
	}
	retcode = AAA_OK;
exit:	
	SPY_POP_RANGE();
	return retcode;
}

#define USE_THREAD_LOCAL 1
#if USE_THREAD_LOCAL
	thread_local c_file_io file_io_tga;
#else
	c_obj_server< c_file_io > server_reader(0);
#endif	
}	//end anonymous namespace

//read tga from file_io memory
AAA_ERR	c_img_2d::read_tga( c_file_io& reader, bool CONST b_load_data )
{
	AAA_ERR retcode;

	ST_IMG_TGA_HEADER	header;
	//	get id
	reader.read_uint8( header.numid );
	//  get map type
	reader.read_uint8( header.maptyp );
	//	get image type
	reader.read_uint8( header.imgtyp );

	//	get color map spec
	reader.read_uint16( header.maporig );
	reader.read_uint16( header.mapsize );
	reader.read_uint8( header.mapbits );

	//	get image spec
	reader.read_uint16( header.xorig );
	reader.read_uint16( header.yorig );

	INT32 size_x, size_y;
	reader.read_uint16( size_x );
	reader.read_uint16( size_y );

	reader.read_uint8( header.pixsize );

	//	get flip code
	reader.read_uint8( header.imgdes );

//	INT32	channel_nb;
	aaa::PIXEL_FORMAT	type = aaa::PIXEL_FORMAT::UNKNOWN;
	switch( header.pixsize )
	{
	case 32:	//	channel_nb = 4;
		type = g_img_master->is_read_bgr_keep() ? aaa::PIXEL_FORMAT::BGRA_8 : aaa::PIXEL_FORMAT::RGBA_8;
		break;
	case 24:	//	channel_nb = 3;
		if( g_img_master->is_read_bgr_keep() )
			type = g_img_master->is_read_rgb_add_alpha() ? aaa::PIXEL_FORMAT::BGRA_8 : aaa::PIXEL_FORMAT::BGR_8;
		else
			type = g_img_master->is_read_rgb_add_alpha() ? aaa::PIXEL_FORMAT::RGBA_8 : aaa::PIXEL_FORMAT::RGB_8;
		break;
	case 16:
	case 15:	//	channel_nb = 3;
		type = aaa::PIXEL_FORMAT::RGB_8;
		break;
	case 8:		//	channel_nb = 1;
		//	read now only RGB and RGBA
		//todo	read 8 bits as 8 bits
		type = aaa::PIXEL_FORMAT::R_8;
		break;
	default:
		retcode = c_img_utils::ERR_CANT_READ_THIS_FORMAT;
		ERR_PRINT_STRING( "IMG : reading tga bad pixsize %ld", header.pixsize );
		goto label_dealloc;
	}
	//img->set_size( size_x, size_y, type );
	retcode = init_with_size( size_x, size_y, type, "tga image" );
	if( ERR(retcode) )
		goto label_dealloc;

//	xsize = header.size_x;
//	ysize = header.size_y;
	{
		INT32 ch_nb = get_channel_nb();
		if( b_load_data )
		{
			st_tga_reader st(reader);
			st.flip_code = (header.imgdes>>4) & 0x3;
			//	skip id data
			retcode = reader.skip_uint8( header.numid );
			if( ERR( retcode ) )
				goto label_dealloc;

			//	skip color map data
			if( header.mapsize > 0 )
			{
				INT32 mapbytes;
				switch(header.mapbits)
				{
				case 15:
				case 16:	mapbytes = 2 * header.mapsize;	break;
				case 24:	mapbytes = 3 * header.mapsize;	break;
				case 32:	mapbytes = 4 * header.mapsize;	break;
				default:
					retcode = c_img_utils::ERR_CANT_READ;
					ERR_PRINT_STRING( "IMG : reading tga bad map entry size %ld", header.mapbits);
					goto label_dealloc;
				}
				retcode = reader.skip_uint8( mapbytes );
				if( ERR( retcode ) )
					goto label_dealloc;
			}

			UINT8*	dst = get_data_uint8();
			if( !dst )
			{
				retcode = c_img_utils::ERR_DATA_NO;
				goto label_dealloc;
			}

			st.dst		= dst;
			st.size_x	= get_size_x();
			st.size_y	= get_size_y();
			st.pitch	= get_byte_pitch();
			st.ch_nb	= ch_nb;
			st.image	= this;
			switch( header.imgtyp )
			{
			case 2:
			case 3:
				{	
					switch( ch_nb )
					{
					case 1:	
						switch( header.pixsize )
						{
						case 8: 	retcode = readuncompressed<1,8>(  st );	break;
						case 15:
						case 16:	retcode = readuncompressed<1,16>( st );	break;
						case 24: 	retcode = readuncompressed<1,24>( st );	break;
						case 32: 	retcode = readuncompressed<1,32>( st );	break;
						}
						break;
					case 3:
						switch( header.pixsize )
						{
						case 8: 	retcode = readuncompressed<3,8>(  st );	break;
						case 15:
						case 16:	retcode = readuncompressed<3,16>( st );	break;
						case 24: 	retcode = readuncompressed<3,24>( st );	break;
						case 32: 	retcode = readuncompressed<3,32>( st );	break;
						}
						break;
					case 4:
						switch( header.pixsize )
						{
						case 8: 	retcode = readuncompressed<4,8>(  st );	break;
						case 15:
						case 16:	retcode = readuncompressed<4,16>( st );	break;
						case 24: 	retcode = readuncompressed<4,24>( st );	break;
						case 32: 	retcode = readuncompressed<4,32>( st );	break;
						}
						break;
					default:
						goto wrong_channel_number;
					}
				}
				break;
			case 10:
			case 11:
				{
					switch( ch_nb )
					{
					case 1:	
						switch( header.pixsize )
						{
						case 8: 	retcode = readrle<1,8>(  st );	break;
						case 15:
						case 16:	retcode = readrle<1,16>( st );	break;
						case 24: 	retcode = readrle<1,24>( st );	break;
						case 32: 	retcode = readrle<1,32>( st );	break;
						}
						break;
					case 3:
						switch( header.pixsize )
						{
						case 8: 	retcode = readrle<3,8>(  st );	break;
						case 15:
						case 16:	retcode = readrle<3,16>( st );	break;
						case 24: 	retcode = readrle<3,24>( st );	break;
						case 32: 	retcode = readrle<3,32>( st );	break;
						}
						break;
					case 4:
						switch( header.pixsize )
						{
						case 8: 	retcode = readrle<4,8>(  st );	break;
						case 15:
						case 16:	retcode = readrle<4,16>( st );	break;
						case 24: 	retcode = readrle<4,24>( st );	break;
						case 32: 	retcode = readrle<4,32>( st );	break;
						}
						break;
					default:	
						goto wrong_channel_number;
					}
				}
				break;
			default:
				retcode = c_img_utils::ERR_CANT_READ_THIS_FORMAT;
				ERR_PRINT_STRING( "IMG : type is %ld, this targa reader only works on type 2, 3, 10 and 11 images only", header.imgtyp );
				goto label_dealloc;
			}
		}
		return AAA_OK;

wrong_channel_number:
		retcode = c_img_utils::ERR_CANT_READ_THIS_FORMAT;
		ERR_PRINT_STRING( "IMG : type is %ld, this targa reader only works on image with 1,3 or 4 channels, %d asked by AAASeed", header.imgtyp, ch_nb );
	}
label_dealloc:
	dealloc_data();
	ERR_PRINT_STRING( "Can't read tga file : retcode %d", retcode );
	return retcode;
}


//read file in file_io
AAA_ERR	c_img_2d::read_tga( o_str CONST & filename, bool CONST b_load_data )
{
	AAA_ERR retcode = ERR_ANY;

	// different buffer for different thread
#if USE_THREAD_LOCAL
	retcode = file_io_tga.read_file( filename, b_load_data ? -1 : 18 );
	if( NOERR(retcode) )
		retcode = read_tga( file_io_tga, b_load_data );
#else
	if( c_file_io* pt_file_io = server_reader.get() )
	{
		retcode = pt_file_io->read_file( filename, b_load_data ? -1 : 18 );
		if( NOERR(retcode) )
			retcode = read_tga( *pt_file_io, b_load_data );
		server_reader.store( pt_file_io );
	}
#endif
	return retcode;
}