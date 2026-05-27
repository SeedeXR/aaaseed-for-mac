
#ifdef AAA_BDD_SMTP_H
#error "BDD_SMTP_H included more than once."
#endif
#define AAA_BDD_SMTP_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

#if AAA_WIN64()
#	define AAA_LOCAL_SMTP_NORMAL() 0
#else
#	define AAA_LOCAL_SMTP_NORMAL() 0
#endif

#if AAA_LOCAL_SMTP_NORMAL()
#	ifndef AAA_AAA_THREAD_H
#		include "Thread/aaa_thread.h"
#	endif

	class	c_thread_smtp;

	struct struct_smtp_send
	{
		o_str	_host;
		INT32	_port;
		o_str	_username;
		o_str	_password;

	//	bool	_b_ssl;
		o_str	_to;
		o_str	_from;

		o_str	_subject;
		o_str	_body;

		bool	_b_attachment;
		o_str	_attach_filename;
	};
//std::list<struct list_async_load> async_load_list;
#endif

class	c_bdd_smtp final : public c_bdd 
{
	FACTORY_DECLARE( c_bdd_smtp, c_bdd );
private:
	bool			_b_send_trig_ui;
	bool			_b_verbose_ui;
	o_str			_host_ui;
	INT32			_port_ui;
	o_str			_username_ui;
	o_str			_password_ui;
	bool			_b_explicit_tls_ui;
	INT32			_timeout_ui;
	bool			_b_timeout_fix_ui;

//	bool			_b_ssl;
	o_str			_to_ui;
	o_str			_from_ui;

	o_str			_subject_ui;
	o_str			_body_ui;

	bool			_b_attachment_ui;
	o_str			_attach_filename_ui;
public:
	virtual	void	param_init_pt();

#if AAA_LOCAL_SMTP_NORMAL()

protected:
	c_thread_smtp*	_thread_send;
	std::list< struct_smtp_send > list_smtp_send;
private:
	static	bool	b_ssl_loaded;
	aaa::MUTEX		_thread_lock;
public:
			void	update_async();
			void	init_thread();
			void	close_thread();

	virtual	void	update();
	virtual	void	draw();

#endif //#if AAA_LOCAL_SMTP_NORMAL()
};

