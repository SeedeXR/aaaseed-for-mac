

#ifdef AAA_CAPTURE_DUO3D_H
#error "CAPTURE_DUO3D_H included more than once."
#endif
#define AAA_CAPTURE_DUO3D_H 1


#ifndef AAA_WRAP_DUO3D_H
#	include "wrap_duo3d.h"
#endif

#if AAA_USE_DUO3D()

#ifndef AAA_CAPTURE_VIDEO_H
#	include "capture_video.h"
#endif

#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

#ifndef _DUOLIB_H
// Include DUO API header file
#	include "Duo3d/DUOLib.h"
#endif

#ifndef _DENSE3D_H
// Include DUO API header file
#	include "Duo3d/Dense3D.h"
#endif

#ifndef AAA_OPEN_CV_IMAGE_H
#	include "wrapper/open_cv_image.h"
#endif

class	c_capture_duo3d;


class	c_capture_duo3d_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_capture_duo3d_ui, c_obj_ui );
	friend	c_capture_duo3d;
protected:

	UINT32		_size_x_ui;
	UINT32		_size_y_ui;
	UINT32		_binning_x_ui;
	UINT32		_binning_y_ui;

	bool		_b_flip_horizontal_ui;
	bool		_b_flip_vertical_ui;
	bool		_b_swap_camera_ui;


	UINT32		_num_disparities;
	UINT32		_num_disparities_ui;
	UINT32		_sad_window_size_ui;
	UINT32		_uniqueness_ratio_ui;
	o_str		_license;
	o_str		_left_intrisic;
	o_str		_right_intrisic;

	o_str		_camera_serial;
	o_str		_camera_name;
	o_str		_camera_firmware_version;
	o_str		_camera_firmware_build;

	bool		_b_gpu_right_ui;
	INT32		_bind_right_ui;
//	bool		_b_cpu_right_ui;
//	INT32		_bind_cpu_right_ui;
//	INT32		_img_right_index;
	INT32		_right_image_index;

	bool		_b_get_rectified_ui;	// get rectified images

	bool		_b_depth_asked_ui;
	bool		_b_depth_color_ui;
	bool		_b_gpu_depth_ui;
	INT32		_bind_depth_ui;
//	bool		_b_cpu_depth_ui;
//	INT32		_bind_cpu_depth_ui;
//	INT32		_img_depth_index;
	INT32		_depth_image_index;

	REAL		_led_power_ui;
	REAL		_gain_ui;
	REAL		_exposure_ui;


	bool		_b_accelerometer_out;	// True if accelerometer chip is present
	REAL		_accel_data[3];			// DUO accelerometer data
	REAL		_gyro_data[3];			// DUO gyroscope data
	REAL		_mag_data[3];			// DUO magnetometer data
	REAL		_temp_data;				// DUO temperature data

public:
	virtual	void	param_init_pt();

			void	set_license(			o_str CONST * CONST str )	{ _license.set( str );			}
			void	set_serial(				C_PCHAR_C str )				{ _camera_serial.set( str );	}
			void	set_name(				C_PCHAR_C str )				{ _camera_name.set( str );		}
			void	set_firmware_build(		C_PCHAR_C str )				{ _camera_firmware_build.set( str );	}
			void	set_firmware_version(	C_PCHAR_C str )				{ _camera_firmware_version.set( str );	}


			INT32	get_image_2_bind();
			INT32	get_image_3_bind();
};

class	c_capture_duo3d final : public c_capture
{
private:
	static UINT32		device_count;
public:
	static	bool		b_dll_duo3d_loaded;
	static	bool		b_dll_dense3d_loaded;
	//static	bool		b_allow;
	CRITICAL_SECTION		_thread_lock;
	//c_recursive_benaphore	_light_lock;
protected:

private:

	bool				_b_init;

	INT32				_cap_size_x;
	INT32				_cap_size_y;
	INT32				_cap_framerate;
	c_capture_duo3d_ui*	_cap_ui;


	REAL				_led_power;
	REAL				_gain;
	REAL				_exposure;
	bool				_b_flip_horizontal;
	bool				_b_flip_vertical;
	bool				_b_swap_camera;


	bool					_b_device_opened;


	c_image_flux*			_img_right;
	c_image_flux*			_img_depth;
	bool					_b_get_rectified;

	DUOInstance				_duo;
	DUOResolutionInfo		_duo_resolution;

	bool					_b_do_depth;
	bool					_b_depth_opened;
	bool					_b_depth_color;
	Dense3DInstance			_dense3d;

	UINT32					_num_disparities;
	UINT32					_sad_window_size;
	INT32					_uniqueness_ratio;


	cv::Mat					_left_rect;
	cv::Mat					_right_rect;
	cv::Mat					_disparity;
	cv::Mat					_depth3d;
	cv::Mat					_disp8;
	cv::Mat					_colorLut;
	cv::Mat					_depth_rgb;

			AAA_ERR			open_device();
			AAA_ERR			close_device();

			AAA_ERR			open_depth();
			AAA_ERR			close_depth();
			void			do_depth( PDUOFrame pFrameData );
public:
	static	void			c_init();
	static	void			c_deinit();
	static	INT32			do_enum( bool CONST b_verbose );

	c_capture_duo3d();
	virtual	~c_capture_duo3d();

	virtual	AAA_ERR			open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void			close_specific();

	virtual	bool			run( bool CONST b_stream_in );
	virtual	void			stop();

	virtual	void			update();
	virtual	void			ask_frame();

			void			process_frame_low( PDUOFrame pFrameData );

	virtual	UINT32			get_device_count()							{ return device_count; }
	virtual	c_image_flux*	get_image_flux( INT32 CONST index );

	c_capture_duo3d_ui*		get_capture_ui()							{ return _cap_ui; }
			void			set_capture_ui( c_capture_duo3d_ui* p_in )	{ _cap_ui = p_in; }

	virtual	UINT32		get_image_flux_bind( UINT32 CONST index )
								{
									if ( _cap_ui )
									{
										if ( index == 1 )	return _cap_ui->get_image_2_bind();
										if ( index == 2 )	return _cap_ui->get_image_3_bind();
									}
									return 0;
								}

};

#endif
