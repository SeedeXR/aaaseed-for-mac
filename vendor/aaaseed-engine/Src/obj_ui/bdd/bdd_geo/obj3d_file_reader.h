
#ifdef AAA_OBJ3D_FILE_READER_H
#error "OBJ3D_FILE_READER_H included more than once."
#endif
#define AAA_OBJ3D_FILE_READER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

//	polygon are deprecated now, the code to draw is still ok but we don't deal anymore with the index
#define AAA_POLY_COMPILE()	0

class c_bdd_tri;
class c_obj_info;
class c_file_io;

class	c_obj3d_file_info
{
public:
	UINT32	_obj	;
	UINT32	_point	;
	UINT32	_normal	;
	UINT32	_uv		;
	UINT32	_tri	;
	UINT32	_face	;
	bool	_b_poly	;

	c_obj3d_file_info();
};

class	c_obj3d_file_reader final : public c_obj
{
private:
	c_file_io*	_reader;

public:
	enum	F3D_TYPE : INT32
	{
		F3D_UNKNOWN = 0,
		F3D_NICHIMEN_GEO,
		F3D_ALIAS_OBJ,
		F3D_SVG,
		F3D_END_ENUM
	};

	F3D_TYPE	_file_type;
	o_str		_filename;		//todo probably not need already in c_file_io

	INT32		_point_index_offset;
	INT32		_normal_index_offset;
	INT32		_uv_index_offset;
	INT32		_tri_index_offset;

#if AAA_POLY_COMPILE()
	INT32		_poly_index_offset;
	INT32		_poly_count_offset;
	INT32		_poly_read;
	INT32		_poly_index_read;
#endif

	INT32		_point_read;
	INT32		_normal_read;
	INT32		_uv_read;

	INT32		_tri_read;

	INT32		_obj_nb;

	REAL*		_p_ver;
	REAL*		_p_uv;
	REAL*		_p_nor;

	bool		_b_flip_x_at_read;
	bool		_b_flip_yz_at_read;
	bool		_b_read_tri;
	c_bdd_tri*	_bdd;

	REAL		_offset_at_load[3];	//todo implement
	REAL		_scale_at_load[3];

	INT32	get_next_obj_line();
	bool	get_next_line_starting_with_char( CHAR the_char );

public:
	void	init();
	void	begin( c_bdd_tri* bdd, C_PCHAR_C filename );
	void	set_file_reader( c_file_io* file_reader )	{	_reader = file_reader;	}

	AAA_ERR	count_elt_from_stream( c_obj3d_file_info* file_info );

private:
	AAA_ERR	read_vertex_data_from_stream_obj();
	AAA_ERR	skip_tri_from_stream_obj();
	AAA_ERR	read_tri_from_stream_obj();
	AAA_ERR	load_data_obj_alias(				INT32 obj_nb_to_read, c_obj_info* info );

public:
	//the tri buffer are directly taken from _bdd because they should be always different
	//	here it can change with the target
	void	set_dst_buffer(		REAL* vertex, REAL* uv, REAL* nor );
	AAA_ERR	load_data(			INT32 obj_nb_to_read, c_obj_info* info );

	c_obj3d_file_reader();
};

