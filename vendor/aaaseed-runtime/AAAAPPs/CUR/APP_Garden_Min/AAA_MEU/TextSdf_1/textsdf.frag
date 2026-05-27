//#version 330 compatibility
//#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable


in ST_AAA_BV BV;

//uniform vec4        aaa_fu_vec4[]; // color de 0 a 3, active ds 4.xyzw et 5.xy
//uniform sampler2D   aaa_tex2d[4];
uniform sampler2D	aaa_samp0;
#define TEX_SDF	aaa_samp0
uniform sampler2D	aaa_samp1;
#define TEX_COLOR	aaa_samp1

//
vec4    col_texture     =   aaa_fu_vec4[0];
vec4    col_ao          =   aaa_fu_vec4[1];
//vec4    col_flare       =   aaa_fu_vec4[2];
//vec4    col_shadow      =   aaa_fu_vec4[3];

int		render_type		=	aaa_fu_int[0];


vec4    gcol;
vec3    nor;


float median( float r, float g, float b )
{
    return max( min(r,g), min( max(r,g), b ) );
}

vec4 get_sdf()
{
	return texture2D( TEX_SDF, BV.tex_coor[0].xy );
}

vec2 get_grad()
{
	vec4 col = get_sdf();
	float gray = compute_gray( col.xyz );
    //grad.xy = laplacian1(frag_coord);
	return vec2( dFdx(gray), dFdy(gray) );
}

void main()
{
//    vec3    grad;
///   float   gray;
//    float   g;
    vec2    frag_coord  = BV.tex_coor[0].xy;
//   vec2    ts          = textureSize( TEX_SDF, 0 );
    //texel = 1./ts;

#if 0
        if( render_type != 4 )
        {
            gcol = texture2D( TEX_SDF, frag_coord );
            //gcol.xyz = blur_gauss_3( TEX_SDF, frag_coord );
            //gcol.xyz = blur_gaussian( TEX_SDF, frag_coord );

            gray = compute_gray( gcol.xyz );
            //grad.xy = laplacian1(frag_coord);
            grad.xy = vec2( dFdx(gray), dFdy(gray) );
            nor.z = 1;
            nor = normalize( nor );
        }
#endif
	switch( render_type )
	{
	case 0:
		gcol = get_sdf();
		break;
	case 1: //Maa
		{
			gcol = get_sdf();
   			float sd = median( gcol.r, gcol.g, gcol.b );
			gcol = BV.color;
			gcol.a *= smoothstep( .5, .6, sd );
		}
		break;
	case 2: //MaaBis
		{
			gcol = get_sdf();
   			float sd = median( gcol.r, gcol.g, gcol.b );
			//sd = abs( sd );
//    float screenPxDistance = screenPxRange()*(sd - 0.5);
 //   float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
  //  color = mix(bgColor, fgColor, opacity);

			gcol = BV.color;
		//gcol.rgb = texture2D( TEX_COLOR, BV.pos_world.xy ).rgb;
			gcol.a *= smoothstep( .5, .6, sd );
		}
		break;	
    case 3:
		{
            vec2 grad = get_grad();
    		gcol = vec4( grad*5.+ .5   , 0, 1 );
		}
		break;
    case 4:
		{
			vec2 grad = get_grad();
        	float g = pow( dot( grad.xy, grad.xy ), .5 ) * 50.;
        	gcol = vec4( g, g, g, 1 );
		}
		break;
    case 5:
		{
			vec3 grad;
			grad.xy = get_grad();
        	grad.z = .01;
        	grad = normalize( grad ) *.5 + .5;
        	gcol = vec4( grad, 1 );
		}
		break;
    }

	if( gcol.a <= 0. )
		discard;

    //Apply slider overlay
    gl_FragColor = gcol;
//	gl_FragColor = vec4(1);
//	gl_FragColor.rg = frag_coord.xy;
//	vec2 uv =  BV.pos_world.xy + .5;
//	gl_FragColor.rgb = texture2D( TEX_COLOR, BV.tex_coor[0].xy ).rgb;
//	gl_FragColor.rg = BV.tex_coor[0].xy;

}
