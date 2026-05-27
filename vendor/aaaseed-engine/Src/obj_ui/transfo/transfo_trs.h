
#ifdef AAA_TRANSFO_TRS_H
#error "TRANSFO_TRS_H included more than once."
#endif
#define AAA_TRANSFO_TRS_H 1



#ifndef	AAA_TRANSFO_H
#	include "transfo.h"
#endif

class	c_transfo_trs final : public c_transfo
{
	FACTORY_DECLARE( c_transfo_trs, c_transfo );

public:
	static	c_transfo_trs*		transfo1_cur;
	static	c_transfo_trs*		transfo2_cur;

private:
	bool	_b_rot_ui;
	bool	_b_rot;
	bool	_b_billboard_ui;
	REAL	_tra[3];
	REAL	_rot[3];
	REAL	_sca[4];
	bool	_b_tra_ui;
	bool	_b_tra;
	bool	_b_sca_ui;
	bool	_b_sca;
//	bool	b_do;
	INT32	_s_rotate_order_ui;
	bool	_b_rotate_first_ui;

protected:
	virtual	void	update_private();

public:
	virtual	void	param_init_pt_static();
	virtual	void	prepare_for_ui();

			void	set_tra( REAL CONST * CONST in );
			void	set_rot( REAL CONST * CONST in );
			void	set_sca( REAL CONST * CONST in );

	virtual	void	set_time( REAL t ) {};

	//todo	implement
	//virtual void	apply_move(		REAL CONST * in, REAL * out )	{}
	//virtual void	apply_rotate(	REAL CONST * in, REAL * out )	{}
	//virtual void	get_speed(		REAL CONST * in, REAL * out )	{}

	virtual void	do_it_low();
	FINLINE	void	rotate();
			void	push();
			void	pop();
};
