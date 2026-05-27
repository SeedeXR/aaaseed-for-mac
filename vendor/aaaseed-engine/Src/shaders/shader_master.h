
#ifdef AAA_SHADER_MASTER_H
#error "SHADER_MASTER_H included more than once."
#endif
#define AAA_SHADER_MASTER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_UI_MASTER_H
#	include "ui/ui_master.h"
#endif

class c_shader_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_shader_master, c_obj_ui );
public:
	static CONSTEXPR INT32	INCLUDE_NB = 16;
	o_str					_include_file_name_amd;
	o_str					_include_file_name_intel;
	o_str					_include_file_name_nvidia;
	o_str					_include_file_name[INCLUDE_NB];

	static CONSTEXPR INT32	INCLUDE_DIR_NB = 8;
	o_str					_include_dir_name[INCLUDE_DIR_NB];

protected:
	bool					_b_do_validate_ui;
	bool					_b_force_reload_trig_ui;
//	bool					_b_force_reload;
	INT32					_reload_frame_index;
	bool					_b_include_reload_trig_ui;

	master::NO_ALLOW_FORCE	_s_file_check;

public:
	FINLINE	bool	is_do_validate()			{	return _b_do_validate_ui;			}
//	FINLINE	bool	is_force_reload()			{	return _b_force_reload;				}
	FINLINE	INT32	get_reload_frame_index()	{	return _reload_frame_index;			}
	FINLINE	bool	is_include_reload_trig()	{	return _b_include_reload_trig_ui;	}

	virtual	void	param_init_pt_static();
	virtual	void	param_init_pt();
	virtual void	prepare_for_ui();
	virtual void	update();

			void	reload_include();

	FINLINE	bool	is_file_check( bool b_in )	{	return master::process_master_switch( _s_file_check, b_in );	}
};

extern	c_shader_master*	g_shader_master;

