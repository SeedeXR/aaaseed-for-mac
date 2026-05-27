
#ifdef AAA_DIRPARSER_H
#error "DIRPARSER_H included more than once."
#endif
#define AAA_DIRPARSER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifdef	WIN32
#	ifndef _INC_IO
#		include <io.h>
#	endif
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

//todo add get_next_file() and get_next_dir() to optimize parsing
class	c_dir_parser final : public c_obj
{
private:
	o_str				_dir;
	o_str				_pat;
	bool				_b_need_first;
	bool				_b_dir_valid;
#ifdef	WIN32
	struct _finddata_t	_file_info;
	intptr_t			_h_file;
#endif
protected:

public:
	static	void	test(		C_PCHAR_C dir,			C_PCHAR_C pattern_in );
	static	bool	have_file(	C_PCHAR_C dir,			C_PCHAR_C pattern_in );

	c_dir_parser();
	c_dir_parser(				C_PCHAR_C dir_in,		C_PCHAR_C pattern_in );
	virtual ~c_dir_parser();

	void		set(			C_PCHAR_C dir_in,		C_PCHAR_C pattern_in );
	void		set_dir(		C_PCHAR_C dir_in		);
	void		set_pattern(	C_PCHAR_C pattern_in	);
	void		open();
	bool		get_next();
	bool		get_next_dir();
	bool		get_next_file();
	void		close();

	C_PCHAR_C	get_name() CONST;
	bool		is_dir() CONST;

	void		list();
};


