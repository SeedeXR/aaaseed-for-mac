
#ifdef AAA_DEINTERLACE_KERNEL_H
#error "DEINTERLACE_KERNEL_H included more than once."
#endif
#define AAA_DEINTERLACE_KERNEL_H 1


#ifndef	AAA_DEINTERLACE_H
#	include "../deinterlace.h"
#endif

class c_deint_kernel final : public c_deinterlace
{
	FACTORY_DECLARE(c_deint_kernel,c_deinterlace);
private:

	//UINT8*	g_moving;
	//UINT8*	g_fmoving;
	//UINT32	*prevFrame, *saveFrame;
	//UINT8	*s_prevFrame, *s_saveFrame, *s_moving, *s_fmoving;
	UINT8*		_s_prevFrame2;
	UINT8*		_merge_src;

//	INT32		final_height_;

	// kernel deinterlacing
	INT32		_threshold;
	BOOL		_b_sharp;
	BOOL		_b_two_way;

	virtual	void	run_frame( UINT8* src, UINT8* dst );

			void	kernel_32( UINT8* p_src, UINT8* p_dst, INT32 );// s_field_order);
			void	kernel_32_field( UINT8* p_src, UINT8* p_src_next, UINT8* p_dst );//, INT32 s_field_order);

public:

	virtual	void	init( INT32 size_x, INT32 size_y );
	virtual	void	close();

	virtual	void	run_full( c_image_flux* flux_in, UINT8* dst, BOOL second_pass );

	virtual	void	param_init_pt();
	virtual	void	update() {};
	virtual	BOOL	is_field_needed( BOOL b_double_framerate = FALSE );
};


