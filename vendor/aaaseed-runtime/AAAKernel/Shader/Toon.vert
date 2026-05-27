//
// Vertex shader for cartoon-style shading
//
// Author: Philip Rideout
//
// Copyright (c) 2005 3Dlabs Inc. Ltd.
//
// See 3Dlabs-License.txt for license information
//

out float f;

uniform vec3 LightPosition;

void main(void)
{
	vec3 Normal = gl_NormalMatrix * gl_Normal;
	Normal = normalize( Normal);
	gl_Position = ftransform();
//	gl_Position = gl_ModelViewMatrix * gl_Vertex;
//    Diffuse         = max(dot(lightVec, tnorm), 0.0);
    vec3 lightVec   = normalize(LightPosition - gl_Position.xyz);
	f = dot(vec3(0.0,0.0,1.0),Normal);
}
