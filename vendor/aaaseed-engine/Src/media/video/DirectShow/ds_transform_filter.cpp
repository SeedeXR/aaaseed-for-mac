#include "ds_transform_filter.h"
#include "../capture/capture_dshow.h"
#include "ds_util.h"
//#include "DXBaseClasses/Mtype.h"

// ----------------------------------------------------------------------------
// Factory
// ----------------------------------------------------------------------------
CUnknown* WINAPI c_aaa_ds_transform_filter::CreateInstance(LPUNKNOWN pUnk, HRESULT* phr, c_capture_dshow* p_parent)
{
    return new c_aaa_ds_transform_filter(pUnk, phr, p_parent );
}

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
c_aaa_ds_transform_filter::c_aaa_ds_transform_filter( LPUNKNOWN pUnk, HRESULT* phr, c_capture_dshow* p_parent )
	// base class lib release is (2026 January) not compiled with unicode, L "AAASeed_transform_filter" will trigger a link error
#if AAA_USE_DS_TRANS_IN_PLACE()
	: CTransInPlaceFilter( "AAASeed_transform_filter", pUnk, CLSID_AAASeed_TransformFilter, phr, false )
#else
	: CTransformFilter( "AAASeed_transform_filter", pUnk, CLSID_AAASeed_TransformFilter)
#endif
{
	set_parent( p_parent );
    if (phr)
        *phr = S_OK;
}

void c_aaa_ds_transform_filter::set_parent( c_capture_dshow* p_parent )
{
	_p_parent = p_parent;
}

// ----------------------------------------------------------------------------
// Accept formats from OBS
// ----------------------------------------------------------------------------
HRESULT c_aaa_ds_transform_filter::CheckInputType(const CMediaType* mtIn)
{
    if( !mtIn || mtIn->majortype != MEDIATYPE_Video)
        return VFW_E_TYPE_NOT_ACCEPTED;

    const GUID& st = mtIn->subtype;

    if (st == MEDIASUBTYPE_NV12 ||
        st == MEDIASUBTYPE_I420 ||
        st == MEDIASUBTYPE_YUY2 ||
        st == MEDIASUBTYPE_RGB24 ||
        st == MEDIASUBTYPE_RGB32)
    {
        return S_OK;
    }

    // Reject compressed formats so DShow inserts decoders
    return VFW_E_TYPE_NOT_ACCEPTED;
}


// ----------------------------------------------------------------------------
// Frame access (ZERO COPY)
// ----------------------------------------------------------------------------
HRESULT c_aaa_ds_transform_filter::Transform(IMediaSample* pIn
#if !AAA_USE_DS_TRANS_IN_PLACE()
	,IMediaSample* pOut 
#endif
)
{
    BYTE* p_data_in = nullptr;
    pIn->GetPointer(&p_data_in);

    _p_parent->got_frame(p_data_in, "Capture Directshow", 0, true);

    // Get output pointer
//   BYTE* p_data_out = nullptr;
//	pOut->GetPointer(&p_data_out);
//	const long size = pIn->GetActualDataLength();
//
//    if (p_data_out != p_data_in)
//    {
//        // Fallback if the allocator gave a different buffer
//       
////        memcpy(p_data_out, p_data_in, size);
//    }

    //// Pass timestamps downstream
    //REFERENCE_TIME start, end;
    //if (SUCCEEDED(pIn->GetTime(&start, &end)))
    //    pOut->SetTime(&start, &end);
    //else
	pOut->SetTime(nullptr, nullptr);	// Without this OBS Stall 

    // Set actual data length
	//   pOut->SetActualDataLength(size);

    return S_OK;
}

#if AAA_USE_DS_TRANS_IN_PLACE()
#	if 0
//HRESULT c_aaa_ds_transform_filter::DecideAllocator(  IMemAllocator* pAllocator, ALLOCATOR_PROPERTIES* pProps)
//{
//    // Accept OBS allocator as-is
//    return S_OK;
//}
HRESULT c_aaa_ds_transform_filter::SetMediaType(PIN_DIRECTION dir,const CMediaType* pmt)
{
    return CTransInPlaceFilter::SetMediaType(dir, pmt);
}
HRESULT c_aaa_ds_transform_filter::CheckTransform( const CMediaType* mtIn, const CMediaType* mtOut )
{
    //if (!mtIn || !mtOut)
    //    return E_POINTER;

    //// Same major type
    //if (mtIn->majortype != mtOut->majortype)
    //    return VFW_E_TYPE_NOT_ACCEPTED;

    // SAME format in = out (passthrough)
    //if (*mtIn != *mtOut)
    //    return VFW_E_TYPE_NOT_ACCEPTED;

    return S_OK;
}
#	endif

#else	//#if AAA_USE_DS_TRANS_IN_PLACE()
// ----------------------------------------------------------------------------
// Output == Input (pass-through)
// ----------------------------------------------------------------------------
HRESULT c_aaa_ds_transform_filter::GetMediaType(int iPosition, CMediaType* pmt)
{
    if (iPosition > 0)
        return VFW_S_NO_MORE_ITEMS;

    *pmt = m_pInput->CurrentMediaType();
    return S_OK;
}

// ----------------------------------------------------------------------------
// Buffer negotiation (OBS-safe)
// ----------------------------------------------------------------------------
#if 0
HRESULT c_aaa_ds_transform_filter::DecideBufferSize( IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProp)
{
    CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pProp, E_POINTER);

    if (!m_pInput->IsConnected())
        return E_UNEXPECTED;

    pProp->cBuffers = 2;
    pProp->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();
    pProp->cbAlign  = 1;
    pProp->cbPrefix = 0;

    ALLOCATOR_PROPERTIES actual;
    return pAlloc->SetProperties(pProp, &actual);
}
#else
// version with one buffer more compact
HRESULT c_aaa_ds_transform_filter::DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProp)
{
    CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pProp, E_POINTER);

    if (!m_pInput->IsConnected())
        return E_UNEXPECTED;

    // Ask upstream input for the sample size
    long sampleSize = m_pInput->CurrentMediaType().GetSampleSize();

    // If pProp->cbBuffer is less than input sample size, increase it
    if (pProp->cbBuffer < sampleSize)
        pProp->cbBuffer = sampleSize;

    // Only 1 buffer is enough for Null Renderer / zero-copy
    pProp->cBuffers = 1;
    pProp->cbAlign = 1;
    pProp->cbPrefix = 0;

    // Set allocator properties
    ALLOCATOR_PROPERTIES actual;
    HRESULT hr = pAlloc->SetProperties(pProp, &actual);
    if (FAILED(hr))
		return hr;

    return S_OK;
}
#endif


HRESULT c_aaa_ds_transform_filter::CheckTransform( const CMediaType* mtIn, const CMediaType* mtOut )
{
    if (!mtIn || !mtOut)
        return E_POINTER;

    // Same major type
    if (mtIn->majortype != mtOut->majortype)
        return VFW_E_TYPE_NOT_ACCEPTED;

    // SAME format in = out (passthrough)
    if (*mtIn != *mtOut)
        return VFW_E_TYPE_NOT_ACCEPTED;

    return S_OK;
}


#endif	//#if AAA_USE_DS_TRANS_IN_PLACE()

//HRESULT c_aaa_ds_transform_filter::StopStreaming()
//{
//    // signal thread to stop
//    _exit = true;
//
//    if (_thread)
//    {
//        WaitForSingleObject(_thread, INFINITE);
//        CloseHandle(_thread);
//        _thread = nullptr;
//    }
//
//    return CTransformFilter::StopStreaming();
//}

