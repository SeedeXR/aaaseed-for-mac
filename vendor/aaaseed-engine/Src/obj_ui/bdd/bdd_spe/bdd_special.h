
#ifdef AAA_BDD_SPECIAL_H
#error "BDD_SPECIAL_H included more than once."
#endif
#define AAA_BDD_SPECIAL_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_averager;

CONST	INT32	BDD_CSV_COL_MAX_NB = 256;

class	c_bdd_csv final : public c_bdd 
{
private:
//	bool			b_draw_cube;
	o_str			model_filename;
	o_str			data_filename;
	INT32			nb_col;
	INT32			nb_row;
	INT32			nb_col_last;
	INT32			nb_row_last;
	INT32			nb_col_draw;
	INT32			nb_row_draw;
	INT32			col_index_cur;
	c_averager*		averager;
	REAL			size[3];
	REAL			data[BDD_CSV_COL_MAX_NB];
	time_t			time_modified;
	REAL			filter_factor_ui;
	REAL			filter_factor;
public:
					c_bdd_csv();
	virtual			~c_bdd_csv();

			void	init();
			void	alloc(); 
			void	dealloc();

	virtual	void	update();
			void	draw_cube();
//					void	draw_bdd();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }

	virtual	void	param_init_pt();
	virtual	void	param_init();

			void	fill_data();
			void	reset_max_factor();
};
