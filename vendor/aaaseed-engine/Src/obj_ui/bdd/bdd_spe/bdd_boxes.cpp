#include "obj_ui/bdd/bdd_spe/bdd_boxes.h"
#include "obj_ui/bdd/bdd_point/bdd_blob.h"
#include "draw/model.h"
#include "draw/rect.h"
#include "draw/line.h"
#include "draw/shape.h"
#include "draw/box.h"
#include "draw/geo/sphere.h"
#include "strnum.h"
#include "math/billboard.h"
#include "ui/alphabet.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "infrastructure/viewport.h"

#include "infrastructure/aaa_const.h"
#include "infrastructure/aaa_window.h"

#include "draw/color.h"
#include "draw/tex_anim.h"
#include "gol/gol.h"
#include "gol/gol_light.h"


namespace {
	CONSTEXPR REAL	color_disabled[4]	=	{ REAL(.1), REAL(.1), REAL(.1), REAL(.1) };
	CONSTEXPR REAL	color_back[4]		=	{ REAL(1),  REAL(1),  REAL(1),  REAL(.4) };
	CONSTEXPR REAL	color_normal[4]		=	{ REAL(.8), REAL(.8), REAL(.8), REAL(.5) };
	CONSTEXPR REAL	color_selected[4]	=	{ REAL(1),  REAL(1),  REAL(1),  REAL(1)  };
}

namespace {
	bool	g_draw_selected_with_line = true;
}



C_PCHAR_C	c_box_one::box_type_str[BOX_TYPE_MAX] =
{
	"FIXE",
	"FREE",
	"VERTICAL",
	"VERTICAL_FRAMED",
	"HORIZONTAL",
	"HORIZONTAL_FRAMED",
	"FINGER",
	"POTENTIOMETER",
};

template< INT32 s_dim >
FINLINE	bool	c_box_one::is_in_box( REAL x, REAL y, REAL z ) CONST
{
/*	if( r.b_sphere)
	{
	}
	else
*/
	switch( _s_type )
	{
	case BOX_VERTICAL_FRAMED:
		if( ABS( x-_pos[0] ) <= (_size[0]*.5) )
			return true;
		break;
	case BOX_HORIZONTAL_FRAMED:
		if( ABS( y-_pos[1] ) <= (_size[1]*.5) )
			return true;
	default:
		if( ABS( x-_pos[0] ) <= (_size[0]*.5) )
			if( ABS( y-_pos[1] ) <= (_size[1]*.5) )
				if( ABS( z-_pos[2] ) <= (_size[2]*.5) )
					return true;
		break;
	}
	return false;
}

FINLINE	REAL CONST *	c_box_one::get_color_factor() CONST
{
	if( !_b_enabled )
		return color_disabled;
	else if( _b_selected )
		return color_selected;
	else
		return color_normal;
}

FINLINE	void	c_box_one::set_color() CONST
{
	c_color::get_cur()->draw_with_color_scaled( get_color_factor() );	//hack fast and quick : no time to fuck fly
}

FINLINE	REAL CONST*	c_box_one::get_color_factor_back() CONST
{
	if( !_b_enabled )
		return color_disabled;
	else 
		return color_back;
}

FINLINE	void	c_box_one::set_color_back() CONST
{
	c_color::get_cur()->draw_with_color_scaled( get_color_factor_back() );	//hack fast and quick : no time to fuck fly
}

void	c_box_one::draw( REAL* pos, REAL* size, INT32 s_dim )
{
	REAL center[3];
	add_v3( center, pos, _pos );
	if( s_dim == 3 )
	{
		set_color();
		if( _b_sphere )
			draw_sphere_at( _size, center );
		else
			draw_box_at( _size, center );
	}
	else if( s_dim == 2 )
	{
		//	BACK
		set_color_back();
		switch( _s_type )
		{
		case BOX_VERTICAL_FRAMED:
			//todo 4 here is a hack
			draw_rect(	center[0]-_size[0]*REAL(.5),	pos[1]-REAL(4),					center[0]+_size[0]*REAL(.5),	pos[1]+REAL(4)				);
			break;
		case BOX_HORIZONTAL_FRAMED:
			draw_rect(	pos[0]-REAL(4),					center[1]-_size[1]*REAL(.5),	pos[0]+REAL(4),					center[1]+size[1]*REAL(.5)	);
			break;
		case BOX_POTENTIOMETER:
			draw_circle_z_at( center, _size[0], GL_TRIANGLE_FAN, 24 );
			break;
		}
	//	if( _b_sphere )
	//		draw_circle_z_at( _pos, _size[2] );
	//	else
		set_color();
		switch( _s_type )
		{
		case BOX_POTENTIOMETER:	draw_circle_z_at( center, _size[0], GL_TRIANGLE_FAN, 12 );	break;
		default:				draw_rect_size_at( _size, center );						break;
		}
		REAL	s;
		switch( _s_type )
		{
		case BOX_VERTICAL_FRAMED:
		case BOX_VERTICAL:
			s = _b_selected ? size[1] : _size[1]*REAL(.5);
			draw_line(	center[0],		center[1]-s,	center[0],		center[1]+s );
			break;
		case BOX_HORIZONTAL_FRAMED:
		case BOX_HORIZONTAL:
			s = _b_selected ? size[0] : _size[0]*REAL(.5);
			draw_line(	center[0]-s,	center[1],	center[0]+s,	center[1] );
			break;
		case BOX_FREE:
		case BOX_FINGER:
			if( _b_selected && g_draw_selected_with_line )
			{
				GOL::begin( GL_LINES );
					GOL::vertex2f( center[0],	-size[1]	);
					GOL::vertex2f( center[0],	size[1]		);
					GOL::vertex2f( -size[0],	center[1]	);
					GOL::vertex2f( size[0],		center[1]	);
				GOL::end();
			}
			break;
		case BOX_POTENTIOMETER:
			draw_line( _pos[0], _pos[1], center[0] + _size[0]*REAL(1.2)*COS_TURN(_out[0]), center[1] + _size[0]*REAL(1.2)*SIN_TURN(_out[0]) );
			break;
		}
	}
}

void	c_box_one::update( REAL* size, REAL* box_size )
{
	if( is_active() )
	{
		REAL vec[3];
		scale_v3( vec, _size_ui, _size_ui[3] );
		mul_v3( vec, box_size );
		//we scale according to the general scale
		mul_v3( _pos, _pos_ui, size );
		mul_v3( _size, vec, size );
	}
}

CONSTEXPR	INT32 BDD_BOXES_CHANNEL_NB_MAX = CHANNEL_NB_MAX;

FACTORY_CREATE_PROP_V1( c_bdd_boxes, bdd_boxes, Boxes, bdd_boxes, sub_menu = "UI"; );

namespace	n_bdd_boxes
{
	CONSTEXPR INT32	BASE_PARAM_NB		= 17 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	ITEM_PARAM_NB		= 27;
	CONSTEXPR INT32	BUILD_PARAM_NB		= 8;
	CONSTEXPR INT32	TRANSFER_PARAM_NB	= 9;
	CONSTEXPR INT32	GROUP_PARAM_NB		= 2;
	CONSTEXPR INT32	START_BOX_INDEX		=	BASE_PARAM_NB
										+	BUILD_PARAM_NB
										+	TRANSFER_PARAM_NB
										+	GROUP_PARAM_NB;
	CONSTEXPR INT32	PARAM_NB_MAX	=	START_BOX_INDEX
									+	( ITEM_PARAM_NB + 1 ) * c_bdd_boxes::BOXES_NB_MAX;

	c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ(		origin )
		//PARAM_DEF_INT32( channel_id_offset, 2, 1, 1, CHANNEL_NB_MAX )
		//PARAM_DEF_INT32( control_id_offset, 2, 1, 1, CONTROL_NB_MAX )

		//	extend to 1D and (why not 4D, 5D, 6D
		PARAM_DEF_SYMBO_MIN_MAX(	dimension,	3, 2,	2, PT_NB_STR(gstr::dim), gstr::dim )

		PARAM_DEF_INT32(			start,	2, 1,	1, c_bdd_boxes::BOXES_NB_MAX )
		PARAM_DEF_INT32(			stop,	2, 1,	1, c_bdd_boxes::BOXES_NB_MAX )

		PARAM_DEF_BOOL_OFF(			ui_intercept )
		PARAM_DEF_BOOL_OFF(			ui_crop_to_viewport )
		PARAM_DEF_BOOL_OFF(			verbose )
		PARAM_DEF_SYMBO(			number_draw, 4, 0,	4, gstr::draw_number )
		PARAM_DEF_BOOL_ON(			draw_selected_with_line )

		PARAM_DEF_GROUP_CLOSED( Build, BUILD_PARAM_NB )
			PARAM_DEF_REAL_INF( build_right,		0, -.5	)
			PARAM_DEF_REAL_INF( build_left,		0, -.5	)
			PARAM_DEF_REAL_INF( build_bottom,	0, -.5	)
			PARAM_DEF_REAL_INF( build_top,		0, -.5	)
			PARAM_DEF_BOOL_OFF( build_horizontal )
			PARAM_DEF_BOOL_OFF( build_vertical )
			PARAM_DEF_BOOL_OFF( build_finger )
			PARAM_DEF_BOOL_OFF( frame_trig )

		PARAM_DEF_SCALE_XYZF(	box_size )
		PARAM_DEF_REAL_ONE(		number_size )
		PARAM_DEF_REAL_ONE(		text_size )

		PARAM_DEF_GROUP_CLOSED( Transfer, TRANSFER_PARAM_NB )
			PARAM_DEF_REF(			transfer_bdd_target )
			PARAM_DEF_REAL_ONE(		transfer_radius_factor )
			PARAM_DEF_POINT_XYZ(	transfer_center )
			PARAM_DEF_SCALE_XYZF(	transfer_size )

		PARAM_DEF_GROUP_CLOSED( Box01, ITEM_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			Box01_active			)
			PARAM_DEF_BOOL_OFF(			Box01_enabled			)
			PARAM_DEF_BOOL_OFF(			Box01_manual			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	Box01_type,				c_box_one::box_type_str )
			PARAM_DEF_REF(				Box01_name				)
			PARAM_DEF_INT32(			Box01_channel_id,		2, 1, 1, CHANNEL_NB_MAX )
			PARAM_DEF_INT32(			Box01_control_id,		2, 1, 1, CONTROL_NB_MAX )
			PARAM_DEF_POINT_XYZ(		Box01_position			)
			PARAM_DEF_SCALE_XYZF(		Box01_size				)
			PARAM_DEF_POINT_XYZ(		Box01_frame_position	)
			PARAM_DEF_SCALE_XYZF(		Box01_frame_size		)
			PARAM_DEF_BOOL_OFF(			Box01_sphere			)
			PARAM_DEF_BOOL_LOCKED(		Box01_inside			)
			PARAM_DEF_BOOL_OFF(			Box01_selected			)
			PARAM_DEF_UVW_LOCKED(		Box01_out				)
	};
}

void	c_bdd_boxes::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _origin );

	//param_set_pt( h, _channel_id );
	//param_set_pt( h, _control_id );

	param_set_pt( h, _s_dim );

	param_set_pt( h, _start );
	param_set_pt( h, _stop );

	param_set_pt( h, _b_ui_intercept_ui );
	param_set_pt( h, _b_ui_crop_to_viewport );
	param_set_pt( h, _b_verbose );
	param_set_pt( h, _s_number_draw );
	param_set_pt( h, _b_draw_selected_with_line );

	++h;
		param_set_pt( h, _build_left );
		param_set_pt( h, _build_right );
		param_set_pt( h, _build_bottom );
		param_set_pt( h, _build_top );
		param_set_pt( h, _b_hori_trig );
		param_set_pt( h, _b_vert_trig );
		param_set_pt( h, _b_finger_trig );
		param_set_pt( h, _b_frame_trig );

	param_set_pt_4( h, _box_size_ui );
	param_set_pt( h, _number_size );
	param_set_pt( h, _text_size );

	++h;
		param_set_pt_attach_obj(	h, _target_name_symbo,		_bdd_target );
		param_set_pt(				h, _transfer_radius_factor	);
		param_set_pt_3(			h, _transfer_translate		);
		param_set_pt_4(			h, _transfer_scale_ui		);

	for( INT32 index = 0; index < BOXES_NB_MAX; ++index )
	{
		++h;
			param_set_pt( h, _box[index]._b_active );
			param_set_pt( h, _box[index]._b_enabled );
			param_set_pt( h, _box[index]._b_manual );
			param_set_pt( h, _box[index]._s_type );
			param_set_pt( h, _box[index]._o_name );
			param_set_pt( h, _box[index]._channel_id );
			param_set_pt( h, _box[index]._control_id );

			param_set_pt_3( h, _box[index]._pos_ui );
			param_set_pt_4( h, _box[index]._size_ui );
			param_set_pt_3( h, _box[index]._frame_pos_ui );
			param_set_pt_4( h, _box[index]._frame_size_ui );

			param_set_pt( h, _box[index]._b_sphere );
			param_set_pt( h, _box[index]._b_inside );

			param_set_pt( h, _box[index]._b_selected );
			param_set_pt_3( h, _box[index]._out );
	}

	err_param_init_pt(h);
}

void c_bdd_boxes::alloc()
{
}

void c_bdd_boxes::dealloc()
{
}

CONSTRUCTOR_CREATE( c_bdd_boxes )
,_ui_index(-1)
,_bdd_target(nullptr)
,_nb_selected(0)
{
	if( is_obj_first() )
	{	//par	we have to duplicate the first group "by hand"
		c_param_def* psrc_start = &n_bdd_boxes::param[n_bdd_boxes::START_BOX_INDEX];
		c_param_def* psrc;
		c_param_def* pdst = psrc_start + n_bdd_boxes::ITEM_PARAM_NB+1;
		CHAR	     str[256];
		for( INT32 index=1; index<BOXES_NB_MAX; ++index )
		{
			psrc = psrc_start;
			for( INT32 i = n_bdd_boxes::ITEM_PARAM_NB+1; i>0 ; --i )
			{//hack	//todoqq cleanup
				*pdst = *psrc;
				strcpy( str, psrc->get_name());
				strnum::make( str+3, 2, index+1 );
				pdst->set_name( str );

				if( i==5 )
					pdst->set_def( REAL(index) ); ;

				++psrc;
				++pdst;
			}
		}
	}
	//init the locked param
	for( INT32 index = 0; index < BOXES_NB_MAX; ++index )
		_box[index]._b_inside = false;

	clear_v3( _ui_start );
	clear_v3( _ui_start_pos );
	param_init_with( n_bdd_boxes::param, n_bdd_boxes::PARAM_NB_MAX ); // bdd_boxes_param, BDD_BOXES_PARAM_NB_MAX);
	alloc();
}

c_bdd_boxes::~c_bdd_boxes()
{
	dealloc();
}

void	c_bdd_boxes::build_partiel( INT32 index, INT32 type )
{
	c_box_one& box = _box[index];
	
	box._b_active = true;
	box._b_enabled = true;
	box._b_sphere = false;
	box._control_id = index + 1;
	box._s_type = type;
}

void	c_bdd_boxes::build_hori()
{
	REAL	ri = 0.5;
	REAL	f = _build_bottom - _build_top;

	for( INT32 i = _start - 1; i < _stop; ++i )
	{
		if( _box[i].is_manual() )
			continue;
		build_partiel( i, c_box_one::BOX_HORIZONTAL );
		set_v3( _box[i]._pos_ui, REAL(0), ri * f/REAL(_auto_nb) + _build_top, REAL(0) );
		set_v4( _box[i]._size_ui, .4, -f*.8, 1.,  1./REAL(_auto_nb) );
		ri += 1.;
	}
	_b_hori_trig = false;
}

void	c_bdd_boxes::build_vert()
{
	REAL	ri = 0.5;
	REAL	f = _build_right-_build_left;
	REAL	cy = ( _build_top + _build_bottom ) * REAL(.5);

	for( INT32 i = _start - 1; i < _stop; ++i )
	{
		if( _box[i].is_manual() )
			continue;
		build_partiel( i, c_box_one::BOX_VERTICAL );
		set_v3( _box[i]._pos_ui, ri * f/REAL(_auto_nb) + _build_left, cy, REAL(0) );
		set_v4( _box[i]._size_ui, f*.8, .4, 1.,  1./REAL(_auto_nb) );
		ri += 1.;
	}
	_b_vert_trig = false;
}

void	c_bdd_boxes::build_finger()
{
	REAL	ri = 0.5;
	REAL	cy = ( _build_top + _build_bottom ) * REAL(.5) ;

	for( INT32 i = _start - 1; i < _stop; ++i )
	{
		if( _box[i].is_manual() )
			continue;
		build_partiel( i, c_box_one::BOX_FINGER );
		set_v3( _box[i]._pos_ui, REAL(ri/REAL(_auto_nb)-.5), cy, REAL(0) );
		set_v4( _box[i]._size_ui, .8, .8, 1.,  1./REAL(_auto_nb) );
		ri += 1.;
	}
	_b_finger_trig = false;
}

void	c_bdd_boxes::switch_to_framed()
{
	for( INT32 i = _start - 1; i < _stop; ++i )
	{
		switch( _box[i]._s_type  )
		{
		case c_box_one::BOX_VERTICAL:	_box[i]._s_type = c_box_one::BOX_VERTICAL_FRAMED;	break;
		case c_box_one::BOX_HORIZONTAL:	_box[i]._s_type = c_box_one::BOX_HORIZONTAL_FRAMED;	break;
		}
	}
	_b_frame_trig = false;
}

void	c_bdd_boxes::update()
{
	if( _b_ui_intercept_ui )
		ui_register( this );

	_viewport = c_viewport::get_cur();	//todo all the intercept bdd should have this
	_cam = c_seedcam::get_cur();

	g_draw_selected_with_line = _b_draw_selected_with_line;

	c_model::cur->get_size_v3( _size );
	scale_v3( _box_size, _box_size_ui, _box_size_ui[3] );

	if( _b_hori_trig || _b_vert_trig || _b_finger_trig || _b_frame_trig )
	{
		_auto_nb = 0;
		for( INT32 i = _start-1; i < _stop; ++i )
		{
			if( !_box[i].is_manual() )
				++_auto_nb;
		}
		if( _auto_nb )
		{
			if( _b_hori_trig )
				build_hori();
			if( _b_vert_trig )
				build_vert();
			if( _b_finger_trig )
				build_finger();
			if( _b_frame_trig )
				switch_to_framed();
		}
	}
	_nb_selected = 0;
	c_box_one**	_pt_active = _box_selected;
	for( INT32 i = _start - 1; i < _stop; ++i )
	{
		_box[i].update( _size,  _box_size );
		if( _box[i].is_active_and_enabled() && _box[i].is_selected() )
		{
			++_nb_selected;
			*_pt_active++ = &_box[i];
		}
	}
	if( !_target_name_symbo.is_empty() )
	{
		if( !_bdd_target || !_bdd_target->is_name_symbo( _target_name_symbo ) || !_bdd_target->get_root() )
			_bdd_target = (c_bdd*) find_by_class_and_name_symbo( "bdd_blob", _target_name_symbo );
	}
	else
		_bdd_target = nullptr;
	if( _bdd_target )
		((c_bdd_blob*)_bdd_target)->register_as_src( this );
}

INT32	c_bdd_boxes::get_point_nb()								{	return _nb_selected;			}
//REAL*	c_bdd::get_points()									{	return nullptr;		}
REAL*	c_bdd_boxes::get_point_pt( INT32 CONST index )
{	
	if( 0 <= index && index < _nb_selected )
		return _box_selected[index]->get_pos();
	return nullptr;
}

void	c_bdd_boxes::transfer_blobs_to( BLOBS_CONT& blobs )
{	//todoopt by avoiding copy
	REAL	sca[3];
	REAL	pos[2];		//todonow extend to 3
	c_blob	blob;
	REAL	g_size[3];
	REAL	size[3];

	scale_v3( sca, _transfer_scale_ui, _transfer_scale_ui[3] );
	c_model::cur->get_size_v3( g_size );
	mul_v3( g_size, sca );
	scale_v3( g_size, _transfer_radius_factor );
	blobs.reserve( blobs.size() + _stop - _start + 1);
	for( INT32 i = _start - 1; i < _stop; ++i )
	{
		if( _box[i].is_active() && _box[i]._b_selected )
		{
			REAL vec[2];
			cpy_v2( vec, _box[i].get_pos() );
			add_mul_v2r( pos, _transfer_translate, vec, sca );
			_cam->viewport_transform_hack( pos[0], pos[1] );

			mul_v2( size, _box[i].get_size(), g_size );
			blob.set_quick( i, pos[0], pos[1], size[0], size[1] );
			blobs.push_back(blob);
		}
	}
}

FINLINE	void	c_bdd_boxes::draw_number()
{
	if( _s_number_draw )
	{
		INT32	number;

		for( INT32 i = _start-1; i < _stop; ++i )
		{
			if( !_box[i].is_active() )
				continue;
			_box[i].set_color();

			REAL	vec[3];

			GOL::matrix::push();
				add_scale_v3( vec, _box[i].get_pos(), _box[i].get_size(), -.5 );
				add_v3( vec, _origin );
				GOL::matrix::translate3v( vec );
				switch( _s_number_draw )
				{
				case 4:
					billboard::do_y( _box[i].get_pos() );
					break;
				case 1:
					GOL::matrix::rotate_y_deg( 90. );
					break;
				case 2:
					GOL::matrix::rotate_x_deg( -90. );
					GOL::matrix::rotate_z_deg( -90. );
					break;
				}
				GOL::matrix::scale( _number_size );
				number = _box[i].get_control_id();
				if( number < 10 )
					GOL::matrix::translate( -.5, -.5, .0 );
				else if( number < 100 )
					GOL::matrix::translate( REAL(-1.1), REAL(-.5), .0 );
				else
					GOL::matrix::translate( REAL(-1.7), -.5, .0 );
				GOL::matrix::scale_x( .5 );
				aaa::alphabet::draw_int32( number, 0 );
			GOL::matrix::pop();

			GOL::matrix::push();
				scale_v3( vec, _box[i].get_size(), .5 );
				vec[0] = -vec[0];
				add_v3( vec, _box[i].get_pos(), vec );
				add_v3( vec, _origin );
				GOL::matrix::translate3v( vec );
				switch( _s_number_draw )
				{
				case 4:
					billboard::do_y( _box[i].get_pos() );
					break;
				case 1:
					GOL::matrix::rotate_y_deg( 90. );
					break;
				case 2:
					GOL::matrix::rotate_x_deg( -90. );
					GOL::matrix::rotate_z_deg( -90. );
					break;
				}
				GOL::matrix::scale( _text_size );
				GOL::matrix::scale_x( .5 );
				aaa::alphabet::draw_str( _box[i]._o_name.get(), 0 );
			GOL::matrix::pop();
		}
	}
}

void c_bdd_boxes::draw_single()
{
//INT32	axe;
//REAL	resolution;

//	axe = c_model::cur->get_axe();
//	resolution = c_model::cur->get_resolution();

	GOL::push_att();	//( GL_TEXTURE_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		GOL::push_lighting( false );
		GOL::push_depth_test( false );
		GOL::push_texture_dim( 0 );
		GOL::push_color();
		
		for( INT32 i = _start - 1; i < _stop; ++i )
		{
	//		REAL loc_size[3];
	//		REAL loc_pos[3];
	//		uv_to_xyz_v3r( loc_size, size[0], axe);
	//		uv_to_xyz_v3r( loc_pos, pos[0], axe);
			if( _box[i].is_active() )	
				_box[i].draw( _origin, _size, _s_dim );
		}
		draw_number();
		//c_color::cur->draw_with_alpha_scaled( 1. );	//hack fast and quick : no time to fuck fly
	GOL::pop_att();
}

void c_bdd_boxes::draw_multiple()
{
	//INT32	axe;
	//REAL	resolution;

	//	axe = c_model::cur->get_axe();	
	//	resolution = c_model::cur->get_resolution();
	REAL	center[3];
	INT32	nb_to_draw = _stop - _start + 1;
	c_multiple::cur->set_nb( nb_to_draw );

	for( INT32 i = _start - 1; i < _stop; ++i )
	{
		if( _box[i].is_active() )
		{
			c_multiple::cur->set_index( _box[i].get_control_id() );
			c_tex_anim::cur->bind_this( _box[i].get_control_id() );
			add_v3( center, _origin, _box[i].get_pos() );
			c_multiple::cur->align_then_draw( center, _box[i].get_size() );
		}
	}
}

INT32	c_bdd_boxes::get_index_in( REAL x, REAL y, REAL z )
{
	for( INT32 i=_start-1; i<_stop; ++i )
	{
		if( _box[i].is_active_and_enabled() )
		{
/*
			if( box[i].b_sphere)
			{
			}
			else
*/
			if( _box[i].is_in_box<3>( x, y, z ) )
				return i;
		}
	}
	return -1;
}

INT32	c_bdd_boxes::get_control_in( REAL x, REAL y, REAL z )
{
	INT32	index = get_index_in( x, y, z );
	if( index>=0 )
		return _box[index].get_control_id();
	else
		return 0;
}

bool	c_bdd_boxes::do_key( INT32 c, bool CONST b_special, INT32* modifiers, INT32* x, INT32* y )
{
	bool	b_return = false;
	if( _b_ui_intercept_ui )
	{
		//	if( modifier_is_none(*modifiers) )
		if( b_special )
		{
			b_return = true;
			switch ( c )
			{
			case 0:	//	avoid warning at compile
			default:
				b_return = false;
				break;
			}
/*			if( b_return )
			{
				CHAR	str[1024];
				sprintf( str, "do_command( \"set_point_cur( %d, %d)\" )", _i_u_cur, _i_v_cur );	
				command_send( this, str );
			}
*/
		}
	}
	return b_return;
}

//done with an horizontal focal for now
void	c_bdd_boxes::get_mouse_uv( FP32& u, FP32& v )
{
	INT32 ix,iy;
	c_mouse::get_cur()->get_xy_pixel( ix,iy );
	iy = get_render_window_sy() - iy;	//hack we need a central mecanism desktop to window

	REAL uv[2];
	if( _b_ui_crop_to_viewport )
		_viewport->convert_pix_xy_center_hori_scale_crop( uv[0], uv[1], REAL(ix), REAL(iy) );
	else
		_viewport->convert_pix_xy_center_hori_scale( uv[0], uv[1], REAL(ix), REAL(iy) );
	
	FP32 fp32_uv[3];
	fp32_uv[2]= 0;
	//printf( "avant u %f, v %f, w %f\n", uv[0], uv[1], uv[2] );
	fp32_uv[0] = uv[0] * _cam->get_ortho_sx();
	fp32_uv[1] = uv[1] * _cam->get_ortho_sx();	//todo check this
	_cam->coor_camera_to_world( uv );
	u = fp32_uv[0] - _origin[0] ;
	v = fp32_uv[1] - _origin[1] ;
	//printf( "apres � %f, v %f, w %f\n", uv[0], uv[1], uv[2] );
}

bool	c_bdd_boxes::is_mouse_use()
{
	FP32	u,v;
	get_mouse_uv( u,v );
	return get_index_in( u,v,REAL(0) ) >= 0 ;
}

bool	c_bdd_boxes::mouse_down( FP32& u, FP32& v )
{
	get_mouse_uv( u,v );	
	if( _b_verbose )
		print_string( "mouse down %lf %lf", u, v );

	_ui_index = get_index_in( u, v, 0 );
	if( _ui_index < 0 )
		return false;

	c_box_one& box = _box[_ui_index];

	bool	b  = box._b_selected;
	INT32	type = box._s_type;
	switch( type )
	{
	case c_box_one::BOX_FIXE:		b = !b;		break;
	case c_box_one::BOX_FINGER:	break;
	case c_box_one::BOX_FREE:
	case c_box_one::BOX_VERTICAL:
	case c_box_one::BOX_HORIZONTAL:
	case c_box_one::BOX_VERTICAL_FRAMED:
	case c_box_one::BOX_HORIZONTAL_FRAMED:
	default:			b = true;	break;
	}
	box._b_selected = b;
	box._b_inside = true;

	if( _b_verbose )
		print_string( "box %d touched", _ui_index );
	_ui_start[0] = u;
	_ui_start[1] = v;
	switch( type )
	{
	case c_box_one::BOX_HORIZONTAL_FRAMED:	box._pos_ui[0] = u;		break;
	case c_box_one::BOX_VERTICAL_FRAMED:	box._pos_ui[1] = v;		break;
	default:											break;
	}
	cpy_v3( _ui_start_pos, box._pos_ui );

	return true;
}

void	c_bdd_boxes::mouse_move( FP32 u_in, FP32 v_in )
{
	if( _ui_index < 0 )
		return;
	c_box_one& box = _box[_ui_index];

	FP32	u, v;
	get_mouse_uv( u, v );
	if( _b_verbose )
		print_string( "mouse move %lf %lf", u, v );

	REAL	du, dv;
	du = (u-_ui_start[0]) * OVER_ONE_AS_FP32( _size[0] );
	dv = (v-_ui_start[1]) * OVER_ONE_AS_FP32( _size[1] );

	INT32	type = box._s_type;
	switch( type )
	{
	case c_box_one::BOX_FREE:							break;
	case c_box_one::BOX_FINGER:							break;
	case c_box_one::BOX_VERTICAL_FRAMED:
	case c_box_one::BOX_VERTICAL:		du = 0.;		break;
	case c_box_one::BOX_HORIZONTAL_FRAMED:
	case c_box_one::BOX_HORIZONTAL:	dv = 0.;			break;
	case c_box_one::BOX_FIXE:			du = dv = 0.;	break;
	}
	box._pos_ui[0] = _ui_start_pos[0] + du;
	box._pos_ui[1] = _ui_start_pos[1] + dv;
}

void	c_bdd_boxes::mouse_up( FP32 u_in, FP32 v_in )
{
	if( _ui_index < 0 )
		return;
	c_box_one& box = _box[_ui_index];

	FP32 u,v;
	get_mouse_uv( u,v );
	if( _b_verbose )
		print_string( "mouse up %lf %lf", u, v );
	REAL du, dv;
	du = u-_ui_start[0];
	dv = v-_ui_start[1];

	bool	b  = box._b_selected;
	INT32	type = box._s_type;
	switch( type )
	{
	case c_box_one::BOX_FREE:
	case c_box_one::BOX_VERTICAL_FRAMED:
	case c_box_one::BOX_VERTICAL:
	case c_box_one::BOX_HORIZONTAL_FRAMED:
	case c_box_one::BOX_HORIZONTAL:		b = false;		break;
	case c_box_one::BOX_FIXE:				break;
	case c_box_one::BOX_FINGER:
		if( ABS(du) < .005 || ABS(dv) < .005 )
			b = !b;
		break;
	}
	box._b_selected = b;
	box._b_inside = false;
}
