
#ifdef AAA_LIST_FIX_H
#error "LIST_FIX_H included more than once."
#endif
#define AAA_LIST_FIX_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif


class	c_list_fix final : public c_obj_ui
{
	FACTORY_DECLARE( c_list_fix, c_obj_ui );
private:
	INT32			index_max_;
	INT32			index_cur_;
//	INT32			base_id_;
	vector<o_str>	_cont;

	INT32			menu_id_;
	INT32			menu_sub_item_max_;
	INT32			menu_sub_nb_;
	bool			_b_draw_number;
	bool			_b_file_relative;
	vector<INT32>	_menu_sub;

	c_list_fix*		menu_sub_name_;

	o_str			_ext;
protected:

public:
	virtual	C_PCHAR_C	get_fname_ext()	CONST			{ return _ext.get(); }
	virtual	void		set_fname_ext( C_PCHAR_C ext)	{ _ext.set(ext); }
	
//constructor linked
	void set(	CHAR*	name,
				INT32	nb = 32,
				INT32	menu_sub_nb = 1,
				bool	b_draw_number = true,
				bool	b_file_relative = false
				);
	void	alloc( INT32 nb, INT32 menu_sub_nb ); 
	void	dealloc();

	virtual	void	param_init_pt();
	FINLINE	INT32	get_elt_nb()	{	return	index_max_; }
	virtual	AAA_ERR	load_do_after( o_str CONST & filename );

	FINLINE	INT32	get_menu_id(	INT32 index );
	FINLINE	INT32	get_menu_index(	INT32 index );

			INT32	menu_build( INT32 base_id, PT_MENU_FN pt_menu_fn );
			void	menu_item_set( CHAR* title, INT32 index, bool flag );
			void	menu_item_make_active(		INT32 index, bool flag );
			void	menu_item_set_before_after(	INT32 index, CHAR* before, CHAR* after, bool flag );

	FINLINE	void	make_valid_index( INT32& index )	{	CLAMP_REF( index, 0, index_max_-1 );							}

	FINLINE	CHAR*	get_str(			INT32 index )	{
															make_valid_index(index);
															return index >= 0 ? _cont[index].get() : nullptr;
														}
	FINLINE	INT32	get_str_len(		INT32 index )	{
															make_valid_index(index);
															return index >= 0 ? _cont[index].get_len() : 0;
														}
	FINLINE	INT32	get_str_line_nb(	INT32 index )	{
															make_valid_index(index);
															return index >= 0 ? _cont[index].get_line_nb() : 0;	}
			void	set_item( INT32 index, CHAR* str_in );
			INT32	find( CHAR* pat);
			void	swap_item( INT32 dst, INT32 src );

			void	set_index_cur( INT32 index);
	FINLINE	INT32	get_index_cur() { return index_cur_; }
};

