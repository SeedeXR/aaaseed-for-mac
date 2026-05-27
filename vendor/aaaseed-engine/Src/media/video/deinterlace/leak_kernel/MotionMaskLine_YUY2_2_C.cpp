{
	while (bytesLeft > 0)
	{
		signed short valY11;
		signed short valY12;

		#if (PART != 2) || (defined TOPFIRST)
			valY11= (signed short)*(curSrcAPtr)
			       -(signed short)*(curSrcBPtr)
				   +threshold;
		#else
			valY11=0;
		#endif

		#if (PART > 1) && ((PART < 4) || !(defined TOPFIRST))
			valY12= (signed short)*(curSrcAPtr+srcAPitch)
			       -(signed short)*(curSrcBPtr+srcBPitch)
				   +threshold;
		#else
			valY12=0;
		#endif

		curSrcAPtr++; curSrcBPtr++;


		signed short valU1;
		signed short valU2;

		#if (PART != 2) || (defined TOPFIRST)
			valU1= (signed short)*(curSrcAPtr)
			      -(signed short)*(curSrcBPtr)
				  +threshold;
		#else
			valU1=0;
		#endif

		#if (PART > 1) && ((PART < 4) || !(defined TOPFIRST))
			valU2= (signed short)*(curSrcAPtr+srcAPitch)
			      -(signed short)*(curSrcBPtr+srcBPitch)
				  +threshold;
		#else
			valU2=0;
		#endif

		curSrcAPtr++; curSrcBPtr++;


		signed short valY21;
		signed short valY22;

		#if (PART != 2) || (defined TOPFIRST)
			valY21= (signed short)*(curSrcAPtr)
			       -(signed short)*(curSrcBPtr)
				   +threshold;
		#else
			valY21=0;
		#endif

		#if (PART > 1) && ((PART < 4) || !(defined TOPFIRST))
			valY22= (signed short)*(curSrcAPtr+srcAPitch)
			       -(signed short)*(curSrcBPtr+srcBPitch)
				   +threshold;
		#else
			valY22=0;
		#endif

		curSrcAPtr++; curSrcBPtr++;


		signed short valV1;
		signed short valV2;

		#if (PART != 2) || (defined TOPFIRST)
			valV1= (signed short)*(curSrcAPtr)
			      -(signed short)*(curSrcBPtr)
				  +threshold;
		#else
			valV1=0;
		#endif

		#if (PART > 1) && ((PART < 4) || !(defined TOPFIRST))
			valV2= (signed short)*(curSrcAPtr+srcAPitch)
			      -(signed short)*(curSrcBPtr+srcBPitch)
				  +threshold;
		#else
			valV2=0;
		#endif

		curSrcAPtr++; curSrcBPtr++;


		#if defined LINKED

			unsigned long maskVal;

			if      (((unsigned short)valU1 > doubleThreshold) ||
					 ((unsigned short)valU2 > doubleThreshold) ||

					 ((unsigned short)valV1 > doubleThreshold) ||
					 ((unsigned short)valV2 > doubleThreshold))
				maskVal=0xffffffff;

			else
			{
				maskVal=0x00000000;

				if (((unsigned short)valY11 > doubleThreshold) ||
					((unsigned short)valY12 > doubleThreshold))
					maskVal|=0xff00ffff;

				if (((unsigned short)valY21 > doubleThreshold) ||
					((unsigned short)valY22 > doubleThreshold))
					maskVal|=0xffffff00;
			}

		#else

			unsigned long maskVal;

			if      (((unsigned short)valU1 > doubleThreshold) ||
					 ((unsigned short)valU2 > doubleThreshold) ||

					 ((unsigned short)valV1 > doubleThreshold) ||
					 ((unsigned short)valV2 > doubleThreshold))
				maskVal=0xff00ff00;
			else
				maskVal=0x00000000;

			if (((unsigned short)valY11 > doubleThreshold) ||
				((unsigned short)valY12 > doubleThreshold))
				maskVal|=0x000000ff;

			if (((unsigned short)valY21 > doubleThreshold) ||
				((unsigned short)valY22 > doubleThreshold))
				maskVal|=0x00ff0000;

		#endif

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

