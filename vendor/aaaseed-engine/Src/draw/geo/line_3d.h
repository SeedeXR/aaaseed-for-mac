
#ifdef AAA_LINE_3D_H
#error "LINE_3D_H included more than once."
#endif
#define AAA_LINE_3D_H 1


#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif


class c_bdd_line_3d;

class	c_line_3d
{
friend c_bdd_line_3d;
public:

private:
//	bool		_b_valid_data;
//	bool		_b_valid_curve;

//	bool		_b_compute_needed;		// recalculate lod points
//	bool		_b_just_changed;		//	

//	bool		_b_loop;


	REAL*		_controls;
	UINT32		_controls_nb;
	UINT32		_controls_alloc_nb;
	REAL*		_controls_dist_next;
	DOUBLE*		_controls_len;

	DOUBLE		_len;

private:
//			void			update_curve_valid();
//	FINLINE	bool			find_arc_index(				DOUBLE& s,	INT32& i_next );
			bool			find_index(					DOUBLE& s,	INT32& i_prev, INT32& i_next );

protected:
			void			realloc( UINT32 CONST nb, bool CONST b_clear = false );
			void			dealloc();
//			void			alloc_curve( UINT32 CONST nb );
//			void			dealloc_curve();
//			void			clear_curve();

//			void			compute_curve();
//			void			compute_points();
public:
//	FINLINE	void			compute_check();
//			void			ask_update_points()				{ _b_compute_needed = true; }
//			void			clear_update_point()			{ _b_compute_needed = false; _b_just_changed = true; }
//			bool			is_changed()					{ return _b_just_changed; }
public:
	c_line_3d();
	~c_line_3d();

//			void			update();

	FINLINE	bool			is_valid_curve()				{ return _controls_nb >= 2; }

	// set/get control points
//			void			set_control_points( REAL CONST * CONST points, UINT32 CONST nb );
public:
	FINLINE	UINT32			get_control_point_nb()			{ return _controls_nb; }

			REAL*			get_control_point( INT32 index );
			void			set_control_point( INT32 index, REAL CONST * CONST vec );
//void			set_control_point( UINT32 index, REAL CONST * CONST src );

			void			push_control_point( REAL CONST * CONST src = nullptr );
//			void			insert_control_point( UINT32 index, REAL CONST * CONST src = nullptr );
//			void			delete_control_point( UINT32 index );
			void			pop_control_point( INT32 nb_to_remove );
			void			clear();

	// get draw points, calculated with curve lod
	//		REAL*			get_point_curve( UINT32 CONST index );
	//FINLINE	REAL*			get_point_curve_pt()	{ return get_control_point_pt();	 }
	FINLINE	UINT32			get_point_curve_nb()	{ return get_control_point_nb();	 }

	//		void	set_points_nb( UINT32 nb );
	//		REAL*	get_points();				// return nb points equally spaced on the the curve, interpolation is dependent on the curve
	//FINLINE	UINT32	get_points_nb();

//			REAL*			get_point( UINT32 index );
			void			get_point(			REAL* dst,				DOUBLE s );
			void			get_tangent(		REAL* dst,				DOUBLE s );
			void			get_point_tangent(	REAL* dst, REAL* tgn,	DOUBLE s );

			DOUBLE			compute_len();

	FINLINE	DOUBLE			get_len() CONST				{ return _len; }


//			void			set_loop( bool CONST b_loop );
//	FINLINE	bool			is_loop() CONST			{ return _b_loop; }

//			void			write_in_file(	C_PCHAR_C name );
//			void			read_from_file(	C_PCHAR_C name );
};

//FINLINE	void	c_line_3d::compute_check()
//{
//	if( _b_compute_needed )
//		compute_curve();
//}

