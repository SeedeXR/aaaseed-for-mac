#include "bdd_draw_instance.h"
#include "draw/render.h"
#include "gol/gol_draw.h"
#include "gol/gol_shader.h"

FACTORY_CREATE_PROP_V1( c_bdd_draw_instance, bdd_draw_instance, Draw instanced, draw_instance, sub_menu = "GOL"; );

namespace	n_bdd_draw_instance
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 3 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	= BASE_PARAM_NB
									+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		
		PARAM_DEF_BOOL_ON(			draw_mesh		)

		PARAM_DEF_INT32_POS_ONE(	vertex_nb		)
		PARAM_DEF_INT32_POS_ONE(	instance_nb		)
	};
}

CONSTRUCTOR_CREATE(c_bdd_draw_instance)
{
	init();
}

EMPTY_DESTRUCTOR(c_bdd_draw_instance)

void	c_bdd_draw_instance::param_init_pt()
{
	INT32	h = param_init_pt_geo();
	
	param_set_pt(		h, _b_draw_ui		);
	param_set_pt(		h, _vertex_nb_ui	);

	param_set_pt(		h, _instance_nb_ui	);

	err_param_init_pt(h);
}

void c_bdd_draw_instance::init()
{
	param_init_with( n_bdd_draw_instance::param, n_bdd_draw_instance::PARAM_NB_MAX );
}

void c_bdd_draw_instance::update()
{
}

// debugging
//namespace {
//	GLuint dummy_vbo = 0;
//}

void c_bdd_draw_instance::draw()
{
	if( _b_draw_ui && _instance_nb_ui > 0 && _vertex_nb_ui > 0 )
	{
		GOL::unbind_vao();

		GLenum prim = c_render::get_cur()->get_draw_primitive();
#if AAA_DEBUG()
		if( prim > GL_POLYGON )
		{
			err_print( "%s() primitive is %d : Unknowned.", __FUNCTION__, prim );
			return;
		}
		if( GOL::get_program_used() == 0 )
		{
			err_print( "%s() no glsl program binded.", __FUNCTION__ );
			return;
		}
#endif
		// debugging
		//if( dummy_vbo == 0 )
		//{
		//	// Create a dummy buffer (do this once during initialization)
		//	glGenBuffers(1, &dummy_vbo);
		//	glBindBuffer(GL_ARRAY_BUFFER, dummy_vbo);
		//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4, nullptr, GL_STATIC_DRAW); // 3 floats (vec3)
		//}

		// Before rendering, bind the dummy buffer to the attribute location
		//glEnableVertexAttribArray(1); // location = 1
		//glBindBuffer( GL_ARRAY_BUFFER, dummy_vbo );
		//glVertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, 0, nullptr );

		//todo here a shader with a used access to buffer defined as: layout(location = 0) in vec3 in_position; will crash the app
		if( _instance_nb_ui == 1 )
			GOL::draw_arrays( prim, _vertex_nb_ui );
		else
			GOL::draw_arrays_instanced( prim, _vertex_nb_ui, _instance_nb_ui );

		// debugging
		//glDisableVertexAttribArray(1);
	}
}

