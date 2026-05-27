
#ifdef AAA_BDD_KRAMER_P2K_H
#error "BDD_KRAMER_P2K_H included more than once."
#endif
#define AAA_BDD_KRAMER_P2K_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_serial;

class	c_bdd_kramer_p2k final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_kramer_p2k, c_bdd );
protected:
	bool			_b_open_ui;
	bool			_b_open;
	bool			_b_verbose;

	DOUBLE			_inter_command_delay;
	DOUBLE			_command_time_last;

	INT32			_s_type;

	c_serial*		_serial;
	INT32			_com_port_id_ui;

	INT32			_output[16];
	INT32			_output_ui[16];

//	bool			b_trig_start;	
//	bool			b_trig_stop;	
public:

			void	set_port( INT32 port_id );
			void	init();
	virtual	void	param_init_pt();

			void	open();
			void	close();

	virtual	void	update();
			void	update_low();
	virtual	void	draw();
};

