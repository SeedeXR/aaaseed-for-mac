
#ifdef AAA_TIME_BUF_H
#error "TIME_BUF_H included more than once."
#endif
#define AAA_TIME_BUF_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

class c_obj_ui;

class	c_tdata final
{
protected:
	DOUBLE			_time;		//in millisec ?
	FP32			_value;
	o_str			_text;
	void CONST *	_p;
public:
	c_tdata();
	virtual	~c_tdata();

	FINLINE	void	set( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in );
	FINLINE	void	set( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in, DOUBLE CONST time );

	FINLINE	FP32				get_value()		CONST		{ return _value; };
	FINLINE	DOUBLE				get_time()		CONST		{ return _time; };
	FINLINE	o_str CONST *		get_text()		CONST		{ return &_text; };
	FINLINE	void CONST *		get_pointer()	CONST		{ return _p; };
	FINLINE	c_obj_ui CONST *	get_obj_ui()	CONST		{ return (c_obj_ui*) _p; };
};


class	c_tbuf final : public c_obj
{
	friend class c_tbuf_master;
public:
	typedef	std::deque<c_tdata*>	cont_data;
	typedef	cont_data::iterator		iter_data;

	typedef	std::deque<c_tdata*>	cont_tmp;
	typedef	cont_tmp::iterator		iter_tmp;
protected:
	cont_tmp*			_in_cur;
	cont_tmp*			_in_other;

	cont_tmp			_in0;
	cont_tmp			_in1;
	cont_tmp			_free_cur;
	cont_tmp			_free_other;

	o_str				_name;
	cont_data			_data;
//	INT32				size;

	FP32				_last_value;
	
	mutable aaa::MUTEX	_ta_inout;

	bool				_b_const;	//tell if value is constant between sample 
//	INT32				free_min_nb;
	static bool			b_active_ui;
private:
	FINLINE	void	add_low( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in );
			void	swap_data_path();
			void	deinit();
//	void	alloc_free( cont_tmp* p);
protected:
	FINLINE	void	add( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in );
	FINLINE	void	inc( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in );
	FINLINE	void	dec( FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in );
public:
	static FINLINE bool	is_active()	{	return b_active_ui;	}
	static void	add( INT32 CONST channel_id, FP32 CONST val_in, C_PCHAR_C tex_in, void CONST * CONST p_in );
	static void	inc( INT32 CONST channel_id, FP32 CONST inc_in, C_PCHAR_C tex_in, void CONST * CONST p_in );
	static void	dec( INT32 CONST channel_id, FP32 CONST dec_in, C_PCHAR_C tex_in, void CONST * CONST p_in );

	c_tbuf();
	virtual	~c_tbuf();

//	void	clear();

//	void	sort();

			void	update( DOUBLE time );

	FINLINE cont_data *	get_data()					{ return &_data; } ;

	FINLINE	bool	is_const()	CONST				{ return _b_const; }
	FINLINE	void	set_const( bool CONST b_in )	{ _b_const = b_in; }
};

namespace tbuf
{
	enum CH : INT32
	{
		CH_SWAP_BUF = 0,
		CH_VBL,
		CH_CALLBACK_DISPLAY,
		CH_RENDER,
		CH_DRAW_MODULES,
		CH_DRAW_MODULE,
		CH_DRAW_LAYERS,
		CH_LUA,
		CH_DRAW_UI,
		CH_CAPTURE_CALLBACK,
		CH_CAPTURE_CONVERT,
		CH_VIDEO_GET_FRAME,
		CH_VIDEO_MOVE_FRAME,
		CH_VIDEO_PROCESS,
		CH_GPU_MOVE,
		CH_GPU_READ,
		CH_GL_TEX,
		CH_COMPUTE_TASK,
		CH_GL_FLUSH,
		CH_GL_FINISH,
		CH_OPEN_CV,
		CH_FBO_RENDER,
		CH_OPENCL,
		CH_SLEEP,
		CH_PROCESS_EVENT,
		CH_MAX_NB
	};

	extern	C_PCHAR_C	str_ch_name[CH_MAX_NB];
	extern	void	init();
	extern	void	deinit();
	extern	void	update();
}

#ifndef TBUF_ADD_OBJ
#	define TBUF_ADD_OBJ(	channel_id, val, text,	obj )	{	if( c_tbuf::is_active() )	c_tbuf::add( channel_id, FP32(val), text, obj );		}
#	define TBUF_ADD(		channel_id, val, text	)		{	if( c_tbuf::is_active() )	c_tbuf::add( channel_id, FP32(val), text, nullptr );	}
#	define TBUF_INC_OBJ(	channel_id, val, text,	obj )	{	if( c_tbuf::is_active() )	c_tbuf::inc( channel_id, FP32(val), text, obj );		}
#	define TBUF_INC(		channel_id, val, text	)		{	if( c_tbuf::is_active() )	c_tbuf::inc( channel_id, FP32(val), text, nullptr );	}
#	define TBUF_DEC_OBJ(	channel_id, val, text,	obj )	{	if( c_tbuf::is_active() )	c_tbuf::dec( channel_id, FP32(val), text, obj );		}
#	define TBUF_DEC(		channel_id, val, text	)		{	if( c_tbuf::is_active() )	c_tbuf::dec( channel_id, FP32(val), text, nullptr );	}
#endif


