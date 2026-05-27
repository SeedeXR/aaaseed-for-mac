//#version 330 compatibility


// --------------------------------------------------------------------------
// VARIABLES

in VS_out
{
	vec4			color;
} fs_in;

//	output
out		vec4	fragColor;

// --------------------------------------------------------------------------
// MAIN  FUNCTION
void main (void)
{
	//	discard;
	vec4 tex = fs_in.color;
	tex.r = 1.0;
	tex.a = 1.0;

	fragColor = tex;
}
