

#ifdef AAA_REMANENCE_H
#error "REMANENCE_H included more than once."
#endif
#define AAA_REMANENCE_H 1

#ifndef AAA_IMG_H
#	include "image/img.h"
#endif

class	c_remanence final : public c_obj
{
protected:
	REAL		_time;
	bool		_b_color;
	bool		_b_diff;
	bool		_b_restart;
	REAL*		_data;
	INT32		_size_x;
	INT32		_size_y;
	REAL		_diff_min;
	REAL		_diff_factor;
//	REAL		_time_got_frame;
	REAL		_time_got_frame_last;
	UINT32		_channel_nb;
public:
	c_remanence();
	virtual	~c_remanence();

	bool	alloc_remanence( INT32 size_x, INT32 size_y, INT32 channel_nb );
	void	restart_remanence( UINT8* bgra );
	void	compute_remanence( UINT8* data );
public:
	void	do_remanence( c_img_2d* img );
	void	set_remanence( REAL CONST time, bool CONST b_color, bool CONST b_restart, bool CONST b_diff, REAL diff_min, REAL diff_factor );
};

