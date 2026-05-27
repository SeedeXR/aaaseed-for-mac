
#ifdef AAA_BDD_CELL_AUTOMATA_H
#error "BDD_CELL_AUTOMATA_H included more than once."
#endif
#define AAA_BDD_CELL_AUTOMATA_H 1


#ifndef AAA_BDD_UV_H
#	include "obj_ui/bdd/util/bdd_uv.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif

template<class T>	class	c_data_2d;
class	c_img_2d;

class	c_bdd_cell_automata final : public c_bdd_uvw
{
	FACTORY_DECLARE( c_bdd_cell_automata, c_bdd_uvw );
protected:
//todo inmplement or destroy
//	INT32					_death_min;
//	INT32					_death_max;
//	INT32					_birth_min;
//	INT32					_birth_max;

//	REAL					generation_per_sec;
	bool					_b_start_trig;
	bool					_b_stop_trig;
	bool					_b_play;
	bool					_b_fill_random_trig;
	bool					_b_shift_trig_ui;
	bool					_b_gene_trig;

	REAL					_shift_freq;
	REAL					_phase_offset;
	REAL					_phase;
	REAL					_phase_out;
	c_delta_t				_delta_t;
	INT32					_data_index_offset;
//	bool					b_verbose;
	std::deque< c_data_2d<REAL>* >	_datas;
public:

	void	init();

	void	alloc();
	void	dealloc();
	
	virtual	void	update();
			void	build_grid( REAL z );
	virtual	void	build_geo();

	virtual	void	draw_multiple();

			void	shift();
			void	gene();

			void	move_to( c_img_2d* img );
			void	move_from( c_img_2d* img );

	virtual	void	param_init_pt();
};
