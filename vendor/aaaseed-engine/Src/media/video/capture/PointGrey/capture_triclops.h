

#ifdef AAA_CAPTURE_TRICLOPS_H
#error "CAPTURE_TRICLOPS_H included more than once."
#endif
#define AAA_CAPTURE_TRICLOPS_H 1


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
#if !AAA_WIN64()
#	ifndef TRICLOPS_H
#		include "triclops.h"
#	endif
#	ifndef __DIGICLOPS_H__
#		include "digiclops.h"
#	endif
#	ifndef _PNMUTILS_H_
#		include "pnmutils.h"
#	endif
#	ifndef __PGRCAMERAGUI_H__
#		include "pgrcameragui.h"
#	endif
#endif //#if !AAA_WIN64()

class	c_triclops_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_triclops_ui,c_obj_ui );
public:
	static	C_PCHAR_C	digiclops_camera_type_str[ 2 ];
	static	C_PCHAR_C	digiclops_camera_device_str[ 3 ];
	static	C_PCHAR_C	digiclops_camera_resolution_str[ 4 ];
	static	C_PCHAR_C	triclops_stereo_quality_str[ 2 ];
	static	C_PCHAR_C	triclops_camera_image_str[ TRICLOPS_IMAGE_MAX_NB ];

protected:
	INT32					_serial_number_ui;
	INT32					_bandwidth;

	TRICLOPS_IMAGE_TYPE		_image_type_ui;

	TRICLOPS_IMAGE_TYPE		_image_2_type_ui;
	UINT32					_image_2_bind_ui;

	TRICLOPS_IMAGE_TYPE		_image_3_type_ui;
	UINT32					_image_3_bind_ui;


	INT32			_disparity_min_ui;
	INT32			_disparity_max_ui;

	REAL			_disp_min_ui;
	REAL			_disp_max_ui;

	INT32			_stereo_resolution_x_ui;
	INT32			_stereo_resolution_y_ui;

#if AAA_WIN64()
	INT32			_stereo_quality_ui;
#else
	TriclopsStereoQuality		_stereo_quality_ui;
#endif
	INT32			_stereo_mask_ui;
	bool			_b_edge_correlation_ui;
	INT32			_edge_mask_ui;

	bool			_b_sub_pixel_interpolation_ui;
	bool			_b_sub_pixel_interpolation_strict_ui;
	UINT8			_sub_pixel_validation_mapping_ui;

	bool			_b_surface_validation_ui;
	INT32			_surface_validation_size_ui;
	REAL			_surface_validation_difference_ui;
	UINT8			_surface_validation_mapping_ui;

	bool			_b_texture_validation_ui;
	REAL			_texture_validation_threshold_ui;
	UINT8			_texture_validation_mapping_ui;

	bool			_b_uniqueness_validation_ui;
	REAL			_uniqueness_validation_threshold_ui;
	UINT8			_uniqueness_validation_mapping_ui;

	bool			_b_backforth_validation_ui;
	UINT8			_backforth_validation_mapping_ui;


public:

	FINLINE	INT32	get_disparity_min()						{ return _disparity_min_ui; }
	FINLINE	INT32	get_disparity_max()						{ return _disparity_max_ui; }
	FINLINE	INT32	get_stereo_resolution_x()				{ return _stereo_resolution_x_ui; }
	FINLINE	INT32	get_stereo_resolution_y()				{ return _stereo_resolution_y_ui; }
	FINLINE	INT32	get_stereo_mask()						{ return _stereo_mask_ui; }
	FINLINE	bool	is_edge_correlation()					{ return _b_edge_correlation_ui; }
	FINLINE	INT32	get_edge_mask()							{ return _edge_mask_ui; }
	FINLINE	bool	is_sub_pixel_interpolation()			{ return _b_sub_pixel_interpolation_ui; }
	FINLINE	bool	is_sub_pixel_interpolation_strict()		{ return _b_sub_pixel_interpolation_strict_ui; }
	FINLINE	UINT8	get_sub_pixel_validation_mapping()		{ return _sub_pixel_validation_mapping_ui; }
	FINLINE	bool	is_surface_validation()					{ return _b_surface_validation_ui; }
	FINLINE	INT32	get_surface_validation_size()			{ return _surface_validation_size_ui; }
	FINLINE	REAL	get_surface_validation_difference()		{ return _surface_validation_difference_ui; }
	FINLINE	UINT8	get_surface_validation_mapping()		{ return _surface_validation_mapping_ui; }
	FINLINE	bool	is_texture_validation()					{ return _b_texture_validation_ui; }
	FINLINE	REAL	get_texture_validation_threshold()		{ return _texture_validation_threshold_ui; }
	FINLINE	UINT8	get_texture_validation_mapping()		{ return _texture_validation_mapping_ui; }
	FINLINE	bool	is_uniqueness_validation()				{ return _b_uniqueness_validation_ui; }
	FINLINE	REAL	get_uniqueness_validation_threshold()	{ return _uniqueness_validation_threshold_ui; }
	FINLINE	UINT8	get_uniqueness_validation_mapping()		{ return _uniqueness_validation_mapping_ui; }
	FINLINE	bool	is_backforth_validation()				{ return _b_backforth_validation_ui; }
	FINLINE	UINT8	get_backforth_validation_mapping()		{ return _backforth_validation_mapping_ui; }
#if AAA_WIN64()
	FINLINE	INT32	get_stereo_quality()					{ return _stereo_quality_ui; }
#else
	FINLINE	TriclopsStereoQuality	get_stereo_quality()	{ return _stereo_quality_ui; }
#endif
			TRICLOPS_IMAGE_TYPE		get_image_type()		{ return _image_type_ui; }

			TRICLOPS_IMAGE_TYPE		get_image_2_type()		{ return _image_2_type_ui; }
			INT32	get_image_2_bind()						{ return _image_2_bind_ui; }

			TRICLOPS_IMAGE_TYPE		get_image_3_type()		{ return _image_3_type_ui; }
			INT32	get_image_3_bind()						{ return _image_3_bind_ui; }

	FINLINE	REAL	get_disp_min()							{ return _disp_min_ui; }
	FINLINE	REAL	get_disp_max()							{ return _disp_max_ui; }
	FINLINE	INT32	get_bandwidth()							{ return _bandwidth; }

			void	set_serial( INT32 in )					{ _serial_number_ui = in; }

	virtual	void	param_init_pt();
	virtual	void	update() {};

//	void	pass_param_to_capture_triclops( c_capture_triclops* p);
};


class	c_capture_triclops final : public c_capture
{
protected:
	//
	// defines for the disparity Look-up-table
	//
	static	CONST	INT32	DISPARITY_LUT_SHIFT_BITS = 4;
	static	CONST	INT32	DISPARITY_LUT_BITS = 12;
	static	CONST	INT32	DISPARITY_VALID_LUT_ENTRIES = ( 1 << DISPARITY_LUT_BITS );

	static	bool	b_dll_digiclops_loaded;
	static	bool	b_dll_triclops_loaded;
private:
	static UINT32	device_count;
	/**
	* The look up table for valid 16 bit disparity values.
	* 
	* @see mungeDisparityImage()
	* @see generateLookupTable()
	*/
public:
	UINT8	m_ucSubpixelValidDisparityMapLUT[ DISPARITY_VALID_LUT_ENTRIES ];

	/**
	* The look up table for invalid 16 bit disparity values (i.e. where the
	* 16 bit value is greater than 0xFF00.
	* 
	* @see mungeDisparityImage()
	* @see generateLookupTable()
	*/
	static	UINT8*	m_ucSubpixelInvalidDisparityMapLUT;

protected:
	/** Look up table min disparity. */
	int		m_nLUTMinDisp;

	/** Look up table max disparity. */
	int		m_nLUTMaxDisp;

	/** Look up table min disparity map. */
	UINT8	m_ucLUTMinDispMap;

	/** Look up table max disparity map. */
	UINT8	m_ucLUTMaxDispMap;

	/** Checks and generates the disparity look up table. */ 
	void generate_lut();

protected:
	c_triclops_ui*			_triclops;

#if !AAA_WIN64()
	TriclopsContext		_triclops_context;
	DigiclopsContext	_digiclops_context;
	// PGRCameraGUI handle
	CameraGUIContext	_m_guicontext;

	TriclopsInput		_img_stereo_data;
	TriclopsInput		_img_color_data;
	TriclopsImage		_img_reference;
	TriclopsImage16		_img_depth_16;
	TriclopsColorImage	_img_color;
	TriclopsStereoQuality	_stereo_quality;
#endif //#if !AAA_WIN64()
	DWORD				_triclops_serial_number;
	INT32				_serial_number;


	TRICLOPS_IMAGE_TYPE	_image_type;

	c_image_flux*		_img_2;
	TRICLOPS_IMAGE_TYPE	_image_2_type;
	INT32				_image_2_bind;

	c_image_flux*		_img_3;
	TRICLOPS_IMAGE_TYPE	_image_3_type;
	INT32				_image_3_bind;


	INT32				_stereo_resolution_x;
	INT32				_stereo_resolution_y;


	INT32			_disparity_min;
	INT32			_disparity_max;

	REAL			_disp_min;
	REAL			_disp_max;

	INT32			_stereo_mask;
	bool			_b_edge_correlation;
	INT32			_edge_mask;

	bool			_b_sub_pixel_interpolation;
	bool			_b_sub_pixel_interpolation_strict;
	UINT8			_sub_pixel_validation_mapping;

	bool			_b_surface_validation;
	INT32			_surface_validation_size;
	REAL			_surface_validation_difference;
	UINT8			_surface_validation_mapping;

	bool			_b_texture_validation;
	REAL			_texture_validation_threshold;
	UINT8			_texture_validation_mapping;

	bool			_b_uniqueness_validation;
	REAL			_uniqueness_validation_threshold;
	UINT8			_uniqueness_validation_mapping;

	bool			_b_backforth_validation;
	UINT8			_backforth_validation_mapping;

	void			update_params();	//update parameters

#if !AAA_WIN64()
	AAA_ERR			triclops_got_image( c_image_flux* img_flux, TriclopsImageType type, TriclopsCamera which );
#endif
public:
	static	o_str	version;

	static	void	c_init();
	static	void	c_deinit();
	static	INT32	do_enum( bool CONST b_verbose );

					c_capture_triclops();
	virtual			~c_capture_triclops();
	
			c_triclops_ui*	get_triclops()						{ return _triclops; }
			void		set_triclops( c_triclops_ui* p_in )	{ _triclops = p_in; }
			INT32		get_serial()						{ return _serial_number; }
	virtual	UINT32		get_image_flux_bind( UINT32 CONST index )
								{
									if( _triclops )
									{
										if ( index == 1 )	return _triclops->get_image_2_bind();
										if ( index == 2 )	return _triclops->get_image_3_bind();
									}
									return 0;
								}
#if AAA_WIN64()
	//c_image_flux*	get_image_flux( INT32 index )		{ return nullptr; }
	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr )	{ return ERR_ANY; }
	virtual	void	close_specific()		{}

	virtual	bool	run( bool CONST b_stream_in )		{ return false; }
	virtual	void	stop()		{}

	virtual	void	update()		{}
	virtual	void	ask_frame()		{}

	virtual	void	dlg_source()		{}
	virtual	void	dlg_display()		{}

#else
	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void	close_specific();

	virtual	bool	run( bool CONST b_stream_in );
	virtual	void	stop();

	virtual	void	update();
	virtual	void	ask_frame();

	virtual	void	dlg_source();
	virtual	void	dlg_display();

	virtual	c_image_flux*	get_image_flux( INT32 CONST index );
#endif //#if AAA_WIN64()
	virtual	UINT32	get_device_count()			{ return device_count; }
};

#endif	//#if AAA_USE_POINT_GREY()
