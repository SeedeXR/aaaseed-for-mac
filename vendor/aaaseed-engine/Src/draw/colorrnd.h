
#ifdef AAA_COLORRND_H
#error "COLORRND_H included more than once."
#endif
#define AAA_COLORRND_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif

class c_color_random
{
public:
	typedef	void	(*RANDOM_COLOR_FN)( INT32 index );
private:
	static	RANDOM_COLOR_FN	random_color_fn;

public:
	static	INT32 CONST		TYPE_NB	= 7;
	static	C_PCHAR_C		str_type_name[TYPE_NB+1];

	static			AAA_ERR	init();
	static			void	begin();
	static			void	end();
	static	FINLINE	void	set_and_inc( INT32 inc );
	static	FINLINE	void	set( INT32 index )			{	random_color_fn( index );	}
	static			void	set();
};

class c_color_drawer
{
private:
			INT32	_cur_index		{};
			INT32	_nb;
			bool	_b_fix			{ false	};
			FP32*	_color_loop;
			FP32	_color_fix[4];

public:
						c_color_drawer( INT32 nb = 6 );
	virtual				~c_color_drawer();

			void		set_color_fix( FP32 CONST * color );

	FINLINE	FP32*	get_color( INT32 ind )	{ return _b_fix ? _color_fix : &_color_loop[ IMOD( ind, _nb) * 4 ];	}
	FINLINE	FP32*	get_color_cur()			{ return _b_fix ? _color_fix : &_color_loop[_cur_index*4];	}

			void		draw_color_cur();
			void		draw_color( INT32 ind );
			void		draw_color( INT32 ind, FP32 alpha );
	
	FINLINE	void		begin()			{ _cur_index = 0; }
	FINLINE	void		next()			{ _cur_index = IMOD( _cur_index+1, _nb); }
};


