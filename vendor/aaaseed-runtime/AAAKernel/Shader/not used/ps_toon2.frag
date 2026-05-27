// Toon Shader



in float specular;
in float diffuse;
in float sil;

uniform float	fu_float[6];
uniform vec4	aaa_fu_vec4[6];


float silhouetteThreshold = fu_float[0];
//float shininess = fu_float[1];
//#define shininess 20.0

void main (void)
{
//	vec3 vVertex = vec3( ecPosition );

	// Material Color:
//	vec4 materialColor = aaa_fu_vec4[0]; //gl_FrontMaterial.diffuse;

	// Silhouette Color:
//	vec4 silhouetteColor = aaa_fu_vec4[1]; //vec4( 0.0, 0.0, 0.0, 1.0 );

	// Specular Color:
//	vec4 specularColor = aaa_fu_vec4[2]; //gl_FrontMaterial.specular;

	// Lighting
//	vec3 eyePos = normalize( -vVertex );
//	vec3 lightPos = gl_LightSource[0].position.xyz;

//	vec3 Normal = transformedNormal;
//	vec3 EyeVert = normalize( eyePos - vVertex );
//	vec3 LightVert = normalize( lightPos - vVertex );
//	vec3 EyeLight = normalize( LightVert + EyeVert );

//	// Simple Silhouette
//	float sil = max( dot( Normal, EyeVert ), 0.0);

//	vec4	color;
	if( sil < silhouetteThreshold )
		gl_FragColor  = aaa_fu_vec4[1]; //silhouetteColor;
	else
		{
		gl_FragColor  = aaa_fu_vec4[0]; //materialColor;

//	   	// Specular part
//	   	float spec = pow( max( dot( Normal, EyeLight ), 0.0 ), shininess );

	   	if( spec < 0.2 )
	   		gl_FragColor  *= 0.98;
	   	else
	   		gl_FragColor  = aaa_fu_vec4[2]; //specularColor;

	   	// Diffuse part
//	   	float diffuse = max( dot( Normal, LightVert ), 0.0 );
	   	if( diffuse < 0.5 )
	   		gl_FragColor  *= 0.9;
		}
//	gl_FragColor = color;
//	gl_FragColor = aaa_fu_vec4[2];
}
