
#ifdef AAA_DEINTERLACE_BOB_H
#error "DEINTERLACE_BOB_H included more than once."
#endif
#define AAA_DEINTERLACE_BOB_H 1


#ifndef	AAA_DEINTERLACE_H
#	include "../deinterlace.h"
#endif

class c_deint_bob final : public c_deinterlace
{
	FACTORY_DECLARE(c_deint_bob,c_deinterlace);
private:
	virtual	void	run_frame( UINT8* src, UINT8* dst );

public:
	void	init( INT32 size_x, INT32 size_y );
	void	close();

	virtual	void	run_full( c_image_flux* flux_in, UINT8* dst, BOOL second_pass );

	virtual	void	param_init_pt();
	virtual	void	update() {};
	virtual	BOOL	is_field_needed( BOOL b_double_framerate = FALSE );
};


