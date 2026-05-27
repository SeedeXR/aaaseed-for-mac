

#ifdef AAA_IMG_COMPO_H
#error "IMG_COMPO_H included more than once."
#endif
#define AAA_IMG_COMPO_H 1


#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif


namespace aaa
{
	namespace img
	{
		CONSTEXPR	REAL	REAL_NEARLY_256 = REAL(255.9999999);
		CONSTEXPR	DOUBLE	DOUBLE_NEARLY_256_x_256		= 256. * 256 - .0000001	;
		CONSTEXPR	DOUBLE	DOUBLE_256_x_256_MINUS_1	= 256. * 256 - 1.		;
		class c_compo
		{
		private:
			static	FP32	sta_uint8_to_fp32  [256];
			static	FP16	sta_uint8_to_fp16  [256];
			static	UINT16	sta_uint8_to_uint16[256];

			static	FP32	sta_uint16_to_fp32 [256*256];
			static	FP16	sta_uint16_to_fp16 [256*256];
			static	UINT8	sta_uint16_to_uint8[256*256];

		public:
			static void c_init();

// general conversion
//
			template< typename TS, typename TD >	static FINLINE TD convert( TS CONST val ) { return TD(val); }

//
// to FP32
//
			template<>	static FINLINE FP32   convert<UINT8, FP32  >( UINT8  CONST val )	{ return sta_uint8_to_fp32[val];	}
			template<>	static FINLINE FP32   convert<UINT16,FP32  >( UINT16 CONST val )	{ return sta_uint16_to_fp32[val];	}
			template<>	static FINLINE FP32   convert<FP16,  FP32  >( FP16   CONST val )	
#if HALF_FLOAT_USE()==0
				//return half_float::detail::half2float<FP32>( val );
				{ return (FP32)val;	}
#elif HALF_FLOAT_USE()==1
				{ return val;	}
#endif	
			template<typename TS>	static FINLINE FP32 to_fp32( TS CONST val )				{ return convert< TS, FP32>( val );	}

//
// to FP16
//
			template<>	static FINLINE FP16   convert<UINT8, FP16  >( UINT8  CONST val )	{ return sta_uint8_to_fp16[val];	}
			template<>	static FINLINE FP16   convert<UINT16,FP16  >( UINT16 CONST val )	{ return sta_uint16_to_fp16[val];	}
			template<>	static FINLINE FP16   convert<FP32,  FP16  >( FP32   CONST val )
#if HALF_FLOAT_USE()==0
				//return half_float::detail::float2half< (std::numeric_limits<half_float::half>::round_style) >( val );
				{ return (FP16)val;	}
#elif HALF_FLOAT_USE()==1
				{ return half(val); }
#endif
			template<typename TS>	static FINLINE FP16 to_fp16( TS CONST val )				{ return convert< TS, FP16>( val );	}

//
// to UINT16
//
			template<>	static FINLINE UINT16 convert<FP32  ,UINT16>( FP32   CONST val )	{ return (UINT16)CLAMP( val * DOUBLE_NEARLY_256_x_256, DOUBLE(0), DOUBLE_256_x_256_MINUS_1 ); }
			template<>	static FINLINE UINT16 convert<FP16  ,UINT16>( FP16   CONST val )	{ return convert<FP32,UINT16>(to_fp32(val));	}
			template<>	static FINLINE UINT16 convert<UINT8 ,UINT16>( UINT8  CONST val )	{ return sta_uint8_to_uint16[val];	}

			template<typename TS>	static FINLINE UINT16 to_uint16( TS CONST val )			{ return convert< TS, UINT16>( val );	}
//
// to UINT8
// 		
			template<>	static FINLINE UINT8  convert<FP32  ,UINT8 >( FP32   CONST val )	{ return (UINT8)CLAMP( val * REAL_NEARLY_256, 0.f, 255.f );	}
			template<>	static FINLINE UINT8  convert<FP16  ,UINT8 >( FP16   CONST val )	{ return convert<FP32,UINT8>(to_fp32(val));	}
			template<>	static FINLINE UINT8  convert<UINT16,UINT8 >( UINT16 CONST val )	{ return sta_uint16_to_uint8[val];	}

			template<typename TS>	static FINLINE UINT8 to_uint8( TS CONST val )			{ return convert< TS, UINT8>( val );	}


			////{ return (UINT8)CLAMP( half_float::detail::half2float<FP32>( val ) * REAL_NEARLY_256, 0.f, 255.f );	


//
// substration of component
// 

			static FINLINE FP32	sub_to_fp32	(	UINT8   CONST a,	UINT8   CONST b )	{ 	return (FP32)	CLAMP( a - b, 0, INT32(std::numeric_limits<UINT8 >::max()) );		}
			static FINLINE FP32	sub_to_fp32	(	UINT16  CONST a,	UINT16  CONST b )	{ 	return (FP32)	CLAMP( a - b, 0, INT32(std::numeric_limits<UINT16>::max()) );		}
			static FINLINE FP32	sub_to_fp32	(	FP16	CONST a,	FP16	CONST b )	{ 	return (FP32)	(a-b);	}
			static FINLINE FP32	sub_to_fp32	(	FP32	CONST a,	FP32	CONST b )	{ 	return			a-b;	}	

		};	//class c_compo
	}	//namespace img
}	//namespace aaa

