
#ifdef AAA_AAA_RAND_H
#error "AAA_RAND_H included more than once."
#endif
#define AAA_AAA_RAND_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern	REAL	rnd_maa();
extern	void	rnd_init();
extern	REAL	rnd_gauss();
extern	REAL	rnd_constant();
extern	REAL	rnd_flip();


