
#ifdef AAA_LAYERS_ATT_H
#error "LAYERS_ATT_H included more than once."
#endif
#define AAA_LAYERS_ATT_H 1


#ifndef	AAA_LAYER_ATT_H
#	include "layer_att.h"
#endif
#ifndef	AAA_LAYERS_H
#	include "infrastructure/layer/layers.h"
#endif
#ifndef AAA_SEEDDRAW_H
#	include "draw/seeddraw.h"
#endif
#ifndef AAA_SEEDCAM_H
#	include "draw/seedcam.h"
#endif
#ifndef AAA_LIGHTS_H
#	include "draw/lights.h"
#endif

class c_layers;

class	c_layers_att
{
public:
	static	void	c_init();
	static	void	c_deinit();
private:
	static			INT32			stack_index;
	static			c_layers_att*	stack;

	c_layers*			_layers;
	c_lights*			_lights;
	c_seedcam*			_camera;
	bool				_b_render_first_pass;
public:
	FINLINE	void	get_cur()
	{
		c_layer_att::push();
		_layers					= c_layers::get_cur();
		_lights					= c_lights::get_cur();
		_camera					= c_seedcam::get_cur();
		_b_render_first_pass	= draw::is_render_first_pass();
	}

	FINLINE	void	set_cur()
	{
		c_layer_att::pop();
		c_layers::set_cur(		_layers	);
		c_lights::set_cur(		_lights	);
		c_seedcam::set_cur(		_camera );
		draw::set_render_first_pass( _b_render_first_pass );
	}
	static	void	push();
	static	void	pop();
};

