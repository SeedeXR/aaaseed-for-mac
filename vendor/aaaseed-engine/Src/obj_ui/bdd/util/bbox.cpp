#include "obj_ui/bdd/util/bbox.h"
#include "draw/box.h"

extern	INT32	s_bbox_force;

void	c_bbox::draw()
{
	if( s_bbox_force < 3 && s_bbox_force != 0 )
		draw_cube_at( _radius * FP32(2), _origin );
	else
		draw_box_sxyz_at( _size_half[0]*FP32(2), _size_half[1]*FP32(2), _size_half[2]*FP32(2), _origin );
}

void	c_bbox::compute_radius_from_half()
{
	_radius = norm_v3r( _size_half );
}