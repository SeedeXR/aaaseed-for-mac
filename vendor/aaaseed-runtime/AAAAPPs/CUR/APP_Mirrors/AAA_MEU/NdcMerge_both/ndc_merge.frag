
uniform sampler2D   aaa_tex2d[4];

CONST int what            = aaa_fu_int[0];

CONST float th_alpha      = aaa_fu_float[0];
CONST float th            = aaa_fu_float[1];

//   du,  dv,  -du, 0 
CONST vec4 duv = vec4( aaa_fu_vec4[0].zw, -aaa_fu_vec4[0].z, 0 ) * 1;   


layout(location = 0) in vec2 in_texcoord;


bool test( sampler2D tex, vec2 uv, float th )
{
     if(       texture2D( tex, uv + duv.xw ).a < th 
          ||   texture2D( tex, uv - duv.xw ).a < th
          ||   texture2D( tex, uv + duv.wy ).a < th
          ||   texture2D( tex, uv - duv.wy ).a < th
          ||   texture2D( tex, uv + duv.xy ).a < th
          ||   texture2D( tex, uv - duv.xy ).a < th
          ||   texture2D( tex, uv + duv.zy ).a < th
          ||   texture2D( tex, uv - duv.zy ).a < th
     )
          return false;
     return true;
}

void main()
{
    	vec2 uv = in_texcoord;

     vec4 a = texture2D( aaa_tex2d[0], uv );
     vec4 b = texture2D( aaa_tex2d[1], uv );
     vec4 col;

     if( what < 2 )
     {
          if( what==0 )
          {    // A
               if( a.a <= th_alpha )
                   discard;
               col = a;
               //col.a = 1;
              // col = vec4(1);
          }
          else
          {    // B
               if( b.a <= th_alpha )
                    discard;
               col = b;
          }
     }
     else
     {
          if( a.a <= th_alpha && b.a <= th_alpha )
               discard;
          if( what == 8 )
          {    // Debug
               float d = th;
               if( a.r > b.r + d  )
                    col.rgb = vec3( a.r, 0, 0 );
               else if( a.r < b.r - d  )
                    col.rgb = vec3( 0, a.r, 0 );
               else
                    col.rgb = vec3( .5 );
               col.a = 1;
          }
          else if( what == 7 )
          {    // abs(A-B)+
               col = abs(a-b);
               if( col.r <= th )
                    discard;
               col = vec4( max(a,b) );
               //col.a = min( a.a, b.a );
          }
          else
          {
               switch( what )
               {
               case 2:   col = min(a,b);     break;
               case 3:   col = max(a,b);     break;
               case 4:   col = a-b;          break;
               case 5:   col = b-a;          break;    
               case 6:   col = abs(a-b);     break;
               }
               if( col.r <= th && col.g <= th && col.b <= th )
                    discard;
               col.a = 1;
          }
     }



     // if( !test( aaa_tex2d[0], uv, .1 ) )
     //      discard;
     // if( !test( aaa_tex2d[1], uv, .1 ) )
     //      discard;

     //Apply slider overlay
     gl_FragColor = col;
}