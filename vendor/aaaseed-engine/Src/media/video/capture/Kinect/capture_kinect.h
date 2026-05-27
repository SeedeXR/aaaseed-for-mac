

#ifdef AAA_CAPTURE_KINECT_H
#error "CAPTURE_KINECT_H included more than once."
#endif
#define AAA_CAPTURE_KINECT_H 1


#ifndef AAA_CAPTURE_VIDEO_H
#	include "../capture_video.h"
#endif
#ifndef AAA_KINECT_UI_H
#	include "kinect_ui.h"
#endif
#ifndef KINECTWIN32
#	include "Kinect-win32.h"
#endif
#ifndef K4A_H
#	include "k4a/k4a.h"
#endif

extern	void	KINECT_PRINT_STRING( C_PCHAR_C fmt, ...);

class	c_capture_kinect;
class	c_bdd_mocap;
typedef struct _NUI_SKELETON_DATA	NUI_SKELETON_DATA;

struct	INuiSensor;
struct	INuiCoordinateMapper;
struct	ICoordinateMapper;

class c_k1_kinect_listener : Kinect::KinectListener
{
public:
	unsigned short	t_gamma[2048];

	Kinect::Kinect*		_k1_kinect;
	c_capture_kinect*	_capture_kinect;

	static	CONST	INT32	 DEPTH_RANGE = 6 * 6 * 256;

	c_k1_kinect_listener( Kinect::Kinect* inK );

	//virtual	void KinectDisconnected(Kinect::Kinect *K) {}
	virtual	void DepthReceived( Kinect::Kinect* K );
	virtual	void ColorReceived( Kinect::Kinect* K );
	void Run( c_capture_kinect* capture_kinect );
	void Stop();
};

class c_kinect_ui;

class	c_capture_kinect final : public c_capture
{
private:
	static	bool	b_sdk_k4;
	static	bool	b_sdk_k2;
	static	bool	b_sdk_k1;

private:
	static UINT32				device_count;

	k4a_device_t				_k4_device;
	k4a_capture_t				_k4_capture;
	k4a_device_configuration_t	_k4_config;
	k4a_transformation_t		_k4_transformation;

public:
	static	bool	b_allow;
	static	bool	b_sdk_k4_ui;
	static	bool	b_sdk_k2_ui;
	static	bool	b_sdk_k1_ui;
	static	INT32	do_enum( bool CONST b_verbose );
	static	void	c_init();
	static	void	c_deinit();
	static	DWORD	WINAPI	k1_nui_process_callback( LPVOID pParam );

	static	bool	k2_is_lib_loaded();
	static	bool	k4_is_lib_loaded();
	static	INT32	k4_get_nb();

protected:
//			DWORD					_serial_number;
//			INT32					_camera_id;
			c_kinect_ui*			_ui;
			Kinect::Kinect*			_k1_kinect;
			c_k1_kinect_listener*	_k1_kinect_listener;
			UINT8*					_buf_depth;

			c_image_flux*			_img_flux_rgb;
			c_image_flux*			_img_flux_rgb_aligned;

			INT64					_k2_time_depth;	
			INT64					_k2_time_rgb;	
			INT64					_k2_time_body;

			// Buffers for corrected color image
			UINT32					_k1_sdk_color_aligned[	640 * 480 		];
			LONG					_k1_sdk_mapped_coords[	640 * 480 * 2	];

			ICoordinateMapper*		_k2_mapper;
			INT32					_depth_deform_bind;
			INT32					_k2_depth_pixel_size[2];
			FP32*					_k2_depth_to_cam_table;

	//		NUI_DEPTH_IMAGE_PIXEL	_depth_tex[		640 * 480		];
	//		NUI_DEPTH_IMAGE_POINT	_depth_points[	640 * 480		];
			bool					_b_k1_sdk_mapping_ok;

			INuiSensor*				_k1_sdk_p_nui_instance;
			INuiCoordinateMapper*	_k1_sdk_p_mapper;
			HANDLE					_k1_hd_event_depth;
			HANDLE					_k1_hd_event_rgb;
			HANDLE					_k1_hd_event_skeleton;

			HANDLE					_k1_hd_stream_depth;
			HANDLE					_k1_hd_stream_rgb;

			HANDLE					_k1_hd_event_stop;
			HANDLE					_k1_hd_nui_process;
			// camera settings
			bool					_b_emitter_ir;

protected:
			AAA_ERR		open_driver_k1_sdk( INT32 CONST index, o_str CONST * CONST o_device_id  );
			AAA_ERR		open_driver( INT32 CONST index, o_str CONST * CONST o_device_id  );
			AAA_ERR		disconnect_driver();

			bool		set_depth_buf_size( INT32 sx, INT32 sy, aaa::PIXEL_FORMAT format = aaa::PIXEL_FORMAT::RGBA_8 );
			INT32		k1_nui_process();
			void		transfert_skeleton( c_bdd_mocap* mo, NUI_SKELETON_DATA& skel, INT32 nb );
//			void		convert_skeleton_to_rgb( c_bdd_mocap* mo, NUI_SKELETON_DATA& skel, INT32 nb );
			C_PCHAR		get_error_string( CONST HRESULT hr );

			void		erase_rects_depth( INT32 CONST sx, INT32 CONST sy, INT32 CONST stride );

			AAA_ERR		k4_open( INT32 CONST index, o_str CONST * CONST pt_device_id );	// params unused for now
			void		k4_close();
			AAA_ERR		k4_run();
			void		k4_stop();

			void		k4_update();

			//todo use for other kinect
			AAA_ERR		k4_set_rgb_flux( INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST pixel_format_in );
			void		k4_update_depth_frame();
			void		k4_update_rgb_frame();
			void		k4_update_color_control();
			void		k4_dump_color_control( k4a_color_control_command_t CONST command );
			void		k4_dump_color_control_all();

//			void		k4_update_skel_frame();

			AAA_ERR		k2_open( INT32 CONST index, o_str CONST * CONST pt_device_id );	// params unused for now
			void		k2_close();
			AAA_ERR		k2_run();
			void		k2_stop();

			void		k2_do_depth( INT64 CONST time, UINT16 CONST * buf, INT32 CONST sx, INT32 CONST sy );	//, INT32 depth_min, INT32 depth_max );
			void		k2_update();

			void		k2_stop_depth();
			void		k2_stop_rgb();
			void		k2_stop_skel();

			AAA_ERR		k2_start_depth();
			AAA_ERR		k2_start_rgb();
			AAA_ERR		k2_start_skel();
			void		k2_cpy_skeleton( c_bdd_mocap* mo, c_kinect_ui::c_skel CONST * CONST skel );
			void		k2_do_skeleton();

			INT32		_k2_skel_tracked_index[6];

			void		k2_update_depth_frame();
			void		k2_update_rgb_frame();
			void		k2_update_skel_frame();

			void		stop_low();
			AAA_ERR		run_low(	bool CONST b_stream_in );

			void		process_skel();
public:

	FINLINE	static bool	is_sdk()		{ return b_sdk_k1; }
	static	void		callback_status( HRESULT hrStatus );
			void		k1_do_depth_sdk();
			void		do_depth_stuff();
			void		k1_do_rgb_sdk();
			void		do_rgb_stuff();
			void		k1_do_skeleton();

			c_capture_kinect();
	virtual	~c_capture_kinect();

	virtual	AAA_ERR			open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void			close_specific();

			bool			k1_run_sdk(	bool CONST b_stream_in );
	virtual	bool			run(		bool CONST b_stream_in );

			void			k1_stop_sdk();
	virtual	void			stop();

	virtual	void			update();
	virtual	void			ask_frame()	{}

	FINLINE	INT32			get_image_2_bind()		CONST				{ return _ui->get_image_2_bind(); }

	virtual	c_image_flux*	get_image_flux( INT32 CONST index );

	FINLINE c_kinect_ui*	get_kinect() CONST							{ return _ui; }
	FINLINE	void			set_kinect_ui( c_kinect_ui* CONST p_in )	{ _ui = p_in; }

	virtual	UINT32			get_device_count()							{ return device_count; }

	virtual	UINT32			get_image_flux_bind(	UINT32 CONST index );
	virtual	bool			is_keep_on_cpu(			UINT32 CONST index );
	virtual	bool			is_keep_on_gpu(			UINT32 CONST index );

/*
	//ft stand for Face Track
			void ft_init();
			void ft_test();
			void ft_deinit();
*/
};

