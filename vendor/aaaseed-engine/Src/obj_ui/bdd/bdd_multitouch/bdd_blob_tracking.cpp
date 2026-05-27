#include "bdd_blob_tracking.h"
//#include "obj_ui/bdd/bdd_point/bdd_blob.h"

#include "obj_ui/bdd/bdd_ui_pref.h"
#include "gol/gol.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "draw/color.h"
#include "draw/model.h"
#include "draw/line.h"
#include "bdd_img_contour.h"
#include "obj_ui/com/osc/OscPrintReceivedElements.h"
#include "obj_ui/com/osc/OscOutboundPacketStream.h"

#include "infrastructure/flux_filter.h"
#include "math/rand.h"

//	MOUSE
#include "ui/event/event_mouse.h"
#include "draw/aaa_glut.h"

#include "platform\win32\touch_windows.h"


FACTORY_CREATE_PROP_V1( c_bdd_blob_tracking, bdd_blob_tracking, Blob Tracking, bdd_blob_tracking, sub_menu="Multitouch"; );

c_bdd_blob_tracking*	c_bdd_blob_tracking::cur = nullptr;
C_PCHAR					_osc_finger_header		= "/aaa/finger";
C_PCHAR					_net_str_finger_header	= "/aaa/multitouch";
CONST	INT32	MOUSE_ID_TRACKING = -1;

enum TRACKING_DRAW_TYPE : INT32
{
	DRAW_ALL = 0,
	DRAW_FINGER,
	DRAW_LAST,
	DRAW_NB
};

static	C_PCHAR_C	str_draw[DRAW_NB] =
{
	//	"BLUR_NO_SCALE",
	"TRACKING_ALL",
	"TRACKING",
	"LAST",
};

static INT32 CONST	EVENT_NB_UI = 32;
namespace n_bdd_blob_tracking
{

	CONSTEXPR INT32 BASE_PARAM_NB		= 12 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 NET_IN_PARAM_NB		= 15;
	CONSTEXPR INT32 NET_OUT_PARAM_NB	= 10;
	CONSTEXPR INT32 MATRIX_PARAM_NB		= 12;
	CONSTEXPR INT32 EVENT_PARAM_NB		= 1 + EVENT_NB_UI;
	CONSTEXPR INT32 BLOB_PARAM_NB		= 14;
	CONSTEXPR INT32 DRAW_PARAM_NB		= 10;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 6 + c_bdd_blob_tracking::BLOB_NB_UI;

	CONSTEXPR INT32	PARAM_NB	=	BASE_PARAM_NB
								+	NET_IN_PARAM_NB
								+	NET_OUT_PARAM_NB
								+	MATRIX_PARAM_NB
								+	DRAW_PARAM_NB
								+	2 * EVENT_PARAM_NB
								+	c_bdd_blob_tracking::BLOB_NB_UI * BLOB_PARAM_NB
											+	GROUP_PARAM_NB;

#define	PARAM_DEF_BLOB( nb )\
	PARAM_DEF_GROUP_CLOSED( blob_##nb, BLOB_PARAM_NB )\
		PARAM_DEF_INT32_LOCKED(	blob_##nb##_id_tracking )\
		PARAM_DEF_INT32_LOCKED(	blob_##nb##_id_tag )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_x )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_y )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_dx )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_dy )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_x_filtered )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_y_filtered )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_size_x )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_size_y )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_angle )\
		PARAM_DEF_REAL_LOCKED(	blob_##nb##_area )\
		PARAM_DEF_INT32_LOCKED(	blob_##nb##_point_nb )\
		PARAM_DEF_BOOL_LOCKED(	blob_##nb##_ghost )

#define	PARAM_DEF_EVENT( name )\
	PARAM_DEF_GROUP_CLOSED( name##_event, EVENT_PARAM_NB )\
		PARAM_DEF_INT32_LOCKED( name##_event_nb )\
		PARAM_DEF_INT32_LOCKED( name##_id_01 )\
		PARAM_DEF_INT32_LOCKED( name##_id_02 )\
		PARAM_DEF_INT32_LOCKED( name##_id_03 )\
		PARAM_DEF_INT32_LOCKED( name##_id_04 )\
		PARAM_DEF_INT32_LOCKED( name##_id_05 )\
		PARAM_DEF_INT32_LOCKED( name##_id_06 )\
		PARAM_DEF_INT32_LOCKED( name##_id_07 )\
		PARAM_DEF_INT32_LOCKED( name##_id_08 )\
		PARAM_DEF_INT32_LOCKED( name##_id_09 )\
		PARAM_DEF_INT32_LOCKED( name##_id_10 )\
		PARAM_DEF_INT32_LOCKED( name##_id_11 )\
		PARAM_DEF_INT32_LOCKED( name##_id_12 )\
		PARAM_DEF_INT32_LOCKED( name##_id_13 )\
		PARAM_DEF_INT32_LOCKED( name##_id_14 )\
		PARAM_DEF_INT32_LOCKED( name##_id_15 )\
		PARAM_DEF_INT32_LOCKED( name##_id_16 )\
		PARAM_DEF_INT32_LOCKED( name##_id_17 )\
		PARAM_DEF_INT32_LOCKED( name##_id_18 )\
		PARAM_DEF_INT32_LOCKED( name##_id_19 )\
		PARAM_DEF_INT32_LOCKED( name##_id_20 )\
		PARAM_DEF_INT32_LOCKED( name##_id_21 )\
		PARAM_DEF_INT32_LOCKED( name##_id_22 )\
		PARAM_DEF_INT32_LOCKED( name##_id_23 )\
		PARAM_DEF_INT32_LOCKED( name##_id_24 )\
		PARAM_DEF_INT32_LOCKED( name##_id_25 )\
		PARAM_DEF_INT32_LOCKED( name##_id_26 )\
		PARAM_DEF_INT32_LOCKED( name##_id_27 )\
		PARAM_DEF_INT32_LOCKED( name##_id_28 )\
		PARAM_DEF_INT32_LOCKED( name##_id_29 )\
		PARAM_DEF_INT32_LOCKED( name##_id_30 )\
		PARAM_DEF_INT32_LOCKED( name##_id_31 )\
		PARAM_DEF_INT32_LOCKED( name##_id_32 )

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF( active					)
		PARAM_DEF_BOOL_OFF( restart_trig			)
		PARAM_DEF_BOOL_OFF( verbose					)
		PARAM_DEF_BOOL_OFF( verbose_matrix			)
		PARAM_DEF_BOOL_OFF( use_mouse				)
		PARAM_DEF_BOOL_OFF( use_mouse_when_no_touch	)

		PARAM_DEF_GROUP_CLOSED( In, NET_IN_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		net_in						)
			PARAM_DEF_BOOL_OFF(		tuio_in						)
			PARAM_DEF_BOOL_OFF(		tuio_in_skip_alive			)
			PARAM_DEF_INT32_POS(	tuio_silent_frame_max_nb,	60,32 )
			PARAM_DEF_BOOL_OFF(		tuio_in_flip_x				)
			PARAM_DEF_BOOL_OFF(		tuio_in_flip_y				)
			PARAM_DEF_SCALE_XY(		tuio_in_scale				)
			PARAM_DEF_POINT_XY(		tuio_in_offset				)
			PARAM_DEF_SCALE_UV(		tuio_in_blob_size			)
			PARAM_DEF_REAL_POS(		tuio_in_blob_size_factor,	1./64., .01 )
			//PARAM_DEF_BOOL_OFF( tuio_simulator )
			PARAM_DEF_INT32_LOCKED( frame_in )
			PARAM_DEF_INT32(		net_verbose_in,		1,0,	0,2 )

		PARAM_DEF_GROUP_CLOSED( Out, NET_OUT_PARAM_NB )
//			PARAM_DEF_BOOL_OFF(		tuio_in_2dobj )
			PARAM_DEF_BOOL_OFF(		net_out				)
			PARAM_DEF_BOOL_OFF(		tuio_out			)
			PARAM_DEF_SCALE_XY(		tuio_out_factor		)
			PARAM_DEF_POINT_XY(		tuio_out_offset		)
			PARAM_DEF_BOOL_OFF(		send_as_string		)
			PARAM_DEF_INT32(		osc_dst,			0,1,	0, osc::DST_NB - 1	)
			PARAM_DEF_REF(			osc_header			)
			PARAM_DEF_INT32_LOCKED(	frame_out			)
//			PARAM_DEF_BOOL_OFF( tuio_out_2dobj )
//			PARAM_DEF_BOOL_OFF( tuio_verbose_out )

		PARAM_DEF_BOOL_ON(		read_bdd_img_contour_as_src	)
		PARAM_DEF_BOOL_OFF(		read_bdd_blob_as_src		)
		PARAM_DEF_INT32(		blob_max_nb,				16,32,	1,PARAM_MAX_INT32 )	
		PARAM_DEF_INT32_LOCKED(	blob_nb						) 
		PARAM_DEF_BOOL_LOCKED(	mouse						)

		PARAM_DEF_GROUP_CLOSED( Matrix, MATRIX_PARAM_NB )
			PARAM_DEF_BOOL_ON(		matrix_use			)
			PARAM_DEF_BOOL_OFF(		use_predicted_pos	)
			PARAM_DEF_BOOL_ON(		use_squared_dist	)
			PARAM_DEF_REAL_ZERO(	min_dist			)
			PARAM_DEF_REAL_ONE(		reject_dist			)
			PARAM_DEF_INT32_POS_ONE(ghost_frame_nb		)
			PARAM_DEF_BOOL_ON(		by_subset			)
			PARAM_DEF_INT32(		branch_try_max,		1, 4,			1,	PARAM_MAX_INT32	)
			PARAM_DEF_INT32(		iteration_max,		100000, 50000,	1,	PARAM_MAX_INT32	)
			PARAM_DEF_REAL(			filter,				1, 0,			0, .999		)
			PARAM_DEF_REAL(			filter_x,			1, 0,			0, .999		)
			PARAM_DEF_REAL(			filter_y,			1, 0,			0, .999		)

		PARAM_DEF_NONE( Bdd_ui_pref )
	
		PARAM_DEF_GROUP_CLOSED( Draw, DRAW_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ONE(	draw_mode,				str_draw	)
			PARAM_DEF_BOOL_ON(			color_auto				)
			PARAM_DEF_INT32(			frames_nb,				16, 2,		2, PARAM_MAX_INT32	)		
			PARAM_DEF_INT32(			frames_nb_to_draw,		16, 1,		1, PARAM_MAX_INT32	)		
			PARAM_DEF_REAL_ZERO(		contour_alpha			)
			PARAM_DEF_BOOL_OFF(			contour_with_quad		)
			PARAM_DEF_REAL_ZERO(		bounding_alpha			)
			PARAM_DEF_REAL_ONE(			bounding_size_factor	)
			PARAM_DEF_BOOL_OFF(			bounding_as_ellipse		)
			PARAM_DEF_REAL_ZERO(		segment_alpha			)

		PARAM_DEF_EVENT( touch )
		PARAM_DEF_EVENT( untouch )

		PARAM_DEF_BLOB( 01 )
		PARAM_DEF_BLOB( 02 )
		PARAM_DEF_BLOB( 03 )
		PARAM_DEF_BLOB( 04 )
		PARAM_DEF_BLOB( 05 )
		PARAM_DEF_BLOB( 06 )
		PARAM_DEF_BLOB( 07 )
		PARAM_DEF_BLOB( 08 )
		PARAM_DEF_BLOB( 09 )
		PARAM_DEF_BLOB( 10 )
		PARAM_DEF_BLOB( 11 )
		PARAM_DEF_BLOB( 12 )
		PARAM_DEF_BLOB( 13 )
		PARAM_DEF_BLOB( 14 )
		PARAM_DEF_BLOB( 15 )
		PARAM_DEF_BLOB( 16 )
		PARAM_DEF_BLOB( 17 )
		PARAM_DEF_BLOB( 18 )
		PARAM_DEF_BLOB( 19 )
		PARAM_DEF_BLOB( 20 )
		PARAM_DEF_BLOB( 21 )
		PARAM_DEF_BLOB( 22 )
		PARAM_DEF_BLOB( 23 )
		PARAM_DEF_BLOB( 24 )
		PARAM_DEF_BLOB( 25 )
		PARAM_DEF_BLOB( 26 )
		PARAM_DEF_BLOB( 27 )
		PARAM_DEF_BLOB( 28 )
		PARAM_DEF_BLOB( 29 )
		PARAM_DEF_BLOB( 30 )
		PARAM_DEF_BLOB( 31 )
		PARAM_DEF_BLOB( 32 )
	};
}

using namespace std;

// FIXME: If there is a lot of background noise this function starts to 
// take up tons of CPU.. 

// FIXME: findFinger should search back a few frames just in case 

//using namespace multitouch;

///////////////////////////////////////////////////
// blob detecting

void	c_bdd_blob_tracking::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_restart_trig_ui );
	param_set_pt( h, _b_verbose_ui );
	param_set_pt( h, _b_verbose_matrix_ui );
	param_set_pt( h, _b_mouse_use_ui );
	param_set_pt( h, _b_mouse_use_when_no_touch_ui );

	++h;
		param_set_pt( h, _b_net_in );
		param_set_pt( h, _b_tuio_in );
		param_set_pt( h, _b_tuio_in_skip_alive );
		param_set_pt( h, _tuio_silent_frame_max_nb );
		param_set_pt( h, _b_tuio_in_flip_x );
		param_set_pt( h, _b_tuio_in_flip_y );
		param_set_pt( h, _tuio_in_scale_x );
		param_set_pt( h, _tuio_in_scale_y );
		param_set_pt( h, _tuio_in_offset_x );
		param_set_pt( h, _tuio_in_offset_y );
		param_set_pt_3( h, _tuio_in_blob_size_ui );
//		param_set_pt( h, _b_tuio_simulator );
		param_set_pt( h, _frame_in );
		param_set_pt( h, _s_net_verbose_in );
	++h;
		param_set_pt( h, _b_net_out );
		param_set_pt( h, _b_tuio_out );
		param_set_pt_2( h, _tuio_out_scale );
		param_set_pt_2( h, _tuio_out_offset );
		param_set_pt( h, _b_net_as_string );
		param_set_pt( h, _osc_dst );
		param_set_pt( h, _osc_header );
		param_set_pt( h, _frame_out );
//		param_set_pt( h, _b_tuio_verbose_out );

	param_set_pt( h, _b_read_img_contour_as_src );
	param_set_pt( h, _b_read_bdd_blob_as_src );
	param_set_pt( h, _blob_max_nb_ui );
	param_set_pt( h, _blob_nb );
	param_set_pt( h, _b_mouse );

	++h;
		param_set_pt( h, _b_matrix_use_ui			);
		param_set_pt( h, _b_use_predicted_pos_ui	);
		param_set_pt( h, _b_use_squared_dist_ui		);
		param_set_pt( h, _min_dist_ui				);
		param_set_pt( h, _reject_dist_ui			);
		param_set_pt( h, _ghost_frame_nb_ui			);
		param_set_pt( h, _matrix._b_by_subset		);	
		param_set_pt( h, _matrix._branch_try_max	);	
		param_set_pt( h, _matrix._loop_count_max	);	
		param_set_pt( h, _filter_factor_ui			);
		param_set_pt( h, _filter_factor_x_ui		);
		param_set_pt( h, _filter_factor_y_ui		);

	param_attach_obj( h, c_bdd_ui_pref::cur );
	++h;
		param_set_pt( h, _s_draw );
		param_set_pt( h, _b_color_auto_ui );
		param_set_pt( h, _frames_nb );
		param_set_pt( h, _frames_nb_to_draw_ui );
		param_set_pt( h, _contour_alpha );
		param_set_pt( h, _b_contour_draw_with_quad );
		param_set_pt( h, _bounding_alpha );
		param_set_pt( h, _bounding_size_factor );
		param_set_pt( h, _b_bounding_draw_as_ellipse_ui );
		param_set_pt( h, _segment_alpha );

	++h;
		param_set_pt( h, _event_touch_count );
		for( INT32 i = 0; i < EVENT_NB_UI; ++i )
			param_set_pt( h, _event_touch_id[i] );

	++h;
		param_set_pt( h, _event_untouch_count );
		for( INT32 i = 0; i < EVENT_NB_UI; ++i )
			param_set_pt( h, _event_untouch_id[i] );

	for( INT32 i = 0; i < BLOB_NB_UI; ++i )
	{
		++h;
		c_blob_ext*	blob = &_blobs[i];
		param_set_pt( h, blob->_id );
		param_set_pt( h, blob->_id_tag );

		param_set_pt( h, blob->_x );
		param_set_pt( h, blob->_y );
		param_set_pt( h, blob->_dx );
		param_set_pt( h, blob->_dy );	
		param_set_pt( h, blob->_x_filtered );
		param_set_pt( h, blob->_y_filtered );
		param_set_pt( h, blob->_size_x );
		param_set_pt( h, blob->_size_y );
		param_set_pt( h, blob->_angle );
		param_set_pt( h, blob->_area );
		param_set_pt( h, blob->_point_nb );
		param_set_pt( h, blob->_b_ghost );
	}
	err_param_init_pt( h );
}


CONSTRUCTOR_CREATE( c_bdd_blob_tracking )
,_current_id(1)
,_w_pos(0)
,_frame_out(0)
,_frame_in(0)
//,_b_net_need_reset_current(true)
,_b_mouse_use(false)
,_b_mouse_button_left_down(false)
,_b_mouse(false)
,_b_net_received(false)
,_p_receiving(nullptr)
,_p_received_ready(nullptr)
,_net_no_frame(0)
,_mouse_ignore_count(0)
{
	_history.push_back( _current );	//like this it never empty
	restart();

	param_init_with( n_bdd_blob_tracking::param, n_bdd_blob_tracking::PARAM_NB );

	clear_v3( _origin );

	_p_receiving = &_received;
}

c_bdd_blob_tracking::~c_bdd_blob_tracking() 
{
	if( cur == this )
		cur = nullptr;

	for( auto const & p : _filter )
	{
		delete p.second.first;
		delete p.second.second;
	}
}

void	c_bdd_blob_tracking::clear_event()
{
	//	BLOB_TRACKING_PRINT_STRING( "event clear" );
	_event_touch_count = 0;
	MEMCLEAR( _event_touch_id,   TOUCH_NB_MAX * sizeof(INT32) );

	_event_untouch_count = 0;
	MEMCLEAR( _event_untouch_id, TOUCH_NB_MAX * sizeof(INT32) );
}


void	c_bdd_blob_tracking::restart()
{
	clear_event();
	_blob_nb = 0;
	_current.clear();
	_history.clear();
	_b_restart_trig_ui = false;
}

FINLINE	void c_bdd_blob_tracking::draw_segment_one( c_finger const & fing )
{
	vector2df	center		= fing._center;
	vector2df	center_prev = center - fing._delta;

	draw_line( center_prev.get_x(), center_prev.get_y(), _w_pos+_w_step, center.get_x(), center.get_y(), _w_pos );
}

FINLINE	void c_bdd_blob_tracking::draw_color( c_finger const & fing, REAL alpha )
{
	if( fing._b_marked_for_deletion )
		alpha = REAL( alpha * .4 );
	_color_drawer.draw_color( fing._id_tracking, alpha );
}

FINLINE	void c_bdd_blob_tracking::draw_contour_one_color( c_finger & fing, REAL alpha )
{
	draw_color( fing, alpha );
	fing.draw_contour( _b_contour_draw_with_quad, _w_pos );
}

FINLINE	void c_bdd_blob_tracking::draw_contour_bounding_one_color( c_finger const & fing, REAL alpha )
{
	draw_color( fing, alpha );
	fing.draw_bounding(	_bounding_size_factor, _bounding_size_factor, 0.,
						_b_bounding_draw_as_ellipse_ui, _w_pos
						);
}

FINLINE	void c_bdd_blob_tracking::draw_segment_one_color( c_finger const & fing, REAL alpha )
{
	draw_color( fing, alpha );
	draw_segment_one( fing );
}

FINLINE	void c_bdd_blob_tracking::draw_tracking_one( c_finger & fing )
{
	if( _contour_alpha != 0. )		draw_contour_one_color(				fing, _contour_alpha	);
	if( _bounding_alpha != 0. )		draw_contour_bounding_one_color(	fing, _bounding_alpha	);
	if( _segment_alpha != 0. )		draw_segment_one_color(				fing, _segment_alpha	);
}

void c_bdd_blob_tracking::draw_tracking( std::vector<c_finger>& fingers )
{
	for( auto & elt : fingers )
		draw_tracking_one( elt );
}

void c_bdd_blob_tracking::draw_tracking()
{
	draw_tracking( _current );
}

void c_bdd_blob_tracking::draw_history_one( INT32 index )
{
	std::vector<INT32>	ids;
	INT32			i;
	INT32			deep;
	INT32			deep_stop = (INT32)_history.size() - _frames_nb_to_draw;

	for( deep=(INT32)_history.size()-1; deep>=deep_stop; --deep )
	{
		if(! _history[deep].empty() )	//todo quickly done to avoid a crash, check logic of this
		{
			ids.push_back( index );
			index = _history[deep][index]._hist_prev_index;
			if( index<0 )
			{
				--deep;	// to cope with the ++ after the loop
				break;
			}
		}
	}
	++deep;
	i=(INT32)ids.size()-1;
	for( ; i >= 0; --i, ++deep )
	{
		_w_pos = i * _w_step;
		draw_tracking_one( _history[deep][ids[i]] );
	}		
}

void c_bdd_blob_tracking::draw_history()
{
	INT32	stop = (INT32)_current.size();
	for( INT32 i = 0; i < stop; ++i )
	{
		draw_history_one( i );
	}
}

void c_bdd_blob_tracking::draw_history_raw()
{
	INT32	deep_start = (INT32)_history.size() - _frames_nb_to_draw;
	for( INT32 i = 0; i < _frames_nb_to_draw; ++i )
	{
		_w_pos = _w_max - i * _w_step;
		draw_tracking( _history[deep_start+i] );
	}		
}

void c_bdd_blob_tracking::maintain_history_size()
{
	while( INT32(_history.size()) > _frames_nb )
	{
		_history.erase( _history.begin() );
	}
}

void c_bdd_blob_tracking::add_finger_to_current( c_finger CONST & fing )
{
	_current.push_back( fing );
}
bool c_bdd_blob_tracking::add_blob_to_current( c_blob CONST & blob )
{
	bool b_valid = true;
/*
	REAL CONST delta_half = .005;	//this is 3mm for a 84" display and the mouse info seems to be slightly different
	if( _mouse_x != -42 )
	{
		auto CONST & cen = blob._center;
		auto CONST x = cen.get_x();
		if( INSIDE_MIN_MAX( _mouse_x, x-delta_half, x+delta_half ) )	
		{
			auto CONST y = cen.get_y();
			if( INSIDE_MIN_MAX( _mouse_y, y-delta_half, y+delta_half ) )
			{
				b_valid = false;
				if( _b_verbose_ui )
					VERBOSE_PRINT_STRING( "same blob than the one from mouse, skipping it" );
			}
		}
	}
*/
	if( b_valid )
	{
		_current.push_back( c_finger( blob ) );
		c_finger& fing = _current.back();
		fing._id_tracking = blob.get_id();
		if( fing._id_tracking == -1 )
			debug_break( "id is mouse" );
	}
	return b_valid;
}

void c_bdd_blob_tracking::build_from_old( std::vector<c_finger>& current, std::vector<c_finger>& previous )
{
	//if( !current.empty() || !previous.empty() )
	std::vector<c_finger>::iterator			cur;
	std::vector<c_finger>::const_iterator	cur_end = current.end();
	std::vector<c_finger>::iterator			prev;
	std::vector<c_finger>::const_iterator	prev_end = previous.end();
	//	link to the old blob or add a touch_event
	for( cur=_current.begin(); cur!=cur_end; ++cur )
	{
		INT32 id = cur->_id_tracking;
		bool b_found = false;
		INT32 j = 0;
		for( prev=previous.begin(); prev!=prev_end; ++prev )
		{
			if( prev->_id_tracking == id )
			{
				cur->_delta = cur->_center - prev->_center;
				assign_old_finger( &(*cur), j );
				b_found = true;
				break;
			}
			++j;
		}
		if( !b_found )
			event_add_touch( id );
	}
	//	find the untouch
	//todo use the link in the old
	for( prev=previous.begin(); prev!=prev_end; ++prev )
	{
		INT32 id = prev->_id_tracking;	
		bool b_found = false;
		for( cur=_current.begin(); cur!=cur_end; ++cur )
		{
			if( cur->_id_tracking == id )
			{
				b_found = true;
				break;
			}
		}
		if( !b_found )
			event_add_untouch( id );
	}
}

void c_bdd_blob_tracking::update()
{
	if( _b_restart_trig_ui )
		restart();

	_b_mouse_use = _b_mouse_use_ui;	// && c_mouse::is_grabbed();

	scale_v2( _tuio_in_blob_size_half, _tuio_in_blob_size_ui, _tuio_in_blob_size_ui[2] );
	if( _b_net_in )
	{
		if( _b_tuio_in )
			osc_take_and_process( osc::tuio_header, is_net_in_verbose() );
		osc_take_and_process( _osc_finger_header,	is_net_in_verbose() );
	}
	c_model::cur->get_size_v3( _size );
	if( is_active() )
	{
		if( _filter_factor_ui == 0. )
		{
			_filter_factor_x = _filter_factor_x_ui;
			_filter_factor_y = _filter_factor_y_ui;
		}
		else
		{
			_filter_factor_x = _filter_factor_ui;
			_filter_factor_y = _filter_factor_ui;
		}

//		BLOB_TRACKING_PRINT_STRING( "update" );
		if( _frames_nb < _frames_nb_to_draw_ui )
			_frames_nb = _frames_nb_to_draw_ui;

		_frames_nb_to_draw = MIN( _frames_nb_to_draw_ui, (INT32)_history.size() );
		INT32 blob_max_nb = _blob_max_nb_ui; 

		//todo tuio_in and aaaseed internal send is out for now reintroduce and check
		if( !_b_net_in && !_b_tuio_in )
		{
			bool	need_to_read_img	=	_b_read_img_contour_as_src	&& c_bdd_img_contour::cur	&& c_bdd_img_contour::cur->is_content_changed();
			bool	need_to_read_blob	=	_b_read_bdd_blob_as_src		&& c_bdd_blob::cur			;
			//bool	need_to_read_win	=	g_multitouch_master->is_content_changed();
			bool	need_to_read_win	=	g_multitouch_master->is_active() && g_multitouch_master->is_possible();
			bool	need_to_read		=	need_to_read_blob || need_to_read_img || need_to_read_win;
			bool	need_to_process		=	_b_mouse_use || need_to_read;

			//g_multitouch_master->transfer_blobs_to( _current );

			if( need_to_process )
			{
				_blob_nb = 0;
				clear_event();
				maintain_history_size();
				_current.clear();

				//g_multitouch_master->transfer_blobs_to( &_current );
				//_blob_nb = _current.size();
				//blob_max_nb -= _blob_nb;

				if( _b_mouse_use && !_b_mouse_use_when_no_touch_ui ) //&& !need_to_read_win )
				{
					process_from_mouse();
					if( _b_mouse_button_left_down )
					{
						++_blob_nb;
						--blob_max_nb;
					}
				}

				if( need_to_read ) 
				{
					//if( !_b_mouse_clicked && need_to_read_win )
					if( need_to_read_win )
					{
						g_multitouch_master->lock();
						UINT32 blob_nb = MIN( g_multitouch_master->get_blob_nb(), blob_max_nb );
						for( UINT32 i = 0; i < blob_nb; ++i )
						{
							if( add_blob_to_current( g_multitouch_master->get_blob_index(i) ) )
								--blob_max_nb;
						}
						g_multitouch_master->mark_read();
						g_multitouch_master->unlock();
					}

					if( need_to_read_img )
					{
						INT32 blob_nb = MIN( (INT32)c_bdd_img_contour::cur->get_blob_nb(), blob_max_nb );
						for( INT32 i = 0; i < blob_nb; ++i )
						{
							if( add_blob_to_current( c_bdd_img_contour::cur->get_blob_index(i) ) )
								--blob_max_nb;
						}
					}

					if( need_to_read_blob )
					{
						INT32 blob_nb = MIN( (INT32)c_bdd_blob::cur->get_blob_nb(), blob_max_nb );
						for( INT32 i = 0; i < blob_nb; ++i )
						{
							if( add_blob_to_current( c_bdd_blob::cur->get_blob_index(i) ) )
								--blob_max_nb;
						}
					}
				}

				if( _b_matrix_use_ui )
					process_results();
				else
					build_from_old( _current, _history.back() );

				_blob_nb += (INT32)_current.size();
				if( _blob_nb > 0 )
					_mouse_ignore_count = 3;
				_history.push_back( _current );
				if( _b_net_out )
				{
					if( _b_tuio_out )
						osc_send_2dcur();
					else if( _b_net_as_string )
						net_send_finger_string();
					else
						osc_send_finger();
				}
			}
		}
		else // if( !_b_net_in && !_b_tuio_in )
		{
			// no mouse when receiving for now
			//_blob_max_nb = _blob_max_nb_ui;
			//if( _b_mouse_use )
			//{
			//	process_from_mouse();
			//	if( _b_mouse_clicked )
			//	{
			//		_blob_nb = 1;
			//		_blob_max_nb -= _blob_nb;
			//	}
			//}
			if( _p_received_ready )
			{
				_net_no_frame = 0;
			}
			else
			{
				//vector<c_finger>& prev = _history.back();
				//_history.push_back( prev );

				++_net_no_frame;
				if( _net_no_frame > _tuio_silent_frame_max_nb )
				{
					_blob_nb = 0;
					clear_event();
					maintain_history_size();
					_current.clear();

					if( _b_mouse_use )
					{
						process_from_mouse();
						if( _b_mouse_button_left_down )
						{
							++_blob_nb;
							--blob_max_nb;
						}
					}
				}
				else
				{
				//todo pick what we do in this case (we should update if we use the mouse
				/*
					if ( !_history.empty() )
					{
					vector<c_finger>& prev = _history.back();
					_received = prev;
					}
				*/
				}
			}

			if( _p_received_ready )
			{
				_blob_nb = 0;
				clear_event();
				maintain_history_size();
				_current.clear();

				if( _b_mouse_use )
				{
					process_from_mouse();
					if( _b_mouse_button_left_down )
					{
						++_blob_nb;
						--blob_max_nb;
					}
				}

				//todo implement blob limitation
				//todo implement double buffer to avoid copy
				if( INT32(_p_received_ready->size()) > blob_max_nb )
				{
					INT32	nb = blob_max_nb;
					for( INT32 i = 0; i < nb; ++i )
						add_finger_to_current( (*_p_received_ready)[i] );
				}
				else
					_current = *_p_received_ready;
				_p_received_ready = nullptr;
	
				build_from_old( _current, _history.back() );

				_blob_nb += (INT32)_current.size();
				_history.push_back( _current );
				if( _b_net_out )
				{
					if( _b_tuio_out )
						osc_send_2dcur();
					else if( _b_net_as_string )
						net_send_finger_string();
					else
						osc_send_finger();
				}
			}
			else
			{
				clear_event();

			//	vector<c_finger>& prev = _history.back();
			//	_history.push_back( prev );

			//	++_net_no_frame;
			//	if ( _net_no_frame > 5)
			//	{
			//		// nothing received, clear
			//		_history.clear();
			//	}
			}
		}

		_b_mouse = false;
		if( _b_mouse_use )
		{
			if( _b_mouse_use_when_no_touch_ui && _blob_nb==0 ) //&& !need_to_read_win )
			{
				process_from_mouse();
				if( --_mouse_ignore_count < 0 && _b_mouse_button_left_down )
					++_blob_nb;
			}
			if( _b_mouse_button_left_down )
			{
				_current.insert( _current.begin(), _fing_mouse );
				_b_mouse = true;
			}
		}

		update_ui_blobs();
		c_bdd_blob_tracking::cur = this;
	}

	if( _frames_nb_to_draw == 1 )
	{
		_w_max	= 0.;
		_w_step	= 0.;
	}
	else
	{
		_w_max	= -_size[2];
		_w_step	= _w_max / REAL( _frames_nb_to_draw - 1 );
	}
}

void c_bdd_blob_tracking::draw_single()
{
	GOL::push_att();	//todoatt	//( GL_CURRENT_BIT | GL_ENABLE_BIT );
		GOL::push_color();
		_color_drawer.set_color_fix( _b_color_auto_ui ? nullptr : c_color::get_cur()->get_color_pt() );
		switch( _s_draw )
		{
		case DRAW_ALL:		draw_history_raw();		break;
		case DRAW_FINGER:	draw_history();			break;
		case DRAW_LAST:		draw_tracking();		break;
		}
	GOL::pop_att();
}

void c_bdd_blob_tracking::draw_multiple()
{
	//	lock();
	c_multiple::cur->set_nb( (INT32)_current.size() );

	vector<c_finger>::iterator			it;
	vector<c_finger>::const_iterator	it_end = _current.end();

	//todo	generalize at the same time
	//			normal on moving obj
	//			here on a plane so 2D and 3D version
	/*
	if( c_multiple::cur->is_align_normal() )
	{
		REAL	nor[3];
		REAL	u[3];
		REAL	v[3];

		for( ; i>0; i--, ++it )	//, p+=3 )
		{
			c_multiple::cur->index_set( (*it)->first );
			normalize_v3r( nor, (*it)->speed );
			cross_normalize_v3r( v, nor, (*it)->speed_old );
			cross_normalize_v3r( u, nor, v);
			//			normalize_v3r( nor, b->speed );
			//			make_axe( u, v, nor);
			multiple_draw_one_at_tgn( p, u, v, nor );
		}
	}
	else
	*/
	{
		_size[0] *= 2.;
		_size[1] *= 2.;
		REAL	p[3];
		p[2] = 0;
		for( it=_current.begin(); it!=it_end; ++it )
		{
			c_finger&	fing = *it;

			c_multiple::cur->set_index( fing._id_tracking );
			p[0] = fing._x_filtered;
			p[1] = fing._y_filtered;
			mul_add_v3( p, _size, _origin );
			c_multiple::cur->align_then_draw( p );
		}
	}
	//	unlock();
}

FINLINE	REAL c_bdd_blob_tracking::get_error( c_finger& prev, c_finger& now )
{
	vector2df	dif = prev._center - now._center;
	REAL		d = dif.get_length_squared();

	if( _b_use_predicted_pos_ui )
	{
		dif = prev._pos_predicted - now._center;
		d = MIN( d, dif.get_length_squared() );
	}

	if( _b_use_squared_dist_ui )
		return d;
	else
		return SQRT( d );
}



FINLINE	static	REAL	do_filter_one( c_flux_filter* filter, REAL filter_factor, REAL value )
{
	if( filter )
	{
		filter->put( value );
		filter->set_filter_factor( filter_factor );
		return REAL(filter->get());
	}
	else
	{
		ERR_PRINT_STRING( "%s() null filter", __FUNCTION__ );
		//		debug_break();
		return value;
	}
}

FINLINE	void	c_bdd_blob_tracking::do_filter( c_finger* fing )
{
	if( _filter.count( fing->_id_tracking ) == 0 )
	{
		pair_p_filter	filter_pair;
		filter_pair.first = new c_flux_filter;	//todo code analysys warn on this
		if( !filter_pair.first )
		{
			ERR_PRINT_STRING( "%s() can't alloc c_flux_filter first", __FUNCTION__ );
		}

		filter_pair.second = new c_flux_filter;
		if( !filter_pair.second )
		{
			ERR_PRINT_STRING( "%s() can't alloc c_flux_filter first", __FUNCTION__ );
		}
		_filter[fing->_id_tracking] = filter_pair;
	}
	fing->_x_filtered = do_filter_one( _filter[fing->_id_tracking].first, _filter_factor_x, fing->_center.get_x() );
	fing->_y_filtered = do_filter_one( _filter[fing->_id_tracking].second, _filter_factor_y, fing->_center.get_y() );
}

FINLINE	void	c_bdd_blob_tracking::assign_new_finger_with_id( c_finger* pt, INT32 tracking_id )
{
	pt->_id_tracking = tracking_id;

	pt->_pos_predicted = pt->_center;
	//double link for easy history parsing
	pt->_hist_prev_index = -1;	//this compensate probably a bug
	pt->_area_delta = 0;

	event_add_touch( pt );
}

void	c_bdd_blob_tracking::assign_new_finger( c_finger* pt )
{
	pt->_delta = vector2df(0,0);
	// assign ID's for any blobs that are new this frame (ones that didn't get 
	// matched up with a blob from the previous frame).
	assign_new_finger_with_id( pt, _current_id );
	if( ++_current_id >= 0X3fffffff )
		_current_id = 1;
}

void	c_bdd_blob_tracking::assign_old_finger( c_finger* pt, INT32 old_index )
{
	c_finger& old = _history.back()[old_index];

	old._b_reused = true;

	pt->_id_tracking = old._id_tracking;
	pt->_delta = (pt->_center - old._center);
	pt->_area_delta = pt->_area - old._area;
	if( pt->_delta.get_length() <= _min_dist_ui )
	{
		pt->_center = old._center;
		pt->_pos_predicted = pt->_center;
	}
	else
	{
		pt->_pos_predicted = pt->_center + pt->_delta;
	}
	//	link for easy history parsing
	pt->_hist_prev_index = old_index;

	do_filter( pt );
}


void c_bdd_blob_tracking::process_results()
{	
	_matrix._b_verbose = _b_verbose_matrix_ui;

	vector<c_finger>* prev = _history.empty() ? nullptr : &(_history.back());

	UINT32 prev_size = prev ? (UINT32)prev->size() : 0;
	UINT32 next_size = (UINT32)_current.size();

	//if( _b_matrix_use_ui )
	{
		// the error is for now the distance between too blob
		//	this could be refine
		//	in particular the smallest deviation from barycenter is very useful too
		//	we could also consider
		//		size,
		//		orientation,
		//		...

		REAL reject = _reject_dist_ui;
		if( _b_use_squared_dist_ui )
		{
			reject *= _reject_dist_ui;
			_matrix.set_penality_for_new( 1. );
		}
		else
		{
			_matrix.set_penality_for_new( 1. );
		}

		_matrix.init_size( next_size, prev_size );
		for( UINT32 i = 0; i < next_size; ++i )
		{
			c_finger		fing = _current[i];
			//fing._metric_row.clear();
			for( UINT32	j = 0; j < prev_size; ++j )
			{
				REAL error  = get_error( (*prev)[j], fing );
				if( error <  reject )
				{
					_matrix.add( i, j, error );
				}
			}
		}
		_matrix.solve();

		for( UINT32 i = 0; i < _matrix.get_row_nb(); ++i )
		{
			c_finger* next = &_current[i]; //all the next in this row are equivalent
			INT32	prev_index = _matrix.get_solution(i);
			if( prev_index == -1 )	assign_new_finger( next );
			else					assign_old_finger( next, prev_index );
		}
	}

	// if a blob disappeared this frame, send a finger up event
	for( UINT32 i = 0; i < prev_size; ++i )		// for each one in the last frame, see if it still exists in the new frame.
	{
		c_finger&	fing = (*prev)[i];

		if( fing._b_reused )
			fing._b_marked_for_deletion = false;
		else
		{
			//todo make sure mark for deletion is reset
			if( _ghost_frame_nb_ui == 0 )
				event_add_untouch( &fing );
			else if( fing._b_marked_for_deletion )
			{
				if( --(fing._frames_left) <= 0 )
					event_add_untouch( &fing );
				else
				{
					add_finger_to_current( fing );	// keep it around until framesleft = 0
					_current.back()._hist_prev_index = i;	// is in i in the previous one (ok we repeat drawing for now)
				}
			}
			else
			{
				add_finger_to_current( fing );	// keep it around until framesleft = 0
				_current.back()._hist_prev_index = i;	// is in i in the previous one (ok we repeat drawing for now)
				_current.back()._b_marked_for_deletion = true;
				_current.back()._frames_left = _ghost_frame_nb_ui;
			}
		}
	}
}

/*
void	c_bdd_blob_tracking::add_mouse_finger()
{
	FP32	x,y;
	c_mouse::get_cur()->get_pos_in_render( x,y );

	bool	b_left_click = c_mouse::get_cur()->get_but_state(  mouse::BUTTON_LEFT );

	if( b_left_click )
	{
		if( !_b_mouse_clicked )
		{
			// touch
			_fing_mouse._id_tracking = MOUSE_ID_TRACKING;
			_fing_mouse._area_delta = 0;
			_fing_mouse._center.set( x, y );
			_fing_mouse._id_tracking = MOUSE_ID_TRACKING;
			_fing_mouse._delta = vector2df( 0,0 );
		}
	}
	_b_mouse_clicked = b_left_click;
}
*/

extern deque<c_event_mouse>	events_mouse_exported;

void	c_bdd_blob_tracking::process_from_mouse()
{
//	FP32	x,y;
//	c_mouse::get_cur()->get_pos_in_render( x,y );

//	bool	b_left_click = ui::bdd_eating_mouse ? false : c_mouse::get_cur()->get_but_state( 0 );

	bool	b_left_down = false;
	FP32	x,y;
	x = y = .5;
	_mouse_x = -42;
	_mouse_y = -42;

	if( ui::bdd_eating_mouse )
		events_mouse_exported.clear();
	else
	{
		bool	b_event = false;
		if( !events_mouse_exported.empty() )
		{
			mouse::BUTTON	button;
			mouse::STATE	state;
			INT32	ix,iy;
			events_mouse_exported.front().get_event_button( ix, iy, button, state );
			events_mouse_exported.pop_front();
			if( button==mouse::BUTTON_LEFT )
			{
				b_event = true;
				b_left_down = ( state == mouse::DOWN );
				mouse_convert_xy_pixel_to_render( ix,iy, x,y );
			}
		}
		//	in case we lost unclick event
		if( !b_event && _b_mouse_button_left_down )
		{
			c_mouse::get_cur()->get_xy_render( x,y );
			b_left_down = c_mouse::get_cur()->get_but_state( mouse::BUTTON_LEFT );
			if( !b_left_down )
			{
				b_event = true;
			}
		}	
	}

	if( _b_mouse_button_left_down )
	{
		if( b_left_down )
		{
			//if( _b_verbose )
			//	VERBOSE_PRINTF( "_b_mouse_clicked left_click" );
			// update position
			_fing_mouse._delta = ( vector2df(x,y) - _fing_mouse._center );
			_fing_mouse._center.set( x,y );
			_mouse_x = x;
			_mouse_y = y;
			_fing_mouse._pos_predicted = _fing_mouse._center + _fing_mouse._delta;
			do_filter( &_fing_mouse );
		}
		else
		{
			//if( _b_verbose )
			//	VERBOSE_PRINTF( "_b_mouse_clicked No left_click" );
			// untouch
			event_add_untouch( &_fing_mouse );
		}
	}
	else
	{
		if( b_left_down )
		{
			//if( _b_verbose )
			//	VERBOSE_PRINTF( "No _b_mouse_clicked left_click" );
			// touch
			_fing_mouse._center.set( x, y );
			_mouse_x = x;
			_mouse_y = y;
			_fing_mouse._id_tracking = MOUSE_ID_TRACKING;
			_fing_mouse._delta = vector2df( 0,0 );
			_fing_mouse._area_delta = 0;
			_fing_mouse._pos_predicted = _fing_mouse._center;
			//double link for easy history parsing
			_fing_mouse._hist_prev_index = MOUSE_ID_TRACKING;	//this compensate probably a bug
			event_add_touch( &_fing_mouse );
		}
		else
		{
			// do nothing
		}
	}
	_b_mouse_button_left_down = b_left_down;
}

INT32		c_bdd_blob_tracking::get_finger_nb()
{
	return	_blob_nb;
//	return INT32( _current.size() );	//cost too much
}
c_finger*	c_bdd_blob_tracking::get_finger( INT32 index )
{
	if( IS_INDEX_VALID( index, _blob_nb ) )
	{
		return &_current[index];
	}
	return nullptr;
}
INT32	c_bdd_blob_tracking::get_point_nb()
{
	return get_finger_nb();
}
INT32	c_bdd_blob_tracking::get_point_and_id( REAL* dst, INT32 CONST index )
{
	c_finger*	finger = get_finger( index );
	if( finger )
	{
		set_v3( dst, finger->_center.get_x(), finger->_center.get_y(), REAL(0) );
		return finger->_id_tracking;
	}
	ERR_PRINT_STRING( "%s() can't find blob index %d", __FUNCTION__, index );
	clear_v3( dst );
	return 0;
}

static	c_rand_lin		rand_point;

//todo use t_in
bool	c_bdd_blob_tracking::get_point_and_speed_rnd_time_rel( REAL* CONST pos, REAL* CONST speed, REAL CONST t_in )
{
	UINT32 nb = get_point_nb();
	if( nb )
	{
		//	get one finger
		UINT32	i = (rand_point.get_uint32()>>8) % nb;
		c_finger*	finger = get_finger( i );
		set_v3( speed, finger->_delta.get_x(), finger->_delta.get_y(), REAL(0) );
		//	get a position between previous and current 
		REAL f = rand_point.get_fp32_01();
		set_v3( pos, finger->_center.get_x() - f * speed[0], finger->_center.get_y() - f * speed[1], REAL(0) );
		return true;
	}
	clear_v3( pos ); 
	clear_v3( speed );
	return false;
}

bool	c_bdd_blob_tracking::get_ghost( INT32 index )
{
	c_finger*	finger = get_finger( index );
	return finger ? finger->_b_marked_for_deletion : false;
}

void	c_bdd_blob_tracking::update_ui_blobs()
{
	UINT32	nb = MIN( BLOB_NB_UI, get_finger_nb() );
	//if( _b_verbose && nb )
	//	VERBOSE_PRINTF( "%d blobs", nb );
	//_blob_nb = nb;
	for( UINT32 i = 0; i < nb ; ++i )
	{
		_blobs[i].from_finger( _current[i] );
	//	if( _b_verbose )
	//		VERBOSE_PRINTF( "\t blob id(%d) %f,%f", _blobs[i]._id, _blobs[i]._x, _blobs[i]._y );
	}
}

/*

bool	c_bdd_blob_tracking::get_blob_info( UINT32 index, c_blob_ext& data )
{
	if ( index < _current.size() add_blob
	{
		data.from_finger( _current[index] );
		return true;
	}
	return false;
}

bool	c_bdd_blob_tracking::get_blob_info_by_id( UINT32 id, c_blob_ext& data )
{
	if( !_current.empty() )
	{
		UINT32	nb = _current.size();
		for( UINT32 i=0; i<nb; ++i )
		{
			if( _current[i]._id_tracking == id )
			{
				data.from_finger( _current[i] );
				return true;
			}
		}
	}
	DBG_PRINT_STRING( "in %s() blob with id %d not found", __FUNCTION__, id );
	return false;
}
*/

//EVENT
void	c_bdd_blob_tracking::event_add_touch( UINT32 id )
{
	if( _b_verbose_ui )	
		VERBOSE_PRINT_STRING( "event_add_touch %d", id );
	if( _event_touch_count < BLOB_NB_MAX )
	{
		_event_touch_id[_event_touch_count] = id;
		++_event_touch_count;
	}
}

void	c_bdd_blob_tracking::event_add_touch( c_finger* pfing )
{
	do_filter( pfing );
	event_add_touch( pfing->_id_tracking );
}

void	c_bdd_blob_tracking::event_add_untouch( UINT32 id )
{
	if( _b_verbose_ui )
		VERBOSE_PRINT_STRING( "event_add_untouch %d", id );
	if( _event_untouch_count < BLOB_NB_MAX )
	{
		_event_untouch_id[_event_untouch_count] = id;
		++_event_untouch_count;
	}
}

void	c_bdd_blob_tracking::event_add_untouch( c_finger* pfing )
{
	delete _filter[pfing->_id_tracking].first;
	delete _filter[pfing->_id_tracking].second;
	_filter.erase( pfing->_id_tracking );
	event_add_untouch( pfing->_id_tracking );
}

void	c_bdd_blob_tracking::net_send_finger_string()
{

	o_str	buffer;

	++_frame_out;
	buffer.set( _net_str_finger_header );

	CHAR	str[1024];

	sprintf( str, " frame %d", _frame_out );
	buffer.add( str );
	if( _b_mouse_use )
	{
		sprintf( str, " set %d %f %f", _fing_mouse._id_tracking, _fing_mouse._x_filtered, _fing_mouse._y_filtered );
		buffer.add( str );
	}

	if( _current.size() > 0 )
	{	
		std::vector<c_finger>::iterator		it;
		it = _current.begin();

		// send data
		for(; it != _current.end(); ++it )
		{
			sprintf( str, " set %d %f %f", (*it)._id_tracking, (*it)._x_filtered, (*it)._y_filtered );
			buffer.add( str );
		}
	}
	// send touch and untouch
	if( _event_touch_count > 0 )
	{
		buffer.add( " touch" );
		for( INT32 i = 0; i < _event_touch_count; ++i )
		{
			sprintf( str, " %d", _event_touch_id[i] );
			buffer.add( str );
		}
	}
	if( _event_untouch_count > 0 )
	{
		buffer.add( " untouch");
		for( INT32 i = 0; i < _event_untouch_count; ++i )
		{
			sprintf( str, " %d", _event_untouch_id[i] );
			buffer.add( str );
		}
	}

	buffer.add( "\n\n" );
	net->sendto( _osc_dst, 0, c_net::BLK_ASCII, (UINT8*)buffer.get(), buffer.get_len() );
}

namespace{
namespace {
//todo what is this size stuff
	#define	MAX_UDP_SIZE	65536
	char						buffer[osc::BLK_SIZE_MAX];	//todobeurk
	osc::OutboundPacketStream	pkt( buffer, osc::BLK_SIZE_MAX );
//	char						buffer[OSC_BLK_SIZE_MAX];
//	osc::OutboundPacketStream	p( buffer, OSC_BLK_SIZE_MAX );
//	char						buffer[ MAX_UDP_SIZE ];
//	osc::OutboundPacketStream	pkt( buffer, MAX_UDP_SIZE );
}

}

void	c_bdd_blob_tracking::osc_send_finger()
{

	++_frame_out;
	if( _blob_nb > 0 )
	{	
		pkt.clear();
		pkt << osc::BeginBundle();
		pkt << osc::BeginMessage( _osc_finger_header ) << "frame" << (osc::int32)_frame_out << osc::EndMessage;
		pkt << osc::EndBundle;
		if( pkt.is_ready() )
		{
			net->send_osc( _osc_dst, &pkt );
		}

		if( _b_mouse_use && _b_mouse_button_left_down )
		{
			pkt.clear();
			pkt << osc::BeginBundle();
			pkt << osc::BeginMessage( _osc_finger_header ) << "set" << _fing_mouse._id_tracking << _fing_mouse._x_filtered << _fing_mouse._y_filtered << _fing_mouse._delta.get_x() << _fing_mouse._delta.get_y();
			pkt << _fing_mouse._center.get_x() << _fing_mouse._center.get_y() << _fing_mouse._area << _fing_mouse._angle;
			pkt << _fing_mouse._weight << (osc::int32)_fing_mouse._point_nb << (osc::int32)_fing_mouse._id_tag;
			pkt << _fing_mouse._box.get_bottom_left().get_x() << _fing_mouse._box.get_bottom_left().get_y();
			pkt << _fing_mouse._box.get_top_right().get_x() << _fing_mouse._box.get_top_right().get_y();
			pkt << _fing_mouse._b_size_valid << osc::EndMessage;
			pkt << osc::EndBundle;
			if( pkt.is_ready() )
				net->send_osc( _osc_dst, &pkt );
		}

		if(  _current.size() > 0 )
		{	
			std::vector<c_finger>::iterator	it = _current.begin();

			// send data
			bool	b_done = false;
			INT32	count = 0;
			while( !b_done )
			{
				pkt.clear();
				pkt << osc::BeginBundle();

				for( ; it != _current.end(); ++it )
				{
					osc::int32	id;
					id = (*it)._id_tracking;
					pkt << osc::BeginMessage( _osc_finger_header ) << "set" << id << (*it)._x_filtered << (*it)._y_filtered << (*it)._delta.get_x() << (*it)._delta.get_y();
					pkt << (*it)._center.get_x() << (*it)._center.get_y() << (*it)._area << (*it)._angle;
					pkt << (*it)._weight << (osc::int32)(*it)._point_nb << (osc::int32)(*it)._id_tag;
					pkt << (*it)._box.get_bottom_left().get_x() << (*it)._box.get_bottom_left().get_y();
					pkt << (*it)._box.get_top_right().get_x() << (*it)._box.get_top_right().get_y();
					pkt << (*it)._b_size_valid << osc::EndMessage;


					++count;
					if( count >= 5 )
					{
						++it;
						count = 0;
						break;
					}
				}
				if( it == _current.end() )
					b_done = true;

				pkt << osc::EndBundle;

				if( pkt.is_ready() )
					net->send_osc( _osc_dst, &pkt );

			}		
		}
		// send touch and untouch
		pkt.clear();
		pkt << osc::BeginBundle();


		//if( _event_touch_count > 0 )
		//{
		//	pkt << osc::BeginMessage( _osc_finger_header ) << "touch";
		//	for( INT32 i = 0; i < _event_touch_count; ++i )
		//		pkt << (osc::int32)_event_touch_id[i];
		//	pkt << osc::EndMessage;
		//}
		//if( _event_untouch_count > 0 )
		//{
		//	pkt << osc::BeginMessage( _osc_finger_header ) << "untouch";
		//	for( INT32 i = 0; i < _event_untouch_count; ++i )
		//		pkt << (osc::int32)_event_untouch_id[i];
		//	pkt << osc::EndMessage;
		//}

		pkt << osc::BeginMessage( _osc_finger_header ) << "frame_end" << osc::EndMessage;
		pkt << osc::EndBundle;

		if( pkt.is_ready() )
		{
			net->send_osc( _osc_dst, &pkt );
		}
	}
	else
	{
		//p << osc::BeginBundleImmediate;
		pkt.clear();
		pkt << osc::BeginBundle();

		pkt << osc::BeginMessage( _osc_finger_header ) << "frame" << (osc::int32)_frame_out << osc::EndMessage;
		pkt << osc::BeginMessage( _osc_finger_header ) << "frame_end" << osc::EndMessage;
		pkt << osc::EndBundle;

		if( pkt.is_ready() )
		{
			net->send_osc( _osc_dst, &pkt );
		}
	}
}

void	c_bdd_blob_tracking::osc_receive_aaa_finger( CONST osc::ReceivedMessage& msg )
{
	// accept incoming net data
	if( !_b_net_in )
		return;

	_b_net_received = true;
	osc::ReceivedMessageArgumentStream args		= msg.ArgumentStream();
	osc::ReceivedMessage::const_iterator arg	= msg.ArgumentsBegin();

	CONST char* cmd;
	args >> cmd;
	VERBOSE_PRINT_STRING( "cmd = %s\n", cmd );
	if( strcmp( cmd, "set" ) == 0 )
	{
		VERBOSE_PRINT_STRING( "touch set" );
		osc::int32	id, tag_id, point_nb;
		REAL	x, y, dx, dy;
		REAL	cx, cy, area, angle, weight, bl_x, bl_y, ur_x, ur_y;
		bool	b_size_valid;

		args >> id >> x >> y >> dx >> dy >> cx >> cy >> area >> angle >> weight >> point_nb >> tag_id >> bl_x >> bl_y >> ur_x >> ur_y >> b_size_valid >> osc::EndMessage;
		if( is_net_in_verbose_detailed() )
			VERBOSE_PRINT_STRING( "id %d, x %f, y %f, dx %f, dy %f", id, x, y, dx, dy );

		// new finger
		c_finger	fing;
		fing._id_tracking = id;
		fing._id_tag = tag_id;
		fing._point_nb = point_nb;
		fing._box.set_bottom_left( bl_x, bl_x );
		fing._box.set_top_right( ur_x, ur_y );
		fing._angle = angle;
		fing._area = area;
		fing._center.set( cx, cy );
		fing._weight = weight;
		fing._x_filtered = x;
		fing._y_filtered = y;
		fing._b_size_valid = b_size_valid;
		fing._delta = vector2df( dx,dy );

		bool	b_found = false;
		UINT32	nb = (UINT32)_received.size();
		for( UINT32 i = 0; i < nb; ++i )
		{
			if( _received[i]._id_tracking == id )
			{
				b_found = true;
				break;
			}
		}
		if( !b_found )	//	we accumulate so we want the first contact only
			_received.push_back( fing );
		//_received.push_back( fing );
		//_current.push_back( fing );
	}
	//else if( strcmp( cmd, "touch" ) == 0 )
	//{
	//	//if( _b_tuio_verbose_in )
	//		VERBOSE_PRINTF( "touch touch" );
	//	osc::int32 id;
	//	while(!args.Eos())
	//	{
	//		args >> id;

	//		if( id > 0 )
	//		{
	//			event_add_touch( id );
	//		}
	//	}
	//	args >> osc::EndMessage;
	//}
	//else if( strcmp( cmd, "untouch" ) == 0 )
	//{
	//	//if( _b_tuio_verbose_in )
	//		VERBOSE_PRINTF( "touch untouch" );
	//	osc::int32 id;
	//	while(!args.Eos())
	//	{
	//		args >> id;

	//		if( id > 0 )
	//		{
	//			event_add_untouch( id );
	//		}
	//	}
	//	args >> osc::EndMessage;
	//}
	else if( strcmp( cmd, "frame" ) == 0 )
	{
		osc::int32 frame_nb;
		args >> frame_nb >> osc::EndMessage;

		if( is_net_in_verbose() )
		{
			VERBOSE_PRINT_STRING( "%s() touch frame %d", __FUNCTION__, frame_nb );
		}
		// new frame
		if( frame_nb > _frame_in )
		{
			// new frame
			_frame_in = frame_nb;
				
			//_history.push_back( _received );
			//_received.clear();


			//	clear_event();
			_history.push_back( _current );
			maintain_history_size();
			_current.clear();
		}
	}
	else if( strcmp( cmd, "frame_end" ) == 0 )
	{
		// end of frame
	}
	else
	{
		ERR_PRINT_STRING( "%s() unknown cmd : %s", __FUNCTION__, cmd );
	}
}

void	c_bdd_blob_tracking::osc_send_2Dobj()
{
}


//error
//this do not correspond to specification
void	c_bdd_blob_tracking::osc_send_2dcur()
{


	++_frame_out;
	pkt.clear();
	pkt << osc::BeginBundle();

	REAL	fx = _tuio_out_scale[0];
	REAL	fy = _tuio_out_scale[1];
	REAL	ox = _tuio_out_offset[0];
	REAL	oy = _tuio_out_offset[1];

	if( _b_mouse_use && _b_mouse_button_left_down )
	{
		pkt << osc::BeginMessage( osc::tuio_header ) << "set" << _fing_mouse._id_tracking << _fing_mouse._x_filtered *fx+ox << _fing_mouse._y_filtered*fy+oy << _fing_mouse._delta.get_x() << _fing_mouse._delta.get_y();
		pkt << _fing_mouse._center.get_x() << _fing_mouse._center.get_y() << _fing_mouse._area << _fing_mouse._angle << _fing_mouse._weight << (osc::int32)_fing_mouse._point_nb << (osc::int32)_fing_mouse._id_tag;
		pkt << _fing_mouse._box.get_bottom_left().get_x() << _fing_mouse._box.get_bottom_left().get_y();
		pkt << _fing_mouse._box.get_top_right().get_x() << _fing_mouse._box.get_top_right().get_y();
		pkt << _fing_mouse._b_size_valid << osc::EndMessage;
	}

	if( _current.size() > 0 )
	{	
		std::vector<c_finger>::iterator	it = _current.begin();

		bool	b_done = false;
		//INT32	count = 0;
		// send data
		while( !b_done )
		{
			//	p.Clear();
			//	p << osc::BeginBundle();
			for( ; it != _current.end(); ++it )
			{
				osc::int32	id = (*it)._id_tracking;
				REAL	x = (*it)._x_filtered * fx + ox;
				REAL	y = (*it)._y_filtered * fy + oy;
				REAL	dx = (*it)._box.get_size_x() * fx;
				REAL	dy = (*it)._box.get_size_y() * fy;

				pkt << osc::BeginMessage( osc::tuio_header ) << "set" << id << x << y << dx << dy;
				pkt << 0.0f << osc::EndMessage;

				//	++count;
				//	if( count >= 5 )
				//	{
				//		count = 0;
				//		++it;
				//		break;
				//	}
			}

			if( it == _current.end() )
				b_done = true;

			//	pkt << osc::EndBundle;

			//	if( pkt.is_ready() )
			//		net->send_osc( _osc_dst, &pkt );
		}		

		// send touch and untouch
		//	pkt.Clear();

//		if( _event_untouch_count > 0 )
		{
//			pkt << osc::BeginBundle();
			pkt << osc::BeginMessage( osc::tuio_header ) << "alive";
			it = _current.begin();
			for( ; it != _current.end(); ++it )
			{
				osc::int32	id;
				id = (*it)._id_tracking;
				pkt << id;
			}
			pkt << osc::EndMessage;

//			pkt << osc::EndBundle;

//			if( pkt.is)ready() )
//				net->send_osc( _osc_dst, &pkt );
		}
	}
	else
	{
		pkt << osc::BeginMessage( osc::tuio_header ) << "alive" << osc::EndMessage;
	}
	pkt << osc::BeginMessage( osc::tuio_header ) << "fseq" << (osc::int32)_frame_out << osc::EndMessage;

	pkt << osc::EndBundle;

	if( pkt.is_ready() )
	{
		net->send_osc( _osc_dst, &pkt );
	}
}

CONSTEXPR C_PCHAR_C		TUIO_HEADER = "### TUIO -> ";
void	TUIO_PRINTF( C_PCHAR fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	HEADER_PRINT_STRING_VA( TUIO_HEADER, fmt, args );
	va_end(args);
}

//	documented at http://www.tuio.org/?specification
void	c_bdd_blob_tracking::osc_receive_tuio_2Dcur( CONST osc::ReceivedMessage& msg )
{
	//	at least consume the block
	if( !_b_tuio_in )
		return;

	osc::ReceivedMessageArgumentStream		args	= msg.ArgumentStream();
	osc::ReceivedMessage::const_iterator	arg		= msg.ArgumentsBegin();

	CONST char* cmd;
	args >> cmd;

	if( strcmp( cmd, "set" ) == 0 )
	{
		osc::int32	s_id;
		REAL		x, y, dx, dy;
		REAL		m_acc;
		REAL		a = 0.;
		REAL		da;
		//REAL dda;

		if( strcmp( msg.TypeTags(), "sifffffff" )==0 )	//	TUIO 1.1 tbeta ???
		{
			args >> s_id >> x >> y >> a >> dx >> dy >> da >> m_acc >> osc::EndMessage;
		}
		else	//	TUIO 1.0 and 1.1 
		{
			args >> s_id >> x >> y >> dx >> dy >> m_acc >> osc::EndMessage;
		}

		//listener->updateTuioCur((int)s_id,x,y,X,Y,m);
		INT32	id = s_id;
		if( is_net_in_verbose_detailed() )
		{
			TUIO_PRINTF( "2Dcur set id %d, x,y : %f,%f, xspeed,yspeed : %f,%f, accel %f", id , x, y, dx, dy, m_acc );
		}

		if(  _b_tuio_in_flip_x )
			x = REAL(1) - x;
		//	it seem that tuio is flipped by default
		if( !_b_tuio_in_flip_y )
			y = REAL(1) - y;

		x = ((x-REAL(.5))*_tuio_in_scale_x) + REAL(.5) + _tuio_in_offset_x; 
		y = ((y-REAL(.5))*_tuio_in_scale_y) + REAL(.5) + _tuio_in_offset_y;
		// new finger
		c_finger	fing;
		fing._id_tracking = s_id;
		fing._x_filtered = x;
		fing._y_filtered = y;
		fing._center = vector2df( x,y );
		fing._box.set_bottom_left(	x - _tuio_in_blob_size_half[0],	y - _tuio_in_blob_size_half[1] );
		fing._box.set_top_right(	x + _tuio_in_blob_size_half[0],	y + _tuio_in_blob_size_half[1] );
		fing._b_marked_for_deletion = false;
		fing._b_size_valid = true;
		fing._delta = vector2df( dx,dy );

		bool	b_found = false;
		UINT32	nb = (UINT32)_p_receiving->size();
		for( UINT32 i = 0; i < nb; ++i )
		{
			if( (*_p_receiving)[i]._id_tracking == s_id )
			{
				b_found = true;
				(*_p_receiving)[i] = fing;
				break;
			}
		}
		if( !b_found )	//	we accumulate so we want the first contact only
			_p_receiving->push_back( fing );
	}
	else if( strcmp( cmd, "alive" ) == 0 )
	{	//	list of alive blobs
		//	infact if blob don't move they don't need to be send with a set, so if alive we have to get the data from the last frame

		if( is_net_in_verbose_detailed() )
		{
			TUIO_PRINTF( "2Dcur alive" );
		}
		if( _b_tuio_in_skip_alive )
		{
			return;
		}
		//if( _b_net_need_reset_current )
		//{
		//	_b_net_need_reset_current = false;
		//	_current.clear();
		//	maintain_history_size();
		//}

		osc::int32 s_id;
		while( !args.Eos() )
		{
			args >> s_id;
			if( is_net_in_verbose_detailed() )
				TUIO_PRINTF( "%d is alive", s_id );
			//todo optimize this search
			//	already in the receiving data
			bool	b_found = false;
			UINT32	nb = (UINT32)_p_receiving->size();
			for( UINT32 i = 0; i < nb; ++i )
			{
				if( (*_p_receiving)[i]._id_tracking == s_id )
				{
					b_found = true;
					break;
				}
			}
			//	or else get from the previous one
			if( !b_found )
			{
				//todo optimize this search
				vector<c_finger>& prev_finger = _history.back();
				std::vector<c_finger>::const_iterator	prev_end = prev_finger.end();
				std::vector<c_finger>::iterator			prev;
				for( prev = prev_finger.begin(); prev != prev_end; ++prev )
				{
					if( prev->_id_tracking == s_id )
					{
						_p_receiving->push_back( *prev );
						break;
					}
				}
			}
		}
	}
	else if( strcmp( cmd, "fseq" ) == 0 )
	{	//	start a frame
		osc::int32 currentFrame;
		args >> currentFrame  >> osc::EndMessage;
		if( currentFrame == -1 )
		{
			// no signification only redundant bundle
		}
		else
		{
			if( is_net_in_verbose() )
				TUIO_PRINTF( "2Dcur fseq %d with %d blobs", (INT32)currentFrame, (INT32)_p_receiving->size() );
			_p_received_ready = _p_receiving;
			if( _p_receiving == &_received )	{	_p_receiving = &_received_bis;	}
			else								{	_p_receiving = &_received;		}
			_p_receiving->clear();
			//_blob_nb = 0;

			/* maa make tuio work for erasme
			// compare prev and current to find deleted finger
			vector<c_finger>& prev = _history.back();
			bool	b_found = false;
			if ( prev.size() > 0 )
			{
				UINT32	nb = prev.size();
				UINT32	nb_cur = _current.size();
				for( UINT32 i = 0; i < nb; ++i )
				{
					for( UINT32 j = 0; j< nb_cur; ++j )
					{
						if( prev[i]._id_tracking == _current[j]._id_tracking )
						{
							b_found = true;
								break;
						}
					}
					if( !b_found )
						event_add_untouch( prev[i]._id_tracking );
				}
			}
			*/
			//_history.push_back( _current );
			//_b_net_need_reset_current = true;
		}
	}
	else if( strcmp( cmd, "source" ) == 0 )
	{
		CONST char* str;
		args >> str  >> osc::EndMessage;
			// new bundle
		if( is_net_in_verbose_detailed() )
		{
			TUIO_PRINTF( "2Dcur source %s", str );
		}
	}
	else
	{
		ERR_PRINT_STRING( "TUIO %s() unknown cmd : %s", __FUNCTION__, cmd );
	}
}

//	documented at http://www.tuio.org/?specification
// accept tuio in fiducial data
void	c_bdd_blob_tracking::osc_receive_tuio_2Dobj( CONST osc::ReceivedMessage& msg )
{
	if( !_b_tuio_in )
		return;

	_b_net_received = true;
	osc::ReceivedMessageArgumentStream		args = msg.ArgumentStream();
	osc::ReceivedMessage::const_iterator	arg = msg.ArgumentsBegin();

	CONST char* cmd;
	args >> cmd;

	if( strcmp( cmd, "set" ) == 0 )
	{	
		if( is_net_in_verbose_detailed() )
		{
			TUIO_PRINTF( "2Dobj set" );
		}
		osc::int32 s_id, f_id;
		REAL xpos, ypos, angle, xspeed, yspeed, rspeed, maccel, raccel;

		args >> s_id >> f_id >> xpos >> ypos >> angle >> xspeed >> yspeed >> rspeed >> maccel >> raccel >> osc::EndMessage;

		INT32	id = s_id;
		INT32	fid = f_id;
		if( is_net_in_verbose_detailed() )
		{
			TUIO_PRINTF( "s_id : %d, f_id : %d, xpos : %f, ypos : %f, angle: %f, xspeed: %f, yspeed: %f, rspeed: %f, maccel: %f, raccel ", id , fid , xpos , ypos , angle , xspeed , yspeed , rspeed , maccel , raccel );
		}
	}
	else if( strcmp( cmd, "alive" ) == 0 )
	{
		if( is_net_in_verbose_detailed() )
		{
			TUIO_PRINTF( "2Dobj alive" );
		}
		osc::int32 s_id;
		while(!args.Eos())
		{
			args >> s_id;
			if( is_net_in_verbose_detailed() )
			{
				TUIO_PRINTF( "2Dobj Alive %d", s_id );
			}
			//INT32	id = s_id;
			//objectBuffer.push_back((unsigned int)s_id);

			//std::list<unsigned int>::iterator iter;
			//iter = find(aliveObjectList.begin(), aliveObjectList.end(), (unsigned int)s_id); 
			//if (iter != aliveObjectList.end()) aliveObjectList.erase(iter);
		}
		//args >> osc::EndMessage;
	}
	else if( strcmp( cmd, "fseq" ) == 0 )
	{
		osc::int32 currentFrame;
		args >> currentFrame  >> osc::EndMessage;
		if( currentFrame == -1 )
		{
			// no signification only redundant bundle
		}
		else if( is_net_in_verbose() )
		{
			TUIO_PRINTF( "2Dobj fseq %d", (INT32)currentFrame );
		}
	}
	else
	{
		ERR_PRINT_STRING( "%s() unknown cmd : %s", __FUNCTION__, cmd );
	}

}

void	c_bdd_blob_tracking::osc_process_message( CONST osc::ReceivedMessage& msg )
{
	if( c_net::b_verbose_osc_processed_ui )
	{
		c_net::osc_print_message( "OSC Process :", msg );
	}
	CONST char*	addr =  msg.AddressPattern();
	if( strcmp( addr, "/aaa/finger" ) == 0 )
	{
		osc_receive_aaa_finger( msg );
	}
	else if( str_is_equal( addr, "/tuio/", 6 ) )
	{
		addr += 6;
		//	documented at http://www.tuio.org/?specification
		if( strcmp( addr, "2Dobj" ) == 0 )
		{
			osc_receive_tuio_2Dobj( msg );
		} 
		else if( strcmp( addr, "2Dcur" ) == 0 )
		{
			osc_receive_tuio_2Dcur( msg ); 
		}
		else
		{
			ERR_PRINT_STRING( "%s() TUIO address not implemented : %.64s", __FUNCTION__, addr );
		}
	}
	else
	{
		ERR_PRINT_STRING( "%s() OSC address not implemented : %.64s", __FUNCTION__, addr );
	}
	//} catch( Exception& e ){
	//	std::cout << "error while parsing message: "<< msg.AddressPattern() << ": " << e.what() << "\n";
	//}
}
 