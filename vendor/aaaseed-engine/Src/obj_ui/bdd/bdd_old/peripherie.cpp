#include "peripherie.h"
#include "infrastructure/layer/layer.h"
#include "obj_ui/tracker/neat.h"
#include "draw/seedcam.h"
#include "math/rand.h"
#include "draw/lights.h"
#include "infrastructure/layer/layers.h"

FACTORY_CREATE_V1( c_peri, peri, Peripherie, peri );

c_peri*	peri = NULL;
REAL	peri_dist = 0;

#define	LAYERS_RAIN			1
#define	LAYERS_RENDERING	2
#define	LAYERS_ARRAY		5

#define	LAYER_RED			0
#define	LAYER_GREEN			1
#define	LAYER_BLUE			2


namespace	n_peri
{
	static	CONST	INT32	BASE_PARAM_NB		= 33;
	static	CONST	INT32	GROUP_PARAM_NB		= 0;
	static	CONST	INT32	PARAM_NB_MAX	 =	BASE_PARAM_NB
											+	GROUP_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_BOOL_OFF( neat_active )
		{	(void *) NULL,	PARAM_REAL,	"rotation_speed",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"translation_speed",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		ST_PARAM_BOOL_OFF( star_red )
		ST_PARAM_BOOL_OFF( star_green )
		ST_PARAM_BOOL_OFF( star_blue )
		{	(void *) NULL,	PARAM_INT32,	"render_choice",	1., 0.,	0., 2., NULL, NULL },
		ST_PARAM_BOOL_OFF( rain )
		{	(void *) NULL,	PARAM_REAL,	"peri_dist",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		ST_PARAM_BOOL_OFF( init )
		ST_PARAM_BOOL_OFF( tex_1 )
		ST_PARAM_BOOL_OFF( tex_2 )
		ST_PARAM_BOOL_OFF( tex_3 )
		ST_PARAM_BOOL_OFF( left )
		ST_PARAM_BOOL_OFF( right )
		ST_PARAM_BOOL_OFF( up )
		ST_PARAM_BOOL_OFF( down )

		{	(void *) NULL,	PARAM_REAL,	"red_light",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"red_red",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"red_green",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"red_blue",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"red_grey",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		{	(void *) NULL,	PARAM_REAL,	"green_light",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"green_red",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"green_green",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"green_blue",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"green_grey",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		{	(void *) NULL,	PARAM_REAL,	"blue_light",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"blue_red",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"blue_green",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"blue_blue",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,	"blue_grey",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
	};
}

void	c_peri::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, b_active);
	param_set_pt( h, b_neat_active);
	param_set_pt( h, rot_speed);
	param_set_pt( h, tra_speed);

	param_set_pt( h, b_red);
	param_set_pt( h, b_green);
	param_set_pt( h, b_blue);
	param_set_pt( h, render_choice);
	param_set_pt( h, b_rain);
	param_set_pt( h, peri_dist);

	param_set_pt( h, b_init);
	param_set_pt( h, b_tex_1);
	param_set_pt( h, b_tex_2);
	param_set_pt( h, b_tex_3);
	param_set_pt( h, b_left);
	param_set_pt( h, b_right);
	param_set_pt( h, b_up);
	param_set_pt( h, b_down);

	for( INT32 i=0; i>3; ++i )
	{
		param_set_pt( h, light_intensity[i] );
		param_set_pt( h, red[i] );
		param_set_pt( h, green[i] );
		param_set_pt( h, blue[i] );
		param_set_pt( h, grey[i] );
	}

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_peri)
{
	param_init_with( n_peri::param, n_peri::PARAM_NB_MAX ); // peri_param, PERI_PARAM_NB_MAX);
}

c_peri::~c_peri()
{
}

void	c_peri::update()
{
	INT32	val[4];
	INT32	tmp;
	c_layer	* a_layer;	 

	if( b_neat_active)
	{
		INT32	count;

		val[0] =  get_neat( 0);
		val[1] =  get_neat( 1);
		val[2] =  get_neat( 2);
		val[3] =  get_neat( 3);
		
		b_init = (val[0] >= 180);

		tmp = val[3];
		b_left = b_right = 0;
		if ( tmp < 208 && tmp > 120 )
		{
			if ( tmp > 179 )
				b_left = TRUE;
			else
				b_right = TRUE;
		}

		tmp = val[2];
		b_up = b_down = 0;
		if ( tmp < 208 && tmp > 120 )
		{
			if ( tmp > 179 )
				b_down = TRUE;
			else
				b_up = TRUE;
		}

		tmp = val[1];
		b_tex_1 = b_tex_2 = b_tex_3 = 0;
		count = 0;
		if ( tmp > 187 )
		{
			b_tex_1 = TRUE;
			count++;
			if( tmp > 207 )
			{
				if ( tmp < 212 )
				{
					b_tex_3 = TRUE;
					count++;
				}
				else
				{
					b_tex_2 = TRUE;
					count++;
					if ( tmp > 216 )
					{
						b_tex_3 = TRUE;
						count++;
					}
				}
			}
		}
		else
		{
			if( tmp > 125 )
			{
				b_tex_2 = TRUE;
				count++;
				if ( tmp > 161 )
				{
					b_tex_3 = TRUE;
					count++;
				}
			}
			else if ( tmp > 50 )
			{
				b_tex_3 = TRUE;
				count++;
			}
		}
		if( count == 1 )
		{
			b_green = b_tex_1;
			b_blue = b_tex_2;
			b_red = b_tex_3;
			if( b_tex_3 )
				render_choice = 0;
			else if( b_tex_1 )
				render_choice = 1;
			else if( b_tex_2 )
				render_choice = 2;
			b_rain = FALSE;
		}
		else if ( b_init )
			b_rain = TRUE;
	}

	if ( b_active )
	{
		if( c_seedcam::ui )
			c_seedcam::ui->set_dist_to_center(0);

		g_lights_def.set_intensity_factor( light_intensity[render_choice]);
		g_lights_def.set_ambient_5f( red[render_choice], green[render_choice], blue[render_choice], 1., grey[render_choice]);
		g_lights_def.set();


/*		if ( b_init && b_tex_1 && b_tex_2 && b_tex_3 )
		{
			feedback_cur->set_active( TRUE);
			layer_array[ LAYER_RAIN_1].b_feedback = TRUE;
		}
		else
		{
			feedback_cur->set_active( FALSE);
			layer_array[ LAYER_RAIN_1].b_feedback = FALSE;
		}
*/

		layers_array[LAYERS_RAIN].set_active( b_rain);
//		layers_array[LAYERS_RAIN]->layer_get_always(LAYER_RAIN_0)->set_active( b_rain);
//		layers_array[LAYERS_RAIN]->layer_get_always(LAYER_RAIN_0)->set_active( b_rain);
		layers_array[LAYERS_ARRAY].set_active( !b_rain);
		
		if( !b_rain)
		{
			c_layer	* layer_src;	 

			layers_array[LAYERS_RENDERING].layer_get_always(LAYER_RED)->set_active( b_red);
			layers_array[LAYERS_RENDERING].layer_get_always(LAYER_GREEN)->set_active( b_green);
			layers_array[LAYERS_RENDERING].layer_get_always(LAYER_BLUE)->set_active( b_blue);
//			layer_array[ LAYER_RED].set_active( b_red);
//			layer_array[ LAYER_GREEN].set_active( b_green);
//			layer_array[ LAYER_BLUE].set_active( b_blue);
			layer_src = layers_array[LAYERS_RENDERING].layer_get_always(render_choice);
//			layer_src = &layer_array[ LAYER_RED + render_choice];
			a_layer = layers_array[LAYERS_ARRAY].layer_get_always(0);			
//			a_layer = &layer_array[ LAYER_ARRAY];
			a_layer->_bind_1d_ui = layer_src->_bind_1d_ui;
			a_layer->_bind_2d_ui = layer_src->_bind_2d_ui;
			a_layer->_mat_front_ui = layer_src->_mat_front_ui;
			a_layer->_mat_back_ui = layer_src->_mat_back_ui;
		}

		REAL	f;
	//todo make fn for this
		delta_t.update();
		dt = delta_t.get_dt();

		f = dt * rot_speed;
		if ( b_left )
			g_camera_cur->add_to_yaw( -f );
		else if ( b_right )
			g_camera_cur->add_to_yaw( f );
		f = dt * tra_speed;
		if ( b_up )
			peri_dist -= f;
		else if ( b_down )
			peri_dist += f;
	}


}

