
in ST_AAA_BV BV;

uniform sampler2D   aaa_tex2d[4];


CONST vec2 strenght          = aaa_fu_vec4[0].xy;
CONST vec2 scale             = aaa_fu_vec4[0].zw;
CONST vec4 step              = aaa_fu_vec4[1];

CONST float angle            = aaa_fu_float[8] * PI2;


void main()
{
     vec2 uv = BV.tex_coor[0].st;
     vec2 duv;

     vec4 col;

     if( aaa_fu_int[0]>1 )
     {
     // rotate
          float si = sin( angle );
	     float co = cos( angle );
	     uv = mat2( co, -si, si, co ) * (uv - .5 ) + .5;

          if( aaa_fu_int[0]==2 )
          {
               //uv *= factor;
               duv = sin( PI * scale * uv );
               duv = pow( abs(duv), vec2(4.) );
          }
          else if( aaa_fu_int[0]==3 )
          {
               duv = fract( scale * uv );
               duv = (1. - smoothstep( step.xy, step.zw, duv) ) * duv;
          }
          duv *= strenght / scale;
          //uv = clamp_01( uv );
          // uv = clamp( uv, .000, 1. );
          uv -= duv;
     // rotate back
          uv = mat2( co, si, -si, co ) * (uv - .5 ) + .5;

          col = texture2D( aaa_tex2d[0], uv );
     }
     else
     {
          col = texture2D( aaa_tex2d[0], uv );
          //col.rgba = vec4( uv, 1, 1 );
     }

     col.rgb = vec3(1) - col.rgb;
     //Apply slider overlay
     gl_FragColor = col;
}