
#ifdef AAA_BDD_EMOTIV_H
#error "BDD_EMOTIV_H included more than once."
#endif
#define AAA_BDD_EMOTIV_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

struct emokit_device;

class	c_bdd_emotiv final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_emotiv, c_bdd );

public:

protected:
	bool					_b_open				;
	bool					_b_open_ui			;

	bool					_b_verbose_ui		;
	INT32					_nb				{0}	;

	struct emokit_device*	_epoc_device		;

	REAL					_electrodes_raw[14];
	REAL					_electrodes[14];
	INT32					_index_raw;
	INT32					_gyro_raw[2];
	REAL					_gyro[2];

	INT32					_battery;
	REAL					_factor;
	REAL					_offset;

	bool					_b_color_auto;
	bool					_b_moy;
	INT32					_curve_begin;
	INT32					_curve_end;

			void	update_low();

public:
			void	init();
	virtual	void	param_init_pt();

//todo open and close() should go to boyttle net no ?
			bool	open();
			void	close();

	virtual	void	update();
	virtual	void	draw();
};

