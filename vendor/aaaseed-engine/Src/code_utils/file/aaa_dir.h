
#ifdef AAA_AAA_DIR_H
#error "AAA_DIR_H included more than once."
#endif
#define AAA_AAA_DIR_H 1


#ifndef AAA_AAA_FILE_H
#	include "aaa_file.h"
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif


class	c_dir	final
{
private:
	static	o_str	dir_start;
	static	o_str	dir_kernel;
	static	o_str	dir_net;
	static	o_str	dir_user;

public:
	static			bool		b_verbose_def;
	static			bool		is_exist(					C_PCHAR_C dir_name );
	static			bool		is_exist(					o_str CONST & dir_name );

	static			AAA_ERR			make(					o_str CONST &	dir_name	);


	static			AAA_ERR			remove(					C_PCHAR_C		dir_name	);

	static			void			reset();
	static			AAA_ERR			change(					C_PCHAR			dir,		bool CONST b_create=false );

	static			C_PCHAR_C		update_cur();
	static			o_str CONST &	get_cur();

	static			AAA_ERR			push_def(				C_PCHAR			dirname,	bool CONST b_create=false );
	static			AAA_ERR			push_def(				o_str CONST &	dirname,	bool CONST b_create=false );
	static			AAA_ERR			push_def_from_filename( C_PCHAR_C		dirname,	bool CONST b_create=false );
	static			AAA_ERR			pop_def();

	static			AAA_ERR			set_def(				C_PCHAR			dirname,	bool CONST b_create=false );
	static			AAA_ERR			set_def(				o_str CONST &	dirname,	bool CONST b_create=false )	{	return set_def( dirname.get(),	b_create );	}
	static			o_str CONST &	get_def();
	static			o_str CONST &	get_def_from_index(		INT32 CONST		index );


	static	FINLINE	void			change_to_def()		{ change( get_def().get() );	}

//unused	static	void			change_to_pref();

//	static			o_str CONST &	get_start();
	static			void			set_cur_as_start();
	static	FINLINE	o_str CONST &	get_start()			{ return dir_start;				}

	static			void			set_cur_as_kernel();
	static			o_str CONST &	get_kernel()		{ return dir_kernel;			}
//	static	FINLINE	o_str CONST &	get_kernel()		{ return get_def_from_index(0);	}

	static			void			set_cur_as_net();
	static	FINLINE	o_str CONST &	get_net()			{ return dir_net;				}

	static			void			set_cur_as_user();
	static	FINLINE	o_str CONST &	get_user()			{ return dir_user;				}
};

