
#if USE_AAA_UNIFORM == 1
#else
#endif

#define MAX_DISTANCE 20.
#define MAX_STEPS    100
#define EPSILON        0.0001

#define time 		aaa_fu_float[0]

layout(binding = 0) uniform sampler2D g_input_texture_0;

layout(location = 0) in vec2 in_texcoord;	
layout(location = 1) in vec4 layer_color;

layout(location = 0) out vec4 out_result;


float sdf_sphere(vec3 p, vec3 center, float radius)
{
    return length(p - center) - radius;
}

float sdf_cube(vec3 p, vec3 center, float width)
{
    vec3 d = abs(p - center) - width * .5;
    
    float inside_distance = min(max(d.x, max(d.y, d.z)), 0.0);
    float outside_distance = length(max(d, 0.0));
    
    return inside_distance + outside_distance;
}

float sdf_intersect(float distA, float distB)
{
    return max(distA, distB);
}
float sdf_union(float distA, float distB)
{
    return min(distA, distB);
}
float sdf_difference(float distA, float distB)
{
    return max(distA, -distB);
}

float sdf_scene(vec3 p)
{
	float dist = MAX_DISTANCE;
	// dist = sdf_intersect(sdf_sphere(p, vec3(0.), 1.3), sdf_cube(p, vec3(0.), 2.));
	dist = sdf_cube(p, vec3(0.), 2.);
    // dist *= .8 + .15 * cos(time);
	dist = sdf_difference(dist, sdf_sphere(p, vec3(0.), 1.3));
    // dist *= .7 + .25 * sin(time);
	// dist = sdf_difference(dist, sdf_sphere(p, vec3(0., .4, 0.), .9));

	return dist;
}

vec3 noise(vec3 p)
{
    return EPSILON * normalize(vec3(cos(0.99 * time + 100 * p.x), sin(1.51 * time + 100 * p.y), cos(0.57 * time + 100 * p.z + 0.91)));
}

float ray_marching(vec3 eye, vec3 ray, float min_distance, float max_distance)
{
    float depth = min_distance;
    vec3 pos = eye + ray * depth;
	// for (int i = 0; i < MAX_STEPS; ++i) {
	// 	float dist = sdf_scene(eye + depth * ray);
	// 	if (dist <= EPSILON) 	   return depth;
	// 	if (dist >= MAX_DISTANCE) return max_distance;
	// 	depth += dist;
    //     ray += 3 * noise(ray);
	// }
	for (int i = 0; i < MAX_STEPS; ++i)
    {
		float dist = sdf_scene(pos);
		if( dist <= EPSILON )
            return depth;
		if( dist >= MAX_DISTANCE )
            return max_distance;
        ray = normalize(ray + 500 * noise(ray));
		pos += ray * dist;
        depth += dist;
	}
	return max_distance;
}

vec3 ray_direction(float fov, vec2 texcoord)
{
    vec2 xy = texcoord - .5;
    float z = 1. / tan(radians(fov) * .5);
    return normalize(vec3(xy, -z));
}

vec3 estimate_sdf_normal(vec3 p)
{
    return normalize(vec3(
        sdf_scene(vec3(p.x + EPSILON, p.y,           p.z))            - sdf_scene(vec3(p.x - EPSILON, p.y,           p.z)),
        sdf_scene(vec3(p.x,           p.y + EPSILON, p.z))            - sdf_scene(vec3(p.x,           p.y - EPSILON, p.z)),
        sdf_scene(vec3(p.x,           p.y,           p.z  + EPSILON)) - sdf_scene(vec3(p.x,           p.y,           p.z - EPSILON))
    ));
}

vec3 phong_contrib_for_light(vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye,
                          vec3 light_pos, vec3 light_intensity)
{
    vec3 N = estimate_sdf_normal(p);
    vec3 L = normalize(light_pos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));
    
    float dotLN = dot(L, N);
    float dotRV = dot(R, V);
    
    if (dotLN < 0.0) {
        // Light not visible from this point on the surface
        return vec3(0.0, 0.0, 0.0);
    } 
    
    if (dotRV < 0.0) {
        // Light reflection in opposite direction as viewer, apply only diffuse
        // component
        return light_intensity * (k_d * dotLN);
    }
    return light_intensity * (k_d * dotLN + k_s * pow(dotRV, alpha));
}

vec3 phong_illumination(vec3 k_a, vec3 k_d, vec3 k_s, float alpha, vec3 p, vec3 eye)
{
    const vec3 ambient_light = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambient_light * k_a;
    
    vec3 light1_pos = vec3(4.0 * sin(time),
                           2.0,
                           4.0 * cos(time));
    vec3 light1_intensity = vec3(0.4, 0.4, 0.4);
    
    color += phong_contrib_for_light(k_d, k_s, alpha, p, eye,
                                  light1_pos,
                                  light1_intensity);
    
    vec3 light2_pos = vec3(2.0 * sin(0.37 * time),
                           2.0 * cos(0.37 * time),
                           2.0);
    vec3 light2_intensity = vec3(0.4, 0.4, 0.4);
    
    color += phong_contrib_for_light(k_d, k_s, alpha, p, eye,
                                  light2_pos,
                                  light2_intensity);    
    return color;
}

mat4 view_matrix(vec3 eye, vec3 center, vec3 up) {
    // Based on gluLookAt man page
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(f, up));
    vec3 u = cross(s, f);
    return mat4(
        vec4(s, 0.0),
        vec4(u, 0.0),
        vec4(-f, 0.0),
        vec4(0.0, 0.0, 0.0, 1)
    );
}


CONST float u_mix_begin = 1. - aaa_fu_float[1];
CONST float u_mix_end   = u_mix_begin + .1;

void main(void)
{
    vec4 color = texture( g_input_texture_0, in_texcoord );
    //vec4 color = vec4( in_texcoord,0, 1 );
    if( in_texcoord.x < u_mix_end )
    {
        vec3 ray = ray_direction( 45., in_texcoord );
        // vec3 eye = vec3(5. * cos(time), 7. + 2. * cos(time * .989), 5. * sin(time));
        vec3 eye = vec3(5., 7., 5.);
        // vec3 eye = vec3(0, 2, 5);
        vec3 center = vec3(0., 0., 0.);

        mat4 view_to_world = view_matrix(eye, center, vec3(0.0, 1.0, 0.0));
        
        vec3 world_ray = (view_to_world * vec4(ray, 0.0)).xyz;
        float dist = ray_marching(eye, world_ray, 0., MAX_DISTANCE);

        // The closest point on the surface to the eyepoint along the view ray
        vec3 p = eye + dist * world_ray;
        vec4 color_ray;
        if( dist >= MAX_DISTANCE )
        {
            // Didn't hit anything
            color_ray = vec4( 0,0,0, 1 );
        }
        else
        {
            const vec3 K_a = vec3(0.2, 0.2, 0.2);
            const vec3 K_d = vec3(0.7, 0.2, 0.2);
            const vec3 K_s = vec3(1.0, 1.0, 1.0);
            const float shininess = 10.0;
        
            // vec4 color = vec4(1);
           color_ray = vec4( phong_illumination( K_a, K_d, K_s, shininess, p, eye ), 1 );
        }
        color = mix( color_ray, color, smoothstep( u_mix_begin, u_mix_end, in_texcoord.x ) );
    }

	// result
	// out_result = color;

	// color = abs(estimate_sdf_normal(p));

	out_result = color * layer_color;
	// out_result = vec4(in_texcoord, 0, 1);
	// out_result = layer_color;
}
