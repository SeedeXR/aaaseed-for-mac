// Created by sebastien durand - 11/2016
//-------------------------------------------------------------------------------------
// Based on "Type 2 Supernova" by Duke (https://www.shadertoy.com/view/lsyXDK)
// Sliders from IcePrimitives by Bers (https://www.shadertoy.com/view/MscXzn)
// License: Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
//-------------------------------------------------------------------------------------

//#version 330 compatibility
//#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable

#define TEX_COOR_DIM    4

in VS_out
{
    vec4            pos_world;
    vec4            pos_ec;
    vec3            nor_ec;
    float           alpha;
    vec4            color;
    TEX_COOR_VEC    tex_coor[TEX_UNIT_NB];
//  vec4            color;
//  vec3            normal;
//  float           fall_off;
//  float           z_to_eye;
} vs_out;


uniform sampler2D   aaa_tex2d[4];

#define I_MAX   100
#define E       0.002
vec2    uv;

mat3    Rot3X( float a )
{
    float c = cos( a );
    float s = sin( a );
    return mat3( 1, 0, 0,
                 0, c,-s,
                 0, s, c );
}

mat3    Rot3Y( float a )
{
    float c = cos( a );
    float s = sin( a );
    return mat3( c, 0, s,
                 0, 1, 0,
                 -s, 0, c );
}

float   sdSphere( vec3 p, float r )
{
    return( length(p) - r );
}

float   scene( vec3 p )
{
    float   mind = 1e5;
    float   d = 0.0;

    d = sdSphere(( -0.9 + 0.1 * cos(aaa_fu_float[3]) ) * 0.9 * p + (0.35 * cos(aaa_fu_float[3]) )
                   * cos( 5.0 * p.x * sin(aaa_fu_float[3]) )
                   * sin( 5.0 * p.y * cos(aaa_fu_float[3]) )
                   * sin( 5.0 * p.z * -sin(aaa_fu_float[3]) ),
                   1.0);
    mind = min( d, mind );
    return( mind );
}

vec4    march( vec3 pos, vec3 dir )
{
    vec2    dist = vec2( 0.0 );
    vec3    p = vec3( 0.0 );
    vec4    step = vec4( 0.0 );
    for ( int i = -1; i < I_MAX; ++i )
    {
        p = pos + dir * dist.y;
        dist.x = scene( p );
        dist.y += dist.x;
        if ( dist.x < E || dist.y > 20. )
           break;
        step.x++;
    }
    step.y = dist.x;
    step.w = dist.y;
    return ( step );
}

vec3    calcNormal( in vec3 pos, float e, vec3 dir )
{
    vec3 eps = vec3( e, 0.0, 0.0 );
    return normalize(vec3(  march( pos + eps.xyy, dir ).w - march( pos - eps.xyy, dir ).w,
                            march( pos + eps.yxy, dir ).w - march( pos - eps.yxy, dir ).w,
                            march( pos + eps.yyx, dir ).w - march( pos - eps.yyx, dir ).w ) );
}

vec3    camera(vec2 uv)
{
    float   fov =        1.;
    vec3    forw  = vec3( 0.0, 0.0, 1.0 );
    vec3    right = vec3( 1.0, 0.0, 0.0) ;
    vec3    up    = vec3( 0.0, 1.0, 0.0) ;

    return ( normalize( ( uv.x - 1. ) * right
                        + ( uv.y - 0.5 ) * up
                        + fov * forw ) );
}

void main( )
{
   // uv  = fragCoord.xy / iResolution.xy;
   // uv.x *= iResolution.x / iResolution.y;
   // float   dirtRatio = iResolution.x / iResolution.y;
   // mat3    rotx = Rot3Y( -( iMouse.x / iResolution.x - .5 ) * dirtRatio );
   // mat3    roty = Rot3X( ( iMouse.y / iResolution.y - .5 ) * dirtRatio );
   // mat3    rot = roty * rotx;
   // vec3    dir = camera( uv ) * rot;
   // vec3    pos = vec3( .0, .0, -10.0 );
    vec4    col = vec4( 1.0 );

    vec2    frag_coord = vs_out.tex_coor[0].xy;
    vec3    pos = vec3( 0.0, 0.0, -10.0 );
    vec3    dir = normalize( vec3( (frag_coord-.5)*2., aaa_fu_float[5]) );

    vec4    inter = ( march( pos, dir ) );

    vec3    v = pos + inter.w * dir;
    if ( inter.w <= 17. )
    {
        vec3    n = calcNormal( pos, E, dir );
        vec3    ev = normalize( v - pos );
        vec3    ref_ev = reflect( ev, n );
        vec3    light_pos   = vec3( -20.0, 10.0, -25.0 );

        vec3    vl = normalize( light_pos - v );
        float   diffuse  = max( 0.0, dot( vl, n ) );
        float   specular = pow( max( 0.0, dot( vl, ref_ev ) ), 42. );
        col.xyz = col.xyz * ( diffuse + specular );
    }
    gl_FragColor = vec4(aaa_fu_float[6]*col.xyz + aaa_fu_vec4[3].xyz, 1.);
}