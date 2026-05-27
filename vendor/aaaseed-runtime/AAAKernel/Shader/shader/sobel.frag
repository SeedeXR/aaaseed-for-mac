
/*
horizontal sobel edge detection kernel

-1 -2 -1
+0 +0 +0
+1 +2 +1

*/

uniform sampler2D texUnit;

void main(void)
{
    vec2 texCoord;
    vec4 c1 = vec4(0);
    vec2 move;
    float i = 1.0/1024.0;
    texCoord = gl_TexCoord[0].xy;

    move.x=-i  ;move.y=+i; c1 -= 1.0*texture2D(texUnit, texCoord+move);
    move.x=+0.0;move.y=+i; c1 -= 2.0*texture2D(texUnit, texCoord+move);
    move.x=+i  ;move.y=+i; c1 -= 1.0*texture2D(texUnit, texCoord+move);

    move.x=-i;  move.y=-i; c1 += 1.0*texture2D(texUnit, texCoord+move);
    move.x=+0.0;move.y=-i; c1 += 2.0*texture2D(texUnit, texCoord+move);
    move.x=+i;  move.y=-i; c1 += 1.0*texture2D(texUnit, texCoord+move);


    	float b = (	c1.x*4.0/21.0+
				c1.y*16.0/21.0+
				c1.z*1.0/21.0);
   vec4 vec4col = vec4(b);
   vec4col.a = 1.0;

    gl_FragColor = vec4col;

}