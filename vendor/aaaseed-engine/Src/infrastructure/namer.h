
#ifdef AAA_NAMER_H
#error "NAMER_H included more than once."
#endif
#define AAA_NAMER_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class c_obj_ui;

class	c_namer final
{
private:
	o_str	_dir;
	o_str	_file;
	bool	_b_file_forced;	//todo always false check what going on
	bool	_b_start;

	static	c_namer*		get_namer_always(		c_obj_ui* CONST obj	);
public:
	static	c_namer*		build_dir( o_str& dst,	c_obj_ui* CONST obj	);
	static	void			set_dir_and_file(		c_obj_ui* CONST obj,	C_PCHAR_C dir,		C_PCHAR_C file		);
	static	void			set_dir_and_file(		c_obj_ui* CONST obj,	o_str CONST & dir,	o_str CONST & file	);
	static	void			set_dir_and_file(		c_obj_ui* CONST obj,	o_str CONST & str	);
	static	void			set_file_only(			c_obj_ui* CONST obj,	o_str CONST & str	);
	static	void			set_file_forced(		c_obj_ui* CONST obj,	o_str CONST & str	);
	static	void			set_dir_only(			c_obj_ui* CONST obj,	o_str CONST & str	);
	static	void			set_namer_start(		c_obj_ui* CONST obj	);
	static	void			build_name( o_str& dst, c_obj_ui&		obj	);
	static	void			check_one(				c_obj_ui* CONST obj	);
	static	void			check_all();
public:
	c_namer();
	~c_namer();

	FINLINE	void			set_start()		{ _b_start = true; }
	FINLINE	bool			is_start()		{ return _b_start; }

	FINLINE	o_str CONST &	get_dir()		{ return _dir;	}
	FINLINE	o_str CONST &	get_file()		{ return _file;	}
};

