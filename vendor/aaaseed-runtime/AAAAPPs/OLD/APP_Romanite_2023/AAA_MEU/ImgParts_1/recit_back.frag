
layout(binding = 0) uniform sampler2D input_sampler;	

layout(location = 0) out vec4 out_result;

//uniform sampler2D   aaa_tex2d[4];

in ST_AAA_BV BV;

void main(void)
{
    vec2 uv = BV.tex_coor[0].xy;
	float alpha = texture( input_sampler, uv ).r;
    //uv = (uv - .5) * 2.; 
    //alpha *= 1. - length(uv);
    //alpha = pow( alpha, 1.5 );
	//out_result = vec4( 243./255., 236./255., 221./255., alpha ) * BV.color;
	out_result = vec4( 1., 1., 1., alpha ) * BV.color;
}