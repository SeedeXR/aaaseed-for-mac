
#ifdef AAA_FILE_CSV_H
#error "FILE_CSV_H included more than once."
#endif
#define AAA_FILE_CSV_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class file_csv
{
public:
// OPEN CLOSE
// a call to open should ALWAYS correspond with a call to close (because of internal push/pop)
	static	FILE*	open(							C_PCHAR_C	filename,	bool CONST b_write = false );
	static	FILE*	open(							o_str CONST & filename, bool CONST b_write );
	static	void	close(							FILE* CONST	file		);

// WRITE
	static	void	write_linefeed(					FILE* CONST file		);

	static	AAA_ERR	write_int32(					FILE* CONST	file,		INT32 CONST data	);
	static	AAA_ERR	write_real(						FILE* CONST	file,		REAL  CONST	data	);

	static	INT32	write_fp32(						FILE* CONST	file,		FP32 CONST *       data,	INT32 CONST nb_x	);
	static	INT32	write_real(						FILE* CONST	file,		REAL CONST * CONST data,	INT32 CONST nb_x	);
	//todo all version don't skip trailing zero : done quickly please clean
	static	INT32	write_real_line(				FILE* CONST	file,		REAL CONST *       data,	INT32 CONST nb_x	);
	static	INT32	write_real_line_all(			FILE* CONST	file,		REAL CONST *       data,	INT32 CONST nb_x	);
	static	INT32	write_real_grid(				FILE* CONST	file,		REAL CONST *       data,	INT32 CONST nb_x,	INT32 CONST nb_y	);
	static	INT32	write_real_grid_all(			FILE* CONST	file,		REAL CONST *       data,	INT32 CONST nb_x,	INT32 CONST nb_y	);
	static	INT32	write_real_grid(				C_PCHAR_C	filename,	REAL CONST * CONST data,	INT32 CONST nb_x,	INT32 CONST nb_y	);
	static	INT32	write_real_grid_all(			C_PCHAR_C	filename,	REAL CONST * CONST data,	INT32 CONST nb_x,	INT32 CONST nb_y	);

	static	INT32	write_str( FILE* file,			C_PCHAR_C	str			);

// READ
	static	UINT32	read_float(						CHAR CONST * & pc,		REAL* dst,			INT32 CONST nb		);
	static	UINT32	read_int32(						CHAR CONST * & pc,		INT32* dst,			INT32 CONST nb		);
//	static	bool	read_float(						CHAR CONST * & pc,		REAL& dst			);
	static	UINT32	read_line_of_float(				C_PCHAR_C	line,		REAL* dst,			INT32 CONST nb_x	);

	static	INT32	read_float_grid(				FILE*		file,		REAL* CONST data,	INT32 CONST nb_x,	INT32 CONST nb_y	);
	static	INT32	read_float_grid(				C_PCHAR_C	filename,	REAL* CONST data,	INT32 CONST nb_x,	INT32 CONST nb_y	);

	static	void	count_float(					FILE*		file,							UINT32& nb_x,		UINT32& nb_y		);
	static	void	count_float(					C_PCHAR_C	filename,						UINT32& nb_x,		UINT32& nb_y		);

	static	INT32	read_int32_grid(				C_PCHAR_C	filename,	INT32* CONST data,	INT32 CONST nb_x,	INT32 CONST nb_y	);

	static	UINT32	read_line_of_int32_with_hole(	C_PCHAR_C	line,		INT32*       dst,	INT32 CONST nb_x	);
	static	INT32	read_int32_with_hole(			FILE*		file,		INT32* CONST dst,	INT32 CONST nb_x,	INT32 CONST nb_y	);
	static	INT32	read_int32_with_hole(			C_PCHAR_C	filename,	INT32*       data,	INT32 CONST nb_x,	INT32 CONST nb_y	);

protected:
private:
};

