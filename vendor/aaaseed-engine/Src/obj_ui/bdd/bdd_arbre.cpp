#include "bdd_arbre.h"
#include "infrastructure/seedfile.h"
#include "file/fname.h"
#include "file/aaa_file.h"
//#include "asc_line.h"
#include "draw/bind_img.h"
#include "ui/alphabet.h"
#include "gol.h"
#include "draw/seeddraw.h"
#include "math/billboard.h"
#include "draw/tex.h"
#include "infrastructure/viewport.h"
#include "multitouch/bdd_multitouch.h"
#include "obj_ui/com/net.h"
#include "draw/map.h"


using namespace tcl;

#define	BDD_ARBRE_HEADER  "# BDD_ARBRE "
void	BDD_ARBRE_PRINT_STRING( const CHAR* const fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	TRACKER_PRINT_STRING( BDD_ARBRE_HEADER, fmt, va_pass( args ) );
	va_end( args );
}

struct st_touch_info
{
	INT32		_id;
	vector2df	_touch_cur;
	vector2df	_touch_start;
};

class c_interact
{
private:
//	std::vector< touch_data >	_touch_start;
	std::vector<st_touch_info>	_touch_info;

//	vector2df	_touch_0_ref;
//	vector2df	_touch_1_ref;

//	REAL		_angle_t0_t1_ref;

//	REAL		u;
//	REAL		v;
//	std::vector< vector2df >	_touch_ref;
//	std::vector< vector2df >	_touch_cur;

	BOOL		_b_updated;

	void	close();
	void	init();
public:
	c_interact();
	~c_interact();
	BOOL		input_remove( INT32 id );
	BOOL		input_add( touch_data data );
	BOOL		input_update( touch_data data );
	void		update();
	INT32		get_size();
	INT32		get_id( INT32 index );
	BOOL		is_updated()	{ return _b_updated; }
	void		set_update( BOOL in )	{ _b_updated = in; }
	void		clear_data();
	rect_2d_f		get_rect()	{ return _rect_cur; }
	void		set_rect( rect_2d_f in )	{ _rect_ref = in; _rect_cur = in; }

	rect_2d_f	_rect_ref;
	rect_2d_f	_rect_cur;

};

c_interact::c_interact()
{
	init();
}

c_interact::~c_interact()
{
	close();
}

void	c_interact::init()
{
//	_scale = 1.0f;
//	_dx = .0f;
//	_dy = .0f;
//	_rotation = .0f;
	_b_updated = FALSE;
//	_center = vector2df( 0., 0. );

}

void	c_interact::close()
{
	// clear data
//	_touch_cur.clear();
//	_touch_start.clear();
//	_touch_ref.clear();
	_touch_info.clear();
}

void	c_interact::clear_data()
{
//	_scale = 1.0f;
//	_dx = .0f;
//	_dy = .0f;
//	_rotation = .0f;
	_b_updated = FALSE;
//	_center = vector2df( 0., 0. );

}

INT32	c_interact::get_size()
{
	return _touch_info.size();
}

INT32	c_interact::get_id( INT32 index )
{
	if( index < _touch_info.size() )
	{
//		return _touch_cur[index].id;
		return _touch_info[index]._id;
	}
	return 0;
}
BOOL	c_interact::input_remove( INT32 id )
{
	BOOL	b_remove = FALSE;
	std::vector< st_touch_info >::iterator	it_info;
	if( _touch_info.size() > 0 )
	{
		for( it_info = _touch_info.begin(); it_info != _touch_info.end(); it_info++ )
		{
			if( (*it_info)._id == id )
			{
				// id found, remove
				_touch_info.erase( it_info );
				_b_updated = TRUE;
				_rect_ref = _rect_cur;
				BDD_ARBRE_PRINT_STRING( "Remove id %d", id );
				b_remove = TRUE;
				break;
			}
		}
	}
	if( b_remove && _touch_info.size() == 1 )
	{
		//update start info
		_touch_info[0]._touch_start = _touch_info[0]._touch_cur;

	}
	return b_remove;
}

BOOL	c_interact::input_add( touch_data data )
{
	if( _touch_info.size() < 2 )
	{
		vector2df	tmp( data.x_filtered, data.y_filtered );
		st_touch_info	info_temp;

		info_temp._touch_cur = tmp;
		info_temp._touch_start = tmp;
		info_temp._id = data.id;
		_touch_info.push_back( info_temp );
		_touch_info[0]._touch_start = _touch_info[0]._touch_cur;
		_rect_ref = _rect_cur;
		_b_updated = TRUE;
		BDD_ARBRE_PRINT_STRING( "Adding new blob %d, size = %d", data.id, _touch_info.size() );
		return TRUE;
	}
	return FALSE;
}


BOOL	c_interact::input_update( touch_data data )
{
	// update data
	std::vector< st_touch_info>::iterator	it_cur;
	if( _touch_info.size() > 0 )
	{
		for( it_cur = _touch_info.begin(); it_cur != _touch_info.end(); it_cur++ )
		{
			if( (*it_cur)._id == data.id )
			{
				// id found, update
				(*it_cur)._touch_cur.set( data.x_filtered, data.y_filtered );
				_b_updated = TRUE;
				BDD_ARBRE_PRINT_STRING( "Updating blob %d, x = %f, y = %f", data.id, data.x_filtered, data.y_filtered );
			}
			else
				BDD_ARBRE_PRINT_STRING( "Can't update an existing blob!!!" );
		}
		//update();
		_b_updated = TRUE;
		return TRUE;
	}
	else
	{

		//			BDD_ARBRE_PRINT_STRING( "c_interact : updating touch_data for non existent blob!!!!" );
	}
//	BDD_ARBRE_PRINT_STRING( "Size touch = %d, start_touch = %d", _touch_cur.size(), _touch_start.size() );
	return FALSE;
}

void	c_interact::update()
{
	// update data
	if( _touch_info.empty() )
	{
		// no interaction
//		_dx = 0.f;
//		_dy = 0.f;
//		_scale = 1.0f;
//		_rotation = 0.0f;
		return;
	}
	if( _touch_info.size() == 1 )
	{
		// only one input
//		_dx = _touch_cur[0].x_filtered - _touch_start[0].x_filtered;
//		_dy = _touch_cur[0].y_filtered - _touch_start[0].y_filtered;
		_rect_cur.set_center( _rect_ref.get_x() + _touch_info[0]._touch_cur.get_x() - _touch_info[0]._touch_start.get_x(), _rect_ref.get_y() + _touch_info[0]._touch_cur.get_y() - _touch_info[0]._touch_start.get_y() );
		BDD_ARBRE_PRINT_STRING( "new x %f, new y %f", _rect_cur.get_x(), _rect_cur.get_y());
		BDD_ARBRE_PRINT_STRING("%f, %f, %f, %f", _touch_info[0]._touch_cur.get_x(), _touch_info[0]._touch_cur.get_y(), _touch_info[0]._touch_start.get_x(), _touch_info[0]._touch_start.get_y());
//		_rotation = .0f;
//		_scale = 1.0f;
//		_center = vector2df( 0, 0 );
	}
	else if ( _touch_info.size() == 2 )
	{
		// 2 fingers


		REAL	px, py;
		px = ( _touch_info[0]._touch_start.get_x() - _rect_ref.get_center().get_x() ) * cos( -_rect_ref.get_angle() ) - ( _touch_info[0]._touch_start.get_y() - _rect_ref.get_center().get_y() ) * sin( -_rect_ref.get_angle() ) ; 
		px /= (_rect_ref.get_scale() );

		py = ( _touch_info[0]._touch_start.get_x() - _rect_ref.get_center().get_x() ) * sin( -_rect_ref.get_angle() ) + ( _touch_info[0]._touch_start.get_y() - _rect_ref.get_center().get_y() ) * cos( -_rect_ref.get_angle() ) ; 
		py /= (_rect_ref.get_scale() );


		REAL	beta, beta_p;
		beta = (_touch_info[1]._touch_start - _touch_info[0]._touch_start ).get_angle();
		beta_p = (_touch_info[1]._touch_cur - _touch_info[0]._touch_cur ).get_angle();
		
		REAL	alpha;
		alpha = beta_p - beta + _rect_ref.get_angle();

		REAL	scale_2;
		scale_2 = (_touch_info[1]._touch_cur - _touch_info[0]._touch_cur ).get_length() / (_touch_info[1]._touch_start - _touch_info[0]._touch_start ).get_length() * _rect_ref.get_scale();
		REAL	cx, cy;

		REAL	cosa = COS_RAD(-alpha);
		REAL	sina = SIN_RAD(-alpha);

		cx = _touch_info[0]._touch_cur.get_x() - scale_2 * (  py* sina + px * cosa );
		cy = _touch_info[0]._touch_cur.get_y() + scale_2 * (  -py* cosa + px * sina );
		//if( cosa == .0f )
		//{
		//	cx = _touch_info[0]._touch_cur.get_x() - scale_2 * py;
		//	cy = _touch_info[0]._touch_cur.get_y() + scale_2 * py;
		//}
		//else if ( sina == .0f )
		//{
		//	cx = _touch_info[0]._touch_cur.get_x() - scale_2 * px;
		//	cy = _touch_info[0]._touch_cur.get_y() - scale_2 * py;
		//}
		//else
		//{
		//	cy =  ( scale_2 * ( px / cosa - py / sina ) + _touch_info[0]._touch_cur.get_y() * ( sina/cosa + cosa/sina) ) / ( sina/cosa + cosa/sina);
		//	cx =  _touch_info[0]._touch_cur.get_x() - scale_2 * px / cosa + ( _touch_info[0]._touch_cur.get_y() - cy ) * sina / cosa;
		//}

//		cy = scale_2 * ( px - py ) - _touch_info[0]._touch_cur.get_x() * ( cosa + sina ) - _touch_info[0]._touch_cur.get_y() * ( sina + cosa );
//		cx = - scale_2 * px + _touch_info[0]._touch_cur.get_x() + ( cy - _touch_info[0]._touch_cur.get_y() )* sina / cosa;

		_rect_cur.set_center( cx, cy );
		_rect_cur.set_angle( alpha );
		_rect_cur.set_scale( scale_2 );
		BDD_ARBRE_PRINT_STRING("x : %f, y : %f, angle : %f, rotation : %f", _rect_cur.get_center().get_x(), _rect_cur.get_center().get_y(), _rect_cur.get_angle(), _rect_cur.get_scale() );

		
		//REAL	p2x, p2y;
		//p2x = ( _touch_info[0]._touch_cur.get_x() - _rect_cur.get_center().get_x() ) * cos( _rect_cur.get_angle() ) - ( _touch_info[0]._touch_cur.get_y() - _rect_cur.get_center().get_y() ) * sin( _rect_cur.get_angle() ) ; 
		//p2x /= (_rect_cur.get_scale() );

		//p2y = ( _touch_info[0]._touch_cur.get_x() - _rect_cur.get_center().get_x() ) * sin( _rect_cur.get_angle() ) + ( _touch_info[0]._touch_cur.get_y() - _rect_cur.get_center().get_y() ) * cos( _rect_cur.get_angle() ) ; 
		//p2y /= (_rect_cur.get_scale() );
		
		
		
		//		REAL	u;
//		REAL	v;
//		vector2df	q_p( _touch_info[1]._touch_start - _touch_info[0]._touch_start );
//		REAL	length = vector2df( q_p ).get_length();
//
//
//		vector2df	q_p_2( _touch_info[1]._touch_cur - _touch_info[0]._touch_cur );
//		REAL	length2 = q_p_2.get_length();
//
//		
//		// new point
//		vector2df	point1;
//		point1.set( 0.5, 0. );
//		point1 *= _rect_ref.get_scale();
//		point1.rotate( _rect_ref.get_angle() );
////		point1 += _rect_ref.get_center();
//		u = vector2df( point1 - _touch_info[0]._touch_start + _rect_ref.get_center() ).dot_product( q_p ) / ( length * length );
//		v = vector2df( point1 - _touch_info[0]._touch_start + _rect_ref.get_center() ).dot_product( vector2df( -q_p.get_y(), q_p.get_x() ) ) / length;
//
//		vector2df	y;
//		y = _touch_info[0]._touch_cur - _rect_ref.get_center() + q_p_2 * u + vector2df( -q_p_2.get_y(), q_p_2.get_x()  ) * v / length2;
//		
//		y -= _rect_ref.get_center();
//		REAL	angle = y.get_angle();
//		
//		// new point
//		vector2df	point2;
//		point2.set( -0.5, 0. );
//		point2 *= _rect_ref.get_scale();
//		point2.rotate( _rect_ref.get_angle() );
//	//	point2 += _rect_ref.get_center();
//		u = vector2df( point2 - _touch_info[0]._touch_start + _rect_ref.get_center() ).dot_product( q_p ) / ( length * length );
//		v = vector2df( point2 - _touch_info[0]._touch_start + _rect_ref.get_center() ).dot_product( vector2df( -q_p.get_y(), q_p.get_x() ) ) / length;
//
//		vector2df	z;
//		z = _touch_info[0]._touch_cur -_rect_ref.get_center() + q_p_2 * u + vector2df( -q_p_2.get_y(), q_p_2.get_x()  ) *  v  / length2;
//
//		vector2df	zy_center( ( y + z ) / 2 );
//		vector2df	z_y( y - z );
//		REAL	scale;
//		scale = z_y.get_length();
////		BDD_ARBRE_PRINT_STRING("%f, %f, %f, %f", z_y.get_x(), z_y.get_y(), _rect_cur.get_x(), _rect_cur.get_y());
//		_rect_cur.set_center( _rect_ref.get_center() + zy_center );
//		_rect_cur.set_scale( scale );
//		_rect_cur.set_angle( _rect_ref.get_angle() + z_y.get_angle_trig() );
////		_rect_cur.set_scale( _rect_cur.get_scale() * length2 / length );
////		_rect_cur.set_scale( MAX( MIN( length2 / length, 6. ), 0.5 ) );
//		
////		BDD_ARBRE_PRINT_STRING("%f, %f, %f", _rect_cur.get_scale(), length2, length );
//		BDD_ARBRE_PRINT_STRING( "new x %f, new y %f, scale %f, angle %f", _rect_cur.get_x(), _rect_cur.get_y(), _rect_cur.get_scale(), _rect_cur.get_angle() );
////		BDD_ARBRE_PRINT_STRING("%f, %f, %f, %f", _touch_info[0]._touch_cur.get_x(), _touch_info[0]._touch_cur.get_y(), _touch_info[0]._touch_start.get_x(), _touch_info[0]._touch_start.get_y());
//



		//REAL	u;
		//REAL	v;
		//vector2df	q_p( _touch_info[1]._touch_start - _touch_info[0]._touch_start );
		//REAL	length = vector2df( q_p ).get_length();
		////		if( length == .0f )
		////			length = 1.0f;
		//u = vector2df( _rect_ref.get_center() - _touch_info[0]._touch_start ).dot_product( q_p ) / ( length * length );
		//v = vector2df( _rect_ref.get_center() - _touch_info[0]._touch_start ).dot_product( vector2df( -q_p.get_y(), q_p.get_x() ) ) / length;

		//vector2df x;

		//vector2df	q_p_2( _touch_info[1]._touch_cur - _touch_info[0]._touch_cur );
		//REAL	length2 = q_p_2.get_length();
		//// new center
		//x = _touch_info[0]._touch_cur + q_p_2 * u + vector2df( -q_p_2.get_y(), q_p_2.get_x()  ) * v / length2;


		//// new point
		//vector2df	point1;
		//point1.set( 0.5, 0. );
		//point1 *= _rect_ref.get_scale();
		//point1.rotate( _rect_ref.get_angle() );
		//point1 += _rect_ref.get_center();
		//u = vector2df( point1 - _touch_info[0]._touch_start ).dot_product( q_p ) / ( length * length );
		//v = vector2df( point1 - _touch_info[0]._touch_start ).dot_product( vector2df( -q_p.get_y(), q_p.get_x() ) ) / length;

		//vector2df	y;
		//y = vector2df( 1, 1 ) * u;
		//y = _touch_info[0]._touch_cur + q_p_2 * u + vector2df( -q_p_2.get_y(), q_p_2.get_x()  ) * v / length2;

		//y -= _rect_ref.get_center();
		//REAL	angle = y.get_angle();

		//// new point
		//vector2df	point2;
		//point2.set( -0.5, 0. );
		//point2 *= _rect_ref.get_scale();
		//point2.rotate( _rect_ref.get_angle() );
		//point2 += _rect_ref.get_center();
		//u = vector2df( point2 - _touch_info[0]._touch_start ).dot_product( q_p ) / ( length * length );
		//v = vector2df( point2 - _touch_info[0]._touch_start ).dot_product( vector2df( -q_p.get_y(), q_p.get_x() ) ) / length;

		//vector2df	z;
		//z = _touch_info[0]._touch_cur + q_p_2 * u + vector2df( -q_p_2.get_y(), q_p_2.get_x()  ) *  v  / length2;

		//_rect_cur.set_center( x );
		////	_rect_cur.set_angle( _rect_ref.get_angle() - angle );
		//vector2df	zy_center( ( y + z ) / 2 );
		//vector2df	z_y( y - z );
		//REAL	scale;
		//scale = z_y.get_length();
		//BDD_ARBRE_PRINT_STRING("%f, %f, %f, %f", z_y.get_x(), z_y.get_y(), _rect_cur.get_x(), _rect_cur.get_y());
		//_rect_cur.set_center( zy_center );
		//_rect_cur.set_scale( MAX( MIN( scale, 6. ), 0.5 ) );
		//_rect_cur.set_angle( z_y.get_angle() );
		////		_rect_cur.set_scale( _rect_cur.get_scale() * length2 / length );
		////		_rect_cur.set_scale( MAX( MIN( length2 / length, 6. ), 0.5 ) );

		//BDD_ARBRE_PRINT_STRING("%f, %f, %f", _rect_cur.get_scale(), length2, length );
		////		BDD_ARBRE_PRINT_STRING( "new x %f, new y %f, scale %f, angle %f", _rect_cur.get_x(), _rect_cur.get_y(), _rect_cur.get_scale(), _rect_cur.get_angle() );
		////		BDD_ARBRE_PRINT_STRING("%f, %f, %f, %f", _touch_info[0]._touch_cur.get_x(), _touch_info[0]._touch_cur.get_y(), _touch_info[0]._touch_start.get_x(), _touch_info[0]._touch_start.get_y());

		////		_rect_cur.set_angle( angle_cur - _angle_t0_t1_ref);
		////		_dx = _touch_cur[0].x_filtered - _touch_start[0].x_filtered;
		////		_dy = _touch_cur[0].y_filtered - _touch_start[0].y_filtered;
		////		_dx = _touch_cur[0].x_filtered - _touch_start[0].x_filtered + _touch_cur[1].x_filtered - _touch_start[1].x_filtered;
		////		_dy = _touch_cur[0].y_filtered - _touch_start[0].y_filtered + _touch_cur[1].y_filtered - _touch_start[1].y_filtered;
		////		vector2df	d;
		////		d = _touch_info[0]._touch_cur - _touch_info[0]._touch_start + _touch_info[1]._touch_cur - _touch_info[0]._touch_start;
		////
		////		_rect_cur.set_center( _rect_ref.get_x() + _touch_info[0]._touch_cur.get_x() - _touch_info[0]._touch_start.get_x(), _rect_ref.get_y() + _touch_info[0]._touch_cur.get_y() - _touch_info[0]._touch_start.get_y() );
		////
		//////		REAL scale;
		////		scale = vector2df( _touch_info[1]._touch_cur - _touch_info[0]._touch_cur ).get_length() /  vector2df( _touch_info[1]._touch_start - _touch_info[0]._touch_start ).get_length() * _rect_ref.get_scale();
		////		_rect_cur.set_scale( scale );
		////		_rect_cur.set_scale( scale );


		////		_rotation = .0f;
		////		_scale = c.get_length() * 1.25;
		////		_center = vector2df( 0, 0 );
	}
	else
	{
		// not supported for now
	}
	//BDD_ARBRE_PRINT_STRING( "Size = %d",_touch_cur.size() );
}

struct st_image_arbre 
{
	INT32		_image_index;
	INT32		_id_node;
	//	REAL		_rotation_start;	// memorize rotation when starting interaction
	rect_2d_f	_rect_cur;
	rect_2d_f	_rect_orig;
	rect_2d_f	_rect_ref;
	c_interact	_interaction;
	BOOL		_b_interacting;
	INT32		_nb_finger;

};


class c_node_arbre
{
public:
	c_node_arbre() : _id(0), _parent_id(0), _picto_nb(0), _show_child(TRUE) {}
	c_node_arbre( INT32 id ) : _id(id), _parent_id(0), _picto_nb(0), _show_child(TRUE) {}
	c_node_arbre( INT32 id, INT32 parent_id ) : _id(id), _parent_id(parent_id), _picto_nb(0), _show_child(TRUE) {}

	friend bool operator <(const c_node_arbre& lhs, const c_node_arbre& rhs)
	{
		return lhs._id < rhs._id;
	}

	INT32		get_id()			const	{ return _id; }
	INT32		get_parent_id()		const	{ return _parent_id; }
	INT32		get_picto_nb()		const	{ return _picto_nb; }
	rect_2d_f	get_rect()			const	{ return _rect_node; }
	bool		get_show_child()	const	{ return _show_child; }
	o_str		get_name()			const	{ return _name; }

	void		set_id( INT32 id )					{ _id = id; }
	void		set_parent_id( INT32 parent_id )	{ _parent_id = parent_id; }
	void		set_picto_nb( INT32 picto_nb )		{ _picto_nb = picto_nb; }
	void		set_rect( rect_2d_f rect)			{ _rect_node = rect; }
	void		set_show_child( bool show_child )	{ _show_child = show_child; }
	void		set_name( o_str name )				{ _name = name; }

protected:
private:
	INT32		_id;
	INT32		_parent_id;
	rect_2d_f	_rect_node;
	o_str		_name;
	INT32		_picto_nb;
	bool		_show_child;

};

struct c_node_compare
{
	bool operator()(const c_node_arbre& lhs, const c_node_arbre& rhs)
	{
		return lhs.get_id() < rhs.get_id();
	}
};

enum MODE_INTERACT_TYPE
{
	LIBRE = 0,
	EXACT,
	EDITION,
	MODE_INTERACT_MAX_NB,
};

char*	_arbre_method_str[MODE_INTERACT_MAX_NB] =
{
	"Libre",
	"Exact",
	"Edition",
};



FACTORY_CREATE_PROP_V1( c_bdd_arbre, bdd_arbre, Arbre, bdd_arbre, sub_menu="Special"; );


namespace n_bdd_arbre
{
	static	const	INT32	BASE_PARAM_NB	= 14 + BDD_BASE_PARAM_NB;
	static	const	INT32	TEXT_PARAM_NB	= 10;
	static	const	INT32	INFO_PARAM_NB	= 5;
	static	const	INT32	BRANCH_PARAM_NB = 5;
	static	const	INT32	INTERACT_PARAM_NB = 16;
	static	const	INT32	GROUP_PARAM_NB	= 4;

	static	const	INT32	PARAM_NB		=	BASE_PARAM_NB
											+	TEXT_PARAM_NB
											+	INFO_PARAM_NB
											+	BRANCH_PARAM_NB
											+	INTERACT_PARAM_NB
											+	GROUP_PARAM_NB;



	static	ST_PARAM	param[PARAM_NB] =
	{
		BDD_BASE_PARAMS
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_BOOLC_OFF( verbose )
		{	NULL,	PARAM_FILENAME,		"tree_filename",	TYPE_IO_BDD_ARBRE, 0,	0, 0,	NULL, NULL },
		ST_PARAM_BOOL_OFF( reload_arbre_trig )
		ST_PARAM_INT32_POS_ZERO( picto_image_src ) 
		ST_PARAM_INT32_POS_ZERO( image_src )
		ST_PARAM_BOOL_ON( draw_arbre )
		ST_PARAM_BOOL_ON( draw_image )
		ST_PARAM_BOOL_ON( draw_picto )
		ST_PARAM_INT32_LOCKED( nb_node )
		ST_PARAM_INT32_LOCKED( nb_image )
		{	NULL,	PARAM_REAL,		"image_scale_min",		1., 0.5,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
		{	NULL,	PARAM_REAL,		"image_scale_max",		1., 6.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
		{	NULL,	PARAM_REAL,		"picto_scale",			1., .75,	-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },

		{	NULL,	PARAM_GROUP_CLOSED,	"ZoneInfo", INFO_PARAM_NB },	
			ST_PARAM_BOOLC_ON( draw_info )
			ST_PARAM_INT32_POS_ZERO( info_src )
			{	NULL,	PARAM_REAL,		"info_size",		0., 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,		"info_position_x",	1., 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,		"info_position_y",	1., 0.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },

		{	NULL,	PARAM_GROUP_CLOSED,	"Text", TEXT_PARAM_NB },	
			ST_PARAM_BOOL_OFF( test_show )
			{	NULL,	PARAM_INT32,	"font",				0., 0.,		-1, FONT_MAX_NB-1,				NULL, NULL },
			{	NULL,	PARAM_BOOL,		"font_outline",		1., 0.,		0., 1.,							NULL, NULL },
			{	NULL,	PARAM_REAL,		"text_size",		0., 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,		"text_position_x",	0., 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,		"text_position_y",	0., 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_COLOR_RGBA(text)

		{	NULL,	PARAM_GROUP_CLOSED,	"Branch", BRANCH_PARAM_NB },	
			ST_PARAM_BOOL_ON( branch_draw )
			ST_PARAM_COLOR_RGBA( branch )

		{	NULL,	PARAM_GROUP_CLOSED,	"Interact", INTERACT_PARAM_NB },
			ST_PARAM_BOOL_OFF( interact_do )
			{	NULL,	PARAM_SYMBOLIC,		"mode",		1, 0,		0, MODE_INTERACT_MAX_NB - 1,		NULL, _arbre_method_str },
			ST_PARAM_BOOL_OFF( reset_trig )
			ST_PARAM_BOOL_OFF( one_finger )
			ST_PARAM_BOOL_OFF( no_sizing )
			ST_PARAM_BOOL_OFF( constraing_image )
			{	NULL,	PARAM_REAL,	"blob_scale_x",		1, 256,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"blob_scale_y",		1, 256,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"blob_offset_x",	1, 0,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,	"blob_offset_y",	1, 0,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_BOOL_OFF( blob_verbose )
			ST_PARAM_BOOL_OFF( fingers_draw )
			ST_PARAM_COLOR_RGBA( fingers_color )
			

	};
}



void	c_bdd_arbre::param_init_pt()
{
	INT32	h;

	h = param_init_pt_bdd_start();

	param_set_pt( h, _b_active );
	param_set_pt( h, _b_verbose );
	param_set_pt( h, _tree_filename );
	param_set_pt( h, _b_reload_arbre_trig );
	param_set_pt( h, _picto_image_src );
	param_set_pt( h, _image_src );
	param_set_pt( h, _b_draw_arbre );
	param_set_pt( h, _b_draw_image );
	param_set_pt( h, _b_draw_picto );
	param_set_pt( h, _noeud_nb );
	param_set_pt( h, _image_nb );
	param_set_pt( h, _image_scale_min );
	param_set_pt( h, _image_scale_max );
	param_set_pt( h, _picto_scale );

	h++;
	param_set_pt( h, _b_draw_info );
	param_set_pt( h, _info_src );
	param_set_pt( h, _info_size );
	param_set_pt( h, _info_pos_x );
	param_set_pt( h, _info_pos_y );


	h++;
	param_set_pt( h, _b_text_show );
	param_set_pt( h, _s_font );
	param_set_pt( h, _b_font_outline );
	param_set_pt( h, _text_size );
	param_set_pt( h, _text_pos_x );
	param_set_pt( h, _text_pos_y );
	param_set_pt_v4( h, _text_color );

	h++;
	param_set_pt( h, _b_branch_draw );
	param_set_pt_v4( h, _branch_color );

	h++;
	param_set_pt( h, _b_do_interact );
	param_set_pt( h, _s_mode_interact );
	param_set_pt( h, _b_reset_trig );
	param_set_pt( h, _b_one_finger );
	param_set_pt( h, _b_no_sizing );
	param_set_pt( h, _b_constraint_image );
	param_set_pt( h, _blob_scale_x );
	param_set_pt( h, _blob_scale_y );
	param_set_pt( h, _blob_offset_x );
	param_set_pt( h, _blob_offset_y );
	param_set_pt( h, _b_verbose_blob );
	param_set_pt( h, _b_fingers_draw );
	param_set_pt_v4( h, _fingers_color );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_arbre )
{
	_b_init = false;
	_b_loaded = false;
	_b_tree_loaded = FALSE;

	set_name( "c_bdd_arbre" );
	param_init_with( n_bdd_arbre::param, n_bdd_arbre::PARAM_NB );
	init_low();
}

c_bdd_arbre::~c_bdd_arbre()
{
	close();
}

void	c_bdd_arbre::close()
{
	if( this )
	{
		_arbre.clear();

	}
}



//todo init is a start so name it or split it
void	c_bdd_arbre::init_low()
{

	_b_info_touch = FALSE;
	_info_touch_image_index = 0;
	_noeud_nb = 0;
	_image_nb = 0;
	_arbre.clear();
	_arbre.allow_orphans( TRUE );


}

void	c_bdd_arbre::load_tree()
{
FILE*	file;

	file = c_file::FOPEN( _tree_filename.get(), "r" );
	if( !file )
	{
		ERR_PRINT_STRING( "bdd_arbre, error opening arbre file" );
	}
	else
	{
		read_arbre( file );
		c_file::FCLOSE( file );
	}

}


void	c_bdd_arbre::read_arbre( FILE* file )
{
	if( !file )
		return;

	CHAR		buf[4096];
	BOOL		b_image = FALSE;	
	_image_nb = 0;
	_noeud_nb = 0;

	_arbre.clear();
	_image_list.clear();

	while( get_next_line_no_empty( file, buf, sizeof(buf) ) > 0 )
	{
		CHAR*	token;
		 
		token = strtok( buf, ";" );
		if( strcmp( token, "-----" ) == 0 )
		{
			b_image = TRUE;
		}
		else
		{
			if( b_image )
			{
				INT32	index;
				INT32	parent;
				REAL	x, y;
				REAL	angle;
				REAL	scale;
				if( token != NULL )
					index = atoi( token );
				token = strtok( NULL, ";" );
				if( token != NULL )
					parent = atoi( token );
				token = strtok( NULL, ";" );
				if( token != NULL )
					x = atof( token );
				token = strtok( NULL, ";" );
				if( token != NULL )
					y = atof( token );
				token = strtok( NULL, ";" );
				if( token != NULL )
					angle = atof( token );
				token = strtok( NULL, ";" );
				if( token != NULL )
					scale = atof( token );

				_image_nb++;

				BDD_ARBRE_PRINT_STRING( "Adding Image %d", index );
				
				rect_2d_f		rect_tmp;
				rect_tmp.set_center( x, y );
				rect_tmp.set_angle( angle );
				rect_tmp.set_scale( scale );

				st_image_arbre	image_tmp;
				image_tmp._rect_cur = rect_tmp;
				image_tmp._rect_orig = rect_tmp;
				image_tmp._rect_ref = rect_tmp;
				image_tmp._id_node = parent;
				image_tmp._image_index = index;
				image_tmp._b_interacting = FALSE;

				_image_list.push_back( image_tmp );
			}
			else
			{
				INT32	data;
				INT32	picto;
				INT32	parent;
				CHAR	name[256];
				REAL	x, y;
				INT32	len;
				if( token != NULL )
					data = atoi( token );
				token = strtok( NULL, ";" );
				if( token != NULL )
					parent = atoi( token );
				token = strtok( NULL, ";" ); 
				if( token != NULL )
				{
					strncpy( name, token, strlen( token ) );
					len = strlen( token );
				}
				token = strtok( NULL, ";" );
				if( token != NULL )
					picto = atoi( token );
				token = strtok( NULL, ";" );
				if( token != NULL )
					x = atof( token );
				token = strtok( NULL, ";" );
				if( token != NULL )
					y = atof( token );

				BDD_ARBRE_PRINT_STRING( "Adding Node %d, %s", data, name );

				_noeud_nb++;
				c_node_arbre	node_tmp;
				rect_2d_f		rect_tmp;
				rect_tmp.set_center( x, y );
				o_str	name_tmp;
				name_tmp.set( name, len, 1 );

				node_tmp.set_id( data );
				node_tmp.set_picto_nb( picto );
				node_tmp.set_rect( rect_tmp );
				node_tmp.set_parent_id( parent );
				node_tmp.set_show_child( true );
				node_tmp.set_name( name_tmp );
				//try ton find parent node first
				arbre_unique::iterator it_find;
				//		it_find = arbre_test.find_deep( parent );
				//		if( it_find != arbre_test.end() )
				{
					// parent if find, insert new node
					arbre_unique::const_iterator it = _arbre.insert( parent, node_tmp );
				}
			}
		}
	}
	_b_tree_loaded = TRUE;
}

void	c_bdd_arbre::set_tree_filename( CONST CHAR* CONST filename )
{
	char	tmp_filename[_MAX_PATH];
	dealloc();
	fname::get_rel_name( tmp_filename, filename );
	_tree_filename.set( tmp_filename);
}

AAA_ERR	c_bdd_arbre::load_do_after( CONST CHAR* CONST filename )
{
	set_tree_filename( _tree_filename.get() );
	return AAA_OK;
}

void	c_bdd_arbre::analyze_blob_data()
{

//	if( _multitouch_cur != NULL )
//	{
//		if( _b_verbose )
//		{
//			BDD_ARBRE_PRINT_STRING( "Blob Nb nb %d\n", _multitouch_cur->_tracker.get_blobs_nb() );
//			BDD_ARBRE_PRINT_STRING( "Blob Touch nb %d\n", _multitouch_cur->_tracker._event_touch_count );
//			BDD_ARBRE_PRINT_STRING( "Blob Untouch nb %d\n", _multitouch_cur->_tracker._event_untouch_count );
//		}
//
//
//		// find intersect with image
////		REAL	ortho_size = g_camera_cur->get_ortho_size();
//
//		REAL	ortho_max_x = g_camera_cur->get_ortho_max_x();
//		REAL	ortho_max_y = g_camera_cur->get_ortho_max_y();
//
//		if( _image_list.size() > 0 )
//		{
//			std::vector<INT32>	blob_id_used;
//
//			std::vector<st_image_arbre>::iterator	it_image;
//			for( it_image = _image_list.begin(); it_image != _image_list.end(); it_image++ )
//			{
//				//untouch event, remove id from vector
//				INT32	event_untouch_count = _multitouch_cur->_tracker._event_untouch_count;
//				for( INT32 j = 0 ; j < event_untouch_count; j++ )
//				{
//					BOOL b_return = (*it_image)._interaction.input_remove( _multitouch_cur->_tracker._event_untouch_id[j] );
//					if( b_return && (*it_image)._interaction.get_size() == 1 )
//					{
//						// finger remove, memorize new ref position
//						(*it_image)._rect_ref = (*it_image)._rect_cur;
//					}
//					if( (*it_image)._interaction.get_size() == 0 )
//					{
//						if( b_return )
//							(*it_image)._rect_ref = (*it_image)._rect_cur;
//						(*it_image)._b_interacting = FALSE;
//
//						// untouch, check if image intersect node
//						unique_tree< c_node_arbre, std::less<c_node_arbre>, c_node_compare>::ordered_iterator  it;
//						it = _arbre.ordered_begin();
//						for( ; it != _arbre.ordered_end(); ++it )
//						{
//							rect_2d_f node_rect = it.node()->get()->get_rect();
//							vector2df	center = node_rect.get_center();
//							rect_2d_f	rect_tmp = (*it_image )._rect_cur;
//							if( rect_tmp.is_point_inside( center.get_x(), center.get_y() ) )
//							{
//								BDD_ARBRE_PRINT_STRING( "Image %d intersect picto %d", (*it_image)._image_index, it.node()->get()->get_id()  );
//
//								if( _s_mode_interact == LIBRE )
//								{
//									// mode libre, just center the image on the picto
//									(*it_image )._rect_cur.set_center( center );
//								//	(*it_image )._rect_cur.set_scale( 1.25f );
//								}
//								else
//								{
//									// mode exact, check to see if the image is on a valid node
//									BOOL b_exact = FALSE;
//									//it2 = it;
//									arbre_unique*	current_node;
//									current_node = it.node();
//									arbre_unique::iterator it_ins_from = current_node->find_deep( c_node_arbre((*it_image )._id_node) );
//									if( it_ins_from != _arbre.end() )
//									{
//										(*it_image )._rect_cur.set_center( center );
//										BDD_ARBRE_PRINT_STRING( "Image placed on correct node" );
//									//	(*it_image )._rect_cur.set_scale( 1.25f );
//									}
//									else
//									{
//										BDD_ARBRE_PRINT_STRING( "Image placed on incorrect node" );
//										(*it_image )._rect_cur = (*it_image)._rect_orig;
//										(*it_image )._rect_ref = (*it_image)._rect_orig;
//									}
//									break;
//									it = _arbre.ordered_end();
//
//								}
//
//
//							}
//
//						}
//
//					}
//				}
//
//				rect_2d_f	rect_tmp;
//				BOOL		b_update = FALSE;
//				rect_tmp = (*it_image )._rect_cur;
//
//				if( _b_one_finger && (*it_image)._interaction.get_size() == 1  )
//				{
//					// skip, we have already one finger
//				}
//				else
//				{
//					// touch event
//					INT32	touch_event_nb = _multitouch_cur->_tracker._event_touch_count;
//					for ( INT32 i = 0; i < touch_event_nb; i++ )
//					{
//
//						std::vector<BOOL>::iterator	it_blob_used;
//						// check to see if blob is already used
//						it_blob_used = find( blob_id_used.begin(), blob_id_used.end(), _multitouch_cur->_tracker._event_touch_id[i] );
//						if( it_blob_used == blob_id_used.end() )
//						{
//							// blob not found, analyse it
//							// get blob
//							touch_data blob;
//							_multitouch_cur->_tracker.get_blob_info_by_id( _multitouch_cur->_tracker._event_touch_id[i], &blob );
//
//							// rescale blob to correct value
//							blob.x_filtered = ( blob.x_filtered / _blob_scale_x - 0.5) * ortho_max_x * 2. + _blob_offset_x;
//							blob.y_filtered = - ( blob.y_filtered / _blob_scale_y - 0.5) * ortho_max_y * 2. + _blob_offset_y;
//							// draw finger
//							draw_finger( blob.x_filtered, blob.y_filtered );
//							if( _b_verbose_blob )
//								BDD_ARBRE_PRINT_STRING( "blob %d : x = %f, y = %f", i, blob.x_filtered, blob.y_filtered );
//
////							vector2df	center = rect_tmp.get_center();
//
//		//					BDD_ARBRE_PRINT_STRING( "rect_temp x = %f, y = %f", center.get_x(), center.get_y() );
//							if( rect_tmp.is_point_inside( blob.x_filtered, blob.y_filtered ) )
//							{
//							//	if( _b_verbose )
//								{
//									BDD_ARBRE_PRINT_STRING( "Blob %d intersect image %d", i, (*it_image)._image_index );
//		//							BDD_ARBRE_PRINT_STRING( "blob %d : x = %f, y = %f,", i, blob.x_filtered, blob.y_filtered );
//		//							BDD_ARBRE_PRINT_STRING( "rect_temp x = %f, y = %f", center.get_x(), center.get_y() );
//								}
//
//								// add blob id to used id
//								blob_id_used.push_back( blob.id );
//								// add blob to image map, only consider the 1st 2 finger
//								// memorize original position
//								(*it_image)._rect_ref = (*it_image)._rect_cur;
//								(*it_image)._b_interacting = TRUE;
//								// add blob to node interact
//								if( (*it_image)._interaction.get_size() == 0 )
//									(*it_image)._interaction.set_rect( (*it_image)._rect_cur );
//								(*it_image)._interaction.input_add( blob );
//								//b_update |= (*it_image)._interaction.touch( blob, TRUE );
//								BDD_ARBRE_PRINT_STRING( "Image %d, new blob interaction", (*it_image )._image_index );
//								
//							}
//							else
//							{
//								// update vector
//								//b_update |= (*it_image)._interaction.touch( blob );
//								(*it_image)._interaction.input_update( blob );
//							}
//						}
//						else
//						{
//							//id found, so already used by another image
//							// do nothing
//							
//						}
//					}
//				}
//
//				// see what id we have in memory and update it
//				for( INT32 i = 0; i < (*it_image)._interaction.get_size(); i ++ )
//				{
//					INT32	id = (*it_image)._interaction.get_id( i );
//					touch_data blob;
//
//					_multitouch_cur->_tracker.get_blob_info_by_id( id, &blob );
//					BDD_ARBRE_PRINT_STRING( "Updating blob %d id %d, Size %d", i, id, (*it_image)._interaction.get_size() );
//
//					// rescale blob to correct value
//					blob.x_filtered = ( blob.x_filtered / _blob_scale_x - 0.5) * ortho_max_x * 2. + _blob_offset_x;
//					blob.y_filtered = - ( blob.y_filtered / _blob_scale_y - 0.5) * ortho_max_y * 2. + _blob_offset_y;
//					// draw finger
//					draw_finger( blob.x_filtered, blob.y_filtered );
//					// update data
//					(*it_image)._interaction.input_update( blob );
//
//				}
//				if( (*it_image)._interaction.is_updated() )
//				{
//					if( (*it_image)._interaction.get_size() == 0 )
//						(*it_image)._b_interacting = FALSE;
//					//if( (*it_image)._interaction.get_size() == 2 )
//					//	BDD_ARBRE_PRINT_STRING( "Size == 2" );
//					(*it_image)._interaction.update();
//					(*it_image)._interaction.set_update( FALSE );
////					REAL	dx, dy, scale, rotation;
//				//	vector2df	center;
////					dx = (*it_image)._interaction.get_dx();
////					dy = (*it_image)._interaction.get_dy();
////					scale = (*it_image)._interaction.get_scale();
////					rotation = (*it_image)._interaction.get_rotation();
////					center = (*it_image)._interaction.get_center();
//				//	vector2df	center_rect = (*it_image )._rect_ref.get_center();
//					//REAL	x, y;
//					//x = (*it_image )._rect_ref.get_x() + dx;
//					//y = (*it_image )._rect_ref.get_y() + dy;
//					//if( _b_constraint_image )
//					//{
//					//	x = MIN( ortho_max_x, x );
//					//	x = MAX( -ortho_max_x, x );
//					//	y = MIN( ortho_max_y, y );
//					//	y = MAX( -ortho_max_y, y );
//					//}
//					//(*it_image )._rect_cur.set_center( x, y );
//					//if( !_b_no_sizing )
//					//{
//
//					//	REAL	new_scale = (*it_image )._rect_ref.get_scale() * scale;
//					//	if( (*it_image)._interaction.get_size() > 1 )
//					//	{
//					//		new_scale = MIN( MAX( new_scale, _image_scale_min ), _image_scale_max );
//					//		(*it_image )._rect_cur.set_scale( new_scale );
//					//	}
//					//}
//					(*it_image )._rect_cur = (*it_image)._interaction.get_rect();
//
////					(*it_image)._interaction.clear_data();
//					{
////						BDD_ARBRE_PRINT_STRING( "Translate dx : %f, dy : %f", dx, dy );
////						BDD_ARBRE_PRINT_STRING( "Scale : %f", (*it_image )._rect_cur.get_scale() );
////						BDD_ARBRE_PRINT_STRING( "Original Pos x = %f, y = %f", (*it_image )._rect_orig.get_center().get_x(), (*it_image )._rect_orig.get_center().get_y() );
////						BDD_ARBRE_PRINT_STRING( "Current Pos x = %f, y = %f", (*it_image )._rect_cur.get_center().get_x(), (*it_image )._rect_cur.get_center().get_y() );
//					}
//
//					// have to recalculate angle from center of rotation
//					//	(*it_image )._rect.set_angle( (*it_image)._rotation_start + rotation );
//				}
//
//				if( _b_info_touch )
//				{
//					// check to see if image has left
//					if( _info_touch_image_index == (*it_image)._image_index )
//					{
//						rect_2d_f	rect_tmp( _info_pos_x, _info_pos_y, 1., 1., .0, _info_size );
//						vector2df	center = (*it_image)._rect_cur.get_center();
//
//						if( rect_tmp.is_point_inside( center.get_x(), center.get_y() ) )
//						{
//							// still inside do nothin
//						}
//						else
//						{
//							//leaving zone
//							net->send_osc( 1, "/erasme/sensor/matable/0/leave", "", (UINT8*)"" );
//							net->osc_flush();
//							_b_info_touch = FALSE;
//							_info_touch_image_index = 0;
//						}
//					}
//				}
//				else
//				{
//					rect_2d_f	rect_tmp( _info_pos_x, _info_pos_y, 1., 1., .0, _info_size );
//					vector2df	center = (*it_image)._rect_cur.get_center();
//
//					if( rect_tmp.is_point_inside( center.get_x(), center.get_y() ) )
//					{
//						// still inside do nothin
//						CHAR	txt[256];
//						sprintf( txt, "%d", (*it_image)._image_index );
//						net->send_osc( 1, "/erasme/sensor/matable/0/enter", ",s", (UINT8*)txt );
//						net->osc_flush();
//						_b_info_touch = TRUE;
//						_info_touch_image_index = (*it_image)._image_index;
//
//					}
//
//				}
//
//
//			}
//		}
//
//
//	}
}

void	c_bdd_arbre::test_hit()
{


	rect_2d_f	rect_ref;
	rect_2d_f	rect_cur;
	vector2df	p(-0.25, 0.75 );
	vector2df	q( 0.5, 0.5 );
	vector2df	pp( -1, 0.75 );
	vector2df	qp( 1., 0.25 );
	vector2df	q_p( q-p);
	vector2df	q_p_2( qp - pp );
	// new point
	vector2df	point1;
	point1.set( 0.5, 0. );
	point1 *= rect_ref.get_scale();
	point1.rotate( rect_ref.get_angle() );
	point1 += rect_ref.get_center();
	REAL	u, v;
	u = vector2df( point1 - p ).dot_product( q_p ) / ( q_p.get_length() * q_p.get_length() );
	v = vector2df( point1 - p ).dot_product( vector2df( -q_p.get_y(), q_p.get_x() ) ) / q_p.get_length();

	vector2df	y;
	y = vector2df( 1, 1 ) * u;
	y = pp + q_p_2 * u + vector2df( -q_p_2.get_y(), q_p_2.get_x()  ) * v / q_p_2.get_length();

	// new point
	vector2df	point2;
	point2.set( -0.5, 0. );
	point2 *= rect_ref.get_scale();
	point2.rotate( rect_ref.get_angle() );
	point2 += rect_ref.get_center();
	u = vector2df( point2 - p ).dot_product( q_p ) / ( q_p.get_length() * q_p.get_length() );
	v = vector2df( point2 - p ).dot_product( vector2df( -q_p.get_y(), q_p.get_x() ) ) / q_p.get_length();

	vector2df	z;
	z = pp + q_p_2 * u + vector2df( -q_p_2.get_y(), q_p_2.get_x()  ) *  v  / q_p_2.get_length();


	//	_rect_cur.set_angle( _rect_ref.get_angle() - angle );
	REAL	scale;
	vector2df	z_y( ( z + y ) / 2 );
	scale = vector2df( y - z ).get_length();
	//BDD_ARBRE_PRINT_STRING("%f, %f, %f, %f", z_y.get_x(), z_y.get_y(), rect_cur.get_x(), rect_cur.get_y());
	rect_cur.set_center( z_y );
	rect_cur.set_scale( MAX( MIN( scale, 6. ), 0.5 ) );
	rect_cur.set_angle( - vector2df( y - z ).get_angle() );



	// untouch, check if image intersect node
	unique_tree< c_node_arbre, std::less<c_node_arbre>, c_node_compare>::pre_order_iterator	it;
	it = _arbre.pre_order_begin();
	for( ; it != _arbre.pre_order_end(); ++it )
	{
		rect_2d_f node_rect = it.node()->get()->get_rect();
		vector2df	center = node_rect.get_center();
		//BDD_ARBRE_PRINT_STRING( "%f %f", node_rect.get_x(), node_rect.get_y() );
//		rect_2d_f	rect_tmp = (*it_image )._rect_cur;
		if( node_rect.is_point_inside( 0, 3.0 ) )
		{
			BDD_ARBRE_PRINT_STRING( "Image %d intersect picto %d", 1,1  );

			if( _s_mode_interact == LIBRE )
			{
				// mode libre, just center the image on the picto
	//			(*it_image )._rect_cur.set_center( center.get_x(), center.get_y() );
				//	(*it_image )._rect_cur.set_scale( 1.25f );
			}
			else
			{
				// mode exact, check to see if the image is on a valid node
				BOOL b_exact = FALSE;
				//it2 = it;
				arbre_unique*	current_node;
				current_node = it.node();
				arbre_unique::iterator it_ins_from = current_node->find_deep( 120 );
				arbre_unique::iterator it_ins_from2 = _arbre.end();
				if( it_ins_from != current_node->end() )
				{
//					(*it_image )._rect_cur.set_center( center.get_x(), center.get_y() );
					BDD_ARBRE_PRINT_STRING( "Image placed on correct node" );
					//	(*it_image )._rect_cur.set_scale( 1.25f );
				}
				else
				{
					BDD_ARBRE_PRINT_STRING( "Image placed on incorrect node" );
//					(*it_image )._rect_cur = (*it_image)._rect_orig;
//					(*it_image )._rect_ref = (*it_image)._rect_orig;
				}
				break;
				it = _arbre.pre_order_end();

			}
		}
	}

}

void	c_bdd_arbre::analyze_blob_data_ex()
{

//	if( _multitouch_cur != NULL )
//	{
////		test_hit();
//		if( _b_verbose )
//		{
//			BDD_ARBRE_PRINT_STRING( "Blob Nb nb %d\n", _multitouch_cur->_tracker.get_blobs_nb() );
//			BDD_ARBRE_PRINT_STRING( "Blob Touch nb %d\n", _multitouch_cur->_tracker._event_touch_count );
//			BDD_ARBRE_PRINT_STRING( "Blob Untouch nb %d\n", _multitouch_cur->_tracker._event_untouch_count );
//		}
//
//		// real screen dimension
//		REAL	ortho_max_x = g_camera_cur->get_ortho_max_x();
//		REAL	ortho_max_y = g_camera_cur->get_ortho_max_y();
//
//		if( _image_list.size() > 0 )
//		{
//			std::vector<INT32>	blob_id_used;
//
//			std::vector<st_image_arbre>::iterator	it_image;
//			for( it_image = _image_list.begin(); it_image != _image_list.end(); it_image++ )
//			{
//				//untouch event, remove id from vector
//				INT32	event_untouch_count = _multitouch_cur->_tracker._event_untouch_count;
//				for( INT32 j = 0 ; j < event_untouch_count; j++ )
//				{
//					//touch_data	blob;
//					//_multitouch_cur->_tracker.get_blob_info_by_id( _multitouch_cur->_tracker._event_untouch_id[j], &blob );
//					//// rescale blob to correct value
//					//blob.x_filtered = ( blob.x_filtered / _blob_scale_x - 0.5) * ortho_max_x * 2. + _blob_offset_x;
//					//blob.y_filtered = - ( blob.y_filtered / _blob_scale_y - 0.5) * ortho_max_y * 2. + _blob_offset_y;
//
//					BOOL b_return = (*it_image)._interaction.input_remove( _multitouch_cur->_tracker._event_untouch_id[j] );
//					if( b_return && (*it_image)._interaction.get_size() == 1 )
//					{
//						// finger remove, memorize new ref position
//						(*it_image)._rect_ref = (*it_image)._rect_cur;
//						(*it_image)._interaction._rect_ref = (*it_image)._rect_cur;
//						(*it_image)._interaction._rect_cur = (*it_image)._rect_cur;
//					}
//					if( (*it_image)._interaction.get_size() == 0 )
//					{
//						if( b_return )
//						{
//							(*it_image)._rect_ref = (*it_image)._rect_cur;
//							(*it_image)._interaction._rect_ref = (*it_image)._rect_cur;
//							(*it_image)._interaction._rect_cur = (*it_image)._rect_cur;
//						
//							(*it_image)._b_interacting = FALSE;
//
//							// untouch, check if image intersect node
//							unique_tree< c_node_arbre, std::less<c_node_arbre>, c_node_compare>::pre_order_iterator  it;
//							it = _arbre.pre_order_begin();
//							for( ; it != _arbre.pre_order_end(); ++it )
//							{
//								rect_2d_f node_rect = it.node()->get()->get_rect();
//								vector2df	center = node_rect.get_center();
//								rect_2d_f	rect_tmp = (*it_image )._rect_cur;
//	//							if( rect_tmp.is_point_inside( center.get_x(), center.get_y() ) )
//								if( rect_tmp.is_point_inside( center.get_x(), center.get_y() ) )
//								{
//									BDD_ARBRE_PRINT_STRING( "Image %d intersect picto %d", (*it_image)._image_index, it.node()->get()->get_id()  );
//
//									if( _s_mode_interact == LIBRE )
//									{
//										// mode libre, just center the image on the picto
//										(*it_image )._rect_cur.set_center( center );
//										(*it_image )._rect_cur.set_scale( 1.25f );
//									//	(*it_image )._rect_cur.set_scale( 1.25f );
//									}
//									else if( _s_mode_interact == EXACT )
//									{
//										// mode exact, check to see if the image is on a valid node
//										BOOL b_exact = FALSE;
//										if( it.node()->get()->get_id() == (*it_image)._id_node )
//										{
//											(*it_image )._rect_cur.set_center( center );
//											BDD_ARBRE_PRINT_STRING( "Image placed on correct node" );
//											(*it_image )._rect_cur.set_scale( 1.25f );
//										}
//										else
//										{
//											//it2 = it;
//											arbre_unique*	current_node;
//											current_node = it.node();
//											arbre_unique::iterator it_ins_from = current_node->find_deep( (*it_image )._id_node );
//											if( it_ins_from != current_node->end() )
//											{
//												(*it_image )._rect_cur.set_center( center );
//												BDD_ARBRE_PRINT_STRING( "Image placed on correct node" );
//												(*it_image )._rect_cur.set_scale( 1.25f );
//											}
//											else
//											{
//												BDD_ARBRE_PRINT_STRING( "Image placed on incorrect node" );
//												(*it_image )._rect_cur.set_scale( 1.25f );
//												if( (*it_image )._rect_cur.get_y() > 0 )
//													(*it_image )._rect_cur.set_center( (*it_image )._rect_cur.get_center() - vector2df( .0, 0.5 ) );
//												else
//													(*it_image )._rect_cur.set_center( (*it_image )._rect_cur.get_center() - vector2df( .0, 0.5 ) );
//
//												(*it_image)._interaction._rect_cur = (*it_image)._rect_cur;
//												(*it_image)._interaction._rect_ref = (*it_image)._rect_cur;
//											//	(*it_image )._rect_ref = (*it_image)._rect_orig;
//											}
//										}
//											break;
//											it = _arbre.pre_order_end();
//																		
//									}
//							}
//						}
//					}
//				}
//				}
//
//				rect_2d_f	rect_tmp;
//				BOOL		b_update = FALSE;
//				rect_tmp = (*it_image )._rect_cur;
//
//				if( _b_one_finger && (*it_image)._interaction.get_size() == 1  )
//				{
//					// skip, we have already one finger
//				}
//				else
//				{
//					// touch event
//					INT32	touch_event_nb = _multitouch_cur->_tracker._event_touch_count;
//					for ( INT32 i = 0; i < touch_event_nb; i++ )
//					{
//						std::vector<BOOL>::iterator	it_blob_used;
//						// check to see if blob is already used
//						it_blob_used = find( blob_id_used.begin(), blob_id_used.end(), _multitouch_cur->_tracker._event_touch_id[i] );
//						if( it_blob_used == blob_id_used.end() )
//						{
//							// blob not found, analyse it
//							// get blob
//							touch_data blob;
//							_multitouch_cur->_tracker.get_blob_info_by_id( _multitouch_cur->_tracker._event_touch_id[i], &blob );
//
//							// rescale blob to correct value
//							blob.x_filtered = ( blob.x_filtered / _blob_scale_x - 0.5) * ortho_max_x * 2. + _blob_offset_x;
//							blob.y_filtered = - ( blob.y_filtered / _blob_scale_y - 0.5) * ortho_max_y * 2. + _blob_offset_y;
//
////							// draw finger
////							draw_finger( blob.x_filtered, blob.y_filtered );
//							if( _b_verbose_blob )
//								BDD_ARBRE_PRINT_STRING( "blob %d : x = %f, y = %f", i, blob.x_filtered, blob.y_filtered );
//
//							if( rect_tmp.is_point_inside( blob.x_filtered, blob.y_filtered ) )
//							{
//									if( _b_verbose )
//								{
//									BDD_ARBRE_PRINT_STRING( "Blob %d intersect image %d", i, (*it_image)._image_index );
//								}
//
//								// add blob id to used id
//								blob_id_used.push_back( blob.id );
//								// add blob to image map, only consider the 1st 2 finger
//								// memorize original position
//								(*it_image)._rect_ref = (*it_image)._rect_cur;
//								(*it_image)._b_interacting = TRUE;
//								// add blob to node interact
//							//	if( (*it_image)._interaction.get_size() == 0 )
//									(*it_image)._interaction.set_rect( (*it_image)._rect_cur );
//								(*it_image)._interaction.input_add( blob );
//								//b_update |= (*it_image)._interaction.touch( blob, TRUE );
//								BDD_ARBRE_PRINT_STRING( "Image %d, new blob interaction", (*it_image )._image_index );
//
//							}
//							//else
//							//{
//							//	// update vector
//							//	(*it_image)._interaction.touch( blob );
//							//}
//						}
//						else
//						{
//							//id found, so already used by another image
//							// do nothing
//
//						}
//					}
//				}
//
//				// see what id we have in memory and update it
//				for( INT32 i = 0; i < (*it_image)._interaction.get_size(); i ++ )
//				{
//					INT32	id = (*it_image)._interaction.get_id( i );
//					touch_data blob;
//
//					if( id != 0 )
//					{
//						_multitouch_cur->_tracker.get_blob_info_by_id( id, &blob );
//						BDD_ARBRE_PRINT_STRING( "Updating blob %d id %d, Size %d", i, id, (*it_image)._interaction.get_size() );
//
//						// rescale blob to correct value
//						blob.x_filtered = ( blob.x_filtered / _blob_scale_x - 0.5 ) * ortho_max_x * 2. + _blob_offset_x;
//						blob.y_filtered = - ( blob.y_filtered / _blob_scale_y - 0.5 ) * ortho_max_y * 2. + _blob_offset_y;
//						// draw finger
//						draw_finger( blob.x_filtered, blob.y_filtered );
//						// update data
//						(*it_image)._interaction.input_update( blob );
//					}
//
//				}
//				if( (*it_image)._interaction.is_updated() )
//				{
//					if( (*it_image)._interaction.get_size() == 0 )
//						(*it_image)._b_interacting = FALSE;
//					(*it_image)._interaction.update();
//					(*it_image)._interaction.set_update( FALSE );
//					(*it_image )._rect_cur = (*it_image)._interaction.get_rect();
//					REAL	x = (*it_image )._rect_cur.get_x();
//					REAL	y = (*it_image )._rect_cur.get_y();
//					if( _b_constraint_image )
//					{
//						x = MIN( ortho_max_x - 0.5, x );
//						x = MAX( -ortho_max_x + 0.5, x );
//						y = MIN( ortho_max_y - 0.5, y );
//						y = MAX( -ortho_max_y + 0.5, y );
//					}
//					(*it_image )._rect_cur.set_center( x, y );
//				//	(*it_image )._rect_cur.set_scale( MAX( MIN( (*it_image )._rect_cur.get_scale(), 6. ), 0.5 ) );
//					//(*it_image )._rect_cur = (*it_image)._interaction.get_rect();
//				}
//
//				if( _b_info_touch )
//				{
//					// check to see if image has left
//					if( _info_touch_image_index == (*it_image)._image_index )
//					{
//						rect_2d_f	rect_tmp( _info_pos_x, _info_pos_y, 1., 1., .0, _info_size );
//						vector2df	center = (*it_image)._rect_cur.get_center();
//
//						if( rect_tmp.is_point_inside( center.get_x(), center.get_y() ) )
//						{
//							// still inside do nothin
//						}
//						else
//						{
//							//leaving zone
//							net->send_osc( 1, "/erasme/sensor/matable/0/leave", "", (UINT8*)"" );
//							net->osc_flush();
//							_b_info_touch = FALSE;
//							_info_touch_image_index = 0;
//						}
//					}
//				}
//				else
//				{
//					rect_2d_f	rect_tmp( _info_pos_x, _info_pos_y, 1., 1., .0, _info_size );
//					vector2df	center = (*it_image)._rect_cur.get_center();
//
//					if( rect_tmp.is_point_inside( center.get_x(), center.get_y() ) )
//					{
//						// still inside do nothin
//						CHAR	txt[256];
//						sprintf( txt, "%d", (*it_image)._image_index );
//						net->send_osc( 1, "/erasme/sensor/matable/0/enter", ",s", (UINT8*)txt );
//						net->osc_flush();
//						_b_info_touch = TRUE;
//						_info_touch_image_index = (*it_image)._image_index;
//					}
//				}
//			}
//		}
//	}
}

void	c_bdd_arbre::update()
{	
	if( _b_active )
	{

		if( _s_font >= 0 )
			_font_size_cache = _text_size / font_get_size( _s_font, _b_font_outline );
	//	scale_v3r( _text_color, _text_color_ui, _text_color_ui[3] );

		if(_b_reload_arbre_trig )
		{
			_b_tree_loaded = FALSE;
			_b_reload_arbre_trig = FALSE;
		}
		if( !_b_tree_loaded && _tree_filename.is_not_empty() )
		{
			load_tree();
		}
		if( _b_tree_loaded )
		{
			if( _b_do_interact )
				analyze_blob_data_ex();
			if( _b_reset_trig )
			{
				std::vector<st_image_arbre>::iterator	it_image;

				// check to see if this obj is picked
				if( _image_list.size() > 0 )
				{
					//	BDD_ARBRE_PRINT_STRING( "draw_image()" );
					for( it_image = _image_list.begin(); it_image != _image_list.end(); it_image++ )
					{
						(*it_image)._rect_cur = (*it_image)._rect_orig;
						(*it_image)._rect_ref = (*it_image)._rect_orig;
						(*it_image)._interaction._rect_ref = (*it_image)._rect_orig;
						(*it_image)._interaction._rect_cur = (*it_image)._rect_orig;


					}
				}
				_b_reset_trig = FALSE;
			}

		}

	}
	cur = NULL;
}

void	c_bdd_arbre::draw_finger( REAL x, REAL y )
{
	if( _b_fingers_draw )
	{
		GOL::push_matrix();

		GOL::color4v( _fingers_color );
		GOL::color4( 1.0, .0, .0, 1.0 );
		//		printf( "dx %f, dy %f, x %f, y %f\n", dx, dy, center.get_x(), center.get_y() );
		GOL::begin( GL_POINTS );
			GOL::vertex3( x, y, 0.1 );
		GOL::end();
		GOL::pop_matrix();
	}

}

void	c_bdd_arbre::draw_image()
{

	// check to see if this obj is picked
	if( _image_list.size() > 0 )
	{
		std::vector<st_image_arbre>::iterator	it_image;
		REAL	z = 0.02;
	//	BDD_ARBRE_PRINT_STRING( "draw_image()" );
		for( it_image = _image_list.begin(); it_image != _image_list.end(); it_image++ )
		{
			INT32	image_index;
			image_index = (*it_image )._image_index;
			//TEXTURE
			tex_2d_switch( _image_src + image_index - 1 );
			c_map::cur->update_blending();
			 
			rect_2d_f	rect_tmp;
			rect_tmp = (*it_image )._rect_cur;

//			vector2d<REAL>	center;
//			center = rect_tmp.get_center();
			GOL::push_matrix();
			if( (*it_image )._b_interacting )
			{
				GOL::translate( rect_tmp.get_x(), rect_tmp.get_y(), z );
				z += 0.01;
			}
			else
			{
				GOL::translate( rect_tmp.get_x(), rect_tmp.get_y(), 0.01 );				
			}
			GOL::rotate_xyz( 0, 0, rect_tmp.get_angle() / PI_TIME_2 );
			GOL::scale( rect_tmp.get_scale(), rect_tmp.get_scale(), 1.0f );
			REAL	alpha = 0.75f;
			if( (*it_image )._b_interacting )
				alpha = 1.0f;

			GOL::color4( 1.0, 1.0, 1.0, alpha );
			//		printf( "dx %f, dy %f, x %f, y %f\n", dx, dy, center.get_x(), center.get_y() );
			GOL::begin( GL_QUADS );
				GOL::texcoord2( 0., .0 );
				GOL::vertex3( -0.5f, -.5f, 0. );
				GOL::texcoord2( 1., 0. );
				GOL::vertex3( .5f, -.5f, 0.);
				GOL::texcoord2( 1, 1 );
				GOL::vertex3( .5f, .5f, 0. );
				GOL::texcoord2( 0., 1. );
				GOL::vertex3( -.5f, 0.5f, 0. );
			GOL::end();
			GOL::pop_matrix();
		}
	}

}

void	c_bdd_arbre::draw_zone_info()
{
	//TEXTURE
	tex_2d_switch( _info_src );
	GOL::push_matrix();
	GOL::translate( _info_pos_x, _info_pos_y, -0.1f );
	GOL::scale( _info_size, _info_size, 1.0f );
	GOL::begin( GL_QUADS );
		GOL::texcoord2( 0., .0 );
		GOL::vertex3( -0.5f, -.5f, 0. );
		GOL::texcoord2( 1., 0. );
		GOL::vertex3( .5f, -.5f, 0.);
		GOL::texcoord2( 1, 1 );
		GOL::vertex3( .5f, .5f, 0. );
		GOL::texcoord2( 0., 1. );
		GOL::vertex3( -.5f, 0.5f, 0. );
	GOL::end();
	GOL::pop_matrix();
}

void	c_bdd_arbre::draw_picto( c_node_arbre* node )
{

	INT32	picto_nb;
	rect_2d_f	rect_temp;

	if ( node != NULL )
	{
	//	BDD_ARBRE_PRINT_STRING( "draw_picto()" );
		picto_nb = node->get_picto_nb();
		if( picto_nb == 0 )
		{
			// if image_index == 0, don't show picto
			return;
		}
		rect_temp = node->get_rect();

//		REAL dx, dy;
//		dx = rect_temp.get_width() * 0.5 * rect_temp.get_scale();
//		dy = rect_temp.get_height() * 0.5 * rect_temp.get_scale();

//		vector2d<REAL>	center;
//		center = rect_temp.get_center();

//		printf( "dx %f, dy %f, x %f, y %f\n", dx, dy, center.get_x(), center.get_y() );
		//TEXTURE
		tex_2d_switch( _picto_image_src + picto_nb - 1 );
		c_map::cur->update_blending();

		GOL::color_white4();
		GOL::push_matrix();
		GOL::translate( rect_temp.get_x(), rect_temp.get_y(), -0.001f );
		GOL::scale( _picto_scale, _picto_scale, 1.0f );
			GOL::begin( GL_QUADS );
			GOL::texcoord2( 0., .0 );
			GOL::vertex3( -0.5f, -.5f, 0. );
			GOL::texcoord2( 1., 0. );
			GOL::vertex3( .5f, -.5f, 0.);
			GOL::texcoord2( 1, 1 );
			GOL::vertex3( .5f, .5f, 0. );
			GOL::texcoord2( 0., 1. );
			GOL::vertex3( -.5f, 0.5f, 0. );
		GOL::end();
		GOL::pop_matrix();
		//GOL::push_matrix();
		//GOL::begin( GL_QUADS );
		//	GOL::texcoord2( 1., 1. );
		//	GOL::vertex3( center.get_x() + dx, center.get_y() - dy, .001 );
		//	GOL::texcoord2( .0, 1. );
		//	GOL::vertex3( center.get_x() - dx, center.get_y() - dy, .001);
		//	GOL::texcoord2( 1., 0. );
		//	GOL::vertex3( center.get_x() - dx, center.get_y() + dy, .001 );
		//	GOL::texcoord2( 0., .0 );
		//	GOL::vertex3( center.get_x() + dx, center.get_y() + dy, .001 );
		//GOL::end();
		//GOL::pop_matrix();

	}
	
}

void	c_bdd_arbre::draw_name( c_node_arbre* node )
{
	// draw name
	GOL::push_attrib( GL_LIGHTING_BIT);
	glDisable(GL_LIGHTING);
	GOL::color3v( _text_color );

	o_str	tmp_str;
	tmp_str = node->get_name();
	CONST CHAR* txt = tmp_str.get();
	if( txt == NULL )
		txt = "No Name";

//	BDD_ARBRE_PRINT_STRING( "draw_name()" );


	REAL tmp = 1.f;
	if( _s_font < 0 )
	{
		GOL::translate( _text_pos_x, _text_pos_y, .0 );
		tmp = _text_size;
		GOL::scale( tmp * .4, tmp, tmp );
		//		GOL::rotate_y_deg( -90. );
		n_alphabet::draw_str( (CHAR*)txt );
	}
	else
	{
//		REAL size_text;
//		vector2d<REAL>	v2_tmp;
		GOL::disable_texture();
		//draw object
		GOL::push_matrix();
		//GOL::translatev( pos);
		//GOL::scale( size );


//		v2_tmp = node->get_rect().get_center();
		GOL::translate( node->get_rect().get_x() + _text_pos_x, node->get_rect().get_y() + _text_pos_y , 0 );
		GOL::scale( _text_size );
//		size_text = _font_size_cache;
		font_render( (CHAR*)txt, _s_font, _b_font_outline, .5 );
		if( _b_font_outline )
		{
			font_render( (CHAR*)txt, _s_font, true, .5 );
		}
		GOL::set_texture_2D();
		GOL::pop_matrix();
	}
	GOL::pop_attrib();
}

void	c_bdd_arbre::draw_branch( rect_2d_f node, rect_2d_f parent )
{
	GOL::disable_texture();

	GOL::push_matrix();
	GOL::translate( .0f, .0f, -0.01 );
	GOL::begin( GL_LINES );		
		GOL::vertex2( parent.get_x(), parent.get_y() );
		GOL::vertex2( parent.get_x(), node.get_y() );
		GOL::vertex2( parent.get_x(), node.get_y() );
		GOL::vertex2( node.get_x(), node.get_y() );
	GOL::end();
	GOL::pop_matrix();
	GOL::set_texture_2D();

}

void	c_bdd_arbre::draw()
{

	if( _b_active )
	{
		if( _b_tree_loaded )
		{
			if( _b_draw_info )
			{
				draw_zone_info();
			}
			if( _b_draw_image )
			{
				draw_image();
			}
			if( _b_draw_arbre )
			{
				arbre_unique::pre_order_node_iterator it;
				it = _arbre.pre_order_node_begin();
				for( ; it != _arbre.pre_order_node_end(); ++it )
				{
					o_str	ostr_temp;
					ostr_temp = it->get()->get_name();
					//		printf( "Item id %d, name %s\n", it->get()->get_id(), ostr_temp.get() );
					if( _b_branch_draw && !it->is_root() )
					{
						// get parent
						arbre_unique*	parent;
						parent = it->parent();
						if( !parent->is_root() )
							draw_branch( it->get()->get_rect(), parent->get()->get_rect() );
					}
					if( _b_draw_picto )
					{
						draw_picto( it->get() );
					}
					if( _b_text_show )
					{
						draw_name( it->get() );
					}

				}
			}

		}

	}

}

void	c_bdd_arbre::draw_multiple()
{
	INT32						row;

	row = 1;
	{
	}
}



c_bdd_arbre*	c_bdd_arbre::cur				= NULL;



void			c_bdd_arbre::c_init()
{
}

void			c_bdd_arbre::c_deinit()
{
	cur->close();	//todo cur means only one we should limit to one
}


