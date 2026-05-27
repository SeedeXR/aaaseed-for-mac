/* By Palliaci
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 */

#define I_MAX	50
#define FAR		5.
#define E		.01
#define LIGHTS
#define REFL_I	.5
//#define MARCH		// Disable if you wanna see only the noise

mat3	rotX( float a ) {
    float c = cos( a );
    float s = sin( a );
    return mat3( 1, 0, 0, 0, c, -s, 0, s, c );
}
mat3	rotY( float a ) {
    float c = cos( a );
    float s = sin( a );
    return mat3( c, 0, s, 0, 1, 0, -s, 0, c );
}
mat3	rotZ(float a) {
	float c = cos( a );
	float s = sin( a );
	return mat3( c, s, 0, -s, c, 0, 0, 0, 1.0 );
}

// Sphere uv mapping from aiekick : https://www.shadertoy.com/view/MtS3DD
vec2	sphere_map(vec3 p)
{
    vec2 uv;
    uv.x = 0.5 + atan(p.z, p.x) / (2.*3.14159);
    uv.y = 0.5 - asin(p.y) / 3.14159;
	return uv;
}

float	map(vec3 p) { return length(p) -.6; }

float	march(vec3 pos, vec3 dir)
{
    float d = 0.;
    float prec = E;
    for (int i = -1; i < I_MAX; i++)
    {
        prec = map(pos + dir * d);
        d += prec;
        if (d < E || d > FAR)
            break;
    }
    return d;
}

vec3	camera(vec2 uv) {
    float fov = 1.;
    vec3 forw = vec3( 0.0, 0.0, 1.0 );
    vec3 right = vec3( 1.0, 0.0, 0.0) ;
    vec3 up = vec3( 0.0, 1.0, 0.0) ;
    return ( normalize((uv.x-1.)*right + (uv.y-.5)*up + fov*forw) );
}


void main( void )
{
	vec2 uv = vs_out.tex_coor[0].st;
#ifdef MARCH
    uv.x *= iResolution.x / iResolution.y;
    vec2 mickey = ((iMouse.xy) / iResolution.xy )*3.14;
    mickey.x *= iResolution.x / iResolution.y;
    vec3 pos = vec3(.2, .0, -.5*(mickey.x+mickey.y))*rotX(mickey.y+.15)*rotY(mickey.x-.12);
    vec3 dir = camera(uv.xy)*rotX(mickey.y+.15)*rotY(mickey.x-.12);
    vec3 col = vec3(0.);

	float d = march(pos, dir);
    if (d < FAR)
    {
		vec3 p = pos + dir * d;
        vec2 sp_uv = sphere_map(normalize(p));
        col += texture(iChannel0, sp_uv).xyz;
# ifdef LIGHTS
        vec2 e = vec2(-1., 1.)*0.005;
        vec3 n = normalize(e.yxx*map(p + e.yxx) + e.xxy*map(p + e.xxy) + e.xyx*map(p + e.xyx) + e.yyy*map(p + e.yyy) );
		vec3 ev = normalize( p - pos );
		vec3 ref_ev = reflect( ev, n );
        vec3 refl_cube = texture(iChannel1, ref_ev).rgb * REFL_I;
       // col *= clamp(refl_cube, .15, 1.);
        vec3 light_pos = pos + vec3(sin(iTime), 0., -.1);

        vec3 vl = normalize( light_pos - p );
		float diffuse  = max( 0.001, dot( vl, n ) );
		float specular = pow( max( 0.001, dot( vl, ref_ev ) ), 1. );
        float	brdf = (diffuse + specular) * .5 + .5;
        col *= brdf;
# endif
    }
    else
        col = texture(iChannel1, dir).rgb*vec3(.2, .3, .5);
    col = pow(col, vec3(.75));
    fragColor = vec4(col,1.0);
#else
    vec3 col = texture(iChannel0, uv).xyz;// * texture(iChannel2, uv).xyz;
	fragColor = vec4(col, 1.0);
#endif	//
}