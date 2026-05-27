
#ifdef AAA_BDD_VORONOI_H
#error "BDD_VORONOI_H included more than once."
#endif
#define AAA_BDD_VORONOI_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

namespace voronoi {
	class	generator;
	struct Edge;
}

class	c_bdd_voronoi final : public c_bdd_multiple 
{
	FACTORY_DECLARE( c_bdd_voronoi, c_bdd_multiple );
private:
	o_str		_target_name_symbo;
	c_bdd*		_bdd_target_src;
	INT32		_target_axe;

	REAL*		_point;
	INT32		_point_nb;
	INT32		_point_nb_allocated;

	class	voronoi::generator*	_vdg;

	INT32		_index_begin;
	INT32		_index_end;

	REAL		_site_size_ui;
	bool		_b_site_src;
	REAL		_number_size_ui;
	bool		_b_site_src_number;
	bool		_b_vertice;
	bool		_b_edge;
	bool		_b_delaunay_edge_number_ui;
	bool		_b_edge_link_ui;
	bool		_b_vertex;
	bool		_b_delaunay_ui;
	bool		_b_voronoi_ui;

	bool		_b_voronoi_poly;
	bool		_b_voronoi_poly_before;
	REAL		_voronoi_poly_size;
	REAL		_voronoi_poly_area_min_ui;
	REAL		_voronoi_poly_area_max_ui;

	REAL		_voronoi_poly_size_barycenter;
	FP32		_voronoi_poly_color_ui[5];
	FP32		_voronoi_poly_color[4];
	REAL		_voronoi_poly_line_size;
	bool		_b_voronoi_poly_as_triangle;
	bool		_b_voronoi_triangle;
	REAL		_voronoi_draw_alpha_threshold;

	bool		_b_draw_boundary;

	REAL		_img_color_size[2];

	FP32		_delaunay_color_ui[5];
	FP32		_delaunay_color[4];
	REAL		_delaunay_line_size;
	bool		_b_delaunay_orientation_ui;

	bool		_b_img_color_use;
	bool		_b_img_color_clamped_ui;
	bool		_b_img_color_by_point;
	bool		_b_img_color_by_triangle;
	INT32		_s_image_color_bind;
	REAL		_img_color_z_influence;

	REAL		_dist_min;

	REAL		_center[3];
	REAL		_limit_min_ui[3];
	REAL		_limit_max_ui[3];
	REAL		_scale_axe;

	bool		_b_EL;

	INT32		_escape_nb_site;
	INT32		_escape_nb_edge;
	INT32		_escape_nb_half_edge;

	bool		_b_save_poly_trig_ui;
private:
			void	init(); 
			void	alloc(); 
			void	dealloc();
			void	prepare_poly();
			bool	alloc_point( INT32 point_nb );
			void	dealloc_point();
			void	draw_edge( struct voronoi::Edge* e );
			void	draw_voronoi_poly();
public:
//	virtual	void	restart();

	virtual	void	param_init_pt();

	virtual bool	can_implicit() final override { return true; }

	virtual	void	update();
			void	save_poly();
	virtual	void	draw_single();
	virtual	void	draw_multiple();
//todo
	virtual	INT32	get_segment_nb();
	virtual	void	get_segment( REAL* a, REAL* b, INT32 seg_index );

//	virtual	void	draw_normal_point( REAL len );

	virtual	INT32	get_point_nb() final override;
	virtual	bool	get_point( REAL* CONST dst, INT32 CONST index ) final override;
};


