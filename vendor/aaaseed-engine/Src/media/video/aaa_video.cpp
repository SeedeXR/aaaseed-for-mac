#include "win_avi.h"
#include "infrastructure/bind/bind.h"
#include "media/video/aaa_video.h"
#include "DirectShow/ds_avi.h"
#include "mov_quicktime.h"
#include "mov_ffmpeg.h"
#include "media/video/tex_video.h"
#include "infrastructure/layer/app.h"
#include "file_flux.h"

c_bind* g_video_bank_def;

template< class T >
class	c_flux_store_elt
{
	std::vector<T*>	_vector_pt;
public:
	bool	find_iterator( std::vector<T*>::iterator& it, c_tex_video CONST * CONST caller )
	{
		//std::vector<T*>::const_iterator	it_end = _vector_pt.end();
		auto it_end = _vector_pt.end();
		for( it=_vector_pt.begin(); it!=it_end; ++it )
		{
			if( (*it)->get_tex_video() == caller )
				return true;
		}
		return false;
	}
	void release_all()
	{
		for( auto const & elt : _vector_pt )
			delete elt;
		_vector_pt.clear();
	}
	void remove( std::vector<T*>::iterator& it )
	{
		_vector_pt.erase( it );
	}
	void	add( T * CONST pt )
	{
		_vector_pt.push_back( pt );
	}
	bool	remove( c_tex_video CONST * CONST tex_video )
	{
		typename std::vector<T*>::iterator it;
		bool b_ret = find_iterator( it, tex_video );
		if( b_ret )
			remove( it );
		return b_ret;
	}

	bool release( c_tex_video * CONST tex_video, c_movie_player * CONST obj )
	{
		bool b_ret = remove( tex_video );
		if( b_ret )
		{
			if( obj->get_tex_video()==tex_video )
				delete obj;
			else
				debug_break( "%s() movie player already deleted: skypping delete", __FUNCTION__ );
			if( tex_video->get_movie_player() == obj )
				tex_video->set_movie_player( nullptr );
		}
		return b_ret;
	}
	void release_by_tex_video( c_tex_video * CONST tex_video )
	{
		INT32 CONST nb = (INT32)_vector_pt.size();
		for( INT32 i=0; i<nb; ++i )
		{
			if( release( tex_video, _vector_pt[i] ) )
				break;
		}
	}
};

template< class T >
class	c_flux_store
{
	//todo use a vector< map<c_tex_video*,T*> >
	std::vector< c_flux_store_elt<T> > _by_bind;	//	external vector is for the video bind, the internal one is for the tex_video objects
											//	the goal of this is to have a unique object for each bind and video_object
//	std::map< T*, INT32 >		obj_to_bind;	//	
private:
	c_flux_store_elt<T> * get_elt( INT32 CONST bind )
	{
		//todo 2025 May we have a but here we protect from it but yet to be solved
		if( INSIDE_MIN_MAX( bind, INT32(0), INT32(_by_bind.size())-1 ) )
			return &_by_bind[bind];
		else
		{
			return nullptr;
		}
	}


	FINLINE bool	find_iterator( typename std::vector<T*>::iterator& it, c_tex_video CONST * CONST tex_video, INT32 CONST bind )
	{
		c_flux_store_elt<T> * elt =  get_elt( bind );
		if( elt )
			return elt->find_iterator( it, tex_video );
		return false;
	}
public:
	c_flux_store<T>( INT32 CONST nb )
	{
		c_flux_store_elt<T>	vector_pt;
		_by_bind.assign( nb, vector_pt );
		//_by_bind.reserve( nb );
		//for( ; nb>0; --nb )
		//	_by_bind.push_back( vec );
	}
	void release_all()
	{
		for( auto & elt_out : _by_bind )
			elt_out.release_all();
		_by_bind.clear();
	}
	~c_flux_store<T>()
	{
		release_all();
	}

	T*	find( c_tex_video * CONST tex_video, INT32 CONST bind )
	{
		typename std::vector<T*>::iterator it;
		if( find_iterator( it, tex_video, bind ) )
			return *it;
		return nullptr;
	}

	T*	get( c_tex_video * CONST tex_video, INT32 CONST bind )
	{
		T* pt = find( tex_video, bind );
		if( pt )
			return pt;

		pt = find( nullptr, bind );
		if( !pt )
		{
			pt = new T( tex_video->get_image_flux_buf() );
			if( pt )	//todo to be refined
			{		
				pt->init_post_constructor();
				pt->store_bind( bind );
				pt->set_tex_video( tex_video );
				//	obj_to_bind[pt] = bind;
				_by_bind[bind].add( pt );
			}	
		}	
		return pt;
	}
/*
	bool	remove( c_tex_video CONST * CONST tex_video, INT32 bind )
	{
		return _by_bind[bind].remove( tex_video );
	}
	void remove( c_tex_video* tex_video, c_movie_player* obj )
	{
		remove( tex_video,  obj->get_bind() );
	}
	*/
	//todo is this enough ?
	void release( c_tex_video * CONST tex_video, c_movie_player * CONST obj )
	{
		c_flux_store_elt<T> * elt =  get_elt( obj->get_bind() );
		if( elt )
			elt->release( tex_video, obj );
	}
	//was bad before
	//dirty but better
	void release_by_tex_video( c_tex_video * CONST tex_video )
	{
		INT32 CONST nb = (INT32)_by_bind.size();
		for( INT32 i=0; i<nb; ++i )
			_by_bind[i].release_by_tex_video(tex_video);
	}
};

c_flux_store<c_movie_ds>			store_ds(			MOVIE_MAX_NB	);
c_flux_store<c_movie_avi>			store_avi(			MOVIE_MAX_NB	);
#if AAA_USE_QUICKTIME()
c_flux_store<c_movie_qt>			store_qt(			MOVIE_MAX_NB	);
#endif //#if AAA_USE_QUICKTIME()
#if AAA_USE_FFMPEG()
c_flux_store<c_movie_ffmpeg>		store_ffmpeg(		MOVIE_MAX_NB	);
#endif //#if AAA_USE_FFMPEG()
c_flux_store<c_movie_img_seq>		store_img_seq(		MOVIE_MAX_NB	);
//c_flux_store<c_movie_mf>			store_mf_movie(		MOVIE_MAX_NB	);

static	AAA_ERR	video_allocate()
{
	if( !g_video_bank_def )
		g_video_bank_def = c_bind::get_new( "video_bind" );
	if( g_video_bank_def )
	{
		g_video_bank_def->set( MOVIE_MAX_NB, "Video Bind", "video_bind", aaa::file::TYPE_IO_VIDEO, MOVIE_MAX_NB/32, true, true, true );
		return AAA_OK;
	}
	return ERR_ANY;
}

static	void	video_deallocate()
{
	if( g_video_bank_def )
	{
		store_ds.release_all();
		store_avi.release_all();
#if AAA_USE_QUICKTIME()
		store_qt.release_all();
#endif //#if AAA_USE_QUICKTIME()
#if AAA_USE_FFMPEG()
		store_ffmpeg.release_all();
#endif //#if AAA_USE_FFMPEG()
		store_img_seq.release_all();
		//store_mf_movie.release_all();

		SAFE_DELETE( g_video_bank_def );
	}
}

AAA_ERR	video_init()
{
	AAA_ERR	retcode = video_allocate();
	if ( ERR(retcode) )
		BOX_ERR( "Can't allocate the video list" );
	return retcode;
}

void	video_deinit()
{
	video_deallocate();
}

C_PCHAR_C	video_get_bind_filename( INT32 index )
{
	return g_video_bank_def->get_str( index );
}
void	video_set_bind_name( o_str CONST & filename_in )
{
	g_video_bank_def->set_item( g_video_bank_def->get_index_cur(), filename_in );
}
void	video_set_bind_name( INT32 index, o_str CONST & filename_in )
{
	g_video_bank_def->set_item( index, filename_in );
}

void	video_update()
{
}

static	void	MAACALLBACK	video_menu_fn( INT32 in )
{
	g_video_bank_def->set_index_cur( in );
	if( c_tex_video::cur )
		c_tex_video::cur->set_video_bind_ui( in );
}

INT32	movie_build_bind_menu()
{
	return g_video_bank_def->menu_build( 0, video_menu_fn);
}



//AVI Direct SHOW
c_movie_ds*				movie_get_ds(		c_tex_video* CONST tex_video, INT32 CONST bind )		{	return	store_ds.get(		tex_video, bind );	}
//AVI Video for window
c_movie_avi*			movie_get_avi(		c_tex_video* CONST tex_video, INT32 CONST bind )		{	return	store_avi.get(		tex_video, bind );	}

//QUICKTIME
#if AAA_USE_QUICKTIME()
c_movie_qt*				movie_get_qt(		c_tex_video* CONST tex_video, INT32 CONST bind )		{	return	store_qt.get(		tex_video, bind );	}
#endif //#if AAA_USE_QUICKTIME()

//FFMPEG
#if AAA_USE_FFMPEG()
c_movie_ffmpeg*			movie_get_ffmpeg(	c_tex_video* CONST tex_video, INT32 CONST bind )		{	return	store_ffmpeg.get(	tex_video, bind );	}
#endif	//#if AAA_USE_FFMPEG()

//IMAGE SEQUENCE (previously name FILE_FLUX)
c_movie_img_seq*		movie_get_img_seq(	c_tex_video* CONST tex_video, INT32 CONST bind )		{	return	store_img_seq.get(	tex_video, bind );	}
//MEDIA FOUNDATION
//c_movie_mf*			movie_get_mf(		c_tex_video* CONST tex_video, INT32 CONST bind )		{	return	store_mf_movie.get( tex_video, bind );	}

void	movie_release( c_tex_video* tex_video, c_movie_player* movie_player )
{
	//todoqqq urgent eventually erase
	switch( movie_player->get_type() )
	{
	case aaa::MOVIE_LIB::MS_DS:			store_ds.release(		tex_video, movie_player );	break;
	case aaa::MOVIE_LIB::MS_AVI:		store_avi.release(		tex_video, movie_player );	break;

#if AAA_USE_QUICKTIME()
	case aaa::MOVIE_LIB::QT:			store_qt.release(		tex_video, movie_player );	break;
#endif //#if AAA_USE_QUICKTIME()

#if AAA_USE_FFMPEG()
	case aaa::MOVIE_LIB::FFMPEG:		store_ffmpeg.release(	tex_video, movie_player );	break;
#endif	//#if AAA_USE_FFMPEG()

	case aaa::MOVIE_LIB::IMGSEQ:		store_img_seq.release(	tex_video, movie_player );	break;
//	case c_movie_player::MOVIE_LIB_MF:	store_mf_movie.release( tex_video, movie_player );	break;
	default:
		ERR_PRINT_STRING( "Unknown video type %d in %s", (INT32)movie_player->get_type(), __FUNCTION__ );		// todo need a string output
		break;
	}
}

void	tex_video_release(  c_tex_video* pt  )
{
	store_ds.release_by_tex_video( pt );
	store_avi.release_by_tex_video( pt );

#if AAA_USE_QUICKTIME()
	store_qt.release_by_tex_video( pt );
#endif //#if AAA_USE_QUICKTIME()

#if AAA_USE_FFMPEG()
	store_ffmpeg.release_by_tex_video( pt );
#endif	//#if AAA_USE_FFMPEG()
	store_img_seq.release_by_tex_video( pt );
//	store_mf_movie.release_by_tex_video( pt );
}
/*
void	video_open_img_seq( c_img_seq* pt )
{
	if( pt && !pt->is_valid() )
		pt->open_video( video_bind_get_name( pt->get_tex_video()->get_video_bind() ) );
}
*/
