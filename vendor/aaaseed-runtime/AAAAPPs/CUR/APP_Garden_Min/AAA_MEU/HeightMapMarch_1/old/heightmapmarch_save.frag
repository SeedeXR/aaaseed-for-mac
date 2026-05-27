
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

vec3 blackbody(float Temp);
float calculateAO(in vec3 p, in vec3 n);
vec3 calcNormal( in vec3 pos );
vec3 calcNormalOld( in vec3 pos );
vec3	camera(vec2 uv);
mat3    Rot3X( float a );
mat3    Rot3Y( float a );
vec2	texel;


bool	hit;
vec3    nor;
int     id_hit;
vec4    col;
//float   g;
//vec3    grad;

float maxcomp( vec3 p ) {    return max( p.x, max( p.y, p.z ) );    }
// Signed
    // b : Size
    // Leave r=0 if radius not needed
    // ========================================
float sdBox( vec3 p, vec3 b, float r )
{
    vec3 d = abs(p) - b;
    return min( maxcomp( d ), 0.0 ) - r + length( max( d, 0.0 ) );
}

float   map( vec3 p )
{
    float d = 1e6;

    //p *= Rot3X( 1.05+.5*sin(aaa_fu_float[3]*.007) );
    //p *= Rot3X( -1.4136 );
    //col = texture2D( aaa_tex2d[0], p.xy*1.-.5 );
    //vec4 height = vec4( blur_gaussian( p.xy*1.-.5 ), 1. );
    float g = compute_gray( (col.xyz) );
//    d = max(p.z-.5, -p.z-.5)- sin(vs_out.tex_coor[0].y*32.)*.1;
    //d = max(p.z-.5, -p.z-.5)-max(pow(length(g),.25)*.005, .00002);   // use smax ?
    //d = max(p.z-.5, -p.z-.5)-pow(length(g),.25)*.005;
    d = max( p.z-.05, -p.z-.05 )-( g )*.1;
    d = max( d, abs( p.x )-.5 );
    d = max( d, abs( p.y )-.5 );
    //d = min( d, sdBox( p, vec3( .5, .5, g*.025 ), 0. ) );
    float save_d = d;
    d = min( d, length( p-vec3( .20, -.20, -.5 ) ) - .05 );
    id_hit = (save_d != d) ? 2 : 1;
    //d = length( p - vec3( .0, .0, 2.50 ) ) - 1.5 - length(grad) *.5;
    //d = length(grad)-1.95;
    //d = g-.5;

    return d;
}

#define I_MAX   50
#define E       0.001
#define FAR     5.
vec4	march( vec3 pos, vec3 dir ) {
    vec2	dist = vec2( 0.0 );
    vec3	p = vec3( 0.0 );
    vec4	step = vec4( 0.0 );
    for ( int i = I_MAX; i>0; --i )
    {
    	p = pos + dir * dist.y;
        dist.x = map( p );
        dist.y += dist.x;
        if ( dist.x < E )//|| dist.y > 4. )
        {
            hit = true;
            break;
        }
        else if( dist.y >= FAR )
        {
            hit = false;
            return (step);
        }
        step.x++;
    }
    step.y = dist.x;
    step.w = dist.y;
    return ( step );
}
vec2	march_phantom(vec3 pos, vec3 dir)
{
    // Phantom mode from aiekick
    // https://www.shadertoy.com/view/MtScWW
    float accum = 0.;
    float d = 0.;
    float s = 1.;
    for(int i=0;i<100;i++)
    {
		if(s<0.01||d>30.) break;
        s = map(pos + dir * d);

        s = max(abs(s), 0.02);  // Phantom Mode

        d += s * 0.5;

       	accum += 0.005*exp( d*.05 );//*.05; // Phantom Mode
   	}
    return vec2(d, d*.5-accum);
}

void main()
{
    vec3 amb_col;
    vec3 diff_col;
    vec3 spec_col;
    vec2 frag_coord = vs_out.tex_coor[0].xy;
    vec2 ts = textureSize( aaa_tex2d[0], 0 );
    texel = 1./ts;
    col = texture2D( aaa_tex2d[0], frag_coord );

   // vec4 sliderVal = aaa_fu_vec4[0];
    //vec2 m = vec2( aaa_fu_float[1], aaa_fu_float[2] );
    //vec3 rd = normalize( vec3( (frag_coord-.5)*2., aaa_fu_float[5]) );
    float g = compute_gray(col.xyz);
    vec3 grad;
    grad.xy = vec2( dFdx(g), dFdy(g) );

    if( aaa_fu_int[0]==0 )
    {
         col.rgb = vec3( grad.rg*5.+ .5   , 0 );
    }
    else if( aaa_fu_int[0]==1 )
    {
        g = pow( dot( grad.xy, grad.xy ), .5 ) * 50.;
        col.rgb     = vec3( g, g, g );
    }
     else if( aaa_fu_int[0]==2 )
    {
        g = pow( dot( grad.xy, grad.xy ), .5 ) * 50.;
        col.rgb     += vec3( g, g, g ) * pow( col.a, 1.5 );
    }
     else if( aaa_fu_int[0]==3 )
    {
        grad.z = .01;
        grad = normalize( grad ) *.5 + .5;
        col.rgb = grad;
    }
     else if( aaa_fu_int[0]==4 )
    {
        //float dirtRatio = ts.x/ts.y;
        //mat3    rotx = Rot3Y( -( fu_mouse_x / ts.x - .5 ) * dirtRatio );
        //mat3    roty = Rot3X( ( fu_mouse_y / ts.y - .5 ) * dirtRatio );
        //mat3    rot = roty * rotx;
        vec3 pos = vec3(0., 0., -1.);
        vec3 dir = camera( ( frag_coord.xy - .5 ) * 1. );// * Rot3X( .125*sin(aaa_fu_float[3]*.01) );// * rot;
	    hit = false;
        vec4 inter = march( pos, dir );
        //vec2 phant = march_phantom( pos, dir );
	    vec3 v = pos + dir * inter.w;
        vec3 refract_col = vec3(0.);
        if (hit)
        {
            //amb_col  = vec3(1., .75, .5);
            amb_col = vec3(1., 1., 1.);
		    if( id_hit == 2 )
            {
                nor = calcNormal( v );
                diff_col = vec3(.8, .5, .5);
                spec_col = vec3(.8, .5, .5);
            }
            else if( id_hit == 1 )
            {
                col.rgb = texture( aaa_tex2d[0], v.xy+.5 ).rgb;
                float sp = compute_gray( col.rgb );
                nor.xy = vec2( dFdx(sp), dFdy(sp) );
                nor.z = .01;
                //nor = normalize( nor );// *.5 +.5;// *.5 + .75;
                //nor = calcNormal( v );
                vec3 refract_dir = refract(v, nor, .57);
                //nor = nor * .5 + .5;

                // do refract
                refract_col = texture( aaa_tex2d[1], refract_dir.xy+.5 ).xyz*.5;
                //vec4 inter2 = march( pos, dir );
                //vec3 v2 = v +
                diff_col = vec3(1., 1., 1.)*.25;
                spec_col = vec3(1., 1., 1.)*.5;
            }
            vec3	ev = normalize( v - pos );
		    vec3	ref_ev = reflect( ev, nor );
     	    float diffuse  = 1.;
            float specular = 1.;
            float i = 0.;
            //col.xyz = cubeproj( v );
            float light_move_x = 7.*sin( aaa_fu_float[3] * .2 );
            float light_move_y = 5.*cos( aaa_fu_float[3] * .2 );
            //for ( ; i < 3; i++) // For multiple lights
     	    {
            	vec3	light_pos   = vec3( i-light_move_x, i+light_move_y , -2.75 );//path(t+.2), t+.2);

            	vec3	vl = normalize( light_pos - v );
		    	diffuse  += max( 0.001, dot( vl, nor ) );
		    	specular += pow( max( 0.001, dot( vl, ref_ev ) ), 1. );
     	    }

            //float	ao = calculateAO(v, nor);
            vec3 brdf = amb_col * ( diffuse * diff_col + specular * spec_col );// + ao );// * .25 + .75;
            col.xyz *= brdf;
            col.xyz += refract_col;
            //col.xyz *= 40000000.*pow(length(nor),4.);
            //float gray = phant.y  * (exp(-0.001*phant.x*phant.x));
            //col.xyz *= blackbody(gray * 1000.);
            //col.xyz *= blackbody(pow(length(nor),4.) * 10000000000.);
            //col.xyz *= blackbody( 1000. * ( phant.y ));//* ( exp( -0.01*phant.x*phant.x ) ) ) );
            if( nor.x < 0.001 && nor.y < 0.001 )
                col.xyz = vec3(0.);//nor*50.;
            //col.xyz = blackbody( pow(inter.w, 30.)*10. ) *brdf;
            //col =	vec4( 1. ) * brdf;

		    if (id_hit == 2)
                col.xyz = vec3( 1., 0., 0. ) * brdf;
	    }
        else
            col = vec4( 0., 0., 0., 1. );
        //
        //    col.xyz = vec3( inter.x/10.);//, inter.y, inter.w );
    }
    else
    {
        grad.z = .01;
        grad.xy = normalize( grad.xy ) * .5 + .5;
        col.rgb = texture2D( aaa_tex2d[1], grad.xy ).rgb;
    }
    //col.b = 0.;

    //Apply slider overlay
    gl_FragColor = col;
}

vec3 blackbody(float Temp)
{
	vec3 col = vec3(255.);
    col.x = 56100000. * pow(Temp,(-3. / 2.)) + 148.;
   	col.y = 100.04 * log(Temp) - 623.6;
   	if (Temp > 6500.) col.y = 35200000. * pow(Temp,(-3. / 2.)) + 184.;
   	col.z = 194.18 * log(Temp) - 1448.6;
   	col = clamp(col, 0., 255.)/255.;
    if (Temp < 1000.) col *= Temp/1000.;
   	return col;
}

float calculateAO(in vec3 p, in vec3 n)
{

    float sca = 1., occ = 0.;
    for(float i=0.; i<5.; i++)
	{
        float hr = .01 + i*.5/4.;
        float dd = map(n * hr + p);
        occ += (hr - dd)*sca;
        sca *= 0.7;
    }
    return clamp(1.0 - occ, 0., 1.);
}
vec3 calcNormalOld( in vec3 pos )
{
    vec2 e = vec2(1.0,-1.0)*0.5773*0.0005;
    return normalize( e.xyy*map( pos + e.xyy ) +
					  e.yyx*map( pos + e.yyx ) +
					  e.yxy*map( pos + e.yxy ) +
					  e.xxx*map( pos + e.xxx ) );
}
vec3 calcNormal( vec3 pos )
{
    vec3 eps = vec3(0.0001,0.0,0.0);

	return normalize( vec3(
           map(pos+eps.xyy) - map(pos-eps.xyy),
           map(pos+eps.yxy) - map(pos-eps.yxy),
           map(pos+eps.yyx) - map(pos-eps.yyx) ) );
}
vec3	camera(vec2 uv) {
    float   fov = 		 1.;
	vec3    forw  = vec3( 0.0, 0.0, 1.0 );
	vec3    right = vec3( 1.0, 0.0, 0.0) ;
	vec3    up    = vec3( 0.0, 1.0, 0.0) ;

    return ( normalize( uv.x * right
                        + uv.y * up
                        + fov * forw ) );
}
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


    /*
    vec2 ts = textureSize( aaa_tex2d[0], 0 );
    vec2 frag_coord = vs_out.tex_coor[0].xy;
     vec3 e = vec3(vec2(1.)/ts,0.);
        float p10 = compute_gray( texture( aaa_tex2d[0], frag_coord - e.zy ).xyz );
        float p01 = compute_gray( texture( aaa_tex2d[0], frag_coord - e.xz ).xyz );
        float p21 = compute_gray( texture( aaa_tex2d[0], frag_coord + e.xz ).xyz );
        float p12 = compute_gray( texture( aaa_tex2d[0], frag_coord + e.zy ).xyz );
        // Totally fake displacement and shading:
        vec3 grad = normalize( vec3( p21 - p01, p12 - p10, 1. ) );*/