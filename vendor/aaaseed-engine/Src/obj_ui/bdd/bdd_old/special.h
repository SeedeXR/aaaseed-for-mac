
#ifdef AAA_SPECIAL_H
#error "SPECIAL_H included more than once."
#endif
#define AAA_SPECIAL_H 1

//todo remove special ?
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

extern void		special_deinit();
extern	void	special_init();
extern	void	special_go_render();
extern	void	special_set_focus();
extern	void	special_update();
extern	void	special_draw_on_top();
extern	void	special_save( o_str CONST & filename );
extern	void	special_load( o_str CONST & filename );
extern	void	special_about();

extern	void	special_get_user_name( char* str );

extern	bool	special_key(			INT32 key_code, INT32* modifiers, INT32* x, INT32* y );
extern	bool	special_key_special(	INT32 key_code, INT32* modifiers, INT32* x, INT32* y );
extern	void	special_build_message();

extern	INT32	special_get_bdd_tri_id( C_PCHAR_C name_in );

extern	void	special_mocap_set_seq( INT32 index );
class	c_bdd_mocap;
extern	void	special_mocap_end_seq( c_bdd_mocap* mocap );

enum SPECIAL_TYPE : INT32
{
	TYPE_BPI = 0,
	TYPE_NIB,
	TYPE_ORGIA,
	TYPE_STRATO,
	TYPE_LCP,
	TYPE_AICHI,
	TYPE_UPSTREAM,
	SPECIAL_NB
};

extern	C_PCHAR_C	str_special[SPECIAL_NB];
extern	INT32	s_special_type;

extern	void	special_menu_init();
extern	bool	special_menu_add();


