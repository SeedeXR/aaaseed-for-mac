{
	bytesLeft=rowSize;
	curSrcAPtr=srcAPtr;
	curSrcBPtr=srcBPtr;

	unsigned long* curMaskPtr=(unsigned long*)maskPtr;

	#if COLORSPACE == COLORSPACE_YUY2

		if (!linked)
		{
			if      (order == 1)
			{
				#define PREFIX2(x) PREFIX1(P1_##x)

				#undef  LINKED
				#define TOPFIRST

				#include MOTIONMASK_STAGE2
			}
			else if (order != 1)
			{
				#define PREFIX2(x) PREFIX1(P2_##x)

				#undef  LINKED
				#undef  TOPFIRST

				#include MOTIONMASK_STAGE2
			}
		}
		else
	
	#endif
	
	{
		if      (order == 1)
		{
			#define PREFIX2(x) PREFIX1(P3_##x)

			#define LINKED
			#define TOPFIRST

			#include MOTIONMASK_STAGE2
		}
		else if (order != 1)
		{
			#define PREFIX2(x) PREFIX1(P4_##x)

			#define LINKED
			#undef  TOPFIRST

			#include MOTIONMASK_STAGE2
		}
	}

	maskPtr+=maskPitch;
	srcAPtr+=srcAPitch;
	srcBPtr+=srcBPitch;

	#undef PART
	#undef LINKED
	#undef TOPFIRST

	#undef PREFIX1
}