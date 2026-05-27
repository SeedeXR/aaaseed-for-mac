
// Uniform variables for texturing
uniform sampler2D tex0;

uniform float	fu_float[6];
//	0
//	1
//	2	nb_pixel detect
//	3	texture size x
//	4	texture size y
//

vec4 specExp = { 0.5, .0, .0, .0 };



vec3 sobel()
{
//vec4	tex = texture2D( tex0, gl_TexCoord[0].st );

vec4	cx;
vec4	cy;
float NPixels = fu_float[2];
//float Threshold = fu_float[3] * fu_float[3];

float	dx = NPixels/fu_float[3];
float 	dy = NPixels/fu_float[4];
 //todoopt use swizzle
	cx =		texture2D( tex0, gl_TexCoord[0].xy + vec2(	dx,		dy) );
	cx -=		texture2D( tex0, gl_TexCoord[0].xy + vec2(	-dx,	-dy) );
	cy = cx;

	cx += 2.0 *	texture2D( tex0, gl_TexCoord[0].xy + vec2(	0.0,	dy) );
	cx +=		texture2D( tex0, gl_TexCoord[0].xy + vec2(	-dx,	dy) );
	cx -=		texture2D( tex0, gl_TexCoord[0].xy + vec2(	dx,		-dy) );
	cx -= 2.0 *	texture2D( tex0, gl_TexCoord[0].xy + vec2(	0.0,	-dy) );

	cy += 2.0 *	texture2D( tex0, gl_TexCoord[0].xy + vec2(	dx,		0.0) );
	cy +=		texture2D( tex0, gl_TexCoord[0].xy + vec2(	dx,		-dy) );
	cy -=		texture2D( tex0, gl_TexCoord[0].xy + vec2(	-dx,	dy) );
	cy -= 2.0 *	texture2D( tex0, gl_TexCoord[0].xy + vec2(	-dx,	0.0) );


	vec4 r1;
	r1 = cx * cx + cy * cy;

	r1.x = max( r1.x, r1.y );
	r1.x = max( r1.x, r1.z );

	vec3 color;
	color = pow( r1, specExp );
//	cx.x = abs(cx.x);
//	cx.y = abs(cx.y);
//	cx.z = abs(cx.z);

//float gx = gray( cx );

//	cy.x = abs(cy.x);
//	cy.y = abs(cy.y);
//	cy.z = abs(cy.z);
//float gy = gray( cy );
//float dist;

//	dist = fu_float[4]*gx*gx + fu_float[5]*gy*gy;




	return color;

}

vec3 scanly( in vec3 sobel )
{
	vec4	tex = texture2D( tex0, gl_TexCoord[0].st );


//normalize the color hue

//calculate [sum RGB]
	vec4	hue;
	hue.x = tex.x + tex.y + tex.z;

//calculate 1/[sum RGB]
	hue.y = 1 / hue.x;

//multiply pixel color with 1/[sum RGB]
	vec3 color;
	color *= hue.y;

//calculate the pixel intensity

// make everything with [sum RGB] <.25 black

// define darktones ([sum RGB] >.2) and store in channel z
	if( hue.x >= .25 )
		hue.z = 1.0;
	else
		hue.z = 0.0;

// darktones will be normalized HUE * 0.5
	hue.z *= 0.5;

// define midtones ([sum RGB] >.8) and store in channel y
	if( hue.x >= 0.8 )
		hue.y = 1.0;
	else
		hue.y = 0.0;

// midtones will be normalized HUE * (0.5+0.5)
	hue.y *= 0.5;

// define brighttones ([sum RGB] >1.5) and store in channel x
	if( hue.x >= 1.5 )
		hue.x = 1.0;
	else
		hue.x = 0.0;

// brighttones will be normalized HUE * (0.5+0.5+1.5)
	hue.x *= 1.5;

//# sum darktones+midtones+brighttones to calculate final pixel intensity
	hue.x = hue.x + hue.y + hue.z;

//multiply pixel color with rotoscope intensity
	color *= hue.x;

//generate edge mask -> make 1 for all regions not belonging to an edge
//use 0.8 as threshold for edge detection
	if( sobel.x < 0.8 )
		sobel.x = 1.0;
	else
		sobel.x = 0.0;

//blend white into edge artifacts (occuring in bright areas)
//find pixels with intensity>2.5
	if( hue.x >= 2.5 )
		sobel.y = 1.0;
	else
		sobel.y = 0.0;

//set the edge mask to 1 in such areas
	sobel.x = max( sobel.x, sobel.y );

//multiply cleaned up edge mask with final color image
	color *= sobel.x;

//transfer image to output
	return color;

}

void main()
{
	vec3 sob = sobel();
	gl_FragColor.xyz = scanly( sob );
}