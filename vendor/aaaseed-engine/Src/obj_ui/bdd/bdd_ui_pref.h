
#ifdef AAA_BDD_UI_PREF_H
#error "BDD_UI_PREF_H included more than once."
#endif
#define AAA_BDD_UI_PREF_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_ui_pref final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_ui_pref, c_bdd );
public:
	enum COLOR_TYPE : INT32
		{
		COLOR_BACK=0,
		COLOR_CONSTRUCTION,
		COLOR_FRAME,
		COLOR_FRAME_SUB,
		COLOR_OUTPUT,
		COLOR_NB,
		};
private:
	FP32	_color_ui[COLOR_NB][5];
	REAL	_line_size[COLOR_NB];
public:
	static	void c_init();
	static	void c_deinit();
	static	c_bdd_ui_pref*	master;
	static	c_bdd_ui_pref*	cur;
	static	void	begin_frame();
	static	void	begin()	{ begin_ui();	}
	static	void	end()	{ end_ui();		}

			void	init();
	
	virtual	void	draw() {}
	virtual	void	update();

	virtual	void	param_init_pt();

	FINLINE	void	get_color_real( FP32* dst, COLOR_TYPE CONST type );
			void	get_color_255( INT32* dst, COLOR_TYPE CONST type );

			void	set_drawing( COLOR_TYPE CONST type );
			void	set_drawing( COLOR_TYPE CONST type, FP32 CONST alpha );
};

