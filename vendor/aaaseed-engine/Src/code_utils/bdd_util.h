
#ifdef AAA_BDD_UTIL_H
#error "BDD_UTIL_H included more than once."
#endif
#define AAA_BDD_UTIL_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

void bdd_util_build_uv( REAL* uv, INT32 CONST nb_u, INT32 CONST nb_v,
			bool CONST b_u_loop = false, bool CONST b_v_loop = false,
			bool CONST b_flip_uv_in = false,
			REAL CONST factor_u = 1., REAL CONST factor_v = 1,
			REAL CONST offset_u = 0., REAL CONST offset_v = 0.);

