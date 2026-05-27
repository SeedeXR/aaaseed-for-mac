


// --------------------------------------------------------------------------
// IN, OUT, INITIALIZATION
#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
in VS_out
{
	TEX_COOR_VEC	tex_impli;
	vec4			color;
	vec4 			vertex;
	vec3			normal;
	vec4			tex_coord;
	float			fall_off;
} fs_in;

//	output data
out vec4	fragColor;
//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];



// --------------------------------------------------------------------------
// MAIN FUNCTION

void main (void)
{
	vec4	tex;

	vec2	coi = fs_in.tex_impli.st;

	tex = texture( aaa_tex2d[0], coi );

	//fragColor = vec4( tex.r * fs_in.fall_off, tex.g * fs_in.fall_off, tex.b * fs_in.fall_off, 1.0 );
	//fragColor = tex + tex2; //mix(tex, tex2, 1.0);
	//fragColor = tex;
	//fragColor.a = tex.r;
	//float r = 1.0 - fs_in.fall_off;

	// if(fs_in.fall_off < 0.00003 )
	// {
	// fragColor = vec4( .0, 0.0, 1.0, 1.0 );

	// }
//	else

	if (fs_in.fall_off < 0.00001 )
	{
//		fragColor = vec4( tex.r, tex.g, smoothstep( tex.b, 1.0, 1.0+(fs_in.fall_off)/(0.2)*(tex.b-1.0)), tex.a );
		fragColor = vec4( tex.r, tex.g, tex.b , tex.a );
		fragColor.a = tex.r;
		//fragColor = vec4( .0,1.0,.0,1.0);
	}
	else if (fs_in.fall_off < 0.2 )
	{
//		fragColor = vec4( tex.r, tex.g, smoothstep( tex.b, 1.0, 1.0+(fs_in.fall_off)/(0.2)*(tex.b-1.0)), tex.a );
	//	fragColor = vec4( 1.0, .0, .0 , tex.a );
	//	float blue = (fs_in.fall_off-0.005 )/(0.2-0.005)*( 1.0 - 1.0 ) - tex.b
	//	fragColor = vec4( tex.r, tex.g, smoothstep( tex.b, 1.0, 1.0+(fs_in.fall_off)/(0.2)*(tex.b-1.0)), tex.a );

		fragColor = mix( vec4(tex.r, tex.g, tex.b, tex.r), vec4( 0.,.0,1.0, 0.5 ), (fs_in.fall_off - 0.00001)/(0.2-0.00001)  );
	//	fragColor = vec4( 1.0,.0,.0,1.0);
	}
	else
	{
		fragColor = vec4( .0, .0, 1.0, 0.5 );
	}

}
