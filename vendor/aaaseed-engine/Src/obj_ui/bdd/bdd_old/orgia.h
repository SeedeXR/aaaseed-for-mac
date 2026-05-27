
#ifdef AAA_ORGIA_H
#error "ORGIA_H included more than once."
#endif
#define AAA_ORGIA_H 1


#define	APP_SPECIAL_ORGIA()	0

#if	APP_SPECIAL_ORGIA()

#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
struct	c_orgia_mess_abysse
{
	INT32	who;
	INT32	what;
	INT32	more;
	INT32	speed;
	INT32	size;
	INT32	color;
	REAL	target[3];
};

struct	st_group
{
	REAL	nb;
	REAL	pos[3];
	REAL	target[3];
	REAL	target_radius;
	INT32	speed;
	INT32	size;
	INT32	color;
	BOOL	target_on_last;
};

class	c_orgia final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_orgia,c_obj_active_ui);
private:
	BOOL				b_trig_test_send_max_block;
	BOOL				b_trig_test_send_maa_block;
	REAL				value[16];
	c_orgia_mess_abysse	mess;
	REAL				max_send_radius;
	REAL				max_send_offset_z;

	st_group			group[3];
	REAL				apharia_inter_dist;

	REAL				projection_factor;
	REAL				projection_offset;
	REAL				x_normalized_banc;
	REAL				y_normalized_banc;

public:
	virtual	void	param_init_pt();
	virtual	void	update();

	void	net_receive_more( INT32* pi);
	void	net_receive();
	void	send_mess_abysse_to_max();
	void	send_hyssard_to_max();
	void	send_on_target( INT32 who);
};

extern	c_orgia*	orgia;
#endif	//APP_SPECIAL_ORGIA

