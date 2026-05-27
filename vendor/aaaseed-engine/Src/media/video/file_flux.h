
#ifdef AAA_FILE_FLUX_H
#error "FILE_FLUX_H included more than once."
#endif
#define AAA_FILE_FLUX_H 1


#ifndef AAA_VIDEO_PLAYER_H
#	include "video_player.h"
#endif

class	c_thread_img_seq;

class	c_movie_img_seq final : public c_movie_player
{
protected:
	c_img_2d*				_image;

//	mutable aaa::MUTEX*		_lock_fileflux;
	c_thread_img_seq*		_thread_img_seq;
	INT32					_first_frame_nb;
	INT32					_frames_nb;
	UINT32					_index_first;
	INT32					_index_length;		// length of index in filename

	o_str					_root_filename;
	o_str					_ext;
	o_str					_file_to_check;

	INT32					_nb_bits_out;

//	INT32					preroll_index;
	INT32					_frame_index;
	INT32					_last_frame_index;
	bool					_b_back;
	INT32*					_preroll_index;
	bool*					_preroll_loaded;
	bool*					_preroll_moved;
	std::vector<c_img_2d*>	_preroll_img;
//	INT32					_preroll_index_cur;
//	INT32					_frame_index_max;

			//void	load_frame_ex( INT32 frame_index, INT32 i );

			void	load_frame( INT32 CONST frame_index, INT32 CONST i );
			bool	got_frame_data( INT32 CONST frame_index, INT32 CONST preroll_index );
			void	move_frame( INT32 CONST i );

//private:
//	void	c_file_flux::update_deal_with_this_image( INT32 preroll_index, INT32 frame_index );
public:
			c_movie_img_seq( c_image_flux_buffer* buf );
	virtual	~c_movie_img_seq();

	virtual	void	update_async();

	virtual	AAA_ERR	open_specific();
	virtual	void	close_specific();
	virtual	aaa::MOVIE_LIB	get_type();

	virtual	void	play();
	virtual	void	pause();
	virtual	void	stop();
	virtual	void	update();
//	virtual	void	start();

	virtual DOUBLE	set_rate( DOUBLE rate )	{ _rate = rate; return rate; }

	virtual	void	set_position( REAL CONST time );
	virtual	REAL	get_position();

//	virtual	INT32	get_bit_depth()		{ return _nb_bits_out; }
};

