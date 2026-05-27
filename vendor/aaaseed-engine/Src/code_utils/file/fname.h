
#ifdef AAA_FNAME_H
#error "FNAME_H included more than once."
#endif
#define AAA_FNAME_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


#ifdef WIN32
#	define	AAA_PATH_MAX()	_MAX_PATH
#else
#	define AAA_PATH_MAX()	260	//	max. length of full pathname
#endif

class o_str;
//todo make a singleton or make sure that wer don';t have to call dealloc explicitly
class fname final
{
public:
	static			C_PCHAR_C	get_fname(				C_PCHAR_C fname				);

	static			void		get_fname_and_ext(		C_PCHAR_C fname, C_PCHAR& start, INT32& len );
	//todoq deal with overloap on these two fns
	static			AAA_ERR		cpy_fname_pure(			CHAR* CONST dst,			C_PCHAR_C fname );
	static			AAA_ERR		cpy_dir(				CHAR* CONST dst,			C_PCHAR_C fname );
	static			AAA_ERR		cpy_fname_relative(		CHAR* CONST dst,			C_PCHAR_C fname,	C_PCHAR_C dir );
	static			AAA_ERR		cpy_fname_absolute(		CHAR* CONST dst,			C_PCHAR_C fname,	C_PCHAR_C dir );
//	static			AAA_ERR		replace_fname(			CHAR* CONST dst,			C_PCHAR_C name_new );
	static			AAA_ERR		drop_fname(				CHAR* CONST dst				);

	static			bool		is_to_be_ignored(		C_PCHAR_C		filename_or_dirname );
	static			bool		is_to_be_ignored(		o_str CONST &	filename_or_dirname );

	static			AAA_ERR		drop_ext(				CHAR* CONST fname			);
	static			AAA_ERR		drop_ext(				CHAR* CONST dst,			C_PCHAR_C fname );
	static			bool		have_ext(				C_PCHAR_C	fname			);
	static			bool		have_ext(				C_PCHAR_C	filename,		C_PCHAR_C ext );
	static			AAA_ERR		add_ext(				CHAR* CONST full_name,		C_PCHAR_C fname,	C_PCHAR_C ext );
	static			AAA_ERR		replace_ext(			CHAR* CONST fname,			C_PCHAR_C ext );
	static			AAA_ERR		cpy_and_replace_ext(	CHAR* dst,					C_PCHAR_C fname,	C_PCHAR_C ext );
	static			C_PCHAR_C	get_ext(				C_PCHAR_C	fname			);


//	static			AAA_ERR		serie_next(				CHAR* dst,					C_PCHAR_C src );
//	static			void		serie_make(				o_str& dst,					o_str CONST & root,	UINT32 number,	INT32 number_len,	o_str CONST & ext );
	static			void		serie_make(				o_str& dst,					o_str CONST & root,	UINT32 number,	INT32 number_len,	o_str CONST & ext );
	static			void		serie_split(			C_PCHAR_C	src,			o_str& root,		UINT32* number,	INT32* number_len,	o_str& ext );
};


