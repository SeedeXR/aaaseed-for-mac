
#ifdef AAA_BDD_SOUND_H
#error "BDD_SOUND_H included more than once."
#endif
#define AAA_BDD_SOUND_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_SND_INPUT_H
#	include "media/sound/snd_input.h"
#endif

//class	c_averager;
//CONST	INT32	BDD_SOUND_AVERAGER_MAX_NB = 256;

class	c_bdd_sound final : public c_bdd_multiple 
{
	FACTORY_DECLARE(c_bdd_sound,c_bdd_multiple);
private:
	bool			_b_valid;

	INT32			_type_ui;

//	INT32			s_draw_type;
	bool			_b_draw_loop_ui;
	bool			_b_use_max_ui;
	INT32			_col_start_ui;

	INT32			_col_nb_ui;
	INT32			_col_nb;
//	INT32			_col_stop;
	INT32			_row_nb_ui;
	INT32			_row_nb;
	INT32			_data_allocated; 
	REAL*			_data;

	bool			_b_cylinder_ui;
//	INT32			col_start_last_;
//	INT32			col_nb_last_;
//	INT32			row_nb_last_;

//	INT32			col_index_cur;
//	c_averager*		averager;
	REAL			_origin[3];
//	REAL			_offset;
	REAL			_size[3];
	REAL			_size_offset[3];
	REAL			_size_direction[3];
//	REAL			data[BDD_SOUND_AVERAGER_MAX_NB];
//	REAL			filter_factor_ui;
//	REAL			filter_factor_;
	INT32			_strobe_ui;
	INT32			_strobe_count;

	INT32			_channel_ui;
	INT32			_channel;

	//hack	should be dynamic ?
	c_snd_input*	_sound_input_ui;



	INT32			_band_allocated; 
	INT32			_band_nb; 
	REAL*			_max;
	REAL*			_max_factor;
//#define	BAND_NB		c_snd_input::sample_per_capture_	
//	REAL	band_buf[HISTO_NB*BAND_NB];
	INT32			_receipt_index;
//	REAL	max[BAND_NB];
//	REAL	max_factor[BAND_NB];

			void	alloc_data( INT32 row_nb, INT32 col_nb );
			void	dealloc_data();

			void	alloc_band( INT32 band_nb );
			void	dealloc_band();

	FINLINE INT32	get_draw_row_index(	INT32 CONST index	);
	FINLINE REAL	process_max(		REAL CONST tmp,		INT32 CONST index	);
public:

	void	init();

	virtual	void	update();
	virtual	void	draw_single();
	virtual	void	draw_multiple();

	virtual	void	param_init_pt();

			void	init_data( UINT32 nb );
			void	reset_max_factor();
};
