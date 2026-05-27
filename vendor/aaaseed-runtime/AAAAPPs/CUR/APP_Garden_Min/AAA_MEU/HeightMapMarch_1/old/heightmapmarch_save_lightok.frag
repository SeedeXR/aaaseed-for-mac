
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

//uniform vec4        aaa_fu_vec4[]; // color de 0 a 3, active ds 4.xyzw et 5.xy
uniform sampler2D   aaa_tex2d[4];
//uniform float       fu_mouse_x;
//uniform float       fu_mouse_y;
//
vec4    col_texture    =   aaa_fu_vec4[0];
vec4    col_amb        =   aaa_fu_vec4[1];
vec4    col_ao         =   aaa_fu_vec4[2];
vec4    col_diff       =   aaa_fu_vec4[3];
vec4    col_spec       =   aaa_fu_vec4[4];

vec4    light_pos      =   aaa_fu_vec4[5];
vec4    pos            =   aaa_fu_vec4[6];
vec4    rot            =   aaa_fu_vec4[7];

// todo
bool    b_texture   =   bool(   aaa_fu_int[2] 		    & 0x1 );
bool    b_amb       =   bool( ( aaa_fu_int[2] >> 1 )	& 0x1 );
bool    b_ao        =   bool( ( aaa_fu_int[2] >> 2 )	& 0x1 );
bool    b_diff      =   bool( ( aaa_fu_int[2] >> 3 )	& 0x1 );
bool    b_spec      =   bool( ( aaa_fu_int[2] >> 4 )	& 0x1 );
bool    b_shadow    =   bool( ( aaa_fu_int[2] >> 5 )    & 0x1 );
bool    b_refract   =   bool( ( aaa_fu_int[2] >> 6 )	& 0x1 );
bool    b_flare     =   bool( ( aaa_fu_int[2] >> 7 )	& 0x1 );

bool    b_speed     =   bool( ( aaa_fu_int[2] >> 8 )	& 0x1 );

float   speed             =   aaa_fu_float[3];
float   height            =   aaa_fu_float[4];
float   refract_index     =   aaa_fu_float[5];
float   diff              =   aaa_fu_float[6];
float   spec              =   aaa_fu_float[7];

int     num_iter          =   aaa_fu_int[1];

/* Todo
    - change diffuse and spec color
    - change lights pos, type, color
    - decide if can apply shadow without phong
    - use b_texture
    - raymarching options
    - shadow / softshadow params
    - b_blur
    - different gradient calc / interpolate
    - ssao / ao params
    - gamma
    - flare
    - fog
    - camera aaaseed

*/

vec3 blackbody(float Temp);
float calculateAO(in vec3 p, in vec3 n);
vec3 calcNormal( in vec3 pos );
vec3 calcNormalOld( in vec3 pos );
vec3	camera(vec2 uv);
mat3    Rot3X( float a );
mat3    Rot3Y( float a );
vec2	texel;


bool	hit;
vec4    inter;
vec3    nor = vec3(1.,0.,0.);
int     id_hit = 2;
vec4    gcol;
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

float   map2( vec3 p ) // test scene
{
    float d = 1e6;

    //p *= Rot3X( 1.05+.5*sin(aaa_fu_float[3]*.007) );
    //p *= Rot3X( 1.4136 );
    d = min( d, length( p-vec3( .40,    -.40,   -.2 ) ) - .05 );
    d = min( d, length( p-vec3( .40,    .40,    -.2 ) ) - .05 );
    d = min( d, length( p-vec3( -.40,   .40,    -.2 ) ) - .05 );
    d = min( d, length( p-vec3( -.40,   -.40,   -.2 ) ) - .05 );

    d = min( d, length( p-vec3( .0,     .45,    -.1 ) ) - .05 );
    d = min( d, length( p-vec3( .0,     -.45,   -.1 ) ) - .05 );
    d = min( d, length( p-vec3( .45,    .0,     -.1 ) ) - .05 );
    d = min( d, length( p-vec3( -.45,   .0,     -.1 ) ) - .05 );

    d = min( d, sdBox( p, vec3( .5,    .5,    .1 ), 0. ) );
    //d = min( d, sdBox( vec3( p.xy, .31+p.z ), vec3( .1, .1, .02 ), 0. ) );
    d = min( d, length( p - light_pos.xyz ) - .05 );
    id_hit = 2;
    //d = 1e6;
    return d;
}
vec3	sphere_map( vec3 p )
{
    vec2 uv;
    uv.x = 0.5 + atan( p.z, p.x ) / ( 2. * 3.14159 );
    uv.y = 0.5 - asin( p.y ) / 3.14159;
    vec3 col = texture( aaa_tex2d[0], uv ).xyz;
    return col;
}
float   map( vec3 p ) // rename to map to use
{
    float d = 1e6;

    //p *= Rot3X( 1.05+.5*sin(speed) );
    //p *= Rot3X( -1.4136 );
    //p.z += sin(speed);
    //vec4 height = vec4( blur_gaussian( p.xy*1.-.5 ), 1. );
    gcol.xyz = texture2D( aaa_tex2d[0], p.xy*1.-.5 ).xyz;
    //gcol.xyz = blur_gaussian( p.xy*1.-.5, aaa_tex2d[0] );
    //gcol.xyz = sphere_map( p );
    float g = compute_gray( gcol.xyz );
    //float g = compute_gray( blur_gaussian( p.xy*1.-.5 ) );
//    d = max(p.z-.5, -p.z-.5)- sin(vs_out.tex_coor[0].y*32.)*.1;
    //d = max(p.z-.5, -p.z-.5)-max(pow(length(g),.25)*.005, .00002);   // use smax ?
    //d = max(p.z-.5, -p.z-.5)-pow(length(g),.25)*.005;
    d = max( p.z-.05, -p.z-g*height );
    d = max( d, abs( p.x )-.5 );
    d = max( d, abs( p.y )-.5 );
    //d = min( d, sdBox( p, vec3( .5, .5, g*height ), 0. ) );
    //d = p.z - g*height;
    //d = min( d, length(p.xy) - .5 );
float save_d = d;
    d = min( d, length( p - light_pos.xyz ) - .05 );
id_hit = (save_d != d) ? 2 : 1;
    //d = length( p - vec3( .0, .0, 2.50 ) ) - 1.5 - g;
    //d = length(grad)-1.95;
    //d = g-.5;
    //id_hit = 1;

    return d*0.05;
}
float   map_shade( vec3 p ) // rename to map to use
{
    float d = 1e6;
    //return d*1.;
    vec3 col = texture2D( aaa_tex2d[0], p.xy*1.-.5 ).xyz;
    float g = compute_gray( col );

    d = max( p.z-.05, -p.z-g*height );
    d = max( d, abs( p.x )-.5 );
    d = max( d, abs( p.y )-.5 );
    //d = min( d, sdBox( p, vec3( .5, .5, g*height ), 0. ) );

//float save_d = d;
//    d = min( d, length( p - light_pos.xyz ) - .05 );
//id_hit = (save_d != d) ? 2 : 1;
    return d*.05;
}

//#define I_MAX   50
#define E       0.0001
#define FAR     5.
vec4	march( vec3 pos, vec3 dir ) {
    vec2	dist = vec2( 0.0 );
    vec3	p = vec3( 0.0 );
    vec4	step = vec4( 0.0 );
    for ( int i = num_iter; i>0; --i )
    {
    	p = pos + dir * dist.y;
        dist.x = map( p );//*.5;
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

    float softshadow( in vec3 ro, in vec3 rd, float mint, float maxt, float k );
    float shadow( in vec3 ro, in vec3 rd, float mint, float maxt );
vec4 shade( vec3 pos, vec3 dir, vec3 v, vec3 nor )
{
    vec3	ev          =   normalize( v - pos );
	vec3	ref_ev      =   reflect( ev, nor );
    float   diffuse     =   0.;
    float   specular    =   0.;
    float   i           =   0.;
    vec3	vl;
    //if(b_speed)
    //vec3    light       =   normalize(light_pos.xyz);
    vec3    light       =   light_pos.xyz;
                            //vec3( 1.*sin( speed ), 1.*cos( speed ), -.05 );
    float   fDist       =   0.;
    //vec3    light       = gl_LightSource[1].position.xyz;
    //for ( ; i < 3; i++) // For multiple lights
    //{
    	//vec3	light_pos   =   vec3( i-light.x, i+light.y , light.z );

    	    	vl          =   normalize( light - v );
		        diffuse     +=  max( 0.0, dot( vl, nor ) ) * diff;
		        specular    +=  pow( max( 0.001, dot( vl, ref_ev ) ), spec );
        if( b_flare ) //todo adapt for n lights
        {
            float   fPointDot   =   dot( vl, dir );
                    fPointDot   =   clamp( fPointDot, 0.0, inter.w );

            vec3    vClosestPoint = v + dir * fPointDot;
                    fDist       =   length( vClosestPoint - light );
        }
    //}
    if( b_shadow )
        return vec4( diffuse, softshadow( v, vl, .1, 3., 64. ), specular, fDist );
        //return vec4( diffuse, shadow( v, vl, .1, 3./*, 2.*/ ), specular, fDist );
    else
        return vec4( diffuse, 0., specular, fDist );
}

void main()
{
    vec3 amb_col;
    vec3 diff_col;
    vec3 spec_col;
    vec2 frag_coord = vs_out.tex_coor[0].xy;
    vec2 ts = textureSize( aaa_tex2d[0], 0 );
    texel = 1./ts;
    gcol = texture2D( aaa_tex2d[0], frag_coord );

   // vec4 sliderVal = aaa_fu_vec4[0];
    //vec2 m = vec2( aaa_fu_float[1], aaa_fu_float[2] );
    //vec3 rd = normalize( vec3( (frag_coord-.5)*2., aaa_fu_float[5]) );
    float g = compute_gray( gcol.xyz );
    vec3 grad;
    grad.xy = vec2( dFdx(g), dFdy(g) );

    if( aaa_fu_int[0]==0 )
    {
        gcol.rgb = vec3( grad.rg*5.+ .5   , 0 );
    }
    else if( aaa_fu_int[0]==1 )
    {
        g = pow( dot( grad.xy, grad.xy ), .5 ) * 50.;
        gcol.rgb     = vec3( g, g, g );
    }
     else if( aaa_fu_int[0]==2 )
    {
        g = pow( dot( grad.xy, grad.xy ), .5 ) * 50.;
        gcol.rgb     += vec3( g, g, g ) * pow( gcol.a, 1.5 );
    }
     else if( aaa_fu_int[0]==3 )
    {
        grad.z = .01;
        grad = normalize( grad ) *.5 + .5;
        gcol.rgb = grad;
    }
     else if( aaa_fu_int[0]==4 )
    {
        //float dirtRatio = ts.x/ts.y;
        //mat3    rotx = Rot3Y( -( fu_mouse_x / ts.x - .5 ) * dirtRatio );
        //mat3    roty = Rot3X( ( fu_mouse_y / ts.y - .5 ) * dirtRatio );
        //mat3    rot = roty * rotx;
        //vec3 pos = vec3( 0., cos(speed), -1. + 1.*sin(speed) );
        vec3 pos = pos.xyz;//vec3( 0., 0., -1.-sin(speed) );
        vec3 dir = camera( ( frag_coord.xy - .5 ) * 1. ) * Rot3X( rot.x ) * Rot3Y( rot.y );// * rot;
	    hit = false;

        //if( sp > 0.01 )
            inter = march( pos, dir );
        //else
        //    hit = false;
        //vec2 phant = march_phantom( pos, dir );
	    vec3 v = pos + dir * inter.w;
        vec3 refract_col = vec3(0.);
 //       vec3 light = vec3(0.,0.,0.);//gl_LightSource[1].position.xyz;
        //

        //
                //col = texture( aaa_tex2d[0], v.xy+.5 ) * col_texture; //todo choose between apllying after gray or explicitly show it affects height
        if( hit )// && sp > 0.01 )// && g > .001)
        {
		    if( id_hit == 2 )
            {
                nor = calcNormal( v );
                gcol = vec4(0.);
            }
            else if( id_hit == 1 )
            {
                //vec4 col = texture( aaa_tex2d[0], frag_coord.xy ) * col_texture;
                float sp = compute_gray( gcol.rgb );
                if( !b_texture )
                    gcol = vec4( 0. );
                nor.xy = vec2( dFdx(sp), dFdy(sp) );
                nor.z = .001;
            nor = normalize( -nor );// *.5 +.5;// *.5 + .75;
                //nor = calcNormal( v );
                // do refract
                vec3 refract_dir = refract( v, nor, refract_index );
                refract_col = texture( aaa_tex2d[1], refract_dir.xy+.5 ).xyz * .5;
                //nor = nor * .5 + .5;
                //vec4 inter2 = march( pos, dir );
                //vec3 v2 = v +
            }
            if( b_amb )
                if( b_ao )
                    gcol +=  col_amb * col_ao * calculateAO(v, nor);
                else
                    gcol +=  col_amb;
            if( b_diff || b_spec )
            {
                // .x : diffuse, y : shadow, z : spec, w : light flare
                vec4 brdf  =   shade( pos, dir, v, nor );
                if( b_diff )
                    if( b_shadow )
                        gcol    +=  brdf.x * brdf.y * col_diff;
                    else
                        gcol    +=  brdf.x * col_diff;
                if( b_spec )
                    gcol        +=  brdf.z * col_spec;
                vec3 light_col  =   vec3( .5, .5, 0. );
                if( b_flare )
                    gcol.xyz    +=  light_col * 0.5 / (brdf.w * brdf.w);
            }
            if( b_refract )
                    gcol.xyz +=  refract_col;
                    //gcol.xyz = vec3(inter.w);
            //        col.xyz =   vec3(0.);  // + flare;
                    //col.xyz +=  ;
                    //col.xyz   =   pow( col.xyz, vec3(1.) ); // gamma
                    //col.xyz   =   vec3( dir.xy*4., 0. );
                    //col.xyz   *=  vec3(phong.x, phong.x, phong.x);
                    //col.xyz   =   brdf;
                    //gcol.xyz   =   nor;
                    //col.xyz   =  40000000.*pow(length(nor),4.); // cancel refract background
            //float gray    =   phant.y  * (exp(-0.001*phant.x*phant.x));
                    //col.xyz   *=  blackbody(gray * 1000.);
                    //col.xyz   *=  blackbody(pow(length(nor),4.) * 10000000000.);
                    //col.xyz   *=  blackbody( 1000. * ( phant.y ));//* ( exp( -0.01*phant.x*phant.x ) ) ) );
            //if( nor.x < 0.001 && nor.y < 0.001 )
            //    col.xyz = vec3(0.);//nor*50.;
            //col.xyz = blackbody( pow(inter.w, 30.)*10. ) *brdf;
            //col.xyz =	vec3( 1. ) * brdf;
	    }
        else
            gcol = vec4(0.);
        //
        //    col.xyz = vec3( inter.x/10.);//, inter.y, inter.w );
    }
    else
    {
        grad.z = .01;
        grad.xy = normalize( grad.xy ) * .5 + .5;
        gcol.rgb = texture2D( aaa_tex2d[1], grad.xy ).rgb;
    }
    //col.b = 0.;

    //Apply slider overlay
    gl_FragColor = gcol;
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
float softshadow( in vec3 ro, in vec3 rd, float mint, float maxt, float k )
{
    float res = 1.0;
    for( float t=mint; t<maxt; )
    {
        float h = map_shade(ro + rd*t);
        if( h<0.001 )
            return 0.0;
        res = min( res, k*h/t );
        t += h;
    }
    return res;
}
float shadow( in vec3 ro, in vec3 rd, float mint, float maxt )
{
    for( float t=mint; t<maxt; )
    {
        float h = map_shade(ro + rd*t);
        if( h<0.001 )
            return 0.0;
        t += h;
    }
    return 1.0;
}
float calculateAO(in vec3 p, in vec3 n)
{
    float sca = 1., occ = 0.;
    for(float i=0.; i<5.; i++)
	{
        float hr = .001 + .15*i/4.;
        float dd = map(n * hr + p);
        occ += (hr - dd)*sca;
        sca *= 0.95;
    }
    return clamp(1.0 - 1.15*occ, 0., 1.);
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