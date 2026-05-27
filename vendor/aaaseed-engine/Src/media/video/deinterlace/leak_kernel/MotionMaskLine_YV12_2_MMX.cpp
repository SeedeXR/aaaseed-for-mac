__asm
{
	//;mov eax,080808080h
	//;movd mm5,eax
	//;punpcklbw mm5,mm5

	movq mm5,qword_80h

	//;mov eax,001010101h
	//;movd mm6,eax
	//;punpcklbw mm6,mm6

	movq mm6,qword_01h

	mov eax,threshold
	movd mm7,eax
	punpcklbw mm7,mm7
	punpcklbw mm7,mm7
	punpcklbw mm7,mm7

	pxor mm7,mm5

	mov eax,srcAPitch

	mov ebx,maskPitch
	add ebx,ebx

	mov esi,curSrcAPtr
	mov edi,curSrcBPtr
	mov edx,curMaskPtr

	mov ecx,bytesLeft
	add ecx,7
	and ecx,-8

PREFIX2(ColLoop):

	#if (PART != 2) || (defined TOPFIRST)

		movq mm0,[esi]
		movq mm2,[edi]

		//; convert unsigned to signed

		pxor mm0,mm5
		pxor mm2,mm5

		//; build a mask in mm3 of which bytes in mm2 are bigger 
		//; than their counterparts in mm0 

		movq mm3,mm2
		pcmpgtb mm3,mm0
		
		; calculate the differences

		psubb mm0,mm2

		; flip sign of the bytes masked by mm3 (twos complement)

		pxor mm0,mm3
		pand mm3,mm6
		paddb mm0,mm3

		pxor mm0,mm5
		pcmpgtb mm0,mm7
		
	#else
		pxor mm0,mm0
	#endif

	#if (PART > 1) && ((PART < 4) || !(defined TOPFIRST))

		movq mm1,[esi+eax]
		movq mm2,[edi+eax]

		; convert unsigned to signed

		pxor mm1,mm5
		pxor mm2,mm5

		; build a mask in mm3 of which bytes in mm2 are bigger 
		; than their counterparts in mm1

		movq mm3,mm2
		pcmpgtb mm3,mm1
		
		; calculate the differences

		psubb mm1,mm2

		; flip sign of the bytes masked by mm3 (twos complement)

		pxor mm1,mm3
		pand mm3,mm6
		paddb mm1,mm3

		pxor mm1,mm5
		pcmpgtb mm1,mm7

		por mm0,mm1

	#endif

	#if defined HALFSIZE

		movq mm1,mm0

		punpcklbw mm0,mm0
		punpckhbw mm1,mm1

		#if defined OVERWRITE
			movq [edx      ],mm0
			movq [edx+8    ],mm1
			movq [edx+ebx  ],mm0
			movq [edx+ebx+8],mm1
		#else

			movq mm2,[edx      ]
			movq mm3,[edx+8    ]

			por mm2,mm0
			por mm3,mm1

			movq [edx      ],mm2
			movq [edx+8    ],mm3

			movq mm2,[edx+ebx  ]
			movq mm3,[edx+ebx+8]

			por mm2,mm0
			por mm3,mm1

			movq [edx+ebx  ],mm2
			movq [edx+ebx+8],mm3

		#endif

		add edx,16

	#else

		#if defined OVERWRITE
			movq [edx],mm0
		#else
			movq mm1,[edx]
			por mm1,mm0
			movq [edx],mm1
		#endif

		add edx,8

	#endif

	add esi,8
	add edi,8

	sub ecx,8
	jnz PREFIX2(ColLoop)
	
	#undef PREFIX2
}
