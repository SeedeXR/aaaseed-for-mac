
#ifdef AAA_GOL_MATRIX_H
#error "GOL_MATRIX_H included more than once."
#endif
#define AAA_GOL_MATRIX_H 1


#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif
#ifndef AAA_V_H
#	include "math/v.h"
#endif
#ifndef AAA_AAA_MATRIX_H
#	include "math/aaa_matrix.h"		//glm included here
#endif
#ifndef _STACK_
#	include <stack>
#endif


//todo we should have a local layer (or a flag) where we change local matrix on our side without send to opengl
// the send could be done with a validate() when need
namespace GOL::matrix	{

//MATRIX
//
// Fixed Pipeline Matrix Mode
enum class MODE
{
	MODEL_VIEW = 0,
	PROJECTION,
	TEXTURE
//	COLOR
};

#define GOL_MATRIX_CACHE()	1

class matrix_storage
{
public:
	static	MODE						mode_cur;			// Current Matrix Mode
	static	glm::mat4					modelview;			// Current ModelView Matrix
	static	glm::mat4					projection;			// Current Projection Matrix
	static	glm::mat4					texture;			// Current Texture Matrix
	static	std::stack< glm::mat4>		stack_modelview;	// Modelview matrix stack
	static	std::stack< glm::mat4>		stack_projection;	// Projection matrix stack
	static	std::stack< glm::mat4>		stack_texture;		// Texture matrix stack
#if GOL_MATRIX_CACHE()
	static	glm::mat4 *					cur;
	static	std::stack< glm::mat4> *	stack_cur;
#endif
};

extern C_PCHAR_C get_matrix_mode_str( MODE CONST mode );
extern void print_matrix_stack_empty();

FINLINE void	pop_push()
{
	glPopMatrix();
	glPushMatrix();
#if GOL_MATRIX_CACHE()
	if( !matrix_storage::stack_cur->empty() )
		*matrix_storage::cur = matrix_storage::stack_cur->top();
#else
	switch( matrix_storage::mode_cur )
	{
	case MODE::MODEL_VIEW:
		if( !matrix_storage::stack_modelview.empty() )
			matrix_storage::modelview = matrix_storage::stack_modelview.top();
		break;
	case MODE::PROJECTION:
		if( !matrix_storage::stack_projection.empty() )
			matrix_storage::projection = matrix_storage::stack_projection.top();
		break;
	case MODE::TEXTURE:
		if( !matrix_storage::stack_texture.empty() )
			matrix_storage::texture = matrix_storage::stack_texture.top();
		break;
	}
#endif
}

FINLINE void	push()
{
	glPushMatrix();
#if GOL_MATRIX_CACHE()
	matrix_storage::stack_cur->push( *matrix_storage::cur );
#else
	switch( matrix_storage::mode_cur )
	{
	case MODE::MODEL_VIEW:	matrix_storage::stack_modelview.push(	matrix_storage::modelview );	break;
	case MODE::PROJECTION:	matrix_storage::stack_projection.push(	matrix_storage::projection );	break;
	case MODE::TEXTURE:		matrix_storage::stack_texture.push(		matrix_storage::texture );		break;
	}
#endif
}

FINLINE void	pop()
{
	glPopMatrix();
#if GOL_MATRIX_CACHE()
	if( !matrix_storage::stack_cur->empty() )
	{
		*matrix_storage::cur = matrix_storage::stack_cur->top();
		matrix_storage::stack_cur->pop();
	}
	else
		print_matrix_stack_empty();
#else
	switch( matrix_storage::mode_cur )
	{
	case MODE::MODEL_VIEW:
		if( !matrix_storage::stack_modelview.empty() )
		{
			matrix_storage::modelview = matrix_storage::stack_modelview.top();
			matrix_storage::stack_modelview.pop();
		}
		else
		{
			ERR_PRINT_STRING( "Matrix Model Stack Empty: can't Pop" );
		}
		break;
	case MODE::PROJECTION:
		if( !matrix_storage::stack_projection.empty() )
		{
			matrix_storage::projection = matrix_storage::stack_projection.top();
			matrix_storage::stack_projection.pop();
		}
		else
		{
			ERR_PRINT_STRING( "Matrix Projection Stack Empty: can't Pop" );
		}
		break;
	case MODE::TEXTURE:
		if( !matrix_storage::stack_texture.empty() )
		{
			matrix_storage::texture = matrix_storage::stack_texture.top();
			matrix_storage::stack_texture.pop();
		}
		else
		{
			ERR_PRINT_STRING( "Matrix Texture Stack Empty: can't Pop" );
		}
		break;
	}
#endif
}

#if GOL_MATRIX_CACHE()
extern	void	set_matrix_mode( GLenum CONST gl_mode );
extern	void	set_modelview();
extern	void	set_projection();
extern	void	set_texture();
//	extern	void	set_matrix_color();			// not used in aaaseed
#else
FINLINE	void	set_matrix_mode( GLenum CONST gl_mode )
{
	glMatrixMode( gl_mode );
	switch( gl_mode )
	{
	case GL_MODELVIEW:	matrix_storage::mode_cur = MODE::MODEL_VIEW;	break;
	case GL_PROJECTION:	matrix_storage::mode_cur = MODE::PROJECTION;	break;
	case GL_TEXTURE:	matrix_storage::mode_cur = MODE::TEXTURE;		break;
	}
}
FINLINE	void	set_modelview()					{	set_matrix_mode( GL_MODELVIEW );	}
FINLINE	void	set_projection()				{	set_matrix_mode( GL_PROJECTION );	}
FINLINE	void	set_texture()					{	set_matrix_mode( GL_TEXTURE );		}
//	FINLINE	void	set_matrix_color()					{	set_matrix_mode( GL_COLOR );		}		// not used in aaaseed
#endif

FINLINE	void	load_identity()
{
	glLoadIdentity();
#if GOL_MATRIX_CACHE()
	*matrix_storage::cur = aaa::matrix::identity;
#else
	switch( matrix_storage::mode_cur )
	{
	case MODE::MODEL_VIEW:	matrix_storage::modelview	= aaa::matrix::identity;	break;
	case MODE::PROJECTION:	matrix_storage::projection	= aaa::matrix::identity;	break;
	case MODE::TEXTURE:		matrix_storage::texture		= aaa::matrix::identity;	break;
	}
#endif
}

//FINLINE void	get_driver_matrix_double( GLenum CONST which,	double* CONST mat44 )	{	get_double( which, mat44 );	}
//FINLINE	void	get_driver_matrix_modelview_double(				double* CONST mat44 )	{	get_driver_matrix_double( GL_MODELVIEW_MATRIX, mat44 );	}
//FINLINE	void	get_driver_matrix_projection_double(			double* CONST mat44 )	{	get_driver_matrix_double( GL_PROJECTION_MATRIX, mat44 );	}
//FINLINE	void	get_driver_matrix_texture_double(				double* CONST mat44 )	{	get_driver_matrix_double( GL_TEXTURE_MATRIX, mat44 );		}
//
//FINLINE void	get_driver_matrix( GLenum CONST which,			REAL* CONST mat44 )		{	get_real( which, mat44 );	}
//FINLINE	void	get_driver_matrix_modelview(					REAL* CONST mat44 )		{	get_driver_matrix( GL_MODELVIEW_MATRIX, mat44 );	}
//FINLINE	void	get_driver_matrix_projection(					REAL* CONST mat44 )		{	get_driver_matrix( GL_PROJECTION_MATRIX, mat44 );	}
//FINLINE	void	get_driver_matrix_texture(						REAL* CONST mat44 )		{	get_driver_matrix( GL_TEXTURE_MATRIX, mat44 );		}

FINLINE void	get_matrix( MODE CONST which, glm::mat4& mat )
{
	switch( which )
	{
	case MODE::MODEL_VIEW:	mat = matrix_storage::modelview;	break;
	case MODE::PROJECTION:	mat = matrix_storage::projection;	break;
	case MODE::TEXTURE:		mat = matrix_storage::texture;		break;
	}
}
FINLINE	void	get_matrix_modelview(	glm::mat4& mat )	{	mat = matrix_storage::modelview;	}
FINLINE	void	get_matrix_projection(	glm::mat4& mat )	{	mat = matrix_storage::projection;	}
FINLINE	void	get_matrix_texture(		glm::mat4& mat )	{	mat = matrix_storage::texture;		}

FINLINE	glm::mat4 CONST *	get_matrix_modelview_pt()	{	return &matrix_storage::modelview;	}
FINLINE	glm::mat4 CONST *	get_matrix_projection_pt()	{	return &matrix_storage::projection;	}
FINLINE	glm::mat4 CONST *	get_matrix_texture_pt()		{	return &matrix_storage::texture;	}

FINLINE	void	set_matrix( glm::mat4 CONST & mat )
{
#if GOL_MATRIX_CACHE()
	*matrix_storage::cur	= mat;
#else
	switch( matrix_storage::mode_cur )
	{
	case MODE::MODEL_VIEW:	matrix_storage::modelview	= mat;	break;
	case MODE::PROJECTION:	matrix_storage::projection	= mat;	break;
	case MODE::TEXTURE:		matrix_storage::texture		= mat;	break;
	}
#endif
}

FINLINE	void	load_matrix( glm::mat4 CONST * mat )
{
	glLoadMatrixf( (FP32*)mat );
	set_matrix( *mat );
}

FINLINE	void	mul_matrix( glm::mat4 CONST * mat )
{
	glMultMatrixf( (FP32*)mat );
#if GOL_MATRIX_CACHE()
	*matrix_storage::cur *= *mat;
#else
	switch( matrix_storage::mode_cur )
	{
	case MODE::MODEL_VIEW:	matrix_storage::modelview	*= *mat; break;
	case MODE::PROJECTION:	matrix_storage::projection	*= *mat; break;
	case MODE::TEXTURE:		matrix_storage::texture		*= *mat; break;
	}
#endif
}
//FINLINE	void	mul_matrix( FP32 CONST * CONST mat )
//{
//	mul_matrix( glm::make_mat4(mat) );
//}

FINLINE void	translate_matrix( glm::vec3 CONST & vec )
{
#if GOL_MATRIX_CACHE()
	aaa::matrix::translate( *matrix_storage::cur,	vec );	
#else
	switch( matrix_storage::mode_cur )
	{
	case MODE::MODEL_VIEW:	aaa::matrix::translate( matrix_storage::modelview,	vec );	break;
	case MODE::PROJECTION:	aaa::matrix::translate( matrix_storage::projection,	vec );	break;
	case MODE::TEXTURE:		aaa::matrix::translate( matrix_storage::texture,	vec );	break;
	}
#endif
}

FINLINE	void	set_ortho( DOUBLE CONST left, DOUBLE CONST right, DOUBLE CONST bottom, DOUBLE CONST top )
{
	gluOrtho2D( left, right, bottom, top );
	set_matrix( aaa::matrix::ortho( left, right, bottom, top ) );
}

FINLINE	void	set_ortho( DOUBLE CONST left, DOUBLE CONST right, DOUBLE CONST bottom, DOUBLE CONST top, DOUBLE CONST z_near, DOUBLE CONST z_far )
{
	glOrtho( left, right, bottom, top, z_near, z_far );
	set_matrix( aaa::matrix::ortho( left, right, bottom, top, z_near, z_far ) );

}
FINLINE	void	set_frustum( DOUBLE CONST left, DOUBLE CONST right, DOUBLE CONST bottom, DOUBLE CONST top, DOUBLE CONST z_near, DOUBLE CONST z_far )
{
	glFrustum( left, right, bottom, top, z_near, z_far );
	set_matrix( aaa::matrix::frustum( left, right, bottom, top, z_near, z_far ) );
}


extern	void	set_ortho_delta(			REAL CONST sx, REAL CONST sy, REAL CONST dx, REAL CONST dy );
extern	void	set_ortho(					REAL CONST sx, REAL CONST sy );
extern	void	set_ortho_with_secu_margin( REAL CONST sx, REAL CONST sy );

//FINLINE	void	lookat(			 REAL CONST* CONST from, REAL CONST* CONST target, REAL CONST* CONST up )
//{
//	gluLookAt( from[0], from[1], from[2], target[0], target[1], target[2], up[0], up[1], up[2] );
//	set_matrix( aaa::matrix::lookat( glm::make_vec3( from ), glm::make_vec3( target ), glm::make_vec3( up ) ) );
//}
FINLINE	void	lookat_safe( REAL CONST* CONST from, REAL CONST* CONST target )
{
//		gluLookAt( from[0], from[1], from[2], target[0], target[1], target[2], 0, 1, 0 );
	glm::mat4 mat = aaa::matrix::identity;
	aaa::matrix::lookat_safe( &mat, from, target );
	mul_matrix( &mat );
}

FINLINE	void	build_matrix_pos_size_dir_then_mul( REAL CONST* CONST pos, REAL CONST* CONST size, REAL CONST* CONST u, REAL CONST* CONST v, REAL CONST* CONST nor )
{
	glm::mat4 mat;
	mat[0].w = mat[1].w = mat[2].w = 0;
	scale_v3( &mat[0].x, u,   size[0] );
	scale_v3( &mat[1].x, v,   size[1] );
	scale_v3( &mat[2].x, nor, size[2] );
	cpy_v3(   &mat[3].x, pos );
	mat[3].w = 1;
	// same as doing
	//GOL::translatev(pos);
	//GOL::rotate_align_on_x( axe );
	//GOL::scalev( size);
	mul_matrix( &mat );
}
FINLINE	void	build_matrix_pos_dir_then_mul( REAL CONST* CONST pos, REAL CONST* CONST u, REAL CONST* CONST v, REAL CONST* CONST nor )
{
	glm::mat4 mat;
	mat[0].w = mat[1].w = mat[2].w = 0;
	cpy_v3( &mat[0].x, u   );
	cpy_v3( &mat[1].x, v   );
	cpy_v3( &mat[2].x, nor );
	cpy_v3( &mat[3].x, pos );
	mat[3].w = 1;
	// same as doing
	//GOL::translatev(pos);
	//GOL::rotate_align_on_x( axe );
	mul_matrix( &mat );
}

FINLINE	void	build_matrix_pos_nor_then_mul( REAL CONST* CONST pos, REAL CONST* CONST nor )
{
	REAL	u[3],v[3];
	build_normal_vectors_v3r( nor, u, v );
	build_matrix_pos_dir_then_mul( pos, u, v, nor );
}

//extern	void	print_matrix_double( double* mat44 );
//extern	void	print_matrix_modelview();
//extern	void	print_matrix_projection();
//extern	void	print_matrix_texture();

extern	void	print_matrix( CONST glm::mat4 & mat44 );
extern	void	print_matrix_modelview_glm();
extern	void	print_matrix_projection_glm();
extern	void	print_matrix_texture_glm();

/*
extern	void	compare_matrix_projection();
extern	void	compare_matrix_modelview();
*/
//TRANSFORMATION
//


FINLINE	void	translate(		REAL CONST x,	REAL CONST y,	REAL CONST z )
{
#if AAA_REAL_IS_DOUBLE()
	glTranslated( x,y,z );
#else
	glTranslatef( x,y,z );
#endif
	translate_matrix( glm::vec3( x,y,z ) );
}
FINLINE	void	translate(		REAL CONST x,	REAL CONST y )					{	if( x!=0. || y!=0. ) translate( x, y, 0. );	}
FINLINE	void	translate(		REAL CONST x )									{	if( x!=0. ) translate( x,0,0 );	}
FINLINE	void	translate_x(	REAL CONST x )									{	if( x!=0. ) translate( x,0,0 );	}
FINLINE	void	translate_y(	REAL CONST y )									{	if( y!=0. ) translate( 0,y,0 );	}
FINLINE	void	translate_z(	REAL CONST z )									{	if( z!=0. ) translate( 0,0,z );	}
FINLINE	void	translate_neg(	REAL CONST x,	REAL CONST y,	REAL CONST z )	{	translate( -x, -y, -z );			}
FINLINE	void	translate2v(	REAL CONST* CONST pt )							{	translate( *pt, *( pt+1 ) );		}
FINLINE	void	translate3v(	REAL CONST* CONST pt )							{	translate( *pt, *( pt+1 ), *( pt+2 ) );	}
FINLINE	void	translatedv(	DOUBLE CONST* CONST pt )
{
	glTranslated( *pt, *( pt+1 ), *( pt+2 ) );
	translate_matrix( glm::vec3( *pt, *( pt + 1 ), *( pt + 2 ) ) );
}
FINLINE	void	translate_negv(	REAL CONST* CONST pt )							{	translate( -*pt, -*( pt+1 ), -*( pt+2 ) );	}

FINLINE	void	scale(			REAL CONST x,	REAL CONST y,	REAL CONST z )
{
#if AAA_REAL_IS_DOUBLE()
	glScaled( x,y,z );
#else
	glScalef( x,y,z );
#endif
#if GOL_MATRIX_CACHE()
	aaa::matrix::scale( *matrix_storage::cur,	glm::vec3( x,y,z ) );
#else
	switch ( matrix_storage::mode_cur )
	{
	case MODE::MODEL_VIEW:	aaa::matrix::scale( matrix_storage::modelview,	glm::vec3( x,y,z ) );	break;
	case MODE::PROJECTION:	aaa::matrix::scale( matrix_storage::projection,	glm::vec3( x,y,z ) );	break;
	case MODE::TEXTURE:		aaa::matrix::scale( matrix_storage::texture,	glm::vec3( x,y,z ) );	break;
	}
#endif
}
FINLINE	void	scale(			REAL CONST x,	REAL CONST y )					{	if( x!=1. || y!=1.) scale( x, y, 1. );	}
FINLINE	void	scale2v(		REAL CONST* CONST pt )							{	scale( *pt, *( pt+1 ) );	}
FINLINE	void	scale3v(		REAL CONST* CONST pt )							{	scale( *pt, *( pt+1 ), *( pt+2 ) );	}
FINLINE	void	scale(			REAL CONST f )									{	if( f!=1. ) scale( f, f, f );	}
FINLINE	void	scale_x(		REAL CONST f )									{	if( f!=1. ) scale( f, 1., 1. );	}
FINLINE	void	scale_y(		REAL CONST f )									{	if( f!=1. ) scale( 1., f, 1. );	}
FINLINE	void	scale_z(		REAL CONST f )									{	if( f!=1. ) scale( 1., 1., f );	}


FINLINE	void	rotate_deg( REAL CONST angle, REAL CONST x, REAL CONST y, REAL CONST z = 0 )
{
#if AAA_REAL_IS_DOUBLE()
	glRotated( angle, x,y,z );
#else
	glRotatef( angle, x,y,z );
#endif
#if GOL_MATRIX_CACHE()
	aaa::matrix::rotate_deg( *matrix_storage::cur,	angle, glm::vec3( x,y,z ) );
#else
	switch ( matrix_storage::mode_cur )
	{
	case MODE::MODEL_VIEW:	 aaa::matrix::rotate_deg( matrix_storage::modelview,	angle, glm::vec3( x,y,z ) );		break;
	case MODE::PROJECTION:	 aaa::matrix::rotate_deg( matrix_storage::projection,	angle, glm::vec3( x,y,z ) );		break;
	case MODE::TEXTURE:		 aaa::matrix::rotate_deg( matrix_storage::texture,		angle, glm::vec3( x,y,z ) );		break;
	}
#endif
}

FINLINE	void	rotatev_deg(  REAL CONST angle, REAL CONST* CONST axe )			{	rotate_deg( angle, *axe, *(axe+1), *(axe+2) );			}
FINLINE	void	rotate_x_deg( REAL CONST angle )								{	if( angle != REAL(0) )	rotate_deg( angle, 1, 0, 0 );	}
FINLINE	void	rotate_y_deg( REAL CONST angle )								{	if( angle != REAL(0) )	rotate_deg( angle, 0, 1, 0 );	}
FINLINE	void	rotate_z_deg( REAL CONST angle )								{	if( angle != REAL(0) )	rotate_deg( angle, 0, 0, 1 );	}

FINLINE	void	rotate_xyz_deg( REAL CONST x, REAL CONST y, REAL CONST z )
{
	rotate_x_deg( x );
	rotate_y_deg( y );
	rotate_z_deg( z );
}

FINLINE	void	rotate_xyzv_deg( REAL CONST* CONST pt )
{
	rotate_x_deg( pt[0] );
	rotate_y_deg( pt[1] );
	rotate_z_deg( pt[2] );
}

FINLINE	void	rotatev(		REAL CONST f, REAL CONST* CONST axe )	{	rotate_deg( f*360.0f, *axe, *( axe+1 ), *( axe+2 ) );	}
FINLINE	void	rotatev(		REAL CONST f, REAL CONST x, REAL CONST y, REAL CONST z = 0 )
																		{	rotate_deg( f*360.0f, x,y,z );	}

FINLINE	void	rotate_x(		REAL CONST f	)						{	if( f != REAL(0) )	rotate_deg( f*360.0f,					1, 0, 0 );	}
FINLINE	void	rotate_x(		REAL CONST f, bool CONST b_flip )		{	if( f != REAL(0) )	rotate_deg( f*(b_flip?-360.f:360.f),	1, 0, 0 );	}

FINLINE	void	rotate_y(		REAL CONST f	)						{	if( f != REAL(0) )	rotate_deg( f*360.0f,					0, 1, 0 );	}
FINLINE	void	rotate_y(		REAL CONST f, bool CONST b_flip )		{	if( f != REAL(0) )	rotate_deg( f*(b_flip?-360.f:360.f),	0, 1, 0 );	}

FINLINE	void	rotate_z(		REAL CONST f	)						{	if( f != REAL(0) )	rotate_deg( f*360.0f,					0, 0, 1 );	}
FINLINE	void	rotate_z(		REAL CONST f, bool CONST b_flip )		{	if( f != REAL(0) )	rotate_deg( f*(b_flip?-360.f:360.f),	0, 0, 1 );	}

FINLINE	void	rotate_xyz(		REAL CONST x, REAL CONST y, REAL CONST z )
{
	rotate_x( x );
	rotate_y( y );
	rotate_z( z );
}

FINLINE	void	rotate_xyzv( REAL CONST* CONST pt )
{
	rotate_x( pt[0] );
	rotate_y( pt[1] );
	rotate_z( pt[2] );
}

FINLINE	void	rotate( REAL CONST x, REAL CONST y, REAL CONST z, INT32 CONST type = aaa::matrix::ORDER_ZYX )
{
	switch( type & aaa::matrix::ORDER_MASK )
	{
	case aaa::matrix::ORDER_XYZ:
		rotate_x( x, ( type & aaa::matrix::ORDER_X_NEG )!=0 );
		rotate_y( y, ( type & aaa::matrix::ORDER_Y_NEG )!=0 );
		rotate_z( z, ( type & aaa::matrix::ORDER_Z_NEG )!=0 );
		break;
	case aaa::matrix::ORDER_YZX:
		rotate_y( y, ( type & aaa::matrix::ORDER_Y_NEG )!=0 );
		rotate_z( z, ( type & aaa::matrix::ORDER_Z_NEG )!=0 );
		rotate_x( x, ( type & aaa::matrix::ORDER_X_NEG )!=0 );
		break;
	case aaa::matrix::ORDER_ZXY:
		rotate_z( z, ( type & aaa::matrix::ORDER_Z_NEG )!=0 );
		rotate_x( x, ( type & aaa::matrix::ORDER_X_NEG )!=0 );
		rotate_y( y, ( type & aaa::matrix::ORDER_Y_NEG )!=0 );
		break;
	case aaa::matrix::ORDER_XZY:
		rotate_x( x, ( type & aaa::matrix::ORDER_X_NEG )!=0 );
		rotate_z( z, ( type & aaa::matrix::ORDER_Z_NEG )!=0 );
		rotate_y( y, ( type & aaa::matrix::ORDER_Y_NEG )!=0 );
		break;
	case aaa::matrix::ORDER_YXZ:
		rotate_y( y, ( type & aaa::matrix::ORDER_Y_NEG )!=0 );
		rotate_x( x, ( type & aaa::matrix::ORDER_X_NEG )!=0 );
		rotate_z( z, ( type & aaa::matrix::ORDER_Z_NEG )!=0 );
		break;
	case aaa::matrix::ORDER_ZYX:
		rotate_z( z, ( type & aaa::matrix::ORDER_Z_NEG )!=0 );
		rotate_y( y, ( type & aaa::matrix::ORDER_Y_NEG )!=0 );
		rotate_x( x, ( type & aaa::matrix::ORDER_X_NEG )!=0 );
		break;
	}
}

FINLINE	void	rotatev(		REAL CONST* CONST pt, INT32 CONST type = aaa::matrix::ORDER_ZYX )	{	rotate( *pt,	*( pt+1 ),	*( pt+2 ),	type );	}
FINLINE	void	rotatev_neg(	REAL CONST* CONST pt, INT32 CONST type = aaa::matrix::ORDER_ZYX )	{	rotate( -*pt,	-*( pt+1 ),	-*( pt+2 ),	type );	}

FINLINE	void	rotate_align_on_z( INT32 CONST axe )
{
	switch( axe )
	{
		case 0:	rotate_deg( 120., 1., 1., 1. );	break;
		case 1:	rotate_deg( 240., 1., 1., 1. );	break;
/*
		case 1:	rotate_y_deg( -90 );
				rotate_x_deg( -90 );
				break;
		case 2:	rotate_x_deg( 90 );
				rotate_y_deg( 90 );
				break;
*/
	}
}

}	//namespace GOL
