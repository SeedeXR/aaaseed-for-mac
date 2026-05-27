#include "face_tracker_dlib.h"
#include "file/aaa_file.h"

/*
	Finally, note that the face detector is fastest when compiled with at least
	SSE2 instructions enabled.  So if you are using a PC with an Intel or AMD
	chip then you should enable at least SSE2 instructions. Note that AVX is the fastest but requires a CPU from at least
	2011.  SSE4 is the next fastest and is supported by most current machines.  
*/

#include <dlib/opencv.h>
#include <dlib/image_processing/frontal_face_detector.h>
//#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include "dlib/revision.h"


using namespace dlib;
using namespace std;

// ----------------------------------------------------------------------------------------
/*
// This example takes in a shape model file and then a list of images to
		// process.  We will take these filenames in as command line arguments.
		// Dlib comes with example images in the examples/faces folder so give
		// those as arguments to this program.
		if (argc == 1)
		{
			cout << "Call this program like this:" << endl;
			cout << "./face_landmark_detection_ex shape_predictor_68_face_landmarks.dat faces/*.jpg" << endl;
			cout << "\nYou can get the shape_predictor_68_face_landmarks.dat file from:\n";
			cout << "http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
			return 0;
		}
*/
// ----------------------------------------------------------------------------------------

class c_face_engine_dlib final : public c_face_engine
{
	frontal_face_detector				_detector;
	std::vector<rectangle>				_faces;
	shape_predictor						_sp;
	std::vector<full_object_detection>	_shapes;
	o_str								_landmark_detection_filename;
protected:
	virtual void	deinit();
	virtual void	init( UINT32 model_nb, int argc = 0, char **argv = nullptr );
	virtual void	reset();
	virtual void	detect(			cv::UMat* img,	cv::Mat_< float >& img_depth );
	virtual void	find_landmark(	cv::UMat* img,	cv::Mat_< float >& img_depth );

	template< typename image_type >
			void	find_landmark_helper(	cv::UMat* img,	cv::Mat_< float >& img_depth );

	virtual void	store_landmark(	c_face_tracker::c_face_info& info, INT32 model_index, REAL size_x, REAL size_y );

	virtual	bool	is_float_forced()	{ return true; }
	virtual	bool	is_bgr_used()		{ return true; }
public:
			void	set_landmark_detection_filename( C_PCHAR_C fname );
};

void	c_face_engine_dlib::store_landmark( c_face_tracker::c_face_info& info, INT32 model_index, REAL size_x, REAL size_y )
{
	full_object_detection model = _shapes[model_index];

	int nb						= model.num_parts();
	if( ERR(info.set_point_nb( nb )) )
		return;

//	auto const &	rect			= model.get_rect();

	DOUBLE fx = DOUBLE(1) / size_x;	// or size_x-1
	DOUBLE fy = DOUBLE(1) / size_y;
	//INT32 x = 0;
	for( int i = 0; i < nb; ++i )
	{
		auto const & src =  model.part(i);
		//todo not here ?
		//because image inverted in y
		info.set_point_raw( i, DOUBLE(1) - fx * src.x(), DOUBLE(1) - fy * src.y() );
	//	if( INSIDE_MIN_MAX( i, 19, 40 ) )
	//		x = MAX( x, src.x() );
		info._b_visibility[i] = true;
		info._certainty = 2.;
	}
	//DBG_PRINT_STRING( "x max : %d", x ); 
//todonow
//	model.GetShape(  info._point_3d[0], size_x, size_y, -.5, -.5 );
	
}

void	c_face_engine_dlib::deinit()
{
	//todo	fill
}

//todo model nb should not interfere
void	c_face_engine_dlib::init( UINT32 model_nb, int argc, char **argv )
{
	GOOD_PRINT_STRING( "DLIB version is %d.%d", DLIB_MAJOR_VERSION, DLIB_MINOR_VERSION );

	// We need a face detector.  We will use this to get bounding boxes for
	// each face in an image.
	_detector = get_frontal_face_detector();
	GOOD_PRINT_STRING( "get_frontal_face_detector() Done" );
	
	// And we also need a shape_predictor.  This is the tool that will predict face
	// landmark positions given an image and face bounding box.  Here we are just
	// loading the model from the shape_predictor_68_face_landmarks.dat file you gave
	// as a command line argument.
	//deserialize( "Data/sp_o30_nu02_td5.dat" ) >> _sp;
	GOOD_PRINT_STRING( "shape_predictor is %.256s", _landmark_detection_filename.get() );
	if( c_file::is_exist(  _landmark_detection_filename ) )
	{
		deserialize( _landmark_detection_filename.get() ) >> _sp;
		GOOD_PRINT_STRING( "shape_predictor deserialized" );
		//todo load faster eventually by saving ourself
		_model_nb = model_nb;
	}
	else
	{
		ERR_PRINT_STRING( "can't deserialize non existing shape_predictor \"%s\"", _landmark_detection_filename.get() );
		_model_nb = 0;
	}
}

void	c_face_engine_dlib::detect( cv::UMat* img, cv::Mat_< float >& img_depth )
{
	_detector.set_max_pyramid_levels( _face_tracker->_detect_pyramid_level_max_ui );
	// Detect faces 
	// Turn OpenCV's Mat into something dlib can deal with.  Note that this just
	// wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
	// long as img is valid.  Also don't do anything to img that would cause it
	// to reallocate the memory which stores the image as that will make cimg
	// contain dangling pointers.  This basically means you shouldn't modify img
	// while using dlib_img.
	INT32 ch_nb = img->channels();
	if( ch_nb == 1 )
	{
		cv_image<unsigned char> dlib_img( img->getMat( cv::ACCESS_READ ) );
		_faces = _detector( dlib_img );
	}
	else
	{
		cv_image<bgr_pixel> dlib_img( img->getMat( cv::ACCESS_READ ) );
		_faces = _detector( dlib_img );
	}
	_face_tracker->_face_detected_nb = _faces.size();
}

//extern INT32 extend_pixel;
FINLINE rectangle get_rect( rectangle& r, REAL ratio )
{
	INT32 extend_pixel = 0;	
	return rectangle(	(long)( (r.left()	-extend_pixel)	* ratio ),
						(long)( (r.top()	-extend_pixel)	* ratio ),
						(long)( (r.right()	+extend_pixel)	* ratio ),
						(long)( (r.bottom()	+extend_pixel)	* ratio )	);
}

template< typename image_type >
void c_face_engine_dlib::find_landmark_helper( cv::UMat* img, cv::Mat_< float >& img_depth )
{
	_shapes.clear();
	_face_tracker->_active_models.clear();

	REAL ratio = DOUBLE(img->cols) / _face_tracker->_detect_sx;
	cv_image<image_type> dlib_img( img->getMat( cv::ACCESS_READ ) );
	// Find the pose of each face.
	for( unsigned long i = 0; i < _faces.size(); ++i )
	{
		// Resize obtained rectangle for full resolution image. 
		_shapes.push_back( _sp( dlib_img, get_rect( _faces[i], ratio ) ) );
	}
	for( unsigned long i = 0; i < _faces.size(); ++i )
	{
		_face_tracker->_active_models.push_back( true );
		auto& fi = _face_tracker->_face_infos[i];
		fi._b_active = true;
	}
}

void	c_face_engine_dlib::find_landmark( cv::UMat* img, cv::Mat_< float >& img_depth )
{
	INT32 ch_nb = img->channels();

	if( ch_nb == 1 )
		find_landmark_helper<unsigned char>( img, img_depth );
	else
		find_landmark_helper<bgr_pixel>( img, img_depth );

}

void	c_face_engine_dlib::reset()
{
}

void	c_face_engine_dlib::set_landmark_detection_filename( C_PCHAR_C fname )
{
	_landmark_detection_filename.set( fname );
}

 c_face_engine* c_face_tracker::get_dlib( C_PCHAR_C landmark_detection_fname )
 {
	c_face_engine_dlib* fe = new c_face_engine_dlib;
	C_PCHAR fname = landmark_detection_fname;
	fname = (fname && *fname != 0) ? fname : "Data/shape_predictor_68_face_landmarks.dat";
	fe->set_landmark_detection_filename( fname );
	return (c_face_engine*) fe;
 }

