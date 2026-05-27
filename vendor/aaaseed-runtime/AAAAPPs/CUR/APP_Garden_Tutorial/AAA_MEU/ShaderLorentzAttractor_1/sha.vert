#define time        aaa_vu_float[0]
#define nb_over     aaa_vu_float[1]
#define speed 		aaa_vu_float[2]

//#define sigma aaa_vu_float[4]
//define rho aaa_vu_float[5]
//define beta aaa_vu_float[6]

// Constantes pour les paramètres de l'attracteur de Lorenz
const float sigma = 10.0;      
const float rho = 28.0;         
const float beta = 8.0 / 3.0;   

//layout(location = 0) out vec3 vertexColor;
layout(location = 0) out flat vec4 layer_color;

void main(void)
{
    int id = gl_VertexID;
    float dt = 0.01;

    vec3 pp = {1,0,0};
    vec3 pn = pp;

    for (int i = 0; i < id; i++)
    {
        vec3 dis = vec3( sigma * (pp.y - pp.x),  pp.x * (rho - pp.z) - pp.y, pp.x * pp.y - beta * pp.z );
        pp += dis * dt;

        dis = vec3( sigma * (pp.y - pp.x),  pp.x * (rho - pp.z) - pp.y, pp.x * pp.y - beta * pp.z );
        pn = pp + dis * dt;
    }

    // Interpolation varies with time
    float interpolation = fract(speed*time);

    vec3 inter = mix(pp, pn, interpolation);

    gl_Position = gl_ModelViewProjectionMatrix * vec4(inter, 10.0);

  // gl_PointSize = 17.0;

    float id2 = floor(nb_over*fract(time));
    float id3 = mod(float(id), nb_over) / nb_over;

    int nb = int(1.0 + sin(0.005*time)/nb_over);

    int id4 = int(floor(nb_over*fract(time)));

    if( id < nb ) { layer_color = vec4(1.0, 0.0, 0.0, 1.0); } 
    else if (id > nb && id < 5.0*nb) { layer_color = vec4(1., 1., .0, 1.); }
    else if (id > 5.0*nb && id < 10.0*nb) { layer_color = vec4(0.0, 1.0, 0.0, 1.0); } 
    //else { layer_color = vec4(1.0, 1.0, 0.0, 1.0); }
    else { layer_color = vec4(0.0, 0.0, 0.0, 1.0); }
}
