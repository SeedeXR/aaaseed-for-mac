{
//	DBG_PRINT_STRING("motion mask rgb32_c");
	while( bytesLeft > 0 )
		{
//		DBG_PRINT_STRING("motion mask rgb32_c bytes left %d", bytesLeft);
		signed short valR1;
		signed short valR2;

		#if (PART != 2) || (defined TOPFIRST)
			valR1=  (signed short)*(curSrcAPtr)
			      - (signed short)*(curSrcBPtr)
				  + threshold;
		#else
			valR1=0;
		#endif

		#if (PART > 1) && ((PART < 4) || !(defined TOPFIRST))
			valR2=  (signed short)*(curSrcAPtr+srcAPitch)
			      - (signed short)*(curSrcBPtr+srcBPitch)
				  + threshold;
		#else
			valR2=0;
		#endif

		curSrcAPtr++; curSrcBPtr++;


		signed short valG1;
		signed short valG2;

		#if (PART != 2) || (defined TOPFIRST)
			valG1=  (signed short)*(curSrcAPtr)
			      - (signed short)*(curSrcBPtr)
				  + threshold;
		#else
			valG1=0;
		#endif

		#if (PART > 1) && ((PART < 4) || !(defined TOPFIRST))
			valG2=  (signed short)*(curSrcAPtr+srcAPitch)
			      - (signed short)*(curSrcBPtr+srcBPitch)
				  + threshold;
		#else
			valG2=0;
		#endif

		curSrcAPtr++; curSrcBPtr++;


		signed short valB1;
		signed short valB2;

		#if (PART != 2) || (defined TOPFIRST)
			valB1=  (signed short)*(curSrcAPtr)
			      - (signed short)*(curSrcBPtr)
				  + threshold;
		#else
			valB1=0;
		#endif

		#if (PART > 1) && ((PART < 4) || !(defined TOPFIRST))
			valB2=  (signed short)*(curSrcAPtr+srcAPitch)
			      - (signed short)*(curSrcBPtr+srcBPitch)
				  + threshold;
		#else
			valB2=0;
		#endif

		curSrcAPtr+=2; curSrcBPtr+=2;


		unsigned long maskVal;

		if      (((unsigned short)valR1 > doubleThreshold) ||
			     ((unsigned short)valR2 > doubleThreshold) ||

			     ((unsigned short)valG1 > doubleThreshold) ||
			     ((unsigned short)valG2 > doubleThreshold) ||

			     ((unsigned short)valB1 > doubleThreshold) ||
			     ((unsigned short)valB2 > doubleThreshold))
			maskVal=0x00ffffff;
		else
			maskVal=0x00000000;

		#if PART == 1
			*(curMaskPtr) =maskVal;
		#else
			*(curMaskPtr)|=maskVal;
		#endif

 		curMaskPtr++; bytesLeft-=4;
	}

	#undef TOPFIRST

	#undef PREFIX2
}

