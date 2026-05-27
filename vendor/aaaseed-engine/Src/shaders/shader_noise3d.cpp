#include "shader_noise3d.h"
#include "err.h"
#include "math/noisturb.h"
#include "gol/gol.h"
#include "draw/tex.h"


static	CONST	INT32	FREQ_START	= 8;
static	CONST	INT32	HARM_NB 	= 4;

void	build_texture_noise_3d( UINT8* pt_in, INT32 tex_size, INT32 index)
{
	INT32	f, inc;
	REAL	ni[3];
	REAL	finc;
	REAL	frequency = FREQ_START;
	double	amp = .5 * 128.;
	UINT8*	pt;

	GOOD_PRINT_STRING_NO_CR( "building 3D noise texture" );
	for( f = 0, inc = 0; f < HARM_NB; ++f, frequency *= 2, ++inc, amp *= 0.5)
	{
		pt = pt_in;
		if( index == 2) 
			pt += IMOD( inc+index-2, 4);
		else
		{
			pt += inc;
			amp *= 2.;
		}
		//SetNoiseFrequency(frequency);
//		ni[0] = ni[1] = ni[2] = 0.;
		finc = frequency / tex_size;
		ni[0] = 0.;	
		for( INT32 i = tex_size; i > 0; --i, ni[0] += finc )
		{
			ni[1] = 0.;
			for( INT32 j = tex_size; j > 0; --j, ni[1] += finc)
			{
				ni[2] = 0.;
				for( INT32 k = tex_size; k > 0; --k, ni[2] += finc)
				{
					if( index == 0)
						*pt = (GLubyte) ((noise_improved3(ni) + 1.0) * amp);	//	noise
					else if( index == 1)
						*pt = (GLubyte) ((noise3(ni) + 1.0) * amp);	//	noise
					else
					{
						//	test color cube
						if( inc==0 )
							*pt = UINT8(float(i*256)/float(tex_size));
						else if( inc==1 )
							*pt = UINT8(float(j*256)/float(tex_size));
						else if( inc==2 )
							*pt = UINT8(float(k*256)/float(tex_size));
						else
							*pt = (inc==3)?255:0;
					}
					pt += 4;
				}
			}
		}
	}
	PRINT_STRING( "\n");
}

CONST	INT32	tex_size = 128;

void	init_texture_3d_one( UINT8* pt, INT32 index )
{
	build_texture_noise_3d( pt, tex_size, index );

	tex3d.bind( index );

	GOL::set_tex_3d_wrap_s( GL_REPEAT );	//todo tex vs texture
	GOL::set_tex_3d_wrap_t( GL_REPEAT );
	GOL::set_tex_3d_wrap_r( GL_REPEAT );

	GOL::set_tex_3d_min_mag( GL_LINEAR, GL_LINEAR );

	GOL::tex_image_3d( 0, GL_RGBA,
					tex_size, tex_size, tex_size, 
					GL_RGBA, GL_UNSIGNED_BYTE, pt );
}

void	init_texture_3d()
{
	UINT8* pt = (UINT8*) MALLOC_ALIGNED( tex_size*tex_size*tex_size * HARM_NB, 0 );
	if( pt )
	{
		init_texture_3d_one( pt, 0 );
		init_texture_3d_one( pt, 1 );
		init_texture_3d_one( pt, 2 );		
		FREE( pt );
	}
}

