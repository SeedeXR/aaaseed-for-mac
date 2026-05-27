#include "param_declare.h"
#include "err.h"

//todo move away
//C_PCHAR_C	g_str_bind[1] = { "bind" };
//C_PCHAR_C	g_str_same[1] = { "same" };

//todo	add 64 bits types

namespace{
	UINT32	nb = 0;
	UINT32	nb_created = 0;
	void inc_count()
	{
		++nb;
		++nb_created;
	}
}

UINT32*	c_param_def::get_nb_pt()			{ return &nb; }
UINT32*	c_param_def::get_nb_created_pt()	{ return &nb_created; }


void	c_param_def::set_all(	UINT32 CONST type, C_PCHAR_C name,	
								REAL CONST def, REAL CONST ina, REAL CONST min, REAL CONST max,
								aaa::param::fn_update * CONST fn, C_PCHAR_C * CONST  symbolic_str )
{
	_type_and_flag = type;
	_name.set(name);
	_def = def;
	_ina = ina;
	_min = min;
	_max = max;
	_fn_update = fn;
	_symbolic_str = symbolic_str;
}

c_param_def::c_param_def(		UINT32 CONST type, C_PCHAR_C name,	
								REAL CONST def, REAL CONST ina, REAL CONST min, REAL CONST max,
								aaa::param::fn_update * CONST fn, C_PCHAR_C * CONST  symbolic_str )
{
	set_all( type, name, def, ina, min, max, fn, symbolic_str );
	inc_count();
}

c_param_def::~c_param_def()
{
	if( nb > 0 )
		--nb;
	else
		debug_break( "%s() destructor called with param_def count already at zero", __FUNCTION__ );
}


