

#ifdef AAA_CAPTURE_CENSYS_H
#error "CAPTURE_CENSYS_H included more than once."
#endif
#define AAA_CAPTURE_CENSYS_H 1


#ifndef AAA_PTGREY_UTILS_H
#	include "ptgrey_utils.h"
#endif

#if AAA_USE_POINT_GREY()

#ifndef AAA_CAPTURE_VIDEO_H
#	include "../capture_video.h"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

//============================================================================= 
// PGR Includes
//=============================================================================
#if	!defined(__CENSYS_H__) //&& !AAA_WIN64()
#	include "censys.h"
#endif


#ifndef AAA_CENSYS_TRACKING_H
#	include "capture_censys_tracking.h"
#endif
#ifndef AAA_CENSYS_BUMBLEBEE_H
#	include "capture_censys_bumblebee.h"
#endif

class	c_censys_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_censys_ui, c_obj_ui );
private:
	INT32				_camera_id_ui;

	bool				_b_reset_background_ui;
	bool				_b_print_trigger_ui;
	bool				_b_dot_send_verbose_ui;
	bool				_b_dot_send_value_verbose_ui;
	INT32				_dot_speed_ui;
	bool				_b_dot_save_ui;
	bool				_b_dot_send_ui;

	bool				_b_censys3d_param_no_update_ui;

	INT32				_tracking_fps_ui;

	// grabber settings
	INT32				_stereo_resolution_x_ui;
	INT32				_stereo_resolution_y_ui;
	//REAL				GrabDelay_ui;
	// segmentor settings
	REAL				_position_x_ui;	//Camera transformation parameters
	REAL				_position_y_ui;
	REAL				_position_z_ui;
	REAL				_rotation_x_ui;
	REAL				_rotation_y_ui;
	REAL				_rotation_z_ui;
	REAL				_min_z_distance_ui;	// Minimum z-distance (meters) from the camera 
	REAL				_max_z_distance_ui;	// Maximum z-distance (meters) from the camera 
	REAL				_creep_rate_ui;		// The rate at which foreground pixels merge into the background (disparity pixels per second)
	REAL				_depth_threshold_ui; // Height threshold between pixels considered background and pixels considered foreground (meters)
	// extractor settings
	INT32				_point_threshold_ui;	// Number of points in a vertical point column to consider it containing a person
	REAL				_granularity_ui;	// Square size of vertical point columns used as low-pass filters to detect people (meters)
	REAL				_proximity_filter_radius_ui;	//Maximum radius of a person in the ground (x-y) plane (meters)
	REAL				_minimum_height_drop_ui;	// Minimum drop in height expected between people (meters)
	REAL				_voi_min_x_ui;	// minimum bounds in the x-dimension (meters).
	REAL				_voi_max_x_ui;	// maximum bounds in the x-dimension (meters).
	REAL				_voi_min_y_ui;	// minimum bounds in the y-dimension (meters).
	REAL				_voi_max_y_ui;	// maximum bounds in the y-dimension (meters).
	REAL				_min_height_people_ui;	// Minimum and maximum height of people
	REAL				_max_height_people_ui;
	// tracker settings
	INT32				_minimum_path_points_ui;	// Minimum number of path points for a person to be tracked (greater than zero)
	INT32				_min_path_points_penalty_ui;	// Additional path points constraint applied to a person whose first path point does not lie within the image border
	INT32				_frames_before_deleting_person_ui;	// Number of frames before a tracked person with no new path points is no longer considered (greater than zero).
	INT32				_frames_before_deleting_candidate_person_ui; // Same as frames_before_deleting_person except applies to candidate people.
	INT32				_frames_before_deleting_person_in_border_ui;	// Same as frames_before_deleting_person except applies to tracked people and candidate people whose last path point lies in the image border.
	REAL				_max_valid_person_velocity_ui;		// Maximum xy velocity for a person being tracked to be considered valid (meters per second)
	REAL				_maximum_velocity_angle_ui;		// Maximum velocity-angle product that can exist in a person's trajectory

	REAL				_dot_offset_x_ui;
	REAL				_dot_offset_y_ui;
	REAL				_dot_offset_z_ui;

	REAL				_dot_scale_x_ui;
	REAL				_dot_scale_y_ui;
	REAL				_dot_scale_z_ui;

	////experimental
	//REAL				closeMatchDistance_ui;	// If a person is within this distance of another person, do not delete them since they may just be temporarily obscured by the other person (meters, 0 - disabled).
	//REAL				maxExcursion_ui;	// Maximum radius of search space used to maintain tracking (meters, 0 - disabled).
	//REAL				matchTieThreshold_ui;	// If 2 match scores are within this threshold of each other, they are considered a "tie", and the path point is assigned to neither person (0 - disabled).

	c_bumblebee			_bumblebee[2];

public:

			void	set_reset_background( bool p_in )				{ _b_reset_background_ui = p_in; }
			void	set_print_trigger( bool p_in )					{ _b_print_trigger_ui = p_in; }
			void	set_tracking_fps( INT32 p_in )					{ _tracking_fps_ui = p_in; }

	FINLINE	INT32	get_camera_id()									{ return _camera_id_ui; }
	FINLINE	bool	is_reset_background()							{ return _b_reset_background_ui; }
	FINLINE	bool	get_print_trigger()								{ return _b_print_trigger_ui; }
	FINLINE	INT32	get_tracking_fps()								{ return _tracking_fps_ui; }
	FINLINE	bool	is_dot_send_verbose()							{ return _b_dot_send_verbose_ui; }
	FINLINE	bool	is_dot_send_value_verbose()						{ return _b_dot_send_value_verbose_ui; }
	FINLINE	INT32	get_dot_speed()									{ return _dot_speed_ui; }
	FINLINE	bool	is_dot_save()									{ return _b_dot_save_ui; }
	FINLINE	bool	is_dot_send()									{ return _b_dot_send_ui; }

	FINLINE	bool	is_censys3d_param_no_update()					{ return _b_censys3d_param_no_update_ui; }
	FINLINE	INT32	get_stereo_resolution_x()						{ return _stereo_resolution_x_ui; }
	FINLINE	INT32	get_stereo_resolution_y()						{ return _stereo_resolution_y_ui; }
	//FINLINE	REAL	get_GrabDelay()								{ return GrabDelay_ui; }
	FINLINE	REAL	get_position_x()								{ return _position_x_ui; }
	FINLINE	REAL	get_position_y()								{ return _position_y_ui; }
	FINLINE	REAL	get_position_z()								{ return _position_z_ui; }
	FINLINE	REAL	get_rotation_x()								{ return _rotation_x_ui; }
	FINLINE	REAL	get_rotation_y()								{ return _rotation_y_ui; }
	FINLINE	REAL	get_rotation_z()								{ return _rotation_z_ui; }
	FINLINE	REAL	get_min_z_distance()							{ return _min_z_distance_ui; }
	FINLINE	REAL	get_max_z_distance()							{ return _max_z_distance_ui; }
	FINLINE	REAL	get_creep_rate()								{ return _creep_rate_ui; }
	FINLINE	REAL	get_depth_threshold()							{ return _depth_threshold_ui; }
	FINLINE	INT32	get_point_threshold()							{ return _point_threshold_ui; }
	FINLINE	REAL	get_granularity()								{ return _granularity_ui; }
	FINLINE	REAL	get_proximity_filter_radius()					{ return _proximity_filter_radius_ui; }
	FINLINE	REAL	get_minimum_height_drop()						{ return _minimum_height_drop_ui; }
	FINLINE	REAL	get_voi_min_x()									{ return _voi_min_x_ui; }
	FINLINE	REAL	get_voi_min_y()									{ return _voi_min_y_ui; }
	FINLINE	REAL	get_voi_max_x()									{ return _voi_max_x_ui; }
	FINLINE	REAL	get_voi_max_y()									{ return _voi_max_y_ui; }
	FINLINE	REAL	get_min_height_people()							{ return _min_height_people_ui; }
	FINLINE	REAL	get_max_height_people()							{ return _max_height_people_ui; }
	FINLINE	INT32	get_minimum_path_points()						{ return _minimum_path_points_ui; }
	FINLINE	INT32	get_min_path_points_penalty()					{ return _min_path_points_penalty_ui; }
	FINLINE	INT32	get_frames_before_deleting_person()				{ return _frames_before_deleting_person_ui; }
	FINLINE	INT32	get_frames_before_deleting_candidate_person()	{ return _frames_before_deleting_candidate_person_ui; }
	FINLINE	INT32	get_frames_before_deleting_person_in_border()	{ return _frames_before_deleting_person_in_border_ui; }
	FINLINE	REAL	get_max_valid_person_velocity()					{ return _max_valid_person_velocity_ui; }
	FINLINE	REAL	get_maximum_velocity_angle()					{ return _maximum_velocity_angle_ui; }

	FINLINE	REAL	get_dot_offset_x()								{ return _dot_offset_x_ui; }
	FINLINE	REAL	get_dot_offset_y()								{ return _dot_offset_y_ui; }
	FINLINE	REAL	get_dot_offset_z()								{ return _dot_offset_z_ui; }
	FINLINE	REAL	get_dot_scale_x()								{ return _dot_scale_x_ui; }
	FINLINE	REAL	get_dot_scale_y()								{ return _dot_scale_y_ui; }
	FINLINE	REAL	get_dot_scale_z()								{ return _dot_scale_z_ui; }
	//FINLINE	virtual REAL	get_closeMatchDistance()					{ return closeMatchDistance_ui; }
	//FINLINE	virtual REAL	get_maxExcursion()							{ return maxExcursion_ui; }
	//FINLINE	virtual REAL	get_matchTieThreshold()						{ return matchTieThreshold_ui; }
	
	c_capture_bumblebee*	get_bumblebee( INT32 index );

	virtual	void	param_init_pt();
	virtual	void	update()										{}

	virtual	AAA_ERR	load_do_after( o_str CONST & filename_in );
	virtual	AAA_ERR	save_do_after( o_str CONST & filename_in );

};

class	c_capture_censys final : public c_capture
{
public:
	static	bool		b_dll_loaded;
private:
	static UINT32		device_count;
private:
	c_censys_ui*			_censys;

//	c_bumblebee*			bumblebee_settings[2];
	c_capture_bumblebee*	_bumblebee_settings_0;
	c_capture_bumblebee*	_bumblebee_settings_1;
	c_image_flux*			_image_flux_2;
	c_image_flux*			_image_flux_3;

#if !AAA_WIN64()
	CENSYS_SYSTEM_CONFIG	_censys_config;
	CENSYS_INSTANCE			_censys_inst;
	TriclopsContext*		_triclops_context;
	DigiclopsContext*		_digiclops_context;
	CameraGUIContext		_pgrcamGuiContext;

	// Try to match grab framerate with system framerate
	DigiclopsFrameRate		_frameRate;
#endif	//#if !AAA_WIN64()
	DWORD*					_camera_serial_number;
	INT32					_nb_camera;

	bool					_b_init_cam;

	INT32					_tracking_fps;

	//CENSYS_IMAGE_TYPE	image_type;

	bool					_b_dot_send_verbose;
	bool					_b_dot_send_value_verbose;
	INT32					_dot_speed;
	bool					_b_dot_save;
	bool					_b_dot_send;

	bool					_b_censys3d_param_no_update;


	INT32					_camera_id;
	// grabber settings
	INT32					_stereo_resolution_x;
	INT32					_stereo_resolution_y;
	//REAL	grabDelay; //Time to delay before grabbing subsequent image (milliseconds)
	// segmentor settings
	REAL					_position_x;		// Camera transformation parameters
	REAL					_position_y;
	REAL					_position_z;
	REAL					_rotation_x;
	REAL					_rotation_y;
	REAL					_rotation_z;
	REAL					_min_z_distance;	// Minimum z-distance (meters) from the camera 
	REAL					_max_z_distance;	// Maximum z-distance (meters) from the camera 
	REAL					_creep_rate;		// The rate at which foreground pixels merge into the background (disparity pixels per second)
	REAL					_depth_threshold;	// Height threshold between pixels considered background and pixels considered foreground (meters)
	// extractor settings
	INT32					_point_threshold;	// Number of points in a vertical point column to consider it containing a person
	REAL					_granularity;		// Square size of vertical point columns used as low-pass filters to detect people (meters)
	REAL					_proximity_filter_radius;	//Maximum radius of a person in the ground (x-y) plane (meters)
	REAL					_minimum_height_drop;	// Minimum drop in height expected between people (meters)
	REAL					_voi_min_x;			// minimum bounds in the x-dimension (meters).
	REAL					_voi_max_x;			// maximum bounds in the x-dimension (meters).
	REAL					_voi_min_y;			// minimum bounds in the y-dimension (meters).
	REAL					_voi_max_y;			// maximum bounds in the y-dimension (meters).
	REAL					_min_height_people;	// Minimum and maximum height of people
	REAL					_max_height_people;
	// tracker settings
	INT32					_minimum_path_points;			// Minimum number of path points for a person to be tracked (greater than zero)
	INT32					_min_path_points_penalty;		// Additional path points constraint applied to a person whose first path point does not lie within the image border
	INT32					_frames_before_deleting_person;	// Number of frames before a tracked person with no new path points is no longer considered (greater than zero).
	INT32					_frames_before_deleting_candidate_person; // Same as frames_before_deleting_person except applies to candidate people.
	INT32					_frames_before_deleting_person_in_border;	// Same as FramesBeforeDeletingPerson except applies to tracked people and candidate people whose last path point lies in the image border.
	REAL					_max_valid_person_velocity;		// Maximum xy velocity for a person being tracked to be considered valid (meters per second)
	REAL					_maximum_velocity_angle;		// Maximum velocity-angle product that can exist in a person's trajectory

	REAL					_dot_offset_x;
	REAL					_dot_offset_y;
	REAL					_dot_offset_z;
	REAL					_dot_scale_x;
	REAL					_dot_scale_y;
	REAL					_dot_scale_z;

	////experimental
	//REAL	closeMatchDistance_;	// If a person is within this distance of another person, do not delete them since they may just be temporarily obscured by the other person (meters, 0 - disabled).
	//REAL	maxExcursion_;	// Maximum radius of search space used to maintain tracking (meters, 0 - disabled).
	//REAL	matchTieThreshold_;	// If 2 match scores are within this threshold of each other, they are considered a "tie", and the path point is assigned to neither person (0 - disabled).

	c_censys_tracking		_m_censys_tracking;

			void	get_triclops_context();
			void	get_digiclops_context();
			void	get_camera_serial_number();

			void	get_params();		//update parameters

			void	set_b_init_cam( bool CONST p_in )				{ _b_init_cam = p_in; }
			void	init_cam();

#if AAA_WIN64()
			void	set_resolution()						{}
			void	set_transformation()					{}
			void	set_min_max_z_distance()				{}
			void	set_depth_threshold()					{}
			void	set_creep_rate()						{}
			void	set_simple_voi()						{}
			void	set_granularity()						{}
			void	set_point_threshold()					{}
			void	set_proximity_filter_radius()			{}
			void	set_min_height_drop_between_people()	{}
			void	set_max_valid_person_velocity()			{}
			void	set_frames_before_deleting_person()		{}
			void	set_min_path_points()					{}
			void	set_max_velocity_angle()				{}

			void	set_frames_before_deleting_candidate_person()		{}
			void	set_frames_before_deleting_person_in_border()		{}
			void	set_min_path_points_penalty()			{}

			void	reset_background()						{}
			void	print_settings()						{}
#else
			void	set_resolution();
			void	set_transformation();
			void	set_min_max_z_distance();
			void	set_depth_threshold();
			void	set_creep_rate();
			void	set_simple_voi();
			void	set_granularity();
			void	set_point_threshold();
			void	set_proximity_filter_radius();
			void	set_min_height_drop_between_people();
			void	set_max_valid_person_velocity();
			void	set_frames_before_deleting_person();
			void	set_min_path_points();
			void	set_max_velocity_angle();

			void	set_frames_before_deleting_candidate_person();
			void	set_frames_before_deleting_person_in_border();
			void	set_min_path_points_penalty();

			void	reset_background();
			void	print_settings();
#endif //#if AAA_WIN64()
public:
	static	void	c_init();
	static	void	c_deinit();

	c_capture_censys();
	virtual			~c_capture_censys();

#if AAA_WIN64()
	static	INT32	do_enum( bool CONST b_verbose );

	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr )	{ return ERR_ANY; }
	virtual	void	close_specific()		{}

	virtual	bool	run( bool CONST b_stream_in )				{ return false; }
	virtual	void	stop()		{}

	virtual	void	update()	{}
#else
	static	INT32	do_enum( bool CONST b_verbose );

	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void	close_specific();

	virtual	bool	run( bool CONST b_stream_in );
	virtual	void	stop();

	virtual	void	update();

	// Censys data callback functions
	static	void __cdecl	callback_grabber(	PGRABBER_DATA	pData, void* pArg  );
	static	void __cdecl	callback_segmentor( PSEGMENTOR_DATA pData, void* pArg  );
	static	void __cdecl	callback_extractor( PEXTRACTOR_DATA pData, void* pArg  );
	static	void __cdecl	callback_tracker(	PTRACKER_DATA	pData, void* pArg  );
			void			do_got_frame( UINT32 index_flux, CENSYS3D_IMAGE_TYPE image_type, PSEGMENTOR_CAMERA_DATA camera_data );
#endif	//#if AAA_WIN64()
	c_censys_ui*	get_censys()						{ return _censys; }
	void			set_censys( c_censys_ui* p_in )		{ _censys = p_in; }

	// Getting & setting of tracked people information
	virtual c_censys_tracking*	get_censys_tracking();
	//	virtual void setTrackedPeople( CONST PTRACKER_DATA pTP );

	virtual	void	dlg_source();
	virtual	void	dlg_display();
	virtual	void	ask_frame()	{}

	virtual	UINT32	get_device_count()		{ return device_count; }
			void	tracking_fps_new()		{ ++_tracking_fps; }
};

#endif	//#if AAA_USE_POINT_GREY()