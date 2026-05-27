
#ifdef AAA_DATACUBE_H
#error "DATACUBE_H included more than once."
#endif
#define AAA_DATACUBE_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

//todo datacube should be an obj_ui and so the root of the datagrids
CONSTEXPR	INT32	DATAGRID_NB	= 8 ;

class c_datacube final : public c_obj
{
private:
	FINLINE	void	make_valid_index_channel(	INT32& channel	) CONST;

public:
	void			init();
	void			update();

	void			dealloc();

	void			load(			o_str CONST & filename_in	);
	void			save(			o_str CONST & filename_in	);

	bool			is_changed(		INT32 channel_id	) CONST;
	FP32			get_fp32(		INT32 channel_id,	INT32 CONST row_id,	INT32 CONST col_id	) CONST;
	DOUBLE			get_double(		INT32 channel_id,	INT32 CONST row_id,	INT32 CONST col_id	) CONST;
	bool			is_number(		INT32 channel_id,	INT32 CONST row_id,	INT32 CONST col_id	) CONST;
	bool			has_data(		INT32 channel_id,	INT32 CONST row_id,	INT32 CONST col_id	) CONST;
	o_str CONST &	get_str(		INT32 channel_id,	INT32 CONST row_id,	INT32 CONST col_id	) CONST;
//	INT32			get_str_len(	INT32 channel_id,	INT32 CONST row_id,	INT32 CONST col_id	) CONST;
	CHAR			get_char(		INT32 channel_id,	INT32 CONST row_id,	INT32 CONST col_id,	INT32 CONST char_index	) CONST;
															 					 
	void			set_double(		INT32 channel_id,	INT32       row_id,	INT32 CONST col_id,	DOUBLE CONST r		);
	void			set_str(		INT32 channel_id,	INT32       row_id,	INT32 CONST col_id,	C_PCHAR_C  str		);
															        			
	void			insert_double(	INT32 channel_id,	INT32       row_id,	INT32 CONST col_id,	DOUBLE CONST r		);
	void			insert_str(		INT32 channel_id,	INT32       row_id,	INT32 CONST col_id,	C_PCHAR_C str		);

	void			push_row(		INT32 channel_id	);
	void			pop_row(		INT32 channel_id	);
	INT32			get_row_nb(		INT32 channel_id	)  CONST;
};

extern	c_datacube*	g_datacube;

