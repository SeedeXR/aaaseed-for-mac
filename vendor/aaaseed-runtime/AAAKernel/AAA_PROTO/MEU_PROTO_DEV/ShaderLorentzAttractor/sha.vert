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

    float x = 1.;
    float y = 1.;
    float z = 1.;

    float next_x = x;
    float next_y = y;
    float next_z = z;

    for (int i = 0; i < id; i++) {
        float dx = sigma * (y - x) * dt;
        float dy = (x * (rho - z) - y) * dt;
        float dz = (x * y - beta * z) * dt;
        x += dx;
        y += dy;
        z += dz;

        dx = sigma * (y - x) * dt;
        dy = (x * (rho - z) - y) * dt;
        dz = (x * y - beta * z) * dt;
        next_x = x + dx;
        next_y = y + dy;
        next_z = z + dz;
    }

    // Interpolation varies with time
    float interpolation = fract(speed*time);

    float interp_x = mix(x, next_x, interpolation);
    float interp_y = mix(y, next_y, interpolation);
    float interp_z = mix(z, next_z, interpolation);

    vec4 pos = vec4(interp_x, interp_y, interp_z, 10.0);

    // gl_Position = aaa_cam.view_projection * pos;
    gl_Position = gl_ModelViewProjectionMatrix * pos;

    gl_PointSize = 17.0;

    float id2 = floor(nb_over*fract(time));
    float id3 = mod(float(id), nb_over) / nb_over;

    int nb = int(1.0 + sin(0.005*time)/nb_over);

    int id4 = int(floor(nb_over*fract(time)));

    if (id < nb) { layer_color = vec4(1.0, 0.0, 0.0, 1.0); } 
    else if (id > nb && id < 5.0*nb) { layer_color = vec4(1.0, 1.0, 0.0, 1.0); }
    else if (id > 5.0*nb && id < 10.0*nb) { layer_color = vec4(0.0, 1.0, 0.0, 1.0); } 
    //else { layer_color = vec4(1.0, 1.0, 0.0, 1.0); }
    else { layer_color = vec4(0.0, 0.0, 0.0, 1.0); }
}
