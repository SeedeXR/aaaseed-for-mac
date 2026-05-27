// GLSL vertex shader
// Lattice Shader

uniform vec4	vu_vec4_01;
uniform vec4	vu_vec4_02;
uniform vec4	vu_vec4_03;
uniform vec4	vu_vec4_04;
uniform vec4	vu_vec4_05;
//uniform float	vu_float[6];

vec3	LightPosition = vu_vec4_01.xyz;
vec3	LightColor = vu_vec4_02.rgb;
vec3	EyePosition = vu_vec4_03.xyz;
vec3	Specular = vu_vec4_04.rgb;
vec3	Ambient = vu_vec4_05.rgb;
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
