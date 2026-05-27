
out float LightIntensity;
out vec3  MCposition;

uniform vec3  LightPos;	//fix this

uniform vec4 aaa_vu_vec4[8];

void main()
{
vec3	scale = aaa_vu_vec4[0].xyz * aaa_vu_vec4[1].w;
    vec3 ECposition = vec3(gl_ModelViewMatrix * gl_Vertex);
    MCposition      = vec3(gl_Vertex) * scale + aaa_vu_vec4[1].xyz;
    vec3 tnorm      = normalize(vec3(gl_NormalMatrix * gl_Normal));
    LightIntensity  = dot(normalize(LightPos - ECposition), tnorm);
    LightIntensity *= 1.5;
    gl_Position     = ftransform();
}