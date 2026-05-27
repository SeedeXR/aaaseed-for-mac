
uniform int aaa_vu_def_type;
uniform float aaa_vu_def_value[6];

out vec4 ecPosition;
out vec3 transformedNormal;
out float alpha;

vec4 banana( in vec4 gl_pos )
{
//	float screen_def_cos_freq = aaa_vu_def_value[0];
//	float screen_def_cos_limit = aaa_vu_def_value[1];
//	float screen_def_z_factor = aaa_vu_def_value[2];
//	float screen_def_y_origin = aaa_vu_def_value[3];
//	float screen_def_y_factor = aaa_vu_def_value[4];
	vec4 position_new;
	float over;
	float tmp;

	position_new = gl_pos;

	over = 1. / gl_pos.z;
	tmp = ( 1. - cos( gl_pos.x * over * aaa_vu_def_value[0] ) );
	if( tmp > aaa_vu_def_value[1] )
		tmp = aaa_vu_def_value[1];
	tmp *= gl_pos.z;
	position_new.z += tmp * aaa_vu_def_value[2];
	position_new.y += tmp * aaa_vu_def_value[4] * (gl_pos.y * over - aaa_vu_def_value[3] );

	return position_new;
}

void main(void)
{
	alpha = gl_Color.a;
	// Eye-coordinate position of vertex, needed in various calculations
	ecPosition = gl_ModelViewMatrix * gl_Vertex;

	// Do fixed functionality vertex transform
	gl_Position = ftransform();
	transformedNormal = fnormal( gl_Normal );
	if( aaa_lights!=0u )
	{
	//	gl_FrontColor = gl_Color;
		gl_FrontColor = flight( transformedNormal, ecPosition.xyz ) ;
	}
	else
	{
		gl_FrontColor = gl_Color;
	//	gl_BackColor = gl_Color;
	}
	//gl_FrontColor.a = 1.0;	//counter bug ?
	
	//todo
	//ftexgen( transformedNormal, ecPosition );
	float v = clamp( (ecPosition.z + aaa_vu_float[0]) * aaa_vu_float[1], 0.0, 1.0 );
	gl_PointSize = aaa_vu_float[4] + v * ( aaa_vu_float[5] - aaa_vu_float[4] );

	if( aaa_vu_def_type == 1 )
		gl_Position = banana( gl_Position );
}

