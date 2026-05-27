#define WIN32_LEAN_AND_MEAN	//avoid error in include with MS 2017 C++ (problem with std::byte)

#include "face_tracker_clm.h"
#include "infrastructure/compute_parallel.h"
#ifndef AAA_CLM_PARAM_H
#	include <CLMParameters.h>
#endif
#ifndef AAA_CLM_UTILS_H
#	include <CLM_utils.h>
#endif
#include <CLMTracker.h>
#include "file/aaa_file.h"
#include "math/aaa_math.h"


class c_face_engine_clm final : public c_face_engine
{
	std::vector< cv::Rect_< double > >	_face_detections;
protected:
	virtual void	deinit();
	virtual void	init( UINT32 model_nb, int argc = 0, char **argv = nullptr );
	virtual void	reset();
	virtual void	detect(			cv::UMat* img,	cv::Mat_< float >& img_depth );
	virtual void	find_landmark(	cv::UMat* img,	cv::Mat_< float >& img_depth );

	virtual void	store_landmark(	c_face_tracker::c_face_info& info, INT32 model_index, REAL size_x, REAL size_y );

	virtual	bool	is_grey_forced()		{ return true; }
	virtual	bool	is_grey_used()			{ return true; }
	virtual bool	is_point_3d_produced()	{ return true; }

	vector< CLMTracker::CLM >			_clm_models;
	vector< CLMTracker::CLMParameters > _clm_parameters;
};

namespace
{
	vector<string> get_arguments( int argc, char **argv )
	{
		vector<string> arguments;
		for(int i = 0; i < argc; ++i)
		{
			arguments.push_back(string(argv[i]));
		}
		return arguments;
	}

	void eliminate_detection_overlaping( const vector<CLMTracker::CLM>& clm_models, vector<cv::Rect_<double> >& face_detections )
	{
		// Go over the model and eliminate detections that are not informative (there already is a tracker there)

		for( size_t model = 0; model < clm_models.size(); ++model )
		{
			// See if the detections intersect
			cv::Rect_<double> model_rect = clm_models[model].GetBoundingBox();
		
			for( int detection = face_detections.size()-1; detection >= 0; --detection )
			{
				double intersection_area = (model_rect & face_detections[detection]).area();
				double union_area = model_rect.area() + face_detections[detection].area() - 2 * intersection_area;

				// If the model is already tracking what we're detecting ignore the detection, this is determined by amount of overlap
				if( intersection_area/union_area > 0.5 )
				{
					face_detections.erase( face_detections.begin() + detection );
				}
			}
		}
	}
}

void	c_face_engine_clm::store_landmark( c_face_tracker::c_face_info& info, INT32 model_index, REAL size_x, REAL size_y )
{
	CLMTracker::CLM& model = _clm_models[model_index];

	auto const &	shape2D			= model.detected_landmarks;

	int nb = shape2D.rows / 2;
	if( ERR(info.set_point_nb(nb)) )
		return;

	//should be model.patch_experts.visibilities[0][idx] ???
	auto const &	visibilities	= model.patch_experts.visibilities[0][0];

	REAL fx = 1./size_x;
	REAL fy = 1./size_y;
	for( int i = 0; i < nb; ++i )
	{
		//because image inverted in y
		info.set_point_raw( i, 1. -	shape2D.at<double>(i) *	fx, 1. - shape2D.at<double>(i + nb)	* fy );
		if( visibilities.at<int>(i) )
			info._b_visibility[i] = true;
		else
			info._b_visibility[i] = true; //hack
	}

	model.GetShape( info._point_3d[0], size_x, size_y, -.5, -.5 );
}

void	c_face_engine_clm::deinit()
{
	// Reset the model, for the next video
	for( size_t model=0; model < _clm_models.size(); ++model )
	{
		_clm_models[model].Reset();
	}
	_clm_models.	clear();
	_clm_parameters.clear();
	_model_nb = 0;
}

void	c_face_engine_clm::init( UINT32 model_nb, int argc, char **argv )
{
	vector<string> arguments = get_arguments( argc, argv );

	CLMTracker::CLMParameters clm_params( arguments );
	clm_params.use_face_template = true;
	// This is so that the model would not try re-initializing itself
	clm_params.reinit_video_every = -1;
	clm_params.curr_face_detector = CLMTracker::CLMParameters::HOG_SVM_DETECTOR;

//do our init here

//	clm_params.num_optimisation_iteration = 2;

	//clm_parameters.push_back( clm_params );	

	// Get the input output file parameters
	//bool use_camera_plane_pose;
	//CLMTracker::get_video_input_output_params(files, depth_directories, pose_output_files, tracked_videos_output, landmark_output_files, landmark_3D_output_files, use_camera_plane_pose, arguments);
	// Get camera parameters
	//CLMTracker::get_camera_params(device, fx, fy, cx, cy, arguments);    

	// The modules that are being used for tracking

	//int num_faces_max = 3;
	if( c_file::is_exist(  clm_params.model_location.c_str() ) )
	{
		CLMTracker::CLM clm_model(			clm_params.model_location			);
		clm_model.face_detector_HAAR.load(	clm_params.face_detector_location	);
		clm_model.face_detector_location =	clm_params.face_detector_location;

		_clm_models.		reserve( model_nb );
		_clm_parameters.	reserve( model_nb );

		for( UINT32 i = 0; i < model_nb; ++i )
		{
			_clm_models.		push_back( clm_model	);
			_clm_parameters.	push_back( clm_params	);
		}
		_model_nb = model_nb;
	}
	else
	{
		ERR_PRINT_STRING( "%s() can't read non existing \"%s\"", __FUNCTION__, clm_params.model_location.c_str() );
		_model_nb = 0;
	}
}

void	c_face_engine_clm::detect( cv::UMat* img, cv::Mat_< float >& img_depth )
{
	_face_detections.clear();

	// Get the detections (every 8th frame and when there are free models available for tracking)
	if( !_face_tracker->is_model_active_all() )
	{				
		cv::Mat mat_img = img->getMat( cv::ACCESS_READ );
		if( _clm_parameters[0].curr_face_detector == CLMTracker::CLMParameters::HOG_SVM_DETECTOR )
		{
			vector<double> confidences;
			CLMTracker::DetectFacesHOG( _face_detections, mat_img, _clm_models[0].face_detector_HOG, confidences );				
		}
		else
		{
			CLMTracker::DetectFaces( _face_detections, mat_img, _clm_models[0].face_detector_HAAR );
		}
		INT32 nb = _face_detections.size();
		_face_tracker->_face_detected_nb = nb;
		if( nb > 0 )
		{
			// Keep only non overlapping detections (also convert to a concurrent vector
			eliminate_detection_overlaping( _clm_models, _face_detections );
		}
	}
}

void	c_face_engine_clm::find_landmark( cv::UMat* img,	cv::Mat_< float >& img_depth )
{
	cv::Mat mat_img = img->getMat( cv::ACCESS_READ );
	REAL ratio = DOUBLE(img->cols) / _face_tracker->_detect_sx;
//	vector< parallel_lib::atomic<bool> > face_detections_used( _face_detections.size() );
	vector< std::atomic<bool> > face_detections_used( _face_detections.size() );
	// Go through every model and update the tracking TODO pull out as a separate parallel/non-parallel method
	PARALLEL_LIB::parallel_for( 0, (int)_clm_models.size(), [&](int CONST model) NOEXCEPT
	//for(unsigned int model = 0; model < _clm_models.size(); ++model)
		{
			bool detection_success = false;

			// If the current model has failed more than 4 times in a row, remove it
			if( _clm_models[model].failures_in_a_row > _face_tracker->_failures_max_ui )
			{				
				_face_tracker->_active_models[model] = false;
				_clm_models[model].Reset(); 
			}

			// If the model is inactive reactivate it with new detections
			if( !_face_tracker->_active_models[model] )
			{
				for( size_t detection_ind = 0; detection_ind < _face_detections.size(); ++detection_ind )
				{
					// if it was not taken by another tracker take it (if it is false swap it to true and enter detection, this makes it parallel safe)
					bool strange = true;
					if( face_detections_used[detection_ind].compare_exchange_weak( strange, false ) == false )
//					if( face_detections_used[detection_ind].compare_and_swap( true, false ) == false )
					{
						// Reinitialize the model
						_clm_models[model].Reset();

						// This ensures that a wider window is used for the initial landmark localization
						//done in reset already
						//clm_models[model].detection_success = false;
						SPY_PUSH_RANGE( "Detect & LandMark", spy::COL_1 );
							cv::Rect_< double > r = _face_detections[detection_ind];
							r.x *= ratio;
							r.y *= ratio;
							r.width *= ratio;
							r.height *= ratio;
							detection_success = CLMTracker::DetectLandmarksInVideo( mat_img, img_depth, r, _clm_models[model], _clm_parameters[model] );
						SPY_POP_RANGE();

						// This activates the model
						_face_tracker->_active_models[model] = true;

						// break out of the loop as the tracker has been reinitialized
						break;
					}
				}
			}
			else
			{
				// The actual facial landmark detection / tracking
				SPY_PUSH_RANGE( "LandMark Only", spy::COL_2 );
					detection_success = CLMTracker::DetectLandmarksInVideo( mat_img, img_depth, _clm_models[model], _clm_parameters[model] );
				SPY_POP_RANGE();
			}
		}
	);

	REAL	sx = _face_tracker->_detect_sx;
	REAL	sy = _face_tracker->_detect_sy;
	REAL	cx = sx / 2.0f;
	REAL	cy = sy / 2.0f;

	REAL	img_sx = img->cols;
	REAL	img_sy = img->rows;
	REAL	img_cx = img_sx / 2.0f;
	REAL	img_cy = img_sy / 2.0f;

	// Go through every model and visualize the results
	for( size_t model = 0; model < _clm_models.size(); ++model )
	{
		auto& fi		= _face_tracker->_face_infos[model];
		auto& clm_model	= _clm_models[model];

		fi._b_active = true;
		// Visualizing the results
		// Drawing the facial landmarks on the face and the bounding box around it if tracking is successful and initialised
		double detection_certainty = clm_model.detection_certainty;
		fi._certainty = 1. - detection_certainty;

		double visualisation_boundary = -0.1;
		
		cv::Vec6d& v6 = clm_model.params_global;
		// global param scale, euler_x, euler_y, euler_z, tx, ty]
		DOUBLE z = 1.0 / v6[0];
		DOUBLE x = (v6[4] / sx - .5) * z;
		DOUBLE y = (v6[5] / sy - .5) * z;

		set_v3( fi._tra, x, y, z );
		set_v3( fi._rot, RAD_TO_TURN(v6[1]), RAD_TO_TURN(v6[2]), RAD_TO_TURN(v6[3]) );

		// Only draw if the reliability is reasonable, the value is slightly ad-hoc
		if( detection_certainty < visualisation_boundary )
		{
			//CLMTracker::Draw(disp_image, clm_models[model]);
			if( _face_tracker->_b_cv_draw )
			{
				if( _face_tracker->_b_cv_draw_tracker )
					//CLMTracker::Draw( _grayscale_image, _clm_models[model], float(1), false );
					CLMTracker::Draw( mat_img, _clm_models[model] );

				if( _face_tracker->_b_cv_draw_box )
				{
					if( detection_certainty > 1 )		detection_certainty = 1;
					else if( detection_certainty < -1 )	detection_certainty = -1;

					detection_certainty = (detection_certainty + 1) / (visualisation_boundary + 1);

					// A rough heuristic for box around the face width
					//int thickness = (int)std::ceil(2.0* ((double)captured_image.cols) / 640.0);
					int thickness = 4;
					
					// Work out the pose of the head from the tracked model
					cv::Vec6d pose_estimate_CLM = CLMTracker::GetCorrectedPoseWorld( _clm_models[model], sx, sy, cx, cy );
					//_cam_pos[model] = CLMTracker::GetPoseCamera( _clm_models[model], sx, sy, cx, cy, _clm_parameters[model] );
					
					// Draw it in black if uncertain, white if certain
					REAL	grey = (1.-detection_certainty)*.5*255.0;
					//draw
					CLMTracker::DrawBox( mat_img, pose_estimate_CLM, cv::Scalar(grey,grey,grey), thickness, img_sx, img_sy, img_cx, img_cy );
				}
			}
		}
	}
}

void	c_face_engine_clm::reset()
{
	for( UINT32 model = 0; model < _clm_models.size(); ++model )
	{
		_clm_models[model].Reset();
		_face_tracker->_active_models[model] = false;
	}
}

 c_face_engine* c_face_tracker::get_clm()
 {
	 return new c_face_engine_clm;
 }


