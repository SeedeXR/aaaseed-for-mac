

#ifdef AAA_DS_UTIL_H
#error "DS_UTIL_H included more than once."
#endif
#define AAA_DS_UTIL_H 1

#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef __ATLCOMCLI_H__
#	include <atlcomcli.h>
#endif
/*	was
#ifndef __ATLBASE_H__
#include <atlbase.h>
#endif
*/
#ifndef __DSHOW_INCLUDED__
#	include <dshow.h>
#endif
//#include "winnt.h"
/*
#ifndef GUID_DEFINED
#	include "guiddef.h"
#endif
*/
#ifndef __FOURCC__
#	include "DXBaseClasses/fourcc.h"
#endif
#ifndef AAA_PIXEL_FORMAT_H
#	include "image/pixel_format.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

//#ifndef DEFINE_MEDIATYPE_GUID
//#define DEFINE_MEDIATYPE_GUID(name, format) \
//	DEFINE_GUID(name,                       \
//	format, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
//#endif

//DEFINE_MEDIATYPE_GUID( MFVideoFormat_v210,		FCC('v210') );
//struct	__declspec(uuid("{30323449-0000-0010-8000-00AA00389B71}")) MFVideoFormat_v210;
extern	FOURCCMap	MEDIASUBTYPE_I420;
extern	FOURCCMap	MEDIASUBTYPE_IYU2;
extern	FOURCCMap	MEDIASUBTYPE_HDYC;
extern	FOURCCMap	MEDIASUBTYPE_v210;

static const GUID MEDIASUBTYPE_Y800 =
{ 0x30303859, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };
static const GUID MEDIASUBTYPE_Y8 =
{ 0x20203859, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };

struct ISampleGrabber;
struct ISampleGrabberCB;

namespace ds_helper
{
	struct st_media_info
	{
		aaa::PIXEL_FORMAT	pixel_format	= aaa::PIXEL_FORMAT::UNKNOWN;
		INT32				size_x			= 0;
		INT32				size_y			= 0;
		INT32				pitch			= 0;
		INT32				bits_by_pixel	= 0;
		FP32				fps				= FP32(0);
		C_PCHAR				str_format		= nullptr;
		CHAR				fourcc[5]		= {0,0,0,0,0};
		o_str				o_text;
	};

	const	LONGLONG	MILLISECONDS	= (1000);				// 10 ^ 3
	const	LONGLONG	NANOSECONDS		= (1000000000);			// 10 ^ 9
	const	LONGLONG	UNITS			= (NANOSECONDS / 100);	// 10 ^ 7

	extern	DOUBLE			round_double( DOUBLE value, INT32 precision );

	extern	DOUBLE			avg_2_fps( REFERENCE_TIME AvgTimePerFrame, INT32 precision );

	extern	HRESULT			add_to_running_object_table(	CComPtr<IGraphBuilder> p_graph, DWORD* p_register );
	extern	void			remove_from_object_table(		DWORD* p_register );

	#if 0
		extern	void			destroy_graph(	CComPtr<IGraphBuilder>& p_graph );
	#else
		extern	void			destroy_graph(	IGraphBuilder& p_graph );
	#endif
	extern	bool			set_callback(	ISampleGrabberCB* grabber_cb, ISampleGrabber* sample_grabber, bool b_set );

	extern	bool			pin_can_deliver_sub_type(	CComPtr<IPin> pin, CONST std::vector<GUID>& subtypes );
	extern	bool			pin_can_deliver_major_type( CComPtr<IPin> pin, CONST GUID& majortype );
	extern	bool			pin_can_deliver_mjpg(		CComPtr<IPin> pin );
	extern	bool			pin_can_deliver_dv(			CComPtr<IPin> pin );
	extern	bool			pin_can_deliver_video(		CComPtr<IPin> pin );

	extern	HRESULT			DXUtil_ConvertAnsiStringToWide( WCHAR* wstrDestination, const CHAR* strSource, int cchDestChar );

	extern	bool				is_same_video_info(		st_media_info CONST & a, st_media_info CONST & b );
	extern	void				add_media_info_base(	o_str & o, st_media_info CONST * info );
	extern	void				add_media_info(			o_str & o, st_media_info CONST * info );
	extern 	aaa::PIXEL_FORMAT	get_aaa_pixel_format(	AM_MEDIA_TYPE CONST * media_type );
	extern	bool				get_video_info(			AM_MEDIA_TYPE CONST * media_type, st_media_info& info );
	extern	void				get_media_info(			AM_MEDIA_TYPE CONST * media_type, st_media_info& info );

	extern	CComPtr<IPin>	get_pin(				CComPtr<IBaseFilter> p_filter,		PIN_DIRECTION pin_dir_asked );
	extern	HRESULT			get_pin(				CComPtr<IBaseFilter> flt,			PIN_DIRECTION pin_dir_asked, INT32 number, CComPtr<IPin> &p_pin_ret );
	extern	HRESULT			auto_connect_filters(	CComPtr<IBaseFilter> filter_out,	INT32 out_pin_nr, CComPtr<IBaseFilter> in_filter, INT32 in_pin_nr, CComPtr<IGraphBuilder> p_graph_builder );
	extern	HRESULT			connect_filters(		CComPtr<IBaseFilter> filter_out,	INT32 out_pin_nr, CComPtr<IBaseFilter> in_filter, INT32 in_pin_nr );

	extern	HRESULT			find_pin_interface(				CComPtr<IBaseFilter> p_filter,		CONST GUID* p_format,	PIN_DIRECTION pin_dir_asked, const IID& riid, void** ppvInterface );
	extern	HRESULT			find_pin_interface_for_stream(	CComPtr<IBaseFilter> p_filter,								PIN_DIRECTION pin_dir_asked, void** ppvInterface );
	extern	HRESULT			get_pin(				CComPtr<IBaseFilter> p_filter,		CONST GUID* p_format,	PIN_DIRECTION pin_dir_asked, CComPtr<IPin> &p_pin_ret );
//extern	HRESULT			FindPinInterface(		IBaseFilter* pFilter,			const GUID* pFormat,	PIN_DIRECTION pin_dir_asked, const IID& riid, void** ppvInterface );
//extern	HRESULT			GetPin(					IBaseFilter* pFilter,			const GUID* pFormat,	PIN_DIRECTION pin_dir_asked, IPin** ppPin );

	extern	void			print_guid(				CONST GUID& guid );
	extern	void			fourcc_as_string (		CONST GUID& subtype, CHAR str[5] );

	C_PCHAR					get_error_string(		CONST HRESULT hr );


}
