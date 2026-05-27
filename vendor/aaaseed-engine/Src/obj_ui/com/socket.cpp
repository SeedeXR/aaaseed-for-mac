/*  
 *	Emmanuel Maa BERRIET
 *		4 Juillet 1996
 *
 *	Portable layer (WIN32 vs SGI)
 *		to encapsulate Sockets
 */

#include "socket.h"
#include <Ws2tcpip.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#ifndef	WIN32
#	include <unistd.h>
#endif
#include "aaa_util.h"
#include "str_utils.h"

#include "obj_ui/tracker/trackers.h"
#include "spy.h"
#include "aaa_mem.h"

#include <lib_use.h>
AAA_LIB_USE_MESSAGE( "wsock32" )

bool	c_sock::b_verbose = false;
bool	c_sock::b_started = false;

#define	SOCK_HEADER  "# SOCKET "
void	SOCK_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( SOCK_HEADER, fmt, args );
	va_end(args);
}

namespace {
CONST	INT32	MAXHOSTNAME	= 256;

#ifdef	WIN32
bool				b_ws_opened = false;
#endif
CHAR				sock_host_name[ MAXHOSTNAME ];
struct	hostent*	sock_host_hp;
INT32				sock_host_ip_field[ 16 ];
INT32				sock_host_ip_field_nb;

CONST	INT32	MAX_IP_ADDRESS = 4;
char			ip_address[ MAX_IP_ADDRESS ][ 16 ]; // Store system IP Addresses
}

#define	LOC_MAKEWORD(a, b)	((WORD)(((BYTE)(a)) | ((WORD)((BYTE)(b))) << 8))


/*
WSANOTINITIALISED	A successful WSAStartup must occur before using this function.
WSAENETDOWN	The Windows Sockets implementation has detected that the network subsystem has failed.
WSAENOTCONN	The socket is not connected.
WSAEINTR	The (blocking) call was canceled using WSACancelBlockingCall.
WSAEINPROGRESS	A blocking Windows Sockets operation is in progress.
WSAENOTSOCK	The descriptor is not a socket.
WSAEOPNOTSUPP	MSG_OOB was specified, but the socket is not of type SOCK_STREAM.
WSAESHUTDOWN	The socket has been shut down; it is not possible to recv on a socket after shutdown has been invoked with how set to 0 or 2.
WSAEWOULDBLOCK	The socket is marked as nonblocking and the receive operation would block.
WSAEMSGSIZE	The datagram was too large to fit into the specified buffer and was truncated.
WSAEINVAL	The socket has not been bound with bind.
WSAECONNABORTED	The virtual circuit was aborted due to timeout or other failure.
WSAECONNRESET	The virtual circuit was reset by the remote side.

  
WSAEACCES	The requested address is a broadcast address, but the appropriate flag was not set.
WSAEINPROGRESS	A blocking Windows Sockets operation is in progress.
WSAEFAULT	The buf argument is not in a valid part of the user address space.
WSAENETRESET	The connection must be reset because the Windows Sockets implementation dropped it.
WSAENOBUFS	The Windows Sockets implementation reports a buffer deadlock.
R

�	WSAEADDRINUSE   The specified address is already in use.
�	WSAEADDRNOTAVAIL   The specified address is not available from the local machine.
�	WSAEAFNOSUPPORT   Addresses in the specified family cannot be used with this socket.
�	WSAECONNREFUSED   The attempt to connect was forcefully rejected.
�	WSAEDESTADDRREQ   A destination address is required.
�	WSAEFAULT   The lpSockAddrLen argument is incorrect.
�	WSAEINVAL   The socket is already bound to an address.
�	WSAEISCONN    The socket is already connected.
�	WSAEMFILE   No more file descriptors are available.
�	WSAENETUNREACH   The network cannot be reached from this host at this time.
�	WSAENOBUFS   No buffer space is available. The socket cannot be connected.
�	WSAENOTCONN   The socket is not connected.
�	WSAENOTSOCK   The descriptor is a file, not a socket.
�	WSAETIMEDOUT   The attempt to connect timed out without establishing a connection.
*/

//	berkeley to WINDOWS

#if	FALSE	//now already in windows

#ifdef	WIN32
#define EINPROGRESS		WSAEINPROGRESS
#define EALREADY		WSAEALREADY
#define ENOTSOCK		WSAENOTSOCK
#define EDESTADDRREQ	WSAEDESTADDRREQ
#define EMSGSIZE		WSAEMSGSIZE
#define EPROTOTYPE		WSAEPROTOTYPE
#define ENOPROTOOPT		WSAENOPROTOOPT
#define EPROTONOSUPPORT	WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT	WSAESOCKTNOSUPPORT
#define EOPNOTSUPP		WSAEOPNOTSUPP
#define EPFNOSUPPORT	WSAEPFNOSUPPORT
#define EAFNOSUPPORT	WSAEAFNOSUPPORT
#define EADDRINUSE		WSAEADDRINUSE
#define EADDRNOTAVAIL	WSAEADDRNOTAVAIL
#define ENETDOWN		WSAENETDOWN
#define ENETUNREACH		WSAENETUNREACH
#define ENETRESET		WSAENETRESET
#define ECONNABORTED	WSAECONNABORTED
#define ECONNRESET		WSAECONNRESET
#define ENOBUFS			WSAENOBUFS
#define EISCONN			WSAEISCONN
#define ENOTCONN		WSAENOTCONN
#define ESHUTDOWN		WSAESHUTDOWN
#define ETOOMANYREFS	WSAETOOMANYREFS
#define ETIMEDOUT		WSAETIMEDOUT
#define ECONNREFUSED	WSAECONNREFUSED
#define ELOOP			WSAELOOP
#define EHOSTDOWN		WSAEHOSTDOWN
#define EHOSTUNREACH	WSAEHOSTUNREACH
#define EPROCLIM		WSAEPROCLIM
#define EUSERS			WSAEUSERS
#define EDQUOT			WSAEDQUOT
#define ESTALE			WSAESTALE
#define EREMOTE			WSAEREMOTE

#define EDISCON			WSAEDISCON
#endif

#endif

	////////
	//	make the sock layer verbose or not
	////////
void	c_sock::set_verbose( bool CONST flag )
{
	b_verbose = flag;
}

	////////
	//	wrapper to deal with BSD/WIN32 Deviation
	////////
namespace{
C_PCHAR	get_sock_error( INT32 error_code )
{
	C_PCHAR	str;

	switch( error_code )
	{
	case EFAULT:			str = "The buf argument is not in a valid part of the user address space";				break;
	case ECONNRESET:		str = "The virtual circuit was reset by the remote side";								break;
	case ECONNREFUSED:		str = "The attempt to connect was forcefully rejected";									break;
	case EBADF:				str = "not a valid descriptor";															break;
	case ENOTSOCK:			str = "The descriptor is not a socket.";												break;
	case EADDRNOTAVAIL:		str = "The specified address is not available from the local machine";					break;
	case EADDRINUSE:		str = "The specified address is already in use";										break;
	case EINVAL:			str = "The socket is already bound to an address";										break;
	case EACCES:			str = "The requested address is protected...";											break;
	case EMSGSIZE:			str = "The datagram was too large to fit into the specified buffer and was truncated";	break;
#ifdef	WIN32
	case WSAEINTR:			str = "A blocking operation was interrupted by a call to WSACancelBlockingCall.";		break; 
	case WSANOTINITIALISED:	str = "A successful WSAStartup call must occur before using this function.";			break;
	case WSAENETDOWN:		str = "The network subsystem has failed.";												break;
	case WSAEFAULT:			str = "The buffer pointed to by the optval parameter is not in a valid part of the process address space or the optlen parameter is too small.";	break;
	case WSAEINPROGRESS:	str = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";						break;
	case WSAEINVAL:			str = "The level parameter is not valid, or the information in the buffer pointed to by the optval parameter is not valid.";						break;
	case WSAEADDRINUSE:		str = "WSAEADDRINUSE: Only one usage of each socket address (protocol/network address/port) is normally permitted";		break;
	case WSAENETRESET:		str = "WSAENETRESET: The connection has timed out when SO_KEEPALIVE is set.";			break;
	case WSAECONNRESET:		str = "WSAECONNRESET: An existing connection was forcibly closed by the remote host.";	break;
	case WSAENOPROTOOPT:	str = "The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations).";								break;
	case WSAENOTCONN:		str = "The connection has been reset when SO_KEEPALIVE is set.";						break;
	case WSAEACCES:			str = "WSAEACCES / 10013 : Permission denied. An attempt was made to access a socket in a way forbidden by its access permissions.\n" \
								"\tAn example is using a broadcast address for sendto without broadcast permission being set using setsockopt(SO_BROADCAST).\n" \
								"\tAnother possible reason for the WSAEACCES error is that when the bind function is called (on Windows NT 4.0 with SP4 and later),\n" \
								"\tanother application, service, or kernel mode driver is bound to the same address with exclusive access.\n" \
								"\tSuch exclusive access is a new feature of Windows NT 4.0 with SP4 and later, and is implemented by using the SO_EXCLUSIVEADDRUSE option.";
							break;
	case WSAEAFNOSUPPORT:   str = "WSAEAFNOSUPPORT : Only one usage of each socket address (protocol/network address/port) is normally permitted."; break;
	case WSAENOTSOCK:		str = "An operation was attempted on something that is not a socket.";					break;
	case WSAEHOSTUNREACH:	str = "A socket operation was attempted to an unreachable host";						break;
	case WSAEMSGSIZE:		str = "A message sent on a datagram socket was larger than the internal message buffer or some other network limit,\n" \
								"\tor the buffer used to receive a datagram into was smaller than the datagram itself.";	break;
	case WSAETIMEDOUT:		str = "A connection attempt failed because the connected party did not properly respond after a period of time,\n"
								"\tor established connection failed because connected host has failed to respond";	break;
#endif
	default:				str = nullptr;
							break;
	}
	return str;
}

FINLINE INT32 get_last_error()
{
#ifdef	WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

}

void c_sock::sock_error( INT32 error_code )
{
	C_PCHAR	str = get_sock_error( error_code );

	if( error_code == 0 )
		error_code = get_last_error();

	if( str )
		ERR_PRINT_STRING( "SOCKET %d error %ld : %s", _s, error_code, str );
	else
#ifdef	WIN32
		ERR_PRINT_STRING( "SOCKET %d error %ld", _s, error_code );
#else
		ERR_PRINT_STRING( strerror(error_code) );
#endif
}

namespace {
C_PCHAR_C syno_local[]		= { "localhost", "local", nullptr };
C_PCHAR_C syno_broadcast[]	= { "broadcast", "any", "all", nullptr };
}

UINT32	c_sock::get_ip_addr_from_str( C_PCHAR_C str )
{
	if( str_is_similar_table( str, syno_local ) )
		return htonl( INADDR_LOOPBACK );
	if( str_is_similar_table( str, syno_broadcast ) )
		return htonl( INADDR_BROADCAST );

	UINT32 ip_addr = inet_addr( str );
	if( ip_addr == INADDR_NONE )
	{
		ERR_PRINT_STRING( "%s is not a valid ip address", str );
		ip_addr = INADDR_ANY;
		//BOX_ERR( "%s is not a valid ip address, defaulting to localhost for this time", str );
		//ip_addr = INADDR_LOOPBACK;
	}
	return ip_addr;
}

	////////
	////////
void		c_sock::stop()
{
	if( b_started )
	{
#ifdef	WIN32
		if( b_ws_opened )
		{
			WSACleanup();
			b_ws_opened = false;
		}
#endif
		b_started = false;
	}
}	

	////////
	////////
#ifdef	WIN32
static AAA_ERR	sock_start_win( INT32 version_high, INT32 version_low )
{
	//	The following code fragment demonstrates how an application that supports
	//	only version 1.1 of Windows Sockets makes a WSAStartup call:
	WORD				wVersionRequested; 
	WSADATA				wsaData; 
	int					err;

	wVersionRequested = LOC_MAKEWORD( version_high, version_low ); 
	err = WSAStartup( wVersionRequested, &wsaData ); 
 
	if( err != 0 )
	{
		BOX_ERR( "Can't find a usable Window Socket DLL\nbecause of error %ld", err );
		goto exit;
	}
	b_ws_opened = true;

	// Confirm that the Windows Sockets DLL supports 1.1.
	// Note that if the DLL supports versions greater
	// than 1.1 in addition to 1.1, it will still return 
	// 1.1 in wVersion since that is the version we 
	// requested.

	if ( LOBYTE( wsaData.wVersion ) != version_high || HIBYTE( wsaData.wVersion ) != version_low )
	{ 
		BOX_ERR( "Can't find a usable Window Socket DLL\nbecause it didn't support %ld.%ld",
					version_high, version_low );
		goto exit;
	}
	//	The Windows Sockets DLL is acceptable. Proceed.
	SOCK_PRINT_STRING( "Window DLL (%s) started : %s", wsaData.szDescription,wsaData.szSystemStatus );
	return AAA_OK;
exit:
	return ERR_SOCK_CANT_INIT; 
}
#endif

	////////
	////////
AAA_ERR	c_sock::start()
{
	AAA_ERR	retcode = AAA_OK;
	if( !b_started )
	{
#ifdef	WIN32
		retcode = sock_start_win( 1, 1 );
		if( ERR(retcode) )
			goto exit;
#endif
		//	get host name
		if( gethostname( sock_host_name, MAXHOSTNAME ) == SOCKET_ERROR )
		{
			ERR_PRINT_STRING( "Socket can't get host_name" );
			retcode = ERR_ANY;
			goto exit;
		}
		//	get our address info
		if( ( sock_host_hp = gethostbyname(sock_host_name) ) )
		{
			struct	sockaddr_in	socket_address;
			for( size_t i = 0; ( (sock_host_hp->h_addr_list[ i ]) && (i < MAX_IP_ADDRESS) ); ++i )
			{
				MEMCPY( &socket_address.sin_addr, sock_host_hp->h_addr_list[i], sock_host_hp->h_length, __FUNCTION__ );
				strcpy( ip_address[i], inet_ntoa(socket_address.sin_addr) );
				SOCK_PRINT_STRING( "IP Address %d : %s", i, ip_address[i] );
			}

			sock_host_ip_field_nb = sock_host_hp->h_length;
			for( INT32 i = 0; i < sock_host_ip_field_nb; ++i )
				sock_host_ip_field[i] = *( ((UINT8*)(*sock_host_hp->h_addr_list)) + i );
			b_started = true;
		}
		else
		{
			ERR_PRINT_STRING( "Socket can't get hostent" );
			retcode = ERR_ANY;
			goto exit;
		}
	}

	return	retcode;
exit:
	stop();
	return	retcode; 
}

INT32	c_sock::get_ip_field( INT32 index )
{
	c_sock::start();
	if( b_started )
		return sock_host_ip_field[index];
	else
		return 0;
}

CHAR*	c_sock::get_ip( INT32 index )
{
	c_sock::start();
	if( b_started && INSIDE_MIN_MAX( index, 0, MAX_IP_ADDRESS - 1 ) )
		return ip_address[ index ];
	else
		return 0;
}

INT32	c_sock::get_ip_field_lowest()
{
	c_sock::start();
	if( b_started )
		return sock_host_ip_field[sock_host_ip_field_nb-1];
	else
		return 0;
}

	////////
	//	wrapper to deal with BSD/WIN32 Deviation
	////////
INT32	c_sock::close()
{
	INT32 rc = 0;
	_b_udp_dst = false;
	if( is_valid() )
	{
#ifdef	WIN32	
		rc = closesocket( _s );
#else
		rc = close(_s );
	#endif
		if( rc == SOCKET_ERROR )
		{
			ERR_PRINT_STRING( "SOCKET %d %s() failed", _s, __FUNCTION__ );
			sock_error();
		}
		else
			SOCK_PRINT_STRING( "%d net connection closed on port %d.", _s, get_port_nb() );
		_s = INVALID_SOCKET;
	}
	_b_close_asked = false;
	return rc;
}

c_sock::c_sock()
:_b_udp_dst(false)
,_b_close_asked(false)
{
	_s = INVALID_SOCKET;
}


void c_sock::ask_close()
{
	_b_close_asked = true;
	//int rc = shutdown( _s, SD_BOTH );
	//if( rc == SOCKET_ERROR )
	//{
	//	ERR_PRINT_STRING( "SOCKET %d %s() failed", _s, __FUNCTION__ );
	//	sock_error();
	//}
}

c_sock::~c_sock()
{
	close();
}

	////////
	////////
INT32	c_sock::recv_broadcast( UINT8* buf, INT32 len )
{
	INT32	rc;
//two many message with time out
//	if( b_verbose )
//		SOCK_PRINT_STRING( "%d calling %s() with length = %d", _s, __FUNCTION__, len );
// 
	if( _b_close_asked )
		return -1;

	/*
	*   Receive a packet from anyone 
	*/
#if	0	//this get the address from the sender
	struct sockaddr	read_addr;
	int				read_addr_len;
	read_addr_len = sizeof(read_addr);
	rc = recvfrom( _s, buf, len, 0, (struct sockaddr *)&read_addr, &read_addr_len );
#else
	rc = recvfrom( _s, (CHAR*)buf, len, 0, nullptr, nullptr );
	//SOCK_PRINT_STRING( "recv" );
#endif
	if( rc == 0 )
	{
		SOCK_PRINT_STRING( "%d connection close", _s );
		return -1;	// it closed
	}
	if( rc == SOCKET_ERROR )
	{
		int last = get_last_error();
		if( last == WSAETIMEDOUT )
		{
			if( _b_close_asked )
			{
				SOCK_PRINT_STRING( "%d connection closed after timeout in %s()", _s, __FUNCTION__ );
				return -1;
			}
			rc = 0;
		}
		else
		{
			sock_error( last );
			return 0;	// was return -1
		}
	}
	if( b_verbose && rc > 0 )
	{
		buf[rc] = 0;
		SOCK_PRINT_STRING( "%d IN packet %d bytes : %s", _s, rc, buf );
	}
	//SOCK_PRINT_STRING("return");
	return rc;
}

	////////
	////////
INT32	c_sock::recv( UINT8* buf, INT32 len )
{
	INT32	rc = 0;
	if ( b_verbose )
		SOCK_PRINT_STRING( "%d calling recv with length = %d", _s, len );

	rc = ::recv( _s, (CHAR*)buf, len, 0 );
	if( rc == 0 )
	{
		SOCK_PRINT_STRING( "%d connection close", _s );
		return -1;	// it closed
	}
	if( rc == SOCKET_ERROR )
	{
		int last = get_last_error();
		if( last == WSAETIMEDOUT )
		{
			if( _b_close_asked )
			{
				SOCK_PRINT_STRING( "%d connection closed after timeout in %s()", _s, __FUNCTION__ );
				return -1;
			}
			rc = 0;
		}
		else
		{
			sock_error( last );
			return 0;	// was return -1
		}
	}

	if( b_verbose )
	{
		buf[rc] = 0;
		SOCK_PRINT_STRING( "%d IN packet (%d bytes) : %s", _s, rc, buf );
	}
	return rc;
}

/*
		if( *(buf+1)==0 && *(buf+2)==0 && *(buf+3)==0 ) //nb == 4 && 
			continue;
//				if( *buf != 6 && *buf != 13 && *buf != 21)	//eliminate ACK and CR and Nak
		{
			*(buf+nb) = 0;
			SOCK_PRINT_STRING( "%d :%s", nb, buf );
		}
*/

	////////
	////////
AAA_ERR	c_sock::sendto( UINT32 dst, CONST UINT8* CONST buf, INT32 len )
{
	INT32	rc;
	UINT32	dgrms = 0;

	if( _b_close_asked )
		return SOCKET_ERROR;

	if( len > BLK_SEND_SIZE_MAX )
	{
		debug_break( "c_sock::sendto block too big : %d for a maximum of %d", len, BLK_SEND_SIZE_MAX );
		return ERR_OUT_OF_BOUND;
	}
//	while (1)
//	{
	if( _b_close_asked )
		return SOCKET_ERROR;
	struct sockaddr_in* p_sa = _b_udp_dst ? &_sa_udp_dst : &_sa;
	p_sa->sin_addr.s_addr = dst;
	rc = ::sendto( _s, (CHAR*)buf, len, 0, (CONST struct sockaddr*) p_sa, sizeof(struct sockaddr_in) );
	if( _b_close_asked )
		return SOCKET_ERROR;
	if( rc != len )
	{
		if( rc == SOCKET_ERROR )
			ERR_PRINT_STRING( "SOCKET %d %s() failed", _s, __FUNCTION__ );
		else
			ERR_PRINT_STRING( "SOCKET %d %s() send only %d on %d buffer", _s, __FUNCTION__, rc, len );
		sock_error(0);
		return ERR_ANY;
	}
	if( _b_close_asked )
		return SOCKET_ERROR;
	if( b_verbose )
	{
		SOCK_PRINT_STRING( "%d OUT packet %d (%d bytes) : %.*s", _s, ++dgrms, rc, rc, buf );
//		print_saddr(&read_addr);
	}

//Sleep for a little while so we don't bombard the network 
//	Sleep(Sleep_Time);
	return AAA_OK;
}


	////////
	////////
//AAA_ERR	c_sock::send_broadcast( UINT8* buf, INT32 len )
//{	
//	return sendto( INADDR_BROADCAST, buf, len );
////	return sendto( inet_addr("192.168.1.255"), buf, len );
//}
//see SO_REUSEPORT
AAA_ERR	c_sock::reuse_address_enable()
{
	AAA_ERR		retcode = ERR_ANY;
	INT32		rc;
	BOOL		b_reuse = TRUE;

	rc = setsockopt( _s, SOL_SOCKET, SO_REUSEADDR, (char *)&b_reuse, sizeof(BOOL) );
	if( rc == SOCKET_ERROR )
	{
		ERR_PRINT_STRING( "SOCKET %d Can't set option to reuse address.", _s );
		sock_error();
	}
	else
	{
		if( b_verbose )
			SOCK_PRINT_STRING( "%d Set option to reuse address.", _s );
		retcode = AAA_OK;
	}
	return retcode;
}

AAA_ERR	c_sock::broadcast_enable_send( INT32 broadcast_time_to_leave )
{
//  Enable sending of broadcasts
//	NOTE:	This only needs to be done if you want to SEND
//          broadcast packets.  Reception of broadcast
//			packets will happen automatically.

	AAA_ERR		retcode = ERR_ANY;
	INT32		rc;
	BOOL		optval = TRUE;

	rc = setsockopt( _s, SOL_SOCKET, SO_BROADCAST, (char *)&optval, sizeof(BOOL) );
	if( rc == SOCKET_ERROR )
	{
		ERR_PRINT_STRING( "SOCKET %d Can't set option to broadcast.", _s );
		sock_error();
	}
	else
	{
		//if( b_verbose )
			SOCK_PRINT_STRING( "%d Set option to broadcast.", _s );
		retcode = AAA_OK;
	}

	rc = setsockopt( _s, IPPROTO_IP, IP_TTL, (char *)&broadcast_time_to_leave, sizeof(broadcast_time_to_leave) );
	if( rc == SOCKET_ERROR )
	{
		ERR_PRINT_STRING( "SOCKET %d Can't set time_to_leave to %d.", _s, broadcast_time_to_leave );
		sock_error();
	}
	else
	{
		//if( b_verbose )
			SOCK_PRINT_STRING( "%d Set time_to_leave to %d.", _s, broadcast_time_to_leave );
		retcode = AAA_OK;
	}

	return retcode;
}

UINT16	c_sock::get_port_nb()
{
	return ntohs( _sa.sin_port );
}

void	c_sock::fill_address( struct sockaddr_in* dst, unsigned long addr, UINT16 port_nb )
{
	dst->sin_family = sock_host_hp->h_addrtype;
//	dst->sin_family = AF_INET; 
//	dst->sin_family = AF_UNIX; 
	dst->sin_port = htons(port_nb);
	dst->sin_addr.s_addr = addr;	//	0
//	dst->sin_addr.s_addr = inet_addr("10.12.110.57");

// zero the rest of the struct
#if 1	//faster
	*((UINT64 *)&(dst->sin_zero)) = UINT64(0);
#else
	memset( &(dst->sin_zero), '\0', 8 );
#endif
	
}

	////////
	////////
AAA_ERR	c_sock::establish( UINT16 port_nb )
{
	bool	b_socket_open = false;
	//	create socket
	if( (_s = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET )
	{
		ERR_PRINT_STRING( "SOCKET can't create stream socket on port %d", port_nb );
		goto exit;
	}
	b_socket_open = true;
/*
	struct sockaddr * name;
	int * name_len;
		getsockname( _s, name, name_len );
*/

	//	bind
	fill_address( &_sa, INADDR_ANY, port_nb );
	if( bind( _s, (struct sockaddr *)&_sa, sizeof(_sa) ) == SOCKET_ERROR )
	{
		ERR_PRINT_STRING( "SOCKET %d can't bind stream socket on port %d", _s, port_nb );
		goto exit;
	}

	//	listen
	if( listen( _s, 3 ) == SOCKET_ERROR )	//	max # of queued connects
	{
		ERR_PRINT_STRING( "SOCKET %d can't listen stream socket on port %d", _s, port_nb );
		goto exit;
	}
	if( b_verbose )
		SOCK_PRINT_STRING( "%d for stream established on port %d on %s", _s, port_nb, sock_host_name );

	return AAA_OK;
exit:
	sock_error();
	if( b_socket_open )
		close();
	return ERR_ANY;
}

	////////
	////////
c_sock*	c_sock::accept()
{
	INT32				sin_size = sizeof(struct sockaddr_in);
	struct	sockaddr_in	their_addr;
	c_sock*				pt;

	SOCKET	s_in = ::accept( _s, (struct sockaddr*) &their_addr, &sin_size );
	if( s_in == INVALID_SOCKET )
	{
		ERR_PRINT_STRING( "SOCKET error on accept()" );
		goto exit;
	}
	if( b_verbose )
		SOCK_PRINT_STRING( "%d server: got connection from %s with socket %d", _s, inet_ntoa(their_addr.sin_addr), s_in );

	pt = new c_sock;
	pt->_s = s_in;
	pt->_sa = their_addr;
	return pt;
exit:
	sock_error();
	return nullptr;
}

bool	c_sock::set_timeout_receive( INT32 milisec )
{
	INT32		rc;
	int		nb_milli = milisec;
	rc = setsockopt( _s, SOL_SOCKET, SO_RCVTIMEO, (char *)&nb_milli, sizeof(int));
	if( rc == SOCKET_ERROR )
	{
		ERR_PRINT_STRING( "SOCKET %d Can't set receive time out %d milisec.", _s, nb_milli );
		sock_error();
		return false;
	}
	return true;
}
/*
	rc = setsockopt( _s, SOL_SOCKET, SO_SNDTIMEO, (char *)&nb_milli, sizeof(int));
	if (rc == SOCKET_ERROR)
		{
		ERR_PRINT_STRING( "Can't set socket %d send time out %d milisec.", _s, nb_milli );
		sock_error();
		}
*/
	

	////////
	////////
AAA_ERR	c_sock::broadcast_create( UINT16 port_nb, UINT16 _port_nb_udp_dst, INT32 broadcast_time_to_leave )
{
	bool	b_socket_open = false;

	//	create socket
	//_s = socket( PF_INET, SOCK_DGRAM, 0);
	_s = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if( _s == INVALID_SOCKET )
	{
		ERR_PRINT_STRING( "SOCKET can't create broadcast socket on port %d", port_nb );
		goto exit;
	}
	b_socket_open = true;

	reuse_address_enable();
	//	bind
	fill_address( &_sa, INADDR_ANY, port_nb );
	if( bind( _s, (struct sockaddr *)&_sa, sizeof(_sa )) == SOCKET_ERROR )
	{
		ERR_PRINT_STRING( "SOCKET %d can't bind broadcast socket on port %d", _s, port_nb );
		goto exit;
	}
	if( ERR(broadcast_enable_send( broadcast_time_to_leave )) )
		goto exit;
	//if ( b_verbose )
	{
		SOCK_PRINT_STRING( "%d for Broadcast on port %d on %s", _s, port_nb, sock_host_name );
	}
	if( _port_nb_udp_dst )
	{
		_b_udp_dst = true;
		fill_address( &_sa_udp_dst, INADDR_ANY, _port_nb_udp_dst );
	}
	else
		_b_udp_dst = false;
	return AAA_OK;
exit:
	sock_error();
	if( b_socket_open )
		close();
	return ERR_ANY;
}

void	c_sock::set_non_blocking( bool b_non )
{
	if( is_valid() )
	{
		u_long	data	= b_non ? 1 : 0;
		INT32	rc		= ioctlsocket(_s, FIONBIO, &data );
		if( rc == SOCKET_ERROR )
		{
			ERR_PRINT_STRING( "SOCKET %d Can't set socket to %sblocking.", _s, b_non ? "non" : "" );
			sock_error();
		}
	}
	else
		ERR_PRINT_STRING( "SOCKET Can't set an invalid socket to %sblocking.", b_non ? "non" : "" );
}

	////////
	////////
AAA_ERR	c_sock::call( C_PCHAR_C hostname, INT32 port_nb, bool b_non_blocking )
{
	struct	hostent*	hp;
	bool				b_socket_open = false;

	if( b_verbose )
		SOCK_PRINT_STRING( "Try to Call %s on port %ld", hostname, port_nb );

	if( hostname )
		hp = gethostbyname(hostname);
	else
	{	// we need to find our own address
		hp = sock_host_hp;
	}
	if( !hp )
	{	//	do we know the host's
		ERR_PRINT_STRING( "Didn't find hostname : %s", hostname );
		goto exit_no_mess;
	}

	//	get socket
	if( (_s = socket( hp->h_addrtype, SOCK_STREAM, 0) ) == INVALID_SOCKET )
	{
		ERR_PRINT_STRING( "SOCKET can't create stream socket on port %d", port_nb );
		goto exit;
	}
	b_socket_open = true;
	
/*	INT32		rc;
	int		nb_milli = 1000;
	rc = setsockopt( _s, SOL_SOCKET, SO_RCVTIMEO, (char *)&nb_milli, sizeof(int));
	if (rc == SOCKET_ERROR)
		{
		ERR_PRINT_STRING( "Can't set socket %d receive time out %d milisec.", _s, nb_milli );
		sock_error();
		}
	rc = setsockopt( _s, SOL_SOCKET, SO_SNDTIMEO, (char *)&nb_milli, sizeof(int));
	if (rc == SOCKET_ERROR)
		{
		ERR_PRINT_STRING( "Can't set socket %d send time out %d milisec.", _s, nb_milli );
		sock_error();
		}
*/
	
	//	set address
	MEMCPY( (char *)&_sa.sin_addr, hp->h_addr, hp->h_length, __FUNCTION__ );
	_sa.sin_family = hp->h_addrtype;
	_sa.sin_port = htons((UINT16)port_nb);

	INT32		rc;
	if( b_non_blocking )
	{
		set_non_blocking( true );
		INT32	count = 10;
		do		
		{
			//	connect
			rc = connect( _s, (struct sockaddr *)&_sa, sizeof _sa );
			if( rc == SOCKET_ERROR )
			{				
				rc = WSAGetLastError();
				if( rc==WSAEISCONN )
					break;
				if( rc == WSAEWOULDBLOCK )
					spy::sleep( 100, "sleep c_sock::call()" );
				else
				{
					sock_error( rc );
					goto exit;
				}
			}
			else
				break;
		}
		while( --count > 0 );
		if( count <= 0 )
			goto exit;
		set_non_blocking( false );
	}
	else
	{
		//	connect
		rc = connect( _s, (struct sockaddr *)&_sa, sizeof _sa );
		if ( rc == SOCKET_ERROR )
			goto exit;
	}
	if ( b_verbose )
		SOCK_PRINT_STRING( "%d Call to %s on port %ld succeed", _s, hostname, port_nb );
	return AAA_OK;
exit:
	ERR_PRINT_STRING( "SOCKET %d can't connect stream socket on port %d", _s, port_nb );
	sock_error();
exit_no_mess:
	if( b_socket_open )
		close();
	return ERR_ANY;
}

/*	unused un 2018

#ifdef	WIN32	
	#ifdef	CPP4
		#define	SOCK_DATA_MAX	(256*256*256)
	#else
		#define	SOCK_DATA_MAX	(256*64)
	#endif
#else
	#define	SOCK_DATA_MAX	(256*256*256)
#endif

	////////
	//	return only when everything have been received
	////////
INT32	c_sock::read_all( UINT8* buf, INT32 n )
{
	INT32	count = 0;	//	counts bytes read
	INT32	br = 0;		//	bytes read each pass
	INT32	br_asked;	//	bytes to read each pass
	UINT8*	str = buf;

	while( count < n )
	{	//	loop until full buffer
		br_asked = MIN( (INT32) SOCK_DATA_MAX, n-count );
		if( (br= ::recv( _s, (CHAR*)buf, (size_t)br_asked, 0) ) > 0 )
		{
			count += br;	//	increment byte counter
			buf += br;		//	move buffer ptr for next read
			if ( b_verbose )
				VERBOSE_PRINT_STRING( "\t\tread %ld bytes from socket", count );
		}
		else if( br < 0 ) //	signal an error to the caller
		{
			sock_error();
			return(-1);
		}
		else	//	br = 0;
		{
			DBG_PRINT_STRING( "The remote side has shut down the connection gracefully" );
			return(-1);
		}
	}
	if ( b_verbose )
	{
		if ( n<80)
		{
			DBG_PRINT_STRING( "\tReceived : \"%s\"", str );
		}
		else
		{
			INT32	check_sum = 0;
			INT32	nb = n;
			for( ; nb>0 ; --nb )
				check_sum += (UINT32) (*str++);
			GOOD_PRINT_STRING( "\tReceived block of %ld bytes with checksum : %ld", n, check_sum );
		}
	}
	return(count);
}


	////////
	//	return only when everything have been send	
	////////
INT32	c_sock::write_all( CONST UINT8* buf, INT32 n )
{
	INT32	count = 0;	//	counts bytes read
	INT32	br = 0;		//	bytes wrote each pass
	INT32	br_asked;	//	bytes to write each pass
	CONST UINT8*	str = buf;

	while( count < n )
	{	//	loop until full buffer
		br_asked = MIN( (INT32) SOCK_DATA_MAX, n-count) ;
		if ( (br= send( _s, (CHAR*)buf, (size_t)br_asked, 0) ) > 0 )
		{
			count += br;	//	increment byte counter
			buf += br;		//	move buffer ptr for next write
			if ( b_verbose )
				VERBOSE_PRINT_STRING( "\t\twrote %ld bytes to socket", count );
		}
		else if( br < 0 ) //	signal an error to the caller
		{
			sock_error();
			return(-1);
		}
		else	//	br = 0;
		{
			BOX_ERR( "deal with this case" );
		}
	}
	if( b_verbose )
	{
		if ( n<80 )
		{
			GOOD_PRINT_STRING( "\tSend     : \"%s\"", str );
		}
		else
		{
			INT32	check_sum = 0;
			INT32	nb = n;
			for( ; nb>0; --nb )
				check_sum += (INT32) (*str++);
			GOOD_PRINT_STRING( "\tSend     block of %ld bytes with checksum : %ld", n, check_sum );
		}
	}
	return(count);
}
*/

/*
	////////
	////////
SOCKET	sock_establish( UINT16 port_nb)
{
	SOCKET				s;
	struct sockaddr_in	_sa;
	bool				b_socket_open = false;

//	bzero(&_sa,sizeof(struct sockaddr_in)); // clear our address
	_sa.sin_family = sock_host_hp->h_addrtype;	//	this is from our host address
//	_sa.sin_family = AF_INET; 
//	_sa.sin_family = AF_UNIX; 
	_sa.sin_port = htons(port_nb);
	_sa.sin_addr.s_addr = INADDR_ANY;	//	0

	//	create socket
	if( (s = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET )
//	if( (s = socket( PF_UNIX, SOCK_STREAM, 0)) == INVALID_SOCKET )
	{
		ERR_PRINT_STRING( "can't create stream socket on port %d", port_nb);
		goto exit;
	}
	b_socket_open = true;
	if( bind( s, (struct sockaddr *)&_sa, sizeof(struct sockaddr)) == SOCKET_ERROR )
	{
		ERR_PRINT_STRING( "%d can't bind stream socket on port %d", s, port_nb);
		sock_error();
		goto exit;
	}
	if( listen(s, 3) == SOCKET_ERROR)	//	max # of queued connects
	{
		ERR_PRINT_STRING( "%d can't listen stream socket on port %d", s, port_nb);
		goto exit;
	}
	if( b_verbose )
	{
		SOCK_PRINT_STRING( "%d for stream established on port %d on %s", s, port_nb, sock_host_name);
	}
	return(s);
exit:
	sock_error();
	if( b_socket_open )
		sock_close(s);
	return INVALID_SOCKET;
}

	////////
	////////
SOCKET	sock_get_connection( SOCKET	listener)
{
struct sockaddr_in isa;		//	address of socket
int i;						//	size of address
SOCKET t = INVALID_SOCKET;	//	socket of connection

	if( listener != INVALID_SOCKET )
	{
		//	find socket's address
		i = sizeof(isa);
		getsockname( listener, (struct sockaddr *)&isa, &i);

		//	accept connection if there is one
		if( (t = accept( listener, (struct sockaddr *)&isa, &i)) == INVALID_SOCKET)
		{
			ERR_PRINT_STRING( "%d can't accept connection", listener);
			sock_error();
		}
		else
		{
			if( b_verbose )
			{
				SOCK_PRINT_STRING( "%d Got a call and connect it on %d", listener, t);
			}
		}
	}
	else
		ERR_PRINT_STRING( "Can't listen to an Invalid Socket");

	return(t);
}

	////////
	////////
SOCKET	sock_call( char * hostname, INT32 port_nb)
{
	struct sockaddr_in	_sa;
	struct hostent		*hp;
	SOCKET				s;
	bool				b_socket_open = false;

	if( b_verbose )
	{
		SOCK_PRINT_STRING( "Try to Call %s on port %ld", hostname, port_nb);
	}

	if( !hostname )
	{	// we need to find our own address
		hp = sock_host_hp;
	}
	else
		hp = gethostbyname(hostname);

	if( !hp )
	{	//	do we know the host's
		ERR_PRINT_STRING( "Didn't find hostname : %s", hostname);
		goto exit_no_mess;
	}

	//	set address
	MEMCPY( (char *)&_sa.sin_addr, hp->h_addr, hp->h_length);
	_sa.sin_family = hp->h_addrtype;
	_sa.sin_port = htons((UINT16)port_nb);

	//	get socket
	if( (s = socket( hp->h_addrtype, SOCK_STREAM, 0) ) == INVALID_SOCKET)
	{
		ERR_PRINT_STRING( "can't create stream socket on port %d", port_nb);
		goto exit;
	}
	b_socket_open = true;
	//	connect
	if( connect( s, (struct sockaddr *)&_sa, sizeof _sa ) == SOCKET_ERROR )
	{
		ERR_PRINT_STRING( "%d can't connect stream socket on port %d", s, port_nb);
		goto exit;
	}
	if ( b_verbose )
	{
		SOCK_PRINT_STRING( "%d Call to %s on port %ld suceed", s, hostname, port_nb);
	}
	return(s);
exit:
	sock_error();
exit_no_mess:
	if( b_socket_open )
		sock_close(s);
	return INVALID_SOCKET;
}
*/	
