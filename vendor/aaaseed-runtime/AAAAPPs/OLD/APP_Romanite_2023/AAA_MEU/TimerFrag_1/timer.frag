
in ST_AAA_BV BV;

uniform sampler2D   aaa_tex2d[4];

CONST float angle   = aaa_fu_float[0] * 1.05;

layout(binding = 0) uniform sampler2D TEX_A;

void main()
{
     vec2 uv = BV.tex_coor[0].st;

	float alpha = texture2D( TEX_A, uv ).r;

//	if( color.a<=0.005 )
//		discard;

     uv -= .5;
     float a = atan( uv.x, uv.y ) /PI2 + .5; // return in -PI,PI interval 
     alpha *= 1.-smoothstep( angle-.05, angle, a );
     if( alpha < .005 )
           discard;
     float delta = (1. - smoothstep( 1., 1.03, angle )) * .01;
     alpha *= smoothstep( .0, clamp_01(delta), a );
     if( alpha < .005 )
           discard;

	//gl_FragColor = color;
     gl_FragColor = vec4( vec3(1), alpha * BV.color.a);
 //    gl_FragColor = vec4( vec3(a), 1 );
}         