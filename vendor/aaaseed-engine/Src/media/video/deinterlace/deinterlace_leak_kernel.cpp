#include "deinterlace_leak_kernel.h"
#include "spy.h"
#include "../image_flux.h"
#include "infrastructure/param/param_st.h"

struct	KernelDeintBuffer
{
	INT32	_size_x;
	INT32	_size_byte;
	INT32	_size_y;
	INT32	_bytePitch;
	INT32	_pitch;
	INT32	_size;

	UINT8* unalignedBuffer;
	UINT8* buffer;

	KernelDeintBuffer( INT32 size_x, INT32 size_y ) : 
		_size_x( size_x ), _size_byte( size_x ), _size_y( size_y ), _size( 1 )
	{
		_bytePitch = _pitch = ( size_x + 15 ) & ( -16 );
		unalignedBuffer = new UINT8[_bytePitch * size_y + 15];
		buffer = ( UINT8* )( ( ( unsigned long ) unalignedBuffer + 15 ) & ( -16 ) );
	}

	KernelDeintBuffer( INT32 size_x, INT32 size_y, INT32 size ) : 
		_size_x( size_x ), _size_y( size_y ), _size( size )
	{
		_size_byte = size_x * size;
		_bytePitch = ( _size_byte + 15 ) & ( -16 );
		_pitch = _bytePitch / size;
		unalignedBuffer = new UINT8[_bytePitch * size_y + 15];
		buffer = ( UINT8* )( ( ( unsigned long ) unalignedBuffer + 15 ) & ( -16 ) );
	}

	virtual ~KernelDeintBuffer()
	{
		SAFE_DELETE_ARRAY( unalignedBuffer );
	}
};

FACTORY_CREATE_V1( c_deint_leak_kernel, deinterlace_leak_kernel, Deinterlace Leak Kernel, deinterlace_leak_kernel );

#define PITCH_WARNING "Pitches of fields differ - slower C++ implementation used."


#define COLORSPACE_YV12 1
#define COLORSPACE_YUY2 2
#define COLORSPACE_RGB32 3


#define KERNELDEINT_YV12
#define KERNELDEINT_YUY2
#define KERNELDEINT_RGB32

CONST __int64 qword_4354h = 0x4354435443544354i64;
CONST __int64 qword_15c2h = 0x15c215c215c215c2i64;
CONST __int64 qword_0ed9h = 0x0ed90ed90ed90ed9i64;
CONST __int64 qword_0354h = 0x0354035403540354i64;
CONST __int64 qword_03f8h = 0x03f803f803f803f8i64;

CONST __int64 qword_0fh = 0x0f0f0f0f0f0f0f0fi64;
CONST __int64 qword_14h = 0x1414141414141414i64;
CONST __int64 qword_10h = 0x1010101010101010i64;
CONST __int64 qword_140fh = 0x140f140f140f140fi64;

CONST __int64 qword_01h = 0x0101010101010101i64;
CONST __int64 qword_80h = 0x8080808080808080i64;

CONST __int64 qword_ff00h = 0xff00ff00ff00ff00i64;
CONST __int64 qword_000000ffh = 0x000000ff000000ffi64;
CONST __int64 qword_00ff0000h = 0x00ff000000ff0000i64;

enum {
	PLANAR_Y = 1 << 0,
	PLANAR_U = 1 << 1,
	PLANAR_V = 1 << 2,
	PLANAR_ALIGNED = 1 << 3,
	PLANAR_Y_ALIGNED = PLANAR_Y | PLANAR_ALIGNED,
	PLANAR_U_ALIGNED = PLANAR_U | PLANAR_ALIGNED,
	PLANAR_V_ALIGNED = PLANAR_V | PLANAR_ALIGNED,
};


namespace n_deint_leak_kernel
{
	static	CONST	INT32	BASE_PARAM_NB	= 6;
	static	CONST	INT32	GROUP_NB		= 0;

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( show_deinterlace )
		ST_PARAM_INT32( difference_threshold, 15., 10., 0., 255 )
		ST_PARAM_BOOL_OFF( sharp )
		ST_PARAM_BOOL_OFF( two_way )
		ST_PARAM_BOOL_OFF( field_order )
		ST_PARAM_BOOL_OFF( use_mmx )
	};
}

void	c_deint_leak_kernel::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, _b_show_deinterlace );
	param_set_pt( h, _threshold );
	param_set_pt( h, _b_sharp );
	param_set_pt( h, _b_two_way );
	param_set_pt( h, _field_order );
	param_set_pt( h, _b_use_mmx );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_deint_leak_kernel)
{
	_scratch = nullptr;
	_fullsizeMask = nullptr;
	_min_image_nb = 6;
	_src_n = nullptr;
	_src_n_1 = nullptr;
	_src_n_2 = nullptr;
	_src_n_3 = nullptr;

//	set_name( "deint_leak_kernel" );
	param_init_with( n_deint_leak_kernel::param, n_deint_leak_kernel::PARAM_NB_MAX );
}

BOOL	c_deint_leak_kernel::is_field_needed( BOOL b_double_framerate )
{
	if( b_double_framerate )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


void c_deint_leak_kernel::KernelDeint_RGB32( UINT8* src, UINT8* dst, BOOL second_pass )
{
	INT32 usedOrder;
	
	if( second_pass )	{	usedOrder = _field_order ^ ( ( _image_index ) & 1 );		}
	else				{	usedOrder = _field_order ^ ( ( _image_index - 1 ) & 1 );	}

	if( _threshold > 0 )
	{
		BuildMotionMask_RGB32( _fullsizeMask, usedOrder, _image_index, _threshold );
	}
	else if( !_b_masksFilled )
	{
		MEMSET( _fullsizeMask->buffer, 255, _fullsizeMask->_bytePitch * _fullsizeMask->_size_y );
		_b_masksFilled = true;
	}

	if( _b_show_deinterlace )
	{
//		PVideoFrame cur=child->GetFrame( n/2, env );

		MEMCPY( dst, _src_n_1, _size_x * _size_y * 4 );
//		CopyFrame( dst, cur, env );
//		ShowMotionMask_RGB32( dst, usedOrder, fullsizeMask );

		return;
	}

	Deinterlace_RGB32( dst, usedOrder, _image_index, _fullsizeMask, _b_sharp, _b_two_way );
}

void	c_deint_leak_kernel::BuildMotionMask_RGB32( KernelDeintMask* mask, INT32 order, INT32 n, INT32 threshold )
{
	if( _b_use_mmx )	{	BuildMotionMask_RGB32_MMX( mask, order, n, threshold);	}
	else				{	BuildMotionMask_RGB32_C( mask, order, n, threshold);	}
}


void	c_deint_leak_kernel::BuildMotionMask_RGB32_C( KernelDeintMask* mask, INT32 order, INT32 n, INT32 threshold )
{
	#define COLORSPACE COLORSPACE_RGB32
	#define PITCH_MOD 1

	#define MOTIONMASK_STAGE1 "./leak_kernel/MotionMaskLine_YUY2+RGB_1.cpp"
	#define MOTIONMASK_STAGE2 "./leak_kernel/MotionMaskLine_RGB32_2_C.cpp"

	#include "leak_kernel\MotionMask_YUY2+RGB_0.cpp"
}



void c_deint_leak_kernel::BuildMotionMask_RGB32_MMX( KernelDeintMask* mask, INT32 order, INT32 n, INT32 threshold )
{
	int pitch1 = _size_x * 4; //GetField(n+1,env)->GetPitch();
	int pitch2 = _size_x * 4; //GetField(n  ,env)->GetPitch();
	int pitch3 = _size_x * 4; //GetField(n-1,env)->GetPitch();
	int pitch4 = _size_x * 4; //GetField(n-2,env)->GetPitch();

	//if ((pitch1 != pitch2) ||
	//    (pitch2 != pitch3) ||
	//	(pitch3 != pitch4))
	//{
	//	//if (!printedPitchWarning)
	//	//{
	//	//	OutputDebugString(PITCH_WARNING);

	//	//	printedPitchWarning=true;
	//	//}

	//	BuildMotionMask_RGB32_C(mask,order,n,threshold);

	//	return;
	//}

	#define COLORSPACE COLORSPACE_RGB32
	#define PITCH_MOD 8

	#define MOTIONMASK_STAGE1 "leak_kernel/MotionMaskLine_YUY2+RGB_1.cpp"
	#define MOTIONMASK_STAGE2 "leak_kernel/MotionMaskLine_RGB32_2_MMX.cpp"

	#include "./leak_kernel/MotionMask_YUY2+RGB_0.cpp"
	//MotionMask_YUY2_RGB_0
	__asm emms

}


void	c_deint_leak_kernel::ShowMotionMask_RGB32( UINT8* p_dst, INT32 order, KernelDeintMask* mask )
{
	int maskPitch = mask->_bytePitch;
	UINT8* maskPtr = mask->buffer;

	int dstPitch = _size_x * 4 ;//dst->GetPitch();
	UINT8* dstPtr = p_dst + order * dstPitch; //dst->GetWritePtr() + order * dstPitch;

	int rowSize= dstPitch; //dst->GetRowSize();

	for( int rowsLeft = _size_y; rowsLeft > 0; --rowsLeft )
	{
		unsigned long* curMaskPtr=( unsigned long* )maskPtr;
		unsigned long* curDstPtr=( unsigned long* )dstPtr;

		for( int colsLeft = rowSize / 4; colsLeft > 0; --colsLeft )
		{
			*curDstPtr |= *curMaskPtr;
			curMaskPtr++; curDstPtr++;
		}
		maskPtr+=maskPitch;
		dstPtr+=2*dstPitch;
	}
}


void	c_deint_leak_kernel::Deinterlace_RGB32( UINT8* p_dst, INT32 order, INT32 n, KernelDeintMask* mask, BOOL sharp, BOOL twoway )
{
	if( _b_use_mmx )	{	Deinterlace_RGB32_MMX( p_dst, order, n , mask, sharp, twoway );	}
	else				{	Deinterlace_RGB32_C( p_dst, order, n, mask, sharp, twoway );	}
}


void	c_deint_leak_kernel::Deinterlace_RGB32_C( UINT8* dst, INT32 order, INT32 n, KernelDeintMask* mask, BOOL sharp, BOOL twoway )
{
	int plane=PLANAR_Y;

	#define COLORSPACE COLORSPACE_RGB32
	#define PITCH_MOD 1

	#define DEINTERLACE_STAGE1 "leak_kernel/DeinterlaceLine_1.cpp"
	#define DEINTERLACE_STAGE2 "leak_kernel/DeinterlaceLine_2_C.cpp"
	#define COPY_LINE          "leak_kernel/CopyLine_C.cpp"

	#include "leak_kernel/Deinterlace_0.cpp"
}


void c_deint_leak_kernel::Deinterlace_RGB32_MMX( UINT8* dst, INT32 order, INT32 n, KernelDeintMask* mask, BOOL sharp, BOOL twoway )
{
	int pitch1 = _size_x * 4; //GetField(n+1,env)->GetPitch();
	int pitch2 = _size_x * 4; //GetField(n  ,env)->GetPitch();
	int pitch3 = _size_x * 4; //GetField(n-1,env)->GetPitch();

	//if ((pitch1 != pitch2) ||
	//    (pitch2 != pitch3))
	//{
	//	//if (!printedPitchWarning)
	//	//{
	//	//	OutputDebugString(PITCH_WARNING);

	//	//	printedPitchWarning=true;
	//	//}

	//	Deinterlace_RGB32_C( p_dst, order, n, mask, sharp, twoway);

	//	return;
	//}

	int plane = 1 << 0;

	#define	COLORSPACE COLORSPACE_RGB32
	#define	PITCH_MOD 8

	#define	DEINTERLACE_STAGE1	"leak_kernel/DeinterlaceLine_1.cpp"
	#define	DEINTERLACE_STAGE2	"leak_kernel/DeinterlaceLine_2_MMX.cpp"
	#define	COPY_LINE			"leak_kernel/CopyLine_MMX.cpp"

#include "leak_kernel/Deinterlace_0.cpp"
//	Deinterlace_0

	__asm emms
}

void	c_deint_leak_kernel::close()
{
	SAFE_DELETE_ARRAY( _scratch );
	SAFE_DELETE_ARRAY( _fullsizeMask );
}

void	c_deint_leak_kernel::init( INT32 size_x, INT32 size_y )
{
	_scratchPitch= ( size_x * 4 + 15 ) & (-16);
	_scratch = new UINT8[_scratchPitch * 8 * 3];

	_fullsizeMask = new KernelDeintMask( size_x, size_y, 4 );

}

void	c_deint_leak_kernel::run_frame( UINT8* src, UINT8* dst )
{
	ERR_PRINT_STRING( "Leak Kernel is not implemented at half framerate, fallback to copy" );
	simple_copy( src, dst );
}

void	c_deint_leak_kernel::run_field( UINT8* src, UINT8* dst, BOOL second_pass )
{
	KernelDeint_RGB32( src, dst, second_pass );
}

void	c_deint_leak_kernel::run_full(  c_image_flux* image_flux, UINT8* dst, BOOL second_pass )
{
	c_img_with_lock*	img_n;
	c_img_with_lock*	img_n_1;
	c_img_with_lock*	img_n_2;
	c_img_with_lock*	img_n_3;

	//DBG_PRINT_STRING("second pass = %d", second_pass);
	//if( second_pass )
	//	DBG_PRINT_STRING("run full second pass");
	//else
	//	DBG_PRINT_STRING("run full first pass");

	INT32	deinterlace_image_index;

	_b_is_processed = FALSE;
	img_n_3 = img_n_2 = img_n_1 = img_n = NULL;
	_src_n_3 = _src_n_2 = _src_n_1 = _src_n = NULL;

	if( _image_index >= 5 )
	{
		// need 4 fields
		if( second_pass )	{	deinterlace_image_index = _image_index;		}
		else				{	deinterlace_image_index = _image_index - 1;	}

		img_n_3 = image_flux->lock_and_get_image_index( deinterlace_image_index - 3);
		img_n_2 = image_flux->lock_and_get_image_index( deinterlace_image_index - 2);
		img_n_1 = image_flux->lock_and_get_image_index( deinterlace_image_index - 1);
		img_n = image_flux->lock_and_get_image_index( deinterlace_image_index);
	}
	else
	{
		// do nothing	todonow
		return;
	}

	// need to check if both fields are available, one can be lock and not the other
	if( (!img_n) || (!img_n_1) || (!img_n_2) || (!img_n_3) )
	{
		if( img_n )		img_n->unlock();
		if( img_n_1 )	img_n_1->unlock();
		if( img_n_2 )	img_n_2->unlock();
		if( img_n_3 )	img_n_3->unlock();
		ERR_PRINT_STRING( "nothing ready in image_flux" );
		return;
	}
	else
	{
		_src_n	= img_n->get_data_uint8();
		_src_n_1	= img_n_1->get_data_uint8();
		_src_n_2	= img_n_2->get_data_uint8();
		_src_n_3	= img_n_3->get_data_uint8();

	//	dst = image_flux->get_data();
	//	DBG_PRINT_STRING("dst get data size %d", image_flux->get_data_size);

		if( _src_n && _src_n_1 && _src_n_2 && _src_n_3 )
		{
			_nb_bit_out = img_n->get_byte_per_pixel() * 8;

			TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 1., "video_process", this );
			
			// frame based deinterlacing
			run_field( _src_n_1, dst, second_pass );

			_size_out_y = 2 * _size_y;
		}
		img_n->unlock();
		img_n_1->unlock();
		img_n_2->unlock();
		img_n_3->unlock();
		//image_flux->release_data();
		//image_flux->release_bind();
		if( _src_n && _src_n_1 && _src_n_2 && _src_n_3 )
		{
			image_flux->clear_empty();	//todo set_empty() ? set_changed()?
			TBUF_ADD( tbuf::CH_VIDEO_PROCESS, 0., NULL, this );

			_b_is_processed = TRUE;

		}
	}

}

c_deint_leak_kernel::~c_deint_leak_kernel()
{
	close();
}