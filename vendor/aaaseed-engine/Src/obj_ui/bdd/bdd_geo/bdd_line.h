
#ifdef AAA_BDD_LINE_H
#error "BDD_LINE_H included more than once."
#endif
#define AAA_BDD_LINE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_line final : public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_line,c_bdd_multiple);
private:
			REAL	_point[6];
			INT32	_s_draw_text[2];
			o_str	_text[2];
			REAL	_size[3];

			void	init();
public:

	virtual	void	param_init_pt();

	virtual	void	update();
			void	draw_point( INT32 i);
	virtual	void	draw_single();
	virtual	void	draw_multiple();
};
