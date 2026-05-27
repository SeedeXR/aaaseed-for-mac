#include "draw/box.h"
#include "draw/map.h"
#include "infrastructure/layer/layer.h"
#include "draw/guf.h"


//todo try to go back to GLfloat when ok
namespace {
	typedef	FP32 FLOAT3x8[8][3];

	UINT8	CONSTEXPR	box_strip_1[]		= { 1,0,	2,3,	6,7,	5,4 };
	UINT8	CONSTEXPR	box_strip_2[]		= { 7,3,	4,0,	5,1,	6,2 };
	UINT8	CONSTEXPR	box_strip[]			= { 1,0,	2,3,	6,7,	5,4, 4,7,  7,3,	4,0,	5,1,	6,2 };

	UINT8	CONSTEXPR	box_lines_index[]	= {	1,2, 2,6, 6,5, 5,1,	 0,1, 3,2, 7,6, 4,5,	0,3, 3,7, 7,4, 4,0		};
	UINT8	CONSTEXPR	box_faces_quads[]	= {	0,3,2,1,		0,1,5,4,		0,4,7,3,		6,7,4,5,		6,2,3,7,		6,5,1,2			};
	UINT8	CONSTEXPR	box_faces_tri[]		= {	0,3,2,	0,2,1,	0,1,5,	0,5,4,	0,4,7,	0,7,3,	6,7,4,	6,4,5,	6,2,3,	6,3,7,	6,5,1,	6,1,2	};

	//UINT8	CONSTEXPR	box_faces_tri[]		= {	3,0,2,	0,1,2,	1,0,5,	0,4,5 }; //,	0,4,7,	0,7,3,	6,7,4,	6,4,5,	6,2,3,	6,3,7,	6,5,1,	6,1,2	};
	UINT8	CONSTEXPR	box_uv_tri_strip[]	= {	0,1,2,3, 3,4, 4,5,6,7, 7,8,	8,9,10,11, 11,12, 12,13,14,15, 15,16, 16,17,18,19, 19,20, 20,21,22,23 };
}

namespace box
{
	c_guf	guf_box_base;	// just the base data for box, with 24 point so we can use noremal and uv by face
							//	we will use it to draw the box align with axes with different size:
							//	changing the vertex pointer and using the normal and uv defined at start
	c_guf	guf_box;		//	just the base data for box, with 24 point so we can use noremal and uv by face
							//	we will use it to draw box with different size and normal
							//	but we don't change the uv data
	
	static	FLOAT3x8	points_cube_xyz_cano;	// 8 points of a box align with axes with size 1, centered on 0, so from -0.5 to 0.5 on each axe.

	// fill 8 points to draw a cube align with axis of size size
	void	compute_cube_align_points( FLOAT3x8 & vec, FP32 size )
	{
		size *= .5;
		vec[0][0]	=	vec[1][0]	=	vec[2][0]	=	vec[3][0]	=	 size;
		vec[0][1]	=	vec[1][1]	=	vec[4][1]	=	vec[5][1]	=	 size;
		vec[0][2]	=	vec[3][2]	=	vec[4][2]	=	vec[7][2]	=	 size;

		size = -size;
		vec[4][0]	=	vec[5][0]	=	vec[6][0]	=	vec[7][0]	=	 size;
		vec[3][1]	=	vec[2][1]	=	vec[7][1]	=	vec[6][1]	=	 size;
		vec[2][2]	=	vec[1][2]	=	vec[6][2]	=	vec[5][2]	=	 size;
	}

	void init()
	{
		compute_cube_align_points( points_cube_xyz_cano, 1. );	// so we have a canonical cube ready to use

		// guf_box_base and guf_box are specialized for box with 6 faces: 6 x 4 points triangle strip
		guf_box_base.alloc_point( 24, __FUNCTION__ );
		guf_box     .alloc_point( 24, __FUNCTION__ );

		// uv by face
		FP32 src[4][2] = { 0,0, 1,0, 0,1, 1,1 };
		FP32* uv = guf_box_base.get_uv();
			cpy_v2( uv+2, src[0] );
			cpy_v2( uv+6, src[1] );
			cpy_v2( uv+4, src[3] );
			cpy_v2( uv+0, src[2] );
		uv+=8;
			cpy_v2( uv+2, src[2] );
			cpy_v2( uv+6, src[0] );
			cpy_v2( uv+4, src[1] );
			cpy_v2( uv+0, src[3] );
		uv+=8;
			cpy_v2( uv+2, src[2] );	//cpy_v2( uv+2, src[0] );
			cpy_v2( uv+6, src[0] );	//cpy_v2( uv+6, src[1] );
			cpy_v2( uv+4, src[1] );	//cpy_v2( uv+4, src[3] );
			cpy_v2( uv+0, src[3] );	//cpy_v2( uv+0, src[2] );
		uv+=8;
			cpy_v2( uv+2, src[1] );
			cpy_v2( uv+6, src[3] );
			cpy_v2( uv+4, src[2] );
			cpy_v2( uv+0, src[0] );
		uv+=8;
			cpy_v2( uv+2, src[3] );
			cpy_v2( uv+6, src[2] );
			cpy_v2( uv+4, src[0] );
			cpy_v2( uv+0, src[1] );
		uv+=8;
			cpy_v2( uv+2, src[3] );	//cpy_v2( uv+2, src[1] );
			cpy_v2( uv+6, src[2] );	//cpy_v2( uv+6, src[3] );
			cpy_v2( uv+4, src[0] );	//cpy_v2( uv+4, src[2] );
			cpy_v2( uv+0, src[1] );	//cpy_v2( uv+0, src[0] );
		cpy_vn( guf_box.get_uv(), guf_box_base.get_uv(), 24*2 );
			
		// normal by face
		FP32* nor = guf_box_base.get_normal();
			cpy_v3( nor+0, unit_x_v4fp32 );
			cpy_v3( nor+3, unit_x_v4fp32 );
			cpy_v3( nor+6, unit_x_v4fp32 );
			cpy_v3( nor+9, unit_x_v4fp32 );
		nor+=12;
			cpy_v3( nor+0, unit_y_v4fp32 );
			cpy_v3( nor+3, unit_y_v4fp32 );
			cpy_v3( nor+6, unit_y_v4fp32 );
			cpy_v3( nor+9, unit_y_v4fp32 );
		nor+=12;
			cpy_v3( nor+0, unit_z_v4fp32 );
			cpy_v3( nor+3, unit_z_v4fp32 );
			cpy_v3( nor+6, unit_z_v4fp32 );
			cpy_v3( nor+9, unit_z_v4fp32 );
		nor+=12;
			cpy_v3( nor+0, unit_x_neg_v4fp32 );
			cpy_v3( nor+3, unit_x_neg_v4fp32 );
			cpy_v3( nor+6, unit_x_neg_v4fp32 );
			cpy_v3( nor+9, unit_x_neg_v4fp32 );
		nor+=12;
			cpy_v3( nor+0, unit_y_neg_v4fp32 );
			cpy_v3( nor+3, unit_y_neg_v4fp32 );
			cpy_v3( nor+6, unit_y_neg_v4fp32 );
			cpy_v3( nor+9, unit_y_neg_v4fp32 );
		nor+=12;
			cpy_v3( nor+0, unit_z_neg_v4fp32 );
			cpy_v3( nor+3, unit_z_neg_v4fp32 );
			cpy_v3( nor+6, unit_z_neg_v4fp32 );
			cpy_v3( nor+9, unit_z_neg_v4fp32 );
	}

	void deinit()
	{
	}

	FINLINE	void	draw_box_0d( FLOAT3x8 CONST & vec )
	{
		//todo do we need these 2 lines
		GOL::disable_client_state_normal();
		GOL::disable_client_state_texcoor();

		GOL::set_pointer_vertex3( (FP32 CONST * CONST)vec );
		GOL::draw_elements( GL_TRIANGLE_STRIP, 18, GL_UNSIGNED_BYTE, (void*)box_strip );
//		GOL::draw_elements( GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_BYTE, (void*)box_strip_1 );
//		GOL::draw_elements( GL_TRIANGLE_STRIP, 8, GL_UNSIGNED_BYTE, (void*)box_strip_2 );
	}
};



namespace {
	// from 8 points of a box we will the array of 24 points for drawing the box with quads or triangle strip with unique point for each face (we can do better but it's ok for now)
	FINLINE void set_points_unique_for_box( FP32* dst, FLOAT3x8 CONST& src )
	{
		cpy_v3( dst+0, src[0] );	cpy_v3( dst+3, src[3] );	cpy_v3( dst+6, src[1] );	cpy_v3( dst+9, src[2]  );	dst+=12;
		cpy_v3( dst+0, src[0] );	cpy_v3( dst+3, src[1] );	cpy_v3( dst+6, src[4] );	cpy_v3( dst+9, src[5]  );	dst+=12;
		cpy_v3( dst+0, src[0] );	cpy_v3( dst+3, src[4] );	cpy_v3( dst+6, src[3] );	cpy_v3( dst+9, src[7]  );	dst+=12;
		cpy_v3( dst+0, src[6] );	cpy_v3( dst+3, src[7] );	cpy_v3( dst+6, src[5] );	cpy_v3( dst+9, src[4]  );	dst+=12;
		cpy_v3( dst+0, src[6] );	cpy_v3( dst+3, src[2] );	cpy_v3( dst+6, src[7] );	cpy_v3( dst+9, src[3]  );	dst+=12;
		cpy_v3( dst+0, src[6] );	cpy_v3( dst+3, src[5] );	cpy_v3( dst+6, src[2] );	cpy_v3( dst+9, src[1]  );
	}

	FINLINE	void	draw_box_with_guf( FLOAT3x8 CONST& vec, bool CONST b_normal, bool CONST b_uv )
	{
		if( b_normal || b_uv )
		{
			set_points_unique_for_box( box::guf_box_base.get_point(), vec );
#if 0
			box::guf_box.draw( GL_QUADS, 24, b_normal, b_uv );
#else
			box::guf_box_base.draw_before( b_normal, b_uv );
			GOL::draw_elements(	GL_TRIANGLE_STRIP,	34, GL_UNSIGNED_BYTE, box_uv_tri_strip );
			//GOL::draw_elements(	GL_QUADS,	24, GL_UNSIGNED_BYTE, box_faces_quads );
			box::guf_box_base.draw_after();
#endif
		}
		else
			box::draw_box_0d( vec );
	}


	FINLINE	void	draw_box_with_guf_oriented_normal( FLOAT3x8 CONST& vec, FP32 CONST * u, FP32 CONST * v, FP32 CONST * n )
	{
		FP32*	nor = box::guf_box.get_normal();
		cpy_v3( nor, v );	cpy_v3( nor+3, nor );	cpy_v3( nor+6, nor );	cpy_v3( nor+9, nor );	nor+=12;
		cpy_v3( nor, u );	cpy_v3( nor+3, nor );	cpy_v3( nor+6, nor );	cpy_v3( nor+9, nor );	nor+=12;
		neg_v3( nor, n );	cpy_v3( nor+3, nor );	cpy_v3( nor+6, nor );	cpy_v3( nor+9, nor );	nor+=12;
		neg_v3( nor, v );	cpy_v3( nor+3, nor );	cpy_v3( nor+6, nor );	cpy_v3( nor+9, nor );	nor+=12;
		neg_v3( nor, u );	cpy_v3( nor+3, nor );	cpy_v3( nor+6, nor );	cpy_v3( nor+9, nor );	nor+=12;
		cpy_v3( nor, n );	cpy_v3( nor+3, nor );	cpy_v3( nor+6, nor );	cpy_v3( nor+9, nor );

		set_points_unique_for_box( box::guf_box.get_point(), vec );
#if 0
		box::guf_box.draw( GL_QUADS, 24, true, false );
#else
		box::guf_box.draw_before( true, false );
		GOL::draw_elements(	GL_TRIANGLE_STRIP,	34, GL_UNSIGNED_BYTE, box_uv_tri_strip );
		box::guf_box_base.draw_after();
#endif
	}

//todoopt
typedef FP32	FLOAT3[3];
typedef FP32	FLOAT2[2];

	FINLINE	void	draw_box_uvw_low( UVWx8 CONST	&vec )
	{
		FLOAT3 CONST * uvw = (FLOAT3*)c_map::get_quad_uvw();
		bool	CONST b_normal = c_layer::get_cur()->is_normal_draw();

		GOL::begin( GL_TRIANGLES );
			if( b_normal )
				GOL::normal3v( unit_x_v4fp32 );
			GOL::texcoor3v( uvw[0] );	GOL::vertex3v( vec[0] );
			GOL::texcoor3v( uvw[3] );	GOL::vertex3v( vec[3] );
			GOL::texcoor3v( uvw[2] );	GOL::vertex3v( vec[2] );
			GOL::texcoor3v( uvw[0] );	GOL::vertex3v( vec[0] );
			GOL::texcoor3v( uvw[2] );	GOL::vertex3v( vec[2] );
			GOL::texcoor3v( uvw[1] );	GOL::vertex3v( vec[1] );

			if( b_normal )
				GOL::normal3v( unit_y_v4fp32 );
			GOL::texcoor3v( uvw[0] );	GOL::vertex3v( vec[0] );
			GOL::texcoor3v( uvw[1] );	GOL::vertex3v( vec[1] );
			GOL::texcoor3v( uvw[5] );	GOL::vertex3v( vec[5] );
			GOL::texcoor3v( uvw[0] );	GOL::vertex3v( vec[0] );
			GOL::texcoor3v( uvw[5] );	GOL::vertex3v( vec[5] );
			GOL::texcoor3v( uvw[4] );	GOL::vertex3v( vec[4] );

			if( b_normal )
				GOL::normal3v( unit_z_v4fp32 );
			GOL::texcoor3v( uvw[0] );	GOL::vertex3v( vec[0] );
			GOL::texcoor3v( uvw[4] );	GOL::vertex3v( vec[4] );
			GOL::texcoor3v( uvw[7] );	GOL::vertex3v( vec[7] );
			GOL::texcoor3v( uvw[0] );	GOL::vertex3v( vec[0] );
			GOL::texcoor3v( uvw[7] );	GOL::vertex3v( vec[7] );
			GOL::texcoor3v( uvw[3] );	GOL::vertex3v( vec[3] );

			if( b_normal )
				GOL::normal3v( unit_x_neg_v4fp32 );
			GOL::texcoor3v( uvw[6] );	GOL::vertex3v( vec[6] );
			GOL::texcoor3v( uvw[7] );	GOL::vertex3v( vec[7] );
			GOL::texcoor3v( uvw[4] );	GOL::vertex3v( vec[4] );
			GOL::texcoor3v( uvw[6] );	GOL::vertex3v( vec[6] );
			GOL::texcoor3v( uvw[4] );	GOL::vertex3v( vec[4] );
			GOL::texcoor3v( uvw[5] );	GOL::vertex3v( vec[5] );

			if( b_normal )
				GOL::normal3v( unit_y_neg_v4fp32 );
			GOL::texcoor3v( uvw[6] );	GOL::vertex3v( vec[6] );
			GOL::texcoor3v( uvw[2] );	GOL::vertex3v( vec[2] );
			GOL::texcoor3v( uvw[3] );	GOL::vertex3v( vec[3] );
			GOL::texcoor3v( uvw[6] );	GOL::vertex3v( vec[6] );
			GOL::texcoor3v( uvw[3] );	GOL::vertex3v( vec[3] );
			GOL::texcoor3v( uvw[7] );	GOL::vertex3v( vec[7] );

			if( b_normal )
				GOL::normal3v( unit_z_neg_v4fp32 );
			GOL::texcoor3v( uvw[6] );	GOL::vertex3v( vec[6] );
			GOL::texcoor3v( uvw[5] );	GOL::vertex3v( vec[5] );
			GOL::texcoor3v( uvw[1] );	GOL::vertex3v( vec[1] );
			GOL::texcoor3v( uvw[6] );	GOL::vertex3v( vec[6] );
			GOL::texcoor3v( uvw[1] );	GOL::vertex3v( vec[1] );
			GOL::texcoor3v( uvw[2] );	GOL::vertex3v( vec[2] );
		GOL::end();
	}

	template< bool B_NOR, bool B_UV >
	FINLINE void draw_box_xyz_old_school( FLOAT3x8 CONST& vec, FLOAT2 CONST* CONST uv )
	{
		if CONSTEXPR (!B_NOR && !B_UV)
		{
			GOL::begin( GL_TRIANGLE_STRIP );
				//3 faces
				GOL::vertex3v( vec[1] );
				GOL::vertex3v( vec[0] );

				GOL::vertex3v( vec[2] );
				GOL::vertex3v( vec[3] );

				GOL::vertex3v( vec[6] );
				GOL::vertex3v( vec[7] );

				GOL::vertex3v( vec[5] );
				GOL::vertex3v( vec[4] );
				// 2 degenerate triangles to connect the strip
				GOL::vertex3v( vec[4] );
				GOL::vertex3v( vec[7] );
				// then 3 faces
				GOL::vertex3v( vec[7] );
				GOL::vertex3v( vec[3] );

				GOL::vertex3v( vec[4] );
				GOL::vertex3v( vec[0] );

				GOL::vertex3v( vec[5] );
				GOL::vertex3v( vec[1] );

				GOL::vertex3v( vec[6] );
				GOL::vertex3v( vec[2] );
			GOL::end();
		}
		else
		{
			GOL::begin( GL_TRIANGLES );
				if CONSTEXPR (B_NOR) GOL::normal3v( unit_x_v4fp32 );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[0] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[3] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[2] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[0] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[2] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[1] );

				if CONSTEXPR (B_NOR) GOL::normal3v( unit_y_v4fp32 );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[0] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[1] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[5] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[0] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[5] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[4] );

				if CONSTEXPR (B_NOR) GOL::normal3v( unit_z_v4fp32 );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[0] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[4] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[7] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[0] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[7] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[3] );

				if CONSTEXPR (B_NOR) GOL::normal3v( unit_x_neg_v4fp32 );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[6] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[7] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[4] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[6] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[4] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[5] );

				if CONSTEXPR (B_NOR) GOL::normal3v( unit_y_neg_v4fp32 );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[6] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[2] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[3] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[6] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[3] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[7] );

				if CONSTEXPR (B_NOR) GOL::normal3v( unit_z_neg_v4fp32 );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[6] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[5] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[1] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[6] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[1] );
				if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[2] );
			GOL::end();
		}
	}

	FINLINE	void	draw_box_from_points( FLOAT3x8 CONST &vec )
	{
		if( GOL::b_draw_avoid_vertex_use )
		{
			//todo deal with changing uv
			draw_box_with_guf( vec, c_layer::get_cur()->is_normal_draw(), c_layer::get_cur()->is_need_uv() );
		}
		else
		{
			if( c_layer::get_cur()->is_need_uv() )
			{
				FLOAT2 CONST* uv = (FLOAT2*) c_map::get_quad_uv();
				if( c_layer::get_cur()->is_normal_draw() )
					draw_box_xyz_old_school< true, true >( vec, uv );
				else
					draw_box_xyz_old_school< false, true >( vec, uv );
			}
	//		else
	//		{
	//		if( GOL::b_draw_avoid_vertex_use )
	//		{
	//			GOL::unbind_vao_secu();
	//			//	avoid enable_client/disable_client for each box, drawing of several cube will be way faster
	//			//GOL::enable_client_state_vertex();
	//			GOL::set_pointer_vertex3( vec[0] );
	//			if( c_layer::get_cur()->is_normal_draw() )
	//			{
	//				GOL::normal3v( unit_x_v4fp32 );
	//				GOL::draw_elements(	GL_TRIANGLE_STRIP,	4, GL_UNSIGNED_BYTE, box_uv_tri_strip );

	//				GOL::normal3v( unit_y_v4fp32 );
	//				GOL::draw_elements(	GL_TRIANGLE_STRIP,	4, GL_UNSIGNED_BYTE, box_uv_tri_strip + 6 );

	//				GOL::normal3v( unit_z_v4fp32 );
	//				GOL::draw_elements(	GL_TRIANGLE_STRIP,	4, GL_UNSIGNED_BYTE, box_uv_tri_strip + 12 );

	//				GOL::normal3v( unit_x_v4fp32 );
	//				GOL::draw_elements(	GL_TRIANGLE_STRIP,	4, GL_UNSIGNED_BYTE, box_uv_tri_strip + 18 );

	//				GOL::normal3v( unit_y_v4fp32 );
	//				GOL::draw_elements(	GL_TRIANGLE_STRIP,	4, GL_UNSIGNED_BYTE, box_uv_tri_strip + 24 );

	//				GOL::normal3v( unit_z_v4fp32 );
	//				GOL::draw_elements(	GL_TRIANGLE_STRIP,	4, GL_UNSIGNED_BYTE, box_uv_tri_strip + 30 );
	//			}
	//			else
	//			{
	//				GOL::draw_elements(	GL_TRIANGLE_STRIP,	34, GL_UNSIGNED_BYTE, box_uv_tri_strip );
	//			}
	////			GOL::disable_client_state_vertex();
	//		}
			else
			{
				if( c_layer::get_cur()->is_normal_draw() )
					draw_box_xyz_old_school< true, false >( vec, nullptr );
				else
					draw_box_xyz_old_school< false, false >( vec, nullptr );
			}
		}
	}
}

template< bool B_NOR, bool B_UV >
void draw_box_triangles_old_school( FLOAT3x8 CONST & vec, FLOAT2 CONST * CONST uv, FP32 CONST * CONST u, FP32 CONST * CONST v, FP32 CONST * CONST nor )
{
	FP32 v_tmp[3];

	GOL::begin( GL_TRIANGLES );
		if CONSTEXPR (B_NOR)
			GOL::normal3v( v );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[0] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[3] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[2] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[1] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[0] );	
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[2] );

		if CONSTEXPR (B_NOR)
			GOL::normal3v( u );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[0] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[1] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[5] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[4] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[0] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[5] );

		if CONSTEXPR (B_NOR)
		{
			neg_v3( v_tmp, nor );
			GOL::normal3v( v_tmp );
		}
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[0] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[4] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[7] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[3] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[0] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[7] );

		if CONSTEXPR (B_NOR)
		{
			neg_v3( v_tmp, v );
			GOL::normal3v( v_tmp );
		}
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[6] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[7] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[4] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[5] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[6] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[4] );

		if CONSTEXPR (B_NOR)
		{
			neg_v3( v_tmp, u );
			GOL::normal3v( v_tmp );
		}
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[6] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[2] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[3] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[7] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[6] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[3] );

		if CONSTEXPR (B_NOR)
			GOL::normal3v( nor );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[6] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[3] );	GOL::vertex3v( vec[5] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[1] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[2] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[1] );	GOL::vertex3v( vec[6] );
		if CONSTEXPR (B_UV) GOL::texcoord2v( uv[2] );	GOL::vertex3v( vec[1] );
	GOL::end();
}

void	draw_box_at_tgn( FP32 CONST * CONST size, FP32 CONST * CONST pos, FP32 CONST * CONST u, FP32 CONST * CONST v, FP32 CONST * CONST nor )
{
	FLOAT3x8	vec;

	FP32	point[3];
	FP32	us[3];
	FP32	vs[3];
	FP32	ns[3];

//FP32	siz[3];
//	scale_v3( siz, size, .5);
	scale_v3( us, u, size[0] );
	scale_v3( vs, v, size[1] );
	scale_v3( ns, nor, size[2] );

	add_scale_v3( point, pos, ns, FP32(-.5) );
	add_scale_v3( point, us, FP32(-.5) );
	add_scale_v3( point, vs, FP32(-.5) );

	cpy_v3( vec[7], point );
	add_v3( vec[6], vec[7], ns );

	add_v3( vec[4], point, us );
	add_v3( vec[5], vec[4], ns ) ;

	add_v3( vec[0], vec[4], vs );
	add_v3( vec[1], vec[0], ns );

	add_v3( vec[3], vec[7], vs );
	add_v3( vec[2], vec[3], ns );

	if( c_layer::get_cur()->is_need_uv() )
	{
		FLOAT2 CONST *	uv = (FLOAT2*) c_map::get_quad_uv();

		if( c_layer::get_cur()->is_normal_draw() )
			draw_box_triangles_old_school< true, true >( vec, uv, u,v,nor );
		else
			draw_box_triangles_old_school< false, true >( vec, uv, u,v,nor );
	}
	else
	{
		if( GOL::b_draw_avoid_vertex_use )
		{
			if( c_layer::get_cur()->is_normal_draw() )
			{
#if 1
				draw_box_with_guf_oriented_normal( vec, u, v, nor  );
#else
				GOL::unbind_vao_secu();
				//	avoid enable_client/disable_client for each box, drawing of several cube will be way faster
				//GOL::enable_client_state_vertex();
				GOL::set_pointer_vertex3( vec[0] );

				FP32 v_tmp[3];

				GOL::normal3v( v );
				GOL::draw_elements(	GL_QUADS,	4, GL_UNSIGNED_BYTE, box_faces_quads );

				GOL::normal3v( u );
				GOL::draw_elements(	GL_QUADS,	4, GL_UNSIGNED_BYTE, box_faces_quads + 4 );

				neg_v3( v_tmp, nor );
				GOL::normal3v( v_tmp );
				GOL::draw_elements(	GL_QUADS,	4, GL_UNSIGNED_BYTE, box_faces_quads + 8 );

				neg_v3( v_tmp, v );
				GOL::normal3v( v_tmp );
				GOL::draw_elements(	GL_QUADS,	4, GL_UNSIGNED_BYTE, box_faces_quads + 12 );

				neg_v3( v_tmp, u );
				GOL::normal3v( v_tmp );
				GOL::draw_elements(	GL_QUADS,	4, GL_UNSIGNED_BYTE, box_faces_quads + 16 );

				GOL::normal3v( nor );
				GOL::draw_elements(	GL_QUADS,	4, GL_UNSIGNED_BYTE, box_faces_quads + 20 );
#endif
			}
			else
			{
				GOL::unbind_vao_secu();
				box::draw_box_0d( vec );
			}
		}
		else
		{
			if( c_layer::get_cur()->is_normal_draw() )
				draw_box_triangles_old_school< true, false >( vec, nullptr, u,v,nor );
			else
				draw_box_triangles_old_school< false, false >( vec, nullptr, u,v,nor );
		}
	}
}

namespace {
	void	set_box_min_max( FLOAT3x8 &	vec, FP32 CONST min_x, FP32 CONST min_y, FP32 CONST min_z, FP32 CONST max_x, FP32 CONST max_y, FP32 CONST max_z )
	{
		vec[0][0]	=	vec[1][0]	=	vec[2][0]	=	vec[3][0]	=	 max_x;
		vec[4][0]	=	vec[5][0]	=	vec[6][0]	=	vec[7][0]	=	 min_x;

		vec[0][1]	=	vec[1][1]	=	vec[4][1]	=	vec[5][1]	=	 max_y;
		vec[3][1]	=	vec[2][1]	=	vec[7][1]	=	vec[6][1]	=	 min_y;

		vec[0][2]	=	vec[3][2]	=	vec[4][2]	=	vec[7][2]	=	 max_z;
		vec[2][2]	=	vec[1][2]	=	vec[6][2]	=	vec[5][2]	=	 min_z;
	}
}

static	FLOAT3x8	vec_box;
void	draw_box_min_max( FP32 CONST min_x, FP32 CONST min_y, FP32 CONST min_z, FP32 CONST max_x, FP32 CONST max_y, FP32 CONST max_z )
{
	FLOAT3x8&	vec = vec_box;
	set_box_min_max( vec_box, min_x, min_y, min_z, max_x, max_y, max_z );

//	if( c_map::get_cur()->is_3d() && c_layer::get_cur()->is_need_uv() )
//	if( GOL::get_tex_unit_cur()->is_dim( 3 ) && c_layer::get_cur()->is_need_uv() )
	if( GOL::get_tex_unit_cur()->is_dim( 3 ) )
	{
		draw_box_uvw_low( vec );
		return;
	}
	draw_box_from_points( vec );
}

void	draw_box_min_max_line( FP32 CONST min_x, FP32 CONST min_y, FP32 CONST min_z, FP32 CONST max_x, FP32 CONST max_y, FP32 CONST max_z )
{
	FLOAT3x8&	vec = vec_box;
	set_box_min_max( vec_box, min_x, min_y, min_z, max_x, max_y, max_z );

	GOL::unbind_vao_secu();
	//	avoid enable_client/disable_client for each box, drawing of several cube will be way faster
	//GOL::enable_client_state_vertex();
	GOL::set_pointer_vertex3( vec[0] );
	//faster with quad with Maa quadro K5100M
	GOL::draw_elements(	GL_LINES,	24, GL_UNSIGNED_BYTE, box_lines_index );
	//GOL::disable_client_state_vertex();
}

void	draw_box_min_max_top_line( FP32 CONST min_x, FP32 CONST min_y, FP32 CONST min_z, FP32 CONST max_x, FP32 CONST max_y, FP32 CONST max_z )
{
	FLOAT3x8&	vec = vec_box;
	set_box_min_max( vec_box, min_x, min_y, min_z, max_x, max_y, max_z );

	GOL::unbind_vao_secu();
	//	avoid enable_client/disable_client for each box, drawing of several cube will be way faster
	//GOL::enable_client_state_vertex();
	GOL::set_pointer_vertex3( vec[0] );
	//faster with quad with Maa quadro K5100M
	GOL::draw_elements(	GL_LINES,	16, GL_UNSIGNED_BYTE, &box_lines_index[8] );
	//GOL::disable_client_state_vertex();
}

void	draw_cube( FP32 CONST size )
{
static	FLOAT3x8	points_cube;
static	FP32		cube_size_last = FP32(-42.42424242);

	if( cube_size_last != size )
	{
		cube_size_last = size;
		box::compute_cube_align_points( points_cube, size );
	}
	draw_box_from_points( points_cube );
}

void	draw_cube_cano()
{
	draw_box_from_points( box::points_cube_xyz_cano );
}
