#include "bdd_img_mediapipe.h"
#include "tracking/mediapipe/mediapipe.h"
#include "obj_ui/bdd/bdd_ui_pref.h"
#include "image/bind_img_2d.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_mediapipe, bdd_img_mediapipe, Mediapipe Tracking, bdd_img_mediapipe, sub_menu="Image"; );


namespace n_bdd_img_mediapipe
{
	CONSTEXPR INT32	BASE_PARAM_NB		= 10 + c_bdd_img::SUPER_PARAM_NB;
	CONSTEXPR INT32	HOW_PARAM_NB		= 0;
	CONSTEXPR INT32	DRAW_PARAM_NB		= 0;
	CONSTEXPR INT32	FIDUCIAL_PARAM_NB	= 0;
	CONSTEXPR INT32	TRANSFER_PARAM_NB	= 0;
	CONSTEXPR INT32	GROUP_PARAM_NB		= 0;
	CONSTEXPR INT32	PARAM_NB	=	BASE_PARAM_NB
								+	HOW_PARAM_NB
								+	DRAW_PARAM_NB
								+	FIDUCIAL_PARAM_NB
								+	TRANSFER_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		PARAM_DEF_NONE(				Bdd_ui_pref		)

		PARAM_DEF_BOOL_OFF(				draw_in_texture	)
		PARAM_DEF_BIND_2D_CURRENT_SEL(	dst_bind		)
		PARAM_DEF_BOOL_ON(				dst_clear		)

		PARAM_DEF_INT32_ONE_ZERO(	frame_detection			)
		PARAM_DEF_INT32_ONE_ZERO(	frame_detection_no		)

		PARAM_DEF_STR_LOCKED(		arm_left		)
		PARAM_DEF_STR_LOCKED(		arm_right		)
		PARAM_DEF_STR_LOCKED(		hand_left		)
		PARAM_DEF_STR_LOCKED(		hand_right		)
	};
}


void	c_bdd_img_mediapipe::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	param_attach_obj( h, c_bdd_ui_pref::cur );

	param_set_pt( h, _b_draw_in_texture		);
	param_set_pt( h, _dst_img_bind			);
	param_set_pt( h, _b_dst_clear			);

	param_set_pt( h, _frame_detection		);
	param_set_pt( h, _frame_detection_no	);

	param_set_pt( h, _arm_left				);
	param_set_pt( h, _arm_right				);
	param_set_pt( h, _hand_left				);
	param_set_pt( h, _hand_right			);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_mediapipe )
{
	param_init_with( n_bdd_img_mediapipe::param, n_bdd_img_mediapipe::PARAM_NB );
	force_mono();
}

void	c_bdd_img_mediapipe::release()
{
	IF_THIS_NULL_RETURN();
}

c_bdd_img_mediapipe::~c_bdd_img_mediapipe()
{
}

void	c_bdd_img_mediapipe::update()
{
//	bool b_change_it;
//	if( !update_mat_src(b_change_it) )
//	{
//		return;
//	}

//	if( is_size_changed() )
//		release();
//	if( is_content_changed() )
		do_process();

}

//inline uint64_t get_elapsed_time_microseconds(std::chrono::system_clock::time_point const& start_time) {
//  return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now()-start_time).count();
//}

C_PCHAR_C GetGestureResult(int result)
{
	C_PCHAR str = nullptr;
	switch (result)
	{
	case 1:	str = "One";		break;
	case 2:	str = "Two";		break;
	case 3:	str = "Three";		break;
	case 4:	str = "Four";		break;
	case 5:	str = "Five";		break;
	case 6:	str = "Six";		break;
	case 7:	str = "ThumbUp";	break;
	case 8:	str = "Ok";			break;
	case 9:	str = "Fist";		break;
	default:	break;
	}
	return str;
}
C_PCHAR_C GetArmUpAndDownResult(int result)
{
	C_PCHAR str = nullptr;
	switch (result)
	{
	case -1: str = "";		break;
	case 1:  str = "UP";	break;
	case 2:  str = "DWN";	break;
	default:	break;
	}

	return str;
}
int detect_result[4];
float fps_process;


void	c_bdd_img_mediapipe::do_process()
{
	if( !g_mediapipe || !g_mediapipe->is_active() )
		return;
	if( !c_mediapipe::b_holistic_model_loaded )
		return;

	//	cv::Mat src;
	//	src = _src_cur->clone();

	if( _b_draw_in_texture )
	{
		auto src_index = get_src_img_index_ui();
		c_img_2d*	img = g_bind_img_2d->get_ready( src_index );
		if( IS_NULL( img ) )
		{
			err_print( "%s() : no image source at bind %d", __FUNCTION__, src_index );
			return;
		}
		UINT8* data = img->get_data_valid_rgb_uint8( __FUNCTION__ );
		if( !data )
		{
			err_print( "%s() : image source has no valid data at bind %d", __FUNCTION__, src_index );
			return;
		}

		//c_img_2d*	img_b = g_bind_img_2d->get_always( src_index+1 );
		//c_img_2d::img_init_from_mem( img_b,
		//					 img->get_size_x(), img->get_size_y()/2, aaa::PIXEL_FORMAT::RGB_8,
		//					 img->get_data_uint8(), img->get_size_x()*img->get_size_y()*3/2
		//					 );

//		MediapipeHolisticTrackingDll& dll = &g_mediapipe->dll;
		//uchar* pImageData = _src_cur->getMat( cv::ACCESS_RW ).data;
//		auto const start_time = std::chrono::system_clock::now();
		int* pdetect_result = new int[4];
//		spy::sleep( 100, __FUNCTION__ );
		auto res = c_mediapipe::dll.HolisticTrackingDetectFrameDirect( img->get_size_x(), img->get_size_y(), (void*)data, pdetect_result, false );
		if( res )
		{
			++_frame_detection;
			GOOD_PRINT_STRING( "Yesss" );

//			auto duration = get_elapsed_time_microseconds(start_time);
//			fps_process = (float)(1.e+6 / (double)duration );

			_arm_left.set(		GetArmUpAndDownResult(	detect_result[0] ) );
			_arm_right.set(		GetArmUpAndDownResult(	detect_result[1] ) );
			_hand_left.set(		GetGestureResult(		detect_result[2] ) );
			_hand_right.set(	GetGestureResult(		detect_result[3] ) );

			//_src_cur = &_contour_dst;
			//update_dst_index( _contour_img_dst, "Contour Destination" );
		}
		else
			++_frame_detection_no;
		delete[] pdetect_result;
	}

//		else
//			find_blob_contour( _s_contour_mode, nullptr );
//		cur = this;
}


void	c_bdd_img_mediapipe::draw()
{	
	if( !is_active() )
		return;	
}
