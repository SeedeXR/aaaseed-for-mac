
#ifdef AAA_CURVE_FLAT_H
#error "CURVE_FLAT_H included more than once."
#endif
#define AAA_CURVE_FLAT_H 1


#ifndef AAA_POINTS_H
#	include "draw/geo/points.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif
#ifndef AAA_MAP_H
#	include "draw/map.h"
#endif

class	c_curve_flat
{
private:
	bool		_b_build_needed;
	REAL		_center[3];
	REAL		_normal[3];
	c_map_last	_map_last;
protected:
	REAL		_uv_center[2];
	c_points_uv	_points;
public:
	FINLINE	void	set_build_needed()			{	_b_build_needed = true; }
	FINLINE	void	clear_build_needed()		{	_b_build_needed = false; }
	FINLINE	bool	is_build_needed()	CONST	{	return _b_build_needed; }

	FINLINE REAL*	get_center()			{	return _center;	}
	FINLINE REAL*	get_normal()			{	return _normal;	}

	c_curve_flat();
	virtual	~c_curve_flat();

//			void	alloc(); 
//			void	dealloc();

			void	draw_line(	bool const b_open = false );
			void	draw_polygon();
			void	draw_tri(	bool const b_open = false );

	virtual	void	draw_normal_point(	REAL len );
	virtual	void	draw_normal_face(	REAL len );

	FINLINE	INT32	get_point_nb()			{	return _points.get_nb();				}
	FINLINE	REAL*	get_points_to_draw()	{	return _points.get_points_to_draw();	}
	FINLINE	REAL*	get_normals()			{	return _points.get_normals();			}
	FINLINE	REAL*	get_rays()				{	return get_normals();					}

	FINLINE	void	set_center( REAL CONST* a );
	FINLINE	void	set_normal( REAL* a );

	FINLINE	void	deform()				{	_points.deform();						}

			void	save_obj_file_points(	FILE* file );
			void	save_obj_file_normals(	FILE* file );

	FINLINE bool	check_map();
};

FINLINE	void	c_curve_flat::set_center( REAL CONST* a )
{
	if( is_diff_v3( _center, a ) )
	{
		cpy_v3( _center, a );
		set_build_needed();
	}
}

FINLINE	void	c_curve_flat::set_normal( REAL* a )
{
	if( is_diff_v3( _normal, a ) )
	{
		normalize_v3r( a );		//todo this propagate back to the interface in bdd_circle, this not a good practice, we should here and in other fn introdice some CONST qualifier
		if( is_diff_v3( _normal, a ) )
		{
			cpy_v3( _normal, a );
			set_build_needed();
		}
	}
}

class	c_bdd_circle;
class	c_bdd_tube_path;

class	c_circle final : public c_curve_flat
{
	friend	c_bdd_circle;
	friend	c_bdd_tube_path;
protected:
	REAL	_radius[2];
	REAL	_offset[2];


	bool	_b_spirograph;
	bool	_b_direct;

	REAL	_radius_1;
	REAL	_freq_1;
	REAL	_radius_2;
	REAL	_freq_2;
	REAL*	_sin_cos;
	REAL	_angle_offset;
	REAL	_angle_range;
	bool	_b_sym;

private:
	bool	_b_build_sin_cos_needed;

	FINLINE	void	set_build_sin_cos_needed()			{	_b_build_sin_cos_needed = true; set_build_needed(); }
	FINLINE	void	clear_build_sin_cos_needed()		{	_b_build_sin_cos_needed = false; }
	FINLINE	bool	is_build_sin_cos_needed() CONST		{	return _b_build_sin_cos_needed; }

			void	realloc_sin_cos();
			void	dealloc_sin_cos();
			void	build_sin_cos();
public:
	c_circle();
	virtual	~c_circle();

	FINLINE	void	set_offset( REAL* CONST o );
	FINLINE	void	set_radius( REAL* CONST a );
	FINLINE	void	set_point_nb( INT32 CONST nb );
	FINLINE	void	set_direct( bool CONST in );
	FINLINE	void	set_angle_offset( REAL CONST in );
	FINLINE	void	set_angle_range( REAL CONST in );
	FINLINE	void	set_sym( bool CONST b );

	FINLINE	void	build_low();
			void	update();
};

FINLINE	void	c_circle::set_offset( REAL* o )
{
	if( is_diff_v2( _offset, o ) )
	{
		cpy_v2( _offset, o );
		set_build_needed();
	}
}

FINLINE	void	c_circle::set_radius( REAL* a )
{
	if( is_diff_v2( _radius, a ) )
	{
		cpy_v2( _radius, a );
		set_build_needed();
	}
}

FINLINE	void	c_circle::set_point_nb( INT32 nb )
{
	if( _points.set_nb(nb) )
		realloc_sin_cos();
}

FINLINE	void	c_circle::set_direct( bool in )
{
	if( _b_direct != in )
	{
		_b_direct = in;
		set_build_needed();
	}
}

FINLINE	void	c_circle::set_angle_offset( REAL in )
{
	if( _angle_offset != in )
	{
		_angle_offset = in;
		set_build_sin_cos_needed();
	}
}

FINLINE	void	c_circle::set_angle_range( REAL in )
{
	if( _angle_range != in )
	{
		_angle_range = in;
		set_build_sin_cos_needed();
	}
}

FINLINE	void	c_circle::set_sym( bool b )
{
	if( _b_sym != b )
	{
		_b_sym = b;
		set_build_sin_cos_needed();
	}

}


