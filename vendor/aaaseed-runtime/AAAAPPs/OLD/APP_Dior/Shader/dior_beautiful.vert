
out VS_out
{
	vec2	uv_geo;
	vec2	uv;
	vec4	color;
} vs_out;

// --------------------------------------------------------------------------
void main()
{
	vec4 vertex = gl_Vertex;

	vs_out.uv_geo = vec2( (vertex.x)/4.5, -(vertex.y)/9. ) + .5;

	gl_Position = gl_ModelViewProjectionMatrix * vertex;

	vs_out.color = gl_Color;

	vs_out.uv = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy;
}