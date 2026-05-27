{
	while (bytesLeft > 0)
	{
		signed short val1;
		signed short val2;

		#if (PART != 2) || (defined TOPFIRST)
			val1= (signed short)*(curSrcAPtr)
				 -(signed short)*(curSrcBPtr)
				 +threshold;
		#else
			val1=0;
		#endif

		#if (PART > 1) && ((PART < 4) || !(defined TOPFIRST))
			val2= (signed short)*(curSrcAPtr+srcAPitch)
				 -(signed short)*(curSrcBPtr+srcBPitch)
				 +threshold;
		#else
			val2=0;
		#endif

		#if !defined HALFSIZE

 			unsigned char maskVal;

 			if (((unsigned short)val1 > doubleThreshold) ||
 			    ((unsigned short)val2 > doubleThreshold))
 				maskVal=0xff;
 			else
 				maskVal=0x00;

 			#if defined OVERWRITE
 				*(curMaskPtr) =maskVal;
 			#else
 				*(curMaskPtr)|=maskVal;
			#endif

		#else // defined HALFSIZE

 			unsigned short maskVal;

 			if (((unsigned short)val1 > doubleThreshold) ||
 			    ((unsigned short)val2 > doubleThreshold))
 				maskVal=0xffff;
 			else
 				maskVal=0x0000;

 			#if defined OVERWRITE
 				*(curMaskPtr          ) =maskVal;
 				*(curMaskPtr+maskPitch) =maskVal;
 			#else
 				*(curMaskPtr          )|=maskVal;
 				*(curMaskPtr+maskPitch)|=maskVal;
			#endif

		#endif

 		curMaskPtr++; curSrcAPtr++; curSrcBPtr++; bytesLeft--;
	}

	#undef OVERWRITE
	#undef TOPFIRST

	#undef PREFIX2
}

