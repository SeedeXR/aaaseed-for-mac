
#ifdef AAA_MAT_H
#error "MAT_H included more than once."
#endif
#define AAA_MAT_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_OBJ_UI_ARRAY_H
#	include "infrastructure/obj/obj_ui_array.h"
#endif



CONST	INT32	MATERIAL_MAX_NB	= 128;

class	c_material final : public c_obj_ui
{
	FACTORY_DECLARE( c_material, c_obj_ui );
friend	class	c_materials;
private:
	FP32	_ambient_ui[5];
	FP32	_ambient_occlusion_ui;
	FP32	_diffuse_ui[5];
	FP32	_specular_ui[5];
	FP32	_shininess_ui;
	FP32	_emission_ui[5];
	FP32	_reflection_ui;

	typedef struct st_material
	{
		FP32	emissive_color[3];
		FP32	roughness;
		FP32	diffuse_color[3];
		FP32	ao_factor;
		FP32	specular_color[3];
		FP32	reflection_intensity;
		FP32	ambient_intensity;
	} st_material;

	st_material	_ubo_data;

protected:
			void	set_alpha( FP32 alpha_in );
			void	set_it( INT32 face );
public:
	virtual	void	param_init_pt();

	virtual	bool	is_draw_sum_up()	CONST		{ return true; }
	virtual	void	draw_sum_up();
};

extern	void	material_init();

class	c_bind;

class	c_materials final : public c_obj_ui
{
	FACTORY_DECLARE( c_materials, c_obj_ui );

	static	c_materials*	cur;
	static	c_materials*	ui;
	static	c_materials*	def;

	INT32							_index_front;
	INT32							_index_back;
	c_obj_ui_array_pt<c_material>	_array_pt;
	c_bind*							_bind;

public:
	FINLINE	static	c_materials*	get_def()						{	return def;		}
	FINLINE	static	void			set_def( c_materials* mats )	{	def = mats;	}

	FINLINE	static	c_materials*	get_cur()						{	return cur;		}
	FINLINE	static	void			set_cur( c_materials* mats )	{	cur = mats;	}
//	FINLINE	static	void			set_cur_null()					{	cur = def;		}

	FINLINE	static	c_materials*	get_ui()						{	return ui;		}
	FINLINE	static	void			set_ui( c_materials* mats )		{	ui = mats;	}
//	FINLINE	static	void			set_ui_null()					{	ui = def;		}

			static	void c_init();
			static	void c_deinit();

	virtual	void		param_init_pt();

			AAA_ERR		alloc();
			void		dealloc();

	FINLINE bool		check_index(	INT32 index, CHAR* str = nullptr );
	c_material*			get_always(		INT32 index );
	c_material*			get_from_index( INT32 index );

			void		update();

private:
	c_material*			load_one(		INT32 index, o_str CONST & filename );
public:
	virtual AAA_ERR		load_do_after( o_str CONST & filename_in );
	virtual AAA_ERR		save_do_after( o_str CONST & filename_in );

			void		set_focus();
			void		set_focus( INT32 in );
			INT32		menu_build();

//	FINLINE	c_material*	get_front()	{	return get_from_index(front_cur);	}
//	FINLINE	c_material*	get_back()	{	return get_from_index(back_cur);	}

			void		reset();
			void		set_last();
			void		get_last();

			void		set_alpha( INT32 index, FP32 value );
			void		set( INT32 mat_front, INT32 mat_back );
			void		back_cur_set_alpha(		FP32 value );
			void		front_cur_set_alpha(	FP32 value );

	FINLINE	INT32		get_front()				CONST	{	return _index_front;				}
	FINLINE	INT32		get_back()				CONST	{	return _index_back; }
	FINLINE	c_material*	get_material_front()			{	return get_always( _index_front ); }
	FINLINE	c_material*	get_material_back()				{	return get_always( _index_back ); }
};


