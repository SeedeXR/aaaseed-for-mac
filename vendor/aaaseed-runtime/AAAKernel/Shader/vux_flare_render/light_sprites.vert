
#define instances_width		vec3(aaa_vu_float[0], aaa_vu_float[1], aaa_vu_float[2])
#define instances_offset	vec3(aaa_vu_float[3], aaa_vu_float[4], aaa_vu_float[5])
#define instances_size	    aaa_vu_vec4[0].zw

mat2 rot_roll	= mat2( aaa_vu_vec4[0].x, aaa_vu_vec4[0].y, -aaa_vu_vec4[0].y, aaa_vu_vec4[0].x );
mat2 rot_x		= mat2( aaa_vu_vec4[1].x, aaa_vu_vec4[1].y, -aaa_vu_vec4[1].y, aaa_vu_vec4[1].x );

layout(location = 0) out vec2 out_texcoord;
layout(location = 1) out vec3 out_world_position;

const vec2 sprite_positions[6] =
{
	vec2( -1.0, -1.0 ),
	vec2(  1.0, -1.0 ),
	vec2(  1.0,  1.0 ),

	vec2( -1.0, -1.0 ),
	vec2(  1.0,  1.0 ),
	vec2( -1.0,  1.0 ),
};

const vec2 sprite_texcoord[6] =
{
	vec2( 0.0, 0.0 ),
	vec2( 1.0, 0.0 ),
	vec2( 1.0, 1.0 ),

	vec2( 0.0, 0.0 ),
	vec2( 1.0, 1.0 ),
	vec2( 0.0, 1.0) ,
};

int hash( in int n )
{
    n=(n<<13)^n;
    return (n*(n*n*15731+789221)+1376312589) & 0x7fffffff;
}

float hash_float( in int n )
{
    return float(hash(n)) / 1073741824.0;
}

vec3 hash_vec3( in int n)
{
	return vec3(hash_float(n), hash_float(n+1), hash_float(n+2)) - vec3(0.5);
}

void main(void)
{
	int vid = gl_VertexID;
	int iid = gl_InstanceID;

	vec3 sprites_width =  instances_width;
	vec3 sprites_offset = instances_offset;

	vec3 pos_sprite_world = hash_vec3(iid*115) * sprites_width + sprites_offset;

	out_world_position = pos_sprite_world;

//	vec3 pos_sprite_object = vec3( rot_roll * (sprite_positions[vid].xy * instances_size), 0.0 );
	vec3 pos_sprite_object = vec3( sprite_positions[vid].xy * instances_size, 0.0 );
	pos_sprite_object.xy = rot_roll * pos_sprite_object.xy;
	pos_sprite_object.yz = rot_x * pos_sprite_object.yz;

	pos_sprite_object = mat3(aaa_cam.view_inverse) * pos_sprite_object;
	pos_sprite_world += pos_sprite_object;

	gl_Position = aaa_cam.view_projection * vec4(pos_sprite_world, 1.0);

	out_texcoord = sprite_texcoord[vid];
}