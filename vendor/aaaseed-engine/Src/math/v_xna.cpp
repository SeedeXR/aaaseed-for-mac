#include "v_xna.h"

#ifndef __XNAMATH_H__
#	ifndef	AAA_AAA_OS_H
#		include "aaa_os.h"	//needed by xnamath.h
#	endif
#	include "xnamath.h"
#endif
#include "v.h"

//#define		CHECK_ALIGNMENT( ptr, ali )	(UINT32)ptr % ali

#define MEM_NOT_ALIGNED_16_x2(a,b)		(	mem::IS_ALIGNED_16_NOT( a ) || mem::IS_ALIGNED_16_NOT( b ) )
#define MEM_NOT_ALIGNED_16_x3(a,b,c)	(	mem::IS_ALIGNED_16_NOT( a ) || mem::IS_ALIGNED_16_NOT( b ) || mem::IS_ALIGNED_16_NOT( c ) )

#define LOAD_FP32_2(x)		(	mem::IS_ALIGNED_16( x ) ? XMLoadFloat2A( (XMFLOAT2A*)x ) : XMLoadFloat2( (XMFLOAT2*)x ) )
#define LOAD_FP32_3(x)		(	mem::IS_ALIGNED_16( x ) ? XMLoadFloat3A( (XMFLOAT3A*)x ) : XMLoadFloat3( (XMFLOAT3*)x ) )
#define LOAD_FP32_4(x)		(	mem::IS_ALIGNED_16( x ) ? XMLoadFloat4A( (XMFLOAT4A*)x ) : XMLoadFloat4( (XMFLOAT4*)x ) )

#define STORE_FP32_2(dst,x)	{ 	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat2A( (XMFLOAT2A*)dst, x ); else XMStoreFloat2( (XMFLOAT2*)dst, x ); }
#define STORE_FP32_3(dst,x)	{ 	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, x ); else XMStoreFloat3( (XMFLOAT3*)dst, x ); }
#define STORE_FP32_4(dst,x)	{ 	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat4A( (XMFLOAT4A*)dst, x ); else XMStoreFloat4( (XMFLOAT4*)dst, x ); }


FINLINE	void	interpolate_v2_xna( FLOAT* CONST dst, FLOAT CONST * CONST b, FLOAT CONST r )
{
	if( mem::IS_ALIGNED_16( dst ) )
		XMStoreFloat2A( (XMFLOAT2A*)dst, XMVectorLerp( XMLoadFloat2A((XMFLOAT2A*)dst), LOAD_FP32_2(b), r ) );
	else
		XMStoreFloat2 ( (XMFLOAT2 *)dst, XMVectorLerp( XMLoadFloat2 ((XMFLOAT2 *)dst), LOAD_FP32_2(b), r ) );
}
FINLINE	void	interpolate_v2_xna( FLOAT* CONST dst, FLOAT CONST * CONST a, FLOAT CONST * CONST b, FLOAT CONST r )
{
	STORE_FP32_2( dst, XMVectorLerp(	LOAD_FP32_2(a), LOAD_FP32_2(b), r ) );
}

FINLINE	void	interpolate_v3_xna( FLOAT* CONST dst, FLOAT CONST * CONST b, FLOAT CONST r )
{
	if( mem::IS_ALIGNED_16( dst ) )
		XMStoreFloat3A( (XMFLOAT3A*)dst, XMVectorLerp( XMLoadFloat3A((XMFLOAT3A*)dst), LOAD_FP32_3(b), r ) );
	else
		XMStoreFloat3 ( (XMFLOAT3 *)dst, XMVectorLerp( XMLoadFloat3 ((XMFLOAT3 *)dst), LOAD_FP32_3(b), r ) );
}
FINLINE	void	interpolate_v3_xna( FLOAT* CONST dst, FLOAT CONST * CONST a, FLOAT CONST * CONST b, FLOAT CONST r )
{
	STORE_FP32_3( dst, XMVectorLerp(	LOAD_FP32_3(a), LOAD_FP32_3(b), r ) );
}

FINLINE	void	interpolate_v4_xna( FLOAT* CONST dst, FLOAT CONST * CONST b, FLOAT CONST r )	
{
	if( mem::IS_ALIGNED_16( dst ) )
		XMStoreFloat4A( (XMFLOAT4A*)dst, XMVectorLerp( XMLoadFloat4A((XMFLOAT4A*)dst), LOAD_FP32_4(b), r ) );
	else
		XMStoreFloat4 ( (XMFLOAT4 *)dst, XMVectorLerp( XMLoadFloat4 ((XMFLOAT4 *)dst), LOAD_FP32_4(b), r ) );
}
FINLINE	void	interpolate_v4_xna( FLOAT* CONST dst, FLOAT CONST * CONST a, FLOAT CONST * CONST b, FLOAT CONST r )
{
	STORE_FP32_4( dst, XMVectorLerp(	LOAD_FP32_4(a), LOAD_FP32_4(b), r ) );
}

FINLINE	FP32	dist_v2_xna(		FP32 CONST * CONST a, FP32 CONST * CONST b )				{	return XMVectorGetX( XMVector2Length( LOAD_FP32_2(a) - LOAD_FP32_2(b) ) );	}
FINLINE	FP32	dist_v3_xna(		FP32 CONST * CONST a, FP32 CONST * CONST b )				{	return XMVectorGetX( XMVector3Length( LOAD_FP32_3(a) - LOAD_FP32_3(b) ) );	}

//DIST
FINLINE	FP32	dist_squared_v2_xna( FP32 CONST * CONST a, FP32 CONST * CONST b )				{	return XMVectorGetX( XMVector2LengthSq( LOAD_FP32_2(a) - LOAD_FP32_2(b) ) );	}
FINLINE	FP32	dist_squared_v3_xna( FP32 CONST * CONST a, FP32 CONST * CONST b )				{	return XMVectorGetX( XMVector3LengthSq( LOAD_FP32_3(a) - LOAD_FP32_3(b) ) );	}
FINLINE	FP32	dist_box_v3_xna(	  FP32 CONST * CONST a, FP32 CONST * CONST b )
{
	XMVECTOR	tmp = XMVectorAbs( LOAD_FP32_3(a) - LOAD_FP32_3(b) );
	return XMVectorGetX( tmp ) + XMVectorGetY( tmp ) + XMVectorGetZ( tmp );
}

FINLINE	FP32	dist_box_v3_xna(	FP32 CONST * CONST a, FP32 CONST x, FP32 CONST y, FP32 CONST z )
{
	XMVECTOR	tmp = XMVectorAbs( LOAD_FP32_3(a) - XMVectorSet( x,y,z, .0 ) );
	return XMVectorGetX( tmp ) + XMVectorGetY( tmp ) + XMVectorGetZ( tmp );
}

FINLINE	void	scale_v2_xna(		FP32* CONST dst, FP32 CONST * CONST src, FP32 CONST f )
{
	XMVECTOR	tmp = XMVectorScale( LOAD_FP32_2(src), f );
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat2A((XMFLOAT2A*)dst, tmp );
	else							XMStoreFloat2 ((XMFLOAT2 *)dst, tmp );
}
FINLINE	void	scale_v2_xna(		FP32* CONST dst, FP32 CONST f )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat2A( (XMFLOAT2A*)dst, XMVectorScale( XMLoadFloat2A( (XMFLOAT2A*)dst ), f ) );
	else							XMStoreFloat2 ( (XMFLOAT2 *)dst, XMVectorScale( XMLoadFloat2 ( (XMFLOAT2 *)dst ), f ) );
}

FINLINE	void	scale_v3_xna(		FP32* CONST dst, FP32 CONST * CONST src, FP32 CONST f )
{
	XMVECTOR	tmp = XMVectorScale( LOAD_FP32_3(src), f );
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A((XMFLOAT3A*)dst, tmp );
	else							XMStoreFloat3 ((XMFLOAT3 *)dst, tmp );
}
FINLINE	void	scale_v3_xna(		FP32* CONST dst, FP32 CONST f )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMVectorScale( XMLoadFloat3A( (XMFLOAT3A*)dst ), f ) );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMVectorScale( XMLoadFloat3 ( (XMFLOAT3 *)dst ), f ) );
}

FINLINE	void	scale_v4_xna(		FP32* CONST dst, FP32 CONST * CONST src, FP32 CONST f )
{
	STORE_FP32_4( dst, XMVectorScale( LOAD_FP32_4(src), f ) );
}
FINLINE	void	scale_v4_xna(		FP32* CONST dst, FP32 CONST f )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat4A( (XMFLOAT4A*)dst, XMVectorScale( XMLoadFloat4A( (XMFLOAT4A*)dst ), f ) );
	else							XMStoreFloat4 ( (XMFLOAT4 *)dst, XMVectorScale( XMLoadFloat4 ( (XMFLOAT4 *)dst ), f ) );
}


FINLINE	void	mul_v3_xna(		FP32* CONST dst, FP32 CONST * CONST a )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A((XMFLOAT3A*)dst, XMLoadFloat3A( (XMFLOAT3A*)dst ) * LOAD_FP32_3(a) );
	else							XMStoreFloat3 ((XMFLOAT3 *)dst, XMLoadFloat3(  (XMFLOAT3 *)dst ) * LOAD_FP32_3(a) );
}
FINLINE	void	mul_v4_xna(		FP32* CONST dst, FP32 CONST * CONST a )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat4A((XMFLOAT4A*)dst, XMLoadFloat4A( (XMFLOAT4A*)dst ) * LOAD_FP32_4(a) );
	else							XMStoreFloat4 ((XMFLOAT4 *)dst, XMLoadFloat4(  (XMFLOAT4 *)dst ) * LOAD_FP32_4(a) );
}

FINLINE	void	mul_v3_xna(		FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b )	{	STORE_FP32_3( dst, LOAD_FP32_3(a) * LOAD_FP32_3(b) );	}
FINLINE	void	mul_v4_xna(		FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b )	{	STORE_FP32_4( dst, LOAD_FP32_4(a) * LOAD_FP32_4(b) );	}

FINLINE	void	mul_add_v3_xna(	FP32* CONST dst, FP32 CONST * CONST b, FP32 CONST * CONST c )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMVectorMultiplyAdd( XMLoadFloat3A( (XMFLOAT3A*)dst ), LOAD_FP32_3(b), LOAD_FP32_3(c) ) );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMVectorMultiplyAdd( XMLoadFloat3 ( (XMFLOAT3 *)dst ), LOAD_FP32_3(b), LOAD_FP32_3(c) ) );
}
FINLINE	void	mul_add_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c )
{
	STORE_FP32_3( dst, XMVectorMultiplyAdd( LOAD_FP32_3(a), LOAD_FP32_3(b), LOAD_FP32_3(c) ) );
}

FINLINE	void	mul_add_v4_xna(	FP32* CONST dst, FP32 CONST * CONST b, FP32 CONST * CONST c )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat4A( (XMFLOAT4A*)dst, XMVectorMultiplyAdd( XMLoadFloat4A( (XMFLOAT4A*)dst ), LOAD_FP32_4(b), LOAD_FP32_4(c) ) );
	else							XMStoreFloat4 ( (XMFLOAT4 *)dst, XMVectorMultiplyAdd( XMLoadFloat4 ( (XMFLOAT4 *)dst ), LOAD_FP32_4(b), LOAD_FP32_4(c) ) );
}
FINLINE	void	mul_add_v4_xna(	FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c )
{
	STORE_FP32_4( dst, XMVectorMultiplyAdd( LOAD_FP32_4(a), LOAD_FP32_4(b), LOAD_FP32_4(c) ) );
}

FINLINE	void	cpy_v4_xna(		FP32* dst, FP32 CONST * CONST src )											{	STORE_FP32_4( dst, LOAD_FP32_4(src) );										}

FINLINE	void	cpy_v3_xna(		FP32* CONST dst, FP32 CONST * CONST src )									{	STORE_FP32_3( dst, LOAD_FP32_3(src) );										}
FINLINE	void	neg_v3_xna(		FP32* CONST dst, FP32 CONST * CONST src )									{	STORE_FP32_3( dst, -LOAD_FP32_3(src) );										}
FINLINE	void	set_v4_xna(		FP32* CONST dst, FP32 CONST a, FP32 CONST b, FP32 CONST c, FP32 CONST d )	{	STORE_FP32_4( dst, XMVectorSet( a, b, c, d ) );								}


FINLINE	void	set_v3_xna(		FP32* CONST dst, FP32 CONST a, FP32 CONST b, FP32 CONST c )		{	STORE_FP32_3( dst, XMVectorSet( a, b, c, 0 ) );											}
FINLINE	void	set_v3_xna(		FP32* CONST dst, FP32 CONST all )								{	STORE_FP32_3( dst, XMVectorReplicate( all ) );											}
FINLINE	void	set_v4_xna(		FP32* CONST dst, FP32 CONST all )								{	STORE_FP32_4( dst, XMVectorReplicate( all ) );											}

FINLINE	bool	is_equal_v3_xna(	FP32 CONST * CONST a, FP32 CONST * CONST b )					{	return XMComparisonAllTrue( XMVector3EqualR( LOAD_FP32_3(a), LOAD_FP32_3(b) ) );			}
FINLINE	bool	is_diff_v3_xna(	FP32 CONST * CONST a, FP32 CONST * CONST b )					{	return XMComparisonAnyFalse( XMVector3EqualR( LOAD_FP32_3(a), LOAD_FP32_3(b) ) );			}
FINLINE	bool	is_equal_v3_xna(	FP32 CONST * CONST a, FP32 CONST f )							{	return XMComparisonAllTrue( XMVector3EqualR( LOAD_FP32_3(a), XMVectorReplicate( f ) ) );	}
FINLINE	bool	is_equal_v4_xna(	FP32 CONST * CONST a, FP32 CONST * CONST b )					{	return XMComparisonAllTrue( XMVector4EqualR( LOAD_FP32_4(a), LOAD_FP32_4(b) ) );			}
FINLINE	bool	is_diff_v4_xna(	FP32 CONST * CONST a, FP32 CONST * CONST b )					{	return XMComparisonAnyFalse( XMVector4EqualR( LOAD_FP32_4(a), LOAD_FP32_4(b) ) );			}
FINLINE	bool	is_equal_v4_xna(	FP32 CONST * CONST a, FP32 CONST f )							{	return XMComparisonAllTrue( XMVector4EqualR( LOAD_FP32_4(a), XMVectorReplicate( f ) ) );	}


FINLINE	void	add_scale_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST factor )	{	STORE_FP32_3( dst, LOAD_FP32_3(a) + LOAD_FP32_3(b) * XMVectorReplicate( factor ) );		}

FINLINE	void	add_scale_v3_xna(	FP32* CONST dst, FP32 CONST * CONST b, FP32 CONST factor )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A((XMFLOAT3A*)dst, XMLoadFloat3A( (XMFLOAT3A*)dst ) + LOAD_FP32_3(b) * XMVectorReplicate( factor ) );
	else							XMStoreFloat3 ((XMFLOAT3 *)dst, XMLoadFloat3 ( (XMFLOAT3 *)dst ) + LOAD_FP32_3(b) * XMVectorReplicate( factor ) );
}

FINLINE	void	add_scale_add_v3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST factor )
{
	XMVECTOR	tmp = XMVectorScale( LOAD_FP32_3(a) + LOAD_FP32_3(b), factor );
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMLoadFloat3A( (XMFLOAT3A*)dst ) + tmp );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMLoadFloat3 ( (XMFLOAT3 *)dst ) + tmp );
}

FINLINE	void	add_then_scale_v3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST factor )
{
	STORE_FP32_3( dst, XMVectorScale( LOAD_FP32_3(a) + LOAD_FP32_3(b), factor ) );
}

FINLINE	void	add_mul_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c )
{
	STORE_FP32_3( dst, LOAD_FP32_3(a) + LOAD_FP32_3(b) * LOAD_FP32_3(c) );
}
FINLINE	void	add_mul_v3_xna(	FP32* CONST dst, FP32 CONST * CONST b, FP32 CONST * CONST c )
{
	XMVECTOR	tmp = LOAD_FP32_3(b) * LOAD_FP32_3(c);
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMLoadFloat3A( (XMFLOAT3A*)dst ) + tmp );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMLoadFloat3 ( (XMFLOAT3 *)dst ) + tmp );
}
//ADD
FINLINE	void	add_v3_xna(			FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b )	{	STORE_FP32_3( dst, LOAD_FP32_3(a) + LOAD_FP32_3(b) );	}
FINLINE	void	add_v3_xna(			FP32* CONST dst, FP32 CONST * CONST b )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMLoadFloat3A( (XMFLOAT3A*)dst ) + LOAD_FP32_3(b) );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMLoadFloat3 ( (XMFLOAT3 *)dst ) + LOAD_FP32_3(b) );
}
FINLINE	void	add_v4_xna(			FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b )	{	STORE_FP32_4( dst, LOAD_FP32_4(a) + LOAD_FP32_4(b) );	}
FINLINE	void	add_v4_xna(			FP32* CONST dst, FP32 CONST * CONST b )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat4A( (XMFLOAT4A*)dst, XMLoadFloat4A( (XMFLOAT4A*)dst ) + LOAD_FP32_4(b) );
	else							XMStoreFloat4 ( (XMFLOAT4 *)dst, XMLoadFloat4 ( (XMFLOAT4 *)dst ) + LOAD_FP32_4(b) );
}
//SUB
FINLINE	void	sub_v3_xna(			FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b )	{	STORE_FP32_3( dst, LOAD_FP32_3(a) - LOAD_FP32_3(b) );	}
FINLINE	void	sub_v3_xna(			FP32* CONST dst, FP32 CONST * CONST b )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMLoadFloat3A( (XMFLOAT3A*)dst ) - LOAD_FP32_3(b) );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMLoadFloat3 ( (XMFLOAT3 *)dst ) - LOAD_FP32_3(b) );
}
FINLINE	void	sub_v4_xna(			FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b )	{	STORE_FP32_4( dst, LOAD_FP32_4(a) - LOAD_FP32_4(b) );	}
FINLINE	void	sub_v4_xna(			FP32* CONST dst, FP32 CONST * CONST b )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat4A( (XMFLOAT4A*)dst, XMLoadFloat4A( (XMFLOAT4A*)dst ) - LOAD_FP32_4(b) );
	else							XMStoreFloat4 ( (XMFLOAT4 *)dst, XMLoadFloat4 ( (XMFLOAT4 *)dst ) - LOAD_FP32_4(b) );
}

FINLINE	void	sub_reverse_v3_xna( FP32* CONST dst, FP32 CONST * CONST a )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, LOAD_FP32_3(a) - XMLoadFloat3A( (XMFLOAT3A*)dst ) );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, LOAD_FP32_3(a) - XMLoadFloat3 ( (XMFLOAT3 *)dst ) );
}

FINLINE	void	sub_then_scale_v3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST factor )
{
	STORE_FP32_3( dst, XMVectorScale( LOAD_FP32_3(a) - LOAD_FP32_3(b), factor ) );
}

//	a = (a-b)*f;
FINLINE	void	sub_then_scale_v3_xna( FP32* CONST dst, FP32 CONST * CONST b, FP32 CONST f )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMVectorScale( XMLoadFloat3A( (XMFLOAT3A*)dst ) - LOAD_FP32_3(b), f ) );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMVectorScale( XMLoadFloat3 ( (XMFLOAT3 *)dst ) - LOAD_FP32_3(b), f ) );
}

//	dst += (a-b)
FINLINE	void	sub_then_add_to_v3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b )
{
	XMVECTOR	tmp = LOAD_FP32_3(a) - LOAD_FP32_3(b);
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMLoadFloat3A( (XMFLOAT3A*)dst ) + tmp );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMLoadFloat3 ( (XMFLOAT3 *)dst ) + tmp );
}

FINLINE	void	mul_scale_v3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST f )
{
	STORE_FP32_3( dst, LOAD_FP32_3(a) * LOAD_FP32_3(b) * XMVectorReplicate( f ) );
}

//MIX
FINLINE	void	mix_v2_xna(	FP32* CONST dst, FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb )
{
	//written by maa with no knowledge of xna
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat2A( (XMFLOAT2A*)dst, XMVectorScale( LOAD_FP32_2(dst), fa) + XMVectorScale( LOAD_FP32_2(b), fb) );
	else							XMStoreFloat2 ( (XMFLOAT2 *)dst, XMVectorScale( LOAD_FP32_2(dst), fa) + XMVectorScale( LOAD_FP32_2(b), fb) );
}
FINLINE	void	mix_v2_xna(	FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb )
{
	//written by maa with no knowledge of xna
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat2A( (XMFLOAT2A*)dst, XMVectorScale( LOAD_FP32_2(a), fa) + XMVectorScale( LOAD_FP32_2(b), fb) );
	else							XMStoreFloat2 ( (XMFLOAT2 *)dst, XMVectorScale( LOAD_FP32_2(a), fa) + XMVectorScale( LOAD_FP32_2(b), fb) );
}
FINLINE	void	mix_v3_xna(	FP32* CONST dst, FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb )
{
	//written by maa with no knowledge of xna
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMVectorScale( LOAD_FP32_3(dst), fa) + XMVectorScale( LOAD_FP32_3(b), fb) );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMVectorScale( LOAD_FP32_3(dst), fa) + XMVectorScale( LOAD_FP32_3(b), fb) );
}
FINLINE	void	mix_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb )
{
	//written by maa with no knowledge of xna
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMVectorScale( LOAD_FP32_3(a), fa) + XMVectorScale( LOAD_FP32_3(b), fb) );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMVectorScale( LOAD_FP32_3(a), fa) + XMVectorScale( LOAD_FP32_3(b), fb) );
}
FINLINE	void	mix_v4_xna(	FP32* CONST dst, FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb )
{
	//written by maa with no knowledge of xna
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat4A( (XMFLOAT4A*)dst, XMVectorScale( LOAD_FP32_4(dst), fa) + XMVectorScale( LOAD_FP32_4(b), fb) );
	else							XMStoreFloat4 ( (XMFLOAT4 *)dst, XMVectorScale( LOAD_FP32_4(dst), fa) + XMVectorScale( LOAD_FP32_4(b), fb) );
}
FINLINE	void	mix_v4_xna(	FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST fa, FP32 CONST * CONST b, FP32 CONST fb )
{
	//written by maa with no knowledge of xna
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat4A( (XMFLOAT4A*)dst, XMVectorScale( LOAD_FP32_4(a), fa) + XMVectorScale( LOAD_FP32_4(b), fb) );
	else							XMStoreFloat4 ( (XMFLOAT4 *)dst, XMVectorScale( LOAD_FP32_4(a), fa) + XMVectorScale( LOAD_FP32_4(b), fb) );
}

//CLEAR
FINLINE	void	clear_v3_xna(	FP32* CONST dst )	{	STORE_FP32_3( dst, XMVectorZero() );	}
FINLINE	void	clear_v4_xna(	FP32* CONST dst )	{	STORE_FP32_4( dst, XMVectorZero() );	}

FINLINE	void	abs_v3_xna(	FP32* CONST dst, FP32 CONST * CONST src )	{	STORE_FP32_3( dst, XMVectorAbs( LOAD_FP32_3(src) ) );		}
FINLINE	void	abs_v3_xna(	FP32* CONST dst )
{
	if( mem::IS_ALIGNED_16( dst ) )	XMStoreFloat3A( (XMFLOAT3A*)dst, XMVectorAbs( XMLoadFloat3A( (XMFLOAT3A*)dst ) ) );
	else							XMStoreFloat3 ( (XMFLOAT3 *)dst, XMVectorAbs( XMLoadFloat3 ( (XMFLOAT3 *)dst ) ) );
}

FINLINE	void	limit_v3_xna(	FP32* CONST dst, FP32 CONST max )
{
	if( mem::IS_ALIGNED_16( dst ) )
	{
		XMVECTOR	dst_vec = XMLoadFloat3A( (XMFLOAT3A*)dst );
		FP32	size = XMVectorGetX( XMVector3LengthSq( dst_vec ) );
		if( size > max * max )
		{
			size = max * OVER_ONE_AS_FP32( SQRT( size) );
			dst_vec = XMVectorScale( dst_vec, size );
			XMStoreFloat3A((XMFLOAT3A*)dst, dst_vec );
		}
	}
	else
	{
		XMVECTOR	dst_vec = XMLoadFloat3( (XMFLOAT3*)dst );
		FP32	size = XMVectorGetX( XMVector3LengthSq( dst_vec ) );
		if( size > max * max )
		{
			size = max * OVER_ONE_AS_FP32( SQRT( size) );
			dst_vec = XMVectorScale( dst_vec, size );
			XMStoreFloat3((XMFLOAT3*)dst, dst_vec );
		}
	}
}

FINLINE	FP32	dot_v3_xna(	FP32 CONST * CONST a, FP32 CONST * CONST b )
{
	return XMVectorGetX( XMVector3Dot( LOAD_FP32_3(a), LOAD_FP32_3(b) ) );

}

FINLINE	void	cross_v3_xna(	FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b )
{
	STORE_FP32_3( dst, XMVector3Cross( LOAD_FP32_3(a), LOAD_FP32_3(b) ) );
}

FINLINE	void	cross_normalize_v3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b )
{
	STORE_FP32_3( dst, XMVector3Normalize( XMVector3Cross( LOAD_FP32_3(a), LOAD_FP32_3(b) ) ) );
}

FINLINE	void	cross_normalize_scale_v3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST scale_factor )
{
	STORE_FP32_3( dst, XMVector3Normalize( XMVector3Cross( LOAD_FP32_3(a), LOAD_FP32_3(b) ) ) * XMVectorReplicate( scale_factor ) );
}

FINLINE	void	normalize_v3_xna( FP32* CONST dst )
{
	if( mem::IS_ALIGNED_16( dst ) )
		XMStoreFloat3A( (XMFLOAT3A*)dst, XMVector3Normalize( XMLoadFloat3A( (XMFLOAT3A*)dst ) ) );
	else
		XMStoreFloat3 ( (XMFLOAT3 *)dst, XMVector3Normalize( XMLoadFloat3 ( (XMFLOAT3 *)dst ) ) );								
}

FINLINE	void	normalize_v3_xna( FP32* CONST dst, FP32 CONST * CONST src )
{
	STORE_FP32_3( dst, XMVector3Normalize( LOAD_FP32_3(src) ) );
}

FINLINE	void	normalize_scale_v3_xna( FP32* CONST dst, FP32 CONST scale_factor )
{
	if( mem::IS_ALIGNED_16( dst ) )
		XMStoreFloat3A( (XMFLOAT3A*)dst, XMVectorScale( XMVector3Normalize( XMLoadFloat3A( (XMFLOAT3A*)dst ) ), scale_factor ) );
	else
		XMStoreFloat3 ( (XMFLOAT3 *)dst, XMVectorScale( XMVector3Normalize( XMLoadFloat3 ( (XMFLOAT3 *)dst ) ), scale_factor ) );	
}

CONSTEXPR  void normal_of_4_point_v3_xna( FP32 * CONST nor, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d )
{
	if( mem::IS_ALIGNED_16_NOT( nor ) || mem::IS_ALIGNED_16_NOT( a ) || mem::IS_ALIGNED_16_NOT( b ) || mem::IS_ALIGNED_16_NOT( c ) || mem::IS_ALIGNED_16_NOT( d ) )
	{
		XMVECTOR	vec_v = XMLoadFloat3( (XMFLOAT3*)b ) - XMLoadFloat3( (XMFLOAT3*)a );
		XMVECTOR	vec_u = XMLoadFloat3( (XMFLOAT3*)d ) - XMLoadFloat3( (XMFLOAT3*)c );
		XMStoreFloat3( (XMFLOAT3*)nor, XMVector3Cross( vec_u, vec_v ) );
	}
	else
	{
		XMVECTOR	vec_v = XMLoadFloat3A( (XMFLOAT3A*)b ) - XMLoadFloat3A( (XMFLOAT3A*)a );
		XMVECTOR	vec_u = XMLoadFloat3A( (XMFLOAT3A*)d ) - XMLoadFloat3A( (XMFLOAT3A*)c );
		XMStoreFloat3A( (XMFLOAT3A*)nor, XMVector3Cross( vec_u, vec_v ) );
	}
}

CONSTEXPR  void normal_cano_of_4_point_v3_xna( FP32 * CONST nor, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d )
{
	if( mem::IS_ALIGNED_16_NOT( nor ) || mem::IS_ALIGNED_16_NOT( a ) || mem::IS_ALIGNED_16_NOT( b ) || mem::IS_ALIGNED_16_NOT( c ) || mem::IS_ALIGNED_16_NOT( d ) )
	{
		XMVECTOR	vec_v = XMLoadFloat3( (XMFLOAT3*)b ) - XMLoadFloat3( (XMFLOAT3*)a );
		XMVECTOR	vec_u = XMLoadFloat3( (XMFLOAT3*)d ) - XMLoadFloat3( (XMFLOAT3*)c );
		XMStoreFloat3( (XMFLOAT3*)nor, XMVector3Normalize( XMVector3Cross( vec_u, vec_v ) ) );
	}
	else
	{
		XMVECTOR	vec_v = XMLoadFloat3A( (XMFLOAT3A*)b ) - XMLoadFloat3A( (XMFLOAT3A*)a );
		XMVECTOR	vec_u = XMLoadFloat3A( (XMFLOAT3A*)d ) - XMLoadFloat3A( (XMFLOAT3A*)c );
		XMStoreFloat3A( (XMFLOAT3A*)nor, XMVector3Normalize( XMVector3Cross( vec_u, vec_v ) ) );
	}
}

namespace
{
//__declspec(align(16))	static	FP32	axe_y[3] = { .0f, 1.0f , .0f };
static CONST XMVECTOR	axe_x_vec = XMVectorSet( 1.0f, .0f, .0f, .0f );
static CONST XMVECTOR	axe_y_vec = XMVectorSet( .0f, 1.0f, .0f, .0f );
static CONST XMVECTOR	axe_z_vec = XMVectorSet( .0f, .0f, 1.0f, .0f );
}

void	build_normal_vectors_v3_xna( FP32 CONST * CONST nor, FP32* CONST u, FP32* CONST v )
{
	XMVECTOR nor_vec;
	if( mem::IS_ALIGNED_16( nor ) )
		nor_vec = XMLoadFloat3A( (XMFLOAT3A*)nor );
	else
		nor_vec = XMLoadFloat3( (XMFLOAT3*)nor );
	if( mem::IS_ALIGNED_16_NOT( u ) || mem::IS_ALIGNED_16_NOT( v ) )
	{
		XMVECTOR	v_vec = XMVector3Normalize( XMVector3Cross( nor_vec, axe_y_vec ) );
		XMVECTOR	u_vec = XMVector3Normalize( XMVector3Cross( v_vec, nor_vec ) );
		XMStoreFloat3((XMFLOAT3*)u, u_vec );
		XMStoreFloat3((XMFLOAT3*)v, v_vec );
	}
	else
	{
		XMVECTOR	v_vec = XMVector3Normalize( XMVector3Cross( nor_vec, axe_y_vec ) );
		XMVECTOR	u_vec = XMVector3Normalize( XMVector3Cross( v_vec, nor_vec ) );
		XMStoreFloat3A((XMFLOAT3A*)u, u_vec );
		XMStoreFloat3A((XMFLOAT3A*)v, v_vec );
	}
}
void	build_normal_vectors_using_z_v3_xna( FP32 CONST * CONST nor, FP32* CONST u, FP32* CONST v )
{
	XMVECTOR nor_vec;
	if( mem::IS_ALIGNED_16( nor ) )
		nor_vec = XMLoadFloat3A( (XMFLOAT3A*)nor );
	else
		nor_vec = XMLoadFloat3( (XMFLOAT3*)nor );
	if( mem::IS_ALIGNED_16_NOT( u ) || mem::IS_ALIGNED_16_NOT( v ) )
	{
		XMVECTOR	v_vec = XMVector3Normalize( XMVector3Cross( nor_vec, axe_z_vec ) );
		XMVECTOR	u_vec = XMVector3Normalize( XMVector3Cross( v_vec, nor_vec ) );
		XMStoreFloat3((XMFLOAT3*)u, u_vec );
		XMStoreFloat3((XMFLOAT3*)v, v_vec );
	}
	else
	{
		XMVECTOR	v_vec = XMVector3Normalize( XMVector3Cross( nor_vec, axe_z_vec ) );
		XMVECTOR	u_vec = XMVector3Normalize( XMVector3Cross( v_vec, nor_vec ) );
		XMStoreFloat3A((XMFLOAT3A*)u, u_vec );
		XMStoreFloat3A((XMFLOAT3A*)v, v_vec );
	}
}

FINLINE	FP32	norm_squared_v3_xna(	FP32 CONST * CONST a )	{	return XMVectorGetX( XMVector3LengthSq( LOAD_FP32_3(a) ) );	}
FINLINE	FP32	norm_squared_v2_xna(	FP32 CONST * CONST a )	{	return XMVectorGetX( XMVector2LengthSq( LOAD_FP32_2(a) ) );	}
FINLINE	FP32	norm_v3_xna(			FP32 CONST * CONST a )	{	return XMVectorGetX( XMVector3Length(	LOAD_FP32_3(a) ) );	}
FINLINE	FP32	norm_v2_xna(			FP32 CONST * CONST a )	{	return XMVectorGetX( XMVector2Length(	LOAD_FP32_2(a) ) );	}

FINLINE	FP32	get_dist_squared_if_in_dist_squared_v3_xna( FP32 CONST * CONST a, FP32 CONST dist_squared )
{
	XMVECTOR	tmp_vec = XMVector3LengthSq( LOAD_FP32_3(a) );
	return XMVector3Less( tmp_vec, XMVectorReplicate( dist_squared ) ) ? XMVectorGetX( tmp_vec ) : FP32_BIG_VALUE;
}

FINLINE	FP32	get_dist_squared_if_in_dist_squared_v3_xna( FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST dist_squared )
{
	XMVECTOR	tmp_vec = XMVector3LengthSq( LOAD_FP32_3(a) - LOAD_FP32_3(b) );
	return XMVector3Less( tmp_vec, XMVectorReplicate( dist_squared ) ) ? XMVectorGetX( tmp_vec ) : FP32_BIG_VALUE;
}

FINLINE	bool	is_dist_squared_less_v3_xna( FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST dist_squared )
{
	XMVECTOR	tmp_vec = XMVector3LengthSq( LOAD_FP32_3(a) - LOAD_FP32_3(b) );
	return	 XMVector3Less( tmp_vec, XMVectorReplicate( dist_squared ) );
}

FINLINE	void	build_point_v3_xna( FP32* CONST dst, FP32 CONST * CONST o, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST u, FP32 CONST v )
{
	STORE_FP32_3( dst, LOAD_FP32_3(o) + LOAD_FP32_3(a) * XMVectorReplicate( u ) + LOAD_FP32_3(b) * XMVectorReplicate( v ) );
}

FINLINE	void	build_point_v3_xna( FP32* CONST dst, FP32 CONST * CONST o, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST u, FP32 CONST v, FP32 CONST w )
{
	STORE_FP32_3( dst, LOAD_FP32_3(o) + LOAD_FP32_3(a) * XMVectorReplicate( u ) + LOAD_FP32_3(b) * XMVectorReplicate( v ) + LOAD_FP32_3(c) * XMVectorReplicate( w ) );
}

FINLINE	void	build_point_v3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST u, FP32 CONST v, FP32 CONST w )
{
	STORE_FP32_3( dst, LOAD_FP32_3(a) * XMVectorReplicate( u ) + LOAD_FP32_3(b) * XMVectorReplicate( v ) + LOAD_FP32_3(c) * XMVectorReplicate( w ) );
}

// Catmull-Rom Curve calculations
FINLINE	void	cvCatmullRom_3_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d, FP32 CONST t )
{
	STORE_FP32_3( dst, XMVectorCatmullRom( LOAD_FP32_3(a), LOAD_FP32_3(b), LOAD_FP32_3(c), LOAD_FP32_3(d), t ) );
}
FINLINE	void	cvCatmullRom_2_xna( FP32* CONST dst, FP32 CONST * CONST a, FP32 CONST * CONST b, FP32 CONST * CONST c, FP32 CONST * CONST d, FP32 CONST t )
{
	STORE_FP32_2( dst, XMVectorCatmullRom( LOAD_FP32_2(a), LOAD_FP32_2(b), LOAD_FP32_2(c), LOAD_FP32_2(d), t ) );
}

FINLINE	void	matrix_44_add_xna( FP32* CONST dst, FP32 CONST * CONST a )
{
	if( mem::IS_ALIGNED_16_NOT( dst ) || mem::IS_ALIGNED_16_NOT( a ) )
	{
		XMStoreFloat4(	(XMFLOAT4 *)dst,		XMLoadFloat4 ( (XMFLOAT4 *)dst )		+ XMLoadFloat4 ( (XMFLOAT4 *)a		)	);
		XMStoreFloat4(	(XMFLOAT4 *)(dst+4),	XMLoadFloat4 ( (XMFLOAT4 *)(dst+4) )	+ XMLoadFloat4 ( (XMFLOAT4 *)(a+4)	)	);
		XMStoreFloat4(	(XMFLOAT4 *)(dst+8),	XMLoadFloat4 ( (XMFLOAT4 *)(dst+8) )	+ XMLoadFloat4 ( (XMFLOAT4 *)(a+8)	)	);
		XMStoreFloat4(	(XMFLOAT4 *)(dst+12),	XMLoadFloat4 ( (XMFLOAT4 *)(dst+12) )	+ XMLoadFloat4 ( (XMFLOAT4 *)(a+12) )	);
	}																								   
	else
	{
		XMStoreFloat4A(	(XMFLOAT4A*)dst,		XMLoadFloat4A( (XMFLOAT4A*)dst )		+ XMLoadFloat4A( (XMFLOAT4A*)a		)	);
		XMStoreFloat4A(	(XMFLOAT4A*)(dst+4),	XMLoadFloat4A( (XMFLOAT4A*)(dst+4) )	+ XMLoadFloat4A( (XMFLOAT4A*)(a+4)	)	);
		XMStoreFloat4A(	(XMFLOAT4A*)(dst+8),	XMLoadFloat4A( (XMFLOAT4A*)(dst+8) )	+ XMLoadFloat4A( (XMFLOAT4A*)(a+8)	)	);
		XMStoreFloat4A(	(XMFLOAT4A*)(dst+12),	XMLoadFloat4A( (XMFLOAT4A*)(dst+12) )	+ XMLoadFloat4A( (XMFLOAT4A*)(a+12) )	);
	}
}

FINLINE	void	matrix_43_mul_v3_xna( FP32 CONST * CONST m, FP32 * CONST dst, FP32 CONST x, FP32 CONST y, FP32 CONST z )
{
	if( mem::IS_ALIGNED_16_NOT( dst ) || mem::IS_ALIGNED_16_NOT( m ) )
		XMStoreFloat3 ((XMFLOAT3 *)dst, XMVector3Transform( XMVectorSet( x,y,z, .0 ), XMLoadFloat4x4( (XMFLOAT4X4 *)m ) ) );
	else
		XMStoreFloat3A((XMFLOAT3A*)dst, XMVector3Transform( XMVectorSet( x,y,z, .0 ), XMLoadFloat4x4( (XMFLOAT4X4A*)m ) ) );
}

FINLINE	void	matrix_43_mul_v3_xna( FP32 CONST * CONST m, FP32* CONST dst )
{
	if( mem::IS_ALIGNED_16_NOT( dst ) || mem::IS_ALIGNED_16_NOT( m ) )
		XMStoreFloat3 ((XMFLOAT3 *)dst, XMVector3Transform( XMLoadFloat3 ( (XMFLOAT3 *)dst ), XMLoadFloat4x4( (XMFLOAT4X4 *)m ) ) );
	else
		XMStoreFloat3A((XMFLOAT3A*)dst, XMVector3Transform( XMLoadFloat3A( (XMFLOAT3A*)dst ), XMLoadFloat4x4( (XMFLOAT4X4A*)m ) ) );
}
FINLINE	void	matrix_43_mul_v3_xna( FP32 CONST * CONST m, FP32* CONST dst, FP32 CONST * CONST src )
{
	if( mem::IS_ALIGNED_16_NOT( dst ) || mem::IS_ALIGNED_16_NOT( m ) || mem::IS_ALIGNED_16_NOT( src ) )
		XMStoreFloat3( (XMFLOAT3 *)dst, XMVector3Transform( XMLoadFloat3 ( (XMFLOAT3 *)src ), XMLoadFloat4x4( (XMFLOAT4X4 *)m ) ) );
	else
		XMStoreFloat3A((XMFLOAT3A*)dst, XMVector3Transform( XMLoadFloat3A( (XMFLOAT3A*)src ), XMLoadFloat4x4( (XMFLOAT4X4A*)m ) ) );
}

FINLINE	void matrix44_identity_xna( FP32* CONST dst )
{
	if( mem::IS_ALIGNED_16_NOT( dst ) )
		XMStoreFloat4x4( (XMFLOAT4X4 *)dst, XMMatrixIdentity() );
	else
		XMStoreFloat4x4A((XMFLOAT4X4A*)dst, XMMatrixIdentity() );
}

FINLINE	void matrix_44_invert_xna( FP32* CONST dst, FP32 CONST * CONST src )
{
	if( mem::IS_ALIGNED_16_NOT( dst ) || mem::IS_ALIGNED_16_NOT( src ) )
	{
		XMVECTOR	determinent;
		XMStoreFloat4x4( (XMFLOAT4X4*)dst, XMMatrixInverse( &determinent, XMLoadFloat4x4( (XMFLOAT4X4*)src ) ) );
	}
	else
	{
		XMVECTOR	determinent;
		XMStoreFloat4x4A( (XMFLOAT4X4A*)dst, XMMatrixInverse( &determinent, XMLoadFloat4x4A( (XMFLOAT4X4A*)src ) ) );
	}
}

