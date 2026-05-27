
#ifdef AAA_BDD_ARTNET_H
#error "BDD_ARTNET_H included more than once."
#endif
#define AAA_BDD_ARTNET_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

class c_net_link;

class	c_bdd_artnet final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_artnet, c_bdd );

protected:
	bool			_b_open_asked;
	bool			_b_open;
	bool			_b_verbose;
	bool			_b_remove_newline_last;

	c_net_link*		_link;
	INT32			_link_index_ui;
//	INT32			_link_index;

	bool			_b_test;
	INT32			_test_universe_begin_ui;
	INT32			_test_universe_end_ui;
	INT32			_test_led_nb_ui;

	DOUBLE			_phase;
	c_delta_t		_delta_t;
	REAL			_freq_phase;
	REAL			_freq_time;
//	REAL			_freq_time_shift;
	REAL			_phase_shift;

	REAL			_amplitude;
	REAL			_offset;

	AAA_ERR			prepare_blk(	o_str& o	);
	AAA_ERR			send_blk(		o_str& o,	INT32 universe	);

public:
			void	init();
	virtual	void	param_init_pt();

			void	open();
			void	close();

	virtual	void	update();
			void	update_low();

//	virtual	void	draw();
};


