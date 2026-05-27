
#ifdef AAA_LAYER_ATT_H
#error "LAYER_ATT_H included more than once."
#endif
#define AAA_LAYER_ATT_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_mocap;
class	c_color;
class	c_def_node;
class	c_fog;
class	c_layer;
class	c_lights_switch;
class	c_map;
class	c_texturing;
class	c_shading;
class	c_stencil;
class	c_clip;
class	c_tex_video;
class	c_model;
class	c_render;
//		c_multiple*	;
class	c_tex_anim;
class	c_transfo_three;
class	c_transfo_trs;
class	c_bdd_multiple;
class	c_bdd_curve_edit;

class	c_layer_att
{
public:
	static	void	c_init();
	static	void	c_deinit();

	static	c_bdd*	bdd_up;
private:
	static			INT32			stack_index;
	static			c_layer_att*	stack;

	c_bdd*				_bdd;
	c_bdd_mocap*		_bdd_mocap;
	c_color*			_color;
	c_def_node*			_deformer;
	c_fog*				_fog;
	c_layer*			_layer;
	c_lights_switch*	_lights_switch;
	c_map*				_map;
	c_texturing*		_texturing;
	c_shading*			_shading;
	c_stencil*			_stencil;
	c_clip*				_clip;
	c_tex_video*		_tex_video;
	c_model*			_model;
	c_render*			_render;
//	c_multiple*			multiple_;
	c_tex_anim*			_tex_anim;
	c_transfo_three*	_transfo_three;
	c_transfo_trs*		_transfo1;
	c_transfo_trs*		_transfo2;
	c_bdd_multiple*		_bdd_multiple;
	c_bdd_curve_edit*	_bdd_curve_edit;
public:
			void	get_cur();
			void	set_cur() CONST;
	static	void	push();
	static	void	pop();

	static	FINLINE	bool	is_bdd_cur_equal_up()
	{
		return bdd_up == c_bdd::get_cur();
	}

};

