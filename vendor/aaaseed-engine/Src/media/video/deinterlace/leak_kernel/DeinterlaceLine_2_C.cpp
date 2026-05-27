{
	while (bytesLeft > 0)
		{
		if( *curMaskPtr == 0 )
			*curDstPtr=*curNextPtr;
		else
			{
			signed short intVal;

			#if !defined SHARP

				#if !defined TWOWAY

					//val = (8*((int)srcpp[x] + (int)srcpn[x]) + 2*((int)prvp[x]) -
					//  	(int)(prvppp[x]) - (int)(prvpnn[x])) >> 4;

					intVal=+8*( *(curSrcPtr +ASU1*srcPitch)
						       +*(curSrcPtr +ASD1*srcPitch))
						   +2*( *(curPrevPtr              ))
						   -    *(curPrevPtr+U1*prevPitch )
						   -    *(curPrevPtr+D1*prevPitch );

				#else // if defined TWOWAY

					// val = (8*((int)srcpp[x] + (int)srcpn[x]) + 2*((int)srcp[x] + (int)prvp[x]) -
					//		 (int)(srcppp[x]) - (int)(srcpnn[x]) -
					//		 (int)(prvppp[x]) - (int)(prvpnn[x])) >> 4;

					intVal=+8*( *(curSrcPtr +ASU1*srcPitch)
						       +*(curSrcPtr +ASD1*srcPitch))
						   +2*( *(curNextPtr              )
						       +*(curPrevPtr              ))
						   -    *(curNextPtr+U1*nextPitch )
						   -    *(curNextPtr+D1*nextPitch )
						   -    *(curPrevPtr+U1*prevPitch )
						   -    *(curPrevPtr+D1*prevPitch );

				#endif

				intVal/=16;

			#else // if defined SHARP

				signed long lIntVal;

				#if !defined TWOWAY

					//	valf = + 0.526*((int)srcpp[x] + (int)srcpn[x])
					//		   + 0.170*((int)prvp[x])
					//		   - 0.116*((int)prvppp[x] + (int)prvpnn[x])
					//		   - 0.026*((int)srcp3p[x] + (int)srcp3n[x])
					//		   + 0.031*((int)prvp4p[x] + (int)prvp4p[x]);
					//                                             ^ n, most probably

					lIntVal=+17236*( *(curSrcPtr +ASU1*srcPitch)
					                +*(curSrcPtr +ASD1*srcPitch))
							+ 5570*( *(curPrevPtr              ))
							- 3801*( *(curPrevPtr+U1*prevPitch )
							        +*(curPrevPtr+D1*prevPitch ))
							-  852*( *(curSrcPtr +ASU2*srcPitch)
							        +*(curSrcPtr +ASD2*srcPitch))
							+ 1016*( *(curPrevPtr+U2*prevPitch )
							        +*(curPrevPtr+D2*prevPitch ));

				#else // if defined TWOWAY

					//	valf = + 0.526*((int)srcpp[x] + (int)srcpn[x])
					//		   + 0.170*((int)srcp[x] + (int)prvp[x])
					//		   - 0.116*((int)srcppp[x] + (int)srcpnn[x] + (int)prvppp[x] + (int)prvpnn[x])
					//		   - 0.026*((int)srcp3p[x] + (int)srcp3n[x])
					//		   + 0.031*((int)srcp4p[x] + (int)srcp4n[x] + (int)prvp4p[x] + (int)prvp4n[x]);

					lIntVal=+17236*( *(curSrcPtr +ASU1*srcPitch)
					                +*(curSrcPtr +ASD1*srcPitch))
							+ 5570*( *(curNextPtr              )
							        +*(curPrevPtr              ))
							- 3801*( *(curNextPtr+U1*nextPitch )
							        +*(curNextPtr+D1*nextPitch )
							        +*(curPrevPtr+U1*prevPitch )
							        +*(curPrevPtr+D1*prevPitch ))
							-  852*( *(curSrcPtr +ASU2*srcPitch)
							        +*(curSrcPtr +ASD2*srcPitch))
							+ 1016*( *(curNextPtr+U2*nextPitch )
							        +*(curNextPtr+D2*nextPitch )
							        +*(curPrevPtr+U2*prevPitch )
							        +*(curPrevPtr+D2*prevPitch ));

				#endif

				intVal = (signed short)(lIntVal >> 15);	//maa was /32768

			#endif

			#if COLORSPACE == COLORSPACE_YV12

				#if !defined HALFSIZE
					if( intVal > 240 ) intVal = 240;
				#else // defined HALFSIZE
					if( intVal > 235 ) intVal = 235;
				#endif
				else if( intVal < 16 ) intVal = 16;

			#elif COLORSPACE == COLORSPACE_YUY2

				if( ( (unsigned long)curDstPtr & 1 ) == 0 )
					{
					if( intVal > 240 ) intVal = 240;
					else if( intVal < 16 ) intVal = 16;
					}
				else
					{
					if( intVal > 235 ) intVal = 235;
					else if( intVal < 16 ) intVal = 16;
					}

			#else

				if( intVal > 255 ) intVal = 255;
				else if( intVal < 0 ) intVal = 0;

			#endif

			*curDstPtr=(unsigned char)intVal;
			}

		curMaskPtr++; curDstPtr++; curPrevPtr++; curSrcPtr++; curNextPtr++; bytesLeft--;
		}

	#undef  PREFIX2
}
