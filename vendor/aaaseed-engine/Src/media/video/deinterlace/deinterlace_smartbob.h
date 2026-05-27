
#ifdef AAA_DEINTERLACE_SMARTBOB_H
#error "DEINTERLACE_SMARTBOB_H included more than once."
#endif
#define AAA_DEINTERLACE_SMARTBOB_H 1


#ifndef	AAA_DEINTERLACE_H
#	include "../deinterlace.h"
#endif

class c_deint_smartbob final : public c_deinterlace
{
	FACTORY_DECLARE( c_deint_smartbob, c_deinterlace );
private:
	UINT8		*_g_moving;
	UINT8		*_g_fmoving;
	UINT32		*_prevFrame;	//, *saveFrame_;

	// smart deinterlacing
	//INT32		smart_threshold_;
	//INT32		smart_blend_mode_;
	//INT32		s_smart_motion_map_mode_;
	//BOOL		b_smart_motionmap_full_;
	//BOOL		b_smart_colordiff_;
	//INT32		smart_scene_threshold_;
	//BOOL		b_smart_highquality_;
	//BOOL		b_smart_32_bits_;
	//INT32		s_smart_simd_;
	REAL		_denoise_diameter;
	REAL		_denoise_threshold;

	virtual	void	run_frame( UINT8* src, UINT8* dst );
			void	smartbob(UINT8* p_src, UINT8* p_dst, INT32 n);

public:

	virtual	void	init( INT32 size_x, INT32 size_y );
	virtual	void	close();

	virtual	void	run_full( c_image_flux* flux_in, UINT8* dst, BOOL second_pass );

	virtual	void	param_init_pt();
	virtual	void	update() {};
	virtual	BOOL	is_field_needed( BOOL b_double_framerate = FALSE );

};


