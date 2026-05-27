
#ifdef AAA_BIND_H
#error "BIND_H included more than once."
#endif
#define AAA_BIND_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_SEEDFILE_H
#	include "infrastructure/seedfile.h"
#endif

class	c_bind;

class	c_bind_abstract : public c_obj_ui
{
	FACTORY_ABSTRACT_DECLARE( c_bind_abstract, c_obj_ui );
private:
//	static	vector<c_obj_ui*>	menu_offsets;
public:
	static	INT32	menu_suscribe( c_obj_ui* obj );
	static	void	menu_unsuscribe( c_obj_ui* obj );
	static	MENU_FN	menu_callback;
private:
	std::vector<o_str>	_cont;
	o_str				_ext;

	INT32				_index_cur;
	INT32				_index_for_next_load_save;

	INT32				_index_nb;

	INT32				_base_id;
	INT32				_menu_id;
	INT32				_menu_sub_item_max;
	INT32				_menu_sub_nb;
	bool				_b_draw_number;
	bool				_b_draw_number_continuous;
	bool				_b_file_relative;
	std::vector<INT32>	_menu_sub;

	c_bind*				_menu_sub_name;

public:
	virtual	C_PCHAR_C		get_fname_ext()	CONST						{	return _ext.get(); }
	virtual	void			set_fname_ext(		C_PCHAR_C ext )			{	_ext.set( ext ); }

//	void	set( CHAR* name);
			void			alloc(				INT32 CONST nb, INT32 CONST menu_sub_nb ); 
			void			dealloc();

	virtual	void			param_init_pt();

	FINLINE	void			make_valid_index(	INT32& index ) CONST
							{
								INT32 nb = INT32(_cont.size());
#if AAA_DEBUG()
								if( nb <= 0 )
									debug_break( "%s() can't make a valid index for an empty container." );
#endif

								CLAMP_REF( index, 0, INT32( _cont.size() - 1 ) );
							}

	FINLINE o_str CONST &	get_o_str(			INT32 index ) CONST			{	make_valid_index( index );	return _cont[index]; }
	FINLINE	C_PCHAR_C		get_str(			INT32 CONST index ) CONST	{	return get_o_str(index).get(); }
	FINLINE	INT32			get_str_len(		INT32 CONST index ) CONST	{	return get_o_str(index).get_len(); }
	FINLINE	INT32			get_str_line_nb(	INT32 index )				{	make_valid_index( index );	return _cont[index].get_line_nb(); }

			void			set_item(			INT32 index,		o_str CONST & str_in );
			void			set_item(			INT32 CONST index,	C_PCHAR_C str_in )		{ set_item( index, o_str(str_in) ); }
			void			clear_item(			INT32 index );

			INT32			find(				C_PCHAR_C pat	) CONST;
			void			swap_item(			INT32 CONST dst, INT32 CONST src );
			bool			swap_item(			c_param CONST * CONST param, INT32 CONST inc );
	
	//constructor linked
			AAA_ERR			set(				INT32 CONST			nb,
												C_PCHAR_C			name,
												C_PCHAR_C			ext,
												aaa::file::TYPE_IO	type_io,
												INT32 CONST			menu_sub_nb = 1,
												bool CONST			b_draw_number = true,
												bool CONST			b_draw_number_continuous = false,
												bool CONST			b_file_relative = false
												);

	FINLINE	INT32	get_elt_nb() CONST			{	return	_index_nb; }
	virtual	AAA_ERR	load_do_after(				o_str CONST & filename );

	FINLINE	INT32	get_menu_id(				INT32 index );
	FINLINE	INT32	get_menu_index(				INT32 CONST index );

			INT32	menu_build(					INT32 CONST base_id,	PT_MENU_FN pt_menu_fn );
			void	menu_item_set(				INT32 CONST index,		C_PCHAR_C title,	bool CONST flag );
			void	menu_item_make_active(		INT32 CONST index,							bool CONST flag );
			void	menu_item_set_before_after(	INT32 CONST index,		C_PCHAR_C before,	C_PCHAR_C after,	bool CONST flag );


			void	set_index_cur(				INT32 CONST index );
	FINLINE	INT32	get_index_cur() CONST		{ return _index_cur; }

			INT32	get_reset_index_for_next_load_save();
			void	set_index_for_next_load_save( INT32 CONST index );
};

class	c_bind final : public c_bind_abstract
{
	FACTORY_DECLARE( c_bind, c_bind_abstract );
private:
public:
	static c_bind* get_new( C_PCHAR_C name_symbo );
};

