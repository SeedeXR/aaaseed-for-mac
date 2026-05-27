// GLSL vertex shader
// Lattice Shader


vec3	LightPosition = aaa_vu_vec4[0].xyz;
vec3	LightColor = aaa_vu_vec4[1].rgb;
vec3	EyePosition = aaa_vu_vec4[2].xyz;
vec3	Specular = aaa_vu_vec4[3].rgb;
vec3	Ambient = aaa_vu_vec4[4].rgb;
float	Kd;

out vec3  DiffuseColor;
out vec3  SpecularColor;

void main(void)
{
    vec3 ecPosition = vec3 (gl_ModelViewMatrix * gl_Vertex);
    vec3 tnorm      = normalize(gl_NormalMatrix * gl_Normal);
    vec3 lightVec   = normalize(LightPosition - ecPosition);
    vec3 viewVec    = normalize(EyePosition - ecPosition);
    vec3 Hvec       = normalize(viewVec + lightVec);

    float spec = abs(dot(Hvec, tnorm));
    spec = pow(spec, 16.0);

    DiffuseColor    = LightColor * vec3 (Kd * abs(dot(lightVec, tnorm)));
    DiffuseColor    = clamp(Ambient + DiffuseColor, 0.0, 1.0);
    SpecularColor   = clamp((LightColor * Specular * spec), 0.0, 1.0);

    gl_TexCoord[0]  = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position     = ftransform();
}
