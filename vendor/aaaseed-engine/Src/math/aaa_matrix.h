
#ifdef AAA_AAA_MATRIX_H
#error "AAA_MATRIX_H included more than once."
#endif
#define AAA_AAA_MATRIX_H 1


#ifndef AAA_AAA_TYPE_H
#	include "code_utils/aaa_type.h"
#endif

#define GLM_FORCE_INLINE 1
#if AAA_USE_AVX2()
#	define GLM_FORCE_SIMD_AVX2
#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace aaa::matrix
{
enum ROTATE_ORDER : INT32
{
	ORDER_XYZ	= 0,
	ORDER_YZX	= 1,
	ORDER_ZXY	= 2,

	ORDER_XZY	= 3,
	ORDER_YXZ	= 4,
	ORDER_ZYX	= 5,

	ORDER_MASK	= 0xf,

	ORDER_X_NEG = 0x10,
	ORDER_Y_NEG = 0x20,
	ORDER_Z_NEG = 0x40,
};

template< class T >
CONSTEXPR	void	extract_rows_from_mat3( T* a, T* b, T* c, FP32 CONST * mat33 )
{
	*a		= *mat33;
	*++a	= *++mat33;
	*++a	= *++mat33;

	*b		= *++mat33;
	*++b	= *++mat33;
	*++b	= *++mat33;

	*c		= *++mat33;
	*++c	= *++mat33;
	*++c	= *++mat33;
}


CONSTEXPR glm::mat4 identity( 1.f );
//todo declare in .h
CONSTEXPR glm::vec3 unit_x = glm::vec3( 1.0f, 0.0f, 0.0f );
CONSTEXPR glm::vec3 unit_y = glm::vec3( 0.0f, 1.0f, 0.0f );
CONSTEXPR glm::vec3 unit_z = glm::vec3( 0.0f, 0.0f, 1.0f );

FINLINE	glm::mat4	mul_matrix( glm::mat4& m1, CONST glm::mat4& m2 )
{
	return m1 * m2;
}

FINLINE	glm::mat4	mul_matrix( glm::mat4& m1, FP32 CONST * CONST m2 )
{
	return m1 * glm::make_mat4( m2 );
}

FINLINE	glm::mat4	ortho( DOUBLE CONST left, DOUBLE CONST right, DOUBLE CONST bottom, DOUBLE CONST top )
{
	return glm::ortho( left, right, bottom, top );
}

FINLINE	glm::mat4	ortho( DOUBLE CONST left, DOUBLE CONST right, DOUBLE CONST bottom, DOUBLE CONST top, DOUBLE CONST z_near, DOUBLE CONST z_far )
{
	return glm::ortho( left, right, bottom, top, z_near, z_far );
}

//FINLINE	glm::mat4	ortho_bounding_box( const glm::vec3& center, const float extent, const float inflate )
//{
//	float half_length = extent * 0.5f;
//	return glm::ortho(-extent, extent, -extent, extent, -extent * inflate, extent * inflate);
//}

FINLINE	glm::mat4	frustum( DOUBLE CONST left, DOUBLE CONST right, DOUBLE CONST bottom, DOUBLE CONST top, DOUBLE CONST z_near, DOUBLE CONST z_far )
{
	return glm::frustum( left, right, bottom, top, z_near, z_far );
}

FINLINE void	lookat_safe( glm::mat4* mat, FP32 CONST * from, FP32 CONST * target )
{
	if( from[0]==target[0] && from[2]==target[2] )	// dir is vertical
		*mat = glm::lookAt( glm::make_vec3(from), glm::make_vec3(target), aaa::matrix::unit_z );
	else
		*mat = glm::lookAt( glm::make_vec3(from), glm::make_vec3(target), aaa::matrix::unit_y );
}

//FINLINE	glm::mat4	lookat( glm::vec3 CONST & from, glm::vec3 CONST & target, glm::vec3 CONST & up )
//{
//	return glm::lookAt( from, target, up );
//}
//
//FINLINE	glm::mat4	lookat_vertical( glm::vec3 CONST & from, glm::vec3 CONST & target )
//{
//	return glm::lookAt( from, target, unit_y );
//}
//
//FINLINE	glm::mat4	lookat_vertical( REAL CONST * CONST from, REAL CONST * CONST target )
//{
//	return glm::lookAt( glm::make_vec3( from ), glm::make_vec3( target ), unit_y );
//}

// replace gluProject
FINLINE	glm::vec3	project( glm::vec3 CONST& obj, glm::mat4 CONST& mat_modelview, glm::mat4 CONST& mat_proj, glm::vec4 CONST& viewport )
{
	return glm::project( obj, mat_modelview, mat_proj, viewport );
//	gluProject( *pt, *( pt + 1 ), *( pt + 2 ), mat_modelview, mat_proj, viewport, &mouse_x, &mouse_y, &_zdepth );
}

// replace gluUnProject
FINLINE	glm::vec3	unproject( glm::vec3 CONST & win, glm::mat4 CONST & mat_modelview, glm::mat4 CONST & mat_proj, glm::vec4 CONST & viewport )
{
	return glm::unProject( win, mat_modelview, mat_proj, viewport );
//	gluUnProject( win_x, viewport[ 3 ] - win_y, _zdepth, mat_modelview, mat_proj, viewport, &x, &y, &z );
}

//TRANSFORMATION

FINLINE	void	translate( glm::mat4& m1, glm::vec3 CONST & vec )
{
	m1 = glm::translate( m1, vec );
}

FINLINE	void	translate( glm::mat4& m1, REAL CONST x, REAL CONST y, REAL CONST z ) { translate( m1, glm::vec3( x,y,z ) ); }
FINLINE	void	translate( glm::mat4& m1, glm::vec2 CONST & vec ) { translate( m1, glm::vec3( vec, .0f ) ); }
FINLINE	void	translate( glm::mat4& m1, REAL CONST x, REAL CONST y ) { if ( x != 0. || y != 0. ) translate( m1, glm::vec3( x,y,0. ) ); }
FINLINE	void	translate( glm::mat4& m1, REAL CONST x )   { if ( x != 0. ) translate( m1, glm::vec3( x, 0.,0. ) ); }
FINLINE	void	translate_x( glm::mat4& m1, REAL CONST x ) { if ( x != 0. ) translate( m1, glm::vec3( x, 0.,0. ) ); }
FINLINE	void	translate_y( glm::mat4& m1, REAL CONST y ) { if ( y != 0. ) translate( m1, glm::vec3( 0.,y, 0. ) ); }
FINLINE	void	translate_z( glm::mat4& m1, REAL CONST z ) { if ( z != 0. ) translate( m1, glm::vec3( 0.,0.,z  ) ); }
FINLINE	void	translate_neg( glm::mat4& m1, REAL CONST x, REAL CONST y, REAL CONST z ) { translate( m1, glm::vec3( -x, -y, -z ) ); }
FINLINE	void	translate2v( glm::mat4& m1, REAL CONST* CONST pt ) { translate( m1, *pt, *( pt + 1 ) ); }
FINLINE	void	translate3v( glm::mat4& m1, REAL CONST* CONST pt ) { translate( m1, glm::make_vec3( pt ) ); }
//FINLINE	void	translatedv_glm(	DOUBLE CONST* CONST pt )
//{
//	glTranslated( *pt, *( pt+1 ), *( pt+2 ) );
//}
FINLINE	void	translate_negv( glm::mat4& m1, REAL CONST * CONST pt ) { translate( m1, glm::vec3( -*pt, -*( pt + 1 ), -*( pt + 2 ) ) ); }

FINLINE void scale_inplace( glm::mat4 & m, FP32 const * const sca )
{
	m[0] *= sca[0];
	m[1] *= sca[1];
	m[2] *= sca[2];
}
FINLINE	void	scale( glm::mat4& m1, glm::vec3 vec )
{
	m1 = glm::scale( m1, vec );
}
FINLINE	void	scale( glm::mat4& m1, REAL CONST x, REAL CONST y, REAL CONST z ) { scale( m1, glm::vec3( x,y,z ) ); }
FINLINE	void	scale( glm::mat4& m1, REAL CONST x, REAL CONST y ) { if ( x != 1. || y != 1. ) scale( m1, glm::vec3( x,y,1. ) ); }
FINLINE	void	scale2v( glm::mat4& m1, REAL CONST* CONST pt ) { scale( m1, *pt, *( pt + 1 ) ); }
FINLINE	void	scale3v( glm::mat4& m1, REAL CONST* CONST pt ) { scale( m1, glm::make_vec3( pt ) ); }
FINLINE	void	scale( glm::mat4& m1, REAL CONST f ) { if ( f != 1. ) scale( m1, glm::vec3( f, f, f ) ); }
FINLINE	void	scale_x( glm::mat4& m1, REAL CONST f ) { if ( f != 1. ) scale( m1, glm::vec3( f, 1., 1. ) ); }
FINLINE	void	scale_y( glm::mat4& m1, REAL CONST f ) { if ( f != 1. ) scale( m1, glm::vec3( 1., f, 1. ) ); }
FINLINE	void	scale_z( glm::mat4& m1, REAL CONST f ) { if ( f != 1. ) scale( m1, glm::vec3( 1., 1., f ) ); }


	FINLINE void translate_inplace_x( glm::mat4 & m, FP32 CONST x )
	{
		m[3] += m[0] * x;
	}
	FINLINE void translate_inplace_y( glm::mat4 & m, FP32 CONST y )
	{
		m[3] += m[1] * y;
	}
	FINLINE void translate_inplace_z( glm::mat4 & m, FP32 CONST z )
	{
		m[3] += m[2] * z;
	}
	FINLINE void translate_inplace( glm::mat4 & m, FP32 const * const tra )
	{
		m[3] += m[0] * *tra + m[1] * *(tra+1) + m[2] * *(tra+2);
	}
	FINLINE void translate_neg_inplace( glm::mat4 & m, FP32 const * const tra )
	{
		m[3] += m[0] * -*tra + m[1] * -*(tra+1) + m[2] * -*(tra+2);
	}
	FINLINE void set_translate_inplace( glm::mat4 & m, FP32 const * const tra )
	{
		m[3][0] = *tra;
		m[3][1] = *(tra+1);
		m[3][2] = *(tra+2);
	}

//simplify glm rotate
	FINLINE void rotate_inplace_x_quarter_turn( glm::mat4 & m )
	{
		glm::vec4 tmp = m[1];
		m[1] = m[2];
		m[2] = -tmp;
	}
	FINLINE void rotate_inplace_x_quarter_turn_neg( glm::mat4 & m )
	{
		glm::vec4 tmp = m[1];
		m[1] = -m[2];
		m[2] = tmp;
	}
	FINLINE void rotate_inplace_y_quarter_turn( glm::mat4 & m )
	{
		glm::vec4 tmp = m[0];
		m[0] = -m[2];
		m[2] = tmp;
	}
	FINLINE void rotate_inplace_y_quarter_turn_neg( glm::mat4 & m )
	{
		glm::vec4 tmp = m[0];
		m[0] = m[2];
		m[2] = -tmp;
	}
	FINLINE void rotate_inplace_z_quarter_turn( glm::mat4 & m )
	{
		glm::vec4 tmp = m[0];
		m[0] = m[1];
		m[1] = -tmp;
	}
	FINLINE void rotate_inplace_z_quarter_turn_neg( glm::mat4 & m )
	{
		glm::vec4 tmp = m[0];
		m[0] = -m[1];
		m[1] = tmp;
	}

	FINLINE void rotate_inplace_x( glm::mat4 & m, FP32 const angle )
	{
		FP32 const c = cos(angle);
		FP32 const s = sin(angle);
		glm::vec4 tmp = m[1];
		m[1] =  tmp * c + m[2] * s;
		m[2] = -tmp * s + m[2] * c;
	}
	FINLINE void rotate_inplace_y( glm::mat4 & m, FP32 const angle )
	{
		FP32 const c = cos(angle);
		FP32 const s = sin(angle);
		glm::vec4 tmp = m[0];
		m[0] =  tmp * c - m[2] * s;
		m[2] =  tmp * s + m[2] * c;
	}
	FINLINE void rotate_inplace_z( glm::mat4 & m, FP32 const angle )
	{
		FP32 const c = cos(angle);
		FP32 const s = sin(angle);
		glm::vec4 tmp = m[0];
		m[0] =  tmp * c + m[1] * s;
		m[1] = -tmp * s + m[1] * c;
	}

	FINLINE glm::mat4 rotate_x( glm::mat4 const & m, FP32 const angle )
	{
		FP32 const c = cos(angle);
		FP32 const s = sin(angle);
		glm::mat4 Result;
		Result[0] =  m[0];
		Result[1] =  m[1] * c + m[2] * s;
		Result[2] = -m[1] * s + m[2] * c;
		Result[3] =  m[3];
		return Result;
	}
	FINLINE glm::mat4 rotate_y( glm::mat4 const & m, FP32 const angle )
	{
		FP32 const c = cos(angle);
		FP32 const s = sin(angle);
		glm::mat4 Result;
		Result[0] =  m[0] * c - m[2] * s;
		Result[1] =  m[1];
		Result[2] =  m[0] * s + m[2] * c;
		Result[3] =  m[3];
		return Result;
	}
	FINLINE glm::mat4 rotate_z( glm::mat4 const & m, FP32 const angle )
	{
		FP32 const c = cos(angle);
		FP32 const s = sin(angle);
		glm::mat4 Result;
		Result[0] =  m[0] * c + m[1] * s;
		Result[1] = -m[0] * s + m[1] * c;
		Result[2] =  m[2];
		Result[3] =  m[3];
		return Result;
	}


//todo do turn and rad too
FINLINE	void	rotate_deg(		glm::mat4& m1, REAL CONST angle, glm::vec3 CONST & vec )
{
	m1 = glm::rotate( m1, glm::radians(angle), vec );
}
FINLINE	void	rotate_deg(		glm::mat4& m1, REAL CONST angle, REAL CONST x, REAL CONST y, REAL CONST z )
{
	rotate_deg( m1, angle, glm::vec3( x,y,z ) );
}
FINLINE	void	rotatev_deg(	glm::mat4& m1, REAL CONST angle, REAL CONST* CONST axe )
{
	rotate_deg( m1, angle, glm::make_vec3( axe ) );
}
//these should be specialized
FINLINE	void	rotate_x_deg(	glm::mat4& m, REAL CONST angle )
{
	if( angle != REAL(0) )
		rotate_inplace_x( m, glm::radians(angle) );
}
FINLINE	void	rotate_y_deg(	glm::mat4& m, REAL CONST angle )
{
	if( angle != REAL(0) )
		rotate_inplace_y( m, glm::radians(angle) );
}
FINLINE	void	rotate_z_deg(	glm::mat4& m, REAL CONST angle )
{
	if( angle != REAL(0) )
		rotate_inplace_z( m, glm::radians(angle) );
}

FINLINE	void	rotate_xyz_deg(	glm::mat4 & m, REAL CONST x, REAL CONST y, REAL CONST z)
{
	rotate_x_deg( m, x );
	rotate_y_deg( m, y );
	rotate_z_deg( m, z );
}
FINLINE	void	rotate_xyz_deg( glm::mat4 & m, REAL CONST * CONST rot_xyz )
{
	rotate_x_deg( m, *(rot_xyz+0) );
	rotate_y_deg( m, *(rot_xyz+1) );
	rotate_z_deg( m, *(rot_xyz+2) );
}
FINLINE	void	rotate_xyz_deg( glm::mat4 & m, glm::vec3 const rot_xyz )
{
	rotate_x_deg( m, rot_xyz[0] );
	rotate_y_deg( m, rot_xyz[1] );
	rotate_z_deg( m, rot_xyz[2] );
}

FINLINE	void	rotate_order_deg( glm::mat4& m, REAL CONST * CONST xyz, INT32 CONST type = ORDER_ZYX )
{
	switch( type & ORDER_MASK )
	{
	case ORDER_XYZ:
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -*xyz : *xyz );
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -*(xyz+1) : *(xyz+1) );
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -*(xyz+2) : *(xyz+2) );
		break;
	case ORDER_YZX:
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -*(xyz+1) : *(xyz+1) );
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -*(xyz+2) : *(xyz+2) );
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -*xyz : *xyz );
		break;
	case ORDER_ZXY:
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -*(xyz+2) : *(xyz+2) );
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -*xyz : *xyz );
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -*(xyz+1) : *(xyz+1) );
		break;
	case ORDER_XZY:
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -*xyz : *xyz );
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -*(xyz+2) : *(xyz+2) );
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -*(xyz+1) : *(xyz+1) );
		break;
	case ORDER_YXZ:
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -*(xyz+1) : *(xyz+1) );
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -*xyz : *xyz );
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -*(xyz+2) : *(xyz+2) );
		break;
	case ORDER_ZYX:
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -*(xyz+2) : *(xyz+2) );
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -*(xyz+1) : *(xyz+1) );
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -*xyz : *xyz );
		break;
	}
}

FINLINE	void	rotate_order_deg( glm::mat4& m, REAL CONST x, REAL CONST y, REAL CONST z, INT32 CONST type = ORDER_ZYX )
{
	switch( type & ORDER_MASK )
	{
	case ORDER_XYZ:
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -x : x );
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -y : y );
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -z : z );
		break;
	case ORDER_YZX:
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -y : y );
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -z : z );
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -x : x );
		break;
	case ORDER_ZXY:
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -z : z );
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -x : x );
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -y : y );
		break;
	case ORDER_XZY:
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -x : x );
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -z : z );
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -y : y );
		break;
	case ORDER_YXZ:
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -y : y );
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -x : x );
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -z : z );
		break;
	case ORDER_ZYX:
		rotate_z_deg( m, (type & ORDER_Z_NEG) ? -z : z );
		rotate_y_deg( m, (type & ORDER_Y_NEG) ? -y : y );
		rotate_x_deg( m, (type & ORDER_X_NEG) ? -x : x );
		break;
	}
}

FINLINE	void	rotatev( glm::mat4& m1, REAL CONST f, REAL CONST* CONST axe ) { rotate_deg( m1, f * 360.0f, *axe, *( axe + 1 ), *( axe + 2 ) ); }
//	FINLINE	void	rotatev_neg(	REAL CONST f, REAL CONST* CONST axe )	{	rotate_deg( f*360.0f, *axe, *( axe+1 ), *( axe+2 ) );	}

FINLINE	void	rotate_x( glm::mat4& m1, REAL CONST f ) { if ( f != REAL( 0 ) )	rotate_x_deg( m1, f * 360.0f ); }
FINLINE	void	rotate_x( glm::mat4& m1, REAL CONST f, bool CONST b_flip ) { if ( f != REAL( 0 ) )	rotate_x_deg( m1, f * ( b_flip ? -360.f : 360.f ) ); }

FINLINE	void	rotate_y( glm::mat4& m1, REAL CONST f ) { if ( f != REAL( 0 ) )	rotate_y_deg( m1, f * 360.0f ); }
FINLINE	void	rotate_y( glm::mat4& m1, REAL CONST f, bool CONST b_flip ) { if ( f != REAL( 0 ) )	rotate_y_deg( m1, f * ( b_flip ? -360.f : 360.f ) ); }

FINLINE	void	rotate_z( glm::mat4& m1, REAL CONST f ) { if ( f != REAL( 0 ) )	rotate_z_deg( m1, f * 360.0f ); }
FINLINE	void	rotate_z( glm::mat4& m1, REAL CONST f, bool CONST b_flip ) { if ( f != REAL( 0 ) )	rotate_z_deg( m1, f * ( b_flip ? -360.f : 360.f ) ); }

FINLINE	void	rotate_xyz( glm::mat4& m1, REAL CONST x, REAL CONST y, REAL CONST z )
{
	rotate_x( m1, x );
	rotate_y( m1, y );
	rotate_z( m1, z );
}

FINLINE	void	rotate_xyzv( glm::mat4& m, REAL CONST* CONST pt )
{
	rotate_x( m, pt[ 0 ] );
	rotate_y( m, pt[ 1 ] );
	rotate_z( m, pt[ 2 ] );
}

FINLINE	void	rotate_order( glm::mat4& m1, REAL CONST x, REAL CONST y, REAL CONST z, INT32 CONST type = aaa::matrix::ORDER_ZYX )
{
	switch ( type & ORDER_MASK )
	{
	case aaa::matrix::ORDER_XYZ:
		rotate_x( m1, x, ( type & aaa::matrix::ORDER_X_NEG ) != 0 );
		rotate_y( m1, y, ( type & aaa::matrix::ORDER_Y_NEG ) != 0 );
		rotate_z( m1, z, ( type & aaa::matrix::ORDER_Z_NEG ) != 0 );
		break;
	case aaa::matrix::ORDER_YZX:
		rotate_y( m1, y, ( type & aaa::matrix::ORDER_Y_NEG ) != 0 );
		rotate_z( m1, z, ( type & aaa::matrix::ORDER_Z_NEG ) != 0 );
		rotate_x( m1, x, ( type & aaa::matrix::ORDER_X_NEG ) != 0 );
		break;
	case aaa::matrix::ORDER_ZXY:
		rotate_z( m1, z, ( type & aaa::matrix::ORDER_Z_NEG ) != 0 );
		rotate_x( m1, x, ( type & aaa::matrix::ORDER_X_NEG ) != 0 );
		rotate_y( m1, y, ( type & aaa::matrix::ORDER_Y_NEG ) != 0 );
		break;
	case aaa::matrix::ORDER_XZY:
		rotate_x( m1, x, ( type & aaa::matrix::ORDER_X_NEG ) != 0 );
		rotate_z( m1, z, ( type & aaa::matrix::ORDER_Z_NEG ) != 0 );
		rotate_y( m1, y, ( type & aaa::matrix::ORDER_Y_NEG ) != 0 );
		break;
	case aaa::matrix::ORDER_YXZ:
		rotate_y( m1, y, ( type & aaa::matrix::ORDER_Y_NEG ) != 0 );
		rotate_x( m1, x, ( type & aaa::matrix::ORDER_X_NEG ) != 0 );
		rotate_z( m1, z, ( type & aaa::matrix::ORDER_Z_NEG ) != 0 );
		break;
	case aaa::matrix::ORDER_ZYX:
		rotate_z( m1, z, ( type & aaa::matrix::ORDER_Z_NEG ) != 0 );
		rotate_y( m1, y, ( type & aaa::matrix::ORDER_Y_NEG ) != 0 );
		rotate_x( m1, x, ( type & aaa::matrix::ORDER_X_NEG ) != 0 );
		break;
	}
}

FINLINE	void	rotate_orderv(		glm::mat4& m1, REAL CONST* CONST pt, INT32 CONST type = aaa::matrix::ORDER_ZYX ) { rotate_order( m1, *pt, *( pt + 1 ), *( pt + 2 ), type ); }
FINLINE	void	rotate_order_negv(	glm::mat4& m1, REAL CONST* CONST pt, INT32 CONST type = aaa::matrix::ORDER_ZYX ) { rotate_order( m1, -*pt, -*( pt + 1 ), -*( pt + 2 ), type ); }

FINLINE	void	rotate_align_on_z( glm::mat4& m, INT32 CONST axe )
{
	switch ( axe )
	{
	case 0:	rotate_deg( m, 120., 1., 1., 1. );	break;
	case 1:	rotate_deg( m, 240., 1., 1., 1. );	break;
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

//was used with fbx::opengl conversion
extern void make_matrix_tra_rotyzx_deg_sca( glm::mat4* mat	,FP32 CONST * CONST tra, FP32 CONST * CONST rot_deg, FP32 CONST * CONST sca );
extern void make_matrix_tra_rotyzx_rad_sca( glm::mat4* mat	,FP32 CONST * CONST tra, FP32 CONST * CONST rot_rad, FP32 CONST * CONST sca );
//was is with fbx::gol conversion
extern void make_matrix_tra_rotyxz_deg_sca( glm::mat4* mat	,FP32 CONST * CONST tra, FP32 CONST * CONST rot_deg, FP32 CONST * CONST sca );
extern void make_matrix_tra_rotyxz_rad_sca( glm::mat4* mat	,FP32 CONST * CONST tra, FP32 CONST * CONST rot_rad, FP32 CONST * CONST sca );
}


