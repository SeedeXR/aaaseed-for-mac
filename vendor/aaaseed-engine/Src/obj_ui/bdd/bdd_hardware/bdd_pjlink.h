
#ifdef AAA_BDD_PJLINK_H
#error "BDD_PJLINK_H included more than once."
#endif
#define AAA_BDD_PJLINK_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

class	c_net_link;

class	c_bdd_pjlink final : public c_bdd
{
	FACTORY_DECLARE(c_bdd_pjlink, c_bdd);
public:
	enum STATE : INT32
		{
		CLOSED = 0,
		CALLING,
		WAITING_ID,
		READY,
		WAITING_ANSWER,
		CLOSING,
		STATE_NB
		};
	enum CMD : INT32
		{
		CMD_POWER = 0,
		CMD_INPUT,
		CMD_MUTE,
		CMD_ERR_STATUS,
		CMD_LAMP,
		CMD_INPUT_LIST,
		CMD_NAME,
		CMD_INF1,
		CMD_INF2,
		CMD_INF0,
		CMD_CLASS,
		CMD_NB,
		};

	static	CONST	INT32	LAMP_NB = 4;

protected:
	bool		_b_open_ui;
	bool		_b_open;
	bool		_b_verbose;

	STATE		_state;
	INT32		_waiting_count;	//hack  should use time

	c_net_link*	_link;
	INT32		_link_index_ui;
//	INT32		_link_index;

	c_delta_t	_delta_t;
	REAL		_check_time;
	REAL		_check_interval;

	bool		_b_power_asked;
	INT32		_s_power_state;

	INT32		_s_input_type_asked;
	INT32		_s_input_selector_asked;
	INT32		_s_input_type_state;
	INT32		_s_input_selector_state;

	INT32		_lamp_nb;
	bool		_lamp_state[LAMP_NB];
	INT32		_lamp_time[LAMP_NB];

	bool		_b_send_trig_ui;
	o_str		_o_send;
	o_str		_o_received;

	INT32		_result_index;
	bool		_b_result_trig_ui;
	o_str		_o_result[CMD_NB];

private:
			void	set_state(			STATE in );
			INT32	find_cmd(			CHAR* str, INT32 len );
			bool	receive_cmd(		INT32 cmd, CHAR* buf, INT32 len );
			AAA_ERR	make_cmd(			o_str* o, INT32 cmd, C_PCHAR_C str );
			AAA_ERR	add_cr_then_send(	o_str* o );
public:
			void	init();
	virtual	void	param_init_pt();

			void	open();
			void	close();

	virtual	void	update();
			void	update_low();
	virtual	void	draw();
};
