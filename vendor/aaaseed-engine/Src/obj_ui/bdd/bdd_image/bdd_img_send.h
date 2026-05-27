
#ifdef AAA_BDD_IMG_SEND_H
#error "BDD_IMG_SEND_H included more than once."
#endif
#define AAA_BDD_IMG_SEND_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif
#ifndef AAA_IMG_NET_H
#	include "image/img_net.h"
#endif
class c_speed;

class	c_bdd_img_send final : public c_bdd_img
{
public:
private:
	FACTORY_DECLARE( c_bdd_img_send, c_bdd_img );

//	INT32			_difference_bind_ui;
	INT32			_bind_dst_ui;
	st_img_send		_st_send;

public:

protected:
	c_speed*		_speed_bdd			{nullptr};

private:	

public:
	virtual	void	param_init_pt();
	virtual	void	prepare_for_ui();

	virtual	void	update();
//	virtual	void	draw();
};
