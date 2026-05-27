
#ifdef AAA_MOVIE_MS_H
#error "MOVIE_MS_H included more than once."
#endif
#define AAA_MOVIE_MS_H 1

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern	void	movie_position( INT32 x, INT32 y);
extern	void	movie_position( INT32 x, INT32 y, INT32 w, INT32 h );

extern	void	movie_choose();
extern	void	movie_choose( CHAR* filename );
extern	void	movie_play();
extern	void	movie_stop();
extern	void	movie_rewind();
extern	void	movie_close();

extern	void	movie_choose_and_play_async( CHAR* filename );
extern	BOOL	movie_is_playing();
