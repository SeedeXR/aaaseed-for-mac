
#ifdef AAA_TRANSFO_THREE_H
#error "TRANSFO_THREE_H included more than once."
#endif
#define AAA_TRANSFO_THREE_H 1


#ifndef	AAA_TRANSFO_H
#	include "transfo.h"
#endif

class	c_transfo_three final : public c_transfo
{
	FACTORY_DECLARE( c_transfo_three, c_transfo );
public:
	static	c_transfo_three*	cur;

private:
	INT32	_start_channel;
	INT32	_start_node;
	REAL	_start[3];
	INT32	_stop_channel;
	INT32	_stop_node;
	REAL	_stop[3];
	INT32	_third_channel;
	INT32	_third_node;
	REAL	_third[3];
	REAL	_inter;
	INT32	_s_type;
	INT32	_s_scale;
	FP32	_mat[16];	//todo move to glm:mat4
	INT32	_s_axe;

	virtual void	do_it_low();
	virtual	void	update_private();
public:
	virtual	void	param_init_pt();


	//todo	implement
	//virtual void	apply_move(		REAL CONST * in, REAL* out	)	{}
	//virtual void	apply_rotate(	REAL CONST * in, REAL* out	)	{}
	//virtual void	get_speed(		REAL CONST * in, REAL* out	)	{}
};

