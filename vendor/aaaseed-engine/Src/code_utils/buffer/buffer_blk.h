
#ifdef AAA_BUFFER_BLK_H
#error "BUFFER_BLK_H included more than once."
#endif
#define AAA_BUFFER_BLK_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

class c_buffer_blk;

// used for snd / sound

class c_buffer_stream final : public c_obj
{
	static	INT32	count_in_total;
	static	INT32	count_out_total;

private:
	INT32							_blk_size;
	INT32							_nb_in_last_blk;
	INT32							_count_in;
	INT32							_count_out;

	std::deque< c_buffer_blk* >		_blk;
	std::vector< c_buffer_blk* >	_blk_free;

	mutable aaa::MUTEX				_lock;

	c_buffer_blk*	get_blk_free();	
	void			dealloc();
public:
	static	INT32*	get_count_in_pt()		{	return &count_in_total;	}
	static	INT32*	get_count_out_pt()		{	return &count_out_total;	}

	c_buffer_stream();
	~c_buffer_stream();


	void	put( UINT8 CONST * CONST src, INT32 size );
	INT32	get( UINT8* CONST dst, INT32 CONST len_dst );

	INT32	get_count_in()	CONST	{	return _count_in;	}
	INT32	get_count_out()	CONST 	{	return _count_out;	}

	void	set_blk_size(	INT32 CONST size );
};

