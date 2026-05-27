__asm
{
	#define SRC_PREV 1
	#define SRC_CUR  2
	#define SRC_NEXT 3

	add bytesLeft,7
	and bytesLeft,-8

	mov esi,curPrevPtr
	mov ecx,curSrcPtr
	mov edi,curNextPtr
	mov edx,curDstPtr

	mov eax,srcPitch

	align 16

PREFIX2(ColLoop):

	mov ebx,curMaskPtr
	movq mm7,[ebx]

	movq mm6,mm7
	psrlw mm6,1
	packuswb mm6,mm6

	add ebx,8
	mov curMaskPtr,ebx

	movd ebx,mm6
	or ebx,ebx
	jz PREFIX2(CopyThrough)

	pxor mm6,mm6

	#if !defined SHARP

		#if !defined TWOWAY

			; intVal =+ 8 * ( *(curSrcPtr + ASD1 * srcPitch)

			#define SOURCE SRC_CUR
			#define TARGET mm0
			#define DIRECTION ASD1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			;	         +*(curSrcPtr +ASU1*srcPitch))

			#define SOURCE SRC_CUR
			#define TARGET mm4
			#define DIRECTION ASU1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm5,mm4

			punpcklbw mm4,mm6
			punpckhbw mm5,mm6

			paddsw mm4,mm0
			paddsw mm5,mm1

			psllw mm4,3
			psllw mm5,3

			;	     +2*( *(curPrevPtr              ))

			#define SOURCE SRC_PREV
			#define TARGET mm0
			#define DIRECTION 0

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			psllw mm0,1
			psllw mm1,1

			paddsw mm4,mm0
			paddsw mm5,mm1

			; 	     -    *(curPrevPtr+U1*prevPitch )

			#define SOURCE SRC_PREV
			#define TARGET mm0
			#define DIRECTION U1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			psubsw mm4,mm0
			psubsw mm5,mm1

			; 	     -    *(curPrevPtr+D1*prevPitch );

			#define SOURCE SRC_PREV
			#define TARGET mm0
			#define DIRECTION D1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			psubsw mm4,mm0
			psubsw mm5,mm1

			#if defined EAX_INVERTED
				neg eax
			#endif

			#undef EAX_INVERTED

		#else ; defined TWOWAY

			; intVal=+8*( *(curSrcPtr +ASD1*srcPitch)

			#define SOURCE SRC_CUR
			#define TARGET mm0
			#define DIRECTION ASD1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			;		     +*(curSrcPtr +ASU1*srcPitch))

			#define SOURCE SRC_CUR
			#define TARGET mm4
			#define DIRECTION ASU1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm5,mm4

			punpcklbw mm4,mm6
			punpckhbw mm5,mm6

			paddsw mm4,mm0
			paddsw mm5,mm1

			psllw mm4,3
			psllw mm5,3

			;	     +2*( *(curNextPtr              )

			#define SOURCE SRC_NEXT
			#define TARGET mm0
			#define DIRECTION 0

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			;		     +*(curPrevPtr              ))

			#define SOURCE SRC_PREV
			#define TARGET mm2
			#define DIRECTION 0

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm6
			punpckhbw mm3,mm6

			paddsw mm2,mm0
			paddsw mm3,mm1

			psllw mm2,1
			psllw mm3,1

			paddsw mm4,mm2
			paddsw mm5,mm3

			;	     -    *(curNextPtr+U1*nextPitch )

			#define SOURCE SRC_NEXT
			#define TARGET mm0
			#define DIRECTION U1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			psubsw mm4,mm0
			psubsw mm5,mm1

			;	     -    *(curNextPtr+D1*nextPitch )

			#define SOURCE SRC_NEXT
			#define TARGET mm0
			#define DIRECTION D1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			psubsw mm4,mm0
			psubsw mm5,mm1

			;	     -    *(curPrevPtr+D1*prevPitch )

			#define SOURCE SRC_PREV
			#define TARGET mm0
			#define DIRECTION D1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			psubsw mm4,mm0
			psubsw mm5,mm1

			;	     -    *(curPrevPtr+U1*prevPitch );

			#define SOURCE SRC_PREV
			#define TARGET mm0
			#define DIRECTION U1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm6
			punpckhbw mm1,mm6

			psubsw mm4,mm0
			psubsw mm5,mm1

			#if defined EAX_INVERTED
				neg eax
			#endif

			#undef EAX_INVERTED

		#endif

		; dstVal/=16;

		psraw mm4,4
		psraw mm5,4

	#else ; defined SHARP

		#if !defined TWOWAY

			; lIntVal=+17236*( *(curSrcPtr +ASD1*srcPitch)

			#define SOURCE SRC_CUR
			#define TARGET mm0
			#define DIRECTION ASD1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0 ; Byte XX -> Word XXXX
			punpckhbw mm1,mm1 ; (more accurate than XX00)

			psrlw mm0,2
			psrlw mm1,2

			;		          +*(curSrcPtr +ASU1*srcPitch))

			#define SOURCE SRC_CUR
			#define TARGET mm4
			#define DIRECTION ASU1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm5,mm4

			punpcklbw mm4,mm4
			punpckhbw mm5,mm5

			psrlw mm4,2
			psrlw mm5,2

			paddsw mm4,mm0
			paddsw mm5,mm1

			;mov ebx,043544354h ; 32768*0.526
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_4354h

			pmulhw mm4,mm2
			pmulhw mm5,mm2

			;		  + 5570*( *(curPrevPtr              ))

			#define SOURCE SRC_PREV
			#define TARGET mm0
			#define DIRECTION 0

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0
			punpckhbw mm1,mm1

			psrlw mm0,2
			psrlw mm1,2

			;mov ebx,015c215c2h ; 32768*0.170
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_15c2h

			pmulhw mm0,mm2
			pmulhw mm1,mm2

			paddsw mm4,mm0
			paddsw mm5,mm1

			;		  - 3801*( *(curPrevPtr+U1*prevPitch )

			#define SOURCE SRC_PREV
			#define TARGET mm0
			#define DIRECTION U1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0
			punpckhbw mm1,mm1

			psrlw mm0,2
			psrlw mm1,2

			;				  +*(curPrevPtr+D1*prevPitch ))

			#define SOURCE SRC_PREV
			#define TARGET mm2
			#define DIRECTION D1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,2
			psrlw mm3,2
			
			paddsw mm0,mm2
			paddsw mm1,mm3

			;mov ebx,00ed90ed9h ; 32768*0.116
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_0ed9h

			pmulhw mm0,mm2
			pmulhw mm1,mm2

			psubsw mm4,mm0
			psubsw mm5,mm1

			;		  -  852*( *(curSrcPtr +ASD2*srcPitch)

			#define SOURCE SRC_CUR
			#define TARGET mm0
			#define DIRECTION ASD2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0
			punpckhbw mm1,mm1

			psrlw mm0,2
			psrlw mm1,2

			;				  +*(curSrcPtr +ASU2*srcPitch))

			#define SOURCE SRC_CUR
			#define TARGET mm2
			#define DIRECTION ASU2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,2
			psrlw mm3,2

			paddsw mm0,mm2
			paddsw mm1,mm3

			;mov ebx,003540354h ; 32768*0.026
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_0354h

			pmulhw mm0,mm2
			pmulhw mm1,mm2

			psubsw mm4,mm0
			psubsw mm5,mm1

			;		  + 1016*( *(curPrevPtr+U2*prevPitch )

			#define SOURCE SRC_PREV
			#define TARGET mm0
			#define DIRECTION U2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0
			punpckhbw mm1,mm1

			psrlw mm0,2
			psrlw mm1,2

			;				  +*(curPrevPtr+D2*prevPitch ));

			#define SOURCE SRC_PREV
			#define TARGET mm2
			#define DIRECTION D2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,2
			psrlw mm3,2

			paddsw mm0,mm2
			paddsw mm1,mm3

			;mov ebx,003f803f8h ; 32768*0.031
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_03f8h

			pmulhw mm0,mm2
			pmulhw mm1,mm2

			paddsw mm4,mm0
			paddsw mm5,mm1

			; Word XXYY -> Byte XX via XXYY-00XX
			; (result closer to C++ implementation)

			movq mm0,mm4
			movq mm1,mm5

			psraw mm0,8
			psraw mm1,8

			psubsw mm4,mm0
			psubsw mm5,mm1

			#if defined EAX_INVERTED
				neg eax
			#endif

			#undef EAX_INVERTED

			; intVal=lIntVal/16384;

			psraw mm4,5
			psraw mm5,5

		#else ; defined TWOWAY

			; lIntVal=+17236*( *(curSrcPtr +ASD1*srcPitch)

			#define SOURCE SRC_CUR
			#define TARGET mm0
			#define DIRECTION ASD1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0
			punpckhbw mm1,mm1

			psrlw mm0,3
			psrlw mm1,3

			;		          +*(curSrcPtr +ASU1*srcPitch))

			#define SOURCE SRC_CUR
			#define TARGET mm4
			#define DIRECTION ASU1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm5,mm4

			punpcklbw mm4,mm4
			punpckhbw mm5,mm5

			psrlw mm4,3
			psrlw mm5,3

			paddsw mm4,mm0
			paddsw mm5,mm1

			;mov ebx,043544354h
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_4354h

			pmulhw mm4,mm2
			pmulhw mm5,mm2

			;		  + 5570*( *(curNextPtr              )

			#define SOURCE SRC_NEXT
			#define TARGET mm0
			#define DIRECTION 0

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0
			punpckhbw mm1,mm1

			psrlw mm0,3
			psrlw mm1,3

			;				  +*(curPrevPtr              ))

			#define SOURCE SRC_PREV
			#define TARGET mm2
			#define DIRECTION 0

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,3
			psrlw mm3,3

			paddsw mm0,mm2
			paddsw mm1,mm3

			;mov ebx,015c215c2h
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_15c2h

			pmulhw mm0,mm2
			pmulhw mm1,mm2

			paddsw mm4,mm0
			paddsw mm5,mm1

			;		  - 3801*( *(curNextPtr+U1*nextPitch )

			#define SOURCE SRC_NEXT
			#define TARGET mm0
			#define DIRECTION U1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0
			punpckhbw mm1,mm1

			psrlw mm0,3
			psrlw mm1,3

			;				  +*(curNextPtr+D1*nextPitch )

			#define SOURCE SRC_NEXT
			#define TARGET mm2
			#define DIRECTION D1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,3
			psrlw mm3,3

			paddsw mm0,mm2
			paddsw mm1,mm3

			;				  +*(curPrevPtr+D1*prevPitch )

			#define SOURCE SRC_PREV
			#define TARGET mm2
			#define DIRECTION D1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,3
			psrlw mm3,3

			paddsw mm0,mm2
			paddsw mm1,mm3

			;				  +*(curPrevPtr+U1*prevPitch ))

			#define SOURCE SRC_PREV
			#define TARGET mm2
			#define DIRECTION U1

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,3
			psrlw mm3,3

			paddsw mm0,mm2
			paddsw mm1,mm3

			;mov ebx,00ed90ed9h
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_0ed9h

			pmulhw mm0,mm2
			pmulhw mm1,mm2

			psubsw mm4,mm0
			psubsw mm5,mm1

			;		  -  852*( *(curSrcPtr +ASU2*srcPitch)

			#define SOURCE SRC_CUR
			#define TARGET mm0
			#define DIRECTION ASU2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0
			punpckhbw mm1,mm1

			psrlw mm0,3
			psrlw mm1,3

			;				  +*(curSrcPtr +ASD2*srcPitch))

			#define SOURCE SRC_CUR
			#define TARGET mm2
			#define DIRECTION ASD2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,3
			psrlw mm3,3

			paddsw mm0,mm2
			paddsw mm1,mm3

			;mov ebx,003540354h
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_0354h

			pmulhw mm0,mm2
			pmulhw mm1,mm2

			psubsw mm4,mm0
			psubsw mm5,mm1

			;		  + 1016*( *(curNextPtr+D2*nextPitch )

			#define SOURCE SRC_NEXT
			#define TARGET mm0
			#define DIRECTION D2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm1,mm0

			punpcklbw mm0,mm0
			punpckhbw mm1,mm1

			psrlw mm0,3
			psrlw mm1,3

			;				  +*(curNextPtr+U2*nextPitch )

			#define SOURCE SRC_NEXT
			#define TARGET mm2
			#define DIRECTION U2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,3
			psrlw mm3,3

			paddsw mm0,mm2
			paddsw mm1,mm3

			;				  +*(curPrevPtr+U2*prevPitch )

			#define SOURCE SRC_PREV
			#define TARGET mm2
			#define DIRECTION U2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,3
			psrlw mm3,3

			paddsw mm0,mm2
			paddsw mm1,mm3

			;				  +*(curPrevPtr+D2*prevPitch ));
 
			#define SOURCE SRC_PREV
			#define TARGET mm2
			#define DIRECTION D2

			#include "leak_kernel\DeinterlaceLoad.cpp"

			movq mm3,mm2

			punpcklbw mm2,mm2
			punpckhbw mm3,mm3

			psrlw mm2,3
			psrlw mm3,3

			paddsw mm0,mm2
			paddsw mm1,mm3

			;mov ebx,003f803f8h
			;movd mm2,ebx
			;punpcklwd mm2,mm2

			movq mm2,qword_03f8h

			pmulhw mm0,mm2
			pmulhw mm1,mm2

			paddsw mm4,mm0
			paddsw mm5,mm1

			#if defined EAX_INVERTED
				neg eax
			#endif

			#undef EAX_INVERTED

			; intVal=lIntVal/16384;

			movq mm0,mm4
			movq mm1,mm5

			psraw mm0,8
			psraw mm1,8

			psubsw mm4,mm0
			psubsw mm5,mm1

			psraw mm4,4
			psraw mm5,4

		#endif

	#endif

	; convert words to bytes

	packuswb mm4,mm5

	; apply motion mask

	pand mm4,mm7
	pcmpeqb mm6,mm7

	movq mm0,[edi]
	pand mm0,mm6
	por mm0,mm4

	movq [edx],mm0
	
	; Update pointers

	add	esi,8
	add ecx,8
	add edi,8
	add edx,8

	sub bytesLeft,8
	jnz PREFIX2(ColLoop)

	jmp PREFIX2(RowEnd)

	align 16

PREFIX2(CopyThrough):

	movq mm0,[edi]

	add	esi,8
	add ecx,8
	add edi,8

	movq [edx],mm0

	add edx,8

	sub bytesLeft,8
	jnz PREFIX2(ColLoop)

PREFIX2(RowEnd):

	#undef SRC_PREV
	#undef SRC_CUR
	#undef SRC_NEXT

	#undef  PREFIX2
}

