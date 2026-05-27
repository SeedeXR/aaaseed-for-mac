

#ifdef AAA_CAPTURE_VIDEO_H
#error "CAPTURE_VIDEO_H included more than once."
#endif
#define AAA_CAPTURE_VIDEO_H 1


#ifndef AAA_PTGREY_UTILS_H
#	include "PointGrey/ptgrey_utils.h"
#endif
#ifndef AAA_IMAGE_FLUX_H
#	include "../image_flux.h"
#endif`

	enum TRICLOPS_IMAGE_TYPE : INT32
	{
		TRI_IMAGE_NO = 0,
		TRI_IMAGE_RAW_LEFT,
		TRI_IMAGE_RECT_LEFT,
		TRI_IMAGE_EDGE_LEFT,
		TRI_IMAGE_RAW_RIGHT,
		TRI_IMAGE_RECT_RIGHT,
		TRI_IMAGE_EDGE_RIGHT,
		TRI_IMAGE_DEPTH, 
		TRICLOPS_IMAGE_MAX_NB,
	};

	enum CENSYS3D_IMAGE_TYPE : INT32
	{
		IMAGE_SCENE_DISPARITY = 0, 
		IMAGE_FORE_DISPARITY,
		IMAGE_BACK_DISPARITY,
		IMAGE_SCENE,
		IMAGE_FOREGROUND,
		IMAGE_BACKGROUND,
		CENSYS3D_IMAGE_MAX_NB,
	};

	enum CENSYS_FRAMERATE_TYPE : INT32
	{
		CENSYS_FRAMERATE_100,	// 100 percent
		CENSYS_FRAMERATE_050,	//  50 percent
		CENSYS_FRAMERATE_025,	//  25 percent
		CENSYS_FRAMERATE_012,	//  12 percent
		CENSYS_FRAMERATE_MAX_NB,
	};


class	c_capture : public c_image_flux
{
public:
	enum class CAPTURE_SIZE : INT32
	{
		CUSTOM = 0,
		PAL, 
		NTSC,
		HD720p,
		HD1080p,
		MAX_NB,
	};

	static CONSTEXPR INT32 DEVICE_MAX_NB = 16;

	static	C_PCHAR_C	size_str[ (INT32)CAPTURE_SIZE::MAX_NB ];

	static	bool	b_init_ptgrey_ui;
	static	bool	b_init_jai_ui;

	static	void	c_init();
	static	void	c_deinit();
	static	void	trig_device_changed();

	static	bool	b_verbose;	//should be called verbose_class or verbose_static or ...
	static	void	set_verbose( bool CONST b_in );
	static	void	flip_verbose();
private:
	//PIXEL_FORMAT		_pixel_format;
protected:
	bool	_b_opened;
	bool	_b_running;
public:
	FINLINE	bool	is_opened()		CONST		{ return _b_opened;		}
	FINLINE	void	set_opened( bool CONST b )	{ _b_opened = b;		}

	FINLINE	bool	is_running()	CONST		{ return _b_running;	}
	FINLINE	void	set_running( bool CONST b )	{ _b_running = b;		}
protected:
//	INT32	device_index_;
	bool	_b_streaming;	//todo Maa made it symmetric but its role is strange
							//	act as "is running" flag but seems to double active and/or open flag
//solve this local verbose stuff
	bool	_b_verbose;


	HWND	_hd_wind;	//	still there because it used to search for triclops...
	bool	_b_stream;
	bool	_b_preview;

	INT32	_s_field;
//	INT32	_frame_captured_index;
//	INT32	_audio_captured_index;

//	INT32	_interval_millisec;

	INT32	_device_index_used;

	bool	_b_capture_force_default;
	bool	_b_capture_force_fps;

	INT32	_capture_size_x;
	INT32	_capture_size_y;
	INT32	_crossbar_index;	// only used in DirectShow
	o_str	_o_crossbar_name;	// only used in DirectShow
	o_str	_o_device_id;		// only used in DirectShow and Kinect Azure 

	UINT32	_image_flux_count;	// Some capture have multiple image flux : Kinect, Triclops, Duo3d
	//VideoProcAmp
//	bool	_b_vpa_use_default;
//	bool	_b_vpa_use_default_auto;
//	bool	_b_vpa_first;

	INT32	_frames_dropped;
	INT32	_frames_delivered;

	// SAVING AS AVI
	o_str	_o_avi_filename;
	bool	_b_avi_opened;
	bool	_b_avi_run;
	bool	_b_avi_run_last;
	REAL	_avi_quality;
	REAL	_avi_size;
	UINT32	_avi_frame_nb;

public:
	static void inc_device_enum_count();
	static INT32 get_device_enum_count();

public:
	static	void	CAPTURE_PRINT_STRING( C_PCHAR_C fmt, ... );

	c_capture();
	virtual	~c_capture();

			bool	open(			INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr );
	virtual	AAA_ERR	open_specific(	INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id = nullptr ) = 0;

	virtual	void	close_specific()						= 0;
			void	close();

	virtual	bool	run( bool CONST b_stream_in = true )	= 0;
	virtual	void	stop();

	virtual	void	update()								= 0;

	virtual	void	ask_frame()								= 0;
	FINLINE	bool	is_streaming()		    CONST	{ return _b_streaming;			}
//	virtual	INT32	get_bit_depth()					{	return 32;					}

	virtual	void	set_preview( bool CONST b_in )	{}

	virtual	void	dlg_source()					{}
	virtual	void	dlg_format()					{}
	virtual	void	dlg_display()					{}

	virtual	UINT32	get_device_count()				{ return 0;						}
			INT32	get_device_index_used() CONST	{ return _device_index_used;	}
			INT32	get_frames_dropped()    CONST	{ return _frames_dropped;		}
			INT32	get_frames_delivered()  CONST	{ return _frames_delivered;		}
/*			void	stream_index_clear();
			void	frame_index_inc();
			void	audio_index_inc();
			INT32	get_frame_index();
*/
			HWND	get_hd_wind()		    CONST	{	return _hd_wind;			}

			void	dlg();
			//void	convert_to_rgb( bool CONST b_flip, bool CONST b_fast = true );
			//bool	convert_to_rgb_next_field( bool CONST b_flip, bool CONST b_fast = true );

			//solve this local verbose stuff
//			void	set_verbose( bool CONST in )	{	_b_verbose = in;			}

//	FINLINE	PIXEL_FORMAT	get_pixel_format()												{ return _pixel_format;					}
//	FINLINE	VOID			set_pixel_format( PIXEL_FORMAT pixel_format )					{ _pixel_format = pixel_format;			}
																							  
	virtual	UINT32			get_image_flux_bind( UINT32 CONST index )						{ return 0;								}
	//todo should make these for all the object
	virtual	bool			is_keep_on_cpu( UINT32 CONST index )							{ return true;							}
	virtual	bool			is_keep_on_gpu( UINT32 CONST index )							{ return true;							}

	virtual	UINT32			get_image_flux_count()											{ return _image_flux_count;				}
	virtual	c_image_flux*	get_image_flux( INT32 CONST index )								{ return nullptr;						}

	virtual	void			set_default_size( INT32 CONST x, INT32 CONST y );

	virtual	void			set_use_default_value( bool CONST in )							{ _b_capture_force_default = in;		}
			void			set_force_framerate( bool CONST b_in )							{ _b_capture_force_fps = b_in;			}
	virtual bool			set_crossbar( INT32 CONST index )								{ return true;							}
	virtual o_str*			get_crossbar_name_pt()											{ return &_o_crossbar_name;				}
			C_PCHAR_C		get_device_id()			CONST									{ return _o_device_id.get();			}

	virtual	void			set_avi_filename( o_str CONST * CONST filename )				{ _o_avi_filename.add( filename->get() );	}
	virtual	void			set_avi_quality( REAL CONST val )								{ _avi_quality = val;					}
	virtual	bool			avi_start()														{ return false;							}
	virtual	void			avi_stop()														{}
	virtual	bool			avi_open(  o_str CONST * CONST filename, REAL CONST quality )	{ return false;							}
	virtual	void			avi_close()														{}
	virtual	REAL			get_avi_size()													{ return _avi_size;						}
	virtual	UINT32			get_avi_frame_nb()												{ return _avi_frame_nb;					}
};


