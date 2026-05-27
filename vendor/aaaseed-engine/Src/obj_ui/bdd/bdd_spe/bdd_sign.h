
#ifdef AAA_BDD_SIGN_H
#error "BDD_SIGN_H included more than once."
#endif
#define AAA_BDD_SIGN_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_bdd_signature final : public c_bdd 
{
	FACTORY_DECLARE(c_bdd_signature,c_bdd);
private:
public:
	virtual	void	param_init_pt() {}	
	virtual	void	draw();
};
