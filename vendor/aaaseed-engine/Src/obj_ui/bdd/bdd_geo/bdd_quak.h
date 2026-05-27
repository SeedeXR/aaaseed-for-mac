
#ifdef AAA_BDD_QUAK_H
#error "BDD_QUAK_H included more than once."
#endif
#define AAA_BDD_QUAK_H 1


#ifndef	AAA_BSPFILESTRUCTS_H
#   include "draw/bspfilestructs.h"
#endif

extern	void render_bsp();
extern	STATUS bdd_quak_load_level_data();

