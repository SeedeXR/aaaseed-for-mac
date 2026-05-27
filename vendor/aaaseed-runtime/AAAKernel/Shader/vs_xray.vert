//Vertex Shader X-Ray

// vertex to fragment shader io
out vec3 P;
out vec3 N;
out vec3 I;
out vec4 Cs;


void main(void)
{
 	// position in eye space
    P =  vec3( gl_ModelViewMatrix * gl_Vertex );

    // position in clip space
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

	// normal transform (transposed model-view inverse)
    N = gl_NormalMatrix * gl_Normal;

   	// incident vector
    //I = P - vec3(0);
  	I = P;

  	// color
    Cs = gl_Color;
}
