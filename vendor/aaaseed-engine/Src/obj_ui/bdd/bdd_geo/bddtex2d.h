
#ifdef AAA_BDDTEX2D_H
#error "BDDTEX2D_H included more than once."
#endif
#define AAA_BDDTEX2D_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_tex2d final : public c_bdd 
{
	FACTORY_DECLARE( c_bdd_tex2d, c_bdd );
//friend class c_bdd_chat;
protected:
	REAL	_origin[3];
	INT32	_s_axe;

	INT32	_s_type;
	INT32	_s_type_last;
	INT32	_s_font;
	INT32	_s_font_last;

	REAL	_align_hori;
	REAL	_align_vert;
	REAL	_interline;

	bool	_b_generate_trig_ui;
	INT32	_s_generate;
	o_str	_text;

	REAL	_size[3];

	REAL*	_point;
	REAL*	_point_deformed;
	INT32	_point_nb;

	bool	_b_draw_point_ui;

	void	init();

public:	

	virtual	void	param_init_pt();
			void	dealloc_point();
	FINLINE	void	alloc_point(	INT32 nb_in			);

			void	set_text(		C_PCHAR_C in		);
			void	add_text(		C_PCHAR_C in		);

			void	set_text(		o_str CONST & in	);
			void	add_text(		o_str CONST & in	);

			void	set_type(		INT32 s_type_in		)	{ _s_type = s_type_in; };
			void	set_font(		INT32 s_font_in		)	{ _s_font = s_font_in; };
			void	set_axe(		INT32 s_axe_in		)	{ _s_axe = s_axe_in; };
			void	set_align_hori( REAL align_hori_in	)	{ _align_hori = align_hori_in; };
	virtual	void	draw();
	virtual	void	update();
};
