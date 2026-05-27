
#ifdef AAA_BDD_CLEAR_SCREEN_H
#error "BDD_CLEAR_SCREEN_H included more than once."
#endif
#define AAA_BDD_CLEAR_SCREEN_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_clear_screen final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_clear_screen, c_bdd );
private:
	REAL	_depth_value;
	INT32	_color_index;
	FP32	_color_ui[5];
	FP32	_clear_color[4];
	INT32	_stencil_value;

	bool	_b_viewport_only_ui;
	bool	_b_color_ui;
//	bool	_b_color;
	bool	_b_depth_ui;
	bool	_b_stencil_ui;
	bool	_b_accum_ui;

	bool	_b_erase;
	REAL	_trail_alpha;
	REAL	_trail_time;
	REAL	_trail_fps;

public:
	static	CONSTEXPR	INT32	COLOR_NB_MAX	= 12;
	static				bool	b_verbose_ui;
private:
	static				FP32	color[COLOR_NB_MAX][4];

			void	do_trail();

public:
			void	set_erase_color( bool CONST b_in );
			void	flip_erase_color();
	FINLINE	bool	is_erase_color()				CONST { return _b_color_ui; };
	FINLINE	bool	is_erase()						CONST { return _b_erase; };
			void	set_erase_depth( bool CONST b_in );
			void	flip_erase_depth();
	FINLINE	bool	is_erase_depth()				CONST { return _b_depth_ui; }

	virtual	void	param_init_pt();

			void	set_clear_color_index( INT32 CONST index );

	virtual	void	update();
	virtual	void	draw();
			
			void	dec_background_color();
			void	inc_background_color();
};

//move for here
extern	c_bdd_clear_screen* g_bdd_clear_screen;
