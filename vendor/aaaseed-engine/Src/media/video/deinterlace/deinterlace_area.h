
#ifdef AAA_DEINTERLACE_AREA_H
#error "DEINTERLACE_AREA_H included more than once."
#endif
#define AAA_DEINTERLACE_AREA_H 1


#ifndef	AAA_DEINTERLACE_H
#	include"../deinterlace.h"
#endif

class c_deint_area final : public c_deinterlace
{
	FACTORY_DECLARE(c_deint_area,c_deinterlace);
private:
	UINT8*	_merge_src;

	// motion map, full resolution
	//INT32*	s_map_full_;

	// area deinterlacing
	INT32		_threshold;
	BOOL		_b_blend;
	INT32		_edge_threshold;

	void run_frame( UINT8* p_src, UINT8* p_dst );
	void area_32_field( UINT8* p_src, UINT8* p_src_next, UINT8* p_dst );

public:

	virtual	void	init( INT32 size_x, INT32 size_y );
	virtual	void	close();

	virtual	void	run_full( c_image_flux* flux_in, UINT8* dst, BOOL second_pass );

	virtual	void	param_init_pt();
	virtual	void	update() {};
	virtual	BOOL	is_field_needed( BOOL b_double_framerate = FALSE );
};



