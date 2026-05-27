#ifdef AAA_DS_RENDERER_FILTER_H
#error "DS_RENDERER_FILTER_H included more than once."
#endif
#define AAA_DS_RENDERER_FILTER_H 1


#ifndef AAA_AAA_TYPE_H
#   include "aaa_type.h"
#endif
#ifndef __STREAMS__
#   include <DXBaseClasses/streams.h>
#endif
#ifndef INITGUID
#   include <initguid.h>
#endif

// Forward declaration
class c_capture_dshow;

// {8E91F4B3-2C44-4A1E-9A73-6A91B9C4B222}
DEFINE_GUID(CLSID_AAASeed_RendererFilter,
0x8e91f4b3, 0x2c44, 0x4a1e, 0x9a, 0x73, 0x6a, 0x91, 0xb9, 0xc4, 0xb2, 0x22);

class c_aaa_ds_renderer_filter final : public CBaseRenderer
{
protected:
    c_capture_dshow* _p_parent = nullptr;
//	REFERENCE_TIME	start_last;

public:
    static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT* phr, c_capture_dshow* p_parent);

    c_aaa_ds_renderer_filter(LPUNKNOWN pUnk, HRESULT* phr, c_capture_dshow* p_parent);
    ~c_aaa_ds_renderer_filter() override = default;

    void set_parent(c_capture_dshow* p_parent);

protected:
    // --- CBaseRenderer overrides ---
    HRESULT CheckMediaType( const CMediaType* pmt ) override;
    HRESULT DoRenderSample( IMediaSample* pSample ) override;

	STDMETHODIMP Receive( IMediaSample* pSample ) override;
#if 0
	STDMETHODIMP GetState(DWORD dwMSecs, FILTER_STATE* pState)  override;
#endif
};
