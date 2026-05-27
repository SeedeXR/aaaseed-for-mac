///************************************************************************/ /**
// * \file TextureBuffer.cpp
// **/ /************************************************************************/
///*
//#ifdef _WIN32
//#  include <windows.h>
//#  pragma warning(disable:4244)   // No warnings on precision truncation
//#  pragma warning(disable:4305)   // No warnings on precision truncation
//#  pragma warning(disable:4786)   // stupid symbol size limitation
//#else
//#  include <GL/glx.h>
//#endif
//
//#include <iostream>
//*/
//
////#include "draw/aaa_gl.h"
////#define	WGL_WGLEXT_PROTOTYPES	TRUE
////#include "wglew.h"
//#include "media/Video/TextureBuffer.h"
//#include "gol/gol.h"
//
//UINT8*	c_texture_buffer::AGPMem = nullptr;		///< ptr of AGP memory pool
//UINT8*	c_texture_buffer::curAGPMemPtr = nullptr;	///< current ptr for allocation process
//INT32	c_texture_buffer::AGPMemTotalSize = 0;
//INT32	c_texture_buffer::AGPMemSize = 0;
//REAL	c_texture_buffer::agp_uploads = 0;		///< amount of Mbytes the PDR is handling
//REAL	c_texture_buffer::memcpy_uploads = 0;
//
//extern	bool	GOL::b_agpmem_can;
//
///************************************************************************
//	AGP init : allocate a big pool of memory that we'll use for various "sub-allocations"
//*/
//bool	c_texture_buffer::init_agp_memory( INT32 size, REAL app_read, REAL app_write, REAL priority ) 
//{
//	if( GOL::b_agpmem_can )
//	{
//		if( wglAllocateMemoryNV )
//		{
//			AGPMem = (UINT8*)wglAllocateMemoryNV(size, app_read, app_write, priority); // APP only reads from AGP memory
//	//				GLfloat readfreq, GLfloat writefreq, GLfloat priority);
//			if( !AGPMem )
//				ERR_PRINT_STRING( "Could not allocate %d MB of AGP memory", size/(1024*1024));
//		}
//		else
//			debug_break( "%s() wglAllocateMemoryNV not defined", __FUNCTION__ );
//		if( !AGPMem )
//		{
//			curAGPMemPtr = nullptr;
//			AGPMemSize = 0;
//			AGPMemTotalSize = 0;
//			return false;
//		}
//		curAGPMemPtr = AGPMem;
//		AGPMemSize = size;
//		AGPMemTotalSize = size;
//		return true;
//	}
//	else
//		return false;
//}
//
///************************************************************************
//	Allocates a part of the AGP memory pool
//*/
//UINT8*	c_texture_buffer::get_agp_memory( INT32 size )
//{
//	if( size > AGPMemSize )
//	{
//		ERR_PRINT_STRING( "Not enough AGP memory allocated");
//		return nullptr;
//	}
//	curAGPMemPtr += size;
//	AGPMemSize -= size;
//	return curAGPMemPtr - size;
//}
//
//#ifndef	AAASEED
//c_texture_buffer::c_texture_buffer()
//
//:_size_x(0)
//,_size_y(0)
//,_type(0)
//,_images(nullptr)
//,_images_nb(0)
//,_strip_width(-1)
//
//{
//}
//
//c_texture_buffer::~c_texture_buffer() 
//{
//	if( !_images )
//	{
//		free( _images );
//	}
//	if( AGPMem )
//	{
//		wglFreeMemoryNV( AGPMem );
//	}
//}
//
///************************************************************************
//	Initialisation of the buffer depending on the image properties.
//*/	
//
//bool	c_texture_buffer::init( INT32 w, INT32 h, INT32 t, INT32 n, bool b_use_PDR ) 
//{
//	DBG_PRINT_STRING( "c_texture_buffer::init %ix%i %i (%i)", w, h, n, b_use_PDR );
//	_size_x = w;
//	_size_y = h;
//	_type = t;
//	_images_nb = n;
//	_ncomp = get_num_components();
//	if( b_use_PDR )
//		_images = get_agp_memory( _size_x*_size_y*_ncomp*_images_nb );
//	else
//		_images = (UINT8*)CALLOC( _size_x*_size_y*_ncomp*_images_nb, 1 );
//	
//	if( !_images ) 
//	{
//		//LOGMSG( "c_texture_buffer: cannot allocate memory.\n");
//		//else fprintf(stderr, "c_texture_buffer: cannot allocate memory.\n");
//		return false;
//	}
//	return true;
//}
//	
//INT32	c_texture_buffer::get_num_components() 
//{
//INT32	ncomp = -1;
//	switch( _type )
//	{
//	case GL_RGB:
//		ncomp = 3;
//		break;
//	case GL_RGBA:
//	case GL_BGRA_EXT:
//		ncomp = 4;
//		break;
//	default:
//		ERR_PRINT_STRING( "c_texture_buffer: unknown type_. (GL_RGB and GL_BGRA supported)" );
//		break;
//	}
//	return ncomp;
//}
//	
///// return the memory used by this buffer
////unsigned char *	c_texture_buffer::get_images()
//UINT8*	c_texture_buffer::get_images()
//{
//	if( !is_inited() )
//		return nullptr;
//	return _images;
//}
///// return the memory of the picture i in the buffer.
////unsigned char *	c_texture_buffer::get_image( INT32 i ) 
//UINT8*	c_texture_buffer::get_image( INT32 i ) 
//{
//	if( !is_inited() )
//		return nullptr;
//	i = i % _images_nb;
//	return _images + _ncomp*_size_x*_size_y*i;
//}
//
////unused
///*
///// attach an existing OpenGL texture to this buffer
//void	c_texture_buffer::assign_texture( unsigned int glid, GLenum gltarget ) 
//{
//	_target = gltarget;
//	_gltex = glid;
//}
//*/
//
///*
///// simply bind the attached texture
//void c_texture_buffer::bind_texture() 
//{
//	if (! is_inited())
//		return;
//	if( !is_texture_valid() )
//	{
//		ERR_PRINT_STRING( "c_texture_buffer::bind_texture() failed");
//		return;
//	}
//	GOL::bind_texture( target_, gltex_ );
//}
//*/
//
///// make a copy of an image from src at index n to another buffer at index 0
//void	c_texture_buffer::copy_image( c_texture_buffer *src, INT32 n ) 
//{
//	UINT8*	data = src->get_image( n );
//	MEMCPY( _images, data, _size_x * _size_y * _ncomp );
//	memcpy_uploads += (float)_ncomp*_size_x*_size_y/(1024.0*1024.0);
//}
///*
///// do a glTexSubImage() with the image at index n
//void c_texture_buffer::TexSubImage2D(int n) 
//{
//	if( !is_inited() )
//		return;
//		
//	if( !is_texture_valid() )
//	{
//		ERR_PRINT_STRING( "Texture in c_texture_buffer not valid");
//		return;
//	}
//	GOL::bind_texture_2d( gltex_ );
//	n = n % numImages_;
//		
//#if 0
//		// page full width_ strips
//int w = width_;
//int h_left = height_;
//int h_start = 0;
//	while (h_left > 0)
//		{
//int h = h_left;
//		if ((stripWidth_ > 0) && (h > stripWidth_ ))
//			h = stripWidth_;
//		glTexSubImage2D(target_, 0, 0, h_start, w, h, type_,
//				GL_UNSIGNED_BYTE,
//				images_ + ncomp_*width_*height_ * n + ncomp_*width_*h_start);
//		h_start += h;
//		h_left -= h;
//		if ((h_start > height_ ) || (h_start < 0))
//			exit(-1);
//		}
//#else
//	if (stripDoStride_ )
//		GOL::set_pixel_storei(GL_UNPACK_ROW_LENGTH, width_ );
//	else
//		GOL::set_pixel_storei(GL_UNPACK_ROW_LENGTH, 0);
//	// page full height_ strips
//	// XXX - need to handle stride
//int h = height_;
//int w_left = width_;
//int w_start = 0;
//	while (w_left > 0)
//	{
//		int w = w_left;
//		if( (stripWidth_ > 0) && (w > stripWidth_) )
//			w = stripWidth_;
//		glTexSubImage2D(target_, 0, w_start, 0, w, h, type_,
//				GL_UNSIGNED_BYTE,
//				images_ + ncomp_*width_*height_ * n + 
//				ncomp_*w_start);
//		w_start += w;
//		w_left -= w;
//		if ((w_start > width_ ) || (w_start < 0))
//			exit(-1);
//	}
//#endif
//	agpuploads += (float)(ncomp_*width_*height_ )/(1024.0*1024.0);
//}
//*/
//
///// set the strip width_ for paging (-1 means do all at once)
//void	c_texture_buffer::strip_paging( INT32 width )
//{
//	_strip_width = width;
//}
//
///// set the strip width_ for paging (-1 means do all at once)
//void	c_texture_buffer::strip_paging_stride( bool useStride )
//{
//	_b_strip_do_stride = useStride;
//}
//
///// enable the PDR for the AGP memory range of image n
//void	c_texture_buffer::enable_pdr( INT32 n ) 
//{
//	if( GOL::b_agpmem_can )
//	{
//		n = n % _images_nb;
//		glPixelDataRangeNV( GL_WRITE_PIXEL_DATA_RANGE_NV, _size_x*_size_y*_ncomp,
//			_images + (_size_x*_size_y*_ncomp*n));
//		glEnableClientState( GL_WRITE_PIXEL_DATA_RANGE_NV );
//	}
//}
//#endif
//
///// enable PDR on the entire part of allocated AGP memory.
//void	c_texture_buffer::pdr_enable()
//{
//	if( GOL::b_agpmem_can )
//	{
//		glPixelDataRangeNV( GL_WRITE_PIXEL_DATA_RANGE_NV, AGPMemTotalSize, AGPMem );
//		glEnableClientState( GL_WRITE_PIXEL_DATA_RANGE_NV );
//	}
////todo	CHECKGLERRORS();
//}
///// disable the PDR
//void	c_texture_buffer::pdr_disable() 
//{
//	if( GOL::b_agpmem_can )
//	{
//		glPixelDataRangeNV( GL_WRITE_PIXEL_DATA_RANGE_NV, 0, nullptr );
//		glDisableClientState( GL_WRITE_PIXEL_DATA_RANGE_NV );
//	}
////todo	CHECKGLERRORS();
//}
///// force the DMA to copy data from AGP memory : flushing.
//void	c_texture_buffer::pdr_flush() 
//{
//	if( GOL::b_agpmem_can )
//	{
//		glFlushPixelDataRangeNV(GL_WRITE_PIXEL_DATA_RANGE_NV);
//	}
//}
//
