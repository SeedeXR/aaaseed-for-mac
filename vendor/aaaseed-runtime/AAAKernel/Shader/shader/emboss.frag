
/*
To change the direction, change the relative positions of the kernel elements.
Instead of

   -2  -2  0
   -2   6  0
    0   0  0

and its partner . . . 		Try

    0   0   0
    0   6  -2
    0  -2  -2

and its partner.


To increase the emboss effect increase the magnitude of the kernel elements.
Instead of

   -2  -2  0
   -2   6  0
    0   0  0

and its partner . . . 		Try

   -4  -4  0
   -4  12  0
    0   0  0

and its partner. Be sure the kernel elements sum to zero.

*/
uniform sampler2D texUnit;

void main(void)
{
    vec2 texCoord;
    vec4 c1 = vec4(0);
    vec2 move;
	/*
	looks only good on big screen
	*/
    float i = 1.0/1024.0;
    texCoord = gl_TexCoord[0].xy;
	/*
	look up the texture
	*/
    move.x=-i  ;move.y=+i; c1 -= 2.0*texture2D(texUnit, texCoord+move);
    move.x=+0.0;move.y=+i; c1 -= 2.0*texture2D(texUnit, texCoord+move);
    //move.x=+i  ;move.y=+i; c1 -= 1.0*texture2D(texUnit, texCoord+move);

    move.x=-i  ;move.y=0.0; c1 -= 2.0*texture2D(texUnit, texCoord+move);
    move.x=+0.0;move.y=0.0; c1 += 6.5*texture2D(texUnit, texCoord+move);
    //move.x=+i  ;move.y=0; c1 -= 1.0*texture2D(texUnit, texCoord+move);

    //move.x=-i;  move.y=-i; c1 += 1.0*texture2D(texUnit, texCoord+move);
    //move.x=+0.0;move.y=-i; c1 += 2.0*texture2D(texUnit, texCoord+move);
    //move.x=+i;  move.y=-i; c1 += 1.0*texture2D(texUnit, texCoord+move);    
	/*	rgb ( 4-16-1 ) and enlighten a bit (divide by 10 not by 21 )	*/
    float b = (	c1.x*4.0/10.0+ c1.y*16.0/10.0+ c1.z*1.0/10.0);
	vec4 vec4col = vec4(b);
	vec4col.a = 1.0;
    gl_FragColor = vec4col;
}