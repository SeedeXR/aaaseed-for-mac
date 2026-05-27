

#ifdef AAA_WIN_AVI_H
#error "WIN_AVI_H included more than once."
#endif
#define AAA_WIN_AVI_H 1

#ifndef AAA_VIDEO_PLAYER_H
#	include "video_player.h"
#endif
#ifndef _INC_VFW
#	include "vfw.h"
#endif

class	c_movie_avi final : public c_movie_player
{
protected:
	bool				_b_delta_t_update;
	PAVIFILE			_pavi_file;

	PAVISTREAM			_stream;
	INT32				_stream_start;
	INT32				_stream_end;
	INT32				_stream_index;
	INT32				_stream_length;

	INT32				_video_frame_last;

	PGETFRAME			_pgf;

	LPBITMAPINFOHEADER	_pbitmap;
public:
			c_movie_avi(c_image_flux_buffer* buf);
	virtual	~c_movie_avi();

	static	INT32	lib_open_count;
	static	void	lib_open();
	static	void	lib_close();

	virtual	AAA_ERR	open_specific();
	virtual	void	close_specific();

	virtual	aaa::MOVIE_LIB	get_type();

	virtual	void	stop()			{}
	virtual	void	pause()			{}
	virtual	void	start()			{}
	virtual	void	play()			{}

			void	update();

			REAL	get_position()								{ return _time_video; }
			void	set_position( REAL CONST pos );

	//this here for symmetry but unused time is dealt by another mechanismUSDA
	//virtual	void	set_position( REAL in );
	//virtual	REAL	get_position();

	virtual DOUBLE	set_rate( DOUBLE rate )						{ return 1.; }

			void	get_frame_data( INT32 CONST index );
			void	get_frame_data_next();
			void	get_frame_data_from_time( REAL CONST in );
			INT32	get_frame_index_from_time( REAL CONST in );
			INT32	get_frame_index_cur();


//	FINLINE	LPBITMAPINFOHEADER
//					get_pbitmap()	{ return pbitmap; }
//	virtual	INT32	get_bit_depth()	{ return _pbitmap->biBitCount; }
};
