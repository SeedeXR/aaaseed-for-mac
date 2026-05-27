
#ifdef AAA_BDD_BLOB_TRACKING_H
#error "BDD_BLOB_TRACKING_H included more than once."
#endif
#define AAA_BDD_BLOB_TRACKING_H 1


#ifndef	AAA_BDD_BLOB_H
#	include "obj_ui/bdd/bdd_point/bdd_blob.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef _DEQUE_
#	include <deque>
#endif
#ifndef _UNORDERED_MAP_
#	include <unordered_map>
#endif
#ifndef AAA_COLORRND_H
#	include "draw/colorrnd.h"
#endif
#ifndef AAA_BLOB_H
#	include "blob.h"
#endif
#ifndef AAA_TRACKING_MATRIX_H
#	include "tracking/tracking_matrix.h"
#endif

class c_flux_filter;

class c_blob_ext
{
public:
	INT32	_id;
	INT32	_id_tag;

	REAL	_x;
	REAL	_y;
	REAL	_x_filtered;
	REAL	_y_filtered;
	REAL	_dx;
	REAL	_dy;

	REAL	_size_x;
	REAL	_size_y;
	REAL	_angle;
	REAL	_area;
	INT32	_point_nb;
	bool	_b_ghost;

public:
	c_blob_ext() :
		_id(0), _id_tag(0),
		_x(0), _y(0),
		_x_filtered(0), _y_filtered(0),
		_dx(0), _dy(0),
		_size_y(0), _size_x(0),
		_angle(0), _area(0), _point_nb(0), _b_ghost(false)
		{}
		FINLINE void	from_finger( c_finger& fing )
		{
			_id = fing._id_tracking;
			_id_tag = fing._id_tag;

			_x = fing._center.get_x();
			_y = fing._center.get_y();
			_x_filtered = fing._x_filtered;
			_y_filtered = fing._y_filtered;
			_dx = fing._delta.get_x();
			_dy = fing._delta.get_y();

			_point_nb = fing._point_nb;
			_angle = fing._angle;
			_area = fing._area;

			_size_x = fing._box.get_size_x();
			_size_y = fing._box.get_size_y();
			_b_ghost = fing._b_marked_for_deletion;
		}

};

namespace osc
{
	class	ReceivedMessage;
};

class	c_bdd_blob_tracking final : public c_bdd_blob_base
{
	FACTORY_DECLARE( c_bdd_blob_tracking, c_bdd_blob_base );
public:
	static	CONST	INT32	BLOB_NB_UI = 32;
	static	CONST	INT32	BLOB_NB_MAX = 1024;
	static	CONST	INT32	TOUCH_NB_MAX = 1024;
	static	c_bdd_blob_tracking*	cur;

private:
	bool		_b_verbose_ui;
	bool		_b_verbose_matrix_ui;

	bool		_b_mouse_use;
	bool		_b_mouse_use_ui;
	bool		_b_mouse_use_when_no_touch_ui;
	INT32		_mouse_ignore_count;
	bool		_b_mouse_button_left_down;
	c_finger	_fing_mouse;

	INT32		_size_x;
	INT32		_size_y;

	INT32		_src_img_index;

	bool		_b_restart_trig_ui;

	// Net in/out
	bool		_b_net_in;
	bool		_b_net_out;
	bool		_b_tuio_in;
	bool		_b_tuio_in_skip_alive;
	UINT32		_tuio_silent_frame_max_nb;
	bool		_b_tuio_in_flip_x;
	bool		_b_tuio_in_flip_y;
	REAL		_tuio_in_scale_x;
	REAL		_tuio_in_scale_y;
	REAL		_tuio_in_offset_x;
	REAL		_tuio_in_offset_y;
	REAL		_tuio_in_blob_size_ui[3];
	REAL		_tuio_in_blob_size_half[2];
//	bool		_b_tuio_simulator;
	bool		_b_tuio_out;
	REAL		_tuio_out_scale[2];
	REAL		_tuio_out_offset[2];
	INT32		_s_net_verbose_in;
	FINLINE	bool	is_net_in_verbose()				{ return _s_net_verbose_in > 0; }
	FINLINE	bool	is_net_in_verbose_detailed()	{ return _s_net_verbose_in > 1; }

//	bool		_b_tuio_verbose_out;
	bool		_b_net_as_string;
	INT32		_osc_dst;
	o_str		_osc_header;
//	bool		_b_net_need_reset_current;
	bool		_b_net_received;
	UINT32		_net_no_frame;

	INT32		_frame_in;
	INT32		_frame_out;

	INT32		_current_id;

	REAL		_origin[3];
	REAL		_size[3];

	bool		_b_mouse;
	FP32		_mouse_x	{-42};
	FP32		_mouse_y	{-42};

	std::vector<c_finger>						_received;
	std::vector<c_finger>						_received_bis;
	std::vector<c_finger>*						_p_received_ready;
	std::vector<c_finger>*						_p_receiving;

	std::vector<c_finger>						_current;
	std::deque< std::vector<c_finger> >			_history;

	typedef std::pair<c_flux_filter*,c_flux_filter*> pair_p_filter;
	std::unordered_map< INT32, pair_p_filter >	_filter;

	bool			_b_color_auto_ui;
	c_color_drawer	_color_drawer;

	void		maintain_history_size();
private:
	REAL	_w_pos;
	REAL	_w_max;
	REAL	_w_step;

	bool	_b_bounding_draw_as_ellipse_ui;
	REAL	_bounding_size_factor;
	bool	_b_contour_draw_with_quad;

	INT32	_s_draw;
	REAL	_min_dist_ui;
	REAL	_reject_dist_ui;
	INT32	_blob_max_nb_ui;

	INT32	_ghost_frame_nb_ui;
	INT32	_frames_nb;				// nb of frame we store
	INT32	_frames_nb_to_draw;
	INT32	_frames_nb_to_draw_ui;

	REAL	_segment_alpha;
	REAL	_contour_alpha;
	REAL	_bounding_alpha;

	INT32	_event_touch_id[TOUCH_NB_MAX];
	INT32	_event_untouch_id[TOUCH_NB_MAX];
	INT32	_event_touch_count;
	INT32	_event_untouch_count;

	REAL	_filter_factor_ui;
	REAL	_filter_factor_x_ui;
	REAL	_filter_factor_y_ui;
	REAL	_filter_factor_x;
	REAL	_filter_factor_y;

	bool	_b_matrix_use_ui;
	INT32	_branch_try_max;
	bool	_b_use_predicted_pos_ui;
	bool	_b_use_squared_dist_ui;

	c_blob_ext	_blobs[BLOB_NB_UI];
	INT32		_blob_nb;

	bool	_b_read_img_contour_as_src;
	bool	_b_read_bdd_blob_as_src;

	c_tracking_matrix	_matrix;

private:
	FINLINE	REAL				get_error( c_finger& prev, c_finger& now );
	FINLINE void				push_solution( std::vector<INT32>& solution, std::vector<INT32>& ids, REAL error, INT32 iteration_count );
	FINLINE	void				assign_new_finger_with_id( c_finger* pt, INT32 tracking_id );
			void				assign_new_finger( c_finger* pt );
			void				assign_old_finger( c_finger* pt, INT32 old_index );

	FINLINE	void				do_filter( c_finger* fing );
			//c_finger*	find_finger( int hist, int id );

	FINLINE	void				draw_color(							c_finger const & fing,	REAL alpha );

	FINLINE	void				draw_segment_one( const c_finger& fing );

	FINLINE	void				draw_contour_one_color(				c_finger & fing,		REAL alpha );
	FINLINE	void				draw_contour_bounding_one_color(	c_finger const & fing,	REAL alpha );
	FINLINE	void				draw_segment_one_color(				c_finger const & fing,	REAL alpha );

	FINLINE	void				draw_tracking_one(					c_finger & fing );

			void				draw_tracking( std::vector<c_finger>& fingers );
			void				draw_tracking();
			void				draw_history_one( INT32 index );
			void				draw_history();
			void				draw_history_raw();
			void				update_ui_blobs();

			void				osc_send_2Dobj();
			void				osc_send_2dcur();
			void				osc_send_finger();
			void				net_send_finger_string();

			void				build_from_old( std::vector<c_finger>& current, std::vector<c_finger>& prev );
			void				process_from_mouse();

			void				clear_event();
			void				event_add_touch( UINT32 id );
			void				event_add_touch( c_finger* pfing );
			void				event_add_untouch( UINT32 id );
			void				event_add_untouch( c_finger* pfing );

			void				add_finger_to_current(	c_finger CONST & fing );	
			bool				add_blob_to_current(	c_blob	 CONST & blob );	
public:
	virtual	void				param_init_pt();

			void				update();
			void				draw_single();
			void				draw_multiple();

			void				restart();

			void				process_results();
			INT32				get_finger_nb();
			c_finger*			get_finger( INT32 index );

			bool				get_ghost( INT32 index );	//todo add a more generic solution with a property name
	virtual	INT32				get_point_nb();
	virtual	INT32				get_point_and_id(					REAL* dst, INT32 CONST index );
	virtual	bool				get_point_and_speed_rnd_time_rel(	REAL* CONST dst, REAL* CONST speed, REAL CONST t_in );

	virtual	UINT32				get_blob_nb()			{	return get_finger_nb();					}
	virtual	c_blob CONST &		get_blob_index(INT32 i)	{	//todo check this tracking_id stuff quickly done for LV_KNF tokyo
															c_finger * finger = get_finger(i);
															finger->set_id( finger->_id_tracking );
															return (c_blob CONST & ) *finger;
														}
//			BLOBS_CONT CONST &	get_blobs()				{	return _blob;			}
/*
			bool				get_blob_info( UINT32 index, c_blob_ext& data );
			bool				get_blob_info_by_id( UINT32 id, c_blob_ext& data );
*/
	//todo	add fnd to get touch and untouch event directly (thru Lua)
	//todo	reactivate tuio protocols

			void				osc_receive_tuio_2Dcur( CONST osc::ReceivedMessage& msg );
			void				osc_receive_tuio_2Dobj( CONST osc::ReceivedMessage& msg );
			void				osc_receive_aaa_finger( CONST osc::ReceivedMessage& msg );

			void				osc_process_message( CONST osc::ReceivedMessage& msg );
};



