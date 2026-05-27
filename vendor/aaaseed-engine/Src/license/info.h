
#ifdef AAA_INFO_H
#error "INFO_H included more than once."
#endif
#define AAA_INFO_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_VERSION_H
#	include "version.h"
#endif

class	c_version;

class	c_info final : public c_obj_ui
{
	FACTORY_DECLARE( c_info, c_obj_ui );
private:
	c_version	_version;
	o_str		_exe_path;
	o_str		_exe_args;
	o_str		_exe_start_time;
	o_str		_version_long;
	o_str		_version_short;
	o_str		_build;
	o_str		_compiler;
	o_str		_toolset;
	o_str		_sdk_version;
	o_str		_kernel32_version;

			void	get_kernel_version();
public:
	virtual	void	param_init_pt();
			void	init();

	virtual	void	update();

		c_version*	get_version()		{	return &_version;				}
		C_PCHAR_C	get_version_long()	{	return _version_long.get();		}
		C_PCHAR_C	get_version_short()	{	return _version_short.get();	}
		C_PCHAR_C	get_build()			{	return _build.get();			}
		C_PCHAR_C	get_compiler()		{	return _compiler.get();			}
		C_PCHAR_C	get_toolset()		{	return _toolset.get();			}

		void		set_exe( INT32 CONST argc, char** argv );
};

extern	c_info*	g_info;
