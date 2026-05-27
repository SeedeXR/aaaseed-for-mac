#include "bdd_img_detect_face.h"
#include "obj_ui/bdd/bdd_ui_pref.h"
#include "opencv2/objdetect/objdetect.hpp"
#include "spy.h"

c_bdd_img_detect_face*	c_bdd_img_detect_face::cur = nullptr;

static	INT32	contour_mode[4] =
{
	CV_RETR_EXTERNAL,	//	only the extreme outer contours
	CV_RETR_LIST,		//	all the contours and puts them in the list
	CV_RETR_CCOMP,		//	all the contours and organizes them into two-level hierarchy: top level are external boundaries of the components, second level are bounda boundaries of the holes
	CV_RETR_TREE		//	all the contours and reconstructs the full hierarchy of nested contours 
};

FACTORY_CREATE_PROP_V1( c_bdd_img_detect_face, bdd_img_detect_face, Image Detect Face, bdd_img_detect_face, sub_menu="Image"; );


namespace n_bdd_img_detect_face
{
	C_PCHAR_C	contour_mode_str[4] =
	{
		"External",
		"All",
		"All_then_two_level",
		"All_then_full_hierarchy"
	};

	static	CONST	INT32	BASE_PARAM_NB			= 2 + c_bdd_img::SUPER_PARAM_NB;
	//	static	CONST	INT32	CONTOUR_PARAM_NB	= 14;
	static	CONST	INT32	GROUP_PARAM_NB			= 0;

	static	CONST	INT32	PARAM_NB		=	BASE_PARAM_NB
		//											+	CONTOUR_PARAM_NB
		+	GROUP_PARAM_NB;


	static	C_PARAM_DEF	param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		ST_PARAM_NONE( Bdd_ui_pref )

		ST_PARAM_INT32_POS_ZERO( face_detect_image_dst )
	};
}

void	c_bdd_img_detect_face::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( FALSE, FALSE );

	param_attach_obj( h, c_bdd_ui_pref::cur );
	param_set_pt( h, _dst_index );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_detect_face ),
_classifier(nullptr),
_face_storage(nullptr),
_src(nullptr),
_dst(nullptr),
_b_haar_open(FALSE)
{
	param_init_with( n_bdd_img_detect_face::param, n_bdd_img_detect_face::PARAM_NB );
}

void	c_bdd_img_detect_face::release()
{
	if( this )
	{
		_dst.release();	
	}
}

c_bdd_img_detect_face::~c_bdd_img_detect_face()
{
	if( cur == this )	cur = nullptr;
	release();
	close_haar();
}

void	c_bdd_img_detect_face::do_process()
{
	TBUF_ADD( tbuf::CH_OPEN_CV, 1., "bdd_img_detect_face", this );

	c_img_ipl*	ipl_mono = get_in_mono_to_use();

	if( ipl_mono )
	{
		if( _dst.build( _dst_index, *ipl_mono, "Face Detection Image", 3 ) )
		{
			do_haar( ipl_mono, &_dst );
			_dst.set_changed();
		}
	}
	TBUF_ADD( tbuf::CH_OPEN_CV, 1., nullptr, this );
}

void faceDetection( IplImage* src,
					IplImage* dst,
					CvMemStorage* storage, 
					CvHaarClassifierCascade* classifier )
{
	// "Resets" the memory but does not deallocate it.
	cvClearMemStorage( storage );

	// Run the main object recognition function.  The arguments are: 
	// 1. the image to use
	// 2. the pre-trained Haar classifier cascade data
	// 3. memory storage for rectangles around recognized objects
	// 4. a scale factor "by which the search window is scaled between the 
	//    subsequent scans, for example, 1.1 means increasing window by 10%"
	// 5. the "minimum number (minus 1) of neighbor rectangles that makes up 
	//    an object. All the groups of a smaller number of rectangles than 
	//    min_neighbors-1 are rejected. If min_neighbors is 0, the function 
	//    does not any grouping at all and returns all the detected candidate 
	//    rectangles, which may be useful if the user wants to apply a 
	//    customized grouping procedure."
	// 6. flags which determine the mode of operation
	// 7. the minimum object size (if possible, increasing this will 
	//    really speed up the process)
	CvSeq* faces = cvHaarDetectObjects( src, classifier, storage, 1.1, 
										2, CV_HAAR_DO_CANNY_PRUNING, cvSize(50, 50));

	// If any faces were detected, draw rectangles around them.
	if( faces )
	{
		for( INT32 i = 0; i < faces->total; ++i )
		{
			// Setup two points that define the extremes of the rectangle, 
			// then draw it to the image..
			CvPoint point1, point2;
			CvRect* rectangle = (CvRect*)cvGetSeqElem(faces, i);
			point1.x = rectangle->x;
			point2.x = (rectangle->x + rectangle->width);
			point1.y = rectangle->y;
			point2.y = (rectangle->y + rectangle->height);
			cvRectangle( dst, point1, point2, CV_RGB(255,2,0), 3, 8, 0);
		}
	}
}


void	c_bdd_img_detect_face::open_haar()
{
	// Load the pre-trained Haar classifier data.
	_classifier = (CvHaarClassifierCascade*)cvLoad( "CV/HAARCASCADE_FRONTALFACE_DEF.XML", 0, 0, 0);
	if( !_classifier )
	{
		ERR_PRINT_STRING( "%s() Invalid classifier data", __FUNCTION__);
	}

	// Create a CvMemStorage object for use by the face detection function.
	_face_storage = cvCreateMemStorage(0);

	_b_haar_open = TRUE;
}


void	c_bdd_img_detect_face::close_haar()
{
	// Clean up allocated OpenCV objects.
	cvReleaseMemStorage( &_face_storage );
//	if (tempFrame)
//	{
//		cvReleaseImage(&tempFrame);
//	}
}


void	c_bdd_img_detect_face::do_haar( c_img_ipl* src, c_img_ipl* dst )
{
	if( !check_src_dst( src, dst ) )
		return;
	// Copy the current frame into the temporary image.  Also, make 
	// sure the images have the same orientation.
	if( src->get_ipl()->origin == IPL_ORIGIN_TL )
		cvCopy( src->get_ipl(), dst->get_ipl(), 0);
	else
		cvFlip( src->get_ipl(), dst->get_ipl(), 0);
	cvEqualizeHist( src, src );
	// Perform face detection on the temporary image, adding a rectangle 
	// around the detected face.
	faceDetection( src->get_ipl(), dst->get_ipl(), _face_storage, _classifier );
}


void	c_bdd_img_detect_face::update()
{	
	if( _b_active && !_b_haar_open )
		open_haar();
	if( update_img_src() )
	{
		if( is_size_changed() )
			release();
		if( is_content_changed() )
			do_process();
	}
}

void	c_bdd_img_detect_face::draw()
{	
	if( _b_active )
	{
	}
}
