#include "capture_kinect.h"
#include "system/shared/SystemUtils.h"
#include "wrap_kinect2.h"
		 
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
		 
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
		 
#include "obj_ui/tracker/trackers.h"
#include "spy.h"


namespace
{
//only one sensor in sdk for now so static (or anonymous namespace) make sense
	IKinectSensor*			k2_sensor			= nullptr;
	IDepthFrameReader*		depth_frame_reader	= nullptr;
	IColorFrameReader*		rgb_frame_reader	= nullptr;
	IBodyFrameReader*		skel_frame_reader	= nullptr;
	UINT8					rgba_frame[ 1920 * 1080 * 4 ];

	CONST CHAR	KINECT2_HEADER[] = "# KINECT 2 ";
	void	KINECT2_PRINT_STRING( C_PCHAR_C fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		trackers::PRINT_STRING_VA( KINECT2_HEADER, fmt, args );
		va_end(args);
	}
	AAA_ERR error( C_PCHAR_C str )
	{
		KINECT2_PRINT_STRING( str );
		return ERR_ANY;
	}
	template<class T>
	AAA_ERR error_and_release( C_PCHAR_C str, T p )
	{
		KINECT2_PRINT_STRING( str );
		if( p )
			sysutils::safe_release( p );
		return ERR_ANY;
	}

	INT32 CONST BODY_NB = 6;
	IBody* pp_body[BODY_NB] = {0};
}

bool	c_capture_kinect::k2_is_lib_loaded()
{
	if( !dll_k2.is_loaded() )
	{
		// wrapping the "kinect20.dll"
		return dll_k2.init();
	}
	return true;
}



AAA_ERR c_capture_kinect::k2_start_rgb()
{
	//get the rgb reader
	IColorFrameSource* rgb_src = nullptr;
	HRESULT hr = k2_sensor->get_ColorFrameSource( &rgb_src );
	if( FAILED(hr) )
		return error_and_release( "Error get rgb Source",		rgb_src );

	hr = rgb_src->OpenReader( &rgb_frame_reader );
	if( FAILED(hr) )
		return error_and_release( "Error opening color reader",	rgb_src );

	BOOLEAN  active;
	hr = rgb_src->get_IsActive( &active );
	if( FAILED(hr) )
		return error_and_release( "Error testing color reader",	rgb_src );
	if( active==0 )
		return error_and_release( "color stream inactive",		rgb_src );

	sysutils::safe_release( rgb_src );

	//hack remove during capsule
	//set_src_y_inverted(	true );

	SAFE_DELETE( _img_flux_rgb );

	_img_flux_rgb = new c_image_flux( nullptr, true );	//todo	we didn't build the non NULL case for first arg
	_img_flux_rgb->set_use_last_frame( true );
	_img_flux_rgb->set_flux_size_format( 1920, 1080, aaa::PIXEL_FORMAT::RGBA_8 );
	_img_flux_rgb->set_image_nb_to_keep( 4 );
	_img_flux_rgb->set_src_y_inverted( true );
	_img_flux_rgb->set_swap_red_blue( false );

	//SAFE_DELETE( _img_flux_rgb_aligned );
	//_img_flux_rgb_aligned = new c_image_flux( nullptr, true );
	//_img_flux_rgb_aligned->set_use_last_frame( true );
	//_img_flux_rgb_aligned->set_flux_size_format( 640, 480, aaa::PIXEL_FORMAT::RGBA_8 );
	//_img_flux_rgb_aligned->set_image_nb_to_keep( 4 );
	//_img_flux_rgb_aligned->set_src_y_inverted( true );
	////?	_img_flux_rgb->set_swap_red_blue( true );

	return AAA_OK;
}

AAA_ERR c_capture_kinect::k2_start_depth()
{
	//get the depth reader
	IDepthFrameSource* depth_src = nullptr;
	HRESULT hr = k2_sensor->get_DepthFrameSource( &depth_src );
	if( FAILED(hr) )
		return error_and_release( "Error get depth Source",		depth_src );

	hr = depth_src->OpenReader( &depth_frame_reader );
	if( FAILED(hr) )
		return error_and_release( "Error opening depth reader",	depth_src );

	BOOLEAN  active;
	hr = depth_src->get_IsActive( &active );
	if( FAILED(hr) )
		return error_and_release( "Error testing depth reader",	depth_src );
	if( active==0 )
		return error_and_release( "depth stream inactive",		depth_src );

	sysutils::safe_release( depth_src );

	bool b_ok = set_depth_buf_size( 512, 424, aaa::PIXEL_FORMAT::R_32FP );

	_depth_deform_bind = -42;
	return b_ok ? AAA_OK : ERR_ANY;
}

AAA_ERR c_capture_kinect::k2_start_skel()
{
	//get the body reader
	IBodyFrameSource* body_src = nullptr;
	HRESULT hr = k2_sensor->get_BodyFrameSource( &body_src );
	if( FAILED(hr) )
		return error_and_release( "Error get body Source",		body_src );

	hr = body_src->OpenReader( &skel_frame_reader );
	if( FAILED(hr) )
		return error_and_release( "Error opening body reader",	body_src );

	BOOLEAN  active;
	hr = body_src->get_IsActive( &active );
	if( FAILED(hr) )
		return error_and_release( "Error testing body reader",	body_src );
	if( active==0 )	
		return error_and_release( "body stream inactive",		body_src );

	sysutils::safe_release( body_src );
	return AAA_OK;
}

void c_capture_kinect::k2_stop_depth()
{
	if( depth_frame_reader )
	{
		_ui->set_depth( false );
		sysutils::safe_release( depth_frame_reader );
	}
}
void c_capture_kinect::k2_stop_rgb()
{
	if( rgb_frame_reader )
	{
		_ui->set_rgb( false );
		_ui->set_rgb_aligned( false );
		sysutils::safe_release( rgb_frame_reader );
	}
}
void c_capture_kinect::k2_stop_skel()
{
	if( skel_frame_reader )
	{
		_ui->_b_skeleton = false;
		sysutils::safe_release( skel_frame_reader );
	}
	for( INT32 i = 0; i < BODY_NB; ++i )
		sysutils::safe_release( pp_body[i] );
}
void c_capture_kinect::k2_stop()
{
	k2_stop_depth();
	k2_stop_rgb();
	k2_stop_skel();
}
AAA_ERR c_capture_kinect::k2_run()
{
	if( !k2_sensor )
		return ERR_ANY;

	AAA_ERR err = AAA_OK;
	if( _ui->_b_depth_asked_ui )
		k2_start_depth();
	if( _ui->_b_rgb_asked_ui )
		k2_start_rgb();
	if( _ui->_b_skeleton_asked_ui )
		k2_start_skel();
	return err;
}

void	c_capture_kinect::k2_update_depth_frame()
{
	SPY_PUSH_RANGE( "k2_update_depth_frame", spy::IMG );
//get frame
	IDepthFrame*	p_depth_frame	= nullptr;
	HRESULT hr = depth_frame_reader->AcquireLatestFrame( &p_depth_frame );
	if( !SUCCEEDED(hr) )
	{
		if( hr != E_PENDING )
			KINECT2_PRINT_STRING( "Error %d", hr );
		goto exit_depth;
	}

	{
//check time
		INT64 time = 0;
		hr = p_depth_frame->get_RelativeTime( &time );
		if( !SUCCEEDED(hr) || _k2_time_depth == time )
			goto exit_depth;
		_k2_time_depth = time;
	
		set_depth_buf_size( 512, 424, _ui->get_depth_pixel_format() );

//get info
		if( _depth_deform_bind != _ui->_depth_deform_bind_ui )
		{
			SPY_PUSH_RANGE( "k2_get_depth_deformation_frame", spy::IMG );
				//get the deformnation frame only once
				IFrameDescription*	p_description	= nullptr;
				if( SUCCEEDED(hr) )
					hr = p_depth_frame->get_FrameDescription(		&p_description				);
				if( SUCCEEDED(hr) )
					hr = p_description->get_Width(					&_k2_depth_pixel_size[0]	);
				if( SUCCEEDED(hr) )
					hr = p_description->get_Height(					&_k2_depth_pixel_size[1]	);
				if( SUCCEEDED(hr) )
					hr = p_description->get_HorizontalFieldOfView(	&_ui->_depth_fov[0]			);
				if( SUCCEEDED(hr) )
					hr = p_description->get_VerticalFieldOfView(	&_ui->_depth_fov[1]			);
				if( SUCCEEDED(hr) )
				{
				/*
					USHORT nDepthMinReliableDistance	= 0;
					if( SUCCEEDED(hr) )	{	hr = p_depth_frame->get_DepthMinReliableDistance( &nDepthMinReliableDistance );	}
					USHORT nDepthMaxDistance			= 0;
					if( SUCCEEDED(hr) )
					{
						// In order to see the full range of depth (including the less reliable far field depth)
						// we are setting nDepthMaxDistance to the extreme potential depth threshold
						nDepthMaxDistance = USHRT_MAX;

						// Note:  If you wish to filter by reliable depth distance, uncomment the following line.
						//return 4500
						//hr = p_depth_frame->get_DepthMaxReliableDistance( &nDepthMaxDistance );
					}
				*/
						//todo we should pass here everytime we start a kinect: e.g. when we plug/unplug it could be a different kinect so a different table (one by kinect infact)
	

					uint32_t tableCount;
					hr = _k2_mapper->GetDepthFrameToCameraSpaceTable( &tableCount, (PointF**)(&_k2_depth_to_cam_table) );
					if( SUCCEEDED(hr) )
					{
						c_img_2d*	img = g_bind_img_2d->get_always( _ui->_depth_deform_bind_ui );
						if( img )
						{
							//g_bind_img_2d->clear_filename_for_index( _ui->_depth_deform_bind_ui );	//was a try

							INT32 sx = 512;
							INT32 sy = 424;
							aaa::PIXEL_FORMAT format = aaa::PIXEL_FORMAT::RG_32FP;

							// was wrong because because Mâa was giving a pointer to data which is released below
							//img->init_from_mem( sx, sy, format, (UINT8*)_k2_depth_to_cam_table, sx * sy * aaa::c_pixel_format::get_byte_per_pixel(format) );

							img->init_with_size( sx,sy, format, __FUNCTION__ );
							st_img_conv options( sx,sy );
							options.signature = __FUNCTION__;
							options.src_pixel_format = format;
							img->copy_from_src( (UINT8*)_k2_depth_to_cam_table, sx * aaa::c_pixel_format::get_byte_per_pixel(format), options );
						
							img->set_cpu_keep( true );	//todo we keep it to send to other machines, it should be an option
							tex_2d_bind_no_gpu_move( _ui->_depth_deform_bind_ui );
							img->move_to_gpu( __FUNCTION__, _ui->_depth_deform_bind_ui );
							_depth_deform_bind = _ui->_depth_deform_bind_ui;
						}
					}
					else
						ERR_PRINT_STRING( "%s() can't read depth frame table to camera space ", __FUNCTION__ );
				}
				sysutils::safe_release( p_description );
			SPY_POP_RANGE();
		}

		{
			bool b_rgb_aligned = false;
			if( SUCCEEDED(hr) && !_ui->_b_depth_debug_skip_acquire_frame )
			{
				UINT buffer_size = 0;
				UINT16* p_buffer = nullptr;
				hr = p_depth_frame->AccessUnderlyingBuffer( &buffer_size, &p_buffer );
			//deal with data
				if( !_ui->_b_depth_debug_skip_process_frame )
				{
					if( SUCCEEDED(hr) )
					{
						//if( set_depth_buf_size( sx, sy ) )
						k2_do_depth( time, p_buffer, _k2_depth_pixel_size[0], _k2_depth_pixel_size[1] ); //, nDepthMinReliableDistance, nDepthMaxDistance );
						//}
						_ui->set_depth( true );
						if( _ui->_b_rgb_align_asked_ui )
						{
							SPY_PUSH_RANGE( "k2_get_depth_deformation_frame", spy::IMG );
#if 1
								c_img_2d*	img = g_bind_img_2d->get_always( _ui->_rgb_align_bind_ui );
								if( img )
								{
									//g_bind_img_2d->clear_filename_for_index( _ui->_depth_deform_bind_ui );	//was a try
									aaa::PIXEL_FORMAT format = aaa::PIXEL_FORMAT::RG_32FP;
									img->init_with_size( 512, 424, format, __FUNCTION__ );
									//img->set_src_y_inverted( true );	//todo this is in flux
									img->set_cpu_keep( true );	//todo we keep it to send to other machines, it should be an option

									void* dst = img->get_data();
									if( dst )
									{
										hr =_k2_mapper->MapDepthFrameToColorSpace( 512*424, p_buffer, 512*424, (ColorSpacePoint*)dst );
										if( SUCCEEDED(hr) )
										{
											img->set_changed();
											tex_2d_bind_no_gpu_move( _ui->_rgb_align_bind_ui );

											img->move_to_gpu( __FUNCTION__, _ui->_rgb_align_bind_ui );
											b_rgb_aligned = true;
										}
										else
											ERR_PRINT_STRING( "%s() can't read map depth color", __FUNCTION__ );
									}
								}
#else
								hr =_k2_mapper->MapDepthFrameToColorSpace( 512*424, p_buffer, 512*424, tmp_depth_map );
								if( SUCCEEDED(hr) )
								{
									c_img_2d*	img = g_bind_img_2d->get_always( _ui->_rgb_align_bind_ui );
									if( img )
									{
										//g_bind_img_2d->clear_filename_for_index( _ui->_depth_deform_bind_ui );	//was a try

										INT32 sx = 512;
										INT32 sy = 424;
										aaa::PIXEL_FORMAT format = aaa::PIXEL_FORMAT::RG_32FP;

										// was wrong because because Mâa was giving a pointer to data which is released below
										//img->init_from_mem( sx,sy, format, (UINT8*)tmp_depth_map, sx * sy * aaa::c_pixel_format::get_byte_per_pixel(format) );

										img->init_with_size( sx,sy, format, __FUNCTION__ );
										st_img_conv_offset options( 0,0, sx, sy );
										options.signature = __FUNCTION__;
										options.src_pixel_format = format;
										img->copy_from_src( (UINT8*)tmp_depth_map, sx * aaa::c_pixel_format::get_byte_per_pixel(format), options );

										img->set_cpu_keep( true );	//todo we keep it to send to other machines, it should be an option
										tex_2d_bind_no_gpu_move( _ui->_rgb_align_bind_ui );
										img->move_to_gpu( __FUNCTION__, _ui->_rgb_align_bind_ui );
									}
								}
								else
									ERR_PRINT_STRING( "%s() can't read map depth color", __FUNCTION__ );
#endif
							SPY_POP_RANGE();
						}
					}
				}
			}
			_ui->set_rgb_aligned( b_rgb_aligned );
		}
	}

exit_depth:
	sysutils::safe_release( p_depth_frame );
	SPY_POP_RANGE();
}

void c_capture_kinect::k2_update_rgb_frame()
{
	SPY_PUSH_RANGE( "k2_update_rgb_frame", spy::IMG );
//get frame
	IColorFrame* p_rgb_frame			= nullptr;
	HRESULT hr = rgb_frame_reader->AcquireLatestFrame( &p_rgb_frame );
	if( !SUCCEEDED(hr) )
	{
		if( hr != E_PENDING )
			KINECT2_PRINT_STRING( "Error %d", hr );
		goto exit_rgb;
	}
//check time
	{
		INT64 time = 0;
		hr = p_rgb_frame->get_RelativeTime( &time );
		if( !SUCCEEDED(hr) || _k2_time_rgb == time )
			goto exit_rgb;
		_k2_time_rgb = time;
	}
//get info
	{
		IFrameDescription*	p_description	= nullptr;
		if( SUCCEEDED(hr) )		hr = p_rgb_frame->get_FrameDescription( &p_description );
		{
			int sx = 0;
			int sy = 0;
			if( SUCCEEDED(hr) )		hr = p_description->get_Width( &sx );
			if( SUCCEEDED(hr) )		hr = p_description->get_Height( &sy );
			sysutils::safe_release( p_description );
			//	_img_rgb->set_flux_size_format( sx, sy, aaa::PIXEL_FORMAT::RGBA );
		}
	}

//deal with data
	//if( SUCCEEDED(hr) )		{	hr = p_rgb_frame->get_RawColorImageFormat( &rawColorImageFormat );									}
	//if( SUCCEEDED(hr) )		{	hr = p_rgb_frame->AcquireFrame( &buffer_size, &p_buffer );											}
	if( SUCCEEDED(hr) )			{	hr = p_rgb_frame->CopyConvertedFrameDataToArray( 1920*1080*4, rgba_frame, ColorImageFormat_Rgba );	}
	sysutils::safe_release( p_rgb_frame );
	if( SUCCEEDED(hr) )
	{
		_img_flux_rgb->got_frame( rgba_frame, __FUNCTION__ );
		_ui->set_rgb( true );
	}
	return;

exit_rgb:
	sysutils::safe_release( p_rgb_frame );
	SPY_POP_RANGE();
}

void c_capture_kinect::k2_update_skel_frame()
{
	SPY_PUSH_RANGE( "k2_update_skel_frame", spy::IMG );
//get frame
	IBodyFrame* p_body_frame  = nullptr;
	HRESULT hr = skel_frame_reader->AcquireLatestFrame( &p_body_frame );
	if( !SUCCEEDED(hr) )
	{
		if( hr != E_PENDING )
			KINECT2_PRINT_STRING( "Error %d", hr );
	}
	else
	{
//check time
		INT64 time = 0;
		hr = p_body_frame->get_RelativeTime( &time );
		if( SUCCEEDED(hr) && _k2_time_body != time )
		{
			_k2_time_body = time;
//get info
			hr = p_body_frame->GetAndRefreshBodyData( _countof(pp_body), pp_body );
			if( SUCCEEDED(hr) )
			{
				_ui->_b_skeleton = true;
				process_skel();
			}	
			k2_do_skeleton();
		}
	}
	sysutils::safe_release( p_body_frame );
	SPY_POP_RANGE();
}

void c_capture_kinect::process_skel()
{
	HRESULT hr;
//	m_pRenderTarget->BeginDraw();
//	m_pRenderTarget->Clear();

//	RECT rct;
//	GetClientRect(GetDlgItem(m_hWnd, IDC_VIDEOVIEW), &rct);
//	int width = rct.right;
//	int height = rct.bottom;

	INT32 nb_tracked = 0;
	for( INT32 i = 0; i < BODY_NB; ++i )
	{
		IBody* p_body = pp_body[i];
		c_kinect_ui::c_skel&	skel = _ui->_skels[i];

		skel._b_tracked = false;
		skel._id = 0;
		skel._mocap_target_id = 0;
		if( p_body )
		{
			BOOLEAN b_tracked = FALSE;
			hr = p_body->get_IsTracked( &b_tracked );

			if( SUCCEEDED(hr) && b_tracked )
			{
				UINT64 id;
				p_body->get_TrackingId( &id );
				skel._id			= id & 0xffffffff;	//todo whould be ok to keep only 32 bits but...
				skel._b_tracked		= true;
				_k2_skel_tracked_index[nb_tracked] = i;
				++nb_tracked;

				// Create new sources with body TrackingId
				// _faceSource = new FaceFrameSource(_kinect, body.TrackingId, _faceFrameFeatures);
 
				// Create new reader
				//  _faceReader = _faceSource.OpenReader();
 
				HandState			state;
				TrackingConfidence	confidence;

				p_body->get_HandLeftState(			&state );
				skel._hand_left					=	state; 
				p_body->get_HandLeftConfidence(		&confidence );
				skel._hand_left_confidence		=	confidence; 

				p_body->get_HandRightState(			&state );
				skel._hand_right				=	state; 
				p_body->get_HandRightConfidence(	&confidence );
				skel._hand_right_confidence		=	confidence;

				BOOLEAN boolean = FALSE;
				p_body->get_IsRestricted( &boolean );
				skel._b_restricted	= (bool) boolean;

				// forXBox but not windows : https://msdn.microsoft.com/en-us/library/windowspreview.kinect.body.aspx
				//get_Engaged();
				//GetExpressionDetectionResults();
				//GetActivityDetectionResults();
				//GetAppearanceDetectionResults();

				//todo
				//GetJointOrientations();
				//get_ClippedEdges();
				//get_Lean();
				//get_LeanTrackingState();
				
				Joint joints[JointType_Count]; 
			//	D2D1_POINT_2F jointPoints[JointType_Count];
				hr = p_body->GetJoints( _countof(joints), joints );
				if( SUCCEEDED(hr) )
				{
					for( INT32 j = 0; j < _countof(joints); ++j )
					{
						Joint& joint = joints[j];
						CameraSpacePoint& pos = joint.Position;
						skel.set_point( joint.JointType, pos.X, pos.Y, pos.Z );
						//joint.JointType;
						//joint.TrackingState;
						//jointPoints[j] = BodyToScreen( joint, width, height);
					}
				}
				FP32 vec4[4];
				skel.get_point( JointType_SpineBase, vec4 );
				cpy_v4( skel._pos, vec4 );				
			}
		}
	}
	_ui->_body_seen_nb				= nb_tracked;
	_ui->_skeleton_nb_tracked		= nb_tracked;
}

void c_capture_kinect::k2_cpy_skeleton( c_bdd_mocap* mo, c_kinect_ui::c_skel CONST * CONST skel )
{
	if( mo )
	{
		std::lock_guard<c_bdd_mocap> guard(*mo);
	
		mo->set_kinect2();
		bool	b_flip = _ui->_b_skeleton_x_flip;
		REAL	sin, cos;
		GET_SIN_COS_TURN( sin, cos, _ui->_skeleton_rot_x );
		FP32	vec[4];
		REAL	r[3];
		for( INT32 i = 0 ; i < c_kinect_ui::c_skel::POINT_NB; ++i )
		{
			skel->get_point( i, vec );
			r[0] = b_flip ? -vec[0] : vec[0];
			REAL y = vec[1];
			REAL z = vec[2];
			r[1] =  y * cos + z * sin;
			r[2] = -y * sin + z * cos;
			mo->store_tra( i+1, r );
		}
		//	we get the head position here
		//hack 
	//	if( _ui->_b_compute_head_pos_ui )
	//	{
	//		LONG	color_x;
	//		LONG	color_y;
	//		LONG	depth_x;
	//		LONG	depth_y;
	//		USHORT	depth_z;

	//		Vector4&	vb = skel.SkeletonPositions[2] ;
	//		Vector4&	vt = skel.SkeletonPositions[3] ;
	//		Vector4	v;
	//		v.x = (vb.x + vt.x) * .5;
	//		v.y = (vb.y + vt.y) * .5;
	//		v.z = (vb.z + vt.z) * .5;
	//		v.w = (vb.w + vt.w) * .5;
	//		NuiTransformSkeletonToDepthImage( v, &depth_x, &depth_y, &depth_z );
	//		_p_nui_instance->NuiImageGetColorPixelCoordinatesFromDepthPixel( NUI_IMAGE_RESOLUTION_640x480, 0, depth_x, depth_y, depth_z<<3, &color_x, &color_y );

	//		_ui->_head_pos[0] = color_x / 640.;
	//		_ui->_head_pos[1] = 1. - color_y / 480.;
	//	}
	}
}

void c_capture_kinect::k2_do_skeleton()
{
	if( !_b_opened )
		return;
//	NUI_SKELETON_FRAME	skel_frame;
//	if( FAILED( _p_nui_instance->NuiSkeletonGetNextFrame( 0, &skel_frame ) ) )
//		return;


//	_ui->lock();

//	INT32	skeleton_nb_tracked = 0;
//	INT32	skeleton_nb_tracked_not = 0;
//todo	deal with NUI_SKELETON_POSITION_ONLY

	INT32 nb_track = _ui->_skeleton_nb_tracked; 
	for( INT32 i_skel = 0 ; i_skel < nb_track; ++i_skel )
	{
		INT32					index	= _k2_skel_tracked_index[i_skel];
		c_kinect_ui::c_skel*	skel	= &_ui->_skels[index];

		//DWORD track_id[] = { 0, 0 };
		//INT32 id = _ui->select_id();
		//track_id[0] = id;
		skel->_mocap_target_id = i_skel + 1;
		c_bdd_mocap*	mo = _ui->_bdd_target[i_skel];
		k2_cpy_skeleton( mo, skel );
	}

//	_kinect_ui->lock();
//		_kinect_ui->_skeleton_nb_tracked = nb;
//	_kinect_ui->unlock();
/*
	//maa for test
	REAL tra[3];
	REAL t = aaa::time::get();
	for(INT32 i=1; i<=12; ++i )
	{
		tra[0] = SIN_INT( t / 8. + i *.0625 );
		tra[1] = i * .1;
		tra[2] = 0.;
		mo->store_tra( i, tra );
	}
*/
//	_ui->unlock();
	return;
}
void c_capture_kinect::k2_update()
{
	if( !_b_running )
		return;

	// DEPTH
	if( _ui->_b_depth_asked_ui == IS_NULL(depth_frame_reader) )
	{	
		if( _ui->_b_depth_asked_ui )
			k2_start_depth();
		else
			k2_stop_depth();
	}
	if( depth_frame_reader && _ui->_b_depth_use_ui )
		k2_update_depth_frame();
	else
		_ui->set_rgb_aligned( false );

	// RGB
	if( _ui->_b_rgb_asked_ui == IS_NULL(rgb_frame_reader) )
	{	
		if( _ui->_b_rgb_asked_ui )
			k2_start_rgb();
		else
			k2_stop_rgb();
	}
	if( rgb_frame_reader && _ui->_b_rgb_use_ui )
		k2_update_rgb_frame();

	// SKELETON
	if( _ui->_b_skeleton_asked_ui == IS_NULL(skel_frame_reader) )
	{	
		if( _ui->_b_skeleton_asked_ui )
			k2_start_skel();
		else								
			k2_stop_skel();
	}
	if( skel_frame_reader && _ui->_b_skeleton_use_ui )
		k2_update_skel_frame();
}


void c_capture_kinect::k2_close()
{
	if( k2_sensor )
	{
		k2_sensor->Close();
		CoTaskMemFree(_k2_depth_to_cam_table);
		_k2_depth_to_cam_table = nullptr;
		sysutils::safe_release( k2_sensor );
	}
}

AAA_ERR c_capture_kinect::k2_open( INT32 CONST index, o_str CONST * CONST pt_device_id )
{
	if( !dll_k2.is_loaded() )
		return error( "No dll loaded" );

	HRESULT hr = dll_k2.GetDefaultKinectSensor( &k2_sensor );	//	one sensor only for now
	if( FAILED(hr) )
		return error( "Error searching camera" );
	if( !k2_sensor )
		return error( "No camera found!" );

	k2_sensor->get_CoordinateMapper( &_k2_mapper );

	hr = k2_sensor->Open();
	if( FAILED(hr) )
	{
		KINECT2_PRINT_STRING( "Could not open Kinect v2 sensor" );
		k2_close();
		return ERR_ANY;
	}
	_device_index_used = 0;
	return AAA_OK;
}
