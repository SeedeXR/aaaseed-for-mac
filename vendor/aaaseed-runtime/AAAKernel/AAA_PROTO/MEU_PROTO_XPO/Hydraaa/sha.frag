//	HYDRAAA placeholder fragment shader (overwritten by transpile_selected())
//	Default chain : osc(40,0.1,0.8).out()

#define time         aaa_fu_float[0]
#define mouse        vec2(aaa_fu_float[1], aaa_fu_float[2])
#define resolution   vec2(max(aaa_fu_float[3], 1.0), max(aaa_fu_float[4], 1.0))

layout(binding = 0) uniform sampler2D g_input_texture_0;

in VS_out
{
	vec4	color;
	vec2	tex_coor;
} vs_in;

out vec4 out_result;


vec4 osc(vec2 _st, float frequency, float sync, float offset) {
    vec2 st = _st;
    float r = sin((st.x-offset/frequency+time*sync)*frequency)*0.5  + 0.5;
    float g = sin((st.x+time*sync)*frequency)*0.5 + 0.5;
    float b = sin((st.x+offset/frequency+time*sync+1.570796)*frequency)*0.5  + 0.5;
    return vec4(r, g, b, 1.0);
}

void main(void)
{
	vec2 st00 = vs_in.tex_coor;
	vec4 c00 = osc(st00, 40.0, 0.1, 0.8);
	out_result = c00 * vs_in.color;
}
