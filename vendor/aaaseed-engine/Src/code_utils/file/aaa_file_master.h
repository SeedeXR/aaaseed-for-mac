#ifdef AAA_AAA_FILE_MASTER_H
#error "AAA_FILE_MASTER_H included more than once."
#endif
#define AAA_AAA_FILE_MASTER_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_file_master final : public c_obj_ui
{
	FACTORY_DECLARE(c_file_master, c_obj_ui);
private:
public:
	virtual	void	param_init_pt_static();
};

