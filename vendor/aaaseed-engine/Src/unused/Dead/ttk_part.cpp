#include "ttk_part.h"

REAL	c_ttk_particle::get_preroll_time()
{
	if ( b_flow_continuous)
		{
		return life_time * ( 1 + life_time_jitter) ;
		}
	return	0.;
}

//todok	should
void	c_ttk_particle::set_from_data_img_emission_mask( UINT8 * data_in, INT32 size_x_in, INT32 size_y_in, INT32 channel_nb_in)
{
c_img* img;
	img = img_init_from_mem( NULL,
								size_x_in, size_y_in, channel_nb_in, IMG_TYPE_RGB,
								data_in, 0);
	set_img_emission_mask( img);
}

void	c_ttk_particle::set_from_data_img_emission_map( UINT8 * data_in, INT32 size_x_in, INT32 size_y_in, INT32 channel_nb_in)
{
c_img* img;
	img = img_init_from_mem( NULL,
								size_x_in, size_y_in, channel_nb_in, IMG_TYPE_RGB,
								data_in, 0);
	set_img_emission_map( img);
}

void	c_ttk_particle::set_from_data_img_life_color_map( UINT8 * data_in, INT32 size_x_in, INT32 size_y_in, INT32 channel_nb_in)
{
c_img* img;
	img = img_init_from_mem( NULL,
								size_x_in, size_y_in, channel_nb_in, IMG_TYPE_RGB,
								data_in, 0);
	set_img_life_color_map( img);
}
