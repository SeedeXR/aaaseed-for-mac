
#ifdef AAA_FOG_H
#error "FOG_H included more than once."
#endif
#define AAA_FOG_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_GOL_H
#	include "gol/gol.h"
#endif


class	c_fog final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_fog,c_obj_active_ui);
public:
	static	c_fog*	cur;
	static	c_fog*	ui;
private:
	INT32		_s_mode_ui;
	FP32		_density_ui;
	GOL::HINT	_s_hint_ui;

	REAL		_color_ui[5];

	REAL		_start_ui;
	REAL		_end_ui;

	bool		_b_verbose;
public:

	virtual	void	param_init_pt();
	
			void	print();
			void	print_verbose();
			void	set_verbose( bool CONST b );
			void	flip_verbose();

	FINLINE INT32	get_mode()		CONST	{ return _s_mode_ui; };
	FINLINE REAL	get_density()	CONST	{ return _density_ui; };
	FINLINE INT32	get_hint()		CONST	{ return _s_hint_ui; };

//	FINLINE REAL*	get_color()		{ return _color_ui; };

	FINLINE REAL	get_start()		CONST	{ return _start_ui; };
	FINLINE REAL	get_end()		CONST	{ return _end_ui; };

			void	enable();
			void	disable();
	virtual	void	update();
	//	MODE
			void	print_mode();
			void	set_mode(		INT32 CONST i );
			INT32	flip_mode();
	// DENSITY
			void	print_density();
			void	set_density(	REAL CONST density_in );
	//	HIN
			void	print_hint();
			void	set_hint(		INT32 CONST i );
			INT32	flip_hint();
	// COLOR
			void	print_color();
			void	set_color(		REAL CONST red, REAL CONST green, REAL CONST blue, REAL CONST alpha );
	//	START END
			void	print_start_end();
			void	set_start(		REAL CONST start_in );
			void	set_end(REAL end_in);
};

