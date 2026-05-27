
#ifdef AAA_AAA_SHADER_H
#error "AAA_SHADER_H included more than once."
#endif
#define AAA_AAA_SHADER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_BIND_H
#	include "infrastructure/bind/bind.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef AAA_SHADER_H
#	include "shader.h"
#endif

class c_shaders final : public c_obj
{
private:
	c_bind*					_bind;
	std::vector<c_shader*>	_vect;
	c_shader::SHADER_TYPE	_type;			//	vector, geometry, Fragment

public:
	static	CONST	INT32	SHADERS_MAX_NB	= 512;
	static	void	c_init();
	static	void	c_deinit();
	static	void	c_load_from_file	(	o_str CONST & filename );
	static	void	c_save_to_file		(	o_str CONST & filename );

public:	
	c_shaders( c_shader::SHADER_TYPE type );
	~c_shaders();
	
			AAA_ERR		alloc(			INT32 nb, C_PCHAR_C bind_name, C_PCHAR_C bind_ext );
			void		dealloc();

			AAA_ERR		set(			INT32 nb, C_PCHAR_C bind_name, C_PCHAR_C bind_ext );
//			void		update();
			void		menu_do(		INT32	in );
			INT32		menu_build(		PT_MENU_FN menu_fn );
			C_PCHAR_C	bind_get_name(	INT32 index );

			void		load_bind(		o_str CONST & filename );
			void		save_bind(		o_str CONST & filename );
			c_shader*	get_shader(		INT32 index );

	FINLINE	c_bind*						get_bind()		{	return this ? _bind : nullptr;	}
	FINLINE	c_shader::SHADER_TYPE		get_type()		{	return _type;	}
};

extern	c_shaders*	g_shaders_vertex;
extern	c_shaders*	g_shaders_geometry;
extern	c_shaders*	g_shaders_fragment;
extern	c_shaders*	g_shaders_compute;

extern	void	MAACALLBACK	shader_vertex_menu_fn(		INT32 in );
extern	void	MAACALLBACK	shader_geometry_menu_fn(	INT32 in );
extern	void	MAACALLBACK	shader_fragment_menu_fn(	INT32 in );
extern	void	MAACALLBACK	shader_compute_menu_fn(		INT32 in );

