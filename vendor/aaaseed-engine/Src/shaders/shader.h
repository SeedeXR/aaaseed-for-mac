
#ifdef AAA_SHADER_H
#error "SHADER_H included more than once."
#endif
#define AAA_SHADER_H 1

#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class c_shader
{
public:
	enum	SHADER_TYPE : INT32
	{
		SHADER_FRAGMENT = 0,
		SHADER_VERTEX,
		SHADER_GEOMETRY,
		SHADER_COMPUTE,
	};
protected:
	bool			_b_loaded;
	bool			_b_valid;
	SHADER_TYPE		_s_shader_type;

	o_str			_fpath;
private:

public:
	c_shader( SHADER_TYPE shader_type );

	//	operator	GLuint () CONST;
	FINLINE	o_str CONST &		get_shader_fpath()	CONST	{ return _fpath; }
			void				set_shader_fpath( C_PCHAR_C fpath );

	FINLINE	bool				is_loaded()			CONST	{ return _b_loaded; }
	FINLINE	bool				is_valid()			CONST	{ return _b_valid; }

			void				reload();

	virtual	void				load() = 0;
	virtual	void				dealloc() = 0;

};

