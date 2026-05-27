
#ifdef AAA_DIR_POOL_H
#error "DIR_POOL_H included more than once."
#endif
#define AAA_DIR_POOL_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif


class	c_dir_pool	final : public c_obj_ui
{
	FACTORY_DECLARE( c_dir_pool, c_obj_ui );
private:
	INT32 _nb;
	std::vector<INT32>	_index_used;
	std::vector<o_str>	_path;
	std::vector<o_str>	_path_absolute;
	std::vector<o_str>	_symbol;

public:
	static	c_dir_pool*	cur;	//todo singleton
	static void update_cur( c_param* param );

	C_NO_CPY_MOVE( c_dir_pool )

	o_str* get_path_pt(		INT32 CONST index )		{ return &_path[index];		}
	o_str* get_symbol_pt(	INT32 CONST index )		{ return &_symbol[index];	}

	AAA_ERR	set_raw(		C_PCHAR_C symbol,	o_str CONST & path, INT32 index=-42 );	// set_raw don't call update
	AAA_ERR set(			C_PCHAR_C symbol,	o_str CONST & path, INT32 index=-42 );	// set call update

	void	expand_fname(	o_str& dst,			C_PCHAR_C in );
	void	expand_fname(	o_str& dst,			o_str CONST & src );
	void	compact_fname(	char* &dst,			o_str CONST & src );
	void	compact_fname(	o_str& dst,			o_str CONST & src );

	virtual	void	update();
	virtual void	param_init_pt_static();
	virtual AAA_ERR	load_do_after( o_str CONST & filename_in );

};

