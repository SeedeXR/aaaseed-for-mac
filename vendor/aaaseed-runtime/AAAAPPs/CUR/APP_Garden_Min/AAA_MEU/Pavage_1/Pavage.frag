//#version 330 compatibility
////#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable


uniform	sampler2D	aaa_tex2d[4];

in VS_out
{
	vec4	pos_world;
	vec4	pos_ec;
	vec4	color;
	vec2	uv;
} vs_out;

float fold( float v )
{
	v = mod( v+1, 3. );
	//if( abs(v) > 3 )
	//	return 0;
	//else
	return v-1;
}
float fold2( float v )
{
	v = mod( v-1, 2. );
	//if( abs(v) > 3 )
	//	return 0;
	//else
		return v-1;
}
void main (void)
{
	int rot = 0;

	vec2 uv_in = vs_out.uv - .5;
	float dr = aaa_fu_vec4[0].z + aaa_fu_vec4[0].w * dot( aaa_fu_vec4[0].xy, uv_in );
	float r = fold( dr );

	float dg = aaa_fu_vec4[1].z + aaa_fu_vec4[1].w * dot( aaa_fu_vec4[1].xy, uv_in );
	float g = fold( dg );

	float db = aaa_fu_vec4[2].z + aaa_fu_vec4[2].w * dot( aaa_fu_vec4[2].xy, uv_in );
	float b = fold( db );

//if( r > 1 )
//	if( r>1 || g>1 || b>1 ) {	r-=2;	g+=1; b+=1; }
	if( r > 1 )
	{
		r-=2;	g=fold(g+1); b=fold(b+1);
		if( g > 1 )
		{
				{	g+=2;	b=fold(b+1); r+=1; }

		}
		else if( b > 1 )		{	b-=2;	r=r+1; g=g+1; }
//		else {  r=g=b=0.;	}
	}
	else
	{
		if( g > 1 )
		{
			if( b > 1 )
				{	g-=2;	b=fold(b+1); r=fold(r+1); }
		//	else {  r=g=b=0.;	}
			else if ( b < 0 )
				{	g-=2;	b=fold(b+1); r=fold(r+1); }
			//else					{ 		r=g=b=0.;		}
			else {  r=fold(r+1); g=fold(g+2); b=b+1; }
			//else					{ 		r=g=b=0.;		}
			if( r > 1 && b< 0 )
				{  r=fold(r-2); g=fold(g+1); b=fold(b+1); }
		}
		else if( b > 1 )
		{
				{	b+=2;	r=fold(r+1); g=g+1; }
				if( r > 1  )
					{  r=fold(r+1); g=fold(g+1); b=fold(b+3); }
		}
//				else if ( b > 0 )
//				r=g=b=0.;
//		else

	}


//	else					{ r=g=b=0.;		}

	r = fold2(r);
	g = fold2(g);
	b = fold2(b);

	
	vec4 col;
	if		( aaa_fu_int[0] == 24 )	{	g = min(min(abs(r),abs(g)),abs(b));	col = vec4( g, g, g, 1 );	}
	else if	( aaa_fu_int[0] == 25 )	{	g = min(min(r,g),b);	col = vec4( g, g, g, 1 );	}
	else if	( aaa_fu_int[0] == 26 )	{	g = max(max(abs(r),abs(g)),abs(b));	col = vec4( g, g, g, 1 );	}
	else if	( aaa_fu_int[0] == 27 )	{	col = vec4( r, g, b, 1 );		}
	else if	( aaa_fu_int[0] == 28 )	{	col = vec4( abs(r), abs(g), abs(b), 1 );		}
	else if	( aaa_fu_int[0] == 29 )	{	col = vec4( abs(r), 0, 0, 1 );		}
	else if	( aaa_fu_int[0] == 30 )	{	col = vec4( 0, abs(g), 0, 1 );		}
	else if	( aaa_fu_int[0] == 31 )	{	col = vec4( 0, 0, abs(b), 1 );		}
	else
	{
		vec2 uv;
		if		( aaa_fu_int[0] == 1 )	{	uv.x = r;	uv.y = g;	}
		else if	( aaa_fu_int[0] == 2 )	{	uv.x = g;	uv.y = b;	}
		else if	( aaa_fu_int[0] == 3 )	{	uv.x = b;	uv.y = r;	}
		else if	( aaa_fu_int[0] == 4 )
		{
			uv.x = min(min(r,g),b);
			uv.y = max(max(r,g),b);
		}
		else if	( aaa_fu_int[0] == 5 )
		{
			//r = dr;
			//g = dg;
			//b = db;
			uv.x = 0;
			uv.y = 0;
			float m = max(max(abs(r),abs(g)),abs(b));
			//r = abs(r);
			//g = abs(g);
			//b = abs(b);
			if( g > 0 )
			{
				if( b < 0 )	{ uv.y = abs(r); uv.x = m - abs(r)*.5; }
				else		{ uv.x = m - g*.5; uv.y = abs(g); }
			}
			else
			{
				if( r < 0 )	{ uv.x = m + g*.5; uv.y = abs(g); }
				else		{ uv.y = abs(b); uv.x = m - abs(b)*.5; }
			}

		}
	//	uv.x += uv.y *.5;
	//	uv = vec2(r,b);
		if( (aaa_fu_int[2] & 1) > 0 )	{	uv.x = 1.-uv.x;	}
		if( (aaa_fu_int[2] & 2) > 0 )	{	uv.y = 1.-uv.y;	}


		if( aaa_fu_int[3] > 0 )
		{
			//uv += .5;
			uv *= aaa_fu_vec4[3].xy;
			uv += aaa_fu_vec4[3].zw;
			col = texture2D(	aaa_tex2d[0],	uv );
		}
		else
			col = vec4( uv, 0, 1 );
	}
	gl_FragColor = vs_out.color * col;
//	gl_FragColor = vec4( r, g, b, 1 );
//	gl_FragColor = vec4( abs(r), abs(g), abs(b), 1 );

//	gl_FragColor = vec4( r, g, b, 1 );

	//gl_FragColor.xy = vs_out.uv;
	//gl_FragColor.r = vs_out.pos_world.y;
	//gl_FragColor = vec4( 1. );
	//gl_FragColor = vec4( .4 );
	//gl_FragColor = vec4( rot, 0, 0., 1 );
	//float s = max( r, g);
	//gl_FragColor.xyz = vec3(s);
	if( aaa_fu_int[1] == 1 )
	{
		r = mod( dr, 1. );
		g = mod( dg, 1. );
		b = mod( db, 1. );
		gl_FragColor.xyz += vec3(1.) * step( 1.-max(max(r,g),b), .01 );
	}


}
