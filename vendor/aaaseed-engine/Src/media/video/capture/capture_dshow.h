#ifdef AAA_CAPTURE_DSHOW_H
#error "CAPTURE_DSHOW_H included more than once."
#endif
#define AAA_CAPTURE_DSHOW_H 1


#ifndef AAA_CAPTURE_VIDEO_H
#	include "capture_video.h"
#endif
#if 0	// because we use CComObject
#	ifndef __ATLBASE_H__
#		include <atlbase.h>
#	endif
#	ifndef __ATLCOM_H__
#		include <atlcom.h>
#	endif
#else
#	ifndef __ATLCOMCLI_H__
#		include <atlcomcli.h>
#	endif
#endif



class c_ds_cap_ui;

struct IGraphBuilder;
struct ICaptureGraphBuilder2;
struct IBaseFilter;
struct IPin;

struct IAMCrossbar;
struct IAMTuner;
struct IAMTVTuner;
struct IAMVfwCaptureDialogs;
struct IAMStreamConfig;
struct IAMVideoProcAmp;

struct IMediaControl;
struct IMediaEventEx;

struct IAMDroppedFrames;

struct IuEyeCapture;
struct IuEyeCapturePin;
struct IuEyeColorTemperature;
struct IuEyeCaptureEx;

#define AAA_USE_DS_TRANSFORM_FILTER() 0
#if AAA_USE_DS_TRANSFORM_FILTER()
	class c_aaa_ds_transform_filter;
#else
	class c_aaa_ds_renderer_filter;
#endif
//	we use a separate DirectShow node c_aaa_ds_transform_filter or c_aaa_ds_renderer_filter in the capture graph
class	c_capture_dshow final : public c_capture //, public ISampleGrabberCB not any more 
{
public:
	static	CONSTEXPR	UINT32	UEYE_RGB_COLOR_MODEL_MAX = 5;
	static	CONST		INT32	rgb_color_model[UEYE_RGB_COLOR_MODEL_MAX];

	enum	PICK_MODE : INT32
	{
		PICK_FASTEST_BIGGEST,
		PICK_FASTEST_SMALLEST,
		PICK_SLOWEST_BIGGEST,
		PICK_SLOWEST_SMALLEST,
		PICK_BIGGEST_FASTEST,
		PICK_BIGGEST_SLOWEST,
		PICK_SMALLEST_FASTEST,
		PICK_SMALLEST_SLOWEST,
		PICK_NB,
	};

	static	INT32				get_device_enumed();		//todo remove similar member fns ?
	static	o_str CONST * CONST get_device_name( INT32 CONST index );
	static	o_str CONST * CONST get_mode_supported_text( INT32 CONST device_index, INT32 CONST index );

	static	AAA_ERR				enumerate_device_mode(  INT32 CONST device_index, CComPtr<IMoniker> p_video_device_moniker );
	static	INT32 CONST *		get_device_mode_supported_nb_pt( INT32 CONST device_index );

	virtual	UINT32 				get_device_count()			{ return get_device_enumed(); }

private:
	// before this class was derived too from DirectShow ISampleGrabber or ISampleGrabberCB
	//    since 2026 January we pass pointer to and external filter or renderer which call got_frame
	//IUnknown interface
	//HRESULT	WINAPI	QueryInterface( REFIID iid, void** ppvObject );
	//ULONG	WINAPI	AddRef();
	//ULONG	WINAPI	Release();

	//// ISampleGrabberCB interfaces for DS callback
	////HRESULT	WINAPI	SampleCB( double SampleTime, IMediaSample *pSample );
	////HRESULT	WINAPI	BufferCB( double sampleTimeSec, BYTE* bufferPtr, long bufferLength );
	//STDMETHODIMP	SampleCB( double SampleTime, IMediaSample *pSample ) override;
	//STDMETHODIMP	BufferCB( double sampleTimeSec, BYTE* bufferPtr, long bufferLength ) override;
	//unsigned long					_ref_count;

	INT32							_frame_count;

	bool							_b_use_format_index;
	INT32							_format_index;
	bool							_b_use_format_index_asked;
	INT32							_format_index_asked;

	CComPtr<IMoniker>				_p_video_device_moniker;

	// Some essentials for the filter graph.
	CComPtr<IGraphBuilder>			_p_graph_builder;
	CComPtr<ICaptureGraphBuilder2>	_p_cap_graph_builder;
	CComPtr<IMediaControl>			_p_graph_control;
//	CComPtr<IMediaEventEx>			_p_media_event_ex;

	//	Pointers to filters and pins in the graph.
	//		built in init
	CComPtr<IBaseFilter>			_p_cap_filter;
	CComPtr<IPin>					_p_real_capture_pin;			// the one on the cap filter
	CComPtr<IPin>					_p_real_preview_pin;			// the one on the cap filter
	CComPtr<IPin>					_p_cap_filter_video_port_pin;	// on cap filter

	CComPtr<IAMCrossbar>			_p_crossbar;
	CComPtr<IAMCrossbar>			_p_crossbar2;
	CComPtr<IAMTuner>				_p_tuner;
	CComPtr<IAMTVTuner>				_p_tv_tuner;

	CComPtr<IAMCrossbar>			_p_video_crossbar;		// This aliases either p_crossbar or p_crossbar2.
	INT32							_video_crossbar_output;

	CComPtr<IAMVfwCaptureDialogs>	_p_vfw_dialogs;
	CComPtr<IAMStreamConfig>		_p_video_config_cap;
	CComPtr<IAMStreamConfig>		_p_video_config_prv;

	CComPtr<IAMVideoProcAmp>		_p_video_proc_amp;

	//		built in build
	CComPtr<IPin>					_p_capture_pin;

#if AAA_USE_DS_TRANSFORM_FILTER()
	c_aaa_ds_transform_filter*		_p_aaa_filter;
#else
	c_aaa_ds_renderer_filter*		_p_aaa_filter;
#endif
	CComPtr<IAMDroppedFrames>		_p_dropped_frames;

	// uEye specific
	CComPtr<IuEyeCapturePin>		_p_cap_ueye_pin;
	bool							_b_ueye;
	UINT32							_eye_bandwidth;
	REAL							_eye_pixelclock;
	REAL							_eye_pixelclock_ui;
	UINT32							_eye_pixelclock_range_min;
	UINT32							_eye_pixelclock_range_max;
	UINT32							_eye_pixelclock_range_default;

	REAL							_eye_exposure_time;
	REAL							_eye_exposure_time_ui;
	UINT32							_eye_exposure_range_min;
	UINT32							_eye_exposure_range_max;
	UINT32							_eye_exposure_range_interval;

	CComPtr<IuEyeCapture>			_p_cap_ueye;


	CComPtr<IuEyeColorTemperature>	_p_eye_color_temp;
	bool							_b_eye_rgb_model_ok;
	UINT32							_eye_rgb_model_mode;
	UINT32							_eye_rgb_model_mode_ui;
	UINT32							_eye_rgb_model_mode_default;

	bool							_b_eye_color_temp_ok;
	REAL							_eye_color_temp;
	REAL							_eye_color_temp_ui;
	UINT32							_eye_color_temp_default;
	UINT32							_eye_color_temp_min;
	UINT32							_eye_color_temp_max;
	UINT32							_eye_color_temp_interval;

	CComPtr<IuEyeCaptureEx>			_p_eye_capture_ex;
	bool							_b_eye_capture_ex;
	bool							_b_eye_gain_boost;
	bool							_b_eye_hard_gamma;
	bool							_b_eye_gain_boost_ui;
	bool							_b_eye_hard_gamma_ui;

//	CComPtr<IuEyeSaturation>		_p_eye_saturation;
//	bool							_b_eye_saturation_ok;
////REAL							_eye_saturation;
////UINT32							_eye_saturation_val;
//	UINT32							_eye_saturation_default;
//	UINT32							_eye_saturation_min;
//	UINT32							_eye_saturation_max;
//	UINT32							_eye_saturation_interval;

	REAL							_brightness;
	REAL							_contrast;
	REAL							_hue;
	REAL							_saturation;
	REAL							_sharpness;
	REAL							_gamma;
	REAL							_color_enable;
	REAL							_white_balance;
	REAL							_backlight_compensation;
	REAL							_gain;

	bool							_b_brightness_auto;
	bool							_b_contrast_auto;
	bool							_b_hue_auto;
	bool							_b_saturation_auto;
	bool							_b_sharpness_auto;
	bool							_b_gamma_auto;
	bool							_b_color_enable_auto;
	bool							_b_white_balance_auto;
	bool							_b_backlight_compensation_auto;
	bool							_b_gain_auto;

	// DV support
	bool							_b_dv_supported;

	bool							_b_device_requires_terminal_samplegrabber;

	c_ds_cap_ui*					_ds_ui;

	// Misc flags & state
#if	AAA_DEBUG()
	DWORD							_register_graph;				// Used to register filter graph in ROT for graphedit
#endif
	bool							_b_graph_built;					//	true when graph is built
	INT32							_device_index_being_built;		//	I know name is long but if anybodia have better
	bool							_b_graph_running;				//	true when graph is running
	bool							_b_graph_has_preview;			//	true if the graph has separate capture and preview pins
																	//		set but unused for now 3 sep 2008
	// pixel format
	INT32							_cap_size_x;
	INT32							_cap_size_y;
	REAL							_cap_fps;


			void	get_eye_bandwith();
			void	update_ueye_settings();
			void	get_ueye_capture();
			void	get_ueye_capture_pin();
			void	get_ueye_color_temp();
	//		void	get_ueye_saturation();
			void	get_ueye_capture_ex();
			void	get_ueye_settings();
	//		void	clear_format();

			
			AAA_ERR	init_graph_device( REAL CONST framerate, bool CONST b_use_format_index, CONST INT32 format_index );
			void	close_graph_device();
			void	close_graph();
			void	destroy_graph();
			bool	stop_graph();
			bool	start_graph();
			AAA_ERR	init_graph_capture();
			HRESULT	save_graph( CComPtr<IGraphBuilder> pGraph, CONST WCHAR * CONST wszPath );
			void	save_graph();


			INT32	find_device( INT32 CONST index, o_str CONST * CONST device_id );
			bool	display_property_pages( IUnknown *ptr, CONST HWND hwndParent );
			bool	set_callback( bool CONST b_set );

	virtual	void	dlg_source();
	virtual	void	dlg_format();
	virtual	void	dlg_display();

			void	display_crossbar_info( CComPtr<IAMCrossbar> pXBar );
	const	CHAR*	get_physical_pin_name( CONST LONG type );

	HRESULT			vap_reset_parameters( bool CONST b_auto );

	HRESULT			vap_set_parameters_norm( CONST LONG property, DOUBLE CONST value, bool CONST b_auto );
	HRESULT			vap_set_parameters_to_default( CONST LONG property, bool CONST b_auto );
	HRESULT			vap_set_parameter( CONST LONG property, CONST LONG value, bool CONST b_auto ); 
	HRESULT			vap_get_parameter_range( CONST LONG property, LONG *pMin, LONG *pMax, LONG *pSteppingDelta, LONG *pDefault, LONG *pCapsFlags );
	HRESULT			vap_get_parameter( CONST LONG property, LONG* value, bool* b_auto );

public:
	static	void	c_init();
	static	void	c_deinit();
	static	INT32	do_enum( bool CONST b_verbose );

	c_capture_dshow();
	virtual	~c_capture_dshow();

	o_str CONST * CONST get_mode_supported( INT32 CONST index );

	virtual	AAA_ERR	open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	void	close_specific();

	virtual	bool	run( bool CONST b_stream_in = true );
	virtual	void	stop();

	virtual	void	update();
	virtual	void	ask_frame()	{}

			bool	is_ueye()	CONST				{	return _b_ueye;	}
//			void	set_vpa_use_default(			bool b_use_default, bool b_use_default_auto );
			void	set_vpa_brightness(				REAL CONST p_in, bool CONST b_auto_in );
			void	set_vpa_contrast(				REAL CONST p_in, bool CONST b_auto_in );
			void	set_vpa_hue(					REAL CONST p_in, bool CONST b_auto_in );
			void	set_vpa_saturation(				REAL CONST p_in, bool CONST b_auto_in );
			void	set_vpa_sharpness(				REAL CONST p_in, bool CONST b_auto_in );
			void	set_vpa_gamma(					REAL CONST p_in, bool CONST b_auto_in );
			void	set_vpa_color_enable(			REAL CONST p_in, bool CONST b_auto_in );
			void	set_vpa_white_balance(			REAL CONST p_in, bool CONST b_auto_in );
			void	set_vpa_backlight_compensation(	REAL CONST p_in, bool CONST b_auto_in );
			void	set_vpa_gain(					REAL CONST p_in, bool CONST b_auto_in );

	virtual bool	set_crossbar(					INT32 CONST index );

	c_ds_cap_ui*	get_ds_ui()	CONST				{ return _ds_ui; }
	void			set_ds_ui( c_ds_cap_ui* p_in )	{ _ds_ui = p_in; }

			UINT32	get_eye_exposure_time();
			void	set_eye_exposure_time( REAL CONST in );

			UINT32	get_eye_pixelclock();
			void	set_eye_pixelclock( REAL CONST in );

			bool	is_eye_rgb_model()	CONST		{ return _b_eye_rgb_model_ok;			}
			UINT32	get_eye_rgb_model_mode();
			void	set_eye_rgb_model_mode( UINT32 CONST in );
//			UINT32	get_eye_rgb_model_default()		{ return _eye_rgb_model_mode_default;	}

			bool	is_eye_color_temp()	CONST		{ return _b_eye_color_temp_ok;			}
			UINT32	get_eye_color_temp();
			void	set_eye_color_temp( REAL CONST in );

			//bool	is_eye_saturation()				{ return _b_eye_saturation_ok;			}
			//UINT32	get_eye_saturation();
			//void	set_eye_saturation( REAL in );

			bool	is_eye_gain_boost();
			bool	is_eye_hard_gamma();
			void	set_eye_gain_boost( bool CONST b_in );
			void	set_eye_hard_gamma( bool CONST b_in );
};
