
#ifdef AAA_BDD_IRTOUCH_H
#error "BDD_IRTOUCH_H included more than once."
#endif
#define AAA_BDD_IRTOUCH_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_serial;

class	c_bdd_ir_touch final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_ir_touch, c_bdd );

public:
	static	CONST	INT32	IRT_MAX_TOUCH = 7;
	static	CONST	INT32	IRT_BUF_SIZE = 1024;

protected:
	bool			_b_open_ui;
	bool			_b_verbose;

	c_serial*		_serial;
	UINT32			_com_port_nb;
	UINT32			_irt_max_touch;
	//INT32			_com_port_nb_last;
	//bool			_b_com_port_scan;
	//INT32			_com_port_start;
	//INT32			_com_port_stop;
	UINT8			_buf_read[ IRT_BUF_SIZE ];
	UINT8			_buffer[ IRT_BUF_SIZE ];

	INT32			_buff_offset;

	c_blob			_blobs[ IRT_MAX_TOUCH ];
	bool			_b_blobs[ IRT_MAX_TOUCH ];

	o_str			_target_name_symbo;
	c_bdd*			_bdd_target;
	REAL			_transfer_translate[ 3 ];
	REAL			_transfer_scale_ui[ 4 ];
	REAL			_transfer_blob_radius;

			void	blobs_reset();
			void	analyze_serial( UINT8* buf, INT32 nb );
			void	update_low();

public:
			void	init();
	virtual	void	param_init_pt();

			void	open();
			void	close();

	virtual	void	update();
	virtual	void	draw();
	virtual	void	transfer_blobs_to( BLOBS_CONT& blobs );
};

