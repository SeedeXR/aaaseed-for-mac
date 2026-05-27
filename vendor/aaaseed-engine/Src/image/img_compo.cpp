
#include "img_compo.h"

using namespace aaa;
using namespace img;

c_compo	img_utils; //here to force init

FP32	c_compo::sta_uint16_to_fp32[256*256];
FP16	c_compo::sta_uint16_to_fp16[256*256];
UINT8	c_compo::sta_uint16_to_uint8[256*256];

FP32	c_compo::sta_uint8_to_fp32[256];
FP16	c_compo::sta_uint8_to_fp16[256];
UINT16	c_compo::sta_uint8_to_uint16[256];

void c_compo::c_init()
{
	{
		// initialize lookup tables uint8 to fp32, fp16, uint16
		INT32 max_index = 256-1;
		DOUBLE inv_8bits = 1. / DOUBLE(max_index);
		for( auto i = 0; i <= max_index; ++i )
		{
			FP32 f = CLAMP_01( (FP32)( i * inv_8bits ) );
			sta_uint8_to_fp32[i] = f;
#if HALF_FLOAT_USE()==0
			sta_uint8_to_fp16[i] = half_float::half(f);
		//	sta_uint8_to_fp16[i] = half_float::detail::float2half<(std::numeric_limits<half_float::half>::round_style)>( f );
#elif HALF_FLOAT_USE()==1
			//todo check
			sta_uint8_to_fp16[i] = half( f );
#endif
			sta_uint8_to_uint16[i] = static_cast<UINT16>( (i * 65535) / 255 );
		}
	}

	{
		// initialize lookup tables uint16 to fp32, fp16, uint16
		INT32 max_index = 256*256-1;
		DOUBLE inv_16bits = 1. /  DOUBLE(max_index);
		for( auto i = 0; i <= max_index; ++i ) 
		{
			FP32 f = CLAMP_01( (FP32)( i * inv_16bits ) );
			sta_uint16_to_fp32[i] = f;
#if HALF_FLOAT_USE()==0
			sta_uint16_to_fp16[i] = half_float::half(f);
	//		sta_uint16_to_fp16[i] = half_float::detail::float2half<(std::numeric_limits<half_float::half>::round_style)>( f );
#elif HALF_FLOAT_USE()==1
			//todo check
			sta_uint16_to_fp16[i] = half( f );
#endif
			sta_uint16_to_uint8[i] = static_cast<UINT8>( (i * 255) / 65535 );
		}
		//todo we should do ut withfp16 too
	}
}

