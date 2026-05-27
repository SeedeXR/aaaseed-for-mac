
#ifdef AAA_NET_UTIL_OBJ_H
#error "NET_UTIL_OBJ_H included more than once."
#endif
#define AAA_NET_UTIL_OBJ_H 1


#ifndef AAA_NET_H
#	include "obj_ui/com/net.h"
#endif
#ifndef _UNORDERED_MAP_
#	include <unordered_map>
#endif
#ifndef AAA_NET_BLK_H
#	include "obj_ui/com/net_blk.h"
#endif

CONST	INT32	NET_BUF_SIGNATURE	= 0x42000000;
CONST	INT32	NET_BUF_BEGIN		= NET_BUF_SIGNATURE | 0x1;
CONST	INT32	NET_BUF_END			= NET_BUF_SIGNATURE | 0x2;
CONST	INT32	NET_BUF_EMPTY		= NET_BUF_SIGNATURE | 0x3;
CONST	INT32	NET_BUF_ADD			= NET_BUF_SIGNATURE | 0x4;

class c_net_buf_obj
{
protected:
	UINT8*	_buf;
	UINT8*	_buf_cur;
	INT32	_buf_len;
	INT32	_net_link;
	INT32	_net_channel;
	INT32	_net_channel_sub;
	bool	_b_send;
	bool	_b_receive;
	bool	_b_send_ui;
	bool	_b_receive_ui;
	INT32	_version;
public:
	FINLINE	void	set_version( INT32 version )		{ _version = version; }

	FINLINE	INT32	get_net_channel()			{	return	_net_channel;		}

	FINLINE	INT32*	get_net_link_pt()			{	return	&_net_link;			}
	FINLINE	INT32*	get_net_channel_pt()		{	return	&_net_channel;		}
	FINLINE	INT32*	get_net_channel_sub_pt()	{	return	&_net_channel_sub;	}
	FINLINE	bool*	get_send_ui_pt()			{	return	&_b_send_ui;		}
	FINLINE	bool*	get_receive_ui_pt()			{	return	&_b_receive_ui;		}

	FINLINE	bool	is_send()					{	return	_b_send;			}
	FINLINE	bool	is_receive()				{	return	_b_receive;			}
public:
	c_net_buf_obj()
		:_buf(nullptr)
		,_buf_cur(nullptr)
		,_buf_len(0)
		,_b_send(false)
		,_b_receive(false)
		,_net_link(0)
		,_version(0)
	{
	}
	~c_net_buf_obj()
	{
		FREE(_buf);
	}

	FINLINE	void update()
	{
		bool b_active = is_obj_exist_and_active(net);
		_b_send = b_active && _b_send_ui;
		_b_receive = b_active && _b_receive_ui;
	}
	FINLINE	void flush()
	{
		if( _buf && ((_buf_cur-_buf)!=0) )
		{
			net->sendto( _net_link, c_net::build_channel_id( _net_channel, _net_channel_sub ), c_net::BLK_OBJ_DATA, (UINT8*)_buf, INT32(_buf_cur-_buf) );
			_buf_cur = _buf;
		}
	}
	FINLINE	void get_buffer_ready( INT32 len = c_net::BLK_SIZE_MAX )
	{
		if( !_buf )
		{
			_buf = (UINT8*)MALLOC( len+1 );	//hack there is no free or resize if needed	//todobeurk //hack //todoq //todolv
			_buf_cur = _buf;
			_buf_len = len;
		}
	}
	FINLINE	bool flush_if_less_than( INT32 len_needed )
	{
		if( (_buf_cur-_buf) > (c_net::get_blk_obj_data_size_max()-len_needed-32) )	//hack 32 is a hack
		{
			flush();
			return true;
		}
		return false;
	}

	template<class T>	FINLINE void write_value(		T a )				{	_buf_cur = c_net::write_value(		_buf_cur, a );			}
	template<class T>	FINLINE void write_value(		T a, T b )			{	_buf_cur = c_net::write_value(		_buf_cur, a, b );		}
	template<class T>	FINLINE void write_value(		T a, T b, T c )		{	_buf_cur = c_net::write_value(		_buf_cur, a, b, c );	}
	template<class T>	FINLINE void write_value_v2(	T* a )				{	_buf_cur = c_net::write_value_v2(	_buf_cur, a );			}
	template<class T>	FINLINE void write_value_v3(	T* a )				{	_buf_cur = c_net::write_value_v3(	_buf_cur, a );			}
	template<class T>	FINLINE void write_value_v4(	T* a )				{	_buf_cur = c_net::write_value_v4(	_buf_cur, a );			}
	//	not sure next fns are useful
	template<class T>	FINLINE void read_value(		T* a )				{	_buf_cur = c_net::read_value(		_buf_cur, a );			}
	template<class T>	FINLINE void read_value(		T* a, T* b )		{	_buf_cur = c_net::read_value(		_buf_cur, a, b );		}
	template<class T>	FINLINE void read_value(		T* a, T* b, T* c )	{	_buf_cur = c_net::read_value(		_buf_cur, a, b, c );	}
	template<class T>	FINLINE void read_value_v2(		T* a )				{	_buf_cur = c_net::read_value_v2(	_buf_cur, a );			}
	template<class T>	FINLINE void read_value_v3(		T* a )				{	_buf_cur = c_net::read_value_v3(	_buf_cur, a );			}
	template<class T>	FINLINE void read_value_v4(		T* a )				{	_buf_cur = c_net::read_value_v4(	_buf_cur, a );			}

	template< class OBJ >					void	write_begin();
	template< class CONT, class OBJ >		void	send_cont(			CONT& cont );	
	template< class CONT_PT, class OBJ >	void	send_cont_pt(		CONT_PT& cont );	
	template< class OBJ >					void	send_hd(			OBJ** hd, INT32 nb );	
};

//	reception double buffer for regular data in obj
template< class CONT, class OBJ >
class c_net_data_receiver
{
private:
	CONT	_cont_a;
	CONT	_cont_b;
	CONT*	_p_ready;
	CONT*	_p_in;
	INT32	_version;
	bool	_b_valid_begin;
	bool	_b_data_ready;
public:
	c_net_data_receiver()
		:_version(0)
		,_b_valid_begin(false)
		,_b_data_ready(false)
	{
		_p_ready = &_cont_a;
		_p_in = &_cont_b;
	}
	void	flip_in_as_ready()
	{
		SWAP( _p_in, _p_ready );
		_p_in->clear();
		_b_valid_begin = false;
		_b_data_ready = true;
	}
	void	set_version( INT32 version )		{	_version = version;		}
	CONT&	get_objs()							{	return *_p_ready;		}
	bool	is_data_ready()						{	return _b_data_ready;	}
	void	clear_data_ready()					{	_b_data_ready = false;	}
	void	do_receive( c_net_blk* blk_in );
};



template< class CONT, class OBJ >
void	c_net_data_receiver< CONT, OBJ >::do_receive( c_net_blk* blk_in )
{
	CONST UINT8*	pt = blk_in->get_data_pt();
	INT32	what;
	INT32	version;
	INT32	size;
	CONST UINT8*	pt_max;

	pt_max = pt + blk_in->get_len();
	pt = c_net::read_value( pt, &what );
	switch( what )
	{
	case NET_BUF_BEGIN:
		_p_in->clear();
		pt = c_net::read_value( pt, &version );
		version ^= NET_BUF_SIGNATURE;
		if( version != _version )
		{
			debug_break( "%s() receive version %d when expecting version %d", __FUNCTION__, version, _version );
			flip_in_as_ready();
			break;
		}
		pt = c_net::read_value( pt, &size );
		size ^= NET_BUF_SIGNATURE;
		if( size != OBJ::get_mem_net_size() )
		{
			debug_break( "%s() receive bad size %d instead of ", __FUNCTION__, size, OBJ::get_mem_net_size() );
			flip_in_as_ready();
			break;
		}
		_b_valid_begin = true;
	case NET_BUF_ADD:
		if( _b_valid_begin ) 
		{
			//OBJ	obj_one;
			INT32	net_size = OBJ::get_mem_net_size();
			while( pt < ( pt_max - net_size ) )
			{	
				//pt = obj_one.read_from_mem( pt );
				_p_in->emplace_back();	//push_back cause problem when no copy constructor
				pt = _p_in->back().read_from_mem( pt );
			}
			if( (pt_max-pt)==4 )
			{
				pt = c_net::read_value( pt, &what );
				if( what == NET_BUF_END )
					flip_in_as_ready();
			}
		}
		break;
	case NET_BUF_EMPTY:
		_p_in->clear();
		flip_in_as_ready();
		break;
	default:
		if( (what >> 8) == (NET_BUF_SIGNATURE >>8) )
			debug_break( "%s() bad net code 0x%8x", __FUNCTION__, what );
		else
			debug_break( "%s() this is not a net code 0x%8x", __FUNCTION__, what );
		break;
	}
}

template< class OBJ >
void	c_net_buf_obj::write_begin()
{
	write_value( NET_BUF_BEGIN );
	write_value( _version ^ NET_BUF_SIGNATURE );
	write_value( OBJ::get_mem_net_size() ^ NET_BUF_SIGNATURE );
}

template< class CONT, class OBJ >
void	c_net_buf_obj::send_cont( CONT& cont )
{
	get_buffer_ready();

	//todo sign the blk
	if( cont.empty() )
	{	//we just send a block saying empty
		write_value( NET_BUF_EMPTY );
		flush();
	}
	else
	{	//we send blocks START then ADDs, and finish by a END 
		write_begin<OBJ>();
		for( auto const & elt : cont )
		{
			if( flush_if_less_than( 100 ) )
				write_value( NET_BUF_ADD );
			elt.write_to_mem( this );
		}

		write_value( NET_BUF_END );
		flush();
	}
}

template< class CONT_PT, class OBJ >
void	c_net_buf_obj::send_cont_pt( CONT_PT& cont )
{
	get_buffer_ready( );

	//todo sign the blk
	if( cont.empty() )
	{	//we just send a block saying empty
		write_value( NET_BUF_EMPTY );
		flush();
	}
	else
	{	//we send blocks START then ADDs, and finish by a END 
		write_begin<OBJ>();
		for( auto const & elt : cont )
		{
			if( flush_if_less_than( 100 ) )
				write_value( NET_BUF_ADD );				//break;
			elt->write_to_mem( this );
		}

		write_value( NET_BUF_END );
		flush();
	}
}

template< class OBJ >
void	c_net_buf_obj::send_hd( OBJ** hd, INT32 nb )
{
	get_buffer_ready( );

	//todo sign the blk
	if( nb==0 )
	{	//we just send a block saying empty
		write_value( NET_BUF_EMPTY );
		flush();
	}
	else
	{	//we send blocks START then ADDs, and finish by a END 
		--hd;
		write_begin<OBJ>();
		for( ; nb > 0; --nb )
		{
			if( flush_if_less_than( 100 ) )
			{
				write_value( NET_BUF_ADD );				//break;
			}
			(*++hd)->write_to_mem( this );
		}
		write_value( NET_BUF_END );
		flush();
	}
}

//	to receive several channel and machine we need a pool of receiver
template< class CONT, class T >
class c_map_receiver
{
public:
	typedef	c_net_data_receiver< CONT, T >				receiver_type;
	typedef	std::unordered_map< INT32, receiver_type* >	map_type;
private:
	map_type	_map;
public:
	//todo dealloc()
	receiver_type*	get_receiver( INT32 key_1, INT32 key_2 = 0 )
	{
		INT32	key = key_1 + (key_2 << 16);
		auto it = _map.find( key );
		if ( it != _map.end() )
			return it->second;
		receiver_type* pt = new receiver_type;
		_map[key] = pt;
		return pt;
	}
	map_type&	get_map_ref() {	return _map; } 
};

