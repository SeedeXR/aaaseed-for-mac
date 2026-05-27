// Pixel Shader X-Ray

in vec3 N;
in vec3 I;
in vec4 Cs;

// globals
uniform float	fu_float[6];

float edgefalloff = fu_float[0];

// entry point
void main( void )
{
//	vec3 I = vec3( ecPosition );
    float opac = dot( normalize( N ), normalize( -I ) );
    opac = abs( opac );
    opac = 1.0 - pow( opac, edgefalloff );

    gl_FragColor =  vec4( opac * Cs.x, opac * Cs.y, opac * Cs.z, opac) ;
  //  gl_FragColor.a = opac;
//	gl_FragColor = Cs; //vec4( 1.0, 0.0, 0.2, 1.0);
}