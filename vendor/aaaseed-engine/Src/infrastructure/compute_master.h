
#ifdef AAA_COMPUTE_MASTER_H
#error "COMPUTE_MASTER_H included more than once."
#endif
#define AAA_COMPUTE_MASTER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

class	c_compute_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_compute_master, c_obj_ui );
public:
private:
	INT32				_compute_task_max_nb_ui;
	INT32				_compute_task_count;

	mutable aaa::MUTEX	_access;
	mutable aaa::MUTEX	_compute;
public:
	virtual	void	param_init_pt();
	virtual	void	param_init();
	virtual	void	update();

			void	inc();
			void	dec();
			void	set( INT32 nb );
};

extern	c_compute_master*	g_compute_master;

