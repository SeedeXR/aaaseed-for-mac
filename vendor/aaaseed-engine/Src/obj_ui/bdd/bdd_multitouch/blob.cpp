#include "blob.h"
#include "draw/rect.h"
#include "draw/shape.h"
#include "ui/alphabet.h"
#include "draw/render.h"
#include "obj_ui/com/net_util_obj.h"
#include "draw/axe.h"

//todo should be with polyline
//todo optimize
//todo we should deal with point too
void	c_blob::draw_contour( bool b_as_quad, REAL h )
{
	GOL::matrix::translate_z( h );
	if( b_as_quad )
		_polyline.draw_quad();
	else
		_polyline.draw_line();
	GOL::matrix::translate_z( -h );
}

void	c_blob::write_to_mem( c_net_buf_obj* net_buf ) CONST
{
	net_buf->write_value( _id );
	net_buf->write_value( _center.get_x(), _center.get_y() );
	net_buf->write_value( _box.get_size_x(), _box.get_size_y(), _angle );
}

CONST UINT8*	c_blob::read_from_mem( UINT8 CONST * pt )
{
	INT32	id;
	pt = c_net::read_value( pt, &id );
	REAL	x;
	REAL	y;
	pt = c_net::read_value( pt, &x, &y );
	REAL	sx;
	REAL	sy;
	REAL	angle;
	pt = c_net::read_value( pt, &sx, &sy, &angle );

	set_quick( id, x, y, sx, sy, angle );

	return pt;
}

INT32	c_blob::ellipse_seg_nb = 24;
REAL	c_blob::sta_size = 1;
REAL	c_blob::sta_size_min = 0;

void	c_blob::draw_center( REAL CONST size, REAL CONST h ) CONST
{
	if( _b_size_valid )
	{
		REAL	pos[3];	
		pos[0] = _center.get_x();
		pos[1] = _center.get_y();
		pos[2] = h ;

		n_axe::draw_null_2d( _center.get_x(), _center.get_y(), h, size, 2 );
	}
}

//hack size_min should not be passed blob need to be changed
void	c_blob::draw_bounding( REAL* tra, REAL* sca, REAL size_x, REAL size_y, REAL size_min, bool b_draw_ellipse  ) CONST
{
	if( _b_size_valid )
	{
		REAL	pos[3];
		pos[0] = (_center.get_x() + tra[0]) * sca[0];
		pos[1] = (_center.get_y() + tra[1]) * sca[1];
		pos[2] = tra[2] * sca[2];

		REAL	size[2];
		size[0] = MAX( _box.get_size_x() * size_x, size_min );
		size[1] = MAX( _box.get_size_y() * size_y, size_min );

		if( b_draw_ellipse	)
			draw_ellipse_axe_z( pos, size[0], size[1], _angle, c_render::get_cur()->get_draw_primitive(), ellipse_seg_nb );
		else
			draw_rect_z_at( pos, size, _angle );
	}
}

//hack size_min should not be passed blob need to be changed
void	c_blob::draw_bounding( REAL size_x, REAL size_y, REAL size_min, bool b_draw_ellipse, REAL h  ) CONST
{
	if( _b_size_valid )
	{
		REAL	pos[3];	
		pos[0] = _center.get_x();
		pos[1] = _center.get_y();
		pos[2] = h ;

		REAL	size[2];
		size[0] = MAX( _box.get_size_x() * size_x, size_min );
		size[1] = MAX( _box.get_size_y() * size_y, size_min );

		if( b_draw_ellipse	)
			draw_ellipse_axe_z( pos, size[0], size[1], _angle, c_render::get_cur()->get_draw_primitive(), ellipse_seg_nb );
		else
			draw_rect_z_at( pos, size, _angle );
	}
}

void	c_blob::draw_fiducial() const
{
	CONST	REAL	text_scale = REAL(.1);
	CHAR	str[256];
	sprintf( str, "%d", _id_tag );

	GOL::matrix::push();

		GOL::matrix::translate( _center.get_x() - strlen(str) * REAL(.5) * text_scale, _center.get_y() - text_scale * REAL(.5), 0. );
		/*	GOL::scalev( size_);
		if( s_dim_ == 3)
		n_axe::draw_null_3d();
		else
		n_axe::draw_null_2d( c_model::cur->get_axe() );			
		*/

		GOL::matrix::scale( text_scale );
		/*		switch( s_draw_text_ )
		{
		case 4:							
		GOL::translatev( _origin);
		billboard::do_y( _origin);
		break;
		case 1:
		GOL::rotate_y_deg( 90. );
		break;
		case 2:
		GOL::rotate_x_deg( -90. );
		GOL::rotate_z_deg( -90. );
		break;
		}
		*/
		aaa::alphabet::draw_str( str, 0 );
	GOL::matrix::pop();
}

void	c_blob::draw_center( BLOBS_CONT& blobs, REAL CONST size, REAL CONST z )
{
	for( auto & blob : blobs )
		blob.draw_center( size, z );
}

void	c_blob::draw_bounding( const std::vector<c_blob>& blobs, REAL* tra, REAL* sca, REAL size_factor, REAL size_min, bool b_ellipse )
{
	for( auto const & blob : blobs )
		blob.draw_bounding(	tra, sca, size_factor, size_factor, size_min, b_ellipse );
}

void	c_blob::draw_bounding( std::vector<c_blob>& blobs, REAL* size, REAL size_min, bool b_ellipse, REAL z )
{
	for( auto const & blob : blobs )
		blob.draw_bounding(	size[0], size[1], size_min, b_ellipse, z );
}

void	c_blob::draw_contour( BLOBS_CONT& blobs, bool b_as_quad, REAL z )
{
	for( auto & blob : blobs )
		blob.draw_contour( b_as_quad, z );
}

void	c_blob::draw_multiple( std::vector<c_blob>& blobs, REAL h )
{	
	//INT32	i = 0;
	REAL CONST * CONST size_factor	= c_multiple::cur->get_size();
	
	/*	if( c_multiple::cur->is_align_normal() )
	{
	}
	else	*/
	{
		REAL	pos[3];
		REAL	size[3];
		REAL	size_factor_used[2];

		pos[2] = h;
		size[2] = size_factor[2];
		scale_v2( size_factor_used, size_factor, sta_size );

		for( auto const & blob : blobs )
		{
			if( blob._b_size_valid )
			{
				pos[0] = blob._center.get_x();
				pos[1] = blob._center.get_y();
				size[0] = MAX( blob._box.get_size_x() * size_factor_used[0], sta_size_min );
				size[1] = MAX( blob._box.get_size_y() * size_factor_used[1], sta_size_min );
			}
			c_multiple::cur->set_index( blob.get_id() );
			c_multiple::cur->align_then_draw( pos, size );
			//c_multiple::cur->next_index();
		}
	}
}

void	c_blob::draw_multiple( std::vector<c_blob>& blobs, REAL* tra, REAL* sca )
{	
	//INT32	i = 0;
	REAL CONST * CONST size_factor	= c_multiple::cur->get_size();
	
	/*	if( c_multiple::cur->is_align_normal() )
	{
	}
	else	*/
	{
		REAL	pos[3];
		REAL	size[3];
		REAL	size_factor_used[2];

		pos[2] = tra[2] * sca[2];
		size[2] = size_factor[2];
		mul_scale_v2( size_factor_used, size_factor, sca, sta_size );

		for( auto const & blob : blobs )
		{
			if( blob._b_size_valid )
			{
				pos[0] = (blob._center.get_x() + tra[0]) * sca[0];
				pos[1] = (blob._center.get_y() + tra[1]) * sca[1];
				size[0] = MAX( blob._box.get_size_x() * size_factor_used[0], sta_size_min );
				size[1] = MAX( blob._box.get_size_y() * size_factor_used[1], sta_size_min );
			}
			c_multiple::cur->set_index( blob.get_id() );
			c_multiple::cur->align_then_draw( pos, size );
			//c_multiple::cur->next_index();
		}
	}
}

void	c_blob::draw_fiducial( std::vector<c_blob>& blobs )
{
	for( auto const & blob : blobs )
	{
		if( blob._id_tag )
		{
			blob.draw_fiducial();
		}
	}
}

void	c_blob::scale_translate( REAL* scale, REAL* translate )
{
	_polyline.scale_translate( scale, translate );
	vector2df	sca( scale[0], scale[1] );
	vector2df	tra( translate[0], translate[1] );
	_center = tra + _center*sca;
	//	_area *= scale[0] * scale[1];
	_box.scale_translate( scale, translate );
}

void	c_blob::scale_translate( BLOBS_CONT& blobs, REAL* scale, REAL* translate )
{
	for( auto & blob : blobs )
	{
		blob.scale_translate( scale, translate );
	}
}

FINLINE	REAL	c_blob::get_field_at( REAL* pos ) const
{
	if( _box.is_point_inside( pos[0], pos[1] ) )
		return _box.get_proximity( pos[0], pos[1] );
	else
		return 0.;
}

void	c_blob::get_field_at( BLOBS_CONT& blobs, REAL* pos, REAL& value, INT32& nb )
{
	for( auto const & blob : blobs )
	{
		REAL val = blob.get_field_at( pos );
		if( val )
		{
			value += val;
			++nb;
		}
	}
}

FINLINE	void	c_blob::add_field_gradient_at( REAL* grad, REAL* pos )
{
	if( _box.is_point_inside( pos[0], pos[1] ) )
		_box.add_gradient_at( grad, pos[0], pos[1] );
}

void	c_blob::add_field_gradient_at( REAL* grad, BLOBS_CONT& blobs, REAL* pos )
{
	for( auto & blob : blobs )
	{
		blob.add_field_gradient_at( grad, pos );
	}
}

