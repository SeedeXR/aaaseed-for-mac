
#include "aaa_matrix.h"
#include "v.h"
//#include <glm/gtx/transform.hpp>


CONSTEXPR bool is_zero_v3fp32( FP32 CONST * CONST vec )  {   return *vec==0.f && *(vec+1)==0.f && *(vec+2)==0.f ; }
CONSTEXPR bool is_one_v3fp32(  FP32 CONST * CONST vec )  {   return *vec==1.f && *(vec+1)==1.f && *(vec+2)==1.f ; }

void aaa::matrix::make_matrix_tra_rotyzx_deg_sca( glm::mat4 *  mat	,FP32 CONST * CONST	tra
																	,FP32 CONST * CONST	rot_deg
																	,FP32 CONST * CONST	sca	)
{
	*mat = identity;
	if( !is_zero_v3fp32( tra ) )
		set_translate_inplace( *mat, tra );
	rotate_y_deg( *mat, *(rot_deg+1)	);
	rotate_z_deg( *mat, *(rot_deg+2)	);
	rotate_x_deg( *mat, *(rot_deg+0)	);
	if( !is_one_v3fp32( sca ) )
		scale_inplace( *mat, sca );
}

void aaa::matrix::make_matrix_tra_rotyxz_deg_sca( glm::mat4 *  mat	,FP32 CONST * CONST	tra
																	,FP32 CONST * CONST	rot_deg
																	,FP32 CONST * CONST	sca	)
{
	*mat = identity;
	if( !is_zero_v3fp32( tra ) )
		set_translate_inplace( *mat, tra );
	rotate_y_deg( *mat, *(rot_deg+1)	);
	rotate_x_deg( *mat, *(rot_deg+0)	);
	rotate_z_deg( *mat, *(rot_deg+2)	);
	if( !is_one_v3fp32( sca ) )
		scale_inplace( *mat, sca );
}

//was for opengl
void aaa::matrix::make_matrix_tra_rotyzx_rad_sca( glm::mat4 *  mat	,FP32 CONST * CONST	tra
																	,FP32 CONST * CONST	rot_rad
																	,FP32 CONST * CONST	sca	)
{
	*mat = identity;
	if( !is_zero_v3fp32( tra ) )
		set_translate_inplace( *mat, tra );
	if( *(rot_rad+1) != FP32(0) )
		rotate_inplace_y( *mat, *(rot_rad+1) );
	if( *(rot_rad+2) != FP32(0) )
		rotate_inplace_z( *mat, *(rot_rad+2) );
	if( *(rot_rad+0) != FP32(0) )
		rotate_inplace_x( *mat, *(rot_rad+0) );
	if( !is_one_v3fp32( sca ) )
		scale_inplace( *mat, sca );
}

//is for gol 
void aaa::matrix::make_matrix_tra_rotyxz_rad_sca( glm::mat4 *  mat	,FP32 CONST * CONST	tra
																	,FP32 CONST * CONST	rot_rad
																	,FP32 CONST * CONST	sca	)
{
	*mat = identity;
	if( !is_zero_v3fp32( tra ) )
		set_translate_inplace( *mat, tra );
	if( *(rot_rad+1) != FP32(0) )
		rotate_inplace_y( *mat, *(rot_rad+1) );
	if( *(rot_rad+0) != FP32(0) )
		rotate_inplace_x( *mat, *(rot_rad+0) );
	if( *(rot_rad+2) != FP32(0) )
		rotate_inplace_z( *mat, *(rot_rad+2) );
	if( !is_one_v3fp32( sca ) )
		scale_inplace( *mat, sca );
}

