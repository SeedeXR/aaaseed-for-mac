{
	#if( DIRECTION < 0 ) && !defined( EAX_INVERTED )
		#define EAX_INVERTED

		neg eax
	#elif( DIRECTION > 0 ) && defined( EAX_INVERTED )
		#undef EAX_INVERTED

		neg eax
	#endif

	#if SOURCE == SRC_PREV
		#define ASOURCE esi
	#elif SOURCE == SRC_CUR
		#define ASOURCE ecx
	#elif SOURCE == SRC_NEXT
		#define ASOURCE edi
	#endif

	#if (DIRECTION == -2) || (DIRECTION == 2)
		movq TARGET,[ASOURCE + 2*eax]
	#elif (DIRECTION == -1) || (DIRECTION == 1)
		movq TARGET,[ASOURCE + eax]
	#else
		movq TARGET,[ASOURCE]
	#endif

	#undef REGISTER
	#undef SOURCE
	#undef TARGET
	#undef DIRECTION

	#undef ASOURCE
}
