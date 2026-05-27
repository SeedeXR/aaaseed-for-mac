
#ifdef AAA_SOCKET_H
#error "SOCKET_H included more than once."
#endif
#define AAA_SOCKET_H 1


#ifndef AAA_ERR_H
#	include "err.h"
#endif

#ifdef	WIN32
#	ifndef _WINSOCK2API_
#		include <winsock2.h>
#	endif
#	define 	soc_err (WSAGetLastError())
#else
#	include <sys/errno.h>
#	define		soc_err (errno)
#	define		SOCKET	INT32
#	define		ZOBI(s)    <s.h>
#	include	ZOBI(netdb)
#	include	ZOBI(arpa/inet)
#	include	ZOBI(sys/time)
#	include	ZOBI(sys/socket)
#	include	ZOBI(netinet/in)
#	define		SOCKET_ERROR	-1
#	define		INVALID_SOCKET	-1
#endif


enum ERR_SOCK : INT32
{
	ERR_SOCK_CANT_INIT = ERR_SOCK_BASE,
	ERR_SOCK_MAX
};

/*
extern	SOCKET	sock_get_connection( SOCKET	listener);
extern	SOCKET	sock_call( char * hostname, INT32 port_nb);
*/

class c_net_link;

//todo	make it an c_obj ?
class c_sock final
{
	friend c_net_link;

protected:
	static	bool		b_started;
	SOCKET				_s;
	struct sockaddr_in	_sa;
	bool				_b_udp_dst;
	struct sockaddr_in	_sa_udp_dst;
	bool				_b_close_asked;



			AAA_ERR		broadcast_enable_send( INT32 _broadcast_time_to_leave );
			AAA_ERR		reuse_address_enable();

public:
	static INT32 CONSTEXPR	BLK_SEND_SIZE_MAX = 65507 - 128;	// 65535 - 8 (UDP header) - 20 IP header) - 128 (secu/routing switch)
	static bool			b_verbose;

	static	AAA_ERR	start();
	static	void	stop();
	static	CHAR*	get_ip( INT32 index );
	static	INT32	get_ip_field( INT32 index );
	static	INT32	get_ip_field_lowest();
	static	void	set_verbose( bool CONST flag );
	static	UINT32	get_ip_addr_from_str( C_PCHAR_C str );
	c_sock();
	virtual ~c_sock();

	
	void	sock_error( INT32 error_code = 0 );

	FINLINE	SOCKET	get_socket()		{	return _s;						}
	FINLINE	bool	is_valid()	CONST 	{	return _s != INVALID_SOCKET;	}
			void	ask_close();

			UINT16	get_port_nb();
			void	fill_address( struct sockaddr_in* dst, unsigned long addr, UINT16 port_nb );
			void	set_non_blocking( bool b_non );
			bool	set_timeout_receive( INT32 milisec );

			AAA_ERR	broadcast_create( UINT16 port_nb, UINT16 port_nb_udp_dst, INT32 _broadcast_time_to_leave );

			AAA_ERR	establish( UINT16 port_nb );
			c_sock*	accept();
			AAA_ERR	call( C_PCHAR_C hostname, INT32 port_nb, bool b_non_blocking=true );

			INT32	close();

//			AAA_ERR send_broadcast(		CONST UINT8* CONST buf, INT32 len );
			AAA_ERR sendto( UINT32 dst, CONST UINT8* CONST buf, INT32 len );

			INT32	recv_broadcast(		UINT8* buf, INT32 len );
			INT32	recv(				UINT8* buf, INT32 len );

//			INT32	read_all(			UINT8* buf, INT32 n );
//			INT32	write_all(			CONST UINT8* buf, INT32 n );
};

