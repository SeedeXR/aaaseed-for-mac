
//uniform sampler2D	aaa_tex2d[4];
//uniform float		aaa_fu_float[6];
//	2	:	offset for z
//	3	:	scale for z

void main (void)
{
	//gl_TexCoord[0].s *= 2;
	//gl_TexCoord[0].t *= 2;

	vec4 tex;
	float z = gl_FragCoord.z;
	if ( z > .999 )
	{
		tex = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
		tex.xyz *= (1. - z) * 1000.	;
		gl_FragColor = blend( tex );
	}
	else
	{
		float z = aaa_fu_float[0] + aaa_fu_float[1] * gl_FragCoord.z;
		if( gl_FrontFacing )
		{
			vec4 tex;
			if ( z < aaa_fu_float[2] )
				discard;
			if ( z < aaa_fu_float[3] )
			{
				z = (z - aaa_fu_float[2]) / (aaa_fu_float[3]-aaa_fu_float[2]) ;
				z = 1. - z;
				z = z * 32.;
				if ( mod( gl_FragCoord.y, 32.0 ) < z )
					discard;
				if ( mod( gl_FragCoord.x, 32.0 ) < z )
					discard;
			}
			tex = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
			/*
			tex.r = 1.;
			tex.g = 1.;
			tex.b = 1.;
			tex.a = 1.;
			*/
			gl_FragColor = blend( tex );
		}
		else
		{
			if ( mod( gl_TexCoord[0].t*200. + 4., 16.0 ) > 1. && mod( gl_TexCoord[0].s*200. + 4., 16.0 ) > 1. )
			//if ( mod( gl_FragCoord.y + 4., 16.0 ) > 1. && mod( gl_FragCoord.x + 4., 16.0 ) > 1. )
				discard;
			tex = gl_Color  ;
			tex = gl_FrontMaterial.diffuse;
			/*
			tex.r = 0.;
			tex.g = 1.;
			tex.b = 1.;
			tex.a = .5;
			*/
			gl_FragColor = tex ;
		}
 	//vec4 texture = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );

	//texture.g = 1.-texture.g;
	}

	//gl_FragColor = texture;
}

	//dst.r = mod( dFdx(gl_FragCoord.x*aaa_fu_float[4]), 1.0 );
	//dst.g = mod( dFdy(gl_FragCoord.y*aaa_fu_float[5]), 1.0 );