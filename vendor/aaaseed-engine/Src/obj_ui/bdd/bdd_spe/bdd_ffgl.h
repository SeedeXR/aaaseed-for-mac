
#ifdef AAA_BDD_FFGL_H
#error "BDD_FFGL_H included more than once."
#endif
#define AAA_BDD_FFGL_H 1


#ifndef AAA_FFGLPluginInstance_H
#	include "FFGL/FFGLPluginInstance.h"
#endif

#if AAA_USE_FFGL()

#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class c_aaa_plugin_instance final : public FFGLPluginInstance
{
public:
	c_aaa_plugin_instance();

	DWORD	Load(CONST char* filename);
	DWORD	Unload();

	virtual ~c_aaa_plugin_instance();

protected:
	HMODULE m_ffModule;
};

class	c_bdd_ffgl final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_ffgl, c_bdd );
protected:
	o_str		_o_plugin_filename;

	o_str		_o_plugin_name;
	o_str		_o_plugin_version;
	o_str		_o_plugin_type;

	INT32		_input_min;
	INT32		_input_max;

	o_str		_o_plugin_cap;
	bool		_b_set_time;
	INT32		_frame_dst;

	UINT8*		_p_input_frame;
	void		*m_InpFrame[2];	// array of input frames for multi-input plugins

	bool		_b_loaded;
	bool		_b_init;

	FFGLPluginInstance*	_plugin;

	o_str	_o_param_name[ 16 ];
	REAL	_param_value[ 16 ];
	REAL	_param_value_ui[ 16 ];

			bool	load_plugin();
			void	unload_plugin();
			void	populate_params();
			void	draw_low_gl();

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
};

#endif	//#if AAA_USE_FFGL()

