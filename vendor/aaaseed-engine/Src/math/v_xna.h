
#ifdef AAA_V_XNA_H
#error "V_XNA_H included more than once."
#endif
#define AAA_V_XNA_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


#if AAA_USE_XNA()


//	dst == src possible
extern	FINLINE	void	interpolate_v2_xna(		FP32* CONST dst,						FP32 CONST * CONST b, FP32 CONST r );
extern	FINLINE	void	interpolate_v2_xna(		FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST r );
extern	FINLINE	void	interpolate_v3_xna(		FP32* CONST dst,						FP32 CONST * CONST b, FP32 CONST r );
extern	FINLINE	void	interpolate_v3_xna(		FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST r );
extern	FINLINE	void	interpolate_v4_xna(		FP32* CONST dst,						FP32 CONST * CONST b, FP32 CONST r );
extern	FINLINE	void	interpolate_v4_xna(		FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST r );

//DIST
extern	FINLINE	FP32	dist_squared_v2_xna(	FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	FP32	dist_v2_xna(			FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	FP32	dist_squared_v3_xna(	FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	FP32	dist_v3_xna(			FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	FP32	dist_box_v3_xna(		FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	FP32	dist_box_v3_xna(		FP32 CONST * CONST a,	FP32 CONST x, FP32 CONST y, FP32 CONST z );
//extern	FINLINE	FP32	sum_squared_v2_xna(	FP32 CONST a,			FP32 CONST b );					//todo implement and use
//extern	FINLINE	FP32	sum_squared_v3_xna(	FP32 CONST a,			FP32 CONST b, FP32 CONST c );	//todo implement and use
extern	FINLINE	FP32	norm_squared_v3_xna(	FP32 CONST * CONST a );
extern	FINLINE	FP32	norm_squared_v2_xna(	FP32 CONST * CONST a );
extern	FINLINE	FP32	norm_v3_xna(			FP32 CONST * CONST a );
extern	FINLINE	FP32	norm_v2_xna(			FP32 CONST * CONST a );
//SCALE
extern	FINLINE	void	scale_v2_xna(			FP32* CONST dst, FP32 CONST * CONST src, FP32 CONST f );
extern	FINLINE	void	scale_v2_xna(			FP32* CONST dst, 						 FP32 CONST f );
extern	FINLINE	void	scale_v3_xna(			FP32* CONST dst, FP32 CONST * CONST src, FP32 CONST f );
extern	FINLINE	void	scale_v3_xna(			FP32* CONST dst, 						 FP32 CONST f );
extern	FINLINE	void	scale_v4_xna(			FP32* CONST dst, FP32 CONST * CONST src, FP32 CONST f );
extern	FINLINE	void	scale_v4_xna(			FP32* CONST dst, 						 FP32 CONST f );
template<class T>
extern	FINLINE	void	scale_v3_v4_xna(		T* CONST dst,	 FP32 CONST * CONST src, FP32 CONST f );
//todoq add the version with an axe param

//ADD
//todoopt use pre incrementation in other functions
extern	FINLINE	void	add_scale_v3_xna(		FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST factor );
extern	FINLINE	void	add_scale_v3_xna(		FP32* CONST dst,						FP32 CONST * CONST b, FP32 CONST factor );
extern	FINLINE	void	add_scale_add_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST factor );
extern	FINLINE	void	add_then_scale_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST factor );
extern	FINLINE	void	add_mul_v3_xna(			FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST * CONST c );
extern	FINLINE	void	add_mul_v3_xna(			FP32* CONST dst,						FP32 CONST * CONST b, FP32 CONST * CONST c );
//ADD
extern	FINLINE	void	add_v3_xna(				FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	void	add_v3_xna(				FP32* CONST dst,						FP32 CONST * CONST b );
extern	FINLINE	void	add_v4_xna(				FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	void	add_v4_xna(				FP32* CONST dst,						FP32 CONST * CONST b );
//SUB
extern	FINLINE	void	sub_v3_xna(				FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	void	sub_v3_xna(				FP32* CONST dst,						FP32 CONST * CONST b );
extern	FINLINE	void	sub_v4_xna(				FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	void	sub_v4_xna(				FP32* CONST dst,						FP32 CONST * CONST b );

extern	FINLINE	void	sub_reverse_v3_xna(		FP32* CONST dst, FP32 CONST * CONST a	);
//	dst = (a-b)*factor
extern	FINLINE	void	sub_then_scale_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST factor );
//	a = (a-b)*f;
extern	FINLINE	void	sub_then_scale_v3_xna(	FP32* CONST a,							FP32 CONST * CONST b, FP32 CONST f );
//	dst += (a-b)
extern	FINLINE	void	sub_then_add_to_v3_xna( FP32* dst, FP32 CONST * a, FP32 CONST * b );
//MUL
extern	FINLINE	void	mul_v3_xna(			FP32* CONST dst, FP32 CONST * CONST a	);
extern	FINLINE	void	mul_v3_xna(			FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	void	mul_v4_xna(			FP32* CONST dst, FP32 CONST * CONST a	);
extern	FINLINE	void	mul_v4_xna(			FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b );
extern	FINLINE	void	mul_scale_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 f );
extern	FINLINE	void	mul_add_v3_xna(		FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST * CONST c );
extern	FINLINE	void	mul_add_v4_xna(		FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST * CONST b, FP32 CONST * CONST c );
extern	FINLINE	void	mul_add_v3_xna(		FP32* CONST dst,						FP32 CONST * CONST b, FP32 CONST * CONST c );
extern	FINLINE	void	mul_add_v4_xna(		FP32* CONST dst,						FP32 CONST * CONST b, FP32 CONST * CONST c );
//MIX
extern	FINLINE	void	mix_v2_xna(			FP32* CONST dst,						FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb );
extern	FINLINE	void	mix_v2_xna(			FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb );
extern	FINLINE	void	mix_v3_xna(			FP32* CONST dst,						FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb );
extern	FINLINE	void	mix_v3_xna(			FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb );
extern	FINLINE	void	mix_v4_xna(			FP32* CONST dst,						FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb );
extern	FINLINE	void	mix_v4_xna(			FP32* CONST dst, FP32 CONST * CONST a,	FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb );
//TEST
extern	FINLINE	bool	is_equal_v4_xna(	FP32 CONST * CONST a, FP32 CONST * CONST b );
extern	FINLINE	bool	is_equal_v3_xna(	FP32 CONST * CONST a, FP32 CONST * CONST b );
extern	FINLINE	bool	is_diff_v4_xna(		FP32 CONST * CONST a, FP32 CONST * CONST b );
extern	FINLINE	bool	is_diff_v3_xna(		FP32 CONST * CONST a, FP32 CONST * CONST b );
extern	FINLINE	bool	is_equal_v4_xna(	FP32 CONST * CONST a, FP32 CONST f );
extern	FINLINE	bool	is_equal_v3_xna(	FP32 CONST * CONST a, FP32 CONST f );
//COPY
extern	FINLINE	void	cpy_v4_xna(			FP32* CONST dst, FP32 CONST * CONST src );
extern	FINLINE	void	cpy_v3_xna(			FP32* CONST dst, FP32 CONST * CONST src );
extern	FINLINE	void	neg_v3_xna(			FP32* CONST dst, FP32 CONST * CONST src );
//SET
extern	FINLINE	void	set_v3_xna(			FP32* CONST dst, FP32 CONST a, FP32 CONST b, FP32 CONST c );
extern	FINLINE	void	set_v3_xna(			FP32* CONST dst, FP32 CONST a );
extern	FINLINE	void	set_v4_xna(			FP32* CONST dst, FP32 CONST a, FP32 CONST b, FP32 CONST c, FP32 CONST d );
extern	FINLINE	void	set_v4_xna(			FP32* CONST dst, FP32 CONST all );
//CLEAR
extern	FINLINE	void	clear_v3_xna(		FP32* CONST dst );
extern	FINLINE	void	clear_v4_xna(		FP32* CONST dst );
//ABS
extern	FINLINE	void	abs_v3_xna(			FP32* CONST dst, FP32 CONST * CONST src );
extern	FINLINE	void	abs_v3_xna(			FP32* CONST dst );
//NORMALIZE
//extern	FINLINE	void	normalize_v3_low_xna(			FP32* CONST dst, FP32 CONST s0, FP32 CONST s1, FP32 CONST s2, FP32 CONST size_squared );
//extern	FINLINE	void	normalize_scale_v3_low_xna(	FP32* CONST dst, FP32 CONST s0, FP32 CONST s1, FP32 CONST s2, FP32 CONST size_squared, FP32 CONST scale_factor );
extern	FINLINE	void	normalize_v3_xna(				FP32* CONST dst );
extern	FINLINE	void	normalize_v3_xna(				FP32* CONST dst, FP32 CONST * CONST src );
extern	FINLINE	void	normalize_scale_v3_xna(			FP32* CONST dst, FP32 CONST scale_factor );

//NORMALIZE
extern	FINLINE	void	limit_v3_xna(					FP32* CONST dst, FP32 CONST max );
//DOT
extern	FINLINE	FP32	dot_v3_xna(						FP32 CONST * CONST a, FP32 CONST * CONST b );
//CROSS
extern	FINLINE	void	cross_v3_xna(					FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b );
//extern	FINLINE	void	cross_z_v3_xna(				FP32* CONST dst, FP32 CONST * CONST a );
//	calculates a normalized crossproduct to v1, v2
extern	FINLINE	void	cross_normalize_v3_xna(			FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b );
//	calculates a normalized crossproduct to v1, v2
//extern	FINLINE	void	cross_normalize_y_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a );
extern	FINLINE	void	cross_normalize_scale_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST scale_factor );
//MISC
extern	FINLINE	FP32	get_dist_squared_if_in_dist_squared_v3_xna(	FP32 CONST * CONST a,						FP32 CONST dist_squared );
extern	FINLINE	FP32	get_dist_squared_if_in_dist_squared_v3_xna(	FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST dist_squared );
extern	FINLINE	bool	is_dist_squared_less_v3_xna(				FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST dist_squared );

extern	FINLINE	void	build_point_v3_xna( FP32* CONST dst, FP32 CONST * CONST o, FP32 CONST * CONST a, FP32 CONST * CONST b,							FP32 CONST u, FP32 CONST v );
extern	FINLINE	void	build_point_v3_xna( FP32* CONST dst, FP32 CONST * CONST o, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c,	FP32 CONST u, FP32 CONST v, FP32 CONST w );
extern	FINLINE	void	build_point_v3_xna( FP32* CONST dst,					   FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c,	FP32 CONST u, FP32 CONST v, FP32 CONST w );
// Catmull-Rom Curve calculations
extern	FINLINE	void	cvCatmullRom_3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d,	FP32 CONST t );
extern	FINLINE	void	cvCatmullRom_2_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d,	FP32 CONST t );

extern			void	build_normal_vectors_v3_xna(			FP32 CONST * CONST nor, FP32* CONST u, FP32* CONST v );
extern			void	build_normal_vectors_using_z_v3_xna(	FP32 CONST * CONST nor, FP32* CONST u, FP32* CONST v );
extern	CONSTEXPR void	normal_of_4_point_v3_xna(				FP32* CONST nor, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d );
extern	CONSTEXPR void	normal_cano_of_4_point_v3_xna(			FP32* CONST nor, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d );


extern	FINLINE	void	matrix_44_add_xna(		FP32* CONST dst, FP32 CONST * CONST a );
extern	FINLINE	void	matrix_43_mul_v3_xna(	FP32 CONST * CONST m, FP32* CONST dst, FP32 CONST x, FP32 CONST y, FP32 CONST z );
extern	FINLINE	void	matrix_43_mul_v3_xna(	FP32 CONST * CONST m, FP32* CONST dst );
extern	FINLINE	void	matrix_43_mul_v3_xna(	FP32 CONST * CONST m, FP32* CONST dst, FP32 CONST * CONST src );
extern	FINLINE	void	matrix44_identity_xna(	FP32* CONST dst );
extern	FINLINE	void	matrix_44_invert_xna(	FP32* CONST dst, FP32 CONST * CONST src );

extern			void	test_xna();


#else //AAA_USE_XNA()
//	-----------------------------------------------------------------------
//	Mac / arm64 fallback : AAA_USE_XNA() = 0 means there is no DirectX XNA
//	Math implementation linked in. v.cpp still calls four `*_xna` symbols
//	unconditionally (compute_normals_v3r, compute_normals_cano_v3r, plus
//	dispatch in normalize_v3r and is_dist_squared_less_v3r). Provide those
//	symbols as inline forwarders to the `_v3r` scalar variants declared
//	later in v.h. Behaviour-equivalent to the XNA path minus the SSE
//	optimisation (which does not exist on arm64 anyway).
//	-----------------------------------------------------------------------
//	Forward decls must match the qualifiers used on the canonical decls in
//	v.h (CONSTEXPR / FINLINE expand to constexpr / inline) ; mismatching
//	constexpr-ness triggers a hard error. Clang fires `-Wundefined-inline`
//	when one inline references another inline defined in a separate TU
//	(here, v.cpp) ; that warning is suppressed in aaa_compile_flags.cmake
//	because the linker resolves correctly.
extern	CONSTEXPR	void	normal_of_4_point_v3r(		REAL* nor, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d );
extern	CONSTEXPR	void	normal_cano_of_4_point_v3r(	REAL* nor, REAL CONST * a, REAL CONST * b, REAL CONST * c, REAL CONST * d );
extern	CONSTEXPR	bool	is_dist_squared_less_v3r(	REAL CONST * a,	REAL CONST * b,	REAL dist_squared );
extern	FINLINE		void	normalize_v3r(				REAL* dst );
extern	FINLINE		void	normalize_v3r(				FP32* dst, REAL CONST * src );

inline	void	normal_of_4_point_v3_xna(		FP32* CONST nor, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d )
{	normal_of_4_point_v3r(		nor, a, b, c, d ); }
inline	void	normal_cano_of_4_point_v3_xna(	FP32* CONST nor, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d )
{	normal_cano_of_4_point_v3r(	nor, a, b, c, d ); }
inline	bool	is_dist_squared_less_v3_xna(	FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST dist_squared )
{	return is_dist_squared_less_v3r( a, b, dist_squared ); }
inline	void	normalize_v3_xna(				FP32* CONST dst )
{	normalize_v3r( dst ); }
inline	void	normalize_v3_xna(				FP32* CONST dst, FP32 CONST * CONST src )
{	normalize_v3r( dst, src ); }


#endif //AAA_USE_XNA()

