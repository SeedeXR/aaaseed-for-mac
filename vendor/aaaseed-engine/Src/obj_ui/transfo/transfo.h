
#ifdef AAA_TRANSFO_H
#error "TRANSFO_H included more than once."
#endif
#define AAA_TRANSFO_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_transfo : public c_obj_active_ui
{
	FACTORY_ABSTRACT_DECLARE(c_transfo,c_obj_active_ui);
private:
protected:
	bool	_b_todo;
	virtual void	do_it_low() = 0;
	virtual	void	update_private() = 0;
public:
	//virtual void	apply_move(		REAL CONST * in, REAL* out) = 0;
	//virtual void	apply_rotate(	REAL CONST * in, REAL* out) = 0;
	//virtual void	get_speed(		REAL CONST * in, REAL* out) = 0;


	virtual void	update()
	{
		if( is_active() )
			update_private();
		else
			_b_todo = false;
	}

	FINLINE	bool	is_todo()	{	return _b_todo; }
	FINLINE	void	do_it()	
	{
		if( _b_todo )
			do_it_low();
	}
	virtual void	draw()		{	do_it(); }
};

/*
class	c_transfo_dpendulum final : public c_transfo
	{
	REAL	rayon_factor;
	REAL	frequency_factor;

	REAL	a_rayon_ui;
	REAL	a_rayon;
	REAL	a_frequency[3];
	REAL	a_frequency_factor_ui;
	REAL	a_frequency_factor;

	REAL	b_rayon_ui;
	REAL	b_rayon;
	REAL	b_frequency[3];
	REAL	b_frequency_factor_ui;
	REAL	b_frequency_factor;

	INT32	axe;
	REAL	a_sin;
	REAL	a_cos;
	REAL	b_sin;
	REAL	b_cos;
	REAL	a_freq;
	REAL	b_freq;
	INT32	i_u;
	INT32	i_v;

public:
	c_transfo_dpendulum();
	virtual	~c_transfo_dpendulum() {};

	virtual	void	param_init_pt();
	virtual	void	update();
	virtual	void	set_time( REAL t);

	virtual void	apply_move(		pt_3d* in, pt_3d* out );
	virtual void	apply_rotate(	pt_3d* in, pt_3d* out );
	virtual void	get_speed(		pt_3d* in, pt_3d* out );
	virtual void	do_it() {};
	};
*/

