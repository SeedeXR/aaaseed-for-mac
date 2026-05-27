

in VS_out
{
    vec4            pos_world;
    vec4            pos_ec;
    vec3            nor_ec;
    float           alpha;
    vec4            color;
    vec4            tex_coor[TEX_UNIT_NB];
} vs_out;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];


float           time        =   aaa_fu_float[0];



////
const vec2 Diffusion = vec2(0.08,0.03);
const float dt = 2.;

// nine point stencil
vec2 laplacian1( vec2 position )
{
    vec2 pixelSize = 1. / textureSize( aaa_tex2d[0], 0 );
    vec4 P = vec4( pixelSize, 0.0, -pixelSize.x );
	return
	  0.5 * texture( aaa_tex2d[0],  position - P.xy ).xy // first row
	+       texture( aaa_tex2d[0],  position - P.zy ).xy
	+ 0.5 * texture( aaa_tex2d[0],  position - P.wy ).xy

	+       texture( aaa_tex2d[0],  position - P.xz ).xy // seond row
	- 6.0 * texture( aaa_tex2d[0],  position        ).xy
	+       texture( aaa_tex2d[0],  position + P.xz ).xy
	
    + 0.5 * texture( aaa_tex2d[0],  position + P.wy ).xy // third row
	+       texture( aaa_tex2d[0],  position + P.zy ).xy
	+ 0.5 * texture( aaa_tex2d[0],  position + P.xy ).xy;
}
/*
// nine point stencil
vec2 laplacian2(vec2 position) {
    vec2 pixelSize = 5. / textureSize( aaa_tex2d[0], 0 );
    vec4 P = vec4(pixelSize, 0.0, -pixelSize.x);
	return
	0.5* texture( aaa_tex2d[0],  position - P.xy ).xy // first row
	+ texture( aaa_tex2d[0],  position - P.zy ).xy
	+  0.5* texture( aaa_tex2d[0],  position - P.wy ).xy
	+  texture( aaa_tex2d[0],  position - P.xz).xy // seond row
	- 6.0* texture( aaa_tex2d[0],  position ).xy
	+   texture( aaa_tex2d[0],  position + P.xz ).xy
	+  0.5*texture( aaa_tex2d[0],  position +P.wy).xy  // third row
	+ texture( aaa_tex2d[0],  position +P.zy ).xy
	+   0.5*texture( aaa_tex2d[0],  position + P.xy   ).xy;
}
// nine point stencil
vec2 laplacian3(vec2 position) {
    vec2 pixelSize = 7. / textureSize( aaa_tex2d[0], 0 );
    vec4 P = vec4(pixelSize, 0.0, -pixelSize.x);
	return
	0.5* texture( aaa_tex2d[0],  position - P.xy ).xy // first row
	+ texture( aaa_tex2d[0],  position - P.zy ).xy
	+  0.5* texture( aaa_tex2d[0],  position - P.wy ).xy
	+  texture( aaa_tex2d[0],  position - P.xz).xy // seond row
	- 6.0* texture( aaa_tex2d[0],  position ).xy
	+   texture( aaa_tex2d[0],  position + P.xz ).xy
	+  0.5*texture( aaa_tex2d[0],  position +P.wy).xy  // third row
	+ texture( aaa_tex2d[0],  position +P.zy ).xy
	+   0.5*texture( aaa_tex2d[0],  position + P.xy   ).xy;
}
*/
vec2	laplacian_convolution(vec2 uv)
{
	vec2	ret = vec2(0.);
    vec2	texel = 1./textureSize( aaa_tex2d[0], 0 );
    vec4 d = vec4( texel, -texel.y, 0. );
    /*
    if (uv.x == 0.)
		texel2.x = ;
    else if (uv.x== 1.)
        texel2.x = ;
    else if (uv.y == 0.)
        texel2.y = ;
    else if (uv.y ==1.)
        texel2.y = ;*/
    ret += texture(aaa_tex2d[0], uv  ).xy * 4.;
    ret += texture(aaa_tex2d[0], uv - d.xw ).xy * 2.;
    ret += texture(aaa_tex2d[0], uv + d.xw ).xy * 2.;
    ret += texture(aaa_tex2d[0], uv + d.wz ).xy * 2.;
    ret += texture(aaa_tex2d[0], uv + d.wy ).xy * 2.;
    ret += texture(aaa_tex2d[0], uv - d.xy ).xy;
    ret += texture(aaa_tex2d[0], uv + d.xz ).xy;
    ret += texture(aaa_tex2d[0], uv + d.xy ).xy;
    ret += texture(aaa_tex2d[0], uv - d.xz ).xy;
    return (ret/16.);
}
vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}
vec3	voronoi(vec2 uv)
{
    // Scaling
    uv *= 4.;
    // Space tiling
    vec2 i_uv = floor(uv);
    uv = fract(uv);

    float mdist = 1.;
    vec2 mpt;
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));
            vec2 pt = random2(i_uv + neighbor);
            pt = .5 + .5 * sin(time + 6.28*pt);
            vec2 diff = neighbor + pt - uv;
            float dist = length(diff);//smoothstep(.4, .5, length(diff));
            if (dist < mdist)
            {
	            mdist = dist;
                mpt = pt;
            }
        }
    }
    return vec3(mdist, mpt);
}
void main( void )
{
	vec2 uv = vs_out.tex_coor[0].st;
    if(time < 1730) {
        //float rnd= texture(aaa_tex2d[1], vs_out.tex_coor[1].st).x;//random2(uv).x+(sin(50.*uv.x)+sin(50.*uv.y))*0.;//
        //if(rnd>0.5) gl_FragColor.x=.5;
        //else gl_FragColor.y=.5;
        gl_FragColor = texture2D(aaa_tex2d[1], vs_out.tex_coor[1].st);
        return;
    }
    /*
     fragColor.x = clamp(ab.x + (1. * (laplacian_convolution(uv).x) - ab.x * ab.y * ab.y
                                 + FEED_DEFAULT * (1. - ab.x)) ,-1.,1.);
     fragColor.y = clamp(ab.y + (.5 * (laplacian_convolution(uv).y*uv.y) + ab.x * ab.y * ab.y
                                 - (FEED_DEFAULT + KILL_DEFAULT) * ab.y ),-1.,1.);
    */
    float F = uv.y *0.0042+0.014;//* .005  + .015;
    float k = (length(uv-.5)) *0.021+0.082;//* .02 + .028;
    //float F = voronoi(uv).x *0.0042+0.005;//0.014//* .005  + .015;
    //float k = (length(uv-.5)) *0.021+0.084;//* .02 + .028;
    vec4 data = texture(aaa_tex2d[0], uv);
    float u = data.x;
    float v = data.y;
    vec2 Duv = (1.*laplacian_convolution(uv))*Diffusion;//+0.*laplacian2(uv)+0.*laplacian3(uv))*Diffusion;
    float du = Duv.x - u*v*v + F*(1.-u);
    float dv = Duv.y + u*v*v - (F+k)*v;
    //gl_FragColor = vec4(voronoi(uv).x, 0., 0., 1.);
    //gl_FragColor = vec4( laplacian_convolution(uv), 0., 1. );
    gl_FragColor.xy = clamp(vec2(u+du*dt,v+dv*dt), 0., 1.);
    //gl_FragColor = data;
}