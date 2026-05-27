{
	int maskPitch = mask->_bytePitch;

//	PVideoFrame srcA;
//	PVideoFrame srcB;

	int srcAPitch;
	int srcBPitch;

	CONST unsigned char* srcAPtr;
	CONST unsigned char* srcBPtr;

	int rowSize;

	unsigned char* maskPtr;

//	srcA = src_n_1; //GetField(n  ,env);
//	srcB = src_n_3; //GetField(n-2,env);

	srcAPitch = _size_x * 4; //srcA->GetPitch();
	srcBPitch = _size_x * 4; //srcB->GetPitch();

	srcAPtr = _src_n_1; //srcA->GetReadPtr();
	srcBPtr = _src_n_3; //srcB->GetReadPtr();

	rowSize = _size_x * 4; //srcA->GetRowSize();

	bool oddPitch = ( srcAPitch % PITCH_MOD ) != 0;

	maskPtr = mask->buffer;

 	int bytesLeft;
 	unsigned short doubleThreshold = ( unsigned short )( 2 * threshold );

	CONST unsigned char* curSrcAPtr;
	CONST unsigned char* curSrcBPtr;

//	int size_y = srcA->GetHeight();
	int rowsLeft = _size_y;

	if( oddPitch )
		rowsLeft--;

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

		int safePitch = ( _scratchPitch >= srcAPitch ? srcAPitch : _scratchPitch );

		MEMCPY( _scratch, srcAPtr, safePitch );
		MEMCPY( _scratch + _scratchPitch, srcBPtr, safePitch );

		// change pointers to point to scratch buffer

		srcAPtr = _scratch;
		srcBPtr = srcAPtr+_scratchPitch;

		srcAPitch = srcBPitch = _scratchPitch * 2;

		#define PREFIX1(x) P1a_##x

		#define PART 1

		#include MOTIONMASK_STAGE1
		}

	//srcA=GetField( n+1, env);
	//srcB=GetField( n-1, env);

//	srcAPtr = srcA->GetReadPtr() - ( 1 - order )* srcAPitch;
//	srcBPtr = srcB->GetReadPtr() - ( 1 - order ) * srcBPitch;;
	srcAPtr = _src_n - ( 1 - order )* srcAPitch;
	srcBPtr = _src_n_2 - ( 1 - order ) * srcBPitch;;

	maskPtr = mask->buffer;

	#define PREFIX1(x) P2_##x

	#define PART 2

	#include MOTIONMASK_STAGE1

	rowsLeft = _size_y - 2;

	if( oddPitch )
		rowsLeft--;

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

		int safePitch = ( _scratchPitch >= srcAPitch ? srcAPitch : _scratchPitch );

		unsigned char* scratchPtr = _scratch;

		for( rowsLeft = 2; rowsLeft > 0; rowsLeft-- )
			{
			MEMCPY( scratchPtr, srcAPtr, safePitch );
			scratchPtr += _scratchPitch;

			MEMCPY( scratchPtr, srcBPtr, safePitch );
			scratchPtr += _scratchPitch;

			srcAPtr += srcAPitch; srcBPtr += srcBPitch;
			}

		// change pointers to point to scratch buffer

		srcAPtr = _scratch + 2 * _scratchPitch;
		srcBPtr = srcAPtr + _scratchPitch;

		srcAPitch = srcBPitch = _scratchPitch * 2;

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