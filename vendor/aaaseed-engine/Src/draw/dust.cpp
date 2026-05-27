#include "err.h"
#include "time/aaa_time.h"
#include "gol/gol.h"
#include "gol/gol_matrix.h"
#include "gol/gol_draw.h"
#include "gol/gol_color.h"
#include "obj_ui/deformer/deformer.h"
#include "math/rand.h"
#include "obj_ui/deformer/def_node.h"
#include "image/img.h"

#if 1
void	dust_draw( c_img_2d* image, INT32 nb, REAL size_cloud )
{
	REAL time = REAL(aaa::time::get());

	SRAND(85485);

	auto def = c_def_node::get_cur();
	if( !def->is_deforming() )
		def = nullptr;

	GOL::set_point_size(4.);
	GOL::color_white();
	{
		for( INT32 i = nb; i > 0; --i )
		{
			GOL::begin( GL_POINTS );
			for( INT32 j = nb; j > 0; --j )
			{
				REAL	f[3];
				f[0] = RAND_F() * size_cloud;
				f[1] = RAND_F() * size_cloud;
				f[2] = RAND_F() * size_cloud;
				
//				f[0] += SIN_RAD( cy * time);
//				f[1] += SIN_RAD( cz * time);
//				f[2] += SIN_RAD( tx * time);

				if( def )
					def->apply( f, 1 );
				GOL::vertex3v( f );
			}
			GOL::end();
		}
	}
}

#else
void	dust_draw( c_img_2d* image, INT32 nb, REAL size_cloud)
{
	REAL	size_dust;
	GLfloat	axes_ambuse[] = { 0.0, 1.0, 0.0, 1.0 };

	SRAND(854885);
	size_dust = size_cloud/10.;

	GOL::set_modelview();

/*	GOL::begin(GL_LINES);
		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p_axe->red_color);
		GOL::vertex3(15, 0, 0);	GOL::vertex3(-15, 0, 0);
		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p_axe->green_color);
		GOL::vertex3(0, 15, 0);	GOL::vertex3(0, -15, 0);
		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p_axe->blue_color);
		GOL::vertex3(0, 0, 15);	GOL::vertex3(0, 0, -15);
	GOL::end();
*/
#if	0
	for ( nb; nb>0; --nb )
	{
		
//		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &p_axe->red_color[i*4]);
		GOL::matrix::push();
			GOL::translate( RAND_F()*size_cloud, RAND_F()*size_cloud, RAND_F()*size_cloud );
			GOL::scale( RAND_F()*size_dust, RAND_F()*size_dust, 1.0);
			face_draw(image);
		GOL::matrix::pop();
	}
#else
		
//		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &p_axe->red_color[i*4]);
	if( image )
	{
		FP32	ratio = image->size_x/image->size_y;

		GOL::begin( GL_TRIANGLES );
		GOL::normal3v( unit_z_v4fp32 );
		for ( nb; nb>0; --nb )
		{
			FP32	cx = RAND_F()*size_cloud;
			FP32	cy = RAND_F()*size_cloud;
			FP32	cz = RAND_F()*size_cloud;
			FP32	ty = RAND_F()*size_dust;
			FP32	tx = ty*ratio;
			FP32	xl = cx-tx;
			FP32	xr = cx+tx;
			FP32	yb = cy-ty;
			FP32	yt = cy+ty;
			GOL::texcoord2v( zero_v3r );
			GOL::vertex3f( xl, yb, cz );
			GOL::texcoord2v( unit_x_v4fp32 );
			GOL::vertex3f( xr, yb, cz );
			GOL::texcoord2v( one_v4fp32 );
			GOL::vertex3f( xr, yt, cz );
			GOL::texcoord2v( zero_v3r );
			GOL::vertex3f( xl, yb, cz );
			GOL::texcoord2v( one_v4fp32 );
			GOL::vertex3f( xr, yt, cz );
			GOL::texcoord2v( unit_y_v4fp32 );
			GOL::vertex3f( xl, yt, cz );
		}
		GOL::end();
	}
	else
	{
		GOL::begin( GL_TRIANGLES );
		for ( nb; nb>0; --nb )
		{
			FP32	cx = RAND_F()*size_cloud;
			FP32	cy = RAND_F()*size_cloud;
			FP32	cz = RAND_F()*size_cloud;
			FP32	tx = RAND_F()*size_dust;
			FP32	ty = RAND_F()*size_dust;
			FP32	xl = cx-tx;
			FP32	xr = cx+tx;
			FP32	yb = cy-ty;
			FP32	yt = cy+ty;
			GOL::vertex3f( xl, yb, cz );
			GOL::vertex3f( xr, yb, cz );
			GOL::vertex3f( xr, yt, cz );
			GOL::vertex3f( xl, yb, cz );
			GOL::vertex3f( xr, yt, cz );
			GOL::vertex3f( xl, yt, cz );
		}
		GOL::end();
	}
#endif
}

void	dust_show_draw( c_img_2d* image, INT32 nb, REAL size_cloud )
{
	SRAND(85485);

	GOL::set_point_size(4.);
	GOL::color_white3();

	{
		REAL	cx, cy, cz;
		REAL	tx, ty, tz;

		for( INT32 i = nb; i > 0; --i )
		{
			for( INT32 j = nb; j > 0; --j )
			{
				cx = RAND_F()*size_cloud;
				cy = RAND_F()*size_cloud;
				cz = RAND_F()*size_cloud;
				
				REAL time = aaa::time::get();
				GOL::begin(GL_LINE_STRIP);
				for( INT32 k = 100; k > 0; --k )
				{
					tx = cx + SIN_RAD( cy * time );
					ty = cy + SIN_RAD( cz * time );
					tz = cz + SIN_RAD( cx * time );
					GOL::vertex3( tx,ty,tz );
					time += time_step;	//time_step is undefined
				}
				GOL::end();
			}
		}
	}
}
#endif

void	dust_new_draw( c_img_2d* image, INT32 nb, REAL size_cloud )
{
	REAL	time;

	time = REAL(aaa::time::get());

	SRAND(85485);

	GOL::set_point_size(4.);
	GOL::color_white();

	{
		for( INT32 i = nb; i > 0; --i )
		{
			GOL::begin( GL_POINTS);
			for( INT32 j = nb; j > 0; --j )
			{
				FP32	cx = RAND_F()*size_cloud;
				FP32	cy = RAND_F()*size_cloud;
				FP32	cz = RAND_F()*size_cloud;
				FP32	tx = cx;
				cx += SIN_RAD( cy * time);
				cy += SIN_RAD( cz * time);
				cz += SIN_RAD( tx * time);
				GOL::vertex3f( cx, cy, cz );
			}
			GOL::end();
		}
	}
}

void	dust_prev_draw( c_img_2d* image, INT32 nb, REAL size_cloud )
{
	REAL	size_dust;
	//GLfloat	axes_ambuse[] = { 0.0, 1.0, 0.0, 1.0 };

	SRAND(85485);
	size_dust = size_cloud / REAL(100);

	GOL::matrix::set_modelview();

/*	GOL::begin(GL_LINES);
		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p_axe->red_color);
		GOL::vertex3(15, 0, 0);	GOL::vertex3(-15, 0, 0);
		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p_axe->green_color);
		GOL::vertex3(0, 15, 0);	GOL::vertex3(0, -15, 0);
		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, p_axe->blue_color);
		GOL::vertex3(0, 0, 15);	GOL::vertex3(0, 0, -15);
	GOL::end();
*/
#if	0
	nb *= nb;
	for ( nb; nb>0; --nb )
	{
		
//		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &p_axe->red_color[i*4]);
		GOL::matrix::push();
			GOL::translate( RAND_F()*size_cloud, RAND_F()*size_cloud, RAND_F()*size_cloud );
			GOL::scale( RAND_F()*size_dust, RAND_F()*size_dust, 1.0);
			face_draw(image);
		GOL::matrix::pop();
	}
#else
		
//		GOL::materialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, &p_axe->red_color[i*4]);
	if( image )
	{
		GOL::normal3v( unit_z_v4fp32 );
		GOL::begin( GL_TRIANGLES );
		for ( INT32 i = nb; i > 0; --i )
		{
			for ( INT32 j = nb; j > 0; --j )
			{
				FP32	cx = RAND_F() * size_cloud;
				FP32	cy = RAND_F() * size_cloud;
				FP32	cz = RAND_F() * size_cloud;
				FP32	tx = RAND_UF() * size_dust;
				FP32	ty = RAND_UF() * size_dust;
				FP32	bx = cx-tx;
				FP32	by = cy-ty;
				GOL::texcoord2v( zero_v4fp32 );
				GOL::vertex3f( bx,    by,    cz );
				tx = RAND_UF() * size_dust;
				ty = RAND_UF() * size_dust;
				GOL::texcoord2v( unit_x_v4fp32 );
				GOL::vertex3f( cx+tx, cy-ty, cz );
				tx = RAND_UF() * size_dust;
				ty = RAND_UF() * size_dust;
				FP32	tx2 = cx+tx;
				FP32	ty2 = cy+ty;
				GOL::texcoord2v( one_v4fp32 );
				GOL::vertex3f( tx2,   ty2,   cz );
				GOL::texcoord2v( zero_v4fp32 );
				GOL::vertex3f( bx,    by,    cz );
				GOL::texcoord2v( one_v4fp32 );
				GOL::vertex3f( tx2,   ty2,   cz );
				tx = RAND_UF() * size_dust;
				ty = RAND_UF() * size_dust;
				GOL::texcoord2v( unit_y_v4fp32 );
				GOL::vertex3f( cx-tx, cy+ty, cz );
			}
		}
		GOL::end();
	}
	else
	{
		nb *= nb;
		GOL::begin( GL_TRIANGLES );
		for ( ; nb>0; --nb )
		{
			FP32	cx = RAND_F() * size_cloud;
			FP32	cy = RAND_F() * size_cloud;
			FP32	cz = RAND_F() * size_cloud;
			FP32	tx = RAND_F() * size_dust;
			FP32	ty = RAND_F() * size_dust;
			FP32	xl = cx-tx;
			FP32	xr = cx+tx;
			FP32	yb = cy-ty;
			FP32	yt = cy+ty;
			GOL::vertex3f( xl, yb, cz );
			GOL::vertex3f( xr, yb, cz );
			GOL::vertex3f( xr, yt, cz );
			GOL::vertex3f( xl, yb, cz );
			GOL::vertex3f( xr, yt, cz );
			GOL::vertex3f( xl, yt, cz );
		}
		GOL::end();
	}
#endif
}

