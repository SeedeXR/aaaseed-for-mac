;rgbtoyuv.asm

;The loop processes interleaved RGB values for 8 pixels.

;The notation in the comments which describe the data locate

;the first byte on the right. For example in a register containing 

;G2R2B1G1R1B0G0R0, R0 is in the position of the lease significant

;byte and G2 is in the position of the most significant byte. 

;The output is to separate Y, U, and V buffers. Both input and 

;output data are bytes.



	TITLE rgbtoyuv

	.486P

.model FLAT

PUBLIC _rgbtoyuv



_DATA SEGMENT



ALIGN 	8

ZEROSX 	dw	0,0,0,0

ZEROS  	dd 	?,?



OFFSETDX 	dw	0,64,0,64	;offset used before shift 

OFFSETD  	dd 	?,?

OFFSETWX 	dw 	128,0,128,0	;offset used before pack 32

OFFSETW  	dd 	?,?

OFFSETBX 	dw 	128,128,128,128

OFFSETB  	dd 	?,?



TEMP0	 	dd 	?,?

TEMPY 	dd 	?,?

TEMPU	 	dd 	?,?

TEMPV 	dd 	?,?



YR0GRX 	dw 	9798,19235,0,9798

YBG0BX 	dw 	3736,0,19235,3736

YR0GR  	dd 	?,?

YBG0B  	dd 	?,?

UR0GRX 	dw 	-4784,-9437,0,-4784

UBG0BX 	dw 	14221,0,-9437,14221

UR0GR  	dd 	?,?

UBG0B  	dd 	?,?

VR0GRX 	dw 	20218,-16941,0,20218

VBG0BX 	dw 	-3277,0,-16941,-3277

VR0GR  	dd 	?,?

VBG0B  	dd 	?,?



_DATA ENDS

_TEXT SEGMENT



_inPtr$ 	=	 8

_rows$	=	12

_columns$	=	16

_outyPtr$	=	20

_outuPtr$	=	24

_outvPtr$	=	28



_rgbtoyuv PROC NEAR



	push	ebp

	mov	ebp,	esp

	push	eax

	push	ebx

	push	ecx

	push	edx

	push	esi

	push	edi



	lea	eax, 	ZEROSX	;This section gets around a bug

	movq	mm0, 	[eax]	;unlikely to persist

	movq	ZEROS,	mm0

	lea	eax, 	OFFSETDX

	movq	mm0, 	[eax]

	movq	OFFSETD,	mm0

	lea	eax, 	OFFSETWX

	movq	mm0, 	[eax]

	movq	OFFSETW,	mm0

	lea	eax, 	OFFSETBX

	movq	mm0,	[eax]

	movq	OFFSETB,	mm0

	lea	eax, 	YR0GRX

	movq	mm0, 	[eax]

	movq	YR0GR, 	mm0

	lea	eax, 	YBG0BX

	movq	mm0, 	[eax]

	movq	YBG0B, 	mm0

	lea	eax, 	UR0GRX

	movq	mm0, 	[eax]

	movq	UR0GR, 	mm0

	lea	eax, 	UBG0BX

	movq	mm0, 	[eax]

	movq	UBG0B, 	mm0

	lea	eax, 	VR0GRX

	movq	mm0, 	[eax]

	movq	VR0GR, 	mm0

	lea	eax, 	VBG0BX

	movq	mm0, 	[eax]

	movq	VBG0B, 	mm0



	mov	eax, 	_rows$[ebp]

	mov	ebx, 	_columns$[ebp]

	mul	ebx		;number pixels

	shr	eax, 	3	;number of loops

	mov	edi, 	eax	;loop counter in edi

	mov	eax, 	_inPtr$[ebp]

	mov	ebx, 	_outyPtr$[ebp]

	mov	ecx, 	_outuPtr$[ebp]

	mov	edx, 	_outvPtr$[ebp]

	sub	edx, 	8	;incremented before write



RGBtoYUV:

	movq	mm1, 	[eax]	;load G2R2B1G1R1B0G0R0

	pxor	mm6, 	mm6	;0 -> mm6



	movq	mm0, 	mm1	;G2R2B1G1R1B0G0R0 -> mm0

	psrlq	mm1, 	16	;00G2R2B1G1R1B0-> mm1



	punpcklbw 	mm0, 	ZEROS	;R1B0G0R0 -> mm0

	movq	mm7, 	mm1	;00G2R2B1G1R1B0-> mm7



	punpcklbw 	mm1, 	ZEROS 	;B1G1R1B0 -> mm1

	movq	mm2, 	mm0	;R1B0G0R0 -> mm2



	pmaddwd	mm0,	YR0GR	;yrR1,ygG0+yrR0 -> mm0

	movq	mm3, 	mm1	;B1G1R1B0 -> mm3



	pmaddwd	mm1,	YBG0B	;ybB1+ygG1,ybB0 -> mm1

	movq	mm4, 	mm2	;R1B0G0R0 -> mm4



	pmaddwd	mm2,	UR0GR	;urR1,ugG0+urR0 -> mm2

	movq	mm5, 	mm3	;B1G1R1B0 -> mm5



	pmaddwd	mm3,	UBG0B	;ubB1+ugG1,ubB0 -> mm3

	punpckhbw 	mm7, 	mm6;	00G2R2 -> mm7



	pmaddwd	mm4,	VR0GR	;vrR1,vgG0+vrR0 -> mm4

	paddd	mm0, 	mm1	;Y1Y0 -> mm0



	pmaddwd	mm5,	VBG0B	;vbB1+vgG1,vbB0 -> mm5



	movq	mm1,	8[eax]	;R5B4G4R4B3G3R3B2 -> mm1

	paddd	mm2, 	mm3	;U1U0 -> mm2



	movq	mm6, 	mm1	;R5B4G4R4B3G3R3B2 -> mm6



	punpcklbw 	mm1, 	ZEROS	;B3G3R3B2 -> mm1

	paddd	mm4, 	mm5	;V1V0 -> mm4



	movq	mm5, 	mm1	;B3G3R3B2 -> mm5

 	psllq	mm1, 	32	;R3B200 -> mm1



	paddd	mm1, 	mm7	;R3B200+00G2R2=R3B2G2R2->mm1



	punpckhbw 	mm6, 	ZEROS	;R5B4G4R3 -> mm6

	movq	mm3, 	mm1	;R3B2G2R2 -> mm3



	pmaddwd	mm1, 	YR0GR	;yrR3,ygG2+yrR2 -> mm1

	movq	mm7, 	mm5	;B3G3R3B2 -> mm7



	pmaddwd	mm5, 	YBG0B	;ybB3+ygG3,ybB2 -> mm5

	psrad	mm0, 	15	;32-bit scaled Y1Y0 -> mm0



	movq	TEMP0,	mm6	;R5B4G4R4 -> TEMP0

	movq	mm6, 	mm3	;R3B2G2R2 -> mm6

	pmaddwd	mm6, 	UR0GR	;urR3,ugG2+urR2 -> mm6

	psrad	mm2, 	15	;32-bit scaled U1U0 -> mm2



	paddd	mm1, 	mm5	;Y3Y2 -> mm1

	movq	mm5, 	mm7	;B3G3R3B2 -> mm5

	pmaddwd	mm7, 	UBG0B	;ubB3+ugG3,ubB2

	psrad		mm1, 15	;32-bit scaled Y3Y2 -> mm1



	pmaddwd	mm3, 	VR0GR	;vrR3,vgG2+vgR2

	packssdw 	mm0, 	mm1	;Y3Y2Y1Y0 -> mm0



	pmaddwd	mm5, 	VBG0B	;vbB3+vgG3,vbB2 -> mm5

	psrad	mm4, 	15	;32-bit scaled V1V0 -> mm4



	movq	mm1, 	16[eax]	;B7G7R7B6G6R6B5G5 -> mm7

	paddd	mm6, 	mm7	;U3U2 -> mm6	



	movq 	mm7, 	mm1	;B7G7R7B6G6R6B5G5 -> mm1

	psrad	mm6, 	15	;32-bit scaled U3U2 -> mm6



	paddd	mm3, 	mm5	;V3V2 -> mm3

	psllq	mm7, 	16	;R7B6G6R6B5G500 -> mm7



	movq  	mm5, 	mm7	;R7B6G6R6B5G500 -> mm5

	psrad	mm3, 	15	;32-bit scaled V3V2 -> mm3



	movq	TEMPY,	mm0	;32-bit scaled Y3Y2Y1Y0 -> TEMPY

	packssdw 	mm2,	mm6	;32-bit scaled U3U2U1U0 -> mm2



	movq	mm0,	TEMP0	;R5B4G4R4 -> mm0



	punpcklbw 	mm7,	ZEROS	;B5G500 -> mm7

	movq	mm6,	mm0	;R5B4G4R4 -> mm6



	movq	TEMPU,	mm2	;32-bit scaled U3U2U1U0 -> TEMPU

	psrlq	mm0,	32	;00R5B4 -> mm0



	paddw 	mm7,	mm0	;B5G5R5B4 -> mm7

	movq	mm2,	mm6	;B5B4G4R4 -> mm2



	pmaddwd	mm2,	YR0GR	;yrR5,ygG4+yrR4 -> mm2

	movq	mm0,	mm7	;B5G5R5B4 -> mm0



	pmaddwd	mm7,	YBG0B	;ybB5+ygG5,ybB4 -> mm7

	packssdw 	mm4,	mm3	;32-bit scaled V3V2V1V0 -> mm4



	add	eax, 	24	;increment RGB count

	add	edx, 	8	;increment V count



	movq	TEMPV,	mm4	;(V3V2V1V0)/256 -> mm4

	movq 	mm4, 	mm6	;B5B4G4R4 -> mm4



	pmaddwd	mm6, 	UR0GR	;urR5,ugG4+urR4

	movq	mm3, 	mm0	;B5G5R5B4 -> mm0



	pmaddwd	mm0, 	UBG0B	;ubB5+ugG5,ubB4

	paddd	mm2, 	mm7	;Y5Y4 -> mm2



	pmaddwd 	mm4, 	VR0GR	;vrR5,vgG4+vrR4 -> mm4

	pxor	mm7, 	mm7	;0 -> mm7



	pmaddwd	mm3, 	VBG0B	;vbB5+vgG5,vbB4 -> mm3

	punpckhbw 	mm1, 	mm7	;B7G7R7B6 -> mm1



	paddd	mm0, 	mm6	;U5U4 -> mm0

	movq	mm6, 	mm1	;B7G7R7B6 -> mm6



	pmaddwd	mm6, 	YBG0B	;ybB7+ygG7,ybB6 -> mm6

	punpckhbw 	mm5, 	mm7	;R7B6G6R6 -> mm5



	movq	mm7, 	mm5	;R7B6G6R6 -> mm7

	paddd	mm3, 	mm4	;V5V4 -> mm3



	pmaddwd	mm5, 	YR0GR	;yrR7,ygG6+yrR6 -> mm5

	movq	mm4, 	mm1	;B7G7R7B6 -> mm4



	pmaddwd	mm4,	UBG0B	;ubB7+ugG7,ubB6 -> mm4

	psrad	mm0, 	15	;32-bit scaled U5U4 -> mm0



	paddd	mm0, 	OFFSETW 	;add offset to U5U4 -> mm0

	psrad	mm2, 	15	;32-bit scaled Y5Y4 -> mm2

	

	paddd	mm6, 	mm5	;Y7Y6 -> mm6

	movq	mm5, 	mm7	;R7B6G6R6 -> mm5



	pmaddwd	mm7, 	UR0GR	;urR7,ugG6+ugR6 -> mm7

	psrad	mm3, 	15	;32-bit scaled V5V4 -> mm3

	

	pmaddwd	mm1, 	VBG0B 	;vbB7+vgG7,vbB6 -> mm1

	psrad	mm6, 	15	;32-bit scaled Y7Y6 -> mm6



	paddd	mm4, 	OFFSETD	;add offset to U7U6 

	packssdw 	mm2, 	mm6	;Y7Y6Y5Y4 -> mm2



	pmaddwd	mm5, 	VR0GR	;vrR7,vgG6+vrR6 -> mm5

	paddd	mm7, 	mm4	;U7U6 -> mm7	



	psrad	mm7, 	15	;32-bit scaled U7U6 -> mm7



	movq	mm6, 	TEMPY	;32-bit scaled Y3Y2Y1Y0 -> mm6

	packssdw 	mm0, 	mm7	;32-bit scaled U7U6U5U4 -> mm0



	movq	mm4, 	TEMPU	;32-bit scaled U3U2U1U0 -> mm4

	packuswb 	mm6, 	mm2	;all 8 Y values -> mm6



	movq	mm7, 	OFFSETB	;128,128,128,128 -> mm7

	paddd	mm1, 	mm5	;V7V6 -> mm1



	paddw	mm4, 	mm7	;add offset to U3U2U1U0/256 

	psrad	mm1, 	15	;32-bit scaled V7V6 -> mm1



	movq	[ebx],	mm6	;store Y

	packuswb 	mm4, 	mm0	;all 8 U values -> mm4



	movq	mm5, 	TEMPV	;32-bit scaled V3V2V1V0 -> mm5

	packssdw 	mm3, 	mm1	;V7V6V5V4 -> mm3

	paddw	mm5, 	mm7	;add offset to 	V3V2V1V0

	paddw	mm3, 	mm7	;add offset to 	V7V6V5V4



	movq	[ecx],	mm4	;store U

	packuswb 	mm5,	mm3	;ALL 8 V values -> mm5



	add 	ebx, 	8	;increment Y count

	add	ecx, 	8	;increment U count



	movq	[edx],	mm5	;store V



	dec 	edi		;decrement loop counter

	jnz	RGBtoYUV	;do 24 more bytes if not 0



	pop 	edi

	pop	esi

	pop	edx

	pop	ecx

	pop 	ebx

	pop 	eax

	pop	ebp



	ret 	0



_rgbtoyuv ENDP

_TEXT ENDS

END
