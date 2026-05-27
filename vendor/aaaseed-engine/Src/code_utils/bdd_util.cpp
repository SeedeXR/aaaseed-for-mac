#include "bdd_util.h"
#include "draw/map.h"

namespace{

/*! \todo optimize mirror */
FINLINE	REAL mirror( REAL in )
{
	in = FMOD( in, REAL(2) );
	if( in <= 1.)
		return in*REAL(.9994) + REAL(.0005);
	else
		return (REAL(2.)- in)*REAL(.9994) + REAL(.0005);
}

template <bool b_mu, bool b_mv, bool b_flip_uv>
FINLINE	void loop( REAL* uv, INT32 nb_u, INT32 CONST nb_v, REAL CONST u_beg, REAL CONST v_beg, REAL CONST du, REAL CONST dv )
{
	REAL v = v_beg;
	for( INT32 iv = nb_v; iv > 0; --iv )
	{
		REAL fv = b_mv ? mirror(v) : v;
		REAL u = u_beg;
		if( b_flip_uv )
			for( INT32 iu = nb_u; iu > 0; --iu )
			{
				*uv++ = fv;
				*uv++ = b_mu ? mirror(u) : u;
				u += du;
			}
		else
			for( INT32 iu = nb_u; iu > 0; --iu )
			{
				*uv++ = b_mu ? mirror(u) : u;
				*uv++ = fv;
				u += du;
			}
		v += dv;
	}
}

/*
static FINLINE	void loop_hexa( REAL* &uv, INT32 nb_u, INT32 nb_v, REAL su, REAL sv, REAL du, REAL dv )
{
	//INT32	i_v;
	REAL	off[3];
	c_map::get_cur()->get_hexa_offset( off );
	REAL	delta[2];
	c_map::get_cur()->get_hexa_delta( off );

	REAL	stu = delta[1] / MIN( 1, I_FLOOR( nb_u / 3 ) );
	REAL	stv = delta[2] / MIN( 1, I_FLOOR( nb_v / 3 ) );

	REAL	src[6][2];
	REAL	su_start = su;
	REAL	sv_start = sv;
	src[0][0] = su;									src[0][1] = sv + dv;
	src[1][0] = su + du * .5;						src[1][1] = sv + dv * (1 + off[0] * .375) ;
	src[2][0] = su + du;							src[2][1] = sv + dv;
	src[3][0] = su + du * (.75 - off[1] * .25);		src[3][1] = sv + dv * (.5 + off[1] * .125);
	src[4][0] = su + du * .5;						src[4][1] = sv;
	src[5][0] = su + du * (.25 + off[2] * .25);		src[5][1] = sv + dv * (.5 - off[2] * .125);


	INT32 is = 3;
	for( INT32 i_u = nb_u; i_u > 0; --i_u )
	{
		INT32 i = is;

		for( INT32 i_v = nb_v; i_v > 0; --i_v )
		{
			cpy_v2( uv, src[i] );
			uv += 2;
			i -= 3;
			if( i < 0 )
			i += 6;
		}
		if( ++is == 6 )
		is = 0;
	}
}
*/
#if 1
	static FINLINE	void loop_hexa( REAL* &uv, INT32 CONST nb_u, INT32 CONST nb_v, REAL CONST su_begin, REAL sv, REAL CONST du, REAL CONST dv )
	{
		//REAL off[3];
		//c_map::get_cur()->get_hexa_offset( off );
		REAL delta[2];
		c_map::get_cur()->get_hexa_delta( delta );

		REAL stu = delta[0] / MAX1( I_FLOOR( nb_u - 1) );
		REAL stv = delta[1] / MAX1( I_FLOOR( nb_v - 1) );

		INT32 is = 0;

		REAL src[6][2];
		src[0][0] = du * REAL(.25);
		src[1][0] = REAL(0);
		src[2][0] = du * REAL(.5);
		src[3][0] = du;
		src[4][0] = du * REAL(.75);	
		src[5][0] = du * REAL(.5);

		src[0][1] = dv * REAL(.5);
		src[1][1] = dv;
		src[2][1] = dv;
		src[3][1] = dv;
		src[4][1] = dv * REAL(.5);
		src[5][1] = REAL(0);


		--uv;
		for( INT32 i_v = nb_v; i_v > 0; --i_v )
		{
			REAL su = su_begin;
			//	the delta is an approximation here it should be treated by hexagon
			//	the code commented at the top is ok but no sliding from hexagon to hexagon

			INT32 i = is;
			REAL ou = su;
			for( INT32 i_u = nb_u; i_u > 0; --i_u )
			{
				*++uv = src[i][0] + ou;
				*++uv = src[i][1] + sv;

				if( i==5 )
					i=0;
				else
					++i;

				ou += stu;
			}
			is = 3 - is;
			sv += stv;
		}
	}
#else
	// was ok when we where drawing along v
	static FINLINE	void loop_hexa( REAL* &uv, INT32 CONST nb_u, INT32 CONST nb_v, REAL su, REAL CONST sv_start, REAL CONST du, REAL CONST dv )
	{
		//INT32	i_v;
		REAL	off[3];
		c_map::get_cur()->get_hexa_offset( off );
		REAL	delta[2];
		c_map::get_cur()->get_hexa_delta( delta );

		REAL	stu = delta[0] / MAX1( I_FLOOR( nb_u / 3 ) );
		REAL	stv = delta[1] / MAX1( I_FLOOR( nb_v / 3 ) );

		REAL	src[6][2];
	
		INT32 is = 3;	
		for( INT32 i_u = nb_u; i_u > 0; --i_u )
		{
			//	the delta is an approximation here it should be treated by hexagon
			//	the code commented at the top is ok but no sliding from hexagon to hexagon
			src[0][0] = su;
			src[1][0] = su + du * .5;
			src[2][0] = su + du;
			src[3][0] = su + du * (.75 - off[1] * .25);
			src[4][0] = su + du * .5;
			src[5][0] = su + du * (.25 + off[2] * .25);

			REAL sv = sv_start;
			INT32 i = is;
			for( INT32 i_v = nb_v; i_v > 0; --i_v )
			{
				src[0][1] = sv + dv;
				src[1][1] = sv + dv * (1 + off[0] * .375) ;
				src[2][1] = sv + dv;
				src[3][1] = sv + dv * (.5 + off[1] * .125);
				src[4][1] = sv;
				src[5][1] = sv + dv * (.5 - off[2] * .125);

				cpy_v2( uv, src[i] );
				uv += 2;
				i -= 3;
				if( i < 0 )
				{
					sv += stv;
					i += 6;
				}
				else if( i == 3 )
				{
					sv += stv;
				}
			}
			if( ++is == 6 )
			{
				su += stu;
				is = 0;
			}
			else if( is == 3 )
			{
				su += stu;
			}
		}
	}
#endif
}	//namespace

void bdd_util_build_uv( REAL* puv, INT32 CONST nb_u, INT32 CONST nb_v,
				bool CONST b_u_loop, bool CONST b_v_loop,
				bool CONST b_flip_uv_in,
				REAL CONST factor_u, REAL CONST factor_v,
				REAL CONST offset_u, REAL CONST offset_v )
{
	c_map*	map_cur		= c_map::get_cur();

	bool	mirror_u	= map_cur->is_mirror_u();
	bool	mirror_v	= map_cur->is_mirror_v();
	bool	b_flip_uv	= map_cur->is_flip_uv();

	bool	b_hexa		= map_cur->is_tex_hexa();
	REAL	u_begin;
	REAL	du;
	if( b_flip_uv != b_flip_uv_in )
	{
		u_begin = map_cur->get_u_min();
		du = map_cur->get_u_max() - u_begin;
	}
	else
	{
		u_begin = map_cur->get_u_max();
		du = map_cur->get_u_min() - u_begin;
	}

	REAL	v_begin = map_cur->get_v_min();
	REAL	dv = map_cur->get_v_max() - v_begin;

	u_begin += offset_u;
	v_begin += offset_v;

	du *= factor_u;
	dv *= factor_v;
	if( !b_hexa )
	{
		if( b_u_loop )
			du /= nb_u;	// was nb_u-2 2023 March
		else
			du /= nb_u-1;
		if( b_v_loop )
			dv /= nb_v;	// was nb_v-2 2023 March
		else	
			dv /= nb_v-1;
	}

	if( b_hexa )
		loop_hexa( puv, nb_u,nb_v, u_begin,v_begin, du,dv );
	else
	{
		/*! \todo could be optimize */
		if( mirror_u && du < 0 )
			u_begin += 1;
		if( mirror_v && dv < 0 )
			v_begin += 1;
		if( b_flip_uv )
			if( mirror_u )
				if( mirror_v )
					loop<true,  true,  true >( puv, nb_u,nb_v, u_begin,v_begin, du,dv );
				else
					loop<false, true,  true >( puv, nb_u,nb_v, u_begin,v_begin, du,dv );
			else
				if( mirror_v )
					loop<true,  false, true >( puv, nb_u,nb_v, u_begin,v_begin, du,dv );
				else
					loop<false, false, true >( puv, nb_u,nb_v, u_begin,v_begin, du,dv );
		else
			if( mirror_u )
				if( mirror_v )
					loop<true,  true,  false>( puv, nb_u,nb_v, u_begin,v_begin, du,dv );
				else
					loop<true,  false, false>( puv, nb_u,nb_v, u_begin,v_begin, du,dv );
			else
				if( mirror_v )
					loop<false, true,  false>( puv, nb_u,nb_v, u_begin,v_begin, du,dv );
				else
					loop<false, false, false>( puv, nb_u,nb_v, u_begin,v_begin, du,dv );
	}
}
