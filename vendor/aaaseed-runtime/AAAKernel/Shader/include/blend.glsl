// --------------------------------------------------------------------------
// BLEND FUNCTIONS
// (OpenGL fixed path)

uniform int			aaa_tex_env_mode[4];

vec4 blend_low( in vec4 pixel_color, in vec4 color, in int tex_env_mode )
{
	if( tex_env_mode == 0 )  //	"GL_MODULATE"
		color *= pixel_color;
	else if( tex_env_mode == 4 )	//"GL_DECAL"
		{
		vec3	col = mix( color.rgb, pixel_color.rgb, pixel_color.a );
		color = vec4( col, color.a );
		}
	else if( tex_env_mode == 3 )	//"GL_BLEND"
	{
	//	in 330 gl_TextureEnvColor is not there anymore
		vec3	col = mix( color.rgb, gl_TextureEnvColor[0].rgb, pixel_color.rgb );
		color = vec4( col, color.a * pixel_color.a );
	}
	else if( tex_env_mode == 1 )	//"GL_REPLACE"
		color = pixel_color;
	else if( tex_env_mode == 2 )	//"GL_ADD"
	{
		color.rgb += pixel_color.rgb;
		color.a *= pixel_color.a;
		color = clamp( color, 0.0, 1.0 );
	}
// AAASeed don't do it anymore
//	else if( tex_env_mode == 5 )	//"GL_COMBINE"
//		color *= pixel_color;
	return color;
}
vec4 blend( in vec4 pixel_color )
{
	return blend_low( pixel_color, gl_Color, aaa_tex_env_mode[0] )
}
vec4 blend_fs( in vec4 pixel_color )
{
	return blend_low( pixel_color, fs_in.color, aaa_tex_env_mode[0] )
}
