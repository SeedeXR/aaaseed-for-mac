{
	bool halfsize=(plane != PLANAR_Y);

//	PVideoFrame prev=GetField(n-1,env);
//	PVideoFrame src =GetField(n  ,env);
//	PVideoFrame next=GetField(n+1,env);

//	DBG_PRINT_STRING("height = %d",height);
	int prevPitch= _size_x * 4; //prev->GetPitch(plane);
	int srcPitch= _size_x * 4; //src->GetPitch(plane);
	int nextPitch= _size_x * 4; //next->GetPitch(plane);

	int maskPitch = mask->_bytePitch;

	int dstPitch = _size_x * 4; //dst->GetPitch(plane);

	int rowSize = _size_x * 4 ; //src->GetRowSize(plane);

	bool oddPitch=(srcPitch % PITCH_MOD) != 0;

	CONST unsigned char* prevPtr = _src_n_2; //prev->GetReadPtr(plane);
	CONST unsigned char* srcPtr = _src_n_1; //src->GetReadPtr(plane);
	CONST unsigned char* nextPtr = _src_n; //next->GetReadPtr(plane);

	unsigned char* maskPtr = mask->buffer;

	unsigned char* dstPtr= dst; //dst->GetWritePtr(plane);

	int bytesLeft;

	CONST unsigned char* curPrevPtr = NULL;
	CONST unsigned char* curSrcPtr = NULL;
	CONST unsigned char* curNextPtr = NULL;

	unsigned char* curMaskPtr = NULL;

	unsigned char* curDstPtr = NULL;

	if (order == 1)
		#define PREFIX1(x) C1_##x

		#include COPY_LINE

	#define PREFIX1(x) DI1_##x

	#define U2 2
	#define U1 1
	#define D1 1
	#define D2 2
	
	#define SU2 2
	#define SU1 1
	#define SD1 1
	#define SD2 2

	#include DEINTERLACE_STAGE1

	#define PREFIX1(x) C2_##x

	#include COPY_LINE

	#define PREFIX1(x) DI2_##x

	#define U2 1
	#define U1 0
	#define D1 1
	#define D2 2

	#define SU2 1
	#define SU1 0
	#define SD1 1
	#define SD2 2

	#include DEINTERLACE_STAGE1

	#define PREFIX1(x) C3_##x

	#include COPY_LINE

	#define PREFIX1(x) DI3_##x

	#define U2 0
	#define U1 0
	#define D1 1
	#define D2 2

	#define SU2 -1
	#define SU1 0
	#define SD1 1
	#define SD2 2

	#include DEINTERLACE_STAGE1

	#define PREFIX1(x) C4_##x

	#include COPY_LINE

	#define PREFIX1(x) DI4_##x

	#define U2 -1
	#define U1 -1
	#define D1 1
	#define D2 2

	#define SU2 -1
	#define SU1 0
	#define SD1 1
	#define SD2 2

	#include DEINTERLACE_STAGE1

//	unsigned long height = 2 * height; //src->GetHeight(plane);
	int rowsLeft = _size_y - 8;

	if( oddPitch ) rowsLeft--;

	while( rowsLeft > 0 )
	{
		#define PREFIX1(x) C5_##x

		#include COPY_LINE

		#define PREFIX1(x) DI5_##x

		#define U2 -2
		#define U1 -1
		#define D1 1
		#define D2 2

		#define SU2 -1
		#define SU1 0
		#define SD1 1
		#define SD2 2

		#include DEINTERLACE_STAGE1

		rowsLeft--;
	}

	if( oddPitch )
	{
		// copy bottom parts of fields to scratch buffer

		int safePitch = ( _scratchPitch >= srcPitch ? srcPitch : _scratchPitch );

		prevPtr -= 2 * prevPitch;
		srcPtr -= 2 * srcPitch;
		nextPtr -= 2 * nextPitch;

		unsigned char* scratchPtr = _scratch;

		for( rowsLeft = 7; rowsLeft > 0; rowsLeft-- )
			{
			MEMCPY( scratchPtr, prevPtr, safePitch );
			scratchPtr += _scratchPitch;

			MEMCPY( scratchPtr , srcPtr, safePitch );
			scratchPtr += _scratchPitch;

			MEMCPY( scratchPtr, nextPtr, safePitch );
			scratchPtr += _scratchPitch;

			prevPtr += prevPitch; srcPtr += srcPitch; nextPtr += nextPitch;
		}

		if( order == 0 )
			MEMCPY( scratchPtr + _scratchPitch, srcPtr, safePitch);

		// change pointers to point to scratch buffer

		prevPtr = _scratch + 6 * _scratchPitch;
		srcPtr = prevPtr + _scratchPitch;
		nextPtr = srcPtr + _scratchPitch;

		prevPitch = srcPitch = nextPitch = _scratchPitch * 3;

		#define PREFIX1(x) C5a_##x

		#include COPY_LINE

		#define PREFIX1(x) DI5a_##x

		#define U2 -2
		#define U1 -1
		#define D1 1
		#define D2 2

		#define SU2 -1
		#define SU1 0
		#define SD1 1
		#define SD2 2

		#include DEINTERLACE_STAGE1
	}

	#define PREFIX1(x) C6_##x

	#include COPY_LINE

	#define PREFIX1(x) DI6_##x

	#define U2 -2
	#define U1 -1
	#define D1 1
	#define D2 1

	#define SU2 -1
	#define SU1 0
	#define SD1 1
	#define SD2 2

	#include DEINTERLACE_STAGE1

	#define PREFIX1(x) C7_##x

	#include COPY_LINE

	#define PREFIX1(x) DI7_##x

	#define U2 -2
	#define U1 -1
	#define D1 1
	#define D2 1

	#define SU2 -1
	#define SU1 0
	#define SD1 1
	#define SD2 1

	#include DEINTERLACE_STAGE1

	#define PREFIX1(x) C8_##x

	#include COPY_LINE

	#define PREFIX1(x) DI8_##x

	#define U2 -2
	#define U1 -1
	#define D1 0
	#define D2 -1

	#define SU2 -1
	#define SU1 0
	#define SD1 1
	#define SD2 0

	#include DEINTERLACE_STAGE1

	#define PREFIX1(x) C9_##x

	#include COPY_LINE

	#define PREFIX1(x) DI9_##x

	#define U2 -2
	#define U1 -1
	#define D1 -1
	#define D2 -2

	#define SU2 -1
	#define SU1 0
	#define SD1 0
	#define SD2 -1

	#include DEINTERLACE_STAGE1

	if( order == 0 )
 		#define PREFIX1(x) C10_##x

		#include COPY_LINE

	#undef COLORSPACE
	#undef PITCH_MOD
	#undef DEINTERLACE_STAGE1
	#undef DEINTERLACE_STAGE2
	#undef COPY_LINE
}