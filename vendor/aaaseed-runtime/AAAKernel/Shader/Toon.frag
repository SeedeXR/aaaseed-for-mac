//
// Fragment shader for cartoon-style shading
//
// Author: Philip Rideout
//
// Copyright (c) 2005 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//

//uniform float	fu_float[6];

uniform vec3 DiffuseColor;
uniform vec3 PhongColor;
uniform float Edge;
uniform float Phong;

in float f;

//float Edge = fu_float[0];
//float Phong = fu_float[1];

void main (void)
{
	vec3 color = DiffuseColor;
//	float f = dot(vec3(0.0,0.0,1.0),Normal);
	if( abs( f ) < Edge )
		color = vec3(0.0);
	if( f > Phong )
		color = PhongColor; //PhongColor;

	gl_FragColor = vec4(color,1.0);
}
