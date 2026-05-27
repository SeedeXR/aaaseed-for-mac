
#ifdef AAA_CAMERA_FORMAT_H
#error "CAMERA_FORMAT_H included more than once."
#endif
#define AAA_CAMERA_FORMAT_H 1


#ifndef	AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class	cam_format
{
public:
	enum	FORMAT : INT32
	{
		NONE = -1,
		LOW_RES = 0,
		HIGH_RES,
		SVGA,
		XGA,
		SXGA,
		SXGA_PLUS,

		WSXGA_PLUS,
		UXGA,
		WUXGA,
		WQHD,
		WQXGA,

		PAL,
		PAL_IMPACT,
		PAL_STANDART,
		NTSC,
		HD_1920,
		HD_1600,
		HD_1280,
		SQUARE256,
		SQUARE512,
		SQUARE1024,

		F_1152_864,
		F_1280_960,

		CUSTOM,
		FORMAT_MAX
	};
private:

	static	FINLINE	INT32	get_size( INT32 index, FORMAT i );

public:

	static	void		c_init();
	static	void		init(		FORMAT starting_format );
	static	void		set(		FORMAT new_format );
	static	void		ask(		FORMAT new_format );


	static	void		update();
	static	FORMAT		get();
	static	FORMAT		get_clean();

	static	INT32		get_sx( FORMAT i );
	static	INT32		get_sy( FORMAT i );
	static	INT32		get_sx();
	static	INT32		get_sy();
	static	C_PCHAR_C	get_name(	FORMAT new_format );

	static	INT32*		get_custom_x_pt();
	static	INT32*		get_custom_y_pt();
	static	REAL*		get_custom_factor_pt();
	static	REAL*		get_factor_pt();
	static	INT32*		get_win_offset_x_pt();
	static	INT32*		get_win_offset_y_pt();
	static	FORMAT*		get_cur_pt();
	static	FORMAT*		get_last_pt();

	static	INT32		set_fullscreen_mode( INT32 index );
	static	INT32		dec_fullscreen_mode();
	static	INT32		inc_fullscreen_mode();

	static	INT32*		get_fullscreen_mode_pt();
};


