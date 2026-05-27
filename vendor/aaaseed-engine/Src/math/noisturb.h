
#ifdef AAA_NOISTURB_H
#error "NOISTURB_H included more than once."
#endif
#define AAA_NOISTURB_H 1


#ifndef AAA_AAA_TYPE_H
#		include "aaa_type.h"
#endif

extern	REAL	noise1(					REAL CONST arg );
extern	REAL	noise2(					REAL CONST * CONST vec );	//vec is a vec[2]
extern	REAL	noise3(					REAL CONST * CONST vec );	//vec is a vec[3]

extern	REAL	noise3_by_perlin(		REAL CONST * CONST vec );
extern	REAL	noise_improved(			REAL CONST x, REAL CONST y, REAL CONST z );
FINLINE	REAL	noise_improved3(		REAL CONST * CONST pos_in	)	{	return	noise_improved( pos_in[0], pos_in[1], pos_in[2] );	}
extern	REAL	turbulence_by_perlin(	REAL CONST * CONST pos_in	);

extern	REAL	turbulence(				REAL CONST * CONST pos_in, INT32 harm_nb );
extern	REAL	fractalsum(				REAL CONST * CONST pos_in, INT32 harm_nb );

extern	REAL	turbulence_improved(	REAL CONST * CONST pos_in, INT32 harm_nb );
extern	REAL	fractalsum_improved(	REAL CONST * CONST pos_in, INT32 harm_nb );

extern	REAL	turbulence_wavelet(		REAL CONST * CONST pos_in, INT32 harm_nb );
extern	REAL	fractalsum_wavelet(		REAL CONST * CONST pos_in, INT32 harm_nb );

namespace aaa {
namespace noise {
	extern void	init();
	extern void	deinit();
}	//namespace noise
}	//namespace aaa
