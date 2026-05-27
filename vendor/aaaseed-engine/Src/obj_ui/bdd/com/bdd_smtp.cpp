#include "bdd_smtp.h"

#if AAA_LOCAL_SMTP_NORMAL()

#include <lib_use.h>

#	include "Poco/Net/SSLManager.h"
#	include "Poco/Net/MailMessage.h"
#	include "Poco/Net/FilePartSource.h"
#	include "Poco/Net/StringPartSource.h"
#	include "Poco/Net/NetException.h"
#	include "Poco/Net/AcceptCertificateHandler.h"
#	include "Poco/Net/SecureSMTPClientSession.h"
#	include "Poco/Net/SecureStreamSocket.h"
//#	include "Poco/Net/MailRecipient.h"
//#	include "Poco/Net/SMTPClientSession.h"
//#	include "Poco/Net/Context.h"
//#	include "Poco/XML/ParserEngine.h"
//#	include "Poco/AutoPtr.h"

//#if AAA_WIN64()
//#else
#	if AAA_DEBUG()
		AAA_LIB_USE32( "SSL/Debug/libeay32" )
		AAA_LIB_USE32( "SSL/Debug/ssleay32" )
		AAA_LIB_USE32( "Poco/PocoFoundationd" )
		AAA_LIB_USE32( "Poco/PocoNetd" )
		AAA_LIB_USE32( "Poco/PocoNetSSLd" )
		AAA_LIB_USE32( "Poco/PocoCryptod" )
		AAA_LIB_USE32( "Poco/PocoUtild" )
		AAA_LIB_USE32( "Poco/PocoXMLd" )
		AAA_LIB_USE32( "Poco/PocoJSONd" )
		//		AAA_LIB_USE32( "Poco/PocoUtilsd" 
		//		AAA_LIB_USE32( "Poco/PocoMongoDBd" )
		//		AAA_LIB_USE32( "Poco/PocoPDFd" )
		//		AAA_LIB_USE32( "Poco/PocoZipd" )
		//		AAA_LIB_USE32( "Poco/PocoDatad" )
		//		AAA_LIB_USE32( "Poco/PocoDataODBCd" )
		//		AAA_LIB_USE32( "Poco/PocoDataSQLited" )
#	else
		AAA_LIB_USE32( "SSL/Release/libeay32" )
		AAA_LIB_USE32( "SSL/Release/ssleay32" )
		AAA_LIB_USE32( "Poco/PocoFoundation" )
		AAA_LIB_USE32( "Poco/PocoNet" )
		AAA_LIB_USE32( "Poco/PocoNetSSL" )
		AAA_LIB_USE32( "Poco/PocoCrypto" )
		AAA_LIB_USE32( "Poco/PocoUtil" )
		AAA_LIB_USE32( "Poco/PocoXML" )
		AAA_LIB_USE32( "Poco/PocoJSON" )
#	endif

//#endif	//#if AAA_WIN64()

	AAA_LIB_USE_MESSAGE( "Ws2_32" )
	AAA_LIB_USE_MESSAGE( "Crypt32" )


bool	c_bdd_smtp::b_ssl_loaded = false;

class c_thread_smtp : public c_thread
{
public:
	virtual	void	run()								{	run_it< c_bdd_smtp, 1000 > ();	}
	c_thread_smtp() : c_thread( "smtp_thread_loop" )	{}
};

#endif	//#if AAA_LOCAL_SMTP_NORMAL()


FACTORY_CREATE_PROP_V1( c_bdd_smtp, bdd_smtp, Smtp, smtp, sub_menu="Net"; );

namespace n_bdd_smtp
	{


	CONSTEXPR UINT32 BASE_PARAM_NB		= 2 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR UINT32 SERVER_PARAM_NB	= 7;
	CONSTEXPR UINT32 MESSAGE_PARAM_NB	= 6;
	CONSTEXPR UINT32 GROUP_NB			= 2;
	CONSTEXPR UINT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	SERVER_PARAM_NB
									+	MESSAGE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(	send_trig	)
		PARAM_DEF_BOOL_OFF(	verbose		)
		PARAM_DEF_GROUP( Server, SERVER_PARAM_NB )
			PARAM_DEF_REF(			host				)
			PARAM_DEF_INT32_POS(	port,		25, 465	)
			PARAM_DEF_REF(			username			)
			PARAM_DEF_REF(			password			)
			PARAM_DEF_INT32_POS(	timeout,	200, 30	)
			PARAM_DEF_BOOL_OFF(		explicit_tls		)
			PARAM_DEF_BOOL_OFF(		timeout_fix			)
//			PARAM_DEF_BOOL_OFF( ssl )
		PARAM_DEF_GROUP( Message, MESSAGE_PARAM_NB )
			PARAM_DEF_STR(			from				)
			PARAM_DEF_STR(			to					)
			PARAM_DEF_STR(			subject				)
			PARAM_DEF_STR(			body				)
			PARAM_DEF_BOOL_OFF(		attachment_use		)
			PARAM_DEF_STR(			attachment_filename )
	};
}

void	c_bdd_smtp::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt(			h,	_b_send_trig_ui		);
	param_set_pt(			h,	_b_verbose_ui		);

	++h;
		param_set_pt(		h,	_host_ui			);
		param_set_pt(		h,	_port_ui			);
		param_set_pt(		h,	_username_ui		);
		param_set_pt(		h,	_password_ui		);
		param_set_pt(		h,	_timeout_ui			);
		param_set_pt(		h,	_b_explicit_tls_ui	);
		param_set_pt(		h,	_b_timeout_fix_ui	);
//		param_set_pt(		h, _b_ssl				);
	++h;
		param_set_pt(		h, _from_ui				);
		param_set_pt(		h, _to_ui				);
		param_set_pt(		h, _subject_ui			);
		param_set_pt(		h, _body_ui				);
		param_set_pt(		h, _b_attachment_ui		);
		param_set_pt(		h, _attach_filename_ui	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_smtp)
#if AAA_LOCAL_SMTP_NORMAL()
	,_thread_send			(nullptr)
#endif	//#if AAA_LOCAL_SMTP_NORMAL()
{
	param_init_with( n_bdd_smtp::param, n_bdd_smtp::PARAM_NB_MAX );
}

c_bdd_smtp::~c_bdd_smtp()
{
#if AAA_LOCAL_SMTP_NORMAL()
	close_thread();
	dealloc();

	if( b_ssl_loaded )
	{
		Poco::Net::uninitializeSSL();
		b_ssl_loaded = false;
	}
#endif	//#if AAA_LOCAL_SMTP_NORMAL()
}

#if AAA_LOCAL_SMTP_NORMAL()
void	c_bdd_smtp::update()
{
	if( _b_send_trig_ui )
	{
		if( !b_ssl_loaded )
		{
			Poco::Net::initializeSSL();
			b_ssl_loaded = true;
		}
		_b_send_trig_ui = false;

		struct_smtp_send	to_send;

		to_send._host.set( _host_ui.get() );
		to_send._port = _port_ui;
		to_send._username.set( _username_ui.get() );
		to_send._password.set( _password_ui.get() );
	
//		to_send._b_ssl = _b_ssl;
		to_send._to.set( _to_ui.get() );
		to_send._from.set( _from_ui.get() );
	
		to_send._subject.set( _subject_ui.get() );
		to_send._body.set( _body_ui.get() );

		to_send._b_attachment = _b_attachment_ui;
		to_send._attach_filename.set( _attach_filename_ui.get() );

		{
			std::lock_guard<aaa::MUTEX> guard(_thread_lock);
			list_smtp_send.push_back( to_send );
		}

		if( IS_NULL(_thread_send) )
			init_thread();
	}
}

//
//	DRAW
//
void	c_bdd_smtp::draw()
{
}

void	c_bdd_smtp::update_async()
{
	while( !list_smtp_send.empty() )
	{
		struct_smtp_send	first;
		{
			std::lock_guard<aaa::MUTEX> guard(_thread_lock);
			first = list_smtp_send.front();
			list_smtp_send.pop_front();
		}

		std::string host			= first._host.get();
		UINT32		port			= first._port;
		std::string user			= first._username.get();
		std::string password		= first._password.get();
		std::string to				= first._to.get();
		std::string from			= first._from.get();
		std::string subject			= first._subject.get();
		bool		b_attachment	= first._b_attachment;
		try
		{
			if( _b_verbose_ui )
				GOOD_PRINT_STRING( "Send email, building Subject" );
			subject = Poco::Net::MailMessage::encodeWord( subject, "UTF-8" );
			std::string				content = first._body.get();
			Poco::Net::MailMessage	message;
			message.setSender( from );
			if( _b_verbose_ui )
				GOOD_PRINT_STRING( "Send email, add recipient" );
			message.addRecipient( Poco::Net::MailRecipient( Poco::Net::MailRecipient::PRIMARY_RECIPIENT, to ) );
			message.setSubject( subject );
			//message.setContentType( "text/plain; charset=UTF-8" );
			//	message.setContent( content, Poco::Net::MailMessage::ENCODING_8BIT );
			if( _b_verbose_ui )
				GOOD_PRINT_STRING( "Send email, adding content" );
			message.addContent( new Poco::Net::StringPartSource( content ) );
			if( b_attachment )
			{
				if( _b_verbose_ui )
					GOOD_PRINT_STRING( "Send email, building attachment" );
				std::string	attachment = first._attach_filename.get();
				Poco::Net::FilePartSource*	attach = nullptr;
				try
				{
					attach = new Poco::Net::FilePartSource( attachment );
				}
				catch(Poco::FileException &e)
				{
					ERR_PRINT_STRING( "Email not sent, Error opening attachment, FileException : %s", e.displayText().c_str() );
					goto exit_on_error;
				}
				try
				{
					if( _b_verbose_ui )
						GOOD_PRINT_STRING( "Send email, adding attachment" );
					message.addAttachment( Poco::Net::MailMessage::encodeWord( attachment,"UTF-8" ), attach );
				}
				catch (Poco::Net::NetException &e)
				{
					ERR_PRINT_STRING( "Email not sent, Error adding attachment, NetException : %s", e.displayText().c_str() );
					goto exit_on_error;
				}
			}
			try
			{
				if( _b_verbose_ui )
					GOOD_PRINT_STRING( "Send email, init connection to server" );
				if( _b_verbose_ui )
					GOOD_PRINT_STRING( "Send email, create certifcate handler" );
				Poco::SharedPtr<Poco::Net::InvalidCertificateHandler>	ptrHandler = new Poco::Net::AcceptCertificateHandler( false );
				if( _b_verbose_ui )
					GOOD_PRINT_STRING( "Send email, create context" );
				Poco::Net::Context::Ptr									ptrContext = new Poco::Net::Context( Poco::Net::Context::CLIENT_USE, "", "", "", Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
				if( _b_verbose_ui )
					GOOD_PRINT_STRING( "Send email, ssl manager init client" );
				Poco::Net::SSLManager::instance().initializeClient( nullptr, ptrHandler, ptrContext );

				if( _b_explicit_tls_ui )
				{
					Poco::Net::SecureSMTPClientSession	secure_session( host, port );
					try
					{
						if( _b_verbose_ui )
							GOOD_PRINT_STRING( "Send email, unsecure login to server" );
						secure_session.login();

						if( secure_session.startTLS() )
						{
							if( _b_verbose_ui )
								GOOD_PRINT_STRING( "Send email, login to server" );
							secure_session.login(Poco::Net::SMTPClientSession::AUTH_LOGIN, user, password);
							if( _b_verbose_ui )
								GOOD_PRINT_STRING( "Send email, send message" );
							secure_session.sendMessage( message );
							GOOD_PRINT_STRING( "Email successfully sent" );
						}
						secure_session.close();
					}
					catch ( Poco::Net::SMTPException &e )
					{
						ERR_PRINT_STRING( "Email not sent, SMTPException : %s", e.displayText().c_str() );
						goto exit_on_error;
					}
				}
				else
				{
					if( _b_verbose_ui )
						GOOD_PRINT_STRING( "Send email, create socket adress" );
					Poco::Net::SocketAddress		sa( host, port );

					if( _b_verbose_ui )
						GOOD_PRINT_STRING( "Send email, create socket" );
			//		Poco::Net::StreamSocket			socket( sa );
					Poco::Net::StreamSocket			socket;
					Poco::Net::SecureStreamSocket	ssocket;
					try
					{
						if( _b_timeout_fix_ui )
						{
							socket.connect( sa, Poco::Timespan( _timeout_ui, 0 ) );
							socket.shutdown();
						}
						//	Poco::Net::SecureStreamSocket	ssocket( sa );
						try
						{
							if( _b_verbose_ui )
								GOOD_PRINT_STRING( "Send email, create secure socket" );
							ssocket.connect( sa, Poco::Timespan( _timeout_ui, 0 ) );
						}
						catch( Poco::Exception &e )
						{
							ERR_PRINT_STRING( "Email not sent, SecureStreamSocket connect Exception : %s", e.displayText().c_str() );
							goto exit_on_error;
						}
					}
					catch( Poco::Exception &e )
					{
						ERR_PRINT_STRING( "Email not sent, StreamSocket connect Exception : %s", e.displayText().c_str() );
						goto exit_on_error;
					}
					if( _b_verbose_ui )
						GOOD_PRINT_STRING( "Send email, create session" );
					Poco::Net::SMTPClientSession	session( ssocket );
					try
					{
						if( _b_verbose_ui )
							GOOD_PRINT_STRING( "Send email, setting timeout" );
						session.setTimeout( Poco::Timespan( _timeout_ui, 0 ) );
						if( _b_verbose_ui )
							GOOD_PRINT_STRING( "Send email, login to server" );
						session.login(Poco::Net::SMTPClientSession::AUTH_LOGIN, user, password);
						if( _b_verbose_ui )
							GOOD_PRINT_STRING( "Send email, send message" );
						session.sendMessage( message );
						GOOD_PRINT_STRING( "Email successfully sent" );
						session.close();
					}
					catch ( Poco::Net::SMTPException &e )
					{
						ERR_PRINT_STRING( "Email not sent, SMTPException : %s", e.displayText().c_str() );
					}
					catch( Poco::Exception &e )
					{
						ERR_PRINT_STRING( "Email not sent, Exception : %s", e.displayText().c_str() );
					}
				}
			}
			catch ( Poco::Net::NetException &e )
			{
				GOOD_PRINT_STRING( "Email not sent, NetException : %s", e.displayText().c_str() );
			}
		}
		catch( Poco::Net::MessageException &e )
		{
				GOOD_PRINT_STRING( "Email not sent, MessageException : %s", e.displayText().c_str() );
		}
exit_on_error:
	GOOD_PRINT_STRING( "Email processed" );
	}

}

void	c_bdd_smtp::init_thread()
{
	if( IS_NULL( _thread_send ) )
	{
		_thread_send = new c_thread_smtp;
		_thread_send->create( this, 0 );
		_thread_send->start();
	}
}

void	c_bdd_smtp::close_thread()
{
	if( IS_NOT_NULL( _thread_send ) )
	{
		_thread_send->shutdown();
		_thread_send->join();
		SAFE_DELETE( _thread_send );
	}
}
#endif	//#if AAA_LOCAL_SMTP_NORMAL()
