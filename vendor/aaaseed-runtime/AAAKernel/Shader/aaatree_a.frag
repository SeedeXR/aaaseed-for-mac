//#version 330 compatibility

//	AAASeed uniform implicit
uniform	sampler2D	aaa_samp0	;
uniform	sampler2D	aaa_samp1	;
uniform	sampler2D	aaa_samp2	;
uniform	sampler2D	aaa_samp3	;
uniform	sampler2D	aaa_samp4	;
uniform	sampler2D	aaa_samp5	;
uniform	sampler2D	aaa_samp6	;
uniform	sampler2D	aaa_samp7	;
uniform	sampler2D	aaa_samp8	;
uniform	sampler2D	aaa_samp9	;
uniform	sampler2D	aaa_samp10	;
uniform	sampler2D	aaa_samp11	;
uniform	sampler2D	aaa_samp12	;
uniform	sampler2D	aaa_samp13	;
uniform	sampler2D	aaa_samp14	;
uniform	sampler2D	aaa_samp15	;


in VS_out
{
	vec2	uv;
	vec4	color;
} fs_in;

// --------------------------------------------------------------------------
// MAIN FUNCTION
vec4 get_color( vec2 uv, int sel )
{
	vec4 col;
	if( sel < 8 )
	{
		if( sel < 4 )
		{
			if( sel < 2 )	col = texture(	sel == 0  ? aaa_samp0  : aaa_samp1,		uv );
			else			col = texture(	sel == 2  ? aaa_samp2  : aaa_samp3,		uv );
		}
		else
		{
			if( sel < 6 )	col = texture(	sel == 4  ? aaa_samp4  : aaa_samp5,		uv );
			else			col = texture(	sel == 6  ? aaa_samp6  : aaa_samp7,		uv );
		}
	}
	else
	{
		if( sel < 12 )
		{
			if( sel < 10 )	col = texture(	sel == 8  ? aaa_samp8  : aaa_samp9,		uv );
			else			col = texture(	sel == 10 ? aaa_samp10 : aaa_samp11,	uv );
		}
		else
		{
			if( sel < 14 )	col = texture(	sel == 12 ? aaa_samp12 : aaa_samp13,	uv );
			else			col = texture(	sel == 14 ? aaa_samp14 : aaa_samp15,	uv );
		}
	}
	return col;
}

void main (void)
{
	vec2 uv = fs_in.uv;

/*
	vec4 col[15];
	col[0]		= texture(		aaa_samp1,		uv );
	col[1]		= texture(		aaa_samp2,		uv );
	col[2]		= texture(		aaa_samp3,		uv );
	col[3]		= texture(		aaa_samp4,		uv );
	col[4]		= texture(		aaa_samp5,		uv );
	col[5]		= texture(		aaa_samp6,		uv );
	col[6]		= texture(		aaa_samp7,		uv );
	col[7]		= texture(		aaa_samp8,		uv );
	col[8]		= texture(		aaa_samp9,		uv );
	col[9]		= texture(		aaa_samp10,		uv );
	col[10]		= texture(		aaa_samp11,		uv );
	col[11]		= texture(		aaa_samp12,		uv );
	col[12]		= texture(		aaa_samp13,		uv );
	col[13]		= texture(		aaa_samp14,		uv );
	col[14]		= texture(		aaa_samp15,		uv );
*/

	//		uv = floor( uv * vec2( 512., 256. ) ) * vec2( 1./512., 1./256. );
	vec4 mask	= texture(		aaa_samp0,		uv );

/*
	float v;
	if( .2 < mask.a && mask.a < .9	 )
	{
		v = clamp( 1.1-mask.a*2.2, 0.0, 1.0 );
		if( v < .5 )
			gl_FragColor.rgb = mix( cola.rgb, colb.rgb, v*2. );
		else
			gl_FragColor.rgb = mix( colb.rgb, colb.rgb, (v-.5)*2. );
			//gl_FragColor.rgb = vec3(v);
	}
	else
		gl_FragColor.rgb = cola.rgb;
*/
	//float val = fs_in.uv.s;
	int nb 		= aaa_fu_int[0];
	int sel		= aaa_fu_int[1];
	int see		= aaa_fu_int[2];
	float fac	= clamp( aaa_fu_float[0], 0., 1. );

	if( sel >= 5 )
	{
		if( sel == 5 || see == 15 )
			gl_FragColor = mask;
		else if( sel == 6 )
			gl_FragColor = get_color( uv, see );
		else
		{
			if( sel == 7 )
				sel = 2;
			else if( sel == 8 )
				sel = 3;
			else
				sel = 4;
			uv *= sel;
			see = int(floor(uv.g)) * sel;
			see += int(floor(uv.r));
			gl_FragColor = get_color( uv, see );
		}
	}
	else
	{
		float val;
		if( 1 <= sel && sel <= 3 )
		{
	//		vec2 dx = vec2( 1./512., 0. );
	//			vec2 dy = vec2( 0., 1./256. );

			val = mask.r;

	/*		val = max( val, texture(	aaa_samp0,	uv+dx ).r );
			val = max( val, texture(	aaa_samp0,	uv-dx ).r );
			val = max( val, texture(	aaa_samp0,	uv+dy ).r );
			val = max( val, texture(	aaa_samp0,	uv-dy ).r );

			val = max( val, texture(	aaa_samp0,	uv+dy*2 ).r );
			val = max( val, texture(	aaa_samp0,	uv-dy*2 ).r );
			val = max( val, texture(	aaa_samp0,	uv+dx*2 ).r );
			val = max( val, texture(	aaa_samp0,	uv-dx*2 ).r );

			val = max( val, texture(	aaa_samp0,	uv+dx+dy ).r );
			val = max( val, texture(	aaa_samp0,	uv+dx-dy ).r );
			val = max( val, texture(	aaa_samp0,	uv-dx+dy ).r );
			val = max( val, texture(	aaa_samp0,	uv-dx-dy ).r );
	*/
		}
		else
			val = 0.;

		if		( sel <= 1 )	val = fract( val + uv.s );
		else if	( 3 <= sel )	val = fract( val + uv.t );
	//	else if	( sel == 3 )	val = mod( uv.s * 5. + uv.t * 5., 1. );

		val = (val-.5) * (nb-fac) + .5 * nb;
		val -= fac * .5;
		//val = clamp( val, float(0), float(nb) );
	//	val *= aaa_fu_float[1];
	//	val = mod( val, nb );
		int ind = int( val );
		val -= ind;
		val = clamp( 1. - (1. - val) / fac, 0.0, 1.0 );

		gl_FragColor.rgb = mix( get_color(uv,ind+1).rgb, get_color(uv,ind+2).rgb, val );

	/*
		gl_FragColor.rgb = vec3(0);
		if( uv.t < .25 )
			gl_FragColor.r = float(ind)/(nb-1);
		else if( uv.t < .5 )
			gl_FragColor.g = val;
		else if( uv.t < .75 )
			gl_FragColor.b =  float( int(uv.s * nb) ) / nb;
		else
			gl_FragColor.rgb = mix( col[ind].rgb, col[clamp(ind+1,0,nb-1)].rgb, val );
		//gl_FragColor.rgb = 1-mask	.rgb;
	*/

	/*
		vec3 c = col[0].rgb;
		for( int i=1; i<8; ++i )
		{
			c += col[i].rgb;
		}
		for( int i=8; i<15; ++i )
		{
			c -= col[i].rgb;
		}
		gl_FragColor.rgb = c.rgb; // * (1./15.);
	*/

	//	gl_FragColor.r = 1.;
	//	gl_FragColor.g = 1.;
	//	gl_FragColor.b = 1.;
		gl_FragColor.a = 1.;
	}

	gl_FragColor *= fs_in.color;
//	gl_FragColor = vec4( 1 );
}
