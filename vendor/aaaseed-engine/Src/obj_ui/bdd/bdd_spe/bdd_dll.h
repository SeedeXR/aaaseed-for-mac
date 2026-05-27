
#ifdef AAA_BDD_DLL_H
#error "BDD_DLL_H included more than once."
#endif
#define AAA_BDD_DLL_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif
#ifndef	AAA_AAA_DLL_H
#	include "obj_ui/aaa_dll.h"
#endif

#ifndef AAA_DLL_APIENTRY
#	define AAA_DLL_APIENTRY WINAPIV
#endif

class	c_bdd_dll final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_dll, c_bdd );
protected:
	o_str			_o_dll_name;
	HINSTANCE		_h_lib;
	AAA_DLL_PARAM	_param_dll;

	bool			_b_loaded;
	bool			_b_init;

	typedef bool	(AAA_DLL_APIENTRY* AAA_DLL_FN_ALLOC)( IN AAA_DLL_PARAM *param );
	typedef INT32	(AAA_DLL_APIENTRY* AAA_DLL_FN_INT32)();
	typedef void	(AAA_DLL_APIENTRY* AAA_DLL_FN_VOID)();

	AAA_DLL_FN_ALLOC	_pf_alloc;
	AAA_DLL_FN_VOID		_pf_dealloc;
	AAA_DLL_FN_INT32	_pf_draw;
	AAA_DLL_FN_INT32	_pf_update;

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
};

