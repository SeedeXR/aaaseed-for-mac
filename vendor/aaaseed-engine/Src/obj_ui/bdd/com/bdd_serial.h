
#ifdef AAA_BDD_SERIAL_H
#error "BDD_SERIAL_H included more than once."
#endif
#define AAA_BDD_SERIAL_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_serial;
class	c_net_link;

class	c_bdd_serial final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_serial, c_bdd );

protected:
	bool			_b_open_asked;
	bool			_b_open;
	bool			_b_verbose_ui;
	bool			_b_remove_newline_last;

	bool			_b_send_newline_at_the_end;
	bool			_b_send_newline_as_0x0d;
	bool			_b_send_trig_ui;
	o_str			_o_to_send;

	bool			_b_network;
	INT32			_net_link_index_ui;
	c_net_link*		_net_link;


	c_serial*		_serial;
	INT32			_com_port_id_opened;
	INT32			_com_port_id_ui;
	INT32			_s_baudrate;

	DOUBLE			_received_last_time;
	bool			_b_wait_answer_ui;
	bool			_b_wait_answer;
	INT32			_wait_loop_nb_max_ui;
	INT32			_sleep_second_micro_ui;
	INT32			_sleep_second_milli_ui;
//	INT32			_wait_answer_time_out;

	static CONSTEXPR INT32 BUFFER_SIZE = 1024;
	UINT8			_buf_read[BUFFER_SIZE];
	UINT8			_buf_keep[BUFFER_SIZE];
	INT32			_buf_keep_len;

	o_str			_o_received_tmp;
	o_str			_o_received_last;
	o_str			_o_received_last_empty_not;
	INT32			_o_received_number_last;
	o_str			_o_received;

public:
	static CONSTEXPR INT32 LINE_NB_MAX = 8;
protected:
	bool			_b_line_received_split;
	INT32			_line_received_nb_total;
	INT32			_line_received_nb;
	o_str			_o_line_received[LINE_NB_MAX];


			o_str&	get_str_to_send();
			AAA_ERR	send( o_str& o );
			void	do_received( CHAR CONST * buf, INT32 len );

			void	update_send();

public:
//			void	set_port( INT32 port_id );
			void	init();
	virtual	void	param_init_pt();

			void	open_serial();
			void	close_serial();

			void	update_serial();
			void	update_network();		
	virtual	void	update();

//	virtual	void	draw();
};
