
#ifdef AAA_PERIPHERIE_H
#error "PERIPHERIE_H included more than once."
#endif
#define AAA_PERIPHERIE_H 1


#define	APP_SPECIAL_PERI()		0

#if APP_SPECIAL_PERI()

#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

extern	REAL	peri_dist;

class	c_peri final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_peri,c_obj_active_ui);
	BOOL	b_neat_active;

	REAL	rot_speed;
	REAL	tra_speed;

	BOOL	b_red;
	BOOL	b_green;
	BOOL	b_blue;
	INT32	render_choice;
	BOOL	b_rain;

	BOOL	b_init;
	BOOL	b_tex_1;
	BOOL	b_tex_2;
	BOOL	b_tex_3;
	BOOL	b_left;
	BOOL	b_right;
	BOOL	b_up;
	BOOL	b_down;

	REAL	light_intensity[3];
	REAL	red[3];
	REAL	green[3];
	REAL	blue[3];
	REAL	grey[3];
private:
	c_delta_t	delta_t;

public:
	virtual	void	param_init_pt();
	virtual	void	update();
};
extern	c_peri *		peri;

#endif	//APP_SPECIAL_PERI
