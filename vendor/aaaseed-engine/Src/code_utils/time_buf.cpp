
#include "aaa_def.h"
#include "time_buf.h"
#include "time_buf_master.h"
#include "time/aaa_time.h"
#include "spy.h"
#include "infrastructure/obj/root.h"
#include "aaaseed.h"
//								TIME_DATA fns
c_tdata::c_tdata()
{
	++g_tbuf_master->_nb_elt;
}

c_tdata::~c_tdata()
{
	--g_tbuf_master->_nb_elt;
}

FINLINE	void	c_tdata::set( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in, DOUBLE CONST time_in )
{
	_value	= val_in;
	_text.set( tex_in );
	_time	= time_in;
	_p		= p_in;
}

FINLINE	void	c_tdata::set( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in )
{
	set( val_in, tex_in, p_in, aaa::time::get_real_time_from_start() );
}

namespace tbuf
{
	C_PCHAR_C	str_ch_name[CH_MAX_NB] =
	{
		"Swap buffer",
		"Video vertical lines",
		"Callback render",
		"render",
		"Draw modules ALL",
		"Draw module",
		"Draw group",
		"Lua",
		"Draw Interface",
		"Capture Callback",
		"Capture Convert",
		"Video Get Frame",
		"Video Move Frame",
		"Video Process",
		"Move to GPU",
		"Read from GPU",
		"OpenGl Move Texture",
		"Compute Task",
		"GL Flush",
		"GL Finish",
		"Open CV",
		"FBO",
		"OpenCL",
		"Sleep",
		"Process Event",
	};

	void	update()
	{
#if	!AAA_WATCHDOG()
		g_tbuf_master->update();
#endif
	}
	void	init()
	{
		GOOD_PRINT_STRING( "Will do obj_get( g_tbuf_master )" );
		node_pref->obj_get( g_tbuf_master );
	}
	void	deinit()
	{
		SAFE_DELETE( g_tbuf_master );
	}
};

bool c_tbuf::b_active_ui = false;

//								TIME_BUFFER obj
c_tbuf::c_tbuf() :
	_last_value(.0f)
{
//	free_min_nb = 10;
	_b_const = true;
//	_ta_inout = new aaa::MUTEX;

	{
		std::lock_guard<aaa::MUTEX> guard(_ta_inout);
		_in_cur		= &_in0;
		_in_other	= &_in1;
//		alloc_free( &free_cur_ );
	}
}

c_tbuf::~c_tbuf()
{
	deinit();
//	delete _ta_inout;
}

void	c_tbuf::deinit()
{
//	if( !_data.empty()  )
	{
		_data.clear();
	}
}
//	should be done seriously
/*
void	c_tbuf::clear()
{
//	std::lock_guard<aaa::MUTEX> guard(*_ta_inout);
//	data.clear();
}
*/

void	c_tbuf::swap_data_path()
{	//	swap the buffer quickly to avoid lockup during add fns
	INT32	try_nb = 0;
	for( ; ; )
	{
		if( _ta_inout.try_lock() )
		{
			SWAP( _in_cur, _in_other );
			_free_cur.insert( _free_cur.end(), _free_other.begin(), _free_other.end() );
			_free_other.clear();

			g_tbuf_master->_nb_elt_free += (INT32) ( _free_cur.size() + _free_other.size() );

			_ta_inout.unlock();
			break;
		}
		else
		{
			if( (++try_nb & 0x1f) == 0 )
				DBG_PRINT_STRING( "%s() can't lock for %d times", __FUNCTION__, try_nb );
			spy::sleep( 1, "sleep c_tbuf::swap_data_path()" );
		}
	}
}

/*
void	c_tbuf::alloc_free( cont_tmp* p)
{	//	make sure there is enough time data ready to avoid allocation during add fns
INT32		i;
c_tdata*	one;
	i = free_min_nb - p->size();	
	for( ; i>0; --i )	//todostl should be doable without loop
		{
		one = new c_tdata;
		p->push_back( one);
		}
}
*/

//	we work on the _other data here
//	the _cur is used by the add process
//	this update is called only by the tbuf_master so in sync
//	so no locking on _data
//todoqq	deal with the sorting
//todoqq	deal with the time interval instead of number of samples
//todoopt	this too slow

void	c_tbuf::update( DOUBLE time )
{
	//	swap the buffer so we can use the incoming data
	swap_data_path();

	//	make sure free data is ready
	//	alloc_free( free_other_ );

	//	first treat the incoming data

	c_tdata*	one;

	size_t		i = _in_other->size();
	for( ; i > 0; --i )
	{
		one = _in_other->front();
		_in_other->pop_front();
		_data.push_back( one );
	}

	if( !_data.empty() )
	{
		DOUBLE time_begin = time - g_tbuf_master->_buf_time_len;
		//	find the old data
		do
		{
			one = _data.front();
			if( one->get_time() < time_begin )
			{	//	discard it but reuse structure
				_data.pop_front();
				_free_other.push_back(one);	//	recycle it
			}
			else
				break;
		}
		while( !_data.empty() );
	}
}


FINLINE	void	c_tbuf::add_low( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in )
{

	DOUBLE time = aaa::time::get_real_time_from_start();	//AAASeed is realtime so no delay to store the time
	c_tdata*	one;

	if( _free_cur.empty() )
		one = new c_tdata;	//todo add another structure to deliver free time data and avoid allocation here
	else
	{	// tdata are recycled
		one = _free_cur.front();
		_free_cur.pop_front();
	}

	_last_value = val_in;
	one->set( val_in, tex_in, p_in, time );
	_in_cur->push_back( one );
}

FINLINE	void	c_tbuf::add( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in )
{
	std::lock_guard<aaa::MUTEX> guard(_ta_inout);	//from here try to be as fast as possible
	add_low( val_in, tex_in, p_in );
}

FINLINE	void	c_tbuf::inc( FP32 CONST inc, C_PCHAR_C tex_in, void CONST * CONST p_in )
{
	std::lock_guard<aaa::MUTEX> guard(_ta_inout);	//from here try to be as fast as possible	
	FP32	val = _last_value + inc;
	add_low( val, tex_in, p_in );
}

FINLINE	void	c_tbuf::dec( FP32 CONST dec, C_PCHAR_C tex_in, void CONST * CONST p_in )
{
	inc( -dec, tex_in, p_in );
}

void	c_tbuf::add( INT32 CONST channel_id, FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in )
{
	c_tbuf*	ptbuf = g_tbuf_master->get_always( channel_id );
	if( ptbuf )	//todo this test trigger romain strange crash on lv but in March 26 20020 Maa put it back while cleaning spy
		ptbuf->add( val_in, tex_in, p_in );
}

void	c_tbuf::inc( INT32 CONST channel_id, FP32 CONST inc_in, C_PCHAR_C tex_in, void CONST * CONST p_in )
{
	c_tbuf*	ptbuf = g_tbuf_master->get_always( channel_id );
	if( ptbuf )	//todo this test trigger romain strange crash on lv but in March 26 20020 Maa put it back while cleaning spy
		ptbuf->inc( inc_in, tex_in, p_in );
}

void	c_tbuf::dec( INT32 CONST channel_id, FP32 CONST dec_in, C_PCHAR_C tex_in, void CONST * CONST p_in )
{
	c_tbuf* ptbuf = g_tbuf_master->get_always( channel_id );
	if( ptbuf )	//todo this test trigger romain strange crash on lv but in March 26 20020 Maa put it back while cleaning spy
		ptbuf->dec( dec_in, tex_in, p_in );
}

