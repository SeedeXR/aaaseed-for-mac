
/*
kommentar
prinzipieller blur-shader. fehlt das wrap.

*/

uniform float mxw_vertslider_brightness_0x0_2x0_1x0_mxw;


vec4 c1 = vec4(0);
vec2 move;
uniform sampler2D texUnit;
vec2 texCoord;

void dolookup(float i)
{
   move.x=-0.0;     move.y=+i;      c1 += texture2D(texUnit, texCoord+move);
   move.x=+i;       move.y=+0.0;    c1 += texture2D(texUnit, texCoord+move);
   move.x=+0.0;     move.y=-i;      c1 += texture2D(texUnit, texCoord+move);
   move.x=-i;       move.y=-0.0;    c1 += texture2D(texUnit, texCoord+move);
}


void main(void)
{
   texCoord = gl_TexCoord[0].xy;

   float i = 1.0/512.0;
   float j = 1.0*mxw_vertslider_brightness_0x0_2x0_1x0_mxw;

   move.x=-i;move.y=+i; c1 = texture2D(texUnit, texCoord+move);
   move.x=+i;move.y=+i; c1 += texture2D(texUnit, texCoord+move);
   move.x=+i;move.y=-i; c1 += texture2D(texUnit, texCoord+move);
   move.x=-i;move.y=-i; c1 += texture2D(texUnit, texCoord+move);

   i+=i*j;dolookup(i);

   vec4 vec4col = c1*0.17;
    vec4col.a = 1.0;
   gl_FragColor = vec4col;
}