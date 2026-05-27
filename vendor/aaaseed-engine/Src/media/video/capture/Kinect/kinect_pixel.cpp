#include "capture_kinect.h"
#include "Kinect-Utility.h"
#include "wrap_kinect.h"
#include "wrap_kinect2.h"
#include "image/convert/color_space.h"
#include "image/img_compo.h"
#include "spy.h"


void	c_capture_kinect::k1_do_rgb_sdk()
{
	if( !_b_opened )
		return;

	NUI_IMAGE_FRAME nui_image_frame;
	HRESULT			hr = _k1_sdk_p_nui_instance->NuiImageStreamGetNextFrame(_k1_hd_stream_rgb, 0, &nui_image_frame);
	if( FAILED(hr) )
		return;

	INuiFrameTexture*	texture = nui_image_frame.pFrameTexture;
	NUI_LOCKED_RECT		tex_info;
	texture->LockRect( 0, &tex_info, nullptr, 0 );
	if( tex_info.Pitch == 0 )
	{
		ERR_PRINT_STRING( "Buffer length of received rgb texture is bogus" );
		goto exit;
	}
	//_img_rgb->set_src_pitch( tex_info.Pitch );
	_img_flux_rgb->got_frame( reinterpret_cast<UINT8*>(tex_info.pBits), "Kinect RGB", tex_info.Pitch, true, 1. );

	if( _ui->_b_rgb_align_asked_ui && _b_k1_sdk_mapping_ok )
	{
		INT32 CONST rgb_sx = 640;	// depth to frame so dst is 320x240 for aligned image
		INT32 CONST	rgb_sy = 480;

		//NUI_IMAGE_RESOLUTION	depth_res		= NUI_IMAGE_RESOLUTION_320x240;

		UINT32*	src = reinterpret_cast<UINT32*>(tex_info.pBits);
		UINT32*	dst = _k1_sdk_color_aligned - 1;
		LONG*	coors = _k1_sdk_mapped_coords - 1;
		if( _ui->_b_rgb_align_only_valid_ui )
		{
			UINT8*	depth;
			for( INT16 iy = rgb_sy - 1; iy >= 0; --iy )
			{
				depth = _buf_depth + iy*rgb_sx * 4;
				for( INT16 ix = rgb_sx; ix>0; --ix )
				{
					LONG cx = *++coors;
					LONG cy = *++coors;
					if( *depth != 0 && 0 <= cx && cx < rgb_sx && 0 <= cy && cy < rgb_sy )
					{
						size_t	pos = cx + cy * rgb_sx;
						*++dst = src[pos];
					}
					else
					{
						// Out of bounds, black
						*++dst = 0x00000000;
					}
					depth += 4;
				}
			}
		}
		else
		{
			for( INT16 iy = rgb_sy; iy>0; --iy )
			{
				for( INT16 ix = rgb_sx; ix>0; --ix )
				{
					LONG cx = *++coors;
					LONG cy = *++coors;
					if( 0 <= cx && cx < rgb_sx && 0 <= cy && cy < rgb_sy )
					{
						size_t	pos = cx + cy * rgb_sx;
						*++dst = src[pos];
					}
					else
					{
						// Out of bounds, black
						*++dst = 0x00000000;
					}
				}
			}
		}
		_img_flux_rgb_aligned->got_frame(reinterpret_cast<UINT8*>(_k1_sdk_color_aligned), "Kinect RGB Aligned", 0, true, 1. );
	}
	texture->UnlockRect(0);
exit:
	_k1_sdk_p_nui_instance->NuiImageStreamReleaseFrame( _k1_hd_stream_rgb, &nui_image_frame );
}

void	c_capture_kinect::do_rgb_stuff()
{
	//printf( "kinect got_frame_rgb\n" );
	if( !_b_opened )
		return;
	if( _k1_kinect_listener == nullptr )
		return;

	Kinect::Kinect *K = _k1_kinect_listener->_k1_kinect;

	K->ParseColorBuffer32();

	_img_flux_rgb->got_frame( (UINT8*)K->mColorBuffer, "Kinect RGB" );
	spy::sleep( 0, "sleep capture_kinect::do_rgb_stuff()" );
}

namespace {
	FINLINE	void	set_pixel_bgr(			UINT8*& dst, UINT8 const blue, UINT8 const green, UINT8 const red )
	{
		*++dst = red;
		*++dst = green;
		*++dst = blue;
	}
	FINLINE	void	set_pixel_rgba(			UINT8*& dst, UINT8 const red, UINT8 const green, UINT8 const blue, UINT8 const alpha )
	{
		*++dst = red;
		*++dst = green;
		*++dst = blue;
		*++dst = alpha;
	}
	FINLINE	void	set_pixel_rgba(			UINT8*& dst, UINT8 const* col )
	{
		*++dst = col[0];
		*++dst = col[1];
		*++dst = col[2];
		*++dst = col[3];
	}
	FINLINE	void	set_pixel_bgra(			UINT8*& dst, UINT8 const blue, UINT8 const green, UINT8 const red, UINT8 const alpha )
	{
		set_pixel_rgba( dst, red, green, blue, alpha );
	}
	FINLINE	void	set_pixel_grey_alpha(	UINT8*& dst, UINT8 const grey, UINT8 const alpha )
	{
		set_pixel_rgba( dst, grey, grey, grey, alpha );
	}
	FINLINE	void	set_pixel_grey(			UINT8*& dst, UINT8 const grey )
	{
		set_pixel_rgba( dst, grey, grey, grey, 255 );
	}
	FINLINE	void	set_pixel(				UINT8*& dst, UINT8 const val )
	{
		set_pixel_rgba( dst, val, val, val, val );
	}

	FINLINE	void	set_pixel_fp32(			FP32*& dst, FP32 const val )
	{
		*++dst = val;
	}
	FINLINE	void	set_pixel_i16(			INT16*& dst, INT16 const val )
	{
		*++dst = val;
	}
	FINLINE void fill_pixel_left( UINT8*& dst, INT32& x, INT32 const x_left )
	{
		//	fill the empty part on the left
		for( x = 0; x < x_left; ++x )	//todo	use a fill fn()
			set_pixel( dst, 0 );
	}
	FINLINE void fill_pixel_right( UINT8*& dst, INT32& x, INT32 const sx )
	{
		//	fill the empty part on the right
		for( ; x < sx; ++x )
			set_pixel( dst, 0 );

	}
	FINLINE void fill_pixelf_i16_left( INT16*& dst, INT32& x, INT32 const x_left )
	{
		//	fill the empty part on the left
		for( x = 0; x < x_left; ++x )	//todo	use a fill fn()
			set_pixel_i16( dst, 0 );
	}
	FINLINE void fill_pixelf_i16_right( INT16*& dst, INT32& x, INT32 const sx )
	{
		//	fill the empty part on the right
		for( ; x < sx; ++x )
			set_pixel_i16( dst, 0 );
	}
	FINLINE void fill_pixelf_fp32_left( FP32*& dst, INT32& x, INT32 const x_left )
	{
		//	fill the empty part on the left
		for( x = 0; x < x_left; ++x )	//todo	use a fill fn()
			set_pixel_fp32( dst, 0 );
	}
	FINLINE void fill_pixelf_fp32_right( FP32*& dst, INT32& x, INT32 const sx )
	{
		//	fill the empty part on the right
		for( ; x < sx; ++x )
			set_pixel_fp32( dst, 0 );
	}
	CONSTEXPR INT32	colors[8][3] = { { 1, 1, 1 }, { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 1, 0, 1 }, { 1, 1, 0 }, { 1, 1, 0 } };

	void make_color( 	UINT8 * const col, FP32 const * const src )
	{
		col[0] = aaa::img::c_compo::to_uint8( src[0] * src[4] );
		col[1] = aaa::img::c_compo::to_uint8( src[1] * src[4] );
		col[2] = aaa::img::c_compo::to_uint8( src[2] * src[4] );
		col[3] = aaa::img::c_compo::to_uint8( src[3] * src[4] );
	}
}

#define SET_PIXEL_NONE()		set_pixel(	dst, 0 )
#define SET_PIXEL_INVALID()		set_pixel_rgba(	dst, color_invalid )
#define SET_PIXEL_OUT( val )	{												\
									if( b_raw_clip )							\
										set_pixel_rgba(	dst, color_invalid	);	\
									else if( val < i_min )						\
										set_pixel_rgba(	dst, color_far		);	\
									else										\
										set_pixel_rgba(	dst, color_near		);	\
								}

#define MAKE_COLOR_INVALID		UINT8	color_invalid[4];										\
								UINT8	color_near[4];											\
								UINT8	color_far[4];											\
								make_color( color_invalid,	_ui->_depth_color_invalid_ui );		\
								make_color( color_near,		_ui->_depth_color_near_ui );		\
								make_color( color_far,		_ui->_depth_color_far_ui );
	
//todo look at kinect 1 to see what we do with player in v2 ?															
#define K2_DO_BEFORE			for( ; y >= y_bottom; --y )									\
								{															\
									dst = _buf_depth + y * dst_stride - 1;					\
									skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));	\
									fill_pixel_left( dst, x, x_left );						\
									src += x;												\
									for( ; x < x_right; ++x )								\
									{														\
										INT32	val = *++src;								\
										if( val==0 )										\
											SET_PIXEL_INVALID();							\
										else												\
										{													\
											val += skew;									\
											val = DMAXM1 - val;								\
											if( b_raw_clip )								\
											{												\
												if( OUTSIDE_MIN_MAX( val, i_min, i_max ) )	\
												{											\
													SET_PIXEL_INVALID();					\
													continue;								\
												}											\
											}												\
											else											\
												CLAMP_REF( val, i_min, i_max );



#define K2_DO_AFTER						}												\
									}													\
									src += sx - x;										\
									fill_pixel_right( dst, x, sx );						\
								}														\
								break;
/*
bool hasPlayerData = HasSkeletalEngine(m_pNuiInstance);
USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
USHORT Player = hasPlayerData ? s & 7 : 0;
*/

template< bool B_ONLY_WHEN_PLAYER >
static	FINLINE INT32 const * process_pixel_in( INT32& val, bool b_player_data, bool* b_body, struct c_kinect_ui::st_rect* rects, INT32 x )
{
	INT32	player = NuiDepthPixelToPlayerIndex( val );
	if( player )
	{
		--player;
		b_body[player] = true;
		struct c_kinect_ui::st_rect& rect = rects[player];
		UPDATE_MIN_MAX_SAFE( rect.x_min, rect.x_max, x );
		//todo add test for current skeleton
		val = (val & 0xfff8) >> 3;
		return colors[player + 1];
	}
	else
	{
		if( B_ONLY_WHEN_PLAYER )
			val = 0;
		else
			val = (val & 0xfff8) >> 3;
		return colors[0];
	}
}


void c_capture_kinect::erase_rects_depth( INT32 CONST sx, INT32 CONST sy, INT32 CONST stride )
{
	SPY_PUSH_RANGE( "depth_erase_rects", spy::IMG );
	{
		for( INT32 i=0; i<c_kinect_ui::ERASE_RECT_NB; ++i )
		{
			auto& er = _ui->_erase_rect[i];
			if( er._b_active_ui )
			{
				CONST	INT32	x_left		= CLAMP( I_FLOOR(sx * er._x_ui),				0, sx-1 );
				CONST	INT32	x_right		= CLAMP( I_FLOOR(sx * (er._x_ui+er._sx_ui)),	0, sx );
				CONST	INT32	y_bottom	= CLAMP( I_FLOOR(sy * er._y_ui),				0, sy-1 );
				CONST	INT32	y_top		= CLAMP( I_FLOOR(sy * (er._y_ui+er._sy_ui)),	0, sy );

				CONST	INT32	x_nb			= x_right-x_left;
				switch( _ui->_s_depth_transform_type_ui )
				{
				case c_kinect_ui::DEPTH_DIST_INT16:
				case c_kinect_ui::DEPTH_RANGE_INT16:
					for( INT32 y=y_bottom ; y < y_top; ++y )
					{
						INT16* dst = (INT16*) (_buf_depth + y * stride ) + x_left - 1;
						for( INT32 x = x_nb; x > 0; --x )
							set_pixel_i16( dst, 0 );
					}
					break;
				default:
					break;
				}
			}
		}
	}
	SPY_POP_RANGE();
}

void c_capture_kinect::k2_do_depth( INT64 CONST time, UINT16 CONST * src, INT32 CONST sx, INT32 CONST sy )	//, INT32 depth_min, INT32 depth_max )
{
	SPY_PUSH_RANGE( "k2_do_depth", spy::IMG );
	{
		std::lock_guard<c_kinect_ui> guard(*_ui);
		/*
		CONST	INT32	DMAX = 4096;
		CONST	INT32	DMAXM1 = DMAX - 1;
		CONST	INT32	i_min = _ui->_depth_raw_min * (DMAX - .001);	//c_kinect_listener::DEPTH_RANGE;
		CONST	INT32	i_max = _ui->_depth_raw_nax * (DMAX - .001);	//c_kinect_listener::DEPTH_RANGE;
		CONST	INT32	factor_int = (i_max != i_min) ? (1 << 30) / (i_max - i_min) : 0;


		CONST	bool	b_player_data = true;

		_ui->init_bodys();
		struct c_kinect_ui::st_rect*		rects	= &(_ui->_bodys_rect[0]);
		bool*								b_body	= &(_ui->_b_bodys[0]);
		*/
		--src;

		CONST	INT32	DMAX = 4500;	//	depth_max;
		CONST	INT32	DMAXM1 = DMAX - 1;
		INT32	i_min = INT32(_ui->_depth_raw_min_ui * (DMAX - .001));	//c_kinect_listener::DEPTH_RANGE;
		INT32	i_max = INT32(_ui->_depth_raw_max_ui * (DMAX - .001));	//c_kinect_listener::DEPTH_RANGE;
		CONST	INT32	factor_int = (i_max != i_min) ? (1 << 30) / (i_max - i_min) : 0;

		MAKE_COLOR_INVALID;

		CONST	bool	b_raw_clip		= _ui->_b_depth_raw_clip_ui;

		INT32 const x_left			= _ui->_b_depth_clip_side_ui ? CLAMP( INT32(sx * _ui->_depth_clip_left_ui	),	INT32(0),	sx ) : 0;
		INT32 const x_right			= _ui->_b_depth_clip_side_ui ? CLAMP( INT32(sx * _ui->_depth_clip_right_ui	),	INT32(0),	sx ) : sx;
		INT32 const y_bottom		= _ui->_b_depth_clip_side_ui ? CLAMP( INT32(sy * _ui->_depth_clip_bottom_ui	),	INT32(0),	sy ) : 0;
		INT32 const y_top			= _ui->_b_depth_clip_side_ui ? CLAMP( INT32(sy * _ui->_depth_clip_top_ui	),	INT32(0),	sy ) : sy;

		INT32 const dst_stride		= sx * _ui->get_depth_byte_per_pixel();


		UINT8*	dst;
		INT32	x;
		INT32	skew;
		
		INT32	y = sy - 1;
		//	fill the empty part at the top
		//todo only one MEMSET
		for( ; y >= y_top; --y )
		{
			MEMCLEAR( _buf_depth + y * dst_stride, dst_stride );
			src += sx;
		}

		switch( _ui->_s_depth_transform_type_ui )
		{
		case c_kinect_ui::DEPTH_TRANSFORM_R8:
			K2_DO_BEFORE;
				val = ((val - i_min) * factor_int) >> 14;	//go back to 16 bits
				val = val >> 8;
				set_pixel_grey( dst, val );
			K2_DO_AFTER;
		case c_kinect_ui::DEPTH_TRANSFORM_MAA:
			K2_DO_BEFORE;
				val = ((val - i_min) * factor_int) >> 14;	//was >>14 //go back to 16 bits
				FP32 r, g, b;
				aaa::color::rgb_from_h( r, g, b, FP32(val)/0xffff );
				set_pixel_rgba( dst, UINT8(r*255.9999),  UINT8(g*255.9999),  UINT8(b*255.9999), 255 );
			K2_DO_AFTER;
		case c_kinect_ui::DEPTH_DIST_R8G5:
			K2_DO_BEFORE;
				val = DMAXM1 - val;	//todo avoid this: it is done in the macro loop we undo it here
				val <<= 3;
				set_pixel_rgba( dst, val >> 8, val & 0xff, 255, 255 );
			K2_DO_AFTER;
		case c_kinect_ui::DEPTH_DIST_R5G8:
			K2_DO_BEFORE;
				val = DMAXM1 - val;	//todo avoid this: it is done in the macro loop we undo it here
				set_pixel_rgba( dst, val >> 8, val & 0xff, 255, 255 );
			K2_DO_AFTER;
		default:
			K2_DO_BEFORE;
				set_pixel_grey( dst, val % 256 );	
			K2_DO_AFTER;
		case c_kinect_ui::DEPTH_DIST_FLOAT:
		case c_kinect_ui::DEPTH_RANGE_FLOAT:
			{
				//CameraIntrinsics cam_info;
				//_k2_mapper->GetDepthCameraIntrinsics( &cam_info );
				//_k2_mapper->MapDepthFrameToCameraSpace(
				//	512*424, (UINT16*) _buf_depth,	// Depth frame data and size of depth frame
				//	512*424, _k2_depth_to_xyz );			// Output CameraSpacePoint array and size

				FP32 factor  = OVER_ONE_AS_FP32(i_max - i_min);
				for( ; y >= y_bottom; --y )			
				{															
					FP32* dst = (FP32*) (_buf_depth + y * dst_stride) - 1;					
					skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));
					fill_pixelf_fp32_left( dst, x, x_left );
					src += x;
					for( ; x < x_right; ++x )							
					{														
						INT32	val = *++src;								
						if( val==0 )										
							set_pixel_fp32( dst, 0 );	//SET_PIXEL_INVALID();							
						else												
						{													
							val += skew;									
							val = DMAXM1 - val;								
							if( b_raw_clip )								
							{												
								if( OUTSIDE_MIN_MAX( val, i_min, i_max ) )	
								{											
									set_pixel_fp32( dst, 0 );	//SET_PIXEL_INVALID();					
									continue;								
								}											
							}												
							else											
								CLAMP_REF( val, i_min, i_max );
							if( _ui->_s_depth_transform_type_ui ==  c_kinect_ui::DEPTH_DIST_FLOAT )
								set_pixel_fp32( dst, (DMAXM1 - val) * FP32(0.001 *.125) );	//hack we do it to see well
							else
								set_pixel_fp32( dst, ( val- i_min ) * factor  );
						}												
					}													
					src += sx - x;										
					fill_pixelf_fp32_right( dst, x, sx );						
				}
			}
			break;
		case c_kinect_ui::DEPTH_DIST_INT16:
		case c_kinect_ui::DEPTH_RANGE_INT16:
			{
				FP32 const factor = FP32( 256.*256. - .0001 ) / (i_max - i_min);	
				for( ; y >= y_bottom; --y )			
				{															
					INT16* RESTRICT dst = (INT16*) (_buf_depth + y * dst_stride) - 1;					
					skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));
					fill_pixelf_i16_left( dst, x, x_left );
					src += x;
					for( ; x < x_right; ++x )							
					{														
						INT32	val = *++src;								
						if( val==0 )										
							set_pixel_i16( dst, 0 );	//SET_PIXEL_INVALID();							
						else												
						{													
							val += skew;									
							val = DMAXM1 - val;								
							if( b_raw_clip )								
							{												
								if( OUTSIDE_MIN_MAX( val, i_min, i_max ) )	
								{											
									set_pixel_i16( dst, 0 );	//SET_PIXEL_INVALID();					
									continue;								
								}											
							}												
							else											
								CLAMP_REF( val, i_min, i_max );
							if( _ui->_s_depth_transform_type_ui == c_kinect_ui::DEPTH_DIST_INT16 )
								set_pixel_i16( dst, (DMAXM1 - val) << 3 );	//hack we do it to see well
							else
								set_pixel_i16( dst, INT16( ( val- i_min ) * factor) );
						}												
					}													
					src += sx - x;										
					fill_pixelf_i16_right( dst, x, sx );						
				}
			}
			break;
		}

		//	fill the empty part at the bottom
		//todo only one MEMSET
		for( ; y >= 0; --y )
		{
			MEMCLEAR( _buf_depth + y * dst_stride, dst_stride );
		}

		erase_rects_depth( sx, sy, dst_stride );
	//todo
	/*
		INT32 nb = 0;
		for( INT32 i = 0; i<c_kinect_ui::NB_BODY_MAX; ++i )
		{
			if( b_body[i] )
				++nb;
		}
		_ui->_body_seen_nb = nb;
	*/
	}
	SPY_POP_RANGE();

	got_frame( reinterpret_cast<UINT8*>(_buf_depth), "Kinect Depth" );

//exit:
}

void c_capture_kinect::k1_do_depth_sdk()
{
	if( !_b_opened )
		return;
	//	++m_FramesTotal;

//	DBG_HEAP_IS_CORRUPT();

	NUI_IMAGE_FRAME	nui_depth_frame;
	HRESULT			hr = _k1_sdk_p_nui_instance->NuiImageStreamGetNextFrame(_k1_hd_stream_depth, 0, &nui_depth_frame);
	if( FAILED(hr) )
		return;

	CONST	UINT32	sx = get_size_x();
	CONST	UINT32	sy = get_size_y();

	bool b_more_data_need = (_ui->_b_rgb_align_asked_ui && _k1_sdk_p_mapper) || _ui->_b_depth_range_extended_ui;
	INuiFrameTexture*		tex_depth_extended = nullptr;
	NUI_DEPTH_IMAGE_PIXEL*	depth_ext = nullptr;

	_b_k1_sdk_mapping_ok = false;
	if( b_more_data_need )
	{
		//	we need to do this to get past 4m starting from 1.6 sdk (tested on 1.8)
		BOOL b_near_mode;
		if( FAILED(_k1_sdk_p_nui_instance->NuiImageFrameGetDepthImagePixelFrameTexture(_k1_hd_stream_depth, &nui_depth_frame, &b_near_mode, &tex_depth_extended)) )
			return;

//		DBG_HEAP_IS_CORRUPT();
		NUI_LOCKED_RECT depth_rect_locked;
		tex_depth_extended->LockRect(0, &depth_rect_locked, nullptr, 0);
		if( depth_rect_locked.Pitch != 0 )
		{
			// now we can access field .depth and .playerIndex using this
			depth_ext = reinterpret_cast<NUI_DEPTH_IMAGE_PIXEL*>(depth_rect_locked.pBits);
			//	if we need to map rgb and depth
			if( _ui->_b_rgb_align_asked_ui && _k1_sdk_p_mapper )
			{
				NUI_IMAGE_RESOLUTION	depth_res = (sx == 640) ? NUI_IMAGE_RESOLUTION_640x480 : NUI_IMAGE_RESOLUTION_320x240;

				HRESULT	hr;
				//hr = _p_nui_instance->NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(
				//										NUI_IMAGE_RESOLUTION_640x480,		// color resolution,	only 640x480 in aaaseed for now
				//										depth_res,							// depth resolution
				//										sx * sy,							// number of depth values
				//										_depth_buffer,	 					// pointer to depth values array
				//										2 * sx * sy,						// number of color coordinates
				//										_mapped_coords						// pointer to color coordinates array
				//										);

				//	_p_nui_instance->NuiGetCoordinateMapper(&_p_mapper);
				hr = _k1_sdk_p_mapper->MapDepthFrameToColorFrame(depth_res,
					sx * sy,
					depth_ext,
					NUI_IMAGE_TYPE_COLOR,
					NUI_IMAGE_RESOLUTION_640x480,
					sx * sy,	//640 * 480,
					reinterpret_cast<NUI_COLOR_IMAGE_POINT*>(_k1_sdk_mapped_coords)
					);
				// output is wrong, bug inside !!!
				//hr = _p_mapper->MapColorFrameToDepthFrame(	NUI_IMAGE_TYPE_COLOR, 
				//											NUI_IMAGE_RESOLUTION_640x480,
				//											depth_res,
				//											sx * sy,
				//											reinterpret_cast<NUI_DEPTH_IMAGE_PIXEL*>(lockedRectDepth.pBits),
				//											dst_color_sx * dst_color_sy,
				//											reinterpret_cast<NUI_DEPTH_IMAGE_POINT*>(_depth_points)
				//											);

				if( FAILED(hr) )
				{
					ERR_PRINT_STRING("%s(), error with NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution", __FUNCTION__);
				}
				else
				{
					_b_k1_sdk_mapping_ok = true;
				}
			}
		}
	}

	INuiFrameTexture*	texture = nui_depth_frame.pFrameTexture;
	NUI_LOCKED_RECT		tex_info;
	texture->LockRect(0, &tex_info, nullptr, 0);
	if( tex_info.Pitch == 0 )
	{
		ERR_PRINT_STRING( "Buffer length of received depth texture is bogus" );
		goto exit;
	}

	/*
	//BYTE* pBuffer = (BYTE*) tex_info.pBits;
	// draw the bits to the bitmap
	RGBQUAD * rgbrun = m_rgbWk;
	USHORT * pBufferRun = (USHORT*) pBuffer;
	for( int y = 0 ; y < 240 ; y++ )
	{
	for( int x = 0 ; x < 320 ; x++ )
	{
	RGBQUAD quad = Nui_ShortToQuad_Depth( *pBufferRun );
	++pBufferRun;
	*rgbrun = quad;
	++rgbrun;
	}
	}
	*/
	{
		std::lock_guard<c_kinect_ui> guard(*_ui);

		unsigned short* src = (unsigned short*)tex_info.pBits - 1;

		CONST	INT32	DMAX = 4096;
		CONST	INT32	DMAXM1 = DMAX - 1;
		CONST	INT32	i_min = INT32(_ui->_depth_raw_min_ui * (DMAX - .001));	//c_kinect_listener::DEPTH_RANGE;
		CONST	INT32	i_max = INT32(_ui->_depth_raw_max_ui * (DMAX - .001));	//c_kinect_listener::DEPTH_RANGE;
		CONST	INT32	factor_int = (i_max != i_min) ? (1 << 30) / (i_max - i_min) : 0;


		CONST	bool	b_player_data = true;

		_ui->init_bodys();
		struct c_kinect_ui::st_rect*		rects	= &(_ui->_bodys_rect[0]);
		bool*								b_body	= &(_ui->_b_bodys[0]);

		MAKE_COLOR_INVALID;
		CONST	bool	b_raw_clip		= _ui->_b_depth_raw_clip_ui;

		CONST	INT32	x_left			= _ui->_b_depth_clip_side_ui ? CLAMP( UINT32(sx * _ui->_depth_clip_left_ui	),	UINT32(0),	sx ) : 0;
		CONST	INT32	x_right			= _ui->_b_depth_clip_side_ui ? CLAMP( UINT32(sx * _ui->_depth_clip_right_ui	),	UINT32(0),	sx ) : sx;
		CONST	INT32	y_bottom		= _ui->_b_depth_clip_side_ui ? CLAMP( UINT32(sy * _ui->_depth_clip_bottom_ui),	UINT32(0),	sy ) : 0;
		CONST	INT32	y_top			= _ui->_b_depth_clip_side_ui ? CLAMP( UINT32(sy * _ui->_depth_clip_top_ui	),	UINT32(0),	sy ) : sy;

		CONST	INT32	dst_stride		= sx * 4;


		INT32	x;
		INT32	y = sy - 1;
		//	fill the empty part at the top 
		for( ; y >= y_top; --y )
		{
			UINT8* dst = _buf_depth + y * dst_stride - 1;
			for( x = sx; x > 0; --x )	//todo	use a fill fn()
			{
				set_pixel( dst, 0 );
			}
			src += sx;
		}
		INT32	render_type = _ui->_s_depth_transform_type_ui;

	//	DBG_HEAP_IS_CORRUPT();
		switch( render_type )
		{
		case c_kinect_ui::DEPTH_TRANSFORM_RAW:
			for( ; y >= y_bottom; --y )
			{
				UINT8*	dst = _buf_depth + y * dst_stride - 1;
				INT32	skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));

				fill_pixel_left( dst, x, x_left );
				src += x;
				for( ; x < x_right; ++x )
				{
					INT32	val = *++src;	// gam[*++src];
					//USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
					//USHORT Player = hasPlayerData ? s & 7 : 0;
					//INT32* p_color = process_pixel_in<false>( val, b_player_data, b_body, rects, x );
					process_pixel_in<false>( val, b_player_data, b_body, rects, x );

					val += skew;
					set_pixel_grey_alpha( dst, val % 256, 255 );
				}
				src += sx - x;
				fill_pixel_right( dst, x, sx );
			}
			break;
		case c_kinect_ui::DEPTH_TRANSFORM_ZEPHOD:
			for( ; y >= y_bottom; --y )
			{
				UINT8*	dst = _buf_depth + y * dst_stride - 1;
				INT32	skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));

				fill_pixel_left( dst, x, x_left );
				src += x;
				for( ; x < x_right; ++x )
				{
					INT32	val = *++src;	// gam[*++src];
					//USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
					//USHORT Player = hasPlayerData ? s & 7 : 0;
					//INT32* p_color = process_pixel_in<false>( val, b_player_data, b_body, rects, x );
					process_pixel_in<false>( val, b_player_data, b_body, rects, x );

					val += skew;

					if (val == 0)
						{	SET_PIXEL_INVALID();	}
					else
					{
						//					val = DMAXM1 - val;
						//if( INSIDE_MIN_MAX( val, i_min, i_max ) )
						{
							//						val = val >> 1;	//go back to 11 bits
							int lb = val & 0xff;
							switch (val >> 8)
							{
							case 0:		set_pixel_rgba( dst, 255,		255 - lb,	255 - lb,	255);	break;
							case 1:		set_pixel_rgba( dst, 255,		lb,			0,			255);	break;
							case 2:		set_pixel_rgba( dst, 255 - lb,	255,		lb,			255);	break;
							case 3:		set_pixel_rgba( dst, 0,			255,		lb,			255);	break;
							case 4:		set_pixel_rgba( dst, 0,			255 - lb,	255,		255);	break;
							case 5:		set_pixel_rgba( dst, 0,			0,			255 - lb,	255);	break;
							case 6:		set_pixel_rgba( dst, 0,			128,		128,		128);	break;
							case 7:		set_pixel_rgba( dst, 0,			128,		128,		128);	break;
							case 8:		set_pixel_rgba( dst, 0,			128,		128,		128);	break;
							default:	set_pixel_rgba( dst, 0,			255 - lb,	255,		255);	break;
							}
						}
						//else
						//	set_pixel_ga( dst, val_invalid, alpha_invalid );
					}
				}
				src += sx - x;
				fill_pixel_right( dst, x, sx );
			}
			break;
		case c_kinect_ui::DEPTH_TRANSFORM_MAA:
			if( depth_ext )
			{
				for( ; y >= y_bottom; --y )
				{
					UINT8*	dst = _buf_depth + y * dst_stride - 1;
					INT32	skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));

					fill_pixel_left( dst, x, x_left );
					depth_ext += x;
					for( ; x < x_right; ++x )
					{
						//INT32	val = *++src;	// gam[*++src];
						//USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
						//USHORT Player = hasPlayerData ? s & 7 : 0;
						//INT32* p_color = process_pixel_in<false>( val, b_player_data, b_body, rects, x );
						//process_pixel_in<false>( val, b_player_data, b_body, rects, x );

						//todo do players too
						INT32 val = depth_ext->depth + skew;
						++depth_ext;
						if( val == 0 )	SET_PIXEL_INVALID();
						else
						{
							val = DMAXM1 - val;
							if( INSIDE_MIN_MAX( val, i_min, i_max ) )
							{
								val = ((val - i_min) * factor_int) >> 14;	//go back to 16 bits
								set_pixel_rgba( dst, val & 0xff, val >> 8, 0, 255 );
							}
							else	SET_PIXEL_OUT( val );
						}
					}
					depth_ext += sx - x;
					fill_pixel_right( dst, x, sx );
				}
			}
			else
			{
				for (; y >= y_bottom; --y)
				{
					UINT8*	dst = _buf_depth + y * dst_stride - 1;
					INT32	skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));

					fill_pixel_left( dst, x, x_left );
					src += x;
					for( ; x < x_right; ++x )
					{
						INT32	val = *++src;	// gam[*++src];
						//USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
						//USHORT Player = hasPlayerData ? s & 7 : 0;
						//INT32* p_color = process_pixel_in<false>( val, b_player_data, b_body, rects, x );
						process_pixel_in<false>( val, b_player_data, b_body, rects, x );

						val += skew;
						if( val == 0 )	SET_PIXEL_INVALID();
						else
						{
							val = DMAXM1 - val;
							if( INSIDE_MIN_MAX( val, i_min, i_max ) )
							{
								val = ((val - i_min) * factor_int) >> 14;	//go back to 16 bits
								set_pixel_rgba( dst, val & 0xff, val >> 8, 0, 255 );
							}
							else	SET_PIXEL_OUT( val );
						}
					}
					src += sx - x;
					fill_pixel_right( dst, x, sx );
				}
			}
			break;
		case c_kinect_ui::VISU_DEPTH_444:
			{
	//			INT32 val_invalid = INT32((DMAX - .00001) * _ui->_depth_value_for_outside_ui);
				for( ; y >= y_bottom; --y )
				{
					UINT8* dst = _buf_depth + y * dst_stride - 1;
					INT32 skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));

					fill_pixel_left( dst, x, x_left );
					src += x;
					for( ; x < x_right; ++x )
					{
						INT32	val = *++src;	// gam[*++src];
						//USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
						//USHORT Player = hasPlayerData ? s & 7 : 0;
						INT32 const * const p_color = process_pixel_in<false>( val, b_player_data, b_body, rects, x );

						val += skew;
						if( val == 0 )	SET_PIXEL_INVALID();
						else
						{
							INT32 val4 = DMAXM1 - val;
							if( INSIDE_MIN_MAX( val4, i_min, i_max ) )
							{
								val4 = ((val4 - i_min) * factor_int) >> 26;	//go back to 4 bits
								//rgb ?
								set_pixel_bgra(dst,
									( (*(p_color    ) * val4) << 4 ) | ( (val >> 8) & 0xf),
									( (*(p_color + 1) * val4) << 4 ) | ( (val >> 4) & 0xf),
									( (*(p_color + 2) * val4) << 4 ) | ( (val     ) & 0xf),
									//(val>>8) & 0xf ), (val>>4) & 0xf ), (val ) & 0xf,
									255);
							}
							else	SET_PIXEL_OUT( val );
						}
					}
					src += sx - x;
					fill_pixel_right( dst, x, sx );
				}
			}
			break;
		case c_kinect_ui::DEPTH_TRANSFORM_R8:
			if( depth_ext )
			{
				for( ; y >= y_bottom; --y )
				{
					UINT8*	dst = _buf_depth + y * dst_stride - 1;
					INT32	skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));

					fill_pixel_left( dst, x, x_left );
					depth_ext += x;
					for( ; x < x_right; ++x )
					{
						//INT32 val = *++src;	// gam[*++src];
						//USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
						//USHORT Player = hasPlayerData ? s & 7 : 0;
						//process_pixel_in<true>( val, b_player_data, b_body, rects, x );

						//todo do players too
						INT32 val = depth_ext->depth + skew;
						++depth_ext;
						if( val == 0 )	SET_PIXEL_INVALID();
						else
						{
							val = DMAXM1 - val;
							if( INSIDE_MIN_MAX( val, i_min, i_max ) )
							{
								val = ((val - i_min) * factor_int) >> 14;	//go back to 16 bits
								val = val >> 8;
								set_pixel_grey_alpha( dst, val, 255 );
							}
							else	SET_PIXEL_OUT( val );
						}
					}
					depth_ext += sx - x;
					fill_pixel_right( dst, x, sx );
				}
			}
			else
			{
				for( ; y >= y_bottom; --y )
				{
					UINT8*	dst = _buf_depth + y * dst_stride - 1;
					INT32	skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));

					fill_pixel_left( dst, x, x_left );
					src += x;
					for( ; x < x_right; ++x )
					{
						INT32 val = *++src;	// gam[*++src];
						//USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
						//USHORT Player = hasPlayerData ? s & 7 : 0;
						process_pixel_in<true>( val, b_player_data, b_body, rects, x );

						val += skew;
						if( val == 0 )	SET_PIXEL_INVALID();
						else
						{
							val = DMAXM1 - val;
							if( INSIDE_MIN_MAX(val, i_min, i_max) )
							{
								val = ((val - i_min) * factor_int) >> 14;	//go back to 16 bits
								val = val >> 8;
								set_pixel_grey_alpha( dst, val, 255 );
							}
							else	SET_PIXEL_OUT( val );
						}
					}
					src += sx - x;
					fill_pixel_right( dst, x, sx );
				}
			}
			break;
		case c_kinect_ui::DEPTH_TRANSFORM_WHITE:
			for( ; y >= y_bottom; --y )
			{
				UINT8*	dst = _buf_depth + y * dst_stride - 1;
				INT32	skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));

				fill_pixel_left( dst, x, x_left );
				src += x;
				for( ; x < x_right; ++x )
				{
					INT32 val = *++src;	// gam[*++src];
					//USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
					//USHORT Player = hasPlayerData ? s & 7 : 0;
					INT32 const * const p_color = process_pixel_in<false>( val, b_player_data, b_body, rects, x );
					val += skew;

					if( val == 0 )	SET_PIXEL_INVALID();
					else
					{
						val = DMAXM1 - val;
						if( INSIDE_MIN_MAX( val, i_min, i_max ) )
						{
							set_pixel_grey_alpha( dst, 255, 255);
						}
						else	SET_PIXEL_OUT( val );
					}
				}
				src += sx - x;
				fill_pixel_right( dst, x, sx );
			}
			break;
		default:
			//	put this message in sync code not here in callback 
			//		KINECT_PRINT_STRING( "render as %s unimplemented for kinect sdk", str_depth_transform_type[render_type] );
			//		KINECT_PRINT_STRING( "\tusing %s instead", str_depth_transform_type[DEPTH_TRANSFORM_GREY] );
		case c_kinect_ui::DEPTH_TRANSFORM_GREY:
			for( ; y >= y_bottom; --y )
			{
				UINT8*	dst = _buf_depth + y * dst_stride - 1;
				INT32	skew = INT32(_ui->_depth_skew_vert_ui * (y - sy * .5));

				fill_pixel_left( dst, x, x_left );
				src += x;
				for( ; x < x_right; ++x )
				{
					INT32 val = *++src;	// gam[*++src];
					//USHORT RealDepth = hasPlayerData ? (s & 0xfff8) >> 3 : s & 0xffff;
					//USHORT Player = hasPlayerData ? s & 7 : 0;
					INT32 const * const p_color = process_pixel_in<false>( val, b_player_data, b_body, rects, x );
					val += skew;

					if( val == 0 )	SET_PIXEL_INVALID();
					else
					{
						val = DMAXM1 - val;
						if( INSIDE_MIN_MAX( val, i_min, i_max ) )
						{
							val = ((val - i_min) * factor_int) >> 14;	//go back to 16 bits
							val = val >> 8;
							set_pixel_bgra( dst, val * *p_color, val * *(p_color + 1), val * *(p_color + 2), 255 );
						}
						else	SET_PIXEL_OUT( val );
					}

				}
				src += sx - x;
				fill_pixel_right( dst, x, sx );
			}
			break;
			/*
			case c_kinect_ui::DEPTH_TRANSFORM_Z_BLUE_RED:
			case c_kinect_ui::DEPTH_TRANSFORM_Z_BLUE_GREEN:
			case c_kinect_ui::DEPTH_TRANSFORM_Z_WHITE_GREEN:
			{
			REAL	min		= _kinect_ui->_depth_meter_min_ui * .01;
			REAL	central	= _kinect_ui->_depth_meter_central_ui * .01;
			REAL	max		= _kinect_ui->_depth_meter_max_ui * .01;
			REAL	factor;
			if( max != central )
			factor	= 1. / ( central - max );
			else
			factor = 0;
			factor *= REAL_NEARLY_256*256;	// we work on 16 bits
			for( INT32 y = 480 - 1; y >= 0; --y )
			{
			UINT8* dst = _buffer_data + y * 640 * 4 - 1;
			for( INT32 x = 0; x < 640; ++x )
			{
			// calibrated by nicolas.burrus
			REAL val = Kinect::raw_to_cm( *++src );
			//printf( "%f\n", val );
			//if( val == 0x7ff ) val=0;
			if( min <= val && val <= max )
			{
			if( val >= central )
			{
			INT32 v = (val-max) * factor;
			switch( render_type )
			{
			case DEPTH_TRANSFORM_Z_BLUE_RED:
			case DEPTH_TRANSFORM_Z_BLUE_GREEN:
			*++dst = UINT8( v>>8 );	//b
			*++dst = 0;				//g
			*++dst = 0;				//r
			break;
			case DEPTH_TRANSFORM_Z_WHITE_GREEN:
			*++dst = UINT8( v>>8 );	//b
			*++dst = UINT8( v>>8 );	//g
			*++dst = UINT8( v>>8 );	//r
			break;
			}
			}
			else
			{
			switch( render_type )
			{
			case DEPTH_TRANSFORM_Z_BLUE_RED:	*++dst = 0;		*++dst = 0;		*++dst = 0xff;	break;
			case DEPTH_TRANSFORM_Z_BLUE_GREEN:	*++dst = 0;		*++dst = 0xff;	*++dst = 0;		break;
			case DEPTH_TRANSFORM_Z_WHITE_GREEN:	*++dst = 0xff;	*++dst = 0xff;	*++dst = 0xff;	break;
			}
			}
			*++dst = 255;
			}
			else
			set_pixel( dst, 0 );
			}
			}
			}
			break;
			*/
			/*
			case DEPTH_TRANSFORM_Z_TO_GREY:
			{
			REAL	min = _kinect_ui->_depth_min_ui * .01;	//c_kinect_listener::DEPTH_RANGE;
			REAL	max = _kinect_ui->_depth_meter_max_ui * .01;	//c_kinect_listener::DEPTH_RANGE;
			REAL	factor;
			if( max != min )
			factor	= 1. / ( min - max );
			else
			factor = 0;
			factor *= REAL_NEARLY_256*256;	// we work on 16 bits
			for( INT32 y = 480 - 1; y >= 0; --y )
			{
			UINT8* dst = _buffer_data + y * 640 * 4 - 1;
			for( INT32 x = 0; x < 640; ++x )
			{
			// calibrated by nicolas.burrus
			REAL val = Kinect::raw_to_cm( *++src );
			//printf( "%f\n", val );
			//if( val == 0x7ff ) val=0;
			if( min <= val && val <= max )
			{
			INT32 v = (val-max) * factor;
			*++dst = UINT8( v & 0xff	);	//b
			*++dst = UINT8( v>>8		);	//g
			*++dst = UINT8( v>>8		);	//r
			*++dst = 255;
			}
			else
			set_pixel( dst, 0 );
			}
			}
			}
			break;
			*/
		}
		if( tex_depth_extended )
		{
			tex_depth_extended->UnlockRect(0);
			tex_depth_extended = nullptr;
		}
		texture->UnlockRect(0);

		//	fill the empty part at the bottom
		for( ; y >= 0; --y )
		{
			UINT8* dst = _buf_depth + y * dst_stride - 1;
				//todo	use a fill fn()
			for( INT32 x = sx; x > 0; --x )
			{
				set_pixel(dst, 0);
			}
		}

		erase_rects_depth( sx, sy, dst_stride );
	//	DBG_HEAP_IS_CORRUPT();

		INT32 nb = 0;
		for( INT32 i = 0; i<c_kinect_ui::NB_BODY_MAX; ++i )
		{
			if( b_body[i] )
				++nb;
		}
		_ui->_body_seen_nb = nb;
	}

//	DBG_HEAP_IS_CORRUPT();
	got_frame( reinterpret_cast<UINT8*>(_buf_depth), "Kinect Depth" );
//	DBG_HEAP_IS_CORRUPT();

exit:
	if( tex_depth_extended )	{	tex_depth_extended->UnlockRect(0);	}
	_k1_sdk_p_nui_instance->NuiImageStreamReleaseFrame( _k1_hd_stream_depth, &nui_depth_frame );
}

void	c_capture_kinect::do_depth_stuff()
{
	if (_k1_kinect_listener == nullptr)
		return;
	Kinect::Kinect *K = _k1_kinect_listener->_k1_kinect;

	K->ParseDepthBuffer();

	unsigned short* src = K->mDepthBuffer - 1;

	CONST	INT32	DMAX = 4096;
	CONST	INT32	DMAXM1 = DMAX - 1;
	CONST	INT32	i_min = INT32(_ui->_depth_raw_min_ui * 2047.999);	//c_kinect_listener::DEPTH_RANGE;
	CONST	INT32	i_max = INT32(_ui->_depth_raw_max_ui * 2047.999);	//c_kinect_listener::DEPTH_RANGE;
	CONST	INT32	factor_int = (i_max != i_min) ? (1 << 30) / (i_max - i_min) : 0;

	//INT32	val_invalid = INT32(REAL_NEARLY_256 * _ui->_depth_value_for_outside_ui);
	MAKE_COLOR_INVALID;

	INT32	render_type = _ui->_s_depth_transform_type_ui;
	switch( render_type )
	{
	case c_kinect_ui::DEPTH_TRANSFORM_ZEPHOD:
		{
			unsigned short* gam = _k1_kinect_listener->t_gamma;
			for( INT32 y = 480 - 1; y >= 0; --y )
			{
				UINT8* dst = _buf_depth + y * 640 * 4 - 1;
				for( INT32 x = 0; x < 640; ++x )
				{
					int pval = gam[*++src];
					int lb = pval & 0xff;
					switch( pval >> 8 )
					{
					case 0:		set_pixel_bgra( dst, 255,		255 - lb,	255 - lb,	255);	break;
					case 1:		set_pixel_bgra( dst, 255,		lb,			0,			255);	break;
					case 2:		set_pixel_bgra( dst, 255 - lb,	255,		0,			255);	break;
					case 3:		set_pixel_bgra( dst, 0,			255,		lb,			255);	break;
					case 4:		set_pixel_bgra( dst, 0,			255 - lb,	255,		255);	break;
					case 5:		set_pixel_bgra( dst, 0,			0,			255 - lb,	255);	break;
					default:	set_pixel_bgra( dst, 0,			0,			0,			0);		break;
					}
				}
			}
		}
		break;
	case c_kinect_ui::DEPTH_TRANSFORM_MAA:
		for( INT32 y = 480 - 1; y >= 0; --y )	
		{
			UINT8* dst = _buf_depth + y * 640 * 4 - 1;
			for( INT32 x = 0; x < 640; ++x )
			{
				INT32 val = *++src;	// gam[*++src];
				//if( val == 0x7ff ) val=0;
				val = 2047 - val;
				if (i_min <= val && val <= i_max)
				{
					val = ((val - i_min) * factor_int) >> 14;	//go back to 16 bits
					set_pixel_bgra( dst, val & 0xff, val >> 8, 0, (val == 0) ? 0 : 255 );
				}
				else SET_PIXEL_INVALID();
			}
		}
		break;
	case c_kinect_ui::DEPTH_TRANSFORM_GREY:
		for( INT32 y = 480 - 1; y >= 0; --y )
		{
			UINT8* dst = _buf_depth + y * 640 * 4 - 1;
			for( INT32 x = 0; x < 640; ++x )
			{
				INT32 val = *++src;	// gam[*++src];
				//if( val == 0x7ff ) val=0;
				val = 2047 - val;
				if( i_min <= val && val <= i_max )
				{
					val = ((val - i_min) * factor_int) >> 14;	//go back to 16 bits
					set_pixel_grey_alpha(dst, val >> 8, (val == 0) ? 0 : 255);
				}
				else SET_PIXEL_INVALID();
			}
		}
		break;
	case c_kinect_ui::DEPTH_TRANSFORM_WHITE:
		for( INT32 y = 480 - 1; y >= 0; --y )
		{
			UINT8* dst = _buf_depth + y * 640 * 4 - 1;
			for( INT32 x = 0; x < 640; ++x )
			{
				INT32 val = *++src;	// gam[*++src];
				//if( val == 0x7ff ) val=0;
				val = 2047 - val;
				if( i_min <= val && val <= i_max )
					set_pixel_grey_alpha( dst, 255, 255 );
				else
					SET_PIXEL_INVALID();
			}
		}
		break;
	case c_kinect_ui::DEPTH_TRANSFORM_Z_BLUE_RED:
	case c_kinect_ui::DEPTH_TRANSFORM_Z_BLUE_GREEN:
	case c_kinect_ui::DEPTH_TRANSFORM_Z_WHITE_GREEN:
	{
		REAL	min		= _ui->_depth_meter_min_ui * REAL(.01);
		REAL	central = _ui->_depth_meter_central_ui *  REAL(.01);
		REAL	max		= _ui->_depth_meter_max_ui *  REAL(.01);
		REAL	factor;
		// we work on 16 bits
		if( max != central )
			factor =  REAL(aaa::img::DOUBLE_NEARLY_256_x_256 / (central - max));
		else
			factor = 0;
		for( INT32 y = 480 - 1; y >= 0; --y )
		{
			UINT8* dst = _buf_depth + y * 640 * 4 - 1;
			for( INT32 x = 0; x < 640; ++x )
			{
				// calibrated by nicolas.burrus
				REAL val = Kinect::raw_to_cm(*++src);
				//printf( "%f\n", val );
				//if( val == 0x7ff ) val=0;
				if( min <= val && val <= max )
				{
					if( val >= central )
					{
						INT32 v = INT32((val - max) * factor);
						switch (render_type)
						{
						case c_kinect_ui::DEPTH_TRANSFORM_Z_BLUE_RED:
						case c_kinect_ui::DEPTH_TRANSFORM_Z_BLUE_GREEN:
							set_pixel_bgr( dst, UINT8(v >> 8), 0, 0 );
							break;
						case c_kinect_ui::DEPTH_TRANSFORM_Z_WHITE_GREEN:
							set_pixel_bgr( dst, UINT8(v >> 8), UINT8(v >> 8), UINT8(v >> 8) );
							break;
						}
					}
					else
					{
						switch (render_type)
						{
						case c_kinect_ui::DEPTH_TRANSFORM_Z_BLUE_RED:		set_pixel_bgr( dst,	0,		0,		0xff);	break;
						case c_kinect_ui::DEPTH_TRANSFORM_Z_BLUE_GREEN:		set_pixel_bgr( dst, 0,		0xff,	0	);	break;
						case c_kinect_ui::DEPTH_TRANSFORM_Z_WHITE_GREEN:	set_pixel_bgr( dst, 0xff,	0xff,	0xff);	break;
						}
					}
					*++dst = 255;
				}
				else SET_PIXEL_INVALID();
			}
		}
	}
	break;
	case c_kinect_ui::DEPTH_TRANSFORM_Z_TO_GREY:
	{
		REAL	min = _ui->_depth_meter_min_ui *  REAL(.01);	//c_kinect_listener::DEPTH_RANGE;
		REAL	max = _ui->_depth_meter_max_ui *  REAL(.01);	//c_kinect_listener::DEPTH_RANGE;
		REAL	factor;
		// we work on 16 bits
		if( max != min )
			factor = REAL( aaa::img::DOUBLE_NEARLY_256_x_256 / (min - max) );
		else
			factor = 0;

		for( INT32 y = 480 - 1; y >= 0; --y )
		{
			UINT8* dst = _buf_depth + y * 640 * 4 - 1;
			for( INT32 x = 0; x < 640; ++x )
			{
				// calibrated by nicolas.burrus
				REAL val = Kinect::raw_to_cm( *++src );
				//printf( "%f\n", val );
				//if( val == 0x7ff ) val=0;
				if( min <= val && val <= max )
				{
					INT32 v = INT32( (val - max) * factor );
					set_pixel_bgra( dst, UINT8(v & 0xff), UINT8(v >> 8), UINT8(v >> 8), 255 );
				}
				else SET_PIXEL_INVALID();
			}
		}
	}
	break;
	}
	erase_rects_depth( 640, 480, 640 * 4 );
	got_frame( reinterpret_cast<UINT8*>(_buf_depth), "Kinect depth" );
}