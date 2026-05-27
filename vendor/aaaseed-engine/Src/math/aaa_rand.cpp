#include <stdlib.h>
#include "math/aaa_rand.h"

REAL	gauss_fac;

REAL	rnd_maa(void)
{
	//	Cast both operands to REAL : on platforms where RAND_MAX is 2^31-1
	//	(macOS, Linux libc) the implicit int-to-float conversion of RAND_MAX
	//	itself rounds up to 2^31, distorting the divisor by 1 ULP. Casting
	//	RAND_MAX explicitly to REAL is behaviour-preserving on Windows
	//	(RAND_MAX = 2^15-1 fits exactly in float) and accurate on every libc.
	return( (REAL) rand() / (REAL) RAND_MAX - REAL(.5) );
}

void	rnd_init(void)
{
	gauss_fac = REAL( 1./ (RAND_MAX * 3.) );
}

REAL	rnd_gauss()
{
	//	Cast each rand() to REAL before summing : on platforms where RAND_MAX
	//	is 2^31-1 (macOS, Linux libc) three rands overflow signed int. MSVC
	//	RAND_MAX is 2^15-1 so the original "(rand()+rand()+rand())" form
	//	worked on Windows by accident. Casting first is behaviour-preserving
	//	on Windows and correct on every other libc.
	return( ( (REAL) rand() + (REAL) rand() + (REAL) rand() ) * gauss_fac - REAL(.5));
}

REAL	rnd_constant()
{
	return REAL(.1);
}

INT32	flip_cnt = 0;
REAL	rnd_flip()
{
	return (flip_cnt++&1)?-REAL(.1):REAL(.1);
}
