
in VS_out
{
    vec4            pos_world;
    vec4            pos_ec;
    vec3            nor_ec;
    float           alpha;
    vec4            color;
//    TEX_COOR_VEC    tex_coor[TEX_UNIT_NB];
    vec4            tex_coor[4];
//  vec4            color;
//  vec3            normal;
//  float           fall_off;
//  float           z_to_eye;
} vs_out;


uniform sampler2D   aaa_tex2d[4];

float my_random2( vec2 val )
{
    //float tmp = dot( val, vec2( 12.9898, 78.233 ) );
    //float ret = sin( tmp );
    //return fract( ret * 43758.5453123 );
    float tmp = dot( val, vec2( 1, 2 ) );
    float ret = sin( tmp );
    return fract( ret * 3 );
}

float my_noise2d( vec2 val )
{
    float noise = my_random2( val.xy );
    return noise;
}

void main()
{
    vec2 uv = vs_out.tex_coor[0].xy;

    vec3 col = vec3( my_noise2d( uv.xy * 1. ) ); //, aaa_fu_float[5] ) ) );
    //Apply slider overlay
    //gl_FragColor = vec4(aaa_fu_float[6]*col.xyz + aaa_fu_vec4[3].xyz, 1.);
    gl_FragColor = vec4( col.xyz, 1.);
}
