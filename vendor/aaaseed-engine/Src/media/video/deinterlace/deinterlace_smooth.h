
#ifdef AAA_DEINTERLACE_SMOOTH_H
#error "DEINTERLACE_SMOOTH_H included more than once."
#endif
#define AAA_DEINTERLACE_SMOOTH_H 1


#ifndef	AAA_DEINTERLACE_H
#	include "../deinterlace.h"
#endif

class c_deint_smooth final : public c_deinterlace
{
	FACTORY_DECLARE(c_deint_smooth,c_deinterlace);
private:
	//UINT8*		merge_src_;

	// motion map, full resolution
	//INT32*		s_map_full_;

	// smooth deinterlacing
	UINT32*		_pframe;
	UINT32*		_pframeprev;
	INT32*		_piFrameDiffs;
	INT32		_iFrameNo;

	// smooth deinterlacing
	INT32		_smooth_threshold;
	BOOL		_b_smooth_blend;
	BOOL		_b_smooth_alt_order;
	INT32		_smooth_edge_threshold;
	INT32		_smooth_interframe_threshold;
	INT32		_smooth_interframe_average;


	virtual	void	run_frame( UINT8* src, UINT8* dst );
			void	smooth_32(UINT8* p_src, UINT8* p_dst, INT32 lCurrentSourceFrame);

public:

	virtual	void	init( INT32 size_x, INT32 size_y, INT32 iInterFrameLeaveThreshold);
	virtual	void	close();

	virtual	void	run_full( c_image_flux* flux_in, UINT8* dst, BOOL second_pass );

	virtual	void	param_init_pt();
	virtual	void	update() {};
	virtual	BOOL	is_field_needed( BOOL b_double_framerate = FALSE );

};


