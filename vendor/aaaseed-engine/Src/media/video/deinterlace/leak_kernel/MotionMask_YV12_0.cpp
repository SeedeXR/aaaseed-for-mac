{
	bool halfsize=(plane != PLANAR_Y) && linked;
	int maskPitch=mask->pitch;
	if (halfsize) maskPitch/=2;

	PVideoFrame srcA;
	PVideoFrame srcB;

	int srcAPitch;
	int srcBPitch;

	CONST unsigned char* srcAPtr;
	CONST unsigned char* srcBPtr;

	int rowSize;

	unsigned char* maskPtr;

	srcA = GetField( n+1, env );
	srcB = GetField( n-1, env );

	srcAPitch = srcA->GetPitch( plane );
	srcBPitch = srcB->GetPitch( plane );

	srcAPtr = srcA->GetReadPtr( plane );
	srcBPtr = srcB->GetReadPtr( plane );

	rowSize = srcA->GetRowSize( plane );

	bool oddPitch = ( srcAPitch % PITCH_MOD ) != 0;

	maskPtr = mask->buffer;

 	int bytesLeft;
 	unsigned short doubleThreshold = (unsigned short)( 2 * threshold );

	CONST unsigned char* curSrcAPtr;
	CONST unsigned char* curSrcBPtr;

	int height = srcA->GetHeight( plane );
	int rowsLeft = height;

	if( oddPitch ) rowsLeft--;

	while( rowsLeft > 0 )
		{
		#define PREFIX1(x) P1_##x

		#define PART 1

		#include MOTIONMASK_STAGE1

		rowsLeft--;
		}

	if( oddPitch )
		{
		// copy bottom parts of fields to scratch buffer

		int safePitch = ( scratchPitch >= srcAPitch ? srcAPitch : scratchPitch );

		unsigned char* scratchPtr = scratch;

		MEMCPY( scratch, srcAPtr, safePitch );
		MEMCPY( scratch + scratchPitch, srcBPtr, safePitch );

		// change pointers to point to scratch buffer

		srcAPtr = scratch;
		srcBPtr = srcAPtr + scratchPitch;

		srcAPitch = srcBPitch = scratchPitch * 2;

		#define PREFIX1(x) P1a_##x

		#define PART 1

		#include MOTIONMASK_STAGE1
	}

	overwrite = false;

	srcA = GetField( n  , env );
	srcB = GetField( n-2, env );

	srcAPtr = srcA->GetReadPtr( plane ) - ( 1 - order ) * srcAPitch;
	srcBPtr = srcB->GetReadPtr( plane ) - ( 1 - order ) * srcBPitch;;

	maskPtr = mask->buffer;

	#define PREFIX1(x) P2_##x

	#define PART 2

	#include MOTIONMASK_STAGE1

	rowsLeft = height - 2;

	if( oddPitch ) rowsLeft--;

	while( rowsLeft > 0 )
		{
		#define PREFIX1(x) P3_##x

		#define PART 3

		#include MOTIONMASK_STAGE1

		rowsLeft--;
		}

	if( oddPitch )
		{
		// copy bottom parts of fields to scratch buffer

		int safePitch = ( scratchPitch >= srcAPitch ? srcAPitch : scratchPitch );

		unsigned char* scratchPtr = scratch;

		for( rowsLeft = 2; rowsLeft > 0; rowsLeft-- )
			{
			MEMCPY( scratchPtr, srcAPtr, safePitch );
			scratchPtr += scratchPitch;

			MEMCPY( scratchPtr, srcBPtr, safePitch);
			scratchPtr += scratchPitch;

			srcAPtr += srcAPitch; srcBPtr += srcBPitch;
			}

		// change pointers to point to scratch buffer

		srcAPtr = scratch;
		srcBPtr = srcAPtr + scratchPitch;

		srcAPitch = srcBPitch = scratchPitch * 2;

		#define PREFIX1(x) P3a_##x

		#define PART 3

		#include MOTIONMASK_STAGE1
	}

	#define PREFIX1(x) P4_##x

	#define PART 4

	#include MOTIONMASK_STAGE1

	__asm emms

	#undef COLORSPACE
	#undef PITCH_MOD
	#undef MOTIONMASK_STAGE1
	#undef MOTIONMASK_STAGE2
}
