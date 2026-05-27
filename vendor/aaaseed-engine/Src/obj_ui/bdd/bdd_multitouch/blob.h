
#ifdef AAA_BLOB_H
#error "BLOB_H included more than once."
#endif
#define AAA_BLOB_H 1


#ifndef AAA_POLYLINE_H
#	include "draw/geo/polyline.h"
#endif
#ifndef AAA_TOUCHLIB_RECT2D_H
#	include "rect2d.h"
#endif

using namespace rect_2d;
using namespace vector_2d;

class	c_blob;
typedef	std::vector<c_blob>		BLOBS_CONT;

class	c_net_buf_obj;

class c_blob
{
public:
	static	void	draw_multiple( BLOBS_CONT& blobs, REAL h );
	static	void	draw_multiple( std::vector<c_blob>& blobs, REAL* tra, REAL* sca );
	static	void	draw_center( BLOBS_CONT& blobs, REAL CONST size, REAL CONST z );
	static	void	draw_bounding( BLOBS_CONT& blobs, REAL* size, REAL size_min, bool _b_ellipse, REAL z  );
	static	void	draw_bounding( const BLOBS_CONT& blobs, REAL* tra, REAL* sca, REAL size_factor, REAL size_min, bool _b_ellipse );
	static	void	draw_contour( BLOBS_CONT& blobs, bool b_as_quad, REAL z );
	static	void	draw_fiducial( BLOBS_CONT& blobs );
	static	void	get_field_at( BLOBS_CONT& blobs, REAL* pos, REAL& value, INT32& nb );
	static	void	add_field_gradient_at( REAL* grad, BLOBS_CONT& blobs, REAL* pos );

	static	void	scale_translate( BLOBS_CONT& blobs, REAL* scale, REAL* translate );

	//hack now	this is quick and dirty
	static	INT32	ellipse_seg_nb;
	static	REAL	sta_size;
	static	REAL	sta_size_min;

private:
	INT32			_id;
public:
	vector2df		_center;
	rect2df			_box;
	REAL			_area;
	REAL			_angle;

	REAL			_weight;

	INT32			_id_tag;		// for fiducal markers. 0 = regular touchpoint.
	bool			_b_size_valid;

	INT32			_point_nb;
	c_polyline_2d	_polyline;

	FINLINE	INT32	get_id() const		{	return _id;	}
	FINLINE	void	set_id( INT32 id )	{	_id = id;	}


	FINLINE	void	set_quick( INT32 id, REAL x, REAL y, REAL size )
	{
		_id = id;
		_center.set_x( x );
		_center.set_y( y );
		size *= .5;
		_box.set_bottom_left( x-size , y-size );
		_box.set_top_right( x+size , y+size );
		_angle = 0.;
		_b_size_valid = true;
	}
	FINLINE	void	set_quick( INT32 id, REAL x, REAL y, REAL sx, REAL sy, REAL angle = 0. )
	{
		_id = id;
		_center.set_x( x );
		_center.set_y( y );
		sx *= .5;
		sy *= .5;
		_box.set_bottom_left( x - sx, y - sy );
		_box.set_top_right( x + sx, y + sy );
		_angle = angle;
		_b_size_valid = true;
	}
	//hack careful with id_tag
	FINLINE	void	set( CONST c_blob &b )
	{
		_id = b._id;
		_center = b._center;
		_area = b._area;
		_box = b._box;
		_angle = b._angle;
		_weight = b._weight;
		_point_nb = b._point_nb;
		_id_tag = b._id_tag;
		_b_size_valid = b._b_size_valid;
		_polyline = b._polyline;
	}
	FINLINE	void	offset_pos( REAL dx, REAL dy )
	{
		REAL	x = _center.get_x() + dx;
		REAL	y = _center.get_y() + dy;
		_center.set( x, y );

		REAL	sx = REAL( _box.get_size_x() * .5 );
		REAL	sy = REAL( _box.get_size_y() * .5 );
		_box.set_bottom_left( x - sx, y - sy );
		_box.set_top_right( x + sx, y + sy );
	}

	c_blob( CONST c_blob &b )
	{
		set( b );
	}
	c_blob() :
	_id(0),
		_area(0),
		_angle(0),
		_weight(0),
		_point_nb(0),
		_id_tag(0),
		_b_size_valid(false)
	{}
public:
	//todo optimize
			void	draw_center( REAL CONST size, REAL CONST h ) CONST;
			void	draw_bounding( REAL size_x, REAL size_y, REAL size_min, bool b_draw_ellipse, REAL h ) CONST;
			void	draw_bounding( REAL* tra, REAL* sca, REAL size_x, REAL size_y, REAL size_min, bool b_draw_ellipse ) CONST;
			void	draw_contour( bool b_as_quad, REAL h );
			void	draw_fiducial() CONST;

			void	scale_translate( REAL* scale, REAL* translate );
	FINLINE	REAL	get_field_at( REAL* pos ) const;
	FINLINE	void	add_field_gradient_at( REAL* grad, REAL* pos );
			void	write_to_mem( c_net_buf_obj* net_buf ) CONST ;
			CONST UINT8*	read_from_mem( UINT8 CONST * pt );

	static	FINLINE	INT32	get_mem_net_size()			{	return 24;	}

	FINLINE	void	get_center( REAL* dst )	CONST
	{
		dst[0] = _center.get_x();
		dst[1] = _center.get_y();
		dst[2] = 0.;
	}
};

// a finger is more than a blob
//using namespace rect_2d;
//using namespace vector_2d;

class c_finger final : public c_blob
{
public:	//todo public is bad
	INT32			_id_tracking;

	vector2df		_delta;
	vector2df		_pos_predicted;

	REAL			_area_delta;

	bool			_b_marked_for_deletion;
	INT32			_frames_left;

	INT32			_hist_prev_index;	//easy link in history

	REAL			_x_filtered;
	REAL			_y_filtered;

	bool			_b_reused;
public:
	c_finger() :
		_hist_prev_index(-1),
		_id_tracking(-1),
		_b_marked_for_deletion(false),
		_frames_left(0),
		_x_filtered(0),
		_y_filtered(0),
		_b_reused(false)
	{}

	c_finger( CONST c_blob &b ) :
		c_blob(b),
		_hist_prev_index(-1),
		_id_tracking(-1),
		_b_marked_for_deletion(false),
		_frames_left(0),
		_x_filtered(0),
		_y_filtered(0),
		_b_reused(false)
	{}
};
