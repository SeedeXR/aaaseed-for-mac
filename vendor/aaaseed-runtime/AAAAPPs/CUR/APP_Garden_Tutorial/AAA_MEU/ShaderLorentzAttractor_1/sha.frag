
#if USE_AAA_UNIFORM == 1
#else
#endif


#define time    aaa_fu_float[0]


layout(binding = 0) uniform sampler2D g_input_texture_0;
layout(binding = 1) uniform sampler2D g_input_texture_1;
layout(binding = 2) uniform sampler2D g_input_texture_2;
layout(binding = 3) uniform sampler2D g_input_texture_3;


layout(location = 0) in vec4 layer_color;


layout(location = 0) out vec4 out_result;

void main(void)
{
    vec2 uv = gl_PointCoord;
    vec2 uv0 = uv-.5;
    float d = dot( uv0, uv0 );
    if( d > .25 )
        discard;

    float g = 1. - max( uv0.x, uv0.y ) * 2;
    out_result = vec4( vec3(g), 1 ) * layer_color;
}


/*#if USE_AAA_UNIFORM == 1
#else
#endif

#define time    aaa_fu_float[0]

layout(binding = 0) uniform sampler2D g_input_texture_0;
layout(binding = 1) uniform sampler2D g_input_texture_1;
layout(binding = 2) uniform sampler2D g_input_texture_2;
layout(binding = 3) uniform sampler2D g_input_texture_3;

layout(location = 0) in vec4 layer_color;

layout(location = 0) out vec4 out_result;

void main(void)
{
    vec2 uv = gl_PointCoord;
    vec2 uv0 = uv - 0.5;
    float d = dot(uv0, uv0);

    // Si les coordonnées sont en dehors du cercle, on les ignore
    if (d > 0.2)
        discard;

    // Calculer une couleur de fond graduelle basée sur la distance au centre du cercle
    float gradient = 1.0 - max(uv0.x, uv0.y) * 2.0;

    // Définir la couleur de fond avec le gradient et la couleur de la couche
    out_result = vec4(gradient * layer_color.rgb, 1.0);

    // Ajouter un point rouge qui circule le long de la courbe
    float t = fract(0.2*time);  // Valeur fractionnaire du temps

    // Position du point rouge sur le cercle
    vec2 redPointPos = vec2(cos(t * 2.0 * PI), sin(t * 2.0 * PI)) * 0.5 + 0.5;

    // Distance au point rouge
    float distToPoint = length(uv - redPointPos);

    // Rayon du point rouge
    float redPointRadius = 0.5;

    // Si la distance est inférieure au rayon du point rouge, colorer en rouge
    if (distToPoint < redPointRadius) {
        out_result.rgb = vec3(1.0, 0.0, 0.0);  
    }
}*/