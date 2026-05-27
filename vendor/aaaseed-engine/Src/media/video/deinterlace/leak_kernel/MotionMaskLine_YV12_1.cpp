{
	bytesLeft=rowSize;
	curSrcAPtr=srcAPtr;
	curSrcBPtr=srcBPtr;

	if (halfsize)
	{
		#define HALFSIZE

		unsigned short* curMaskPtr=(unsigned short*)maskPtr;

		#if PART == 1

			if      ( overwrite && (order == 1))
			{
				#define PREFIX2(x) PREFIX1(P1_##x)

				#define OVERWRITE
				#define TOPFIRST

				#include MOTIONMASK_STAGE2
			}
			else if ( overwrite && (order != 1))
			{
				#define PREFIX2(x) PREFIX1(P2_##x)

				#define OVERWRITE
				#undef  TOPFIRST

				#include MOTIONMASK_STAGE2
			}
			else

		#endif

		if (!overwrite && (order == 1))
		{
			#define PREFIX2(x) PREFIX1(P3_##x)

			#undef  OVERWRITE
			#define TOPFIRST

			#include MOTIONMASK_STAGE2
		}
		else if (!overwrite && (order != 1))
		{
			#define PREFIX2(x) PREFIX1(P4_##x)

			#undef  OVERWRITE
			#undef  TOPFIRST

			#include MOTIONMASK_STAGE2
		}

		maskPtr+=2*2*maskPitch;
	}
	else
	{
		#undef HALFSIZE

		unsigned char* curMaskPtr=maskPtr;

		#if PART == 1

			if      ( overwrite && (order == 1))
			{
				#define PREFIX2(x) PREFIX1(P5_##x)

				#define OVERWRITE
				#define TOPFIRST

				#include MOTIONMASK_STAGE2
			}
			else if ( overwrite && (order != 1))
			{
				#define PREFIX2(x) PREFIX1(P6_##x)

				#define OVERWRITE
				#undef  TOPFIRST

				#include MOTIONMASK_STAGE2
			}
			else

		#endif

		if (!overwrite && (order == 1))
		{
			#define PREFIX2(x) PREFIX1(P7_##x)

			#undef  OVERWRITE
			#define TOPFIRST

			#include MOTIONMASK_STAGE2
		}
		else if (!overwrite && (order != 1))
		{
			#define PREFIX2(x) PREFIX1(P8_##x)

			#undef  OVERWRITE
			#undef  TOPFIRST

			#include MOTIONMASK_STAGE2
		}

		maskPtr+=maskPitch;
	}

	srcAPtr+=srcAPitch;
	srcBPtr+=srcBPitch;

	#undef PART
	#undef HALFSIZE
	#undef OVERWRITE
	#undef TOPFIRST

	#undef PREFIX1
}