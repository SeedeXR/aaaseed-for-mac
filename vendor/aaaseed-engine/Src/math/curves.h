
#ifdef AAA_CURVES_H
#error "CURVES_H included more than once."
#endif
#define AAA_CURVES_H 1


#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif

//	ok but bad for include (force use of aaa_mem)
/*
class	c_bernstein_polynomial
{
public:
	REAL*	b0;
	REAL*	b1;
	REAL*	b2;
	REAL*	b3;
	size_t	_lod;

	// number of subdivision steps between each control point of the spline3d
	c_bernstein_polynomial( size_t lod )
				{
					_lod = MAX( lod, (size_t)2 );
					b0 = new REAL[ lod ];
					b1 = new REAL[ lod ];
					b2 = new REAL[ lod ];
					b3 = new REAL[ lod ];
					REAL	t = 0;
					REAL	dt = 1.0f / (_lod - 1);
					for( size_t i = 0; i < _lod; ++i )
					{
						REAL	t1	= 1 - t;
						REAL	t12 = t1 * t1;
						REAL	t2	= t * t;
						b0[ i ] = t1 * t12;
						b1[ i ] = 3 * t * t12;
						b2[ i ] = 3 * t2 * t1;
						b3[ i ] = t * t2;
						t += dt;
					}
				}
	~c_bernstein_polynomial()
				{
					SAFE_DELETE_ARRAY( b0 );
					SAFE_DELETE_ARRAY( b1 );
					SAFE_DELETE_ARRAY( b2 );
					SAFE_DELETE_ARRAY( b3 );
				}
};
*/
class c_bdd_curve_3d;
class c_bdd_line_3d;

class	c_curve_3d
{
friend c_bdd_curve_3d;
friend c_bdd_line_3d;
public:
	static INT32 CONST	DEFAULT_LOD			=	16;
	static REAL  CONST	DEFAULT_TIGHTNESS;
	enum	CURVE3D_TYPE : INT32
	{
		CURVE_LINEAR = 0,
		CURVE_CATMULLROM,
		//CURVE_CUBIC_B_SPLINE,
		//CURVE_NURBS,
		//CURVE_BEZIER,
		CURVE_TYPE_MAX_NB,
	};

private:
	bool			_b_valid_data;
	bool			_b_valid_curve;

	bool			_b_compute_needed;		// recalculate lod points
	bool			_b_just_changed;		//	

	bool			_b_loop;


	REAL*			_controls;
	UINT32			_controls_nb;
	UINT32			_controls_alloc_nb;
	DOUBLE*			_controls_arc_len;
	INT32			_controls_arc_len_nb;


	// these are generated from the control point but not need when LINEAR
	REAL*			_points;
	UINT32			_points_nb;
	UINT32			_points_alloc_nb;
	DOUBLE*			_points_arc_len;
	INT32			_points_arc_nb;

	UINT32			_lod;

	REAL			_tightness;
	REAL			_inv_tightness;

	DOUBLE			_len;




	CURVE3D_TYPE	_s_curve_type;

private:
			void			update_curve_valid();
	FINLINE	bool			find_arc_index(				DOUBLE& s,	INT32& i_next );
			bool			find_index(					DOUBLE& s,	INT32& i_prev, INT32& i_next );

	FINLINE	void			get_point_catmull(			REAL* dst,	DOUBLE s );
	FINLINE	void			get_tangent_catmull(		REAL* dst,	DOUBLE s );
	FINLINE	void			get_point_tangent_catmull(	REAL* dst,	REAL* tgn, DOUBLE s );
	FINLINE	void			get_point_linear(			REAL* dst,	DOUBLE s );
	FINLINE	void			get_tangent_linear(			REAL* dst,	DOUBLE s );
protected:
			void			realloc( UINT32 CONST nb, bool CONST b_clear = false );
			void			dealloc();
			void			alloc_curve( UINT32 CONST nb );
			void			dealloc_curve();
			void			clear_curve();

			void			compute_curve();
			void			compute_points();
			void			compute_curve_catmull();
public:
	FINLINE	void			compute_check();
			void			ask_update_points()				{ _b_compute_needed = true; }
			void			clear_update_point()			{ _b_compute_needed = false; _b_just_changed = true; }
			bool			is_changed()					{ return _b_just_changed; }
public:
	c_curve_3d();
	~c_curve_3d();

			void			update();

	FINLINE	bool			is_valid_curve();

	// set/get control points
			void			set_control_points( REAL CONST * CONST points, UINT32 CONST nb );
private:
	FINLINE	REAL*			get_control_point_pt()			{ return _controls; }
public:
	FINLINE	UINT32			get_control_point_nb()			{ return _controls_nb; }

	FINLINE	REAL*			get_control_point( INT32 index )
			{
				if( _controls_nb <= 0 )
				{
					debug_break( "%s() no control points", __FUNCTION__ );
					return nullptr;
				}
				if( _b_loop )
					index = WRAP_ID( index, (INT32)(_controls_nb - 1) );
				else if( index < 0 || (INT32)_controls_nb <= index )
				{
					debug_break( "%s() control points index %d don't exist", __FUNCTION__, index );
					return nullptr;
				}
				return &_controls[ index*3 ];
			}
			void			set_control_point( UINT32 index, REAL CONST * CONST src );

			void			add_control_point( REAL CONST * CONST src = nullptr );
			void			insert_control_point( UINT32 index, REAL CONST * CONST src = nullptr );
			void			delete_control_point( UINT32 index );
			void			clear();

	// get draw points, calculated with curve lod
			REAL*			get_point_curve( UINT32 CONST index );
			REAL*			get_point_curve_pt();
			UINT32			get_point_curve_nb();

	//		void	set_points_nb( UINT32 nb );
	//		REAL*	get_points();				// return nb points equally spaced on the curve, interpolation is dependent on the curve
	//FINLINE	UINT32	get_points_nb();

//			REAL*			get_point( UINT32 index );
			void			get_point(			REAL* dst,				REAL CONST s );
			void			get_tangent(		REAL* dst,				REAL CONST s );
			void			get_point_tangent(	REAL* dst, REAL* tgn,	REAL CONST s );

			DOUBLE			compute_len_linear();

			void			set_lod( UINT32 CONST val );
	FINLINE	INT32			get_lod()  CONST			{ return _lod; }

//	FINLINE	REAL			get_tightness()				{ return _tightness; }
//	FINLINE	void			set_tightness( REAL val )	{ _tightness = val; }

	FINLINE	DOUBLE			get_len() CONST				{ return _len; }

			void			set_type( CURVE3D_TYPE CONST val );
	FINLINE	CURVE3D_TYPE	get_type() CONST			{ return _s_curve_type; }

			void			set_loop( bool CONST b_loop );
	FINLINE	bool			is_loop() CONST				{ return _b_loop; }

			void			write_in_file(	C_PCHAR_C name );
			void			read_from_file(	C_PCHAR_C name );
};

FINLINE	void	c_curve_3d::compute_check()
{
	if( _b_compute_needed )
		compute_curve();
}

FINLINE	bool	c_curve_3d::is_valid_curve()
{
	if( _b_valid_curve )
	{
		compute_check();
		return true;
	}
	return false;
}

