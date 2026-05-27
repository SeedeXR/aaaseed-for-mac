

/*

mirrors along horizontal axis

note: the following variables
uniform float mxw_maxU;
uniform float mxw_maxV;

are provided by the framework

*/


uniform float mxw_maxV;
uniform sampler2D texUnit;
uniform float mxw_vertslider_mirror_0x0_1x0_0x5_mxw;
void main(void)
{

    float f = mxw_vertslider_mirror_0x0_1x0_0x5_mxw * mxw_maxV;
    vec2 texCoord = gl_TexCoord[0].xy;
    texCoord.y = texCoord.y < f ? texCoord.y : f - (texCoord.y-f) ;

    vec4 c = texture2D(texUnit, texCoord);
    gl_FragColor = c;
}