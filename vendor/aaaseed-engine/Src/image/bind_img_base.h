
#ifdef AAA_BIND_IMG_BASE_H
#error "BIND_IMG_BASE_H included more than once."
#endif
#define AAA_BIND_IMG_BASE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_THREAD_H
#	include "Thread/aaa_thread.h"
#endif
#ifndef AAA_BIND_H
#	include "infrastructure/bind/bind.h"
#endif
#ifndef	AAA_LAYER_H
#	include "infrastructure/layer/layer.h"
#endif
#ifndef AAA_AAA_DIR_H
#	include "file/aaa_dir.h"
#endif
#ifndef	AAA_GOL_TEX_H
#	include "gol/gol_tex.h"
#endif
#ifndef AAA_IMG_BASE_H
#	include "image/img_base.h"
#endif


class c_pref;

template< class T >
class c_bind_img : public c_obj
{
	friend c_pref;
protected:
	mutable aaa::MUTEX	_lock;
	std::vector<T*>		_vec_pts;

	c_bind*				_bind;

	INT32				_bank_nb;
	INT32				_bank_size;
	INT32				_bind_max_nb;
	std::string			_name;
	std::string			_ext;

	INT32				_image_refresh_cur;
	//REAL				image_refresh_time;
	bool				_b_image_refresh;
	INT32				_image_refresh_start;
	INT32				_image_refresh_stop;
	REAL				_image_refresh_freq;
	//todoqqq have an object by bank then split static and bank data

			void	alloc();
			void	dealloc();

			T**		get_cur_hd();
			T**		get_ui_hd();
			T**		get_hd( INT32 CONST index );

protected:
	FINLINE void			lock()		{	_lock.lock();		}
	FINLINE void			unlock()	{	_lock.unlock();		}

public:;
	c_bind_img( C_PCHAR_C name, C_PCHAR_C ext, INT32 CONST bank_nb, INT32 CONST bank_size );
	~c_bind_img();

			bool			is_index_valid(	 INT32 CONST index ) CONST ;
			INT32			get_index_valid( INT32 index ) CONST;

			UINT32			get_bank_nb()		CONST	{	return _bank_nb;		}
			UINT32			get_bank_size()		CONST	{	return _bank_size;		}
			UINT32			get_bind_max_nb()	CONST	{	return _bind_max_nb;	}

	FINLINE void			split_index(	INT32 CONST index, INT32& bank, INT32& bind ) CONST
							{
								bank = index / _bank_size;
								bind = index - bank * _bank_size;
							}

			c_bind*			get_bind()	CONST			{	return this ? _bind : nullptr;	}
	virtual	INT32			get_cur_index() CONST 	= 0;
	virtual	INT32			get_ui_index()	CONST	= 0;
			INT32			build_index_from_param(		INT32 CONST index	) CONST;

			void			set(						INT32 CONST index,	T* img	);

			T*				get(						INT32 CONST index	) CONST;
			T*				get_cur() CONST;
			T*				get_ui() CONST;
			T*				get_ready(					INT32 CONST index	);	//	-1 will return cur
			T*				get_always(					INT32 index		);	

			void			destroy(					INT32 index );
			void			swap(						INT32 dst, INT32 src );

	virtual	void			init()		{};
	virtual	void			deinit();

			T*				get_image_data(				INT32 CONST index );
/*
	//hack a grab/release bind/unbind lock/unlock
	static	void			do_after_a_compute(			bool b_force_nearest = false );
*/
			bool			set_cpu_keep(				INT32 CONST index,	bool CONST b_keep	);

			void			load_data_and_move_to_gpu(	INT32 CONST index );
//	static	void			move_to_gpu(				INT32 start,	INT32 nb						);
			AAA_ERR			load_texture(				INT32 CONST index,	o_str CONST & filename, bool CONST b_async,
																									bool CONST b_free = false,
																									bool CONST b_force_keep = false,
																									bool CONST b_premultiply = false );
//	static	AAA_ERR			save_texture_compressed(	INT32 index,	C_PCHAR_C filename		);
//	static	AAA_ERR			save(						INT32 index,	C_PCHAR_C filename,	c_img_2d::FILE_TYPE type,	bool b_verbose );

//			void			load_ui_texture(							o_str CONST & filename	);

			o_str CONST &	get_o_filename_for_index(	INT32 CONST index	);
			C_PCHAR_C		get_filename_for_index(		INT32 CONST index	);
			void			set_o_filename_for_index(	INT32 CONST index, o_str CONST & o );
			void			clear_filename_for_index(	INT32 CONST index	);
			INT32			get_index_from_filename(	C_PCHAR_C filename	);

			T*				refresh(					INT32 CONST index	);
			void			refresh();
			void			refresh_ui();

			AAA_ERR			bank_save(					o_str CONST & filename_in, INT32 CONST start, INT32 CONST nb );
			void			bank_move_to_gpu_all();	//todoqqq add a start stop in the interface and deal by bank

//ASYNC READER
	struct reader_info
	{
		INT32				_thread_nb			{3};
		INT32				_sleep_time			{1};
		INT32				_waiting_nb			{0};
		INT32				_reading_nb			{0};
		std::atomic<INT32>	_request_reading_nb;
	};

	struct st_load_request
	{
		INT32	_index;	
		bool	_b_free;
		bool	_b_force_keep;
		bool	_b_premultiply;
		o_str	_filename;
/*
		st_load_request()
		{}

		st_load_request( 	INT32	index, o_str CONST &	filename, bool b_free )
			:	_index(index)
			,	_filename(filename)
			,	_b_free(b_free)
		{}
*/
	};

	struct reader_info		_st_reader_info;

	template< class T >
	class c_thread_bind_img_load final : public c_thread
	{
	public:
		//virtual void	run()		{	run_it< c_bind_tex2d, 0 > ();	}
		virtual void	run()
		{
			T*	l = (T*) get_arg();
			if( l )
			{
				set_can_run( true );
				while( is_can_run() )
				{
					l->update_async();
					sleep( l->_st_reader_info._sleep_time );
				}
			}
		}

		c_thread_bind_img_load() : c_thread( "bind_img_load_loop" )	{}
	};

	std::deque<struct st_load_request>						_load_requests;
	mutable aaa::MUTEX										_load_requests_lock;
	std::vector< c_thread_bind_img_load< c_bind_img<T> >* > _v_threads_load;

	AAA_ERR	ask_texture_async( INT32 CONST index, C_PCHAR_C filename, bool CONST b_free, bool CONST b_force_keep, bool CONST b_premultiply = false )
	{
		//hack we should make sure texture is loaded
		//_bind->set_item( index, (CHAR*)( *hd )->get_filename() );
		//_bind->set_item( index, filename );

		// now add
		//todoopt reuse request to avoid alloc
		st_load_request request;
		request._index			= index;
		request._filename.set(	filename );
		request._b_free			= b_free;
		request._b_force_keep	= b_force_keep;
		request._b_premultiply	= b_premultiply;

		{
			std::lock_guard<aaa::MUTEX> guard( _load_requests_lock );
			if( _v_threads_load.empty() )
				init_thread();
			_load_requests.push_back( request );
			_st_reader_info._waiting_nb = INT32(_load_requests.size());
		}
		return AAA_OK;
	}

	void	update_async()
	{
		_load_requests_lock.lock();
		if( _load_requests.empty() )
		{
			_st_reader_info._waiting_nb = 0;
			_load_requests_lock.unlock();
		}
		else
		{
			//move avoid copy
			//st_load_request req = std::move(_load_requests.front());
			st_load_request req = _load_requests.front();
			_load_requests.pop_front();
			_st_reader_info._waiting_nb = INT32(_load_requests.size());
			_load_requests_lock.unlock();
			//GOOD_PRINT_STRING( "Async Loading %s", first.filename.get() );

			_st_reader_info._reading_nb = ++(_st_reader_info._request_reading_nb);
			load_texture( req._index, req._filename, false, req._b_free, req._b_force_keep, req._b_premultiply );
			//delete req._filename;
			_st_reader_info._reading_nb = --(_st_reader_info._request_reading_nb);
		}
	}

	void	init_thread()
	{
		if( !_v_threads_load.empty() )
			return;
		INT32 nb = _st_reader_info._thread_nb;
		for( INT32 i=0; i < nb; ++i )
			_v_threads_load.push_back( new c_thread_bind_img_load< c_bind_img<T> > );
		//_v_threads_load.resize( nb );
		for( auto const & elt : _v_threads_load )
		{
			elt->create( this, 0 );
			elt->start();
		}
	}

	void	close_thread()
	{
		for( auto const & elt : _v_threads_load ) 
		{
			elt->shutdown();
			elt->join();
			delete elt;
		}
		_v_threads_load.clear();
		//close();
	}
};

//called only by constructor
//todo called only once for now need to be more generic
template<class T>
void c_bind_img<T>::alloc()
{
	if( _bind )
		return;

	lock();
		_bind_max_nb = _bank_nb * _bank_size;
		_bind = c_bind::get_new( _ext.c_str() );
		_bind->set( _bind_max_nb, _name.c_str(), _ext.c_str(), T::get_dim()==2 ? aaa::file::TYPE_IO_TEXTURE_2D : aaa::file::TYPE_IO_TEXTURE_3D, 
					_bank_nb, true, false, true );

		_vec_pts.resize( _bind_max_nb );	// each element will be nullptr no need to init

		//tex::bind_init( bind_2d_max_nb );
		//tex::alloc( bind_max_nb );
	unlock();
}

//call only by destructor
template<class T>
void			c_bind_img<T>::dealloc()
{
	if( !_bind )
		return;

	lock();
		for( auto const & img : _vec_pts )
			delete img;
		_vec_pts.clear();
	unlock();
		//tex::dealloc();
	SAFE_DELETE( _bind );	//todo could be refined
}

template< class T >
void c_bind_img<T>::deinit()
{
	close_thread();
	dealloc();
}

template< class T >
c_bind_img<T>::c_bind_img( C_PCHAR_C name, C_PCHAR_C ext, INT32 CONST bank_nb, INT32 CONST bank_size )
	:_bank_nb					(	bank_nb		)
	,_bank_size					(	bank_size	)
	,_bind_max_nb				(	0			)
	,_name						(	name		)
	,_ext						(	ext			)
	//,image_refresh_time =			{0}
	,_image_refresh_cur			(	0			)
	,_bind						{	nullptr		}
{
	_st_reader_info._request_reading_nb = 0;
	alloc();
}

template< class T >
c_bind_img<T>::~c_bind_img<T>()
{
	deinit();
}

//	should extend to other things that file
template<class T>
FINLINE	T*		c_bind_img<T>::get_image_data( INT32 CONST index )
{
	T*	pt = get( index );
	//hack ? pt->is_empty() && !pt->is_filename() avoid continuous disk access thru trax but cancel automatic refresh
	//hack ? (pt->is_cpu_keep() && pt->get_data_size_used()==0) avoid reloading all the time
	if( pt==nullptr || (pt->is_empty() && !pt->is_filename()) || (pt->is_cpu_keep() && pt->get_data_size_used()==0) )
	{
		T**	hd = get_hd( index );
		if( hd )
		{
			o_str CONST & filename = _bind->get_o_str( index );
			if( filename.is_empty() )
			{
#if	AAA_DEBUG()
//				DBG_PRINT_STRING( "BIND_IMG %d : Can't open image with no filename ", index );
#endif
				if( *hd )
					(*hd)->set_empty();
			}
			else
			{
				//todo this can be pretty heavy because we check the file modification date every time 
				c_dir::change_to_def();
				*hd = c_img_base::read<T>( pt, filename, true, false, pt ? pt->is_cpu_keep() : false );
			}
			//img_alpha_force( *img_tex, 255 );
			//todo	install other menu for mask texture for ex
			return *hd;
		}
		return nullptr;
	}
	return pt;
}

template<class T>
bool	c_bind_img<T>::is_index_valid( INT32 index ) CONST
{
	if( index >= _bind_max_nb )
	{
		debug_break( "in C++ %s( %d )index too high, maximum is %d", __FUNCTION__, index, _bind_max_nb - 1 );
		return false;
	}
	else if( index < -1 )
	{
		debug_break( "in C++ %s( %d ) index too low, start at 0, and -1 for cur ", __FUNCTION__, index );
		return false;
	}
	return true;
}
template<class T>
INT32	c_bind_img<T>::get_index_valid( INT32 index ) CONST
{
	if( index == -1 )
		index = get_cur_index();
	if( index >= _bind_max_nb )
	{
		debug_break( "in C++ %s( %d )index too high, maximum is %d", __FUNCTION__, index, _bind_max_nb - 1 );
		return 0;
	}
	else if( index < 0 )
	{
		debug_break( "in C++ %s( %d ) index too low, start at 0", __FUNCTION__, index );
		return 0;
	}
	return index;
}

template<class T>
FINLINE	T*		c_bind_img<T>::get( INT32 index ) CONST
{
	if( index == -1 )
		return get_cur();
	else if( is_index_valid( index ) )
		return _vec_pts[index];	//todo is_index_valid() is not optimum here because test -1 again
	else
		return nullptr;
}
template<class T>
FINLINE	T**		c_bind_img<T>::get_hd( INT32 CONST index )
{
	if( is_index_valid( index ) )
		return	&_vec_pts[ get_index_valid( index ) ];
	return nullptr;
}
template<class T>
FINLINE	T*		c_bind_img<T>::get_ready( INT32 CONST index )
{
	return get_image_data( get_index_valid( index ) );
}
template<class T>
T*	c_bind_img<T>::get_always( INT32 index )
{
	if( is_index_valid( index ) )
	{
		index = get_index_valid( index );
		lock();	//remove outside lock/unlock (Maa 2025 Feb)
			T*	pt = _vec_pts[index];
			if( !pt )
			{
				pt = T::create( __FUNCTION__ );
				//lock();
				_vec_pts[index] = pt;
				//unlock();
			}
		unlock();
		return pt;
	}
	else
		return nullptr;
}

//todo we should
// check if the image have a user
// update the texture info side
template<class T>
void	c_bind_img<T>::destroy( INT32 index )
{
	index = get_index_valid( index );
	{
		lock();
			SAFE_DELETE( _vec_pts[index] );
			_bind->clear_item( index );
		unlock();
	}
}
//todo we should
// check if the image have a user
// update the texture info side
template<class T>
void	c_bind_img<T>::swap( INT32 a, INT32 b )
{
	if( is_index_valid(a) && is_index_valid(b) )
	{
		a = get_index_valid( a );
		b = get_index_valid( b );
		{
			lock();
			{
				T* pa = _vec_pts[a];
				T* pb = _vec_pts[b];

				if( pa )
					pa->set_changed();		
				if( pb )
					pb->set_changed();

				_vec_pts[a] = pb;
				_vec_pts[b] = pa;

				_bind->swap_item( a, b );

//				refresh(a);
//				refresh(b);
			}
			unlock();
		}
	}
}
template<class T>
void	c_bind_img<T>::set( INT32 CONST index, T* img )
{
	if( !img || !is_index_valid( index ) )
		return;

	lock();
		if( _vec_pts[index] )
		{	// Free existing img before assigning a new one
			SAFE_DELETE( _vec_pts[index] );
		}
		_vec_pts[index] = img;
	unlock();
}
template<class T>	FINLINE T*	c_bind_img<T>::get_cur() CONST	{ return	get(	get_cur_index() );	}
template<class T>	FINLINE T**	c_bind_img<T>::get_cur_hd()		{ return	get_hd( get_cur_index() );	}
template<class T>	FINLINE	T*	c_bind_img<T>::get_ui()	CONST	{ return	get(	get_ui_index() );	}
template<class T>	FINLINE T**	c_bind_img<T>::get_ui_hd()		{ return	get_hd( get_ui_index() );	}

template<class T>	FINLINE o_str CONST &	c_bind_img<T>::get_o_filename_for_index( INT32 index )
{
	return _bind->get_o_str( index );
}

template<class T>	FINLINE	C_PCHAR_C		c_bind_img<T>::get_filename_for_index( INT32 index )
{
	return _bind->get_str( index );
}

template<class T>	FINLINE	void			c_bind_img<T>::set_o_filename_for_index( INT32 CONST index, o_str CONST & o )
{
	return _bind->set_item( index, o );
}
template<class T>	FINLINE	void			c_bind_img<T>::clear_filename_for_index( INT32 index )
{
	_bind->clear_item( index );
}
template<class T>	INT32					c_bind_img<T>::get_index_from_filename( C_PCHAR_C filename )
{
	for( INT32 i = 0; i < _bind_max_nb; ++i )
	{
		if( _bind->get_o_str(i).is_str_equal( filename ) )
			return i;
	}
	return -1;
}

template<class T>
INT32	c_bind_img<T>::build_index_from_param( INT32 index ) CONST
{
	INT32	bind;
	if( index < 0 )
	{
		if( c_layer::get_cur() )
			bind = get_cur_index();
		else
		{
			ERR_PRINT_STRING( "%s() no layer cur use bind_dst 0", __FUNCTION__ );
			bind = 0;
		}
	}
	else
	{
		if( index <= _bind_max_nb )
			bind = index;
		else
		{
			ERR_PRINT_STRING( "%s() bind is too high %d, max is %d", __FUNCTION__, index, _bind_max_nb );
			bind = 0;
		}
	}
	return bind;
}

template<class T>
bool	c_bind_img<T>::set_cpu_keep( INT32 CONST index, bool CONST b_keep )
{
	T*	img = get_always( index );
	if( img )
	{
		img->set_cpu_keep( b_keep );
		return true;
	}
	return false;
}

template<class T>
FINLINE	void	c_bind_img<T>::load_data_and_move_to_gpu( INT32 CONST index )
{
	//	load texture if there is nothing yet
	T*	img = get_image_data( index );
	if( img )
	{
		//if( index == 0 )
		//	DBG_PRINT_STRING( "avant" );
		img->move_to_gpu( __FUNCTION__, index );
		//if( index == 0 )
		//	DBG_PRINT_STRING( "apres" );
	}
}

template<class T>
AAA_ERR c_bind_img<T>::load_texture( INT32 index, o_str CONST & filename, bool CONST b_async, bool CONST b_free, bool CONST b_force_keep, bool CONST b_premultiply )
{
	AAA_ERR retcode = ERR_ANY;
	if( filename.is_empty() )
	{
		retcode = ERR_STR_EMPTY;
		ERR_PRINT_STRING( "%s() filename is empty. Can't load a texture.", __FUNCTION__ );
	}
	else
	{
		try
		{
			if( b_async )
			{
				T* img = get_always( index );
				if( img )	
				{
					img->lock();
					img->set_reading( true );
					img->unlock();
				}
				//avoid fucking up the menu and faster
				_bind->set_item( index, filename );
				//hack or not hack this make sure we move_to_gpu the texture
				GOL::get_tex_unit_cur()->set_bind_2d(-2);
				//GOL::invalidate_tex_unit();
				retcode = ask_texture_async( index, filename.get(), b_free, b_force_keep, b_premultiply );
				//hack we should make sure texture is loaded
			}
			else
			{
				T** hd = get_hd( index );
				if( hd )
				{
					*hd = T::read( *hd, filename, true, b_free, b_force_keep, b_premultiply );
					if( *hd )
					{
						//avoid fucking up the menu and faster
						_bind->set_item( index, (*hd)->get_filename_o_str() );
						//hack or not hack this make sure we move_to_gpu the texture
						GOL::get_tex_unit_cur()->set_bind_2d(-2);
						//GOL::invalidate_tex_unit();
						retcode = AAA_OK;
					}			
				}
				else
					retcode = ERR_OUT_OF_BOUND;
			}

		}
		catch( const std::exception& e )
		{
			ERR_PRINT_STRING( "%s() Caught exception: %s", __FUNCTION__, e.what() );
		}
	}
	return retcode;
}



//template<class T>
//void	c_bind_img<T>::load_ui_texture( o_str CONST & filename )
//{
//	load_texture( get_ui_index(), filename, false );
//}

template<class T>
void	c_bind_img<T>::refresh_ui()
{
	T** hd = c_bind_img<T>::get_ui_hd();
	if( hd )
	{
		c_dir::change_to_def();
		*hd = T::reread( *hd );
		//todo	move this part to seed ?
		if( (*hd)->is_ok() )
			c_map::get_ui()->set_tex_use_ui( 2 );
	}
	else
	{
		BOX_ERR( "in C++ %s() Bug for Maa : hd_img == NULL.", __FUNCTION__ );
	}
}

template<class T>
T*	c_bind_img<T>::refresh( INT32 CONST index )
{
	T*	pt_img = get( index );
	if( !pt_img )
		return get_image_data( index );
	else
		return T::reread( pt_img );
}

template<class T>
void	c_bind_img<T>::refresh()
{
	if( _b_image_refresh && _image_refresh_freq != 0. )
	{
		INT32 nb = I_FLOOR(_image_refresh_freq);
		//	image_refresh_time += 1./image_refresh_freq;
		while( nb-->0 )
		//	while( image_refresh_time > 1. )
		{
			INT32	mod = _image_refresh_stop - _image_refresh_start + 1;
			if( mod > 0 )
			{
				_image_refresh_cur = IMOD( _image_refresh_cur + 1, mod );
				refresh( _image_refresh_cur + _image_refresh_start );
				//	image_refresh_time -= 1;
			}
		}
	}
}

template<class T>
AAA_ERR	c_bind_img<T>::bank_save( o_str CONST & filename_in, INT32 start, INT32 nb )
{
	FILE*	file = c_file::FOPEN( filename_in, "wt" );
	if( IS_NULL( file ) )
	{
		ERR_PRINT_STRING( "%s() file list %s can't open", __FUNCTION__, filename_in.get() );
		return ERR_ANY;
	}
	else
	{
//		if( b_verbose_param )
//			VERBOSE_PRINTF( "Parsing %s : ", filename );
		for( INT32 i = start; i < start + nb; ++i )
			fprintf( file, "%s\n", get_bind()->get_str(i) );
		c_file::FCLOSE( file );
		return AAA_OK;
	}
}

template<class T>
void	c_bind_img<T>::bank_move_to_gpu_all()	//todoqqq add a start stop in the interface and deal by bank
{
	DBG_PRINT_STRING( "bank_move_to_gpu_all() dim %d Started",	T::get_dim() );
		move_to_gpu( 0, get_bind_max_nb(), true );	// true : 2021 to keep compatible with old code
	DBG_PRINT_STRING( "bank_move_to_gpu_all() dim %d Done",		T::get_dim() );
}
