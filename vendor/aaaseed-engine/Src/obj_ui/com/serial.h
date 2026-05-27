
#ifdef AAA_SERIAL_H
#error "SERIAL_H included more than once."
#endif
#define AAA_SERIAL_H 1

#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

class	c_serial final : public c_obj
{
public:
	enum BAUDRATE : INT32
	{
		BAUDRATE_110	= 0,
		BAUDRATE_300	= 1,
		BAUDRATE_600	= 2,
		BAUDRATE_1200	= 3,
		BAUDRATE_2400	= 4,
		BAUDRATE_4800	= 5,
		BAUDRATE_9600	= 6,

		BAUDRATE_14400	= 7,
		BAUDRATE_19200	= 8,
		BAUDRATE_38400	= 9,
		BAUDRATE_56000	= 10,
		BAUDRATE_57600	= 11,

		BAUDRATE_115200	= 12,
		BAUDRATE_128000	= 13,
		BAUDRATE_256000	= 14,
			
//		BAUDRATE_460800,
//		BAUDRATE_921600,
		BAUDRATE_MAX_NB
	};
	static	C_PCHAR_C	str_baudrate[ BAUDRATE_MAX_NB ];

	static	INT32 CONST	PORT_NB_MAX = 32;
	static	C_PCHAR_C	str_port[ PORT_NB_MAX + 1 ];
private:
	bool			_b_active;
	FILE *			_p;
	BAUDRATE		_baudrate;
	INT32			_port_nb;
#ifdef	WIN32
	HANDLE			_h_com;
	OVERLAPPED		_over_reader;
	OVERLAPPED		_over_writer;
	OVERLAPPED		_over_flush;
	unsigned long	_nb_read;
	bool			_b_read_waiting;
	UINT8 CONST *	_buf_read;
	UINT32			_nb_to_read;
//	COMMTIMEOUTS	_timeout_read;
//	COMMTIMEOUTS	_timeout_write;
	INT32			_timeout_wait_for_single_object_second_milli;
#endif
	o_str			_port_name;
#if	AAA_DEBUG()
	bool			_b_verbose;
#endif
			AAA_ERR		write_low(		UINT8 CONST * buf,	UINT32 nb );
			void		print_err(		C_PCHAR_C fn_name, C_PCHAR_C mess );
public:
	c_serial();
	~c_serial();

			void		idle();
			AAA_ERR		open(			INT32 port_nb_in,	INT32 parity = NOPARITY );
			void		close();

			AAA_ERR		write(			UINT8 CONST * c		);
			AAA_ERR		write(			UINT8 CONST * c,	INT32 CONST len );
	FINLINE AAA_ERR		write(			C_PCHAR_C c,		INT32 CONST len )	{	return write( reinterpret_cast<UINT8 CONST *>(c), len );	}

			AAA_ERR		test();
		
			INT32		read_get_nb_ready();
		
			AAA_ERR		read(			UINT8 * c			);
			AAA_ERR		read(			UINT8 * c,			INT32 nb );
			AAA_ERR		read_ask(		UINT8 CONST * buf,	UINT32 nb );
			AAA_ERR		read_get();
//			AAA_ERR		read_until(		UINT8 * c,			UINT8 stop_char );
			AAA_ERR		read_line(		UINT8 * c,			INT32 nb_max = 64 );
			AAA_ERR		wait_for(		UINT8 c,			INT32 try_nb );
			AAA_ERR		wait_for(		UINT8 c				);
			AAA_ERR		wait_for_str(	UINT8 * c,			INT32 try_nb );
			AAA_ERR		wait_for_str(	UINT8 * c			);
			AAA_ERR		flush();
		
			AAA_ERR		set_baudrate(	BAUDRATE			baudrate_in );
			void		set_timeout_wait_for_single_object(	INT32 second_milli )	{	_timeout_wait_for_single_object_second_milli = second_milli; }
};


