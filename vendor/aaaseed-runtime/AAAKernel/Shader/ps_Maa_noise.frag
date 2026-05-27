in vec3  MCposition;

// Uniform variables for texturing
uniform sampler3D	tex3d_0;
uniform sampler2D	aaa_tex2d[4];



void main (void)
{
vec4 noise;
	noise = texture3D( tex3d_0, MCposition * aaa_fu_float[1]);
//	noise += texture3D( tex3d_0, MCposition * aaa_fu_float[1] * 2.);
//	noise += texture3D( tex3d_0, MCposition * aaa_fu_float[1] * 4.);
//	noise += texture3D( tex3d_0, MCposition * aaa_fu_float[1] * 8.);
vec4	color;
float	comp;
int		index;
int		type;

	type = aaa_fu_int[0];
	index = aaa_fu_int[1];

	if( index == -2)
		comp = noise.r -.25 + noise.g -.125 + noise.b -.0625 + noise.a;
	else if( index == -1)
		comp = abs( noise.r -.25 + noise.g -.125 + noise.b -.0625 + noise.a -.03125);
	else if( index == 0)
		comp = abs( noise.r -.25) + abs( noise.g -.125) + abs( noise.b -.0625) + abs( noise.a -.03125);
	else if( index == 1)
		comp = noise[0];
	else if( index == 2)
		comp = noise[1];
	else if( index == 3)
		comp = noise[2];
	else if( index == 4)
		comp = noise[3];

	comp += aaa_fu_float[2];
	comp *= aaa_fu_float[3];

	if( type == 0 )
		{
		color = noise;
		}
	else if( type == 1 )
		{
		color = vec4( comp, comp, comp, 1.);
		}
	else if( type == 2 )
		{
		comp = sin( MCposition.x * aaa_fu_float[4] + comp ) *.5 + .5;
		color = vec4( comp, comp, comp, 1.);
		}
	else if( type == 2 )
		{
		comp = (sin( MCposition.x * aaa_fu_float[4] ) * cos( MCposition.z * aaa_fu_float[5]) + comp )  *.5 + .5;
		color = vec4( comp, comp, comp, 1.);
		}
	else if( type == 3 )
		{
		comp = ( cos( MCposition.x * aaa_fu_float[4] ) * cos( MCposition.y * aaa_fu_float[4]) * comp)  *.5 + .5;
		color = vec4( comp, comp, comp, 1.);
		}
	gl_FragColor = color;
}