#include "gol/gol_light.h"
#include "math/v.h"


namespace GOL {

bool	b_light_on[LIGHT_NB];
GLenum	s_color_material_face;
GLenum	s_color_material_mode;

INT32	light_nb;

void	init_light()
{
	light_nb = get_integer( GL_MAX_LIGHTS );
	print_light_implementation();
	if( light_nb < LIGHT_NB )
	{
		ERR_PRINT_STRING( "AAASeed deal with %d lights, we have here only %d lights. it could be problematic.", LIGHT_NB, light_nb );
	}
	else if( LIGHT_NB < light_nb )
	{
		DBG_PRINT_STRING( "AAASeed deal with only %d lights, OpenGl here have more : %d lights, this supplement will be ignored.", LIGHT_NB, light_nb );
	}
}

void	print_light_implementation()
{
	GOOD_PRINT_STRING( "Number of lights : %d", light_nb );
}

void	lightv( GLenum CONST light_name, GLenum CONST pname, DOUBLE CONST * CONST params )
{
	FP32	f[4];
	cpy_v4( f, params );
	lightv( light_name, pname, f );
}

}	//namespace GOL
