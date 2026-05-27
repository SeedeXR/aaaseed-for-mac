//TODO
// - stop using SDF thickness to control ceiling thickness
// - ceiling thickness based on where drips are instead of a sine wave
// - simple paint drip transition shader fork
// - motion phases:
//    * drip grow - base drip y (bdy) from len to bounce, tapering to a min size
//                - falling drip at bdy+n1 growing in size, tail tapering to min size at bdy+n1-n2
//    * drip detach and recede when ( n1-n2 == bdy )
//                - base drip recedes quickly, n1 increases & n2 decreases sharply and linearly


const float PI = 3.14159265359;

float seed = 0.25;

float dripDistance = 0.1;
float density = 0.75;

float bCurve = 1.5;
float bFreq = 3.5;
float bRange = 0.35;

float fallSpeed = 6.0;

float sdfWidth = 0.18;


float rand( float x, float y ){
	return sin( x + sin(x * 10.) * .5  ) - y*.2;
//    return texture( iChannel0, vec2(x,y) / (iResolution.x / iResolution.y) ).r;
}

float dripSDF( vec2 uv )
{
    float s = sdfWidth * abs((1.0-uv.y)-0.75) + 0.05;
    float o = 1.0;
    float drip = 999.0;

    float x = uv.x - sdfWidth;
    x += dripDistance - mod(x, dripDistance);

    x -= dripDistance; //ungh... this is dirty... I'll fix it later
    for( int i=0; i<1000; i++ )
    {
        if( x > uv.x + sdfWidth ) break;

        x += dripDistance;
        float isLine = round(rand(x, seed) + density-0.5);
        if( isLine == 0.0 ) continue;

        float y = rand(seed,x) * 0.8 + 0.1;
        //y *= abs(sin(x*3.0))*0.5 + 0.5;
        float animTime = iTime+(y*10.0);
        float bounce = 0.0 - (bCurve * mod(animTime, bFreq)) * exp(1.0-bCurve*mod(animTime, bFreq));
        y += bounce * bRange;
        y = min(y,uv.y);

        float f = y + mod(animTime, bFreq) * fallSpeed * bRange;

        //float d = min( distance(vec2(x,y),uv), distance(vec2(x,f),uv) );
        float d = distance(vec2(x,y),uv);

        o *= clamp(d/s,0.0,1.0);
        drip = min( drip, distance(vec2(x,f),uv) );
    }

    o = min( o, clamp(drip/s,0.0,1.0) );

    s = sin(uv.x*20.0+ iTime * 0.2)*0.3 + 0.4;
    return o * clamp(distance(0.0, uv.y)/s,0.0,1.0);
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord/iResolution.xy;
    uv.x *= iResolution.x / iResolution.y; //square
    uv.y = 1.0 - uv.y; //flip

	vec3 col = vec3(0.0);


    float c = 1.0/sdfWidth * 0.025;
    float w = 0.03;

    if( iMouse.z > 0.0 ){
        density = iMouse.y/iResolution.y;
        c = iMouse.x/iResolution.x;
    }

    float d = dripSDF(uv);


    float v = 1.0 - smoothstep( c - w, c + w, d );
	col.rg = vec2(v, v-.2);
    //col.r = d;

    fragColor = vec4(col,1.0);
    //fragColor = vec4(rand(uv.x, uv.y));
}