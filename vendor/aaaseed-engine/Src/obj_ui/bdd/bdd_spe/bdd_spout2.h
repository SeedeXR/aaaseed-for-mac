
#ifdef AAA_BDD_SPOUT_H
#error "BDD_SPOUT_H included more than once."
#endif
#define AAA_BDD_SPOUT_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#define AAA_SPOUT_7() 1
#ifndef __Spout__
#	if AAA_SPOUT_7()
#		include "SpoutSDK/SpoutGL/Spout.h"
#	else
#		include "SpoutSDKBefore/Spout.h"
#	endif
#endif

class	c_bdd_spout2 final : public c_bdd 
{
	FACTORY_DECLARE( c_bdd_spout2, c_bdd );

protected:
public:
	static INT32 CONST SRC_UI_NB = 16;

private:
	Spout*				_receiver;
	Spout*				_sender;

	bool				_b_enable_ui;
	bool				_b_verbose_ui;
	//IDXGIAdapter*		_dx11_adapter;
// RECEIVER
	bool				_b_receive_ui;
	bool				_b_receive;
	bool				_b_receiving;
	bool				_b_receiver_flip_vertical_ui;
	o_str				_receiver_name_ui;

	UINT32				_receiver_index_ui;
	UINT32				_receiver_index;
	INT32				_receiving_count_out;

	UINT32				_receiver_sender_count;
	UINT32				_receiver_sender_count_done;
	bool				_b_receiver_sender_enum_trig;

	bool				_b_receive_img;
	bool				_b_receive_img_ui;
	bool				_b_receiver_dx9_asked_ui;
	bool				_b_receiver_dx9_used;

	bool				_b_receiver_init;
	bool				_b_receiver_memory_share;
// SENDER
	bool				_b_send_ui;
	bool				_b_send;
	bool				_b_sending;
	bool				_b_sender_flip_vertical_ui;
	o_str				_sender_name_ui;
	INT32				_sending_count_out;

	bool				_b_send_img_ui;
	bool				_b_sender_dx9_asked_ui;
	bool				_b_sender_dx9_used;

	bool				_b_sender_init;
	bool				_b_sender_memory_share;


	UINT32				_size_x;
	UINT32				_size_y;
	//INT32				_format;
	aaa::PIXEL_FORMAT	_pixel_format;
	GLenum				_sender_gl_format;

	INT32				_gpu_adapter_ui;
	CHAR				_name[256];	// avoid dynamic allocation in function

	o_str				_src_name[SRC_UI_NB];
	o_str				_o_version_sdk;
	o_str				_o_version_spout;

	void	init_sender();
	bool	open_sender( UINT32 CONST size_x, UINT32 CONST size_y, DWORD dwFormat );
	void	close_sender();
	void	update_sender();

	void	enum_sender();

	bool	open_receiver( UINT32 CONST index );
	void	close_receiver();
	void	update_receiver();

public:
			void	check( C_PCHAR_C str );

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
};


