



in ST_AAA_BV BV;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];



int             de_type    	=   aaa_fu_int[0];
float           time        =   aaa_fu_float[0];
float           scale       =   aaa_fu_float[1];
vec4            col1        =   aaa_fu_vec4[0];
vec4            col2        =   aaa_fu_vec4[1];

#define E 0.001
#define I_MAX 100
#define MAX_ITTERS	20
#define FAR 20

const float BAILOUT=4.0;

float Power = 3.0;
vec3 Julia = vec3( 0.0 );
vec3 Offset = vec3( sin( time ), cos( time ), 0 );
vec3 Clamp = vec3( 1.0, 3.0, 3.0 );
float Bounds = 5.0;

float fMinDist = 100000.0;
float fOrbitTrap = 0.0;

const int Colouring= 4;

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
mat3	rotZ( float a) {
	float c = cos( a );
	float s = sin( a );
	return mat3( c, s, 0, -s, c, 0, 0, 0, 1.0 );
}

float rand( const vec2 vSeed)
{
	const vec2 vR1= vec2( 12.9898,78.233);
	const float fR1= 43758.5453;

	return fract( sin( dot( vSeed,vR1))*fR1);
}

vec3 rand3( const vec2 vSeed)
{
	const vec2 vR1= vec2( 12.9898,78.233);
	const vec2 vR2= vec2( 4.898,7.23);
	const vec2 vR3= vec2( 0.23,1.111);

	const float fR1= 43758.5453;
	const float fR2= 23421.631;
	const float fR3= 392820.23;

	return vec3(
		fract( sin( dot( vSeed,vR1))*fR1),
		fract( sin( dot( vSeed,vR2))*fR2),
		fract( sin( dot( vSeed,vR3))*fR3));
}

float deCubefield( vec3 vRay)
{
	vec3 v= abs( vRay-( floor( vRay)+Offset))*Clamp;
	float r= max( max( v.x,v.y),v.z)-Power;

	if( r<fMinDist) fMinDist= r;
	if( Colouring>0) fOrbitTrap= dot( v,v);

	return r;
}

float deSponge( vec3 vRay)
{
	int nSteps= int( Power);

	vec3 c=( vRay*0.5)+0.5;
	vec3 v= abs( c-0.5)-0.5;

	float m= Clamp.x;
	float r= max( v.x,max( v.y,v.z));

	if( r<fMinDist) fMinDist= r;

	for( int n= 1;n<6;n++)
	{
		if( n>nSteps) break;

		m*= Clamp.y;
		v= ( 0.5-abs( mod( c*m,Clamp.z)-1.5))+Offset;
		r= max( r,min( max( v.x,v.z),min( max( v.x,v.y),max( v.y,v.z)))/m);

		if( r<fMinDist) fMinDist= r;
		if( n== Colouring) fOrbitTrap= dot( v,v);
	}
	return r*2.0;
}

float deSierpinski( vec3 vRay)
{
	vec3 a1= Offset;
	vec3 a2= vec3(-a1.x,-a1.y,a1.z);
	vec3 a3= vec3( a1.x,-a1.y,-a1.z);
	vec3 a4= vec3(-a1.x,a1.y,-a1.z);

	vec3 v= vRay;
	vec3 c;

	float r;
	float d;

	int nStep= 0;

	for( int n= 1;n<= MAX_ITTERS;n++)
	{
		nStep= n-1;

		c= a1;
		r= length( v-a1);

		d= length( v-a2);
		if( d<r) {c= a2; r= d;}

		d= length( v-a3);
		if( d<r) {c= a3; r= d;}

		d= length( v-a4);
		if( d<r) {c= a4; r= d;}

		v= Power*v-c*Clamp;
		r= length( v);

		if( r<fMinDist) fMinDist= r;
		if( n== Colouring) fOrbitTrap= dot( v,v);
	}
	return length( v)*pow( Power,float(-nStep));
}

float deMandelbulb( vec3 vRay)
{
	vec3 v= vRay;
	vec3 c=( Julia!= vec3( 0.0))?Julia:v;

	float r= 0.0;
	float d= 1.0;

	for( int n= 1;n<= MAX_ITTERS;n++)
	{
		r= length( v);

		if( r<fMinDist) fMinDist= r;
		if( n== Colouring) fOrbitTrap= 0.33*log( dot( v,v))+1.0;

		if( r>BAILOUT) break;

		float theta= acos( v.z/r);
		float phi= atan( v.y,v.x);
		d= pow( r,Power-1.0)*Power*d+1.0;

		float zr= pow( r,Power);
		theta= theta*Power;
		phi= phi*Power;

		v=( vec3( sin( theta)*cos( phi),sin( phi)*sin( theta),cos( theta))*zr)+c;

		if( Clamp.x!= 0.0) v.x= max( v.x,Clamp.x);
		if( Clamp.y!= 0.0) v.y= max( v.y,Clamp.y);
		if( Clamp.z!= 0.0) v.z= max( v.z,Clamp.z);

		v+= Offset;
	}
	return 0.5*log( r)*r/d;
}

float deMandelbox( vec3 vRay)
{
	vec4 v= vec4( vRay,1.0);
	vec4 c=( Julia!= vec3( 0.0))?vec4( Julia,1.0):v;

	vec3 vOffset= Offset*2.0;
	vec3 vNegOffset=-Offset;

	float m= Clamp.x*Clamp.x;
	float f= Clamp.y*m;

	vec4 sv= vec4( Power,Power,Power,abs( Power))/m;

	for( int n= 1;n<= MAX_ITTERS;n++)
	{
		v.xyz=( clamp( v.xyz,vNegOffset,Offset)*vOffset)-v.xyz;

		float r= dot( v.xyz,v.xyz);
		v=(( v*clamp( max( f/r,m),0.0,1.0))*sv)+c;

		if( r<fMinDist) fMinDist= r;
		if( n== Colouring) fOrbitTrap= log( dot( v.xyz,v.xyz))+1.0;
	}
	return ( length( v.xyz)-abs( Power-1.0))/v.w-pow( abs( Power),float( 1-MAX_ITTERS));
}

float deDodecahedron( vec3 vRay)
{
	float fPhi= Power;

	float n1= 0.5/fPhi;
	float n2= 1.0/sqrt( pow( fPhi*( 1.0+fPhi),2.0)+pow( fPhi*fPhi-1.0,2.0)+pow( 1.0+fPhi,2.0));

	vec3 p1= vec3( fPhi*n1,fPhi*fPhi*n1,n1);
	vec3 p2= vec3( fPhi*( 1.0+fPhi)*n2,( fPhi*fPhi-1.0)*n2,( 1.0+fPhi)*n2);

	vec3 v= vRay;

	float s1= sin( Julia.x);
	float c1= cos( Julia.x);

	float s2= sin( Julia.y);
	float c2= cos( Julia.y);

	float s3= sin( Julia.z);
	float c3= cos( Julia.z);

	mat3 rot= mat3(
		c1*c3+s1*s2*s3,c2*s3,c1*s2*s3-c3*s1,
		c3*s1*s2-c1*s3,c2*c3,s1*s3+c1*c3*s2,
		c2*s1,-s2,c1*c2);

	float t;
	float r= length( v);

	if( r<fMinDist) fMinDist= r;

	int nStep= 0;

	for( int n= 1;n<= MAX_ITTERS;n++)
	{
		nStep= n-1;

		if( r>BAILOUT) break;

		v= abs( v*rot)+Offset;

		t= p1.y*v.x+p1.z*v.y-p1.x*v.z;
		if( t<0.0) v+= vec3(-2.0,-2.0,2.0)*t*p1.yzx;

		t=-p1.x*v.x+p1.y*v.y+p1.z*v.z;
		if( t<0.0) v+= vec3( 2.0,-2.0,-2.0)*t*p1.xyz;

		t= p1.z*v.x-p1.x*v.y+p1.y*v.z;
		if( t<0.0) v+= vec3(-2.0,2.0,-2.0)*t*p1.zxy;

		t=-p2.x*v.x+p2.y*v.y+p2.z*v.z;
		if( t<0.0) v+= vec3( 2.0,-2.0,-2.0)*t*p2.xyz;

		t= p2.z*v.x-p2.x*v.y+p2.y*v.z;
		if( t<0.0) v+= vec3(-2.0,2.0,-2.0)*t*p2.zxy;

		v= v*2.0-Clamp;
		r= length( v);

		if( r<fMinDist) fMinDist= r;
		if( n== Colouring) fOrbitTrap= dot( v,v);
	}
	return ( r-2.0)*pow( 2.0,-float( nStep));
}

float deKnot( vec3 p)
{
	int nSteps= int( Power);

	float r= length( p.xz);
	float ang= atan( p.z,p.x);
	float y= p.y;
	float d= 10000.0;

	for( int n= 1;n<= MAX_ITTERS;n++)
	{
		if( n>nSteps) break;

		vec3 p= vec3( r,y,ang+PI2*float( n-1));
		p.x-= Offset.z;

		float ra= p.z*Clamp.x/Clamp.z;
		float raz= p.z*Clamp.y/Clamp.z;

		d= min( d,length( p.xy-vec2( Offset.y*cos( ra)+Offset.z,Offset.y*sin( raz)+Offset.z))-Offset.x);

		if( d<fMinDist) fMinDist= d;
		if( n== Colouring) fOrbitTrap= dot( p,p);
	}
	return d;
}

float deQuaternion( vec3 vRay)
{
	vec4 c= vec4( Julia,Power);

	vec4 v= vec4( vRay,0.0);
	vec4 d= vec4( 1.0,0.0,0.0,0.0);

	for( int n= 1;n<MAX_ITTERS;n++)
	{
		d= 2.0*vec4( v.x*d.x-dot( v.xzw,d.yzw),v.x*d.yzw+d.x*v.yzw+cross( v.yzw,d.yzw));
		v= vec4( v.x*v.x-dot( v.yzw,v.yzw),vec3( 2.0*v.x*v.yzw))+c;

		float r= dot( v,v);

		if( r<fMinDist) fMinDist= r;
		if( n== Colouring) fOrbitTrap= r;

		if( r>10.0) break;
	}
	float r= length( v);
	return 0.5*r*log( r)/length( d);
}

float	map( vec3 p)
{
    float d;
	if ( de_type == 0)
		d = deCubefield( p );
	else if ( de_type == 1)
		d = deSponge( p );
	else if ( de_type == 2)
		d = deSierpinski( p );
	else if ( de_type == 3)
		d = deMandelbulb( p );
	else if ( de_type == 4)
		d = deMandelbox( p );
	else if ( de_type == 5)
		d = deDodecahedron( p );
	else if ( de_type == 6)
		d = deKnot( p );
	else if ( de_type == 7)
		d = deQuaternion( p );
	return d;
}

float	march( vec3 pos, vec3 dir)
{
    float d = 0.;
    float prec = E;
    for ( int i = -1; i < I_MAX; i++)
    {
        prec = map( pos + dir * d);
        d += prec;
        if ( d < E || d > FAR)
            break;
    }
    return d;
}

vec3    calcNormal( in vec3 pos, float e, vec3 dir )
{
    vec3 eps = vec3( e, 0.0, 0.0 );
    return normalize( vec3(  march( pos + eps.xyy, dir ) - march( pos - eps.xyy, dir ),
                            march( pos + eps.yxy, dir ) - march( pos - eps.yxy, dir ),
                            march( pos + eps.yyx, dir ) - march( pos - eps.yyx, dir ) ) );
}

vec3	camera( vec2 uv) {
    float fov = 1.;
    vec3 forw = vec3( 0.0, 0.0, 1.0 );
    vec3 right = vec3( 1.0, 0.0, 0.0) ;
    vec3 up = vec3( 0.0, 1.0, 0.0) ;
    return ( normalize(( uv.x-1.)*right + ( uv.y-.5)*up + fov*forw) );
}
vec3 cubeproj( vec3 p )
{
    vec3 x = texture2D( aaa_tex2d[0], (p.zy/p.x) ).xyz;
    vec3 y = texture2D( aaa_tex2d[0], (p.xz/p.y) ).xyz;
    vec3 z = texture2D( aaa_tex2d[0], (p.xy/p.z) ).xyz;

    //simple coloring/shading
   // x *= vec3(1,0,0)*abs(p.x) + p.x*vec3(0,1,0);
    //y *= vec3(0,1,0)*abs(p.y) + p.y*vec3(0,0,1);
   // z *= vec3(0,0,1)*abs(p.z) + p.z*vec3(1,0,0);

    //select face
    p = abs(p);
    if (p.x > p.y && p.x > p.z) return x;
    else if (p.y > p.x && p.y > p.z) return y;
    else return z;
}
void main( void )
{
	vec3 Camera = vec3( 0. );
	if ( de_type == 0) // Cubefield
	{
		Power = 0.2;
		Offset = vec3( 0.5, 0.5, 0.5 );
		Clamp = vec3( 1.0, 1.0, 1.0 );
	//	Light = vec3( 0.0, 0.0, 0.0 );
	}
	else if ( de_type == 1 ) // Sponge
	{
		Power = 3.0;
		Clamp = vec3( 1.0, 3.0, 3.0 );
		Bounds = 5.0;
		Camera.z -= 3.2;
	}
	else if ( de_type == 2 ) // SIERPINSKI
	{
		Power = 2.0;
		Offset = vec3( 1.0, 1.0, 1.0 );
		Clamp = vec3( 1.0, 1.0, 1.0 );
		Bounds = 5.0;
	//	Smooth = 0.5;
		Camera.z -= 3.2;
	}
	else if ( de_type == 3 ) // MANDELBULB
	{
		Power = 8.0;
		Bounds = 5.0;
		Camera.z -= 2.5;
	}
	else if ( de_type == 4 ) // MANDELBOX
	{
		Power =- 1.77;
		Offset = vec3( 1.0, 1.0, 1.0 );
		Clamp = vec3( 0.5, 1.0, 0.0 );
		Bounds = 25.0;
//		Detail = 0.1;
		Camera.z -= 6.5;
	}
	else if ( de_type == 5 ) // DODECAHEDRON
	{
		Power = 1.61803399;
		Clamp = vec3( 1.0, 1.0, 1.0 );
		Bounds = 10.0;
		Camera.z -= 3.5;
	}
	else if ( de_type == 6 ) // KNOT
	{
		Power = 3.0;
		Offset = vec3( 0.07, 0.29, 0.43 );
		Clamp = vec3( -2.0, -4.0, 3.0 );
		Bounds = 10.0;
	//	Smooth = 0.5;
		Camera.z -= 3.5;
	}
	else if ( de_type == 7 ) // QUATERNION
	{
		Power = 0.16;
		Julia = vec3( 0.18, 0.88, 0.24 );
		Bounds = 10.0;
	//	Smooth = 0.5;
		Camera.z -= 3.5;
	}
	vec2 uv = BV.tex_coor[0].st;

    float st = sin( time );
    vec3 pos = vec3( .75, .0, Camera.z -3.85 + .08 * st ) * rotX( time * .3 );
    vec3 dir = camera( uv ) * rotX( time * .3 - .1 );
    vec3 col = vec3( .942, .732, .523 );

	float d = march( pos, dir );
    if ( d < FAR)
    {
		vec3 p = pos + dir * d;
		col *= cubeproj( p );
        vec2 e = vec2( -1., 1. ) * 0.005;
        vec3 n = calcNormal( pos, E, dir );
// Faster but less precise normals :
//        vec3 n = normalize( e.yxx*map( p + e.yxx) + e.xxy*map( p + e.xxy) + e.xyx*map( p + e.xyx) + e.yyy*map( p + e.yyy) );
		vec3 ev = normalize( p - pos );
		vec3 ref_ev = reflect( ev, n );
       // vec3 refl_cube = texture( iChannel1, ref_ev).rgb * REFL_I;
       // col *= clamp( refl_cube, .15, 1.);
        vec3 light_pos = pos + vec3( 0., 0., -.51 );

        vec3 vl = normalize( light_pos - p );
		float diffuse  = max( 0.001, dot( vl, n ) );
		float specular = pow( max( 0.001, dot( vl, ref_ev ) ), 1. );
        float	brdf = ( diffuse + specular );// * .5 + .5;
        col *=  brdf;
        gl_FragColor = vec4( col, 1. );
    }
    else
        gl_FragColor = vec4( 0. );
}