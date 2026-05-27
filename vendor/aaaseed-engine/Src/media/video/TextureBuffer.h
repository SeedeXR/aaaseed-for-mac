
//#ifndef __TEXTUREBUFFER__
//#define __TEXTUREBUFFER__
///************************************************************************/ /**
// * \file TextureBuffer.h
// **/ /************************************************************************/
////#define GL_WRITE_PIXEL_DATA_RANGE_NV 0x8878
////#define GL_READ_PIXEL_DATA_RANGE_NV 0x8879
//
///************************************************************************/ /**
//This class is a buffer of images wich helps for :
//
//- allocating the memory from system memory or AGP memory
//- attaching an OpenGL texture to this buffer in order to bind various images depending on an index
//- managing the Pixel Data Range
//.
// **/
//
//#pragma once
//
//#ifndef AAA_AAA_TYPE_H
//#	include "aaa_type.h"
//#endif
//
//
//class c_texture_buffer 
//{
//protected:
//#ifndef	AAASEED
//	INT32	_size_x;
//	INT32	_size_y;
//	GLenum	_type;
//	INT32	_ncomp;
//	INT32	_images_nb;
//	//unsigned	char*	images_;
//	UINT8*	_images;
//	bool	_b_use_pdr;
//	INT32	_strip_width;
//	bool	_b_strip_do_stride;
//#endif	
////	static	unsigned	char*	AGPMem;
////	static	unsigned	char*	curAGPMemPtr;
//	static	UINT8*	AGPMem;
//	static	UINT8*	curAGPMemPtr;
//	static	INT32	AGPMemSize;
//	static	INT32	AGPMemTotalSize;
//
//	static	REAL	agp_uploads;
//	static	REAL	memcpy_uploads;
//
////	unsigned	int	_gltex;
////unused	GLenum			_target;
//public:
//	// AGP memory should allocated only once for the whole app.
//	static	bool	init_agp_memory( INT32 size, REAL app_read, REAL app_write, REAL priority );
//	static	UINT8*	get_agp_memory( INT32 size );
//	static	void	clear_statistics()			{ agp_uploads = memcpy_uploads = .0f; }
//	static	REAL	get_total_agp_upload()		{ return agp_uploads; }
//	static	REAL	get_total_memcpy()			{ return memcpy_uploads; }
//	static	REAL	get_agp_total_size()		{ return AGPMemTotalSize / (1024.0f * 1024.0f); }
//	static	REAL	get_agp_free_size()			{ return AGPMemSize / (1024.0f * 1024.0f); }
//
//public:
//#ifndef	AAASEED
//	c_texture_buffer();
//	~c_texture_buffer();
//	
//
//			bool	init( INT32 w, INT32 h, INT32 t, INT32 n, bool use_pdr = false );
//			INT32	get_size_x()	{ return _size_x; }
//			INT32	get_size_y()	{ return _size_y; }
//			INT32	get_type()		{ return _type; }
//	
//			INT32	get_num_components();
//			INT32	get_num_images()	{ return _images_nb; }
//	
//			UCHAR*	get_images();
//			UCHAR*	get_image( INT32 i );
//	
////unused	void	assign_texture( unsigned int glid, GLenum gltarget );
//			void	bind_texture();
//	
//			void	copy_image( c_texture_buffer* src, INT32 n );
//			void	TexSubImage2D( INT32 n );
//	
////unused
///*		bool	is_texture_valid() 
//					{
//						if( !is_inited() ) 
//							return false;
//						return _gltex != 0;
//					}
//*/	
//			bool	is_inited() { return ( _size_x > 0 && _size_y > 0 && _images_nb > 0 && _images != nullptr ); }
//
//			void	strip_paging( INT32 width = -1 );
//			void	strip_paging_stride( bool useStride );
//			void	enable_pdr( INT32 n );
//#endif
//	static	void	pdr_disable();
//	static	void	pdr_flush();
//	static	void	pdr_enable();
//	
//};
//#endif
