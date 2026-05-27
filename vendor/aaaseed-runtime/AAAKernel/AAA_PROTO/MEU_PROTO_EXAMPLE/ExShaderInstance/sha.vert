#define	time 		aaa_vu_float[0]
#define	nb_over 	aaa_vu_float[1]

layout(location = 0) out flat vec4 layer_color;

void main(void)
{
	int id = gl_VertexID;
	float ph = float(id) * nb_over + time * .2;

	float angle = ph * PI2;
	float s = sin(angle);
	float c = cos(angle);

	float r = 1 + pow( cos( ph * PI2 * 5. + time) *.5 + .5,  aaa_vu_float[2]) * aaa_vu_float[3]; //	+ iqNoise2f( vec2( angle,time)*5. ) * .5;
	vec4 pos = vec4( r*c, r*s, 0., 1. );

	gl_Position = aaa_cam.view_projection * pos;

	gl_PointSize = (1. + iqNoise3f( vec3( pos.xy*5., time*2. ) ) ) * 40.;

	layer_color = gl_Color;
}
