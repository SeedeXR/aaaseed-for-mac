{
	MEMCPY( dstPtr, srcPtr, rowSize );

	dstPtr += dstPitch;
	
	if( order == 0 )
		srcPtr += srcPitch;
}

#undef PREFIX1
