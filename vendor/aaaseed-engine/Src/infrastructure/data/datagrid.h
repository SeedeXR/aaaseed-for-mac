
#ifdef AAA_DATAGRID_H
#error "DATAGRID_H included more than once."
#endif
#define AAA_DATAGRID_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DATALINE_H
#	include "infrastructure/data/dataline.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif

class	c_bdd_datagrid final : public c_bdd_multiple
{
	FACTORY_DECLARE(  c_bdd_datagrid, c_bdd_multiple );
private:
//	bool	_b_verbose;
	bool	_b_check_for_change_ui;
//	bool	_b_first;
	bool	_b_load_trig;
	bool	_b_save;
	bool	_b_save_trig;
	bool	_b_local_ui;

	UINT32	_y_max;
	bool	_b_pop_trig_ui;
	bool	_b_data_new_ui;
	INT32	_data_load_count;

	bool	_b_changed;
	bool	_b_comma_is_separator;
	bool	_b_need_checksum;

	o_str	_data_filename;
	o_str	_data_fname;
	o_str	_data_fname_last;
	time_t	_time_modified;
	size_t	_data_size;

	//	move to map ?
	std::deque<c_data_line*>*	_datalines;
	std::deque<c_data_line*>*	_datalines_store;

	c_data_line*				_dataline_push;

//	o_str	_sql_query_text;
//	bool	_b_sql_query_trig;

	void					clear_datalines( std::deque<c_data_line*>* datalines );
	c_data_line*			create_data_line( INT32 y );
	FINLINE	c_data_line*	get_data_line( INT32 y ) CONST;
			void			dealloc();
			void			make_fname( o_str& fname );
public:
			void			init();
	virtual	void			param_init_pt();

			void			update();

	FINLINE	UINT32			get_y_max() CONST	{	return _y_max; }
			UINT32			find_y_max() CONST;
			void			push();
			void			pop();

			AAA_ERR			read(			C_PCHAR_C signature	);
			AAA_ERR			read_data(		CHAR* dst_in		);
			AAA_ERR			save_data();

			AAA_ERR			read_libreoffice( C_PCHAR_C signature );

	FINLINE	bool			is_changed() CONST {	return _b_changed; }

			bool			is_number(			INT32 CONST x, INT32 CONST y	) CONST;
			bool			has_data(			INT32 CONST x, INT32 CONST y	) CONST;
			FP32			get_fp32(			INT32 CONST x, INT32 CONST y	) CONST; 
			DOUBLE			get_double(			INT32 CONST x, INT32 CONST y	) CONST; 
			o_str CONST &	get_str(			INT32 CONST x, INT32 CONST y	) CONST;
			CHAR			get_char(			INT32 CONST x, INT32 CONST y,	INT32 CONST char_index ) CONST;
								 									
			void			set_double(			INT32 CONST x, INT32 CONST y,	DOUBLE CONST d					);
			void			set_str(			INT32 CONST x, INT32 CONST y,	C_PCHAR_C str					); 
			void			set_str_and_double(	INT32 CONST x, INT32 CONST y,	C_PCHAR_C str,	DOUBLE CONST d	); 
								 									
			void			insert_double(		INT32 CONST x, INT32 CONST y,	DOUBLE CONST r	); 
			void			insert_str(			INT32 CONST x, INT32 CONST y,	C_PCHAR_C in	); 

	virtual	AAA_ERR			save_do_after(				o_str CONST & filename		);

			void			set_data_filename(			o_str CONST & filename		);
	virtual	AAA_ERR			load_data_from_filename(	o_str CONST & filename,	INT32 CONST type_io	);
};

