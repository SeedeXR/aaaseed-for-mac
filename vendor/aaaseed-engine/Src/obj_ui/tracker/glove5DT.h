
#ifdef AAA_GLOVE5DT_H
#error "GLOVE5DT_H included more than once."
#endif
#define AAA_GLOVE5DT_H 1


#ifndef	AAA_TRACKERS_H
#	include "trackers.h"
#endif

#if AAA_TRACKER_GLOVE_5DT()

#ifndef _FGLOVE_H_
#	include "glove5DT/fglove.h"
#endif

static	CONST	UINT32	GLOVE_FINGER_NB		= 5;
static	CONST	UINT32	GLOVE_ROTATION_NB	= 3;
static	CONST	UINT32	GLOVE_CHANNEL_NB	= (GLOVE_FINGER_NB+GLOVE_ROTATION_NB);

class	c_glove final : public c_tracker
{
	FACTORY_DECLARE(c_glove,c_tracker);
public:
	static	bool	b_dll_loaded;
protected:
//	c_delta_t	delta_t;
	fdGlove* pglove;

	REAL	flexion[GLOVE_FINGER_NB];
	REAL	flexion_speed[GLOVE_FINGER_NB];
	REAL	flexion_bias[GLOVE_FINGER_NB];
	REAL	flexion_filter;

	REAL	rotation_filter;
	REAL	rotation_factor[GLOVE_ROTATION_NB];
	REAL	rotation_speed[GLOVE_ROTATION_NB];
	REAL	rotation[GLOVE_ROTATION_NB];

//	bool	b_camera_influence;
	INT32	s_rotation_order;

	REAL	data_in[GLOVE_CHANNEL_NB];

	INT32	com_port_nb;
	c_sensor*	p_sensor;
public:
	void	set_port( INT32 port_id);
	FINLINE	void	set_data_in( INT32 index, REAL value)	{	data_in[index] = value; }
	FINLINE	REAL	get_data_in( INT32 index)				{	return data_in[index]; }
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update_low();
	virtual	AAA_ERR	open();
	virtual	void	close();

	virtual	void	calibrate_default()		{};
};

extern	c_glove*		glove_a;
extern	c_glove*		glove_b;

extern	void	fglove_load_lib();
extern	void	fglove_unload_lib();

#endif	//#if AAA_TRACKER_GLOVE_5DT()
