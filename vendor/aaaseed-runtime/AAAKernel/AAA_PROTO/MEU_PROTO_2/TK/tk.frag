//#version 330 compatibility
////#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable

//layout(location = 0) in vec2 in_texcoord;

layout(binding = 0) uniform sampler2D TEX;

in ST_AAA_BV BV;

#define F1 aaa_fu_float[0]
#define F2 aaa_fu_float[1]
#define F3 aaa_fu_float[2]
#define F4 aaa_fu_float[3]

#define B_TEST (aaa_fu_int[0]!=0)

void main (void)
{
    vec2 uv = gl_TexCoord[0].st;
    if( B_TEST )
    {
        gl_FragColor = vec4( uv,0, 1 ); 
        // switch( aaa_tex_dim[0] )-
        // {
        // case 0: gl_FragColor = vec4( 1,0,0, 1 );    break;
        // case 1: gl_FragColor = vec4( 0,1,0, 1 );    break;
        // case 2: gl_FragColor = vec4( 0,0,1, 1 );    break;
        // case 3: gl_FragColor = vec4( 1,1,1, 1 );    break;
        // default:  gl_FragColor = vec4( 1,1,0, 1 );    break;
        // }
        return;
    }
    
    float g = uv.y;
    g = smoothstep( -0.00001, F1, g ) * (1. - smoothstep( F2, 1., g ) );    //we have to do it this way for F2 == 1.
//	gl_FragColor.xyz *= vec3( 1, 1, 1);
    vec4 color = vec4( 1,1,1, g ) * BV.color;
    if( aaa_tex_dim[0]== 2 )
    {
        color *= texture( TEX, uv );
    }
  //  color.y += BV.pos_world.y*5.;
 //   color.a += (BV.pos_world.y-.1)*10.;
    //color.a = floor( color.a * 16. ) / 16.;
    gl_FragColor = color;
    //gl_FragColor.a = g;
	//gl_FragColor = vec4( 1,0,0, 1 );
	//gl_FragColor = vec4( 1. );
	//gl_FragColor = vec4( .4 );
	//gl_FragColor = vec4( 1., 1., 0., 1 );
}
