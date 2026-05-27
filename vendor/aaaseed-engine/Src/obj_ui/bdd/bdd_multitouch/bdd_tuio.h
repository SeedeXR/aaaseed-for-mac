
#ifdef AAA_BDD_TUIO_H
#error "BDD_TUIO_H included more than once."
#endif
#define AAA_BDD_TUIO_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_GOL_TEX
#	include "gol/gol_tex.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif


class c_flux_filter;

class c_tuio_item
{
public:
	INT32	id;
	FP32	x;
	FP32	y;
	FP32	dx;
	FP32	dy;
	FP32 acc;
public:
	c_tuio_item(	INT32 CONST id_in,
					FP32	CONST x_in,		FP32	CONST y_in,
					FP32	CONST dx_in,	FP32	CONST dy_in,
					FP32 CONST acc_in	)
	{
		id = id_in;	
		x = x_in;	y = y_in;
		dx = dx_in; dy = dy_in;
		acc = acc_in;
	}
};	

namespace osc
{
	class	ReceivedMessage;
};

class	c_bdd_tuio final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_tuio, c_bdd_multiple );
public:
//	static	CONST	INT32	BLOB_NB_UI = 32;
//static	CONST	INT32	BLOB_NB_MAX = 1024;
//	static	c_bdd_tuio*	cur;

private:
	c_img_2d*			_img_dst;
	REAL				_center_ui[3];

	bool				_b_verbose_ui;
	bool				_b_img_update_ui;
	INT32				_size_x_ui;
	INT32				_size_y_ui;
	INT32				_size_x;
	INT32				_size_y;
	GOL::INTERNAL_TYPE	_s_channel_type_ui;
	INT32				_channel_nb_ui;
	INT32				_bind_dst;
	INT32				_bind_dst_ui;
	//INT32				_dot_size_ui;
	bool				_b_img_cleared;	// Image was cleared on a previous frame
	//bool				_b_draw_circle{};		// draw dot as circle
	bool				_b_img_clear_ui;		// debug only : don't clear img before drawing

	// Net in
	bool				_b_tuio_in;
	bool				_b_tuio_in_skip_alive;
	bool				_b_tuio_clear_each_frame_ui;
	UINT32				_tuio_silent_frame_max_nb;
	bool				_b_tuio_in_flip_x;
	bool				_b_tuio_in_flip_y;
	REAL				_tuio_in_scale_x;
	REAL				_tuio_in_scale_y;
	REAL				_tuio_in_offset_x;
	REAL				_tuio_in_offset_y;
	//REAL				_tuio_in_blob_size_ui[3];
	//REAL				_tuio_in_blob_size_half[2];
	INT32				_s_net_verbose_in;
	INT32				_tuio_in_message_received_nb;

	FINLINE	bool	is_net_in_verbose()				{ return _s_net_verbose_in > 0; }
	FINLINE	bool	is_net_in_verbose_detailed()	{ return _s_net_verbose_in > 1; }

//	bool		_b_net_received;
	UINT32		_net_no_frame;

	INT32		_frame_in;


	//REAL		_origin[3];
	//REAL		_size[3];

	std::vector<c_tuio_item>			_receiving;

private:
//	INT32	_blob_max_nb_ui;

	//INT32	_event_touch_count;
	//INT32	_event_touch_id[TOUCH_NB_MAX];
	//INT32	_event_untouch_count;
	//INT32	_event_untouch_id[TOUCH_NB_MAX];

	//_tuio_item		_blobs[BLOB_NB_UI];
	INT32			_blob_nb;

private:
			void		render_blobs_in_img();
	//		void		render_blobs();

			void		TUIO_PRINTF(C_PCHAR fmt, ...);
			void		write_img( FP32 CONST x_in, FP32 CONST y_in, INT32 CONST channel_index, FP32 CONST value );
			
public:
	virtual	void		param_init_pt();

	virtual	void		update();
	virtual	void		draw_single();
	virtual	void		draw_multiple();

			void		osc_receive_tuio_2Dcur( CONST osc::ReceivedMessage& msg );

			void		osc_process_message( CONST osc::ReceivedMessage& msg );
};
