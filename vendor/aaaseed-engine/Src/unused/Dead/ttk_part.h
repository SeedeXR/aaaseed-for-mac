
#ifdef AAA_TTK_PART_H
#error "TTK_PART_H included more than once."
#endif
#define AAA_TTK_PART_H 1


#ifndef AAA_BDD_PART_H
#	include "bdd_part.h"
#endif

//extern	ST_PARAM	bdd_particle_param[];	//par
extern	ST_PARAM	n_bdd_particle::param[];	//par
typedef enum TTK_PART_PARAM
	{
	TTK_PART_NB_ALLOCATED = 0,
	TTK_PART_NB_MAX,
	TTK_PART_NB_BY_SEC,
	TTK_PART_FLOW_CONTINUOUS,

	TTK_PART_LIFE_TIME,
	TTK_PART_LIFE_TIME_JITTER,
	TTK_PART_LIFE_TIME_JITTER_GAIN,
	TTK_PART_LIFE_TIME_JITTER_BIAS,

	TTK_PART_LIVING_BOX_SIZE_X,
	TTK_PART_LIVING_BOX_SIZE_Y,
	TTK_PART_LIVING_BOX_SIZE_Z,
	TTK_PART_LIVING_BOX_SIZE_FACTOR,
	TTK_PART_LIVING_BOX_WRAP,
	TTK_PART_LIVING_BOX_WRAP_RANDOM,

	TTK_PART_EMISSION_AXE,
	TTK_PART_EMISSION_BOX_SIZE_U,
	TTK_PART_EMISSION_BOX_SIZE_V,
	TTK_PART_EMISSION_BOX_SIZE_AXE,
	TTK_PART_EMISSION_BOX_SIZE_FACTOR,
	TTK_PART_EMISSION_BOX_SPHERE,
	TTK_PART_EMISSION_USE_MASK,
	TTK_PART_EMISSION_USE_MASK_GREY,
	TTK_PART_EMISSION_MASK_MIN,
	TTK_PART_EMISSION_MASK_MAX,
	TTK_PART_EMISSION_MASK_TRY_MAX,
	TTK_PART_EMISSION_USE_IMAGE,
	TTK_PART_EMISSION_MAP_U,
	TTK_PART_EMISSION_MAP_V,
	TTK_PART_EMISSION_SPEED_U,
	TTK_PART_EMISSION_SPEED_V,
	TTK_PART_EMISSION_SPEED_AXE,
	TTK_PART_EMISSION_SPEED_FACTOR,
	TTK_PART_EMISSION_OBJECT_SPEED_FACTOR,

	TTK_PART_JITTER_CONE_ANGLE,
	TTK_PART_JITTER_SPEED_ABS,
	TTK_PART_JITTER_JITTER,
	TTK_PART_JITTER_JITTER_GAIN,
	TTK_PART_JITTER_JITTER_BIAS,
	TTK_PART_JITTER_VEC_NB,
	TTK_PART_JITTER_SEED,

	TTK_PART_ACCEL_X,
	TTK_PART_ACCEL_Y,
	TTK_PART_ACCEL_Z,

	TTK_PART_TURB_ACCEL,
	TTK_PART_TURB_SPEED,
	TTK_PART_TURB_FRACTAL_SUM,
	TTK_PART_TURB_FREQ,
	TTK_PART_TURB_HARM_NB,
	TTK_PART_TURB_GAIN,
	TTK_PART_TURB_BIAS,
	TTK_PART_TURB_OFFSET_X,
	TTK_PART_TURB_OFFSET_Y,
	TTK_PART_TURB_OFFSET_Z,
	TTK_PART_TURB_FACTOR_X,
	TTK_PART_TURB_FACTOR_Y,
	TTK_PART_TURB_FACTOR_Z,

	TTK_PART_LAST_IN_ENUM
	}
TTK_PART_PARAM;

class	c_ttk_particle final : public c_bdd_particle
	{
private:

public:
	FINLINE	c_ttk_particle() {}
	FINLINE	~c_ttk_particle()	{}

	FINLINE	void	set_value( TTK_PART_PARAM which, void * where)
						{
						param_set_value( &n_bdd_particle::param[which], where);	
//						param_set_value( &bdd_particle_param[which], where);	
						}
	FINLINE	void	get_value( TTK_PART_PARAM which, void * where)
						{
						param_get_value( &n_bdd_particle::param[which], where);	
//						param_get_value( &bdd_particle_param[which], where);	
						}
	FINLINE	void	get_min( TTK_PART_PARAM which, void * where)
						{
						param_get_min( &n_bdd_particle::param[which], where);	
//						param_get_min( &bdd_particle_param[which], where);	
						}
	FINLINE	void	get_max( TTK_PART_PARAM which, void * where)
						{
						param_get_max( &n_bdd_particle::param[which], where);
//						param_get_max( &bdd_particle_param[which], where);
						}
	FINLINE	void	get_def( TTK_PART_PARAM which, void * where)
						{
						param_get_def( &n_bdd_particle::param[which], where);	
//						param_get_def( &bdd_particle_param[which], where);	
						}
	FINLINE	void	get_ina( TTK_PART_PARAM which, void * where)
						{
						param_get_ina( &n_bdd_particle::param[which], where);	
//						param_get_ina( &bdd_particle_param[which], where);	
						}
	FINLINE	void	get_type( TTK_PART_PARAM which, PARAM_TYPE * where)
						{
						*where = param_get_type( n_bdd_particle::param[which]);	
//						*where = param_get_type( bdd_particle_param[which]);	
						}
	FINLINE	void	get_name( TTK_PART_PARAM which, CHAR * where)
						{
						strcpy( where, n_bdd_particle::param[which].pname);	
//						strcpy( where, bdd_particle_param[which].pname);	
						}
	FINLINE	void	get_particle_buffer( c_particle ** pt, INT32 * index)
						{
						*pt = part;
						*index = index_stop;
						}

	void	set_from_data_img_emission_mask( UINT8 * data_in, INT32 size_x_in, INT32 size_y_in, INT32 channel_nb_in);
	void	set_from_data_img_emission_map( UINT8 * data_in, INT32 size_x_in, INT32 size_y_in, INT32 channel_nb_in);
	void	set_from_data_img_life_color_map( UINT8 * data_in, INT32 size_x_in, INT32 size_y_in, INT32 channel_nb_in);

	void	time_set( REAL t);
	REAL	get_preroll_time();

//	AAA_ERR	color_map_read(const char * const filename);
	};

#endif	//  __BDD_PART_H__
