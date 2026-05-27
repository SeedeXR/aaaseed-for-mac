
#ifdef AAA_DEINTERLACE_SMART_H
#error "DEINTERLACE_SMART_H included more than once."
#endif
#define AAA_DEINTERLACE_SMART_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef	AAA_DEINTERLACE_H
#	include "../deinterlace.h"
#endif


class c_deint_smart final : public c_deinterlace
{
	FACTORY_DECLARE(c_deint_smart,c_deinterlace);
protected:
	UINT8			*_g_moving;
	UINT8			*_g_fmoving;
	UINT32			*_prevFrame, *_saveFrame;
//	UINT8			*s_prevFrame_, *s_saveFrame_, *s_moving_, *s_fmoving_;
//	UINT8			*s_prevFrame2;
	UINT8*			_merge_src;
	// motion map, full resolution
	//INT32*			s_map_full_;

	// smart deinterlacing
	INT32			_smart_threshold;
	INT32			_smart_blend_mode;
	INT32			_s_smart_motion_map_mode;
//	BOOL			_b_smart_motionmap_full;
	BOOL			_b_smart_colordiff;
	INT32			_smart_scene_threshold;
	BOOL			_b_smart_highquality;
//	BOOL			b_smart_32_bits_;
//	INT32			s_smart_simd_;


			void	smart_32_blend( UINT32* p_src, UINT32* p_dst, INT32 s_scenechange );
			void	smart_32_copy( UINT32* p_src, UINT32* p_dst, INT32 s_scenechange );
			void	smart_32_cubic( UINT32* p_src, UINT32* p_dst, INT32 s_scenechange );
			INT32	smart_32_field_motion_map_field_rgb( UINT32* p_src_a, UINT32* p_src_b );
			void	smart_32_motion_map_denoise();
			INT32	smart_32_motion_map_field_luma( UINT32* p_src );
			INT32	smart_32_motion_map_field_rgb( UINT32* p_src );
			INT32	smart_32_motion_map_frame_luma( UINT32* p_src );
			INT32	smart_32_motion_map_frame_rgb( UINT32* p_src );
			INT32	smart_32_motion_map_framefield_luma( UINT32* p_src );
			INT32	smart_32_motion_map_framefield_rgb( UINT32* p_src );


	virtual	void	run_frame(UINT8* p_src, UINT8* p_dst );
			UINT32	smart_32_field(UINT8* p_src, UINT8* p_src_next, UINT8* p_dst);


public:
	virtual	void	init( INT32 size_x, INT32 size_y );
	virtual	void	close();

	virtual	void	run_full( c_image_flux* flux_in, UINT8* dst, BOOL second_pass );

	virtual	void	param_init_pt();
	virtual	void	update() {};
	virtual	BOOL	is_field_needed( BOOL b_double_framerate = FALSE );
};


