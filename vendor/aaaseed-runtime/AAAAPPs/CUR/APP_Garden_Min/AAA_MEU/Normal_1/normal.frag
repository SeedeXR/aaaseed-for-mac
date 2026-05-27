
out ST_AAA_BV BV;

uniform sampler2D   aaa_tex2d[4];


float factor =  aaa_fu_float[0];
void main()
{
    vec2 frag_coord = BV.tex_coor[0].xy;
    vec2 ts = textureSize( aaa_tex2d[0], 0 );
    vec4 col = texture2D( aaa_tex2d[0], frag_coord );

   // vec4 sliderVal = aaa_fu_vec4[0];
    //vec2 m = vec2( aaa_fu_float[1], aaa_fu_float[2] );
    //vec3 rd = normalize( vec3( (frag_coord-.5)*2., aaa_fu_float[5]) );
    float g = compute_gray(col.xyz);
    vec3 grad;
    grad.xy = vec2( dFdx(g), dFdy(g) );


    if( aaa_fu_int[0]==-1 )
    {
        //col.rg = abs(col.rg) * .5   ;
        col.rg = (col.rg + 1.) * .5  ;
        col.b = 0.;
        //col.rg = abs(col.rg);
        //col.b = .1;
        //col = normalize( col ) * .5 + .5;
        col.a = 1.;
    }
    else if( aaa_fu_int[0]==0 )
    {
         col.rgb = vec3( grad.rg*5.+ .5   , 0 );
    }
    else if( aaa_fu_int[0]==1 )
    {
        g = pow( dot( grad.xy, grad.xy ), .5 ) * factor;
        col.rgb     = vec3( g, g, g );
    }
     else if( aaa_fu_int[0]==2 )
    {
        g = pow( dot( grad.xy, grad.xy ), .5 ) * factor;
        col.rgb     += vec3( g, g, g ) * pow( col.a, 1.5 );
    }
     else if( aaa_fu_int[0]==3 )
    {
        grad.z = .01;
        grad = normalize( grad ) * .5 + .5;
        col.rgb = grad;
    }
    else if( aaa_fu_int[0]==4  )
    {
        grad.z = .4 ;
        g = pow( dot( grad.xy, grad.xy ), .5 ) * factor;
        grad = normalize( grad ) * .2 + .5;
        col.rgb = texture2D( aaa_tex2d[1], grad.xy ).rgb * .1
                 + texture2D( aaa_tex2d[2], frag_coord ).rrr * vec3(.5,.5,.8)  *4.;
    }
    else
    {
        grad.z = .01;
        grad.xy = normalize( grad.xy ) * .5 + .5;
        col.rgb = texture2D( aaa_tex2d[1], grad.xy ).rgb;
    }
    //col.b = 0.;

    //Apply slider overlay
    gl_FragColor = col;
}