

uniform int vu_def_type;
uniform float vu_def_value[6];
uniform int vu_float[6];

out vec4 ecPosition;
out vec3 transformedNormal;
out float specular;
out float diffuse;
out float sil;

float shininess = fu_float[1];

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
	vec3  transformedNormal;

	// Eye-coordinate position of vertex, needed in various calculations
//		vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ecPosition = gl_ModelViewMatrix * gl_Vertex;


	// Do fixed functionality vertex transform
	gl_Position = ftransform();
	transformedNormal = fnormal();

	// Lighting
	vec3 eyePos = normalize( -ecPosition );
	vec3 lightPos = gl_LightSource[0].position.xyz;

	vec3 Normal = transformedNormal;
	vec3 EyeVert = normalize( eyePos - ecPosition );
	vec3 LightVert = normalize( lightPos - ecPosition );
	vec3 EyeLight = normalize( LightVert + ecPosition );

	sil = max( dot( Normal, EyeVert ), 0.0);
   	specular = pow( max( dot( Normal, EyeLight ), 0.0 ), shininess );
   	diffuse = max( dot( Normal, LightVert ), 0.0 );

	if( vu_def_type == 1 )
		gl_Position = banana( gl_Position );

}

