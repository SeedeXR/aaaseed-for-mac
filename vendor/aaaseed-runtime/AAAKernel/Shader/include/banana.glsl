//not used anymore, kept to keep trace of mathematics

#define BANANA 0

#if BANANA
uniform int aaa_vu_def_type;
uniform float aaa_vu_def_value[6];

vec4 banana( in vec4 pos )
{
//	float screen_def_cos_freq = aaa_vu_def_value[0];
//	float screen_def_cos_limit = aaa_vu_def_value[1];
//	float screen_def_z_factor = aaa_vu_def_value[2];
//	float screen_def_y_origin = aaa_vu_def_value[3];
//	float screen_def_y_factor = aaa_vu_def_value[4];
	vec4 position_new;
	float over;
	float tmp;

	position_new = pos;

	over = 1. / pos.z;
	tmp = ( 1. - cos( pos.x * over * aaa_vu_def_value[0] ) );
	if( tmp > aaa_vu_def_value[1] )
		tmp = aaa_vu_def_value[1];
	tmp *= pos.z;
	position_new.z += tmp * aaa_vu_def_value[2];
	position_new.y += tmp * aaa_vu_def_value[4] * (pos.y * over - aaa_vu_def_value[3] );

	return position_new;
}
#endif

#if BANANA
	if( aaa_vu_def_type == 1 )
		gl_Position = banana( gl_Position );
#endif