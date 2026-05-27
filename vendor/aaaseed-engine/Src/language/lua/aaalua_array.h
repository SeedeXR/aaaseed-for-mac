
#ifdef AAA_AAALUA_ARRAY_H
#error "AAALUA_ARRAY_H included more than once."
#endif
#define AAA_AAALUA_ARRAY_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

struct	lua_State;

namespace aaalua
{
	namespace n_array
	{
		extern	void		register_array(		lua_State* CONST L );

		extern	INT16*		get_ud_data_int16(	lua_State* CONST L, int CONST index, INT32& size	);
		extern	UINT16*		get_ud_data_uint16(	lua_State* CONST L, int CONST index, INT32& size	);
		extern	INT32*		get_ud_data_int32(	lua_State* CONST L, int CONST index, INT32& size	);
		extern	FP32*	get_ud_data_fp32(	lua_State* CONST L, int CONST index, INT32& size	);
		extern	FP32*	get_ud_data_fp32(	lua_State* CONST L, int CONST index					);
		extern	DOUBLE*		get_ud_data_fp64(	lua_State* CONST L, int CONST index, INT32& size	);
	}
}

