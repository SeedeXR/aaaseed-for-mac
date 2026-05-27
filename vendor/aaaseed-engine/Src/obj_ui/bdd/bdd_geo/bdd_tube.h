
#ifdef AAA_BDD_TUBE_H
#error "BDD_TUBE_H included more than once."
#endif
#define AAA_BDD_TUBE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

struct	SECTION2D;
struct	LEGO;
class	c_img_2d;

class	c_bdd_tube final : public c_bdd 
{
	FACTORY_DECLARE(c_bdd_tube,c_bdd);
private:
	REAL	_time_factor;
	REAL	_tube_time;
	REAL	_section_phase_step;
	REAL	_section_phase;

	bool	_b_flat;
	REAL	_rayon_angle;
	bool	_b_rayon_height;
	REAL	_rayon;
	REAL	_rayon_var;
	REAL	_rayon_time_freq;
	REAL	_rayon_space_freq;
	REAL	_rayon_shape_ui;
	REAL	_rayon_shape_time;
	bool	_b_rayon_def_small_on;

	REAL	_len;
	INT32	_lego_nb_ui;
	INT32	_lego_nb;
	LEGO*	_lego;

	REAL	_section_rotation;
	INT32	_section_nb;
	INT32	_section_nb_ui;
	INT32	_section_modulo;
	INT32	_section_index;

	//	number of tile by section
	//	must be a float
	REAL	_tex_lego_nb_u;
	//	increment from section to section
	//	must be a float	
	REAL	_tex_lego_nb_v;
	REAL	_tex_speed_u;
	REAL	_tex_speed_v;
	//	sub texture
	//	must be i
	bool	_b_sub_on;
	INT32	_tex_sub_nb_u;
	INT32	_tex_sub_nb_v;

	REAL		_tex_lego_u;
	REAL		_tex_lego_v;
	REAL		_tex_lego_step_u;
	REAL		_tex_lego_step_v;

	REAL		_tex_lego_u_min;
	REAL		_tex_lego_u_max ;

	REAL		_tex_lego_v_min;
	REAL		_tex_lego_v_max;
	INT32		_tex_sub_counter;

	bool		_b_normal_to_compute;
	c_img_2d*	_img_height;
	o_str		_img_height_fname;

private:

	void	init();

	void	section2d_data_alloc(	SECTION2D* s2d	);
	void	section2d_data_dealloc(	SECTION2D* s2d	);
	void	section2d_build(		SECTION2D* s2d, c_point_xyz_real* ame );
	void	section2d_init(			SECTION2D* s2d	);
	void	build();
//	void	build_from_map_height(	c_img_2d*		);

	void	lego_draw(				LEGO*		l	);
	void	lego_draw_gouraud(		LEGO*		l	);

	void	section2d_deform(		SECTION2D* s2d	);
	void	section2d_deform_no(	SECTION2D* s2d	);
	void	do_deform();

public:
	
	void	alloc( INT32 nb_in ); 
	void	alloc(); 
	void	dealloc();

	virtual	void	param_init_pt();
	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }

	virtual	AAA_ERR	load_do_after( o_str CONST & filename );

	void	flip_height();
	void	flip_flat();

	void	make_normal();
//todo	private
	void	alloc_and_build();

	void	map_uv_init();
	void	compute_normal();
	void	load_img_height( o_str CONST & filename );
//	void	section2d_compute_normal(SECTION2D *sp, SECTION2D *sc, SECTION2D *sn);

};


