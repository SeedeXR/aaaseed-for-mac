

#ifdef AAA_CAPTURE_CENSYS_BUMBLEBEE_H
#error "CAPTURE_CENSYS_BUMBLEBEE_H included more than once."
#endif
#define AAA_CAPTURE_CENSYS_BUMBLEBEE_H 1


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
//#if !AAA_WIN64()
#ifndef __CENSYS_H__
#	include "censys.h"
#endif
#ifndef __PGRCAMERAGUI_H__
#	include "pgrcameragui.h"
#endif
//#include "digiclops.h"
//#include "pnmutils.h"
//#endif //#if !AAA_WIN64()

enum TRICLOPS_STEREO_QUALITY_TYPE
{
	STANDARD = 0, 
	ENHANCED, 
	TRICLOPS_STEREO_QUALITY_MAX_NB,
};

class c_bumblebee;

class	c_capture_bumblebee
{
public:
protected:
			c_bumblebee*		_bumblebee_ui;

#if !AAA_WIN64()
			TriclopsContext		_triclops_context;
			DigiclopsContext	_digiclops_context;
			CameraGUIContext	_pgrcamGuiContext;
			// Try to match grab framerate with system framerate
			DigiclopsFrameRate	_frame_rate;
#endif //#if !AAA_WIN64()
			DWORD				_serial_number;
			INT32				_camera_id;

			bool				_b_censys3d_param_no_update;

			CENSYS3D_IMAGE_TYPE	_image_type;
			CENSYS3D_IMAGE_TYPE	_image_2_type;
			CENSYS3D_IMAGE_TYPE	_image_3_type;
			INT32				_image_2_bind;
			INT32				_image_3_bind;


			// camera settings
			bool	_b_camera_dialog;
			bool	_b_gain_auto;
			bool	_b_shutter_auto;
			bool	_b_exposure_auto;
			INT32	_gain;
			INT32	_shutter;
			INT32	_exposure;
			// segmentor settings
			REAL	_delta_disparity; // Disparity threshold between adjacent points for them to be considered valid
			INT32	_edge_mask;		// Size of stereo processing edge mask filter (pixels)
			INT32	_stereo_mask;		// Size of stereo processing stereo mask (pixels)
			INT32	_segmentation_value;	// Minimum number of adjacent pixels to be considered a valid surface
			REAL	_texture_validation_threshold; //Threshold controlling texture-based validation of pixels.
			REAL	_uniqueness_validation_threshold; // Threshold controlling validation of pixels based upon the uniqueness of the match of a pixel to its corresponding pixels in the top and left images.

			bool							_b_sub_pixel_interpolation;
			TRICLOPS_STEREO_QUALITY_TYPE	_stereo_quality;


			void	open_camera_dialog();
			void	close_camera_dialog();

public:
					c_capture_bumblebee();
	virtual			~c_capture_bumblebee();

	virtual	AAA_ERR	open_specific()			{ return AAA_OK; }

			void	set_censys3d_param_no_update( bool p_in )	{ _b_censys3d_param_no_update = false; /*p_in;*/ }	//hack
			void	set_bumblebee_ui( c_bumblebee* p_in )		{ _bumblebee_ui = p_in; }
			void	set_camera_id( INT32 p_in );

#if !AAA_WIN64()
	virtual	void	close()					{}
			void	get_params()			{}	//update parameters
			void	update_params()			{}
#else
	virtual	void	close_specific();

			void	set_edge_mask();
			void	set_stereo_mask();
			void	set_delta_disparity();
			void	set_segmentation_value();
			void	set_texture_validation_threshold();
			void	set_uniqueness_validation_threshold();

			void	set_frame_rate();
			void	set_camera_dialog();
			void	set_camera_gain();
			void	set_camera_shutter();
			void	set_camera_exposure();

			void	set_sub_pixel_interpolation();
			void	set_stereo_quality();

			CENSYS3D_IMAGE_TYPE	get_image_type()		{ return _image_type; }

			CENSYS3D_IMAGE_TYPE	get_image_2_type()		{ return _image_2_type; }
			INT32	get_image_2_bind()					{ return _image_2_bind; }

			CENSYS3D_IMAGE_TYPE	get_image_3_type()		{ return _image_3_type; }
			INT32	get_image_3_bind()					{ return _image_3_bind; }

			void	get_params();		//update parameters
			void	update_params();

			void	get_triclops_context( CENSYS_INSTANCE censys_inst );
			void	get_digiclops_context( CENSYS_INSTANCE censys_inst );
			void	get_serial_number( CENSYS_INSTANCE censys_inst );

			void	print_settings();
#endif /#if !AAA_WIN64()
};

extern	void	CENSYS_PRINT_STRING( C_PCHAR_C fmt, ...);

class	c_bumblebee final : public c_obj_ui
{
	FACTORY_DECLARE(c_bumblebee,c_obj_ui);

public:
	c_capture_bumblebee			_capture_bumblebee;

protected:
	CENSYS3D_IMAGE_TYPE	_image_type_ui;

	CENSYS3D_IMAGE_TYPE	_image_2_type_ui;
	UINT32				_image_2_bind_ui;

	CENSYS3D_IMAGE_TYPE	_image_3_type_ui;
	UINT32				_image_3_bind_ui;

	// camera settings
	DWORD				_serial_number_ui;
	INT32				_camera_id_ui;
	INT32				_frame_rate_ui;
	bool				_b_camera_dialog_ui;
	bool				_b_gain_auto_ui;
	bool				_b_shutter_auto_ui;
	bool				_b_exposure_auto_ui;
	INT32				_gain_ui;
	INT32				_shutter_ui;
	INT32				_exposure_ui;
	// segmentor settings
	REAL				_delta_disparity_ui; // Disparity threshold between adjacent points for them to be considered valid
	INT32				_edge_mask_ui;		// Size of stereo processing edge mask filter (pixels)
	INT32				_stereo_mask_ui;		// Size of stereo processing stereo mask (pixels)
	INT32				_segmentation_value_ui;	// Minimum number of adjacent pixels to be considered a valid surface
	REAL				_texture_validation_threshold_ui; //Threshold controlling texture-based validation of pixels.
	REAL				_uniqueness_validation_threshold_ui; // Threshold controlling validation of pixels based upon the uniqueness of the match of a pixel to its corresponding pixels in the top and left images.

	bool							_b_sub_pixel_interpolation_ui;
	TRICLOPS_STEREO_QUALITY_TYPE	_stereo_quality_ui;

public:

					void				set_camera_dialog( bool p_in )			{ _b_camera_dialog_ui = p_in; }

					void				set_serial_number( DWORD p_in )			{ _serial_number_ui = p_in; }
					void				set_camera_id( INT32 p_in)				{ _camera_id_ui = p_in; }
//todoq FINLINE ?
					CENSYS3D_IMAGE_TYPE	get_image_type()						{ return _image_type_ui; }
					CENSYS3D_IMAGE_TYPE	get_image_2_type()						{ return _image_2_type_ui; }
					INT32				get_image_2_bind()						{ return _image_2_bind_ui; }
					CENSYS3D_IMAGE_TYPE	get_image_3_type()						{ return _image_3_type_ui; }
					INT32				get_image_3_bind()						{ return _image_3_bind_ui; }
	FINLINE	virtual INT32				get_frame_rate()						{ return _frame_rate_ui; }
	FINLINE	virtual bool				is_camera_dialog()						{ return _b_camera_dialog_ui; }
	FINLINE	virtual bool				is_gain_auto()							{ return _b_gain_auto_ui; }
	FINLINE	virtual bool				is_shutter_auto()						{ return _b_shutter_auto_ui; }
	FINLINE	virtual bool				is_exposure_auto()						{ return _b_exposure_auto_ui; }
	FINLINE	virtual INT32				get_gain()								{ return _gain_ui; }
	FINLINE	virtual INT32				get_shutter()							{ return _shutter_ui; }
	FINLINE	virtual INT32				get_exposure()							{ return _exposure_ui; }
	FINLINE	virtual INT32				get_edge_mask()							{ return _edge_mask_ui; }
	FINLINE	virtual REAL				get_delta_disparity()					{ return _delta_disparity_ui; }
	FINLINE	virtual INT32				get_stereo_mask()						{ return _stereo_mask_ui; }
	FINLINE	virtual INT32				get_segmentation_value()				{ return _segmentation_value_ui; }
	FINLINE	virtual REAL				get_texture_validation_threshold()		{ return _texture_validation_threshold_ui; }
	FINLINE	virtual REAL				get_uniqueness_validation_threshold()	{ return _uniqueness_validation_threshold_ui; }

	FINLINE	virtual bool				is_sub_pixel_interpolation()			{ return _b_sub_pixel_interpolation_ui; }
	FINLINE	virtual TRICLOPS_STEREO_QUALITY_TYPE	get_stereo_quality()		{ return _stereo_quality_ui; }

			virtual	void			param_init_pt();
			virtual	void			update() {}

//	void			update_params();
};

#endif	//#if AAA_USE_POINT_GREY()
