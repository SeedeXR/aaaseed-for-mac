#if USE_AAA_UNIFORM == 1
#else
#endif

layout(binding = 0) uniform sampler2D g_input_texture_0;

layout(location = 0) in vec2 uv;
layout(location = 1) in vec4 layer_color;
layout(location = 0) out vec4 fragColor;

// AAASeed float uniforms mapping
CONST float center_u    = aaa_fu_vec4[0].x;
CONST float center_v    = aaa_fu_vec4[0].y;
CONST float angle       = aaa_fu_vec4[0].z;
CONST float half_length = aaa_fu_vec4[0].w;

CONST float strength    = aaa_fu_float[0];
CONST float radius      = aaa_fu_float[1];
CONST float smooth_type = aaa_fu_int[0]; // 0,1,2,3

CONST float inv_radius2 = 1.0 / (radius * radius + 1e-6);

CONST vec2 center = vec2(center_u, center_v);
CONST vec2 dir = vec2(cos(angle), sin(angle));
CONST vec2 a = center - dir * half_length;
CONST vec2 b = center + dir * half_length;
CONST vec2 ab = b - a;
CONST float ab_len2 = dot(ab, ab);

// helper function: projection of a point on a segment
vec2 project_point_on_segment(vec2 p, vec2 a, vec2 b, vec2 ab, float ab_len2)
{
	if(ab_len2 <= 0.0) return a;
	float t = dot(p - a, ab) / ab_len2;
	t = clamp(t, 0.0, 1.0);
	return a + ab * t;
}

void main(void)
{
	vec2 proj = project_point_on_segment(uv, a, b, ab, ab_len2);
	vec2 perp = uv - proj;
	float dist = length(perp);

	vec2 perp_dir = (dist > 1e-6) ? (perp / dist) : vec2(0.0);

	float fall;

	if(smooth_type == 0.0) // brutal
	{
		fall = (dist < radius) ? 1.0 : 0.0;
	}
	else if(smooth_type == 1.0) // clamp
	{
		fall = clamp(1.0 - dist * inv_radius2, 0.0, 9.0);
	}
	else if(smooth_type == 2.0) // step
	{
		fall = smoothstep(radius, 0.0, dist);
	}
	else // gauss
	{
		fall = exp(- (dist * dist) * inv_radius2);
	}	
	float factor = 1.0 + strength * fall;
	vec2 new_uv = proj + perp_dir * (dist * factor);

	vec4 col = texture(g_input_texture_0, clamp(new_uv, vec2(0.0), vec2(1.0))).rgba;
	fragColor = col * layer_color;
}
