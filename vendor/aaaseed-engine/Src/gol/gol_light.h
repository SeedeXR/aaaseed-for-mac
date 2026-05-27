
#ifdef AAA_GOL_LIGHT_H
#error "GOL_LIGHT_H included more than once."
#endif
#define AAA_GOL_LIGHT_H 1


#ifndef AAA_GOL_ATTRIB_H
#	include "gol_attrib.h"
#endif

namespace GOL	{

extern	void	init_light();
extern	void	print_light_implementation();

CONSTEXPR	INT32	LIGHT_NB = 8;
extern		INT32	light_nb;

extern	bool	b_light_on[LIGHT_NB];
extern	bool	b_color_material_state;
extern	GLenum	s_color_material_face;
extern	GLenum	s_color_material_mode;

//	FINLINE	INT32	get_light_gl_name(	INT32 CONST index )		{	return GL_LIGHT0 + index;	}
CONSTEXPR INT32 light_gl_name[LIGHT_NB] = 
	{	GL_LIGHT0,	GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7	};


FINLINE	void	lightv( GLenum CONST light_name, GLenum CONST pname, FP32 CONST * CONST params )
{
	glLightfv( light_name, pname, params );
}
extern	void	lightv( GLenum CONST light_name, GLenum CONST pname, DOUBLE CONST * CONST params );

//todoqq
//deal with the push attrib

//LIGHT
//
FINLINE	bool	is_light(			INT32 CONST index )		{	return b_light_on[index];	}
FINLINE	void	disable_light(		INT32 CONST index )
{
	if( is_state_cache_no() || is_light( index ) )	//	|| !b_lighting )
	{
		GOL::disable( light_gl_name[index] );
		b_light_on[index] = false;
		//GOOD_PRINT_STRING_INV( "LIGHT %d OFF", index );
	}
}
FINLINE	void	enable_light(		INT32 CONST index )
{
	if( is_state_cache_no() || !is_light( index ) )	//	&& b_lighting )
	{
		GOL::enable( light_gl_name[index] );
		b_light_on[index] = true;
		//GOOD_PRINT_STRING_INV( "LIGHT %d ON" , index );
	}
}
FINLINE	void	set_light(			INT32 CONST index, bool CONST b )
{
	if( b )
		enable_light( index );
	else
		disable_light( index );
}

FINLINE void light( GLenum CONST light, GLenum CONST pname, GLfloat CONST param )
{
	glLightf( light, pname, param );
}

//MATERIAL
//
//todo cache and opt
FINLINE	void	color_material( GLenum CONST face, GLenum CONST mode )
{
	// we should refine (deal with front and back)
	if( is_state_cache_no() || s_color_material_face != face || s_color_material_mode != mode )
	{
		s_color_material_face = face;
		s_color_material_mode = mode;
		glColorMaterial( face, mode );
	}
}

FINLINE	void	disable_color_material()
{
	if( is_state_cache_no() || b_color_material_state )
	{
		GOL::disable( GL_COLOR_MATERIAL );
		b_color_material_state = false;
	}
}
FINLINE	void	enable_color_material()
{
	if( is_state_cache_no() || !b_color_material_state )
	{
		GOL::enable( GL_COLOR_MATERIAL );
		b_color_material_state = true;
	}
}
FINLINE	void	set_color_material( bool CONST b )
{
	if( b )
		enable_color_material();
	else	
		disable_color_material();
}

//	FINLINE	bool	get_color_material()
//	{
//		return	b_color_material_state;
//	}
FINLINE	void	materialv( GLenum CONST face, GLenum CONST pname, FP32 CONST * params )
{
	glMaterialfv( face, pname, params );
}
FINLINE	void	material( GLenum CONST face, GLenum CONST pname, FP32 CONST value )
{
	glMaterialf( face, pname, value );
}

FINLINE	void	reset_light()
{
	for( INT32 i=0; i<LIGHT_NB; ++i )
	{
		b_light_on[i] = true;
		disable_light( i );
	}
	b_color_material_state = true;
	disable_color_material();
	s_color_material_face = -42;
	s_color_material_mode = -42;
}

//LIGHTING
//
extern	bool	b_lighting_allow_ui;

FINLINE	bool	is_lighting_allow()		{ return	b_lighting_allow_ui; }
extern	void	flip_lighting_allow();

extern	bool	b_lighting;
FINLINE	bool	is_lighting()			{ return	b_lighting; }

FINLINE	void	disable_lighting( void )
{
	if( is_state_cache_no() || b_lighting )
	{
		GOL::disable( GL_LIGHTING );
		b_lighting = false;
	}
}
FINLINE	void	enable_lighting( void )
{
	if( (is_state_cache_no() || !b_lighting) && b_lighting_allow_ui )
	{
		GOL::enable( GL_LIGHTING );
		b_lighting = true;
	}
}
FINLINE	void	set_lighting( bool CONST b )
{
	if( b )
		enable_lighting();
	else
		disable_lighting();
}
FINLINE void	reset_lighting()
{
	b_lighting = true;
	disable_lighting();
}
FINLINE	void	push_lighting()
{
	c_poper* poper = get_att_poper();
	poper->add_fn( is_lighting() ? enable_lighting : disable_lighting );	//potential state sync problem
}
FINLINE	void	push_lighting( bool CONST b )
{
	if( is_state_cache_no() || is_lighting() != b )
	{
		push_lighting();
		set_lighting( b );
	}
}

}	//namespace GOL

