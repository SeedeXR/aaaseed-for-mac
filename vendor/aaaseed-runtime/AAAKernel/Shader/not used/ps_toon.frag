// Toon Shader

in vec3 transformedNormal;
in vec4 ecPosition;

uniform float	fu_float[6];
uniform vec4	aaa_fu_vec4[6];

float silhouetteThreshold = fu_float[0];
float shininess = fu_float[1];
//#define shininess 20.0

void main (void)
{
	vec3 vVertex = vec3( ecPosition );

	// Material Color:
	vec4 materialColor = gl_FrontMaterial.diffuse;

	// Silhouette Color:
	vec4 silhouetteColor = aaa_fu_vec4[0]; //vec4( 0.0, 0.0, 0.0, 1.0 );

	// Specular Color:
	vec4 specularColor = gl_FrontMaterial.specular;

	// Lighting
	vec3 eyePos = normalize( -vVertex );
	vec3 lightPos = gl_LightSource[0].position.xyz;

	vec3 Normal = transformedNormal;
	vec3 EyeVert = normalize( eyePos - vVertex );
	vec3 LightVert = normalize( lightPos - vVertex );
	vec3 EyeLight = normalize( LightVert + EyeVert );

	// Simple Silhouette
	float sil = max( dot( Normal, EyeVert ), 0.0);
	if( sil < silhouetteThreshold )
		gl_FragColor = silhouetteColor;
	else
		{
		gl_FragColor = materialColor;

	   	// Specular part
	   	float spec = pow( max( dot( Normal, EyeLight ), 0.0 ), shininess );

	   	if( spec < 0.2 )
	   		gl_FragColor *= 0.98;
	   	else
	   		gl_FragColor = specularColor;

	   	// Diffuse part
	   	float diffuse = max( dot( Normal, LightVert ), 0.0 );
	   	if( diffuse < 0.5 )
	   		gl_FragColor *= 0.9;
		}
}
