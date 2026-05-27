#ifdef AAA_DS_TRANSFORM_FILTER_H
#error "DS_TRANSFORM_FILTER_H included more than once."
#endif
#define AAA_DS_TRANSFORM_FILTER_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef __STREAMS__
#	include <DXBaseClasses/streams.h>   // DirectShow BaseClasses
#endif
#ifndef INITGUID
#	include <initguid.h>
#endif

// Forward declaration
class c_capture_dshow;

// {D4E9F1E2-8B7F-4A2F-9C42-9F7B87C6A111}
DEFINE_GUID(CLSID_AAASeed_TransformFilter,
0xd4e9f1e2, 0x8b7f, 0x4a2f, 0x9c, 0x42, 0x9f, 0x7b, 0x87, 0xc6, 0xa1, 0x11);

#define AAA_USE_DS_TRANS_IN_PLACE() 0
class c_aaa_ds_transform_filter final : 
#if AAA_USE_DS_TRANS_IN_PLACE()
	public CTransInPlaceFilter
#else
	public CTransformFilter
#endif
{
protected:
	c_capture_dshow* _p_parent;
public:
    static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT* phr, c_capture_dshow* p_parent );

    c_aaa_ds_transform_filter( LPUNKNOWN pUnk, HRESULT* phr, c_capture_dshow* p_parent );
    ~c_aaa_ds_transform_filter() override = default;

	void set_parent( c_capture_dshow* p_parent );

    // --- CTransformFilter overrides ---
    HRESULT CheckInputType(		const CMediaType* mtIn) override;

#if AAA_USE_DS_TRANS_IN_PLACE()
    HRESULT Transform(			IMediaSample* pIn) override;
//	HRESULT SetMediaType(		PIN_DIRECTION dir,const CMediaType* pmt) override;
//	HRESULT CheckTransform(		const CMediaType* mtIn, const CMediaType* mtOut	) override;
protected:
//	HRESULT DecideAllocator(	IMemAllocator* pAllocator, ALLOCATOR_PROPERTIES* pProps);
public:
#else
	HRESULT CheckTransform(		const CMediaType* mtIn, const CMediaType* mtOut	) override;
	HRESULT Transform(			IMediaSample* pIn, IMediaSample* pOut) override;
    HRESULT GetMediaType(		int iPosition, CMediaType* pMediaType) override;
    HRESULT DecideBufferSize(	IMemAllocator* pAlloc,	ALLOCATOR_PROPERTIES* pProp) override;
#endif
//	HRESULT StopStreaming()	override;


};

