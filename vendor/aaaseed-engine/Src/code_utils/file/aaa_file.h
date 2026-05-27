
#ifdef AAA_AAA_FILE_H
#error "AAA_FILE_H included more than once."
#endif
#define AAA_AAA_FILE_H 1


//	On macOS the POSIX <fcntl.h> / <sys/fcntl.h> defines flag-bit macros
//	`FREAD` (0x1) and `FWRITE` (0x2) for open(2). The engine declares
//	`c_file::FREAD()` and `c_file::FWRITE()` member methods below ; if any
//	TU pulled fcntl.h before this header (e.g. via <mach/mach.h> in the
//	POSIX'd aaa_mem.cpp), the macros tokenize-replace the method names
//	and break parsing. Defensive undef gated on the compiler-native
//	`__APPLE__` macro (NOT the engine's AAA_OS_MAC() — this header is
//	parsed before aaa_type.h is included on many TUs).
#if defined( __APPLE__ )
#	undef FREAD
#	undef FWRITE
#endif

#ifndef _INC_TIME
#	include <time.h>
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_FNAME_H
#	include "file/fname.h"
#endif
#ifndef AAA_FILE_VIRTUAL_H
#	include "file_virtual.h"
#endif

// forward declaration to avoid cyclic dependancies
class c_file_master;

class	c_file final
{
public:
	static			bool			b_use_filesystem;
	static			c_file_master*	master;

	static			FILE*			file_log_open_file;
	static			o_str			dir_for_log;

private:
	static			bool			b_read_fast;
	static			bool			is_exist_low(				C_PCHAR_C		filename );
	static			bool			is_exist_ext_any_low(		o_str CONST &	filename );
	static			void			debug_break_vfile( CHAR CONST * CONST fn_str )
					{
						debug_break( "%s() when in vfile mode", fn_str );
					}
public:
	enum class TEXT_FILE_ENCODING
	{
		NO_BOM,				// ANSI, ASCII or UTF8 without BOM
		UTF8_BOM,			// UTF8 with BOM
		UTF16_LE_BOM,		// UTF16 LE with BOM
		UTF16_BE_BOM,		// UTF16-BE with BOM
	};

	static	CONST	time_t	TIME_UNDEFINED = -42;


	static			C_PCHAR_C	get_errno_str( INT32 CONST err_code );
	static			void		print_err( C_PCHAR_C signature, C_PCHAR_C filename, C_PCHAR_C mess );

	static	FINLINE	bool		is_read_fast()			{	return b_read_fast;		}	
	static	FINLINE	bool*		get_read_fast_pt()		{	return &b_read_fast;	}

	static	FINLINE	bool		is_vfile()				{	return c_file_virtual::is_active();	}
	static	FINLINE c_file_virtual::st_vfile CONST &	find_vfile_data(		C_PCHAR_C		key )	{	return c_file_virtual::get_data( key );		}			
	static	FINLINE c_file_virtual::st_vfile CONST &	find_vfile_data(		o_str CONST &	key )	{	return find_vfile_data( key.get() );	}

	// a call to open should ALWAYS correspond with a call to close
	static	FINLINE void open_vfile_save( o_str CONST &	filename, C_PCHAR_C mode = "wt" ) { c_file_virtual::open_vfile_save( filename, mode ); }
	static	FINLINE void open_vfile_load( o_str CONST &	filename, C_PCHAR_C mode = "rt" ) { c_file_virtual::open_vfile_load( filename, mode ); }
	static	FINLINE	void				close_vfile()			{	c_file_virtual::close_vfile();		}
	// a call to push should ALWAYS correspond with a call to pop
	static	FINLINE	void				push_vfile()			{	c_file_virtual::push_vfile();		}
	static	FINLINE	void				pop_vfile()				{	c_file_virtual::pop_vfile();		}
		
//	static			INT32		local_stat(				C_PCHAR_C filename,			struct _stat64* CONST buf );
//	static			INT32		local_stat(				FILE* file,					struct _stat64* CONST buf );

	static			time_t		get_mdate(				C_PCHAR_C		filename );
	static	FINLINE	time_t		get_mdate(				o_str CONST &	filename )		{	return get_mdate(filename.get());	}

	static	FINLINE	bool		is_exist_real_file(		C_PCHAR_C		filename )
					{
						return is_exist_low( filename ) && !fname::is_to_be_ignored(filename);
					}
	static			bool		is_existing_dir(		C_PCHAR_C		filename );
	static	FINLINE	bool		is_exist(				C_PCHAR_C		filename )
					{
						if( is_vfile() )
							return find_vfile_data( filename ).data != nullptr ;
						return is_exist_low(filename) && !fname::is_to_be_ignored(filename);
					}
	static	FINLINE	bool		is_exist_real_file(		o_str CONST &	filename )
					{
						return is_exist_low( filename.get() ) && !fname::is_to_be_ignored(filename);
					}
	static	FINLINE	bool		is_exist(				o_str CONST &	filename )
					{
						if( is_vfile() )
							return find_vfile_data( filename ).data != nullptr ;
						return is_exist_real_file(	filename );
					}
	static	FINLINE	bool		is_exist_ext_any_real_file(		o_str CONST &	filename )
					{
						return is_exist_ext_any_low(filename) && !fname::is_to_be_ignored(filename);
					}
	static	FINLINE	bool		is_exist_ext_any(		o_str CONST &	filename )
					{
						//todo
						//if( is_vfile() )
						//	return find_vfile_data( filename ).data != nullptr ;
						return is_exist_ext_any_real_file(	filename );
					}
	static			time_t		get_mdate(				FILE* CONST file );

	static			INT64		get_size(				C_PCHAR_C		filename	);
	static	FINLINE	INT64		get_size(				o_str CONST & 	filename	)	{ return get_size(filename.get());	}
	static			INT64		get_size(				FILE* CONST		file		);

	static			FILE*		FOPEN(					o_str CONST &	filename,	C_PCHAR_C mode );
	static			FILE*		FOPEN(					C_PCHAR_C		filename,	C_PCHAR_C mode );
	static			AAA_ERR		FCLOSE(					FILE* CONST		file		);

	static	FINLINE	INT32		FREMOVE( C_PCHAR_C filename )
								{
									return is_vfile() ? 0 : ::remove( filename );
								}
	static	FINLINE	INT32		FREMOVE( o_str CONST & filename )
								{
									return is_vfile() ? 0 : ::remove( filename.get());
								}

	static	FINLINE	INT32		FPUTS( C_PCHAR_C str, FILE* CONST file )
								{
									// we don't check on FILE*
									//if(	is_vfile() )
									//{
									//	debug_break_vfile( __FUNCTION__ );
									//	return EOF;
									//}
									return fputs( str, file );
								}
private:
	static	FINLINE	INT32		FSEEK( FILE* CONST file, INT32 CONST offset, INT32 CONST how )
								{
									// we don't check on FILE*
									//if(	is_vfile() )
									//{
									//	debug_break_vfile( __FUNCTION__ );
									//	return -1;
									//}
									return fseek( file, offset, how );
								}
public:
	static	FINLINE	INT32		FSEEK_SET(				FILE* CONST	file,		INT32 CONST offset )	{	return FSEEK( file, offset, SEEK_SET );	}
	static	FINLINE	INT32		FSEEK_CUR(				FILE* CONST	file,		INT32 CONST offset )	{	return FSEEK( file, offset, SEEK_CUR );	}
	static	FINLINE	INT32		FSEEK_END(				FILE* CONST	file,		INT32 CONST offset )	{	return FSEEK( file, offset, SEEK_END );	}
	static	FINLINE	INT32		FTELL( FILE* CONST file )
								{
									// we don't check on FILE*
									//if(	is_vfile() )
									//{
									//	debug_break_vfile( __FUNCTION__ );
									//	return -1;
									//}
									return ftell( file );
								}

	static	FINLINE	size_t		FREAD( CP_VOID dst, size_t CONST size, size_t CONST count, FILE* CONST file )
								{
									// we don't check on FILE*
									//if(	is_vfile() )
									//{
									//	debug_break_vfile( __FUNCTION__ );
									//	return 0;
									//}
									return fread( dst, size, count, file );
								}
	static	FINLINE	size_t		FWRITE( CP_CVOID src, size_t CONST size, size_t CONST count, FILE* CONST file )
								{
									// we don't check on FILE*
									//if(	is_vfile() )
									//{
									//	debug_break_vfile( __FUNCTION__ );
									//	return 0;
									//}
									return fwrite( src, size, count, file );
								}
	static	FINLINE	INT32		FPUTC( INT32 CONST ch, FILE* CONST file )
								{
									// we don't check on FILE*		
									//if(	is_vfile() )
									//{
									//	debug_break_vfile( __FUNCTION__ );
									//	return EOF;
									//}
									return fputc( ch, file );
								}
	static			AAA_ERR		FREAD_CHECK(				CP_VOID dst,			size_t CONST size,				FILE* CONST file );
	static			AAA_ERR		FWRITE_CHECK(				CP_CVOID src,			size_t CONST size,				FILE* CONST file );

	static			AAA_ERR		read(						CP_VOID dst,			size_t CONST size,				C_PCHAR_C filename );
	static			AAA_ERR		write(						CP_CVOID src,			size_t CONST size,				C_PCHAR_C filename );


	static			void		start_log_open_file(		C_PCHAR_C filename		);
	static			void		stop_log_open_file();

	static			bool		is_time_changed(			o_str CONST & filename,			time_t& time );


	static			TEXT_FILE_ENCODING	check_bom( FILE* CONST	file );
};
