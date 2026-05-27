

#ifdef AAA_DS_AVI_H
#error "DS_AVI_H included more than once."
#endif
#define AAA_DS_AVI_H 1


#ifndef AAA_VIDEO_PLAYER_H
#	include "../video_player.h"
#endif
#if 1	// because we use CComObject
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

#define AAA_USE_GRABBER() 1// we wanted to do it with a renderer like capture_dshow but some code to write and the sound to handle too
#if AAA_USE_GRABBER()
#	ifndef __qedit_maa_h__	// because we still use ISampleGrabberCB
#		include "DXBaseClasses/qedit_maa.h"
#	endif
#endif

struct	IMediaControl;
struct	IMediaEvent;
struct	IBasicAudio;
struct	IMediaPosition;


class	c_movie_ds final : public c_movie_player
#if AAA_USE_GRABBER()
	, public ISampleGrabberCB
#endif
{
protected:
#if AAA_USE_GRABBER()
	//	IUnknown interface
	HRESULT	WINAPI	QueryInterface( REFIID iid, void** ppvObject );
	ULONG	WINAPI	AddRef();
	ULONG	WINAPI	Release();
	//	ISampleGrabberCB interfaces
	HRESULT	WINAPI	SampleCB( double SampleTime, IMediaSample* pSample );
	HRESULT	WINAPI	BufferCB( double sampleTimeSec, BYTE* bufferPtr, long bufferLength );
	unsigned	long	_ref_count;

	double	_sample_time_last	{-42};
#endif
	//	HWND	hd_wind;	//archi move_it out ?
private:
			//	GUID	media_subtype_;
			UINT32	_pitch;
			bool	_b_audio_filter;
			bool	_b_graph_running;

			// Misc flags & state
#if	AAA_DEBUG()
			DWORD	_register_graph;				// Used to register filter graph in ROT for graphedit
#endif
#if AAA_USE_GRABBER()
			// These have to be nullified when we destroy parts of the graph.
			CComPtr<ISampleGrabber>			_sample_grabber;
			// IBaseFilter*					p_video_renderer_;
			CComPtr<IBaseFilter>			_p_sample_grabber;
#else
			c_aaa_ds_renderer_filter*		_p_aaa_filter;
#endif
			// These are needed by the GraphBuilder
			CComPtr<IGraphBuilder>			_p_graph_builder;	// GraphBuilder
			CComPtr<ICaptureGraphBuilder2>	_p_graph_builder_2;
			// Media Control, Seeking
			CComPtr<IMediaControl>			_p_media_control;	// Media Control
			CComPtr<IMediaSeeking>			_p_media_seeking;	// Media Seeking
			CComPtr<IMediaEvent>			_p_media_event;		// Media Event
			CComPtr<IMediaPosition>			_p_media_position;	// Media Seeking

			// Used for WMV files
			CComPtr<IFileSourceFilter>		_p_wma_reader;		// interface to the WM ASF Reader

			CComPtr<IBaseFilter>			_p_video_source;
			// These are needed by the Source Filter
			CComPtr<IBaseFilter>			_p_source_filter;	// Source Filter
			CComPtr<IPin>					_p_source_pin_out_0;	// #0 Source Filter Output Pin (force to be audio) 
			CComPtr<IPin>					_p_source_pin_out_1;	// #1 Source Filter Output Pin (force to be video) 

			CComPtr<IBaseFilter>			_p_decoder_filter;	// Decoder Filter
			CComPtr<IBaseFilter>			_p_null_renderer;	// Video Renderer
			CComPtr<IPin>					_p_dec_pin_out;		// Decoder Filter Output Pin
			CComPtr<IBaseFilter>			_p_ds_device;		// DirectSound Device
			CComPtr<IBasicAudio>			_p_basic_audio;		// Audio control;

private:
			void	check_media_control();
			void	check_media_seeking();
			void	check_media_event();

			bool	get_media_type();
			void	get_info();
			void	enum_filters();

			void	destroy_graph();
//			void	destroy_sub_graph( CComPtr<IGraphBuilder> pGraph, CComPtr<IBaseFilter> pFilt );

			bool	open_wmv(			C_PCHAR_C filename );
			bool	open_mpg(			C_PCHAR_C filename );
			bool	open_avi(			C_PCHAR_C filename );
			bool	open_renderfile(	C_PCHAR_C filename );
			bool	open_ffdshow_lav(	C_PCHAR_C filename, bool b_lav );

			bool	set_callback( bool b_set );
			void	set_position_low( REAL time );


protected:
	AAA_ERR	open_low();

public:
			c_movie_ds( c_image_flux_buffer* buf );
			~c_movie_ds();

//	static INT32	lib_open_count;
		//	void	lib_open();
			//void	lib_close();
	virtual	aaa::MOVIE_LIB	get_type()		{	return aaa::MOVIE_LIB::MS_DS;	}

	virtual	AAA_ERR	open_specific();
	virtual	void	close_specific();

	virtual	void	play();
	virtual	void	pause();
	virtual	void	stop();
	virtual	void	update();
//	virtual	void	start();

//	virtual	INT32	get_bit_depth()	{ return 4; }		//todo why 4, should it be 32

	virtual	INT32	get_frame_index_cur();

	virtual DOUBLE	set_rate( DOUBLE rate );

	virtual	void	set_position( REAL CONST time );
	virtual	REAL	get_position();
			REAL	get_volume();
	virtual	bool	set_volume( REAL CONST volume );
			REAL	get_pan();
	virtual	bool	set_pan( REAL CONST pan ); 
};
