{
	bytesLeft = rowSize;

	curPrevPtr = prevPtr;
	curSrcPtr = srcPtr;
	curNextPtr = nextPtr;
	curMaskPtr = maskPtr;
	curDstPtr = dstPtr;

	if( !halfsize )
		{
		#undef  HALFSIZE

		if( order == 0 )
			{
			#undef TOPFIRST

			#define ASU2 (SU2-1)
			#define ASU1 (SU1-1)
			#define ASD1 (SD1-1)
			#define ASD2 (SD2-1)

			if( !sharp && !twoway )
				{
				#define PREFIX2(x) PREFIX1(P1_##x)

				#undef  SHARP
				#undef  TWOWAY

				#include DEINTERLACE_STAGE2
				}
			else if( sharp && !twoway )
				{
				#define PREFIX2(x) PREFIX1(P2_##x)

				#define SHARP
				#undef  TWOWAY

				#include DEINTERLACE_STAGE2
				}
			else if( !sharp && twoway )
				{
				#define PREFIX2(x) PREFIX1(P3_##x)

				#undef  SHARP
				#define TWOWAY

				#include DEINTERLACE_STAGE2
				}
			else if( sharp && twoway )
				{
				#define PREFIX2(x) PREFIX1(P4_##x)

				#define SHARP
				#define TWOWAY

				#include DEINTERLACE_STAGE2
				}

			#undef ASU2
			#undef ASU1
			#undef ASD1
			#undef ASD2
			}
		else
			{
			#define TOPFIRST

			#define ASU2 SU2
			#define ASU1 SU1
			#define ASD1 SD1
			#define ASD2 SD2

			if( !sharp && !twoway )
				{
				#define PREFIX2(x) PREFIX1(P5_##x)

				#undef  SHARP
				#undef  TWOWAY

				#include DEINTERLACE_STAGE2
				}
			else if( sharp && !twoway)
				{
				#define PREFIX2(x) PREFIX1(P6_##x)

				#define SHARP
				#undef  TWOWAY

				#include DEINTERLACE_STAGE2
				}
			else if(!sharp &&  twoway)
				{
				#define PREFIX2(x) PREFIX1(P7_##x)

				#undef  SHARP
				#define TWOWAY

				#include DEINTERLACE_STAGE2
				}
			else if ( sharp &&  twoway)
				{
				#define PREFIX2(x) PREFIX1(P8_##x)

				#define SHARP
				#define TWOWAY

				#include DEINTERLACE_STAGE2
				}

			#undef ASU2
			#undef ASU1
			#undef ASD1
			#undef ASD2
			}
		}

	maskPtr += maskPitch;
	dstPtr += dstPitch;
	prevPtr += prevPitch;
	nextPtr += nextPitch;

	if( order == 1 )
		srcPtr += srcPitch;

	#undef HALFSIZE
	#undef TOPFIRST
	#undef SHARP
	#undef TWOWAY

	#undef U2
	#undef U1
	#undef D1
	#undef D2

	#undef SU2
	#undef SU1
	#undef SD1
	#undef SD2

	#undef PREFIX1
}