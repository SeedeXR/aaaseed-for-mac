#include "obj3d_file_reader.h"
#include "bdd_tri.h"
#include "asc_line.h"
#include "obj_ui/bdd/bdd_old/special.h"
#include "file/file_io.h"
#include "spy.h"

c_obj3d_file_info::c_obj3d_file_info()
	:_obj		(0)
	,_point		(0)
	,_normal	(0)
	,_uv		(0)
	,_tri		(0)
	,_face		(0)
	,_b_poly	(false)
{}

static			bool	b_verbose = false;
static			o_str	buf_name;	//tmp buf to read name
static			INT32	buf_id;
static			INT32	usemtl_index;
static	CONST	bool	b_start_at_one = true;

void c_obj3d_file_reader::init()
{
	_point_index_offset		= 0;
	_normal_index_offset	= 0;
	_uv_index_offset		= 0;
	_tri_index_offset		= 0;

	_point_read				= 0;
	_normal_read			= 0;
	_uv_read				= 0;
	_tri_read				= 0;

#if AAA_POLY_COMPILE()
	_poly_index_offset		= 0;
	_poly_count_offset		= 0;
	_poly_read				= 0;
	_poly_index_read		= 0;
#endif
	_obj_nb = 0;

	_bdd = nullptr;

	_p_ver	= nullptr;
	_p_uv	= nullptr;
	_p_nor	= nullptr;

	clear_v3( _offset_at_load );
	set_v3( _scale_at_load, REAL(1) );

}

void c_obj3d_file_reader::begin( c_bdd_tri* bdd, C_PCHAR_C filename )
{
	init();
	_bdd = bdd;
	_filename.set( filename );
		
	_reader = nullptr;

	C_PCHAR_C ext = _filename.get_ext();
	_file_type = F3D_UNKNOWN;
	if( str_is_equal_nocase( ext, "geo" ) )
	{
		ERR_PRINT_STRING( "3D: AAASeed don't read anymore NICHIMEN file %s", filename );
		//reader._file_type = F3D_NICHIMEN_GEO;
	}
	else if( str_is_equal_nocase( ext, "obj" ) )
	{
		GOOD_PRINT_STRING( "3D: Reading Alias Wavefront obj file %s", filename );
		_file_type = F3D_ALIAS_OBJ;
	}
	else if( str_is_equal_nocase( ext, "svg" ) )
	{
		GOOD_PRINT_STRING( "3D : Reading SVG file %s", filename );
		_file_type = F3D_SVG;
	}
	else
	{
		bdd->box_err( "File %s unknown type.", filename );
	}

}

c_obj3d_file_reader::c_obj3d_file_reader()
{
	init();
	_b_read_tri = false;
}

//todo	get line then deal with content
INT32	c_obj3d_file_reader::get_next_obj_line( )
{
	while( _reader->get_size_from_cur() > 0 )
	{
		UINT8*	cur =  _reader->get_cur();
		CHAR start_char = *cur;
		if( ( start_char=='v' && ( *(cur+1)==' ' || *(cur+2)==' ' )) || ( start_char=='f' && *(cur+1)==' ' ) )	// handle v vn vt f
			return 1;

		switch( start_char )
		{		
		case 'o':
			if( b_verbose )
				ERR_PRINT_STRING( "o line : %.128s", cur );
			break;
		case 'g':
			{
				INT32 CONST	OBJ_INFO_NAME_LEN = 256;
				CHAR	name_obj[OBJ_INFO_NAME_LEN+1];
				if( (cur[1] >= 32) && (sscanf( (CHAR*)cur+2, "%256s", name_obj ) == 1) )
				{
					//if( b_verbose )
					GOOD_PRINT_STRING( "\tg_For_Object %s", name_obj );

					buf_id = special_get_bdd_tri_id( name_obj );

					//todo make a fn for that
					buf_name.set( name_obj );

					if( !buf_id )
					{
						CHAR*	cleaner = name_obj;
						CHAR*	marker = nullptr;
						while( *cleaner )
						{
							if( *cleaner == '_' )
							{
								*cleaner = ' ';
								++cleaner;
								if( !marker )
									marker = cleaner;
							}
							else
								++cleaner;
						}
						if( marker && (sscanf( marker, "%d", &buf_id ) == 1) )
						{
							GOOD_PRINT_STRING( "obj id %d", buf_id );
						}
						else
							buf_id = 256*256*256;	//should be last
					}
				}
			}
			break;
		case '#':
			if( b_verbose )
				GOOD_PRINT_STRING( "comment : %.128s", cur );
			break;
		case 's':
			if( b_verbose )
				GOOD_PRINT_STRING( "smooth group : %.128s", cur );
			break;
		case 'm':	//str_is_equal_nocase(str, "mtllib")
			if( b_verbose )
				GOOD_PRINT_STRING( "mtllib line : %.128s", cur );
			break;
		case 'u':	
			if( str_is_equal_nocase( (CHAR*)cur, "usemtl") )
			{	
				INT32 tmp = sscanf( (CHAR*)cur, "usemtl %d", &usemtl_index );
				if( tmp != 1 )
					usemtl_index = -1;
				if( b_verbose )
					GOOD_PRINT_STRING( "usemtl line : %.128s", cur );
			}
			else
			{
				if( b_verbose )
					ERR_PRINT_STRING( "u line : %.128s", cur );
			}
			break;
		case 'p':
			ERR_PRINT_STRING( "unrecognized yet : %.128s", cur );
			break;
		case 'l':
			ERR_PRINT_STRING( "unrecognized yet : %.128s", cur );
			break;
		default:
			BOX_ERR( "unrecognized yet : %.128s", cur );
			break;
		}
		_reader->skip_past_control();
	}

	return EOF;
}



bool	c_obj3d_file_reader::get_next_line_starting_with_char( CHAR the_char )
{
	while( !_reader->is_char(the_char) )
	{
		if( get_next_obj_line() <= 0 )
			return false;
	}
	return	true;
}

AAA_ERR	c_obj3d_file_reader::read_vertex_data_from_stream_obj()
{
	SPY_PUSH_RANGE( "read_vertex", spy::FILE );

	REAL*	ver	= _p_ver	+ 3 * _point_index_offset	;
	REAL*	uv	= _p_uv		+ 2 * _uv_index_offset		;
	REAL*	nor	= _p_nor	+ 3 * _normal_index_offset	;
	INT32	retcode = AAA_OK;
	FP32	f[3];

	_point_read		= 0;
	_uv_read		= 0;
	_normal_read	= 0;

	get_next_line_starting_with_char( 'v' );	
	do
	{
		CHAR* cur = (CHAR*)_reader->get_cur();	
		if( *cur == 'v' )
		{
			if( cur[1] == 't' )	//	uv cooor
			{
				if( uv )
				{
					_reader->skip_uint8(3);
					uv[0] = _reader->get_fp32();
					uv[1] = _reader->get_fp32();
					uv += 2;
					++_uv_read;
				}
			}
			else if( cur[1] == 'n' )	//	normal
			{
				if( nor )
				{
					_reader->skip_uint8(3);
					f[0] = _reader->get_fp32();
					f[1] = _reader->get_fp32();
					f[2] = _reader->get_fp32();
					if( f[0]==FP32(0) && f[1]==FP32(0) && f[2]==FP32(0) )
					{
						f[1] = FP32(1);
						//hack
						DBG_PRINT_STRING( "normal with index %d is NULL", _normal_index_offset + _normal_read );
					}
					if( _b_flip_x_at_read )
						f[0] = -f[0];
					if( _b_flip_yz_at_read )
					{
						nor[0] = f[0];
						nor[1] = f[2];
						nor[2] = -f[1];
					}
					else
						cpy_v3( nor, f );
					normalize_v3r( nor );
					nor += 3;
					++_normal_read;
				}
			}
			else
			{
				//	printf( "%s\n", _line );
				if( ver )
				{
					_reader->skip_uint8(2);
					f[0] = _reader->get_fp32();
					f[1] = _reader->get_fp32();
					f[2] = _reader->get_fp32();
					//do offset_scale
					if( _b_flip_x_at_read )
						f[0] = -f[0];
					if( _b_flip_yz_at_read )
					{
						ver[0] = _offset_at_load[0] + f[0];
						ver[1] = _offset_at_load[1] + f[2];
						ver[2] = _offset_at_load[2] - f[1];
					}
					else
						add_v3( ver, _offset_at_load, f );
					ver += 3;
					++_point_read;
				}
			}
		}
		_reader->skip_past_control();
		if( get_next_obj_line() < 0 )
			break;
	}
	while( !_reader->is_char('f') );

	SPY_POP_RANGE();
	return retcode;
}


AAA_ERR	c_obj3d_file_reader::skip_tri_from_stream_obj()
{
	SPY_PUSH_RANGE( "skip_tri", spy::FILE );

	INT32	retcode = AAA_OK;

	_tri_read			= 0;
#if AAA_POLY_COMPILE()
	_poly_read			= 0;
	_poly_index_read	= 0;
#endif
	get_next_line_starting_with_char( 'f' );
	do
	{
		_reader->skip_past_control();
		if( get_next_obj_line() < 0 )
			break;
	}
	while( _reader->is_char( 'v' ) );

	SPY_POP_RANGE();
	return retcode;
}

AAA_ERR	c_obj3d_file_reader::read_tri_from_stream_obj()
{
	SPY_PUSH_RANGE( "read_tri", spy::FILE );

	INT32	retcode = AAA_OK;
	INT32	offset_one;

	offset_one = b_start_at_one ? 1 : 0;

	INT32	tri_index_offset = _tri_index_offset * 3;

//	INT32*	p				= _bdd->_tri_from_file_x3			+ tri_index_offset - 1;		//	-1 to preincrement

	INT32*	pi				= (INT32 *)_bdd->_tri_index_point	+ tri_index_offset;
	INT32*	pin				= (INT32 *)_bdd->_tri_index_normal	+ tri_index_offset;
	INT32*	piuv			= (INT32 *)_bdd->_tri_index_uv		+ tri_index_offset;

#if AAA_POLY_COMPILE()
	UINT32*	p_poly_count	= nullptr;
	UINT32*	p_poly_index	= nullptr;
	if( _bdd->_b_poly_in_file )
	{
		p_poly_count	= _bdd->_poly_count					+ _poly_count_offset;
		p_poly_index	= _bdd->_poly_index_from_file		+ _poly_index_offset;
	}
#endif
	//todo we assume uv the same as point
	INT32	point_offset	=	_point_index_offset		+	_point_read;
	INT32	normal_offset	=	_normal_index_offset	+	_normal_read;
	INT32	uv_offset		=	_uv_index_offset		+	_uv_read;

	_tri_read			= 0;
#if AAA_POLY_COMPILE()
	_poly_read			= 0;
	_poly_index_read	= 0;
#endif

	get_next_line_starting_with_char( 'f' );
	do
	{
	//	INT32	count;
		//CHAR* pl = get_line()+1;
		//	printf( pl );
		//	first we get the number of point
		_reader->skip_uint8(2);
		INT32	loop_nb = asc_line::count_str_until_control( _reader->get_cur() );
		if( loop_nb < 3 )
		{
			retcode = ERR_ANY;
			BOX_ERR("Pb reading triangle data");
			break;
		}
		//	store it if we read polygon
#if AAA_POLY_COMPILE()
		if( p_poly_count )
			*p_poly_count++ = loop_nb;
#endif

		//	we do a triangle fan from the polygon
		//		123456 will produce 123 134 145 156
		for( INT32 j = 0; j < loop_nb; ++j )
		{
			//	read the index for one point
			INT32 ver_index = _reader->get_int32();

			if( _bdd->_b_normal_in_file )
			{
				//	we do triangle for triangle fans here using the starting point and the last point
				if( j > 2 )	//todoq	deal with poly and uv
				{	//we take the first and the last point of the previous triangle
					*pin = *(pin-3);	++pin;
					*pin = *(pin-2);	++pin;
					*pi = *(pi-3);		++pi;
					*pi = *(pi-2);		++pi;
				}
				if( _bdd->_b_uv_in_file )
				{
					if( j > 2 )	// try to deal with poly and uv
					{
						*piuv = *(piuv-3);	++piuv;
						*piuv = *(piuv-2);	++piuv;
					}
					//	format is ver/uv/nor
					_reader->skip_uint8(1);
					*piuv = _reader->get_uint32();
					_reader->skip_uint8(1);
				}
				else
				{
					//	format is ver//nor
					_reader->skip_uint8(2);
				}
				INT32 tmp = _reader->get_int32();
				if( tmp < 0 )
					*pin++ = tmp + normal_offset;
				else
					*pin++ = tmp - offset_one;
			}
			else
			{
				//	we do triangle for triangle fans here using the starting point and the last point
				if( j > 2 )	//todoq	deal with poly and uv
				{
					*pi = *(pi-3);	++pi;
					*pi = *(pi-2);	++pi;
				}
				if( _bdd->_b_uv_in_file )
				{
					if( j > 2 )	// try to deal with poly and uv
					{
						*piuv = *(piuv-3);	++piuv;
						*piuv = *(piuv-2);	++piuv;
					}
					//	format is ver/uv , don't know if ver/uv/ can happen
					_reader->skip_uint8(1);
					*piuv = _reader->get_uint32();
				}
				//else we have iy already
			}

			if( ver_index < 0 )
				ver_index += point_offset;
			else
				ver_index -= offset_one;
			*pi++ = ver_index;

#if AAA_POLY_COMPILE()
			if( p_poly_index )
			{
				*p_poly_index++ = tmp;
				++_poly_index_read;
			}
#endif
//			*++p = tmp * 3;
			if( _bdd->_b_uv_in_file )
			{
				*piuv -= 1;
				++piuv;
			}
		}
		_tri_read += loop_nb - 2;

#if AAA_POLY_COMPILE()
		++_poly_read;
#endif
		_reader->skip_past_control();
		if( get_next_obj_line() < 0 )
			break;
	}
	while( !_reader->is_char('v') );
	_bdd->_b_to_make_normal = true;

	SPY_POP_RANGE();
	return retcode;
}

/*
		if( str_is_equal_nocase( str, ":matrix" ) )
		{
			DOUBLE f[4];
			INT32 tmp;
			fscanf( file, " (%ld", &tmp);
			fscanf( file, "%lf %lf %lf %lf", f, f, f, f );
			fscanf( file, "%lf %lf %lf %lf", f, f, f, f );
			fscanf( file, "%lf %lf %lf %lf", f, f, f, f );
			fscanf( file, "%lf %lf %lf %lf", f, f+1, f+2, f+3 );
*/

AAA_ERR	c_obj3d_file_reader::count_elt_from_stream( c_obj3d_file_info* file_info_dst )
{
	AAA_ERR retcode = ERR_FILE_BASE;
	SPY_PUSH_RANGE( "count_elt", spy::FILE );

	c_obj3d_file_info info;

	UINT32	nb_mtl		= 0;


	switch( _file_type )
	{
	case F3D_ALIAS_OBJ:
		{
			CHAR	last_read = 0;
			INT32	line_count = 0;
//			INT32	ret;

			do
			{
				CHAR* cur  = (CHAR*)_reader->get_cur();
				++line_count;

				if( *cur == 'v' )
				{
					CHAR ch_next = *(cur+1);
					if( ch_next==0 || ch_next==' ' )	//	a vertex
					{
						++info._point;
						last_read = 'v';
					}
					else if( ch_next=='n' && (*(cur+2)==0 || *(cur+2)==' ') )	//	a normal
					{
						++info._normal;
						last_read = ch_next;
					}
					else if( ch_next=='t' && (*(cur+2)==0 || *(cur+2)==' ') )	//	a texture coordinate
					{
						++info._uv;
						last_read = ch_next;
					}
				}
				else if( *cur == 'f' )
				{
					++info._face;
					_reader->skip_uint8(2);
					INT32 count = _reader->count_str_until_control();
					if( count > 3 )
						info._b_poly = true;
					else if( count < 3 )
					{
						BOX_ERR( "In %s (line : %d) Bad face description.", _reader->get_filename(), line_count );
						goto exit;
					}
					info._tri += count - 2;

					if( last_read != 'f' )
						++info._obj;

					last_read = 'f';
				}
				else if( *cur == '#' )
				{
					//we print it if there is something
					_reader->skip_uint8(1);
					if( *_reader->get_cur() >= 32 )
					{
						INT32 len = _reader->get_len_til_control();
						UINT8* pt = _reader->get_cur() + len;
						UINT8 ch = *pt;
						*pt = 0;
						GOOD_PRINT_STRING( "\tline %d : %.*s", line_count, len, _reader->get_cur() );
						*pt = ch;
					}
				}
				else if( *cur == 'u' )
					++nb_mtl;
				_reader->skip_past_control();
			}
			while( _reader->get_size_from_cur() > 0 );

			//if( ret != -1 )
			//	goto exit;
		}
		break;
	}
	retcode = AAA_OK;

	*file_info_dst = info;

exit:
	SPY_POP_RANGE();
	_reader->seek_from_begin( 0 );
//	c_file::FSEEK_SET( file, INT32(0) );
	return retcode;
}

AAA_ERR	c_obj3d_file_reader::load_data_obj_alias( INT32 obj_nb_to_read, c_obj_info* info )
{
	AAA_ERR retcode = ERR_FILE_BASE;

	for( INT32 obj_index = 1; obj_index <= obj_nb_to_read; ++obj_index )
	{
		//	printf( "reading object %d\n"	, _obj_index);

		buf_name.erase();
		//HEAP_IS_CORRUPT();
		retcode = read_vertex_data_from_stream_obj();
		//HEAP_IS_CORRUPT();
		if( ERR(retcode) )
			goto exit;

		//todo at least skip the line
		if( _b_read_tri )
			retcode = read_tri_from_stream_obj();
		else
			retcode = skip_tri_from_stream_obj();

		if( ERR(retcode) )
			goto exit;

		if( info )
		{
			info->_point_start		= _point_index_offset;
			info->_tri_start		= _tri_index_offset;
#if AAA_POLY_COMPILE()
			info->_poly_start		= _poly_count_offset;
			info->_poly_index_start	= _poly_index_offset;
			info->_poly_nb			= _poly_read;
#endif
			info->_mtl_index		= usemtl_index;
			info->_obj_index		= _obj_nb;
			if( buf_id )	//need id always to save obj legend data
				info->_obj_info_id	= buf_id;
			else
				info->_obj_info_id	= _obj_nb + 1;
			info->_point_nb			= _point_read;
			info->_tri_nb			= _tri_read;

			clear_v3( info->_legend_tra );
			info->_legend_scale		= 1;
			//todo make a fn for that
			info->_name				= buf_name;
			++info;
		}

		_point_index_offset		+=	_point_read;
		_normal_index_offset	+=	_normal_read;
		_uv_index_offset		+=	_uv_read;
		_tri_index_offset		+=	_tri_read;
#if AAA_POLY_COMPILE()
		_poly_count_offset		+=	_poly_read;
		_poly_index_offset		+=	_poly_index_read;
		GOOD_PRINT_STRING( "\tLoaded Object(%d) : %d pt, %d nor, %d uv, %d tri, %d face.",
			obj_index,		_point_read, _normal_read, _uv_read, _tri_read, _poly_read );
#else
		GOOD_PRINT_STRING( "\tLoaded Object(%d) : %d pt, %d nor, %d uv, %d tri.",
			obj_index,		_point_read, _normal_read, _uv_read, _tri_read );
#endif
		++_obj_nb;
//		break;
	}
exit:
	return retcode;
}

void	c_obj3d_file_reader::set_dst_buffer( REAL* vertex, REAL* uv, REAL* nor )
{
	_p_ver	= vertex;
	_p_uv	= uv;
	_p_nor	= nor;
}

AAA_ERR	c_obj3d_file_reader::load_data( INT32 obj_nb_to_read, c_obj_info* info )
{
	AAA_ERR retcode;
	SPY_PUSH_RANGE( "load_data", spy::FILE );

		_obj_nb = 0;
		usemtl_index = -1;
		
		switch( _file_type )
		{
		case F3D_ALIAS_OBJ:	retcode = load_data_obj_alias( obj_nb_to_read, info );	break;
		default:			retcode = ERR_UNIMPLEMENTED_YET;						break;
		}

	SPY_POP_RANGE();
	return retcode;
}