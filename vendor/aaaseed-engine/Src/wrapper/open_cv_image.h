
#ifdef AAA_OPEN_CV_IMAGE_H
#error "OPEN_CV_IMAGE_H included more than once."
#endif
#define AAA_OPEN_CV_IMAGE_H 1


#ifndef AAA_AAA_OPENCV_H
#	include "aaa_opencv.h"
#endif
#ifndef	AAA_IMG_H
#	include "image/img.h"
#endif
#if CV_VERSION_MAJOR >= 4
#	ifndef OPENCV_CORE_TYPES_H
#		include "opencv2\core\types_c.h"
#	endif
#	ifndef OPENCV_CORE_C_H
#		include "opencv2\core\core_c.h"
#	endif
#endif

namespace ipl
{
	typedef IplImage* p_ipl;
	// This class serves as a wrapper to OpenCV's image class
	// to provide a simple interface to pixels and height/width info.
	 
			void		to_gray( p_ipl CONST src, p_ipl CONST dst, aaa::COMPO CONST s_compo );
			void		release( p_ipl& p );

	//todo redo gray image

			void		create_print(	p_ipl CONST p_ipl, C_PCHAR_C str );
			bool		create(			p_ipl& dst, p_ipl CONST ref, INT32 CONST depth, INT32 CONST channel_nb, C_PCHAR_C mess  );
			bool		create_gray(	p_ipl& dst, p_ipl CONST ref, INT32 CONST depth, C_PCHAR_C mess );
			bool		create(			p_ipl& dst, p_ipl CONST ref, C_PCHAR_C mess );

			c_img_2d*	to_aaa(			p_ipl CONST src, INT32 CONST image_dst_index );
			bool		cv_smooth(		IplImage CONST * CONST src, IplImage* CONST dst, INT32 CONST smooth_method, INT32 CONST size_x, INT32 CONST size_y );

	typedef struct	{ unsigned char b,g,r; }	pixel_rgb;
	typedef struct	{ float b,g,r; }			pixel_rgb_float;
	typedef unsigned char						pixel_gray;
	typedef	float								pixel_gray_float;

	//	typedef c_ipl_img<pixel_rgb>				image_rgb;
	//	typedef c_ipl_img<pixel_rgb_float>			image_rgb_float;
	//	typedef c_ipl_img<pixel_gray>				image_gray;
	//	typedef c_ipl_img<pixel_gray_float>			image_gray_float;
}

class c_img_ipl
{

private:
	ipl::p_ipl	_ipl;
	bool		_b_owner;
	bool		_b_only_header;
	INT32		_bind_index;
public:
	c_img_ipl( ipl::p_ipl ipl = nullptr )	{	_ipl = ipl;	_b_owner=false;	_b_only_header=false; _bind_index=-1; }
	~c_img_ipl()							{	release();		}

	void				release();

	ipl::p_ipl			get_ipl()			CONST						{	return this ? _ipl : nullptr ;	}
	INT32				get_size_x()		CONST						{	return _ipl->width ;			}
	INT32				get_size_y()		CONST						{	return _ipl->height ;			}
	INT32				get_depth()			CONST						{	return _ipl->depth ;			}
	INT32				get_channel_nb()	CONST						{	return _ipl->nChannels ;		}

	bool				is_equal_size(		INT32 x, INT32 y )	CONST	{	return x==_ipl->width && y==_ipl->height; }
//	bool				is_equal_channel(	INT32 nb, INT32 depth )		{	return nb==_ipl->nChannels && depth==_ipl->depth; }

	//		void operator=( p_image img )	{ _p_ipl = img;	}

	void				set_changed();

	bool				build_using( c_img_2d CONST * CONST src, bool& b_changed );
	bool				create( c_img_ipl& ref, INT32 CONST depth, INT32 CONST channel_nb, C_PCHAR_C mess  );
	bool				build( INT32 CONST img_index, c_img_ipl& ref, C_PCHAR_C mess, INT32 CONST channel_nb );
	bool				build( INT32 CONST img_index, c_img_ipl& ref, C_PCHAR_C mess );

	FINLINE	bool		create_gray( c_img_ipl& ref, INT32 CONST depth, C_PCHAR_C mess )	{	return	create( ref, depth, 1, mess  );	}
	FINLINE	bool		create( c_img_ipl& ref, C_PCHAR_C mess )							{	return	create( ref, ref.get_depth(), ref.get_channel_nb(), mess );	}
	FINLINE	void		to_gray( c_img_ipl& dst, aaa::COMPO CONST s_compo )					{	ipl::to_gray( get_ipl(), dst.get_ipl(), s_compo );	}
	FINLINE	c_img_2d*	to_aaa( INT32 CONST image_dst_index )								{	ipl::to_aaa( get_ipl(), image_dst_index );	}

	template<class T>
	FINLINE T get_pixel( INT32 CONST x, INT32 CONST y ) CONST
	{
		return CV_IMAGE_ELEM( _ipl, T, y, x );
	}
	template<class T>
	FINLINE	void set_pixel( INT32 CONST x, INT32 CONST y, T value ) 
	{
		CV_IMAGE_ELEM( _ipl, T, y, x ) = value;
	}
	//hack not recommended at all ( done for memanence_maa_max )
	UINT8*	get_data_pt_hack()			{	return (UINT8*)(_ipl->imageData);	}
	FP32*	get_data_float_pt_hack()	{	return (FP32*)(_ipl->imageData);	}

	/*		template<class T>
	FINLINE T* operator[]( INT32 CONST rowIndx ) 
	{
	//return CV_IMAGE_ELEM( imgp, T, rowIndx, 0 );
	return (T *) ( _p_ipl->imageData + rowIndx * _p_img->widthStep ); 
	}
	*/
};
