//#version 330

layout(location = 0) out vec4 o0;
uniform float time;

in vec3 pv;
in vec3 dv;

uniform sampler3D voltex;
uniform sampler2D xfer_func;
uniform float v1;
uniform float v2;
uniform float noise;

#define EPSILON 0.0001

#define PP 1.0
#define PX ( PP / 256.0 )
#define PY ( PP / 256.0 )
#define PZ ( PP / 256.0 )

int LFSR_Rand_Gen( in int n )
{
	n = (n << 13) ^ n;
	return (n * (n*n*15731+789221) + 1376312589) & 0x7fffffff;
}

float noise3f( in vec3 p )
{
	ivec3 ip = ivec3(floor(p));
	int n = ip.x + ip.y*57 + ip.z*113;
	return float(LFSR_Rand_Gen(n)) / 1073741824. - .5;
}


vec3 lightdir = vec3( 0, -1, 0 );

float ambientParam = 0.2;
float diffuseParam = 0.25;
float specularParam = .25;
float shininessParam = 128.;


vec2 box( vec3 p, vec3 d, vec3 c, vec3 s )
{
	vec3 bl = c - s, bh = c + s;
	vec3 ol = (bl - p) / d, oh = (bh - p) / d;
	vec3 l = min(ol, oh);
	vec3 h = max(ol, oh);
	float ff = min(h.x, min(h.y, h.z));
	float fn = max(max(l.x, 0), max(l.y, l.z));
	return vec2(step(fn, ff), fn);
}


vec4 xfer( float center, float width, float i )
{
	vec4 ret = vec4(1,0,0,0);

	float range = width;
	float minThresh = center - width/2.;
	float maxThresh = center + width/2.;
	if ( i < minThresh )
	{
		ret = vec4 (0,0,0,0);
	}
	else
	{
		if( i < maxThresh )
		{
			ret.a = (i-minThresh) /(maxThresh-minThresh);
			if( i < center )
			{
				float t = (i-minThresh)/(center-minThresh);
				ret.r = (108.0 + t * (200.0 - 108.0));
				ret.g = (t * 164.0);
				ret.b = (t * 115.0);
			}
			else
			{
				float t = (i-center)/(center);
				ret.r = (200.0 + t * (254.0 - 200.0));
				ret.g = (164.0 + t * (243.0 - 164.0));
				ret.b = (115.0 + t * (220.0 - 115.0));
			}
			ret.r /= 255.;
			ret.g /= 255.;
			ret.b /= 255.;
		}
		else
		{
			ret.r = 1./254.0;
			ret.g = 1./243.0;
			ret.b = 1./220.0;
			ret.a = 1./255.0;
		}
	}
	return ret;
}

#define STEPS 800
//#define PRF
#define LIGHT
#define SKIP_AHEAD
#ifdef PRF
	float count = 1.0;
#endif

void main(void)
{
	vec3 p = pv, d = normalize(dv);
	vec2 h = box(p, d, vec3(0), vec3(1));
	if (h.x == 0)
	{
		discard;
	}

	vec3 t = (d*h.y + p)* .5 + .5;

	// raycast starts here :)

	vec3 raydir = d*0.001;
	vec4 sum = vec4(0,0,0,0);

	float nnn = 0.25*(2.0+noise3f( vec3(gl_FragCoord.xy,1.0)));
	vec3 sample_position = t + raydir*(noise*nnn);

	vec3 sample_position1 = sample_position + vec3(	PX,		0.0,	0.0	);
	vec3 sample_position2 = sample_position - vec3(	PX,		0.0,	0.0	);
	vec3 sample_position3 = sample_position + vec3(	0.0,	PY,		0.0	);
	vec3 sample_position4 = sample_position - vec3(	0.0,	PY,		0.0	);
	vec3 sample_position5 = sample_position + vec3(	0.0,	0.0,	PZ	);
	vec3 sample_position6 = sample_position - vec3(	0.0,	0.0,	PZ	);


	//vec3 dp_col = sample_position;

	// initial ray warping
#ifdef SKIP_AHEAD
	vec4 test = vec4(0.0);
/*
	float sstep = 32.0;
	int iter = 10;
	for( int j = 0; j < iter; j++ )
	{
		sample_position = sample_position + raydir*sstep;
		test = texture(voltex, sample_position);
		if( test.x > 0.1 )
		{ // huch... zu weit!!
			sample_position = sample_position - raydir*sstep;
			break;
		}
		sstep *= .5;
	}
*/
#endif

/*
	do
	{
		sample_position = sample_position + raydir;
		test = texture(voltex, sample_position);
	}
	while( test.x < 0.1 );
*/

	for( int i=0; i<STEPS; i++ )
	{
#ifdef PRF
		count += 1.0;
#endif

		// sample_position = sample_position - raydir*10;
		sample_position = sample_position + raydir;

		//sampling
		vec4 sample = texture(voltex, sample_position);
		//clasification

		//    sample.x =   0.0;
		//if( i < 1 )
		//{ // advance initial ray if empty cell
		//    sample_position = sample_position + raydir*200;
		//}
		//  }

		sample_position1 = sample_position1 + raydir;
		sample_position2 = sample_position2 + raydir;
		sample_position3 = sample_position3 + raydir;
		sample_position4 = sample_position4 + raydir;
		sample_position5 = sample_position5 + raydir;
		sample_position6 = sample_position6 + raydir;

		if( sample.x < 0.15 )
		{
			sample.x = 0.0;
		}

		// TODO: insert lookup here ... sth like texture( xfer_func, sample.xy );

		vec3 backward;
		backward.x	= texture(voltex, sample_position2).x;
		backward.y	= texture(voltex, sample_position4).x;
		backward.z	= texture(voltex, sample_position6).x;
		vec3 fwd;
		fwd.x		= texture(voltex, sample_position1).x;
		fwd.y		= texture(voltex, sample_position3).x;
		fwd.z		= texture(voltex, sample_position5).x;

		vec3 gradient = backward - fwd;

		float l = length(gradient);

		gradient = normalize(gradient);	//maa opt vthis we have the length

		// vec4 color =  xfer( v1, v2, sample.x);
		//texture(xfer_func, sample.xx );
		vec4 color =  texture( xfer_func, vec2( backward.x+v1, fwd.x+v2 ) );

		//float LdotG = abs( dot(lightdir,gradient) );
		float LdotG = clamp( dot(lightdir,gradient), 0., 1. );

		float diffuse = LdotG;
		//float diffuse =  clamp(dot(lightdir,gradient), 0., 1. );
		float specular = pow( LdotG, shininessParam );

		//color.rgb = 0.01*vec3(specular);
#ifdef LIGHT
		color.rgb = color.rgb * (ambientParam + diffuseParam * diffuse) + specularParam * specular * color.a;
#endif
		//silouette
		//    float factor = pow( cos( abs( dot( lightdir, gradient ) ) ), 2120.0 );
		//  color.rgb = color.rgb - factor;

		//gradient weighting
		//color.rgba = color.rgba * pow( 4.0, 0.99 );

		//integration
		sum = (1 - sum.a) * clamp( color, 0., 1. ) + sum;
		if( sum.a > .9 )
			break;

		// march onwards :P

		sample_position = sample_position + raydir;
		sample_position1 = sample_position1 + raydir;
		sample_position2 = sample_position2 + raydir;
		sample_position3 = sample_position3 + raydir;
		sample_position4 = sample_position4 + raydir;
		sample_position5 = sample_position5 + raydir;
		sample_position6 = sample_position6 + raydir;

		//maa optimize
		if( sample_position.x > 1.0 ) break;
		if( sample_position.y > 1.0 ) break;
		if( sample_position.z > 1.0 ) break;

		if( sample_position.x < 0.0 ) break;
		if( sample_position.y < 0.0 ) break;
		if( sample_position.z < 0.0 ) break;
	}

	// profile
#ifdef PRF
	float vvv = count/(float(STEPS));
	o0 = vec4( vec3(vvv), 1.0 );
#else
	o0 = sum;
#endif


#if 0
	//float nz = 2. + noise3f( vec3(gl_FragCoord.xy,0.00001) );
	o0 = vec4(vec3(nnn),1.0);
	if( nnn < 0.5 )
	{
		o0 = vec4(vec3(1,0,0),1.0);
	}
#endif
}