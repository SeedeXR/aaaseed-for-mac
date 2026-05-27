
#ifdef AAA_DEINTERLACE_WEAVE_H
#error "DEINTERLACE_WEAVE_H included more than once."
#endif
#define AAA_DEINTERLACE_WEAVE_H 1


#ifndef	AAA_DEINTERLACE_H
#	include "../deinterlace.h"
#endif

class	c_deint_weave final : public c_deinterlace
{
	FACTORY_DECLARE(c_deint_weave,c_deinterlace);
private:
	void			weave_32_field( UINT8* src, UINT8* src_prev, UINT8* dst );
	void			run_frame( UINT8* src, UINT8* dst );

public:

	virtual	void	init( INT32 size_x, INT32 size_y );
	virtual	void	close();

	virtual	void	run_full( c_image_flux* flux_in, UINT8* dst, BOOL second_pass );

	virtual	void	param_init_pt();
	virtual	void	update() {};
	virtual	BOOL	is_field_needed( BOOL b_double_framerate = FALSE );

};


