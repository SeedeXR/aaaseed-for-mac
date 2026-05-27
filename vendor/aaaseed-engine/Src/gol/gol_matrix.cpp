#include "gol/gol_matrix.h"
#include "err.h"

namespace GOL::matrix
{

MODE						matrix_storage::mode_cur	= MODE::MODEL_VIEW;			// Current Matrix Mode

glm::mat4					matrix_storage::modelview	= aaa::matrix::identity;	// Current ModelView Matrix
glm::mat4					matrix_storage::projection	= aaa::matrix::identity;	// Current Projection Matrix
glm::mat4					matrix_storage::texture		= aaa::matrix::identity;	// Current Texture Matrix

std::stack< glm::mat4>		matrix_storage::stack_modelview		;					// Modelview matrix stack
std::stack< glm::mat4>		matrix_storage::stack_projection	;					// Projection matrix stack
std::stack< glm::mat4>		matrix_storage::stack_texture		;					// Texture matrix stack

#if GOL_MATRIX_CACHE()
glm::mat4 *					matrix_storage::cur			= &matrix_storage::modelview;
std::stack< glm::mat4> *	matrix_storage::stack_cur	= &matrix_storage::stack_modelview;
#endif

C_PCHAR_C get_matrix_mode_str( MODE mode )
{
	switch( mode )
	{
	case MODE::MODEL_VIEW:	return "ModelView";
	case MODE::PROJECTION:	return "Projection";
	case MODE::TEXTURE:		return "Texture";
	default:				return "Undefined";
	}
}
void print_matrix_stack_empty()
{
	debug_break( "Matrix %s Stack Empty: can't Pop", get_matrix_mode_str( matrix_storage::mode_cur ) );
}

#if GOL_MATRIX_CACHE()
void	set_modelview()
{
	glMatrixMode( GL_MODELVIEW );
	matrix_storage::mode_cur	= MODE::MODEL_VIEW;
	matrix_storage::cur			= &matrix_storage::modelview;
	matrix_storage::stack_cur	= &matrix_storage::stack_modelview;
//		DBG_PRINT_STRING( "switch mode to %s ", get_matrix_mode_str( matrix_storage::mode_cur ) );
}
void	set_projection()
{
	glMatrixMode( GL_PROJECTION );
	matrix_storage::mode_cur	= MODE::PROJECTION;
	matrix_storage::cur			= &matrix_storage::projection;
	matrix_storage::stack_cur	= &matrix_storage::stack_projection;
//		DBG_PRINT_STRING( "switch mode to %s ", get_matrix_mode_str( matrix_storage::mode_cur ) );
}
void	set_texture()
{
	glMatrixMode( GL_TEXTURE );
	matrix_storage::mode_cur	= MODE::TEXTURE;
	matrix_storage::cur			= &matrix_storage::texture;
	matrix_storage::stack_cur	= &matrix_storage::stack_texture;
//		DBG_PRINT_STRING( "switch mode to %s ", get_matrix_mode_str( matrix_storage::mode_cur ) );
}
void	set_matrix_mode( GLenum CONST gl_mode )
{
	glMatrixMode( gl_mode );
	switch( gl_mode )
	{
	case GL_MODELVIEW:	matrix_storage::mode_cur	= MODE::MODEL_VIEW;
						matrix_storage::cur			= &matrix_storage::modelview;
						matrix_storage::stack_cur	= &matrix_storage::stack_modelview;
						break;
	case GL_PROJECTION:	matrix_storage::mode_cur	= MODE::PROJECTION;
						matrix_storage::cur			= &matrix_storage::projection;
						matrix_storage::stack_cur	= &matrix_storage::stack_projection;
						break;
	case GL_TEXTURE:	matrix_storage::mode_cur	= MODE::TEXTURE;
						matrix_storage::cur			= &matrix_storage::texture;
						matrix_storage::stack_cur	= &matrix_storage::stack_texture;
						break;
	}
//		DBG_PRINT_STRING( "switch mode to %s ", get_matrix_mode_str( matrix_storage::mode_cur ) );
}
#endif

void	set_ortho_delta( REAL CONST sx, REAL CONST sy, REAL CONST dx, REAL CONST dy )
{
	set_projection();
	load_identity();
	set_ortho( -dx, sx+dx, -dy, sy+dy,-15, 15 );	//todo -15, 15 should not be imposed here 

	set_modelview();
	load_identity();
}

void	set_ortho(					REAL CONST sx, REAL CONST sy )	{	set_ortho_delta( sx, sy, 0., 0. );			}
void	set_ortho_with_secu_margin( REAL CONST sx, REAL CONST sy )	{	set_ortho_delta( sx, sy, REAL(sx/16.), REAL(sy/16.) );	}

void	print_matrix_double( DOUBLE* mat44 )
{
	for( INT32 i = 0; i < 4; ++i )
	{
		for( INT32 j = 0; j < 4; ++j )
			PRINT_STRING( "\t%f", mat44[i*4+j] );
		PRINT_STRING( "\n" );
	}
}
void	print_matrix_fp32( FP32* mat44 )
{
	for( INT32 i = 0; i < 4; ++i )
	{
		for( INT32 j = 0; j < 4; ++j )
			PRINT_STRING( "\t%f", mat44[i*4+j] );
		PRINT_STRING( "\n" );
	}
}
void	print_matrix_modelview()
{
	FP32* p_mat = (FP32*) get_matrix_modelview_pt();
	print_matrix_fp32( p_mat );
}
void	print_matrix_projection()
{
	FP32* p_mat = (FP32*) get_matrix_projection_pt();
	print_matrix_fp32( p_mat );
}
void	print_matrix_texture()
{
	FP32* p_mat = (FP32*) get_matrix_texture_pt();
	print_matrix_fp32( p_mat );
}

void	print_matrix( CONST glm::mat4 & mat44 )
{
	for ( INT32 i = 0; i < 4; ++i )
	{
		for ( INT32 j = 0; j < 4; ++j )
		{
			PRINT_STRING( "\t%f", mat44[ i ][ j ] );
		}
		PRINT_STRING( "\n" );
	}
}
void	print_matrix_modelview_glm()
{
	print_matrix( matrix_storage::modelview );
}
void	print_matrix_projection_glm()
{
	print_matrix( matrix_storage::projection );
}
void	print_matrix_texture_glm()
{
	print_matrix( matrix_storage::texture );
}
/*
void	compare_matrix_projection()
{
	float	mat44[ 16 ];
	get_matrix_projection( mat44 );
	if( std::memcmp( &matrix_storage::MatProjection[ 0 ], mat44, 16 * sizeof( float ) ) == 0 )
	{
		PRINT_STRING( "Projection Matrices are equal\n" );
	}
	else
	{
		ERR_PRINT_STRING( "Projection Matrices are not equal\n" );
		ERR_PRINT_STRING( "Projection GLM matrix :\n" );
		print_matrix( matrix_storage::MatProjection );
		ERR_PRINT_STRING( "Projection OpenGL matrix :\n" );
		print_matrix_fp32( mat44 );
	}
}
void	compare_matrix_modelview()
{
	float	mat44[ 16 ];
	get_matrix_modelview( mat44 );
	if( std::memcmp( &matrix_storage::MatModelView[ 0 ], mat44, 16 * sizeof( float ) ) == 0 )
	{
		PRINT_STRING( "Modelview Matrices are equal\n" );
	}
	else
	{
		ERR_PRINT_STRING( "Modelview Matrices are not equal\n" );
		ERR_PRINT_STRING( "Modelview GLM matrix :\n" );
		print_matrix( matrix_storage::MatModelView );
		ERR_PRINT_STRING( "Modelview OpenGL matrix :\n" );
		print_matrix_fp32( mat44 );
	}
}
*/

}	//namespace GOL
