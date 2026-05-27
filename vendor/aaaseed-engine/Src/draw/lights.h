
#ifdef AAA_LIGHTS_H
#error "LIGHTS_H included more than once."
#endif
#define AAA_LIGHTS_H 1


#ifndef AAA_LIGHT_H
#	include "light.h"
#endif
#ifndef AAA_GOL_LIGHT_H
#	include "gol/gol_light.h"
#endif

//namespace gl { class ssbo; }
//namespace gl { class ubo; }

class	c_lights final : public c_obj_ui
{
	FACTORY_DECLARE( c_lights, c_obj_ui );

	static	c_lights*	cur;	//todo add ancessor fns
	static	c_lights*	ui;
	static	c_lights*	def;

public:
	FINLINE	static	c_lights*	get_def()					{	return def;		}
	FINLINE	static	void		set_def( c_lights* lights )	{	def = lights;	}

	FINLINE	static	c_lights*	get_cur()					{	return cur;		}
	FINLINE	static	void		set_cur( c_lights* lights )	{	cur = lights;	}
	FINLINE	static	void		set_cur_null()				{	cur = def;		}

	FINLINE	static	c_lights*	get_ui()					{	return ui;		}
	FINLINE	static	void		set_ui( c_lights* lights )	{	ui = lights;	}
	FINLINE	static	void		set_ui_null()				{	ui = def;		}

			static	void		c_init();
			static	void		c_deinit();

	static CONSTEXPR INT32 LIGHT_NB = GOL::LIGHT_NB;

private:
	c_light	_lights[LIGHT_NB];
	FP32	_ambient[5];
	FP32	_intensity_factor;
	bool	_b_atte;
	bool	_b_local_viewer;
	bool	_b_two_side;
	INT32	_psy;
	REAL	_psy_freq;
	REAL	_psy_phase_offset;
	REAL	_phase;
	REAL	_phase_last;
	INT32	_start;
	INT32	_stop;


	void	set();

public:
	virtual	void		param_init_pt();
			void		init();

			void		flip_psy();
			void		set_ambient_5f( FP32 r, FP32 g, FP32 b, FP32 a, FP32 grey );
			void		set_atte( bool const value );
			void		set_intensity_factor( FP32 intensity_factor_in );
			void		mult_intensity_factor( FP32 factor_in );
	FINLINE	FP32		get_intensity_factor()		CONST	{ return _intensity_factor; }
	FINLINE	REAL		get_phase( INT32 index )	CONST	{ return _phase + index * _psy_phase_offset; }
	FINLINE	INT32		get_psy()					CONST	{ return _psy; }
	FINLINE	bool		is_atte()					CONST	{ return _b_atte; }

	virtual	void		update()  override final;
	virtual	void		draw() override final;
			void		set_pos( glm::mat4 CONST * mat_view_inverse );
			void		set_local_viewer( bool flag );
			void		set_two_side( bool flag );
	virtual	AAA_ERR		save_do_after( o_str CONST & filename_in );
	virtual	AAA_ERR		load_do_after( o_str CONST & filename_in );

	FINLINE void		set_start( INT32 start_in )			{ _start = start_in; }
	FINLINE void		set_stop( INT32 stop_in )			{ _stop = stop_in; }
	FINLINE INT32		get_stop()					CONST	{ return _stop; }
	FINLINE INT32		get_start()					CONST	{ return _start; }
	FINLINE	c_light*	get_light( INT32 index )			{ return &_lights[index]; }	
};

class	c_lights_switch final : public c_obj_ui
{
	FACTORY_DECLARE(c_lights_switch,c_obj_ui);
	friend class c_lights;

public:
	static		c_lights_switch *	cur;
	static		c_lights_switch *	ui;

	typedef struct light_nb
	{
		UINT32	_directional_nb;
		UINT32	_point_nb;
		UINT32	_spot_nb;
	} st_light_nb;

private:
	bool		_b_on[c_lights::LIGHT_NB];
	c_lights *	_p_lights					{ nullptr };

//	gl::ubo *	_ubo_light_nb				{ nullptr };
//	gl::ssbo *	_ssbo_directional			{ nullptr };
//	gl::ssbo *	_ssbo_point					{ nullptr };
//	gl::ssbo *  _ssbo_spot                  { nullptr };

public:
			void		build_sum_up( o_str & o );
			void		param_init_pt() override final;

	virtual	void		update() override final;
			void		dump_on();
			void		flip( INT32	index );
};

