

#ifdef AAA_DEINTERLACE_H
#error "DEINTERLACE_H included more than once."
#endif
#define AAA_DEINTERLACE_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class c_image_flux;

class c_deinterlace final : public c_obj_ui
{
	FACTORY_ABSTRACT_DECLARE(c_deinterlace,c_obj_ui);
public:
	enum BLEND_TYPE : INT32
	{
		COPY = 0,
		BLEND, 
		CUBIC,
		ELA,
		BLEND_MAX_NB,
	};

	enum SIMD_TYPE : INT32
	{
		SIMD_NONE = 0,
		SIMD_MMX,
		SIMD_SSE2, 
		SIMD_MAX_NB,
	};

	enum METHOD_TYPE : INT32
	{
		WEAVE = 0,
		SIMPLE_BOB,
//		AREA,
//		SMART,
//		SMOOTH,
		KERNEL,
		LEAK_KERNEL,
		METHOD_MAX_NB,
	};

	enum DEINTERLACE_TYPE : INT32
	{
		DEINTERLACE_NO = 0,
		DEINTERLACE_NORMAL,
		DEINTERLACE_DOUBLERATE,
		DEINTERLACE_TYPE_MAX_NB,
	};

protected:
	INT32		_min_image_nb;
	INT32		_size_out_y;
	INT32		_size_x;
	INT32		_size_y;

//	bool		_b_show_deinterlace;
//	bool		_b_doublerate;			// full deinterlacing
	bool		_b_is_processed;

	INT32		_nb_bit_out;

	bool		_b_create_buffers;

	INT32		_image_index;
	INT32		_field_order;

			void	simple_copy( UINT8 *src, UINT8 *dst );
	virtual	void	run_frame( UINT8* src, UINT8* dst );

public:
	virtual	void	init_with_size( INT32 size_x, INT32 size_y );
	virtual	void	init( INT32 size_x, INT32 size_y )	{}
	virtual	void	close()								{}

	virtual	void	set_size_x( INT32 size_x )						{ _size_x = size_x; }
	virtual	void	set_size_y( INT32 size_y )						{ _size_y = size_y; }
	virtual	void	set_create_buffers( bool p_in )					{ _b_create_buffers = p_in; }
//	virtual	void	set_show_deinterlace( bool p_in )				{ _b_show_deinterlace = p_in; }
	virtual	void	set_image_index( INT32 p_in )					{ _image_index = p_in; }
	virtual	void	set_field_order( INT32 p_in )					{ _field_order = p_in; }
//	virtual	void	set_doublerate( bool p_in )						{ _b_doublerate = p_in; }

	virtual	INT32	get_size_out_y()								{ return _size_out_y; }
	virtual INT32	get_nb_bit_out()								{ return _nb_bit_out; }
	virtual	INT32	get_min_image_nb()								{ return _min_image_nb; }

	virtual	bool	is_processed()									{ return _b_is_processed; }

	virtual	void	run_half( c_image_flux* flux_in, UINT8* dst );
//	virtual	void	run_full( c_image_flux* flux_in, UINT8* dst, bool b_second_pass )		{}
//	virtual	bool	is_field_needed( bool b_double_framerate = false );

};

