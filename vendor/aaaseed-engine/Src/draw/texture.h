
#ifdef AAA_TEXTURE_H
#error "TEXTURE_H included more than once."
#endif
#define AAA_TEXTURE_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_GOL_TEX_H
#	include "gol/gol_tex.h"
#endif

class	c_map;
class	c_shading;
class	c_texturing;

class	c_texture_unit final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_texture_unit,c_obj_active_ui);
	friend	c_texturing;
private:
	bool	_b_mapping_ui;
	c_map*	_map;

	INT32	_bind_2d_out;		//so we keep the one really used

	INT32	_bind_1d_ui;
	INT32	_bind_2d_ui;
	INT32	_bank_2d_ui;
	INT32	_bind_3d_ui;

	INT32	_gol_tex_unit;

private:
	FINLINE	bool	update_low(		);
	FINLINE	void	draw_low(		);
//	FINLINE	void	draw_only_active_low();

public:
			//should be called done at init
			void	set_gol_tex_unit( UINT32 CONST gol_tex_unit )	{ _gol_tex_unit = gol_tex_unit;	}
	FINLINE INT32	get_gol_tex_unit( )	 CONST						{ return _gol_tex_unit;	}

	//todo use active in mapping and dump _b_mapping (maa stuff)
	FINLINE bool	is_mapping()		 CONST						{ return _b_mapping_ui;	}

	virtual void	update();

	virtual	void	prepare_for_ui();
	virtual	void	param_init_pt();

	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
	virtual	AAA_ERR	save_do_after( o_str CONST & filename );

	//hack it force file to be saved so branch will be load/saved in the load/save_do_after
	//		but do_after is done only when file is really loaded or saved
	//		this solution is ok but make sticky files
	//			once saved the obj and the branch will always be saved even if the whole branch is useless
	virtual	bool	is_default_save_always()	{	return true;	}

};

class	c_texturing final : public c_obj_ui
{
	FACTORY_DECLARE(c_texturing,c_obj_ui);
public:
	static	CONST	INT32	TEXTURING_UNIT_NB = 3;

	static	c_texturing*	def;
	static	c_texturing*	cur;
	static	c_texturing*	ui;
public:
	FINLINE	static	c_texturing*	get_cur()						{	return cur;	}
	FINLINE	static	void			set_cur( c_texturing* o_in )	{	cur = o_in;	}
	FINLINE	static	c_texturing*	get_ui()						{	return ui;	}
	FINLINE	static	void			set_ui( c_texturing* o_in )		{	ui = o_in;	}
private:
	c_texture_unit*	_tex_units[TEXTURING_UNIT_NB];
public:
			void	build_sum_up( o_str& o );
	virtual	void	param_init_pt();

	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
	virtual	AAA_ERR	save_do_after( o_str CONST & filename );

	//hack it force file to be saved so branch will be load/saved in the load/save_do_after
	//		but do_after is done only when file is really loaded or saved
	//		this solution is ok but make sticky files
	//			once saved the obj and the branch will always be saved even if the whole branch is useless
	virtual	bool	is_default_save_always()	{	return true;	}

	virtual	void	update();
	virtual	void	draw();
//			void	draw_only_active();

	FINLINE static	void disable()
	{
		GOL::disable_tex_unit_from( 1 );
		set_cur( nullptr );
	}
};

