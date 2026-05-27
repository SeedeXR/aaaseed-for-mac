
#ifdef AAA_DEINTERLACE_LEAKKERNEL_H
#error "DEINTERLACE_LEAKKERNEL_H included more than once."
#endif
#define AAA_DEINTERLACE_LEAKKERNEL_H 1


#ifndef	AAA_DEINTERLACE_H
#	include "../deinterlace.h"
#endif

struct	KernelDeintBuffer;
typedef	struct	KernelDeintBuffer	KernelDeintMask;

class	c_deint_leak_kernel final : public c_deinterlace
{
	FACTORY_DECLARE(c_deint_leak_kernel,c_deinterlace);
private:
	UINT8*			_src_n;		// n+1
	UINT8*			_src_n_1;	// n
	UINT8*			_src_n_2;	// n-1
	UINT8*			_src_n_3;	// n-2

	INT32			_scratchPitch;
	UINT8*			_scratch;

	// kernel deinterlacing
	INT32			_threshold;
	BOOL			_b_sharp;
	BOOL			_b_two_way;

	INT32			_order;
	BOOL			_b_use_mmx;

	KernelDeintMask*	_fullsizeMask;
	BOOL				_b_masksFilled;

	virtual	void	run_frame( UINT8* src, UINT8* dst );
			void	run_field( UINT8* src, UINT8* dst, BOOL second_pass );

			void	KernelDeint_RGB32( UINT8* p_src, UINT8* p_dst, BOOL second_pass );
			void	BuildMotionMask_RGB32( KernelDeintMask* mask, INT32 order, INT32 n, INT32 threshold );
			void	BuildMotionMask_RGB32_C( KernelDeintMask* mask, INT32 order, INT32 n, INT32 threshold );
			void	BuildMotionMask_RGB32_MMX( KernelDeintMask* mask, INT32 order, INT32 n, INT32 threshold );
			void	ShowMotionMask_RGB32( UINT8* p_dst, INT32 order, KernelDeintMask* mask );
			void	Deinterlace_RGB32( UINT8* p_dst, INT32 order, INT32 n, KernelDeintMask* mask, BOOL sharp, BOOL twoway );
			void	Deinterlace_RGB32_C( UINT8* p_dst, INT32 order, INT32 n, KernelDeintMask* mask, BOOL sharp, BOOL twoway );
			void	Deinterlace_RGB32_MMX( UINT8* p_dst, INT32 order, INT32 n, KernelDeintMask* mask, BOOL sharp, BOOL twoway );

public:
	virtual	void	init( INT32 size_x, INT32 size_y );
	virtual	void	close();

	virtual	void	run_full( c_image_flux* flux_in, UINT8* dst, BOOL second_pass );

	virtual	void	param_init_pt();
	virtual	void	update() {};
	virtual	BOOL	is_field_needed( BOOL b_double_framerate = FALSE );
};


