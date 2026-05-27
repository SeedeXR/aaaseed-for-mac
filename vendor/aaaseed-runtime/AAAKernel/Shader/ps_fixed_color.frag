// GLSL fragment shader - Apply Texture
// for Aaaseed
// Author : Franz Hildgen

in float alpha;

void main (void)
{
    vec4 color;

    color = gl_Color;

    color += gl_SecondaryColor;
    color = clamp(color, 0.0, 1.0);

	color.a = alpha;

    gl_FragColor = color;
}

