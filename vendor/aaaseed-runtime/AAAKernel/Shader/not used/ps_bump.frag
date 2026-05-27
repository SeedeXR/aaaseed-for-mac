// Pixel Shader Bump Mapping

in vec3 lightVec;
in vec3 viewVec;
in vec2 texCoord;

// Uniform variables for texturing
uniform sampler2D tex0;
uniform sampler2D tex1;
//uniform sampler2D colorMap;
//uniform sampler2D normalMap;
uniform float	fu_float[6];
float invRadius = fu_float[1];

void main (void)
{
	float distSqr = dot( lightVec, lightVec );
	float att = clamp(1.0 - invRadius * sqrt( distSqr ), 0.0, 1.0 );

	vec3 lVec = lightVec * inversesqrt( distSqr );
	//vec3 lVec = normalize(lightVec);
	vec3 vVec = normalize( viewVec );

	vec4 base = texture2D( tex0, texCoord );
	vec3 bump = normalize( texture2D( tex1, texCoord ).xyz * 2.0 - 1.0 );
//	bump = normalize( bump );

	float diffuse = max( dot(lVec, bump), 0.0 );

	float specular = pow( clamp( dot( reflect( -vVec, bump ), lVec ), 0.0, 1.0 ), gl_FrontMaterial.shininess );

	vec4 vAmbient = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;
	vec4 vDiffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * diffuse;
	vec4 vSpecular = gl_LightSource[0].specular * gl_FrontMaterial.specular *specular;
	gl_FragColor = vAmbient*base + (vDiffuse*base + vSpecular) * att;
}
