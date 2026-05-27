#include "face_tracker.h"
#include "time/speed.h"
#include "spy.h"
#include "math/v.h"


C_PCHAR_C	c_face_tracker::type_str[ static_cast<INT32>( TYPE_NB ) ] =
{
	"Dlib",
	"Clm"
};

c_face_tracker::c_face_info::c_face_info()
{
	clear_v3( _tra );
	clear_v3( _rot );
	for( INT32 i=0; i<POINT_NB_MAX; ++i )
	{
		for( INT32 j=0; j<=AVERAGE_NB_MAX; ++j )
			clear_v2( _point_raw[j][i] );
		clear_v2( _point_cano[i] );
	}
}

AAA_ERR c_face_tracker::c_face_info::set_point_nb( INT32 nb )
{
	if( nb > POINT_NB_MAX )	//todo alloc really
	{
		ERR_PRINT_STRING( "%s() with %d when maximum is %d.", __FUNCTION__, nb, POINT_NB_MAX );
		return ERR_MEM_BASE;
	}
	_point_nb = nb;
	return AAA_OK;
}

extern REAL max_travel;

void c_face_tracker::c_face_info::update_from_raw( DOUBLE filter, INT32 average_nb, REAL* size, bool b_invent_3d )
{
	INT32 nb = _point_nb;
	average_nb = MIN( average_nb, AVERAGE_NB_MAX );
	DOUBLE over = DOUBLE(1.)/average_nb;
	for( int i = 0; i < nb; ++i )
	{
		DOUBLE d2[2];
		set_v2( d2, 0, 0 );
		for( INT32 j=average_nb; j>0; --j )
		{
			DOUBLE* prev = _point_raw[j-1][i];
			add_v2( d2, prev );
			cpy_v2( _point_raw[j][i], prev );
		}
		DOUBLE* point_raw = _point_raw[0][i];
		scale_v2( point_raw, d2, over );

		DOUBLE* point_cano	= _point_cano[i];
/*
		DOUBLE d1 = point_raw[0] - point_cano[0];
		DOUBLE d2 = point_raw[1] - point_cano[1];
		d1 = d1*d1 + d2*d2;
		if( d1 < max_travel*max_travel )
			interpolate_v2( point_cano, point_raw, point_cano, .95 );
		else
*/
		interpolate_v2( point_cano, point_raw, point_cano, filter );

		DOUBLE* point = _point[i];
		set_v3( point,	(point_cano[0] - .5) * size[0],
						(point_cano[1] - .5) * size[1],
						0 ); 

		if( b_invent_3d )
			set_v3( _point_3d[i], point[0], point[1], 0 );
	}

//todonow
//	model.GetShape(  info._point_3d[0], size_x, size_y, -.5, -.5 );
}

c_face_tracker::c_face_tracker()
{
	_speed_detect	= new c_speed( false, 5, g_speed_master->get_pb_print_bdd(),	"detect"			);	//, this );
	_speed_landmark	= new c_speed( false, 5, g_speed_master->get_pb_print_bdd(),	"landmark"			);	//, this );

	for( INT32 i = 0; i < FACE_NB_MAX; ++i )
		_face_infos.push_back( c_face_info{} );

	for( INT32 i = 0; i < TYPE_NB; ++i )
		_engines[i] = nullptr;
}

c_face_tracker::~c_face_tracker()
{
	SAFE_DELETE( _speed_landmark );
	SAFE_DELETE( _speed_detect );

	_face_infos.clear();
	for( INT32 i = 0; i < TYPE_NB; ++i )
		SAFE_DELETE(  _engines[i] );
}

c_face_engine* c_face_tracker::get_engine( INT32 s_type )	//todo rename
{
	c_face_engine* eng = _engines[ s_type ];
	if( !eng )
	{
		switch( s_type )
		{
		case TYPE_CLM:	eng = get_clm();	break;
		case TYPE_DLIB:	eng = get_dlib( _dlib_landmark_detection_filename.get() );	break;
		}
		eng->set_tracker( this );
		_engines[ s_type ] = eng;
	}
	return eng;
}

void c_face_tracker::update_engine( int argc, char** argv )
{
	c_face_engine* eng = get_engine( _s_type_ui );

	if( _model_max_ui != eng->get_model_nb() )
	{
		eng->deinit();
		eng->init( _model_max_ui, argc, argv );
		_active_models.clear();
		for( INT32 i = 0; i < eng->get_model_nb(); ++i )
			_active_models.push_back( false	);
	}
	_engine = eng;
}

void c_face_tracker::reset()
{
	UINT32 nb = (UINT32)_active_models.size();
	for( UINT32 i = 0; i < nb; ++i )
		_active_models[i] = false;
	_engine->reset();	
}

void	c_face_tracker::begin_detect()
{
	_speed_detect->begin();
	SPY_PUSH_RANGE( "DetectFaces", spy::COL_1 );
}
void	c_face_tracker::end_detect()
{
	SPY_POP_RANGE();
	_speed_detect->end();
}

void	c_face_tracker::update( cv::UMat* detect, cv::UMat* landmark, cv::Mat_< float >& img_depth )
{
	if( detect )
	{
		set_size_detect( detect->cols, detect->rows );
		begin_detect();
		_engine->detect( detect, img_depth );
		end_detect();
	}

	_speed_landmark->begin();
	SPY_PUSH_RANGE( "landmark", spy::COL_2 );
		_engine->find_landmark( landmark, img_depth );
	SPY_POP_RANGE();
	_speed_landmark->end();
}

void c_face_tracker::set_size_detect( INT32 sx, INT32 sy )
{
	if( _detect_sx != sx || _detect_sy != sy )
	{
		_detect_sx = sx;
		_detect_sy = sy;
		reset();	//todo do we need this
	}
}

void c_face_tracker::store_landmark( REAL sx, REAL sy )
{
	UINT32 nb_active = (UINT32)_active_models.size();
	bool b_invent_3d = !_engine->is_point_3d_produced();
	for( UINT32 active_model = 0; active_model < nb_active; ++active_model )
	{
		if( _active_models[active_model] )
		{
			auto& info = _face_infos[active_model];
			_engine->store_landmark( info, active_model, sx, sy );
		}
	}
}

void c_face_tracker::process_landmark( REAL* size )
{
	UINT32 nb_active = (UINT32)_active_models.size();
	bool b_invent_3d = !_engine->is_point_3d_produced();
	for( UINT32 active_model = 0; active_model < nb_active; ++active_model )
	{
		if( _active_models[active_model] )
		{
			auto& info = _face_infos[active_model];
			info.update_from_raw( _filter_ui, _average_nb, size, b_invent_3d );
		}
	}
}

INT32 c_face_tracker::get_model_active_nb()
{
	INT32 nb = 0;
	UINT32 nb_active = (UINT32)_active_models.size();
	for( UINT32 active_model = 0; active_model < nb_active; ++active_model )
	{
		if( _active_models[active_model] )
			++nb;
	}
	return nb;
}

bool c_face_tracker::is_model_active_all()
{
	UINT32 nb_active = (UINT32)_active_models.size();
	for( UINT32 model = 0; model < nb_active; ++model )
	{
		if( !_active_models[model] )
			return false;
	}
	return true;
}

void c_face_tracker::get_cube_pos_rot( INT32 i, REAL* pos, REAL* rot )
{
	c_face_info& fi = _face_infos[i];
	cpy_v3( pos, fi._tra );
	cpy_v3( rot, fi._rot );
}

void c_face_engine::set_tracker( c_face_tracker* ft )
{
	_face_tracker = ft;
}

