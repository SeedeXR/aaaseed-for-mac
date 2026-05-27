
#ifdef AAA_BDD_MOVIE_H
#error "BDD_MOVIE_H included more than once."
#endif
#define AAA_BDD_MOVIE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_movie final : public c_bdd 
{
	FACTORY_DECLARE( c_bdd_movie, c_bdd );
private:
	BOOL	_b_trig_open;
	BOOL	_b_trig_start;
	BOOL	_b_trig_stop;
	BOOL	_b_trig_rewind;
	BOOL	_b_trig_close;
	BOOL	_b_loop;
	BOOL	_b_play;
	BOOL	_b_movie_loaded;
	REAL	_pos[2];
	REAL	_pos_ui[2];
	o_str	_movie_filename;

public:
			void	init();
			void	alloc(); 
			void	dealloc();

			void	set_movie_filename( CHAR* CONST filename );
	virtual	AAA_ERR	load_do_after( CONST CHAR* CONST filename );

	virtual	void	draw();
	virtual	void	update();

	virtual	void	param_init_pt();
	virtual	void	param_init();

			void	update_position();

			void	open();
			void	start();
			void	stop();
			void	rewind();
			void	close();

			BOOL	is_playing();
};

#endif	//  __BDD_MOVIE_H__