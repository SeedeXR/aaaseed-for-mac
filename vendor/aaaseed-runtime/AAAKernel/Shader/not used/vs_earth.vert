// GLSL vertex shader for drawing the earth with three textures
// for Aaaseed
// Author : Franz Hildgen


uniform int vu_def_type;
uniform float vu_def_value[6];

out vec4 ecPosition;
out vec3 transformedNormal;
out float diffuse;
out vec3  specular;

void main(void)
{
    vec3 ecPosition = vec3 (gl_ModelViewMatrix * gl_Vertex);
    vec3 tnorm      = normalize(gl_NormalMatrix * gl_Normal);
    vec3 lightVec   = normalize(LightPosition - ecPosition);
    vec3 reflectVec = reflect(-lightVec, tnorm);
    vec3 viewVec    = normalize(-ecPosition);

    float spec      = clamp(dot(reflectVec, viewVec), 0.0, 1.0);
    spec            = pow(spec, 8.0);
    Specular        = vec3 (spec) * vec3 (1.0, 0.941, 0.898) * 0.3;

    Diffuse         = max(dot(lightVec, tnorm), 0.0);

    TexCoord        = gl_MultiTexCoord0.st;
    gl_Position     = ftransform();
}

vec4 banana( in vec4 gl_pos )
{
//	float screen_def_cos_freq = vu_def_value[0];
//	float screen_def_cos_limit = vu_def_value[1];
//	float screen_def_z_factor = vu_def_value[2];
//	float screen_def_y_origin = vu_def_value[3];
//	float screen_def_y_factor = vu_def_value[4];
	vec4 gl_position_new;
	float over;
	float tmp;

	gl_position_new = gl_pos;

	over = 1. / gl_pos.z;
	tmp = ( 1. - cos( gl_pos.x * over * vu_def_value[0] ) );
	if( tmp > vu_def_value[1] )
		tmp = vu_def_value[1];
	tmp *= gl_pos.z;
	gl_position_new.z += tmp * vu_def_value[2];
	gl_position_new.y += tmp * vu_def_value[4] * (gl_pos.y * over - vu_def_value[3] );

	return gl_position_new;

}

void main(void)
{
	//  vec3  transformedNormal;

	// Eye-coordinate position of vertex, needed in various calculations
//		vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ecPosition = gl_ModelViewMatrix * gl_Vertex;


	// Do fixed functionality vertex transform
	gl_Position = ftransform();
	transformedNormal = fnormal();
	gl_FrontColor = flight( transformedNormal, ecPosition );
	ftexgen( transformedNormal, ecPosition );

    vec3 lightVec = normalize( gl_LightSource[i] - ecPosition );
    vec3 reflectVec = reflect( -lightVec, transformedNormal );
    vec3 viewVec    = normalize( -ecPosition );

    float spec      = clamp( dot( reflectVec, viewVec ), 0.0, 1.0 );
    spec            = pow( spec, 8.0 );
    specular        = vec3( spec ) * vec3( 1.0, 0.941, 0.898 ) * 0.3;

    diffuse         = max( dot( lightVec, tnorm ), 0.0 );

	if( vu_def_type == 1 )
		gl_Position = banana( gl_Position );

}

