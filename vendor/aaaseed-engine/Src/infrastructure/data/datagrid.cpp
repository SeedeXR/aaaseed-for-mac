#include "infrastructure/data/datagrid.h"
#include "file/file_csv.h"
#include "code_utils/md5.h"
#include "infrastructure/seedfile.h"
#include "file/file_xls.h"

//extern	void sql_query( CHAR* query_string );
		
FACTORY_CREATE_PROP_V1( c_bdd_datagrid, bdd_datagrid, Data Grid, bdd_datagrid, sub_menu="Data"; );


extern	DOUBLE	g_timecode_unit;

namespace {
				INT32	datagrid_id			= 0;

	CONSTEXPR	INT32	MD5_HEADER_LEN		= 13;
	CONSTEXPR	INT32	MD5_CSV_HEADER_LEN	= (MD5_HEADER_LEN+32);
	CHAR	md5_header[MD5_HEADER_LEN+1] = "checksum;md5;";
}

namespace	n_bdd_datagrid
{
	CONSTEXPR INT32	BASE_PARAM_NB_MAX	=	13 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB_MAX	=	0;
	CONSTEXPR INT32	PARAM_NB_MAX		=	BASE_PARAM_NB_MAX
										+	GROUP_PARAM_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(		active				)
//		PARAM_DEF_BOOL_OFF(		verbose				)
		PARAM_DEF_BOOL_ON(		check_for_change	)
		PARAM_DEF_BOOL_OFF(		need_checksum		)
		PARAM_DEF_BOOL_OFF(		comma_is_separator	)
		PARAM_DEF_BOOL_OFF(		data_load_trig		)
		PARAM_DEF_BOOL_OFF(		data_new			)
		PARAM_DEF_INT32_LOCKED(	data_load_count		)
		PARAM_DEF_BOOL_OFF(		data_save			)
		PARAM_DEF_BOOL_OFF(		data_save_trig		)

		PARAM_DEF_BOOL_OFF(		local				)

		PARAM_DEF_FILENAME(		data_filename,		aaa::file::TYPE_IO_CSV, 0	)

		PARAM_DEF_INT32_LOCKED(	y_max				)
		PARAM_DEF_BOOL_OFF(		pop_trig			)

//		PARAM_DEF_STR(			sql_query			)
//		PARAM_DEF_BOOL_OFF(		sql_query_trig		)
	};
}

void	c_bdd_datagrid::param_init_pt()
{
	INT32	h = param_init_pt_no_geo( );

	param_set_pt( h, get_pt_active()		);
//	param_set_pt( h, _b_verbose				);
	param_set_pt( h, _b_check_for_change_ui	);
	param_set_pt( h, _b_need_checksum		);
	param_set_pt( h, _b_comma_is_separator	);
	param_set_pt( h, _b_load_trig			);
	param_set_pt( h, _b_data_new_ui			);
	param_set_pt( h, _data_load_count		);
	param_set_pt( h, _b_save				);
	param_set_pt( h, _b_save_trig			);
	param_set_pt( h, _b_local_ui			);
	param_set_pt( h, _data_filename			);

	param_set_pt( h, _y_max					);
	param_set_pt( h, _b_pop_trig_ui			);

//	param_set_pt( h, _sql_query_text		);
//	param_set_pt( h, _b_sql_query_trig		);

	err_param_init_pt(h);
}

//todoqq reset ? clear ? reinit ?
void	c_bdd_datagrid::init()
{
//	_b_first = true;
	_data_size = 0;
	_time_modified = 0;
	_b_changed = false;
	_y_max = 0;
	_datalines			=	new std::deque<c_data_line*>;
	_datalines_store	=	new std::deque<c_data_line*>;
	_dataline_push		=	new c_data_line;	//always ready
	_b_data_new_ui		=	false;
	_data_load_count	=	0;
	_data_fname_last.erase();
}

CONSTRUCTOR_CREATE( c_bdd_datagrid )
{
	CHAR str[] = "Datagrid --     ";
	init();

	sprintf( str + 9, "%2d", ++datagrid_id );
	set_name( str );
	param_init_with( n_bdd_datagrid::param, n_bdd_datagrid::PARAM_NB_MAX );
}
c_bdd_datagrid::~c_bdd_datagrid()
{
	dealloc();
}

void	c_bdd_datagrid::clear_datalines( std::deque<c_data_line*>* datalines )
{
	INT32	size = (INT32)datalines->size();
	for( INT32 y=0; y<size; ++y )
	{
		c_data_line*	data_line = (*datalines)[y];
		if( data_line )
		{
			delete data_line;
			(*datalines)[y] = nullptr;
		}
	}
	//datalines.clear();	//tooslow ?
}

void	c_bdd_datagrid::dealloc()
{
	if( b_aaa_exiting && b_aaa_exiting_fast )
		return;

	obj_delete( _dataline_push );
	clear_datalines( _datalines );
	clear_datalines( _datalines_store );
	obj_delete( _datalines );
	obj_delete( _datalines_store );
}

void	c_bdd_datagrid::update()
{
	_b_changed = false;
	_b_data_new_ui = false;
	if( is_active() )
	{
		make_fname( _data_fname );
		C_PCHAR str_read = nullptr;
		if( !_data_fname_last.is_equal(_data_fname) )	// name changed or first time
		{
			if( c_file::is_exist( _data_fname ) )
				str_read = "c_bdd_datagrid::update() did not read this file yet";
			else
			{
				_data_fname_last.set( _data_fname );
				NO_MEDIA_PRINT_STRING( "c_bdd_datagrid \"%s\" no file \"%s\"", get_my_filename(), _data_fname.get() );
			}
		}
		else if( _b_check_for_change_ui )	// date changed
		{
			time_t mtime = c_file::get_mdate( _data_fname );
			if( mtime && _time_modified != mtime )
				str_read = "c_bdd_datagrid::update() file time modified";
		}
		//else if( _b_first )
		//{
		//	str_read = "c_bdd_datagrid::update() file first";
		//	_b_first = false;
		//}
		if( _b_load_trig )
		{
			str_read = "c_bdd_datagrid::update() read trig";
			_b_load_trig = false;
		}
		if( str_read )
			read( str_read );

		if( _b_save_trig )
		{
			save_data();
			_b_save_trig = false;
		}
		if( _b_pop_trig_ui )
		{
			_b_pop_trig_ui = false;
			pop();
		}
	}
/*	if( b_sql_query_trig )
	{
		sql_query( _sql_query_text.get() );
		_b_sql_query_trig = false;
	}
*/
}

FINLINE	c_data_line*	c_bdd_datagrid::get_data_line( INT32 y ) CONST
{
	if( y<0 )
		return _dataline_push;
	//else if( y>=_datalines.size() )
	//	return nullptr;
	if( UINT32(y) < _datalines->size() )
		return (*_datalines)[y];
	return nullptr;
}
c_data_line*	c_bdd_datagrid::create_data_line( INT32 y )
{
	c_data_line*	p_data_line = new c_data_line;
	if( p_data_line )
	{
		INT32	nb = y - (INT32)_datalines->size() + 1;
		_datalines->resize( _datalines->size() + nb, nullptr );
		//while( nb > 0 )
		//{
		//	_datalines->push_back(nullptr);
		//	--nb;
		//}
		(*_datalines)[y] = p_data_line;
	}
	return p_data_line;
}

bool	c_bdd_datagrid::is_number( INT32 x, INT32 y ) CONST
{
	c_data_line*	p_data_line = get_data_line( y );
	return p_data_line ? p_data_line->is_number( x ) : false;
}

bool	c_bdd_datagrid::has_data( INT32 x, INT32 y ) CONST
{
	c_data_line*	p_data_line = get_data_line( y );
	return p_data_line ? p_data_line->has_data( x ) : false;
}

FP32	c_bdd_datagrid::get_fp32( INT32 x, INT32 y ) CONST
{
	c_data_line*	p_data_line = get_data_line( y );
	return p_data_line ? p_data_line->get_fp32( x ) : 0;
}

DOUBLE	c_bdd_datagrid::get_double( INT32 x, INT32 y ) CONST
{
	c_data_line*	p_data_line = get_data_line( y );
	return p_data_line ? p_data_line->get_double( x ) : 0;
}

o_str CONST &	c_bdd_datagrid::get_str( INT32 x, INT32 y ) CONST
{
	c_data_line*	p_data_line = get_data_line( y );
	return p_data_line ? p_data_line->get_str( x ) : o_str::o_empty;
}

CHAR	c_bdd_datagrid::get_char( INT32 x, INT32 y, INT32 char_index ) CONST
{
	c_data_line*	p_data_line = get_data_line( y );
	return p_data_line ? p_data_line->get_char( x, char_index ) : 0;
}

void	c_bdd_datagrid::set_double( INT32 x, INT32 y, DOUBLE d )
{
	c_data_line*	p_data_line = get_data_line( y );
	if( p_data_line )
		p_data_line->set_double( x, d );
	else
	{
		//if( d != .0 )	//june 2018 Maa remove so we can store 0 and nil differently
		{
			p_data_line = create_data_line(y);
			if( p_data_line )
				p_data_line->set_double( x, d );
		}
	}
}

void	c_bdd_datagrid::set_str( INT32 x, INT32 y, C_PCHAR_C str )
{
	c_data_line*	p_data_line = get_data_line( y );
	if( p_data_line )
		p_data_line->set_str( x, str );
	else
	{
		if( str )
		{
			p_data_line = create_data_line(y);
			if( p_data_line )
				p_data_line->set_str( x, str );
		}
	}
}

void	c_bdd_datagrid::set_str_and_double(	INT32 x, INT32 y,	C_PCHAR_C str,	DOUBLE d	)
{
	c_data_line*	p_data_line = get_data_line( y );
	if( p_data_line )
		p_data_line->set_str_and_double( x, str, d );
	else
	{
		if( str || d != .0 )
		{
			p_data_line = create_data_line(y);
			if( p_data_line )
				p_data_line->set_str_and_double( x, str, d );
		}
	}
}

void	c_bdd_datagrid::insert_double( INT32 x, INT32 y, DOUBLE in )
{
	c_data_line*	p_data_line = get_data_line( y );
	if( p_data_line)
		p_data_line->insert_double( x, in );
	else
	{
		if( in != 0. )
		{
			p_data_line = create_data_line(y);
			if( p_data_line )
				p_data_line->insert_double( x, in );
		}
	}
}

void	c_bdd_datagrid::insert_str( INT32 x, INT32 y, C_PCHAR_C in )
{
	c_data_line*	p_data_line = get_data_line( y );
	if( p_data_line )
		p_data_line->insert_str( x, in );
	else
	{
		if( in )
		{
			p_data_line = create_data_line(y);
			if( p_data_line )
				p_data_line->insert_str( x, in );
		}
	}
}

UINT32 c_bdd_datagrid::find_y_max()	CONST //todoqq change name ?
{
	c_data_line*	p_data_line;
	for( INT32 y = (INT32)_datalines->size()-1; y >= 0; --y )
	{
		p_data_line = (*_datalines)[y];
		if( p_data_line && p_data_line->has_data() )
		{
			return y+1;
		}
	}
	return 0;
}

void	c_bdd_datagrid::push()		//todo limited to 1024 for now
{
	if( _y_max >= _datalines->size() )	//todo we had a maximum before (redo ?)
	{
		_datalines->push_back( _dataline_push );
		_dataline_push = new c_data_line;	//always ready
	}
	else
	{
		c_data_line*	p;
		p = (*_datalines)[_y_max];
		(*_datalines)[_y_max] = _dataline_push;
		if( p )
		{
			_dataline_push = p;				//always ready
			p->clear();
		}
		else
			_dataline_push = new c_data_line;	//always ready
	}
	++_y_max;
}

void	c_bdd_datagrid::pop()		//todo limited to 1024 for now
{
	c_data_line*	p = (*_datalines)[0];
	SAFE_DELETE( p );
	_datalines->pop_front();
	//--_y_nb;		//todoqq	check this and connected variables
	--_y_max;	//todoqq
}


void	c_bdd_datagrid::make_fname( o_str& fname )
{
	if( _b_local_ui )
	{
		fname.set_dir_name( get_name_search() );
//		DBG_PRINT_STRING( "dir is %s", fname.get() );
		fname.add_slash();
		fname.add( _data_filename );
//		DBG_PRINT_STRING( "fname is %s", fname.get() );
	}
	else
		fname.set( _data_filename );

}


AAA_ERR	c_bdd_datagrid::read_libreoffice( C_PCHAR_C signature )
{
#if AAA_LIBREOFFICE_USE()
	AAA_ERR	retcode = AAA_OK;
	c_file_xls xls_reader;
	o_str sheet_name;
	retcode = xls_reader.open( _data_fname, sheet_name );
	if( retcode == AAA_OK )
	{
		SWAP( _datalines, _datalines_store );	//	so we could restore if we can't read
		clear_datalines( _datalines );	//do more reuse
		for( INT32 row = 0; row < xls_reader.get_row_count(); ++row )
		{
			auto dataline = get_data_line( row );
			if( !dataline )
			{
				dataline = create_data_line( row );
			}
			if( dataline )
			{
				dataline->clear();
				xls_reader.get_data_line( 0, row, dataline );
			}
			else
			{
				retcode = ERR_ANY;
			}
		}
		if( retcode == ERR_ANY )
		{
			SWAP( _datalines, _datalines_store );
			ERR_PRINT_STRING( "Could not read this, reverting to previous data." );
		}
		else
		{
			_y_max = find_y_max();
		}
		xls_reader.close();
		_time_modified = c_file::get_mdate( _data_fname );
		_b_data_new_ui = true;
		_data_load_count++;
		retcode = AAA_OK;
	}
	else
	{
		ERR_PRINT_STRING( "c_bdd_datagrid : Error opening %s", _data_fname.get() );
	}
	return retcode;
#else
	err_print_unused( "LibreOfficeSDK", __FUNCTION__ );
	return ERR_NOT_COMPILED;
#endif	//#if AAA_LIBREOFFICE_USE()
}

//todoqq correct bug
//			still can read bad data
AAA_ERR	c_bdd_datagrid::read( C_PCHAR_C signature )
{
	AAA_ERR	retcode		= ERR_ANY;
	CHAR*	new_data	= nullptr;

	if( c_file::is_exist( _data_fname ) )
	{
		auto CONST ext = _data_fname.get_ext();
		if( str_is_equal_nocase( ext, "xls" ) || str_is_equal_nocase( ext, "ods" ) || str_is_equal_nocase( ext, "xlsx" ) )
		{
			return read_libreoffice( signature );
		}
		else
		{
			//was done on file after and didn't work (see below)
			time_t	file_date = c_file::get_mdate( _data_fname );
			GOOD_PRINT_STRING( "Begin to read %s", _data_fname.get() );
			FILE* file = c_file::FOPEN( _data_fname, "rb" );
			if( file )
			{
				_data_size = c_file::get_size( file );
				new_data = (CHAR*)MALLOC_SIGNATURE( _data_size + 2, signature );
				//		CHAR*	new_data = (CHAR*) REALLOC( (void*)data, data_size_+1 );
				if( new_data == 0 )
					c_file::FCLOSE( file );
				else
				{
					//this was causing a permanent reload under vs2013 we move it on the filename
					//time_t	file_date = c_file::get_mdate( file );
					_b_changed = true;
					size_t nb_read = c_file::FREAD( new_data, sizeof( CHAR ), _data_size, file );
					c_file::FCLOSE( file );
					if( nb_read == _data_size )
					{
						if( _b_need_checksum && _data_size > MD5_CSV_HEADER_LEN )
						{
							CHAR* pt;
							pt = new_data + _data_size - MD5_CSV_HEADER_LEN;
							if( str_is_equal( md5_header, pt, MD5_HEADER_LEN ) )
							{
								MD5_CTX			md5;
								unsigned char	checksum_here[ 16 ];
								unsigned char	tmp;
								INT32	i;
								MD5Init( &md5 );
								MD5Update( &md5, ( CONST unsigned char* )new_data, (int)nb_read - MD5_CSV_HEADER_LEN );
								MD5Final( checksum_here, &md5 );
								pt += MD5_HEADER_LEN;

								for( i = 0; i < 16; ++i )
								{
									if(      '0' <= *pt && *pt <= '9' )
										tmp = ( *pt - '0' ) << 4;
									else if( 'a' <= *pt && *pt <= 'f' )
										tmp = ( *pt - 'a' + 10 ) << 4;
									else if( 'A' <= *pt && *pt <= 'F' )
										tmp = ( *pt - 'A' + 10 ) << 4;
									else
										tmp = 0;
									++pt;

									if(      '0' <= *pt && *pt <= '9' )
										tmp += ( *pt - '0' );
									else if( 'a' <= *pt && *pt <= 'f' )
										tmp += ( *pt - 'a' + 10 );
									else if( 'A' <= *pt && *pt <= 'F' )
										tmp += ( *pt - 'A' + 10 );
									else
										tmp = 0;
									++pt;

									if( tmp != checksum_here[ i ] )
										break;
								}
								if( i != 16 )
								{
									ERR_PRINT_STRING( "%s bad checksum", __FUNCTION__ );
									goto exit;
								}
							}
							_data_size -= MD5_CSV_HEADER_LEN;
						}

						*( new_data + _data_size ) = 0x0a;
						*( new_data + _data_size + 1 ) = 0x0;
						SWAP( _datalines, _datalines_store );	//	so we could restore if we can't read
						clear_datalines( _datalines );	//do more reuse
						retcode = read_data( new_data );
						if( ERR( retcode ) )
						{
							SWAP( _datalines, _datalines_store );
							ERR_PRINT_STRING( "Could not read this, reverting to previous data." );
						}
						else
						{
							_y_max = find_y_max();
							GOOD_PRINT_STRING( "End to read %s", _data_fname.get() );
							GOOD_PRINT_STRING( "\t%d lines processed", _y_max );
						}
						_time_modified = file_date;
						_b_data_new_ui = true;
						_data_fname_last.set(_data_fname);
						_data_load_count++;
						goto exit_no_mess;
					}
				}
			}
		}
	}
	else
	{
		NO_MEDIA_PRINT_STRING( "c_bdd_datagrid no file %s", _data_fname.get() );
		goto exit_no_mess;
	}
exit:
	if( ERR(retcode) )
		ERR_PRINT_STRING( "c_bdd_datagrid trouble to read" );
exit_no_mess:
	IF_FREE_AND_NULL( new_data );
	return retcode;
}

AAA_ERR	c_bdd_datagrid::save_data()
{
	AAA_ERR	retcode	= ERR_ANY;
	FILE*	file	= c_file::FOPEN( _data_fname, "wb" );
	if( file )
	{
		C_PCHAR			separator = _b_comma_is_separator ? "," : ";";
		//find the max nb of line
		INT32	max = find_y_max();
		for( INT32 y = 0; y < max; ++y )
		{
			if( y )
				fprintf( file, "\x0d\x0a" );

			c_data_line*	p_data_line = (*_datalines)[y];
			if( p_data_line )
			{
				INT32	x_nb = p_data_line->get_size();
				for( ; x_nb >0; --x_nb )
				{
					if( p_data_line->has_data( x_nb-1 ) )
						break;
				}

				for( INT32 x = 0; x < x_nb; ++x )
				{
					if( x )
						fprintf( file, "%s", separator );
					INT32	ret;
					if( p_data_line->is_number( x ) )
						ret = fprintf( file, "%.16g", p_data_line->get_double( x ) );
					else
						ret = file_csv::write_str( file, p_data_line->get_str( x ).get() );
					if( ret < 0 )
						goto exit;
				}//x
			}
		}//y
		retcode = AAA_OK;
		GOOD_PRINT_STRING( "c_bdd_datagrid save data in %s", _data_fname.get() );
		_time_modified = c_file::get_mdate( file );
	}
exit:
	if( file )
		c_file::FCLOSE( file );
	if( ERR( retcode ) )
		ERR_PRINT_STRING( "c_bdd_datagrid trouble to write" );
	return retcode;
}

AAA_ERR	c_bdd_datagrid::save_do_after( o_str CONST & filename ) // Jean say : Do we need remove it ? (seems unused)
{
	if( _b_save )
		return save_data();
	return AAA_OK;
}

AAA_ERR	c_bdd_datagrid::read_data( CHAR* dst_in )
{
	UINT8*	src;
	UINT8*	dst;
	UINT8*	mark;
	UINT8	cur;

	INT32	col;
	INT32	line;

	DOUBLE	dum;
	CHAR	toto[2];
	bool	b_str;

	src = dst = (UINT8*)dst_in;
	line = 0;
	col = 0;

	mark = dst;
	b_str = false;

	for( ; ; )
	{
		cur = *src;
		if( cur == '"' )	// if " this will be a str
		{
			b_str = true;
			for( ; ; )
			{
				++src;
				if( *src == 0 )
				{
					ERR_PRINT_STRING( "bad csv file trouble with \" character in (%s)", _data_fname.get() );
					return ERR_ANY;
				}
				else if( *src == '"' )
				{
					if( *(src+1) != '"' )
						break;
					else
					{
						*dst++ = '"';
						++src;
					}
				}
				else
					*dst++ = *src;
			}
		}
		else if( cur == ';' || ( _b_comma_is_separator && cur == ',' ) || cur == 0x0a )
		{
			*dst = 0;
			if( b_str )
				set_str( col, line, (CHAR*)mark );
			else
			{
				if( *(mark + 2 ) == ':' && *( mark + 5 ) == ':' )	// look like we got timecode
				{
					INT32	m, s, i;
					DOUBLE	tc = 0;
					bool	b_num = false;
					if( ((dst-mark)==11) && (*(mark+8)==':') )
					{	//	we got hours -> hh:mm:ss:ii
						INT32	h;
						if( sscanf( (CHAR*)mark, "%d:%d:%d:%d", &h, &m, &s, &i ) == 4 )
						{
							tc = 60. * 60. * h;
							b_num = true;
						}
					}
					else
					{	//	we got only -> mm:ss:ii
						if( sscanf( (CHAR*)mark, "%d:%d:%d", &m, &s, &i ) == 3 )
							b_num = true;
					}
					if( b_num )
					{
						tc += 60. * m;
						tc += s;
						tc += g_timecode_unit * i;
						set_str_and_double( col, line, (CHAR*)mark, tc );	//we store both 
					}
					else
						set_str( col, line, (CHAR*)mark );	
				}
				else if( sscanf( (CHAR*)mark, "%lg%1s", &dum, toto ) == 1 )	//	6Go should be text so "%f" is not enough
					set_double( col, line, dum );
				else
					set_str( col, line, (CHAR*)mark );
			}
			mark = ++dst;
			b_str = false;
			if( cur == 0x0a )
			{
				++line;
				col = 0;
				if( (size_t)(src-(UINT8 *)dst_in) >= _data_size )
					break;
			}
			else
			{
				++col;
			}
		}
		else if( cur >= 32 )
			*dst++ = cur;
		++src;
	}
	return AAA_OK;
}

void	c_bdd_datagrid::set_data_filename( o_str CONST & filename )
{
	_data_filename.set( filename );
}

AAA_ERR	c_bdd_datagrid::load_data_from_filename( o_str CONST & filename, INT32 type_io )
{
	switch( type_io )
	{
	case aaa::file::TYPE_IO_CSV:
		{
			o_str& filename_rel = o_str::push_name();
				//	place a relative path in the filename
				filename_rel.set_fname_relative( filename );
				set_data_filename( filename_rel );
			o_str::pop_name();
		}
		return AAA_OK;
	}
	return ERR_TYPE_UNUSED;
}
