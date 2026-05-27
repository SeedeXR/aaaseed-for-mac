#include "math_lua.h"
#include "noisturb.h"
#include "language/lua/aaalua_glue.h"
#include "math/gainbias.h"
#include "math/v.h"

namespace aaalua
{
namespace n_math
{
// NOISE 1D
//
	template< INT32 which >
	int	get_noise1d( c_lua_state& l )
	{
		l.check_arg_nb( 1 );
		REAL val = l.get_real( 1 );

		switch( which )
		{
		case 1:	val = noise1( val );	break;
		}
		return l.return_real( val );
	}
	AAALUACALL( get_noise1 )				{	LUAAAA_START( L, __FUNCTION__ );	return get_noise1d<1>( l );	}

// NOISE 2D
//
	template< INT32 which >
	int	get_noise2d( c_lua_state& l )
	{
		l.check_arg_nb( 2 );
		REAL			vec[2];
		l.get_v2( vec, 1 );

		REAL val;
		switch( which )
		{
		case 1:	val = noise2( vec );	break;
		}
		return l.return_real( val );
	}
	AAALUACALL( get_noise2 )				{	LUAAAA_START( L, __FUNCTION__ );	return get_noise2d<1>( l );	}

// NOISE 3D
//
	template< INT32 which >
	int	get_noise3d( c_lua_state& l )
	{
		l.check_arg_nb( 3 );
		REAL			vec[3];
		l.get_v3( vec, 1 );

		REAL val;
		switch( which )
		{
		case 1:	val = noise3(				vec );	break;
		case 2:	val = noise3_by_perlin(		vec );	break;
		case 3:	val = noise_improved3(		vec );	break;
	//	case 4:	val = turbulence_by_perlin(	vec );	break;
		}
		return l.return_real( val );
	}
	AAALUACALL( get_noise3 )				{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d<1>( l );	}
	AAALUACALL( get_noise3_by_perlin )		{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d<2>( l );	}
	AAALUACALL( get_noise3_improved )		{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d<3>( l );	}
	//AAALUACALL( get_turbulence3_by_perlin )	{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d<4>( l );	}

// NOISE 2D WITH Harmonics
//
	//		value = aaa.math.get_turbulence( x,y,z, freq, nb_harm )
	template< INT32 which >
	int	get_noise3d_with_harmonics( c_lua_state& l )
	{
		INT32	CONST	nb_arg	=	l.get_arg_nb_min_max( 3, 5 );
		REAL			vec[3];
		l.get_v3( vec, 1 );
		REAL	CONST	freq	=	(nb_arg > 3) ? l.get_real( 4 )  : REAL(1.);
		INT32	CONST	harm_nb	=	(nb_arg > 4) ? l.get_int32( 5 ) : 1;

		REAL val;
		scale_v3( vec, freq );
		switch( which )
		{
		case 1:	val = turbulence(			vec, harm_nb );	break;
		case 2:	val = fractalsum(			vec, harm_nb );	break;
		case 3:	val = turbulence_improved(	vec, harm_nb );	break;
		case 4:	val = fractalsum_improved(	vec, harm_nb );	break;
		case 5:	val = turbulence_wavelet(	vec, harm_nb );	break;
		case 6:	val = fractalsum_wavelet(	vec, harm_nb );	break;
		}
		return l.return_real( val );
	}
	AAALUACALL( get_turbulence )			{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d_with_harmonics<1>( l );	}
	AAALUACALL( get_fractalsum )			{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d_with_harmonics<2>( l );	}
	AAALUACALL( get_turbulence_improved )	{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d_with_harmonics<3>( l );	}
	AAALUACALL( get_fractalsum_improved )	{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d_with_harmonics<4>( l );	}
	AAALUACALL( get_turbulence_wavelet )	{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d_with_harmonics<5>( l );	}
	AAALUACALL( get_fractalsum_wavelet )	{	LUAAAA_START( L, __FUNCTION__ );	return get_noise3d_with_harmonics<6>( l );	}


	AAALUACALL( gain )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		REAL	val	=	l.get_real( 1 );
		::gain( val, l.get_real(2) );
		return l.return_real( val );
	}
	AAALUACALL( bias )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		REAL	val	=	l.get_real( 1 );
		::bias( val, l.get_real(2) );
		return l.return_real( val );
	}
	AAALUACALL( gain_bias )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		REAL	val	=	l.get_real( 1 );
		::gain_bias( val, l.get_real(2), l.get_real(3) );
		return l.return_real( val );
	}

	AAALUACALL( gain_clamped )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		REAL	val	=	l.get_real( 1 );
		::gain_clamped( val, l.get_real(2) );
		return l.return_real( val );
	}
	AAALUACALL( bias_clamped )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		REAL	val	=	l.get_real( 1 );
		::bias_clamped( val, l.get_real(2) );
		return l.return_real( val );
	}
	AAALUACALL( gain_bias_clamped )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		REAL	val	=	l.get_real( 1 );
		::gain_bias_clamped( val, l.get_real(2), l.get_real(3) );
		return l.return_real( val );
	}

	REAL catmull_data[12];
	AAALUACALL( do_catmull_rom_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb_min_max( 1, 13 );
		REAL	CONST	s		=		l.get_real( 1 );
		if( nb_arg==13 )
			l.get_v12( catmull_data, 2 );
		REAL vec[3];
		catmull_rom_v3( vec, catmull_data, s );
		return l.return_real_v3( vec );
	}
	AAALUACALL( do_catmull_rom_derivative_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb_min_max( 1, 13 );
		REAL	CONST	s		=	l.get_real( 1 );
		if( nb_arg==13 )
			l.get_v12( catmull_data, 2 );
		REAL vec[3];
		catmull_rom_derivative_v3( vec, catmull_data, s );
		return l.return_real_v3( vec );
	}

	AAALUACALL( do_catmull_rom_xy )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb_min_max( 1, 9 );
		REAL	CONST	s		=	l.get_real( 1 );
		if( nb_arg==9 )
			l.get_v8( catmull_data, 2 );
		REAL vec[2];
		catmull_rom_v2( vec, catmull_data, s );
		return l.return_real_v2( vec );
	}
	AAALUACALL( do_catmull_rom_derivative_xy )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb_min_max( 1, 9 );
		REAL	CONST	s		=	l.get_real( 1 );
		if( nb_arg==9 )
			l.get_v8( catmull_data, 2 );
		REAL vec[2];
		catmull_rom_derivative_v2( vec, catmull_data, s );
		return l.return_real_v2( vec );
	}

	REAL bezier_data[12];
	AAALUACALL( do_bezier_xyz )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb_min_max( 1, 13 );
		REAL	CONST	s		=	l.get_real( 1 );
		if( nb_arg==13 )
			l.get_v12( bezier_data, 2 );
		REAL vec[3];
		bezier_v3r( vec, bezier_data, s );
		return l.return_real_v3( vec );
	}
	AAALUACALL( do_bezier_xy )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb_min_max( 1, 9 );
		REAL	CONST	s		=	l.get_real( 1 );
		if( nb_arg==9 )
			l.get_v8( bezier_data, 2 );
		REAL vec[2];
		bezier_v2r( vec, bezier_data, s );
		return l.return_real_v2( vec );
	}

	AAALUACALL( convert_hilbert_xy_to_d )	// INT32 x, INT32 y, INT32 n
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb( 2, 3 );
		INT32	CONST	x	=	l.get_int32( 1 );
		INT32	CONST	y	=	l.get_int32( 2 );
		INT32	CONST	n	=	nb_arg == 3 ? l.get_int32(3) : 256;
		return l.return_int32( c_math::convert_hilbert_xy_to_d( n, x, y ) );
	}


	AAALUACALL( convert_hilbert_d_to_xy )	// INT32 d, INT32 n
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32	CONST	nb_arg	=	l.get_arg_nb( 1, 2 );
		INT32	CONST	d	=	l.get_int32( 1 );
		INT32	CONST	n	=	nb_arg == 2 ? l.get_int32(2) : 256;
		INT32			x,y;
		c_math::convert_hilbert_d_to_xy( n, d, x, y );
		return l.return_int_2( x, y );
	}

	void	register_math( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "math" );
			ADD_FN( get_noise1						);

			ADD_FN( get_noise2						);

			ADD_FN( get_noise3						);
			ADD_FN( get_noise3_by_perlin			);
			ADD_FN( get_noise3_improved				);
			//ADD_FN( get_turbulence3_by_perlin		);	

			ADD_FN( get_turbulence					);
			ADD_FN( get_fractalsum					);
			ADD_FN( get_turbulence_improved			);
			ADD_FN( get_fractalsum_improved			);
			ADD_FN( get_turbulence_wavelet 			);
			ADD_FN( get_fractalsum_wavelet 			);

			ADD_FN( gain							);
			ADD_FN( bias							);
			ADD_FN( gain_bias						);
			ADD_FN( gain_clamped					);
			ADD_FN( bias_clamped					);
			ADD_FN( gain_bias_clamped				);
			ADD_FN( do_catmull_rom_xyz				);
			ADD_FN( do_catmull_rom_derivative_xyz	);
			ADD_FN( do_catmull_rom_xy				);
			ADD_FN( do_catmull_rom_derivative_xy	);
			ADD_FN( do_bezier_xyz					);
			ADD_FN( do_bezier_xy					);

			ADD_FN( convert_hilbert_xy_to_d			);
			ADD_FN( convert_hilbert_d_to_xy			);
		l.pop( 1 );	//pop new table
	}
}	//end namespace n_math
}	//end namespace aaalua
