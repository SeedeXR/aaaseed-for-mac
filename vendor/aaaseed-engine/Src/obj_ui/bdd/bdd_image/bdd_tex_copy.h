
#ifdef AAA_BDD_TEX_CPY_H
#error "BDD_TEX_CPY_H included more than once."
#endif
#define AAA_BDD_TEX_CPY_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_GOL_TEX_H
#	include "gol/gol_tex.h"
#endif

class	c_bdd_tex_copy final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_tex_copy, c_bdd );
private:
public:
protected:

	bool	_b_src_color_attachment_ui;
	INT32	_s_src_color_attachment_ui;
	INT32	_src_bind_ui;
	INT32	_src_bind_out;
	INT32	_dst_bind_ui;
	INT32	_dst_bind_out;


//	bool	_b_strobe_active_ui;
//	FP32	_strobe_freq_ui;

	INT32	_size[2];

	bool	_b_dst_format_force_src_ui;
	bool	_b_tex_to_tex;
	bool	_b_grab_to_gpu_ui;
	bool	_b_grab_to_cpu_ui;
	INT32	_strobe_ui;
	INT32	_strobe_count;
	INT32	_size_ui[2];
	INT32	_size_offset_ui[2];
	REAL	_size_factor_ui;
	REAL	_tex_size[2];
	REAL	_grab_pos[2];
//	bool	_b_grab_alpha_ui;
	bool	_b_verbose_ui;
	bool	_b_check_gl_ui;
//	bool	b_blur;
	bool	_b_smooth_ui;
//	bool	_b_smooth;
	bool	_b_mipmap_generate_ui;
//	INT32	pass_nb;


public:

	virtual	void	param_init_pt();
			void	init();

			void	grab_to_tex( INT32 CONST src_bind );
			void	grab_to_img( INT32 CONST src_bind );


	virtual	void	draw();
	virtual	void	update();

};
