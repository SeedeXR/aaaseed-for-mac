#ifdef AAA_PAY6_H
#error "PAY6 included more than once."
#endif
#define AAA_PAY6_H 1
 
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern	void	terre_main();
extern	void	terre_deinit();
extern	void	terre_reseed();
extern	void	terre_new();
extern	void	my_draw();
extern	void	menu_paysage(int menuval);
extern	int		menu_paysage_build();


#endif
 
