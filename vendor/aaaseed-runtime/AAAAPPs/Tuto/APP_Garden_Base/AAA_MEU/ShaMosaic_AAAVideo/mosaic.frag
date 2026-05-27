
in ST_AAA_BV BV;

uniform sampler2D   aaa_tex2d[4];


CONST vec2 influence	= aaa_fu_vec4[0].xy;
CONST vec2 scale		= aaa_fu_vec4[0].zw;
CONST vec2 scale_over	= 1. / scale;
CONST vec4 step			= aaa_fu_vec4[1];
CONST vec2 gain_factor	= aaa_fu_vec4[2].xy;
CONST vec2 bias_factor	= aaa_fu_vec4[2].zw;
CONST vec2 factor		= aaa_fu_vec4[3].xy;

CONST float angle		= aaa_fu_float[8] * PI2;

vec2  smoothstep_v2( vec2  edge0, vec2  edge1, vec2  v_in )
{
	vec2 v2;
	v2.x = smoothstep_safe( edge0.x, edge1.x, v_in.x );
	v2.y = smoothstep_safe( edge0.y, edge1.y, v_in.y );
	return v2;
}

void main()
{
	vec2 uv = BV.tex_coor[0].st;
	vec2 duv;

	

//	if( 1 < aaa_fu_int[0] )
//	{
	 // rotate
		float si = sin( angle );
		float co = cos( angle );
		uv = mat2( co, -si, si, co ) * (uv - .5 ) + .5;

		if( aaa_fu_int[0] <= 5 )
		{
			duv = fract( scale * uv );

			vec2 gb;
			gb = gain( duv, gain_factor ); 
			gb = bias( gb, bias_factor );

			vec2 def;
			switch( aaa_fu_int[0] )
			{
			case 1:	def = duv;	break;
			case 2:	def = gb;
					def -= duv;
					def *= influence;
					def += duv;
					break;
			case 3: def = sin( -PI2*gb ) / PI2;
					def *= influence;
					def += duv;
					break;
			case 4: def = smoothstep_v2( step.xy, step.zw, duv );
					def = gain( def, gain_factor ); 
					def = bias( def, bias_factor );
					def -= duv;
					def *= influence;
					def += duv;
					break;
			case 5: def = sin( -PI2*duv );
					def = (def + 1.) * .5;
					def = gain( def, gain_factor ); 
					def = bias( def, bias_factor );
					def = def * 2. - 1.;
					def *= influence / PI2;
					def += duv;
					break;
			}
			def = ((def-0.5) * factor) + .5;
			uv += (def-duv) * scale_over;
		

			//uv = def;
			//def = def * factor;
			//uv += def * scale_over;
		}
		else
		{
			if( aaa_fu_int[0]==9 )
			{
				duv = fract( scale * uv );
				vec2 def;
				// def.x = linearstep( step.x, step.z, duv.x);
				// def.y = linearstep( step.y, step.w, duv.y);
				def = smoothstep_v2( step.xy, step.zw, duv );
				def = mix( duv, def, influence );
				def = gain( def, gain_factor ); 
				def = bias( def, bias_factor ); 
				def = (def-.5) * factor + .5;
				uv +=  (def - duv) / scale;
			}
			else if( aaa_fu_int[0]==7 )
			{
				//uv *= factor;

#if 0
				duv = sin( PI * scale * uv );
				duv = pow( abs(duv), vec2(4.) );
#else
				//duv = sin( PI * scale * uv ) *.5 + .5;
				duv = gain( duv, gain_factor ); 
				duv = bias( duv, bias_factor );
				//duv = duv * 2. - 1.;
#endif
				duv *= factor / scale;
				//uv = clamp_01( uv );
				// uv = clamp( uv, .000, 1. );
				uv -= duv;
			}
			else if( aaa_fu_int[0]==8 )
			{
				duv = fract( scale * uv );
				//uv -= duv / scale;
				vec2 def;
				// def.x = linearstep( step.x, step.z, duv.x);
				// def.y = linearstep( step.y, step.w, duv.y);
				def = gain( duv, gain_factor ); 
				def = bias( def, bias_factor );
				def = sin( PI2 * def ) * .1285;
				def = def * factor / scale;
				uv += def;
			}
		}

		// rotate back
		uv = mat2( co,si, -si,co ) * (uv - .5 ) + .5;
	//}

	vec4 col = texture2D( aaa_tex2d[0], uv );
	//col.rgba = vec4( uv, 1, 1 );


	//Apply slider overlay
	gl_FragColor = col * BV.color; 
}