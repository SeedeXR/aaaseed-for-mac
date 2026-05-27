
#ifdef AAA_BILLBOARD_H
#error "BILLBOARD_H included more than once."
#endif
#define AAA_BILLBOARD_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_GOL_MATRIX_H
#	include "gol/gol_matrix.h"
#endif
#ifndef AAA_LAYER_H
#	include "infrastructure/layer/layer.h"
#endif
#ifndef AAA_SEEDCAM_H
#	include "draw/seedcam.h"
#endif
#ifndef AAA_AAA_MATH_H
#	include "math/aaa_math.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif

class billboard
{
private:
	FINLINE	static	void	compute_abs(  REAL* dst, REAL CONST* src );
public:
	FINLINE	static	void	compute( REAL* dst);
	FINLINE	static	void	compute( REAL* dst, REAL CONST* src );
	FINLINE	static	void	do_x();
	FINLINE	static	void	do_x( REAL CONST * in );
	FINLINE	static	void	do_y( REAL CONST * in );
};

FINLINE	void	billboard::compute_abs(  REAL* dst, REAL CONST* src )
{
	dst[1] = REAL(ATAN2_TURN( src[2], src[0] )) + REAL(.25);
	dst[2] = -REAL(ATAN2_TURN( SQRT_R( src[2]*src[2] + src[0]*src[0] ), src[1] ));
	/*	if( src[0] < 0 )
	dst[1] += .5;
	if( src[2] >= 0 )
	dst[2] = -ATAN2_INT( SQRT(src[2]*src[2]+src[0]*src[0]), src[1] );
	else	
	*/
	//	DBG_PRINT_STRING( "src %f, %f, %f SQRT %f dst %f %f", src[0], src[1], src[2], SQRT(src[2]*src[2]+src[0]*src[0]), dst[1], dst[2] );
}

FINLINE	void	billboard::compute( REAL* dst )
{
	if( c_layer::get_cur()->is_camera_linked() )
		clear_v3( dst );
	else
		compute_abs( dst, c_seedcam::get_cur()->get_position_pt() );
}

FINLINE	void	billboard::compute( REAL* dst, REAL CONST * src )
{
	if( c_layer::get_cur()->is_camera_linked() )
		compute_abs( dst, src );
	else
	{
		FP32 vec[3];
		c_seedcam::get_cur()->get_position( vec );
		sub_reverse_v3( &vec[0], src );	// vec = src - vec
		compute_abs( dst, vec );
	}
}

FINLINE	void	billboard::do_x()
{
	REAL	rot[3];
	compute( rot );
	GOL::matrix::rotate_y( rot[1] );
	GOL::matrix::rotate_z( rot[2] );
}

FINLINE	void	billboard::do_x( REAL CONST *in )
{
	REAL	rot[3];
	compute( rot, in );
	GOL::matrix::rotate_y( rot[1] );
	GOL::matrix::rotate_z( rot[2] );
}

FINLINE	void	billboard::do_y( REAL CONST *in )
{
	do_x( in );
	GOL::matrix::rotate_y_deg( 90 );
}

