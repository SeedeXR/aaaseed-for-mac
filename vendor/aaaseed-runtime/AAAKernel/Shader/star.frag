
#define	TEX_COOR_DIM	4
#if (TEX_COOR_DIM == 2)
	#define TEX_COOR_VEC	vec2
#else
	#define TEX_COOR_VEC	vec4
#endif
#define	TEX_UNIT_NB		4

in VS_out
{
	vec4			pos_world;
	vec4			pos_ec;
	vec3			nor_ec;
	float			alpha;
	vec4			color;
	TEX_COOR_VEC	tex_coor[TEX_UNIT_NB];
} fs_in;


//	output
//out vec4	gl_FragColor;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];


#define	BRANCH_NB 5.
void main (void)
{
	vec4 texture;

	vec2 uv = fs_in.tex_coor[0].st;
	uv = (uv - .5) * 2.;
	//texture.rg = uv;
	//texture.ba = vec2(0,1);

	float l = length( uv );
	float angle = atan( uv.y, uv.x );
	angle = angle / PI2 + .5;
	angle = abs(fract(angle*BRANCH_NB)-.5)*2.;
	angle *= PI/BRANCH_NB;
	//texture = vec4( angle, angle, angle, 1 );

//	texture = vec4( l*cos(angle), l*sin(angle), 0, 1 );
	float b = aaa_fu_float[2];
	float a = aaa_fu_float[3]*b;

	float f = -a*l*cos(angle) - b*l*sin(angle) + a*b;
	f /= sqrt( a*a + b*b );
//	f = 1-f;

//	f = mix( f, 1.-l, aaa_fu_float[4] * clamp( 1.-abs( -fs_in.pos_ec.z - 3.33 ),0,1) );
	f = mix( f, 1.-l, aaa_fu_float[4] );
//	f *= 1 - clamp( abs( -fs_in.pos_ec.z - 3.33 ), 0.0, 1.0 );
	//if( f>=1. )
	//	discard;
	f = smoothstep( aaa_fu_float[1]*aaa_fu_float[0], aaa_fu_float[0], 1-f );

	texture = mix( aaa_fu_vec4[0], aaa_fu_vec4[1], f );
//	texture.g = abs( -fs_in.pos_ec.z - 3.33 ) * 2.;

//	texture = vec4( l, l, l, 1 );
//	texture = vec4( f, f, .1, 1 );

	#if 0
		texture = blend_fs( texture );
	#else
	//	texture *= fs_in.color;
	#endif
	gl_FragColor = texture;
//	gl_FragColor.a = 1;
//	gl_FragColor = vec4(0,1,0,.5);

}
