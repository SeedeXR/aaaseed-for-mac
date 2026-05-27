
#ifdef AAA_NET_BLK_H
#error "NET_BLK_H included more than once."
#endif
#define AAA_NET_BLK_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif
#ifndef AAA_NET_H
#	include "net.h"
#endif

//#if	AAA_NET()

class	c_net_blk final
{
public:
	enum MIDI_CODE : INT32
	{
		MIDI_CODE_CONTROL	= 256,
		MIDI_CODE_PROGRAM	= 257,
		MIDI_CODE_NOTEON	= 258,
		MIDI_CODE_NOTEOFF	= 259,
		MIDI_CODE_VELOCITY	= 260,
		//	MIDI_CODE_CONTROL = 256,
		MIDI_CODE_MAX_NB,
	};

	UINT8*				_data_osc;	//todo this is a hack for now
private:
	INT32				_link;
	INT32				_channel;
	INT32				_channel_sub;
	INT32				_sender;
	c_net::BLK_TYPE		_type;
	INT32				_blk_id;
	DOUBLE				_time;
	DOUBLE				_time_received;
	UINT8*				_data;
	INT32				_len;
//	INT32				_checksum;
	bool				_b_valid;

	INT32				_data_size;

	mutable aaa::MUTEX	_do_process_lock;
public:
	c_net_blk();
	~c_net_blk();
	C_NO_CPY_MOVE(c_net_blk)

					void			alloc( INT32 CONST byte_nb );
					void			dealloc();

					void			store_data(								UINT8 CONST * CONST data,	INT32 len );
//					void			set_data(								UINT8 CONST * CONST data,	INT32 len );
					void			check_and_store(	INT32 CONST link,	UINT8 CONST * buf,			INT32 len );

	FINLINE			INT32			get_link()						CONST NOEXCEPT	{	return _link;					}
	FINLINE			INT32			get_channel()					CONST NOEXCEPT	{	return _channel;		}
	FINLINE			INT32			get_channel_sub()				CONST NOEXCEPT	{	return _channel_sub;	}
//	FINLINE			INT32			get_channel_and_sub()			CONST  NOEXCEPT	{	return c_net::build_channel_id(_channel, _channel_sub);	}
	FINLINE			INT32			get_sender()					CONST NOEXCEPT	{	return _sender;			}
	FINLINE			INT32			get_blk_id()					CONST NOEXCEPT	{	return _blk_id;			}
	FINLINE			DOUBLE			get_time()						CONST NOEXCEPT	{	return _time;			}
	FINLINE			DOUBLE			get_time_received()				CONST NOEXCEPT	{	return _time_received;	}
	FINLINE			c_net::BLK_TYPE	get_type()						CONST NOEXCEPT	{	return _type;			}
	FINLINE			bool			is_type( c_net::BLK_TYPE type )	CONST NOEXCEPT	{	return _type == type;	}
	FINLINE			bool			is_type_osc()					CONST NOEXCEPT	{	return _type == c_net::BLK_OSC || _type == c_net::BLK_BUNDLE_OSC;	 }
	FINLINE			INT32			get_len()						CONST NOEXCEPT	{	return _len;			}
//	FINLINE			INT32			get_checksum()					CONST NOEXCEPT	{	return checksum_;		}
	FINLINE			bool			is_valid()						CONST NOEXCEPT	{	return _b_valid;		}
	FINLINE			UINT8 CONST *	get_data_pt()					CONST NOEXCEPT	{	return _data;			}

					void			print()							CONST;
	FINLINE			bool			is_immediate()					CONST NOEXCEPT;
	FINLINE			bool			is_processed_by_do_process()	CONST NOEXCEPT;
					bool			do_process( bool b_verbose );
};

FINLINE	bool	c_net_blk::is_immediate() CONST NOEXCEPT
{
	switch( _type )
	{
	case c_net::BLK_HELLO:
	case c_net::BLK_PING:
	case c_net::BLK_BYE:
		return true;
	default:
		return false;
	}
}

FINLINE	bool	c_net_blk::is_processed_by_do_process() CONST NOEXCEPT
{
	switch( _type )
	{	//	so all except these
	case c_net::BLK_OBJ_DATA:
	case c_net::BLK_ASCII:
	case c_net::BLK_LINK:
		return false;
	default:
		return true;
	}
}

