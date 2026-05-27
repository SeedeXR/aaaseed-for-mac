
#ifdef AAA_BDD_ARBRE_H
#error "BDD_ARBRE_H included more than once."
#endif
#define AAA_BDD_ARBRE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

#ifndef _MAP_
#	include <map>
#endif
#ifndef __RECTANGLE_2D__
#	include "rectangle2d.h"
#endif

#include "unique_tree.h"

class c_node_arbre;
struct c_node_compare;
struct st_image_arbre; 

class touch_data;
class c_interact;

using namespace tcl;

typedef unique_tree< c_node_arbre, std::less<c_node_arbre>, c_node_compare> arbre_unique;

//typedef struct node_arbre
//{
//	INT32		_id;
//	INT32		_parent_id;
//	rect_2d_f	_rect_node;
//	o_str		_name;
//	INT32		_picto_nb;
//} node_arbre;
//

class	c_bdd_arbre final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_arbre, c_bdd );
private:
	static	c_bdd_arbre*	cur;

public:

	static	void			c_deinit();
	static	void			c_init();

protected:
	BOOL		_b_active;
	bool		_b_verbose;

	bool		_b_init;
	bool		_b_loaded;
	BOOL		_b_tree_loaded;
	BOOL		_b_image_loaded;

	o_str		_tree_filename;
	o_str		_image_filename;
	INT32		_picto_image_src;
	INT32		_image_src;
	BOOL		_b_draw_arbre;
	BOOL		_b_draw_image;
	BOOL		_b_draw_picto;

	REAL		_image_scale_min;
	REAL		_image_scale_max;

	INT32		_noeud_nb;
	INT32		_image_nb;

	BOOL		_b_text_show;
	REAL		_text_size;
	INT32		_s_font;
	BOOL		_b_font_outline;
	REAL		_text_pos_x;
	REAL		_text_pos_y;
	REAL		_font_size_cache;

	REAL		_text_color[4];

	REAL		_angle_start;
	REAL		_angle_stop;

	BOOL		_b_branch_draw;
	REAL		_branch_color[4];

	BOOL		_b_do_interact;
	INT32		_s_mode_interact;
	BOOL		_b_reset_trig;

	REAL		_blob_scale_x;
	REAL		_blob_scale_y;
	REAL		_blob_offset_y;
	REAL		_blob_offset_x;

	BOOL		_b_verbose_blob;
	BOOL		_b_fingers_draw;
	REAL		_fingers_color[4];

	bool		_b_draw_info;
	INT32		_info_src;
	REAL		_info_size;
	REAL		_info_pos_x;
	REAL		_info_pos_y;

	REAL		_picto_scale;

	BOOL		_b_info_touch;
	INT32		_info_touch_image_index;

	BOOL		_b_reload_arbre_trig;
	BOOL		_b_one_finger;
	BOOL		_b_no_sizing;
	BOOL		_b_constraint_image;

	arbre_unique	_arbre;
	std::vector<st_image_arbre>	_image_list;


	void	load_tree();
	void	read_arbre( FILE* file );

	void	init_low();
	void	close();

	void	draw_zone_info();
	void	draw_picto( c_node_arbre* node );
	void	draw_name( c_node_arbre* node );
	void	draw_branch( rect_2d_f node, rect_2d_f parent );

	void	draw_finger( REAL x, REAL y );
	void	draw_image();
	void	analyze_blob_data();
	void	analyze_blob_data_ex();
	void	test_hit();

public:
	void	init();
	void	param_init_pt();

	void	set_tree_filename( CONST CHAR* CONST filename );

	virtual	AAA_ERR	load_do_after( CONST CHAR* CONST filename );

	virtual	void	update();
	virtual	void	draw();
	virtual	void	draw_multiple();

};
