{
	__asm
	{
		mov esi,srcPtr
		mov edi,dstPtr

		mov edx,rowSize
		mov ecx,edx

		add edx,7
		and ecx,-32
		and edx,24

		add esi,ecx
		add edi,ecx

		neg ecx

	PREFIX1(ColLoop):
		  mov eax,ecx

		movq mm0,[esi+ecx   ]
		movq mm1,[esi+ecx+8 ]
		movq mm2,[esi+ecx+16]
		movq mm3,[esi+ecx+24]

		;mov eax,ecx

		  add ecx,32

		movq [edi+eax   ],mm0
		movq [edi+eax+8 ],mm1
		movq [edi+eax+16],mm2
		movq [edi+eax+24],mm3

		;add ecx,8
		jnz PREFIX1(ColLoop)

		add esi,edx
		add edi,edx

		neg edx
		jz PREFIX1(RowDone)

	PREFIX1(EndLoop):
		  mov eax,edx

		movq mm0,[esi+edx]

		;mov eax,edx

		  add edx,8

		movq [edi+eax],mm0

		;add edx,8
		jnz PREFIX1(EndLoop)

PREFIX1(RowDone):
	}

	dstPtr += dstPitch;
	
	if( order == 0 )
		srcPtr += srcPitch;

	#undef PREFIX1
}
