
#ifdef AAA_POINTS_H
#error "POINTS_H included more than once."
#endif
#define AAA_POINTS_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_OBJ_H
#	include		"infrastructure/obj/obj.h"
#endif

class	c_points_1d final : public c_obj
{
private:
	UINT32	_nb;
	REAL*	_points;
protected:
	virtual	bool	alloc( UINT32 nb ); 
	virtual	void	dealloc();
public:
	c_points_1d();
	virtual	~c_points_1d();

	FINLINE	bool	set_nb( UINT32 nb )	//	return true when reallocation done
	{
		if( nb<1 )	//todo was there to make sure there is at least a triangle I guess
			nb = 1;
		if( _nb != nb )
			return alloc( nb );
		return false;
	}
	FINLINE	UINT32	get_nb()								CONST				{	return _nb;	}

	FINLINE	bool	is_index_valid(	UINT32 CONST index )	CONST				{	return index<_nb;	}
	FINLINE	REAL*	get_points()												{	return _points;				}
	FINLINE	REAL*	get_point_pt(	UINT32 CONST index )						{	return is_index_valid(index) ? &_points[index] : nullptr;		}
	FINLINE	REAL	get_point(		UINT32 CONST index )	CONST				{	return is_index_valid(index) ? _points[index] : REAL(0.);	}
	FINLINE	REAL	set_point(		UINT32 CONST index, REAL CONST v  )			{	if( is_index_valid(index) ) *get_point_pt(index) = v;		}
			REAL	remap(			REAL u );

			void	write_float_in_file(	C_PCHAR_C name );
			void	read_float_from_file(	C_PCHAR_C name );
};

class	c_points_3d : public c_obj
{
private:
	UINT32	_nb;
	UINT32	_nb_alloc;
	REAL*	_points;
	REAL*	_points_def;
	REAL*	_normals;
	REAL*	_points_draw;
protected:
	virtual	bool	alloc( UINT32 nb );
	virtual	void	dealloc();
public:
	c_points_3d();
	virtual	~c_points_3d();

	FINLINE	bool	set_nb( UINT32 nb )	//	return true when reallocation done
	{
		if( nb<3 )	//todo was there to make sure there is at least a triangle I guess
			nb = 3;
		if( _nb != nb )
			return alloc( nb );
		return false;
	}
	FINLINE	UINT32	get_nb()	CONST							{	return _nb;	}

	FINLINE	REAL*	get_points()								{	return _points;					}
	FINLINE	REAL*	get_points(			INT32 CONST index	)	{	return _points + index*3;		}
	FINLINE	REAL*	get_points_deformed()						{	return _points_def;				}
	FINLINE	REAL*	get_points_deformed(INT32 CONST index	)	{	return _points_def + index*3;	}
	FINLINE	REAL*	get_points_to_draw()						{	return _points_draw;			}
	FINLINE	REAL*	get_points_to_draw(	INT32 CONST index	)	{	return _points_draw + index*3;	}

	FINLINE	REAL*	get_normals()								{	return _normals;	}

	void	deform();

	void	save_obj_file_points(		FILE* CONST file );
};

class	c_points_uv final : public c_points_3d
{
private:
	REAL*	_uv;
protected:
	virtual	bool	alloc( UINT32 nb ); 
	virtual	void	dealloc();
public:
	c_points_uv();
	virtual	~c_points_uv();

	FINLINE	REAL*	get_uv()	{	return _uv;	}
};

class	c_grid_point3d final : public c_obj
{
private:
	INT32	_nb_u;
	INT32	_nb_v;
	REAL*	_points;
	UINT32	_nb_alloc;
protected:
	virtual	bool	alloc(	UINT32 nb_u, UINT32 nb_v, bool b_clear = true ); 
public:
	c_grid_point3d();
	virtual	~c_grid_point3d();

	FINLINE	bool	set_nb(	UINT32 nb_u, UINT32 nb_v, bool b_clear = true )	//	return true when reallocation done
	{
		if( _nb_u != nb_u || _nb_v != nb_v )
			return alloc( nb_u, nb_v, b_clear );
		return false;
	}
	virtual	void	dealloc();

	FINLINE	INT32	get_nb_u() CONST								{	return _nb_u;	}
	FINLINE	INT32	get_nb_v() CONST								{	return _nb_v;	}
	FINLINE	INT32	get_nb() CONST									{	return _nb_u*_nb_v;	}

	FINLINE	REAL*	get_points()									{	return _points;	}
	FINLINE	REAL*	get_point_direct( UINT32 CONST iu, UINT32 CONST iv )	CONST
	{
		return _points + (iv * _nb_u + iu) * 3;
	}
			REAL*	get_point( UINT32 CONST iu, UINT32 CONST iv )	CONST;
	FINLINE	void	set_point( UINT32 CONST iu, UINT32 CONST iv, REAL CONST * CONST vec )
	{	
		auto pt = get_point( iu, iv );
		if(pt)
			cpy_v3( pt, vec );
	} 
			void	copy_from(	 c_grid_point3d CONST * CONST src	);

			bool	get_uv_from_pos(			REAL& u, REAL& v, REAL CONST * CONST vec );
			void	write_float_in_file(		C_PCHAR_C name );
			void	read_float_from_file(		C_PCHAR_C name );
			void	curve_read_float_from_file( C_PCHAR_C name );

		//	void	save_obj_file_points( FILE* file );
};


class	c_point3d_marker final : public c_obj
{
private:
	INT32	_nb_u;
	INT32	_nb_v;
	REAL*	_points;
	o_str*	_text;
	UINT32	_nb_alloc;
protected:
	virtual	bool	alloc( UINT32 nb_u, UINT32 nb_v, bool b_clear = true ); 
public:
	c_point3d_marker();
	virtual	~c_point3d_marker();

	FINLINE	bool	set_nb( UINT32 nb_u, UINT32 nb_v, bool CONST b_clear = true )	//	return true when reallocation done
	{
		if( _nb_u != nb_u || _nb_v != nb_v )
			return alloc( nb_u, nb_v, b_clear );
		return false;
	}
	virtual	void	dealloc();

	FINLINE	INT32	get_nb_u()		CONST							{	return _nb_u;	}
	FINLINE	INT32	get_nb_v()		CONST							{	return _nb_v;	}
	FINLINE	INT32	get_nb()		CONST							{	return _nb_u*_nb_v;	}

//			void	set_text(	UINT32 iu, UINT32 iv, CHAR* text );
	FINLINE	REAL*	get_points()									{	return _points;	}
	FINLINE	REAL*	get_point(	UINT32 CONST iu, UINT32 CONST iv )	{	return &_points[(iv*_nb_u+iu)*3];	}
	FINLINE	void	set_point(	UINT32 CONST iu, UINT32 CONST iv, REAL CONST * CONST vec )
																	{	cpy_v3( get_point( iu, iv ), vec );	}

//	bool	get_uv_from_pos(			REAL& u, REAL& v, REAL CONST * CONST vec );
	void	write_marker_in_file(		C_PCHAR_C name );
	void	read_marker_from_file(		C_PCHAR_C name );
	//void	curve_read_float_from_file(	C_PCHAR_C name );

	//	void	save_obj_file_points(	FILE* file );
};

