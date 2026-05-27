#include "ds_renderer_filter.h"
#include "../capture/capture_dshow.h"
#include "ds_util.h"

// ----------------------------------------------------------------------------
// Factory
// ----------------------------------------------------------------------------
CUnknown* WINAPI c_aaa_ds_renderer_filter::CreateInstance( LPUNKNOWN pUnk, HRESULT* phr, c_capture_dshow* p_parent )
{
    return new c_aaa_ds_renderer_filter( pUnk, phr, p_parent );
}

// ----------------------------------------------------------------------------
// Constructor
// ----------------------------------------------------------------------------
c_aaa_ds_renderer_filter::c_aaa_ds_renderer_filter( LPUNKNOWN pUnk, HRESULT* phr, c_capture_dshow* p_parent )
//#if AAA_DEBUG()	// somnething was weird: like a difference between include and lib
    : CBaseRenderer( CLSID_AAASeed_RendererFilter, L"AAASeed_renderer_filter", pUnk, phr )
//#else
//   : CBaseRenderer( CLSID_AAASeed_RendererFilter, "AAASeed_renderer_filter", pUnk, phr )
//#endif
//	,start_last {-42}
{
    set_parent( p_parent );
    if( phr )
        *phr = S_OK;
}

void c_aaa_ds_renderer_filter::set_parent( c_capture_dshow* p_parent )
{
    _p_parent = p_parent;
}

// ----------------------------------------------------------------------------
// Accept formats (NO conversion here)
// ----------------------------------------------------------------------------
HRESULT c_aaa_ds_renderer_filter::CheckMediaType( const CMediaType* pmt )
{
    if( !pmt || pmt->majortype != MEDIATYPE_Video )
        return VFW_E_TYPE_NOT_ACCEPTED;

    const GUID& st = pmt->subtype;

#if 1
	if (st == MEDIASUBTYPE_NV12 ||
        st == MEDIASUBTYPE_I420 ||
        st == MEDIASUBTYPE_YUY2 ||
        st == MEDIASUBTYPE_UYVY ||
        st == MEDIASUBTYPE_RGB24 ||
        st == MEDIASUBTYPE_RGB32)
#endif
	{
        return S_OK;
    }

    return VFW_E_TYPE_NOT_ACCEPTED;
}

// ----------------------------------------------------------------------------
// Frame access (ZERO COPY, terminal)
// ----------------------------------------------------------------------------
HRESULT c_aaa_ds_renderer_filter::DoRenderSample( IMediaSample* pSample )
{
	BYTE* p_data = nullptr;
	pSample->GetPointer( &p_data );

	const long size = pSample->GetActualDataLength();

	//REFERENCE_TIME start, stop;
	//if (SUCCEEDED(pSample->GetTime(&start, &stop)))
	//{
	//	// start is in 100-ns units
	//	if( start_last != start )
	//	{
	//		start_last = start;
			// No output pin → no copying → no stalls
			_p_parent->got_frame( p_data, "Capture Directshow", 0, true );
	//	}
	//}
    return S_OK;
}

STDMETHODIMP c_aaa_ds_renderer_filter::Receive( IMediaSample* pSample )
{
    if( !pSample )
        return E_POINTER;

    // Forward to your existing DoRenderSample
    return DoRenderSample( pSample );
}

#if 0
HRESULT c_aaa_ds_renderer_filter::ShouldDrawSample(IMediaSample* pSample)
{
    UNREFERENCED_PARAMETER(pSample);
    return S_OK; // ALWAYS accept
}

STDMETHODIMP c_aaa_ds_renderer_filter::GetState(DWORD dwMSecs, FILTER_STATE* pState)
{
    if (!pState)
        return E_POINTER;
    *pState = m_State; // m_State is the internal state (RUNNING, PAUSED, STOPPED)
    return S_OK;
}
#endif