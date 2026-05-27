
#ifdef AAA_OPENCL_TYPES_H
#error "OPENCL_TYPES_H included more than once."
#endif
#define AAA_OPENCL_TYPES_H 1


#include "aaa_prec.h"
//---------------------------------------------------------
class	float2
{
public:
	cl_float	x, y;
	
	float2()	{}
	
	float2( float _x, float _y)
		{
			x = _x;
			y = _y;
		}
	float2( const float2 & pnt)
		{
			x = pnt.x;
			y = pnt.y;
		}
	void set(float _x, float _y)
		{
			x = _x;
			y = _y;
		}
	
	//------ Operators:
	//Negative
	float2 operator-() const
	{
		return float2( -x, -y);
	}
	//equality
	bool operator==( const float2& pnt )
	{
	return (x == pnt.x) && (y == pnt.y);
	}
	//inequality
	bool operator!=( const float2& pnt )
	{
		return (x != pnt.x) || (y != pnt.y);
	}
	//Set
	float2 & operator=( const float2& pnt )
	{
		x = pnt.x;
		y = pnt.y;
		return *this;
	}
	float2 & operator=( const float& val )
	{
		x = val;
		y = val;
		return *this;
	}
	// Add
	float2 operator+( const float2& pnt ) const
	{
		return float2( x+pnt.x, y+pnt.y);
	}
	float2 operator+( const float& val ) const
	{
		return float2( x+val, y+val);
	}
	float2 & operator+=( const float2& pnt )
	{
		x+=pnt.x;
		y+=pnt.y;
		return *this;
	}
	float2 & operator+=( const float & val )
	{
		x+=val;
		y+=val;
		return *this;
	}
	// Subtract
	float2 operator-(const float2& pnt) const
	{
		return float2( x-pnt.x, y-pnt.y);
	}
	float2 operator-(const float& val) const
	{
		return float2( x-val, y-val);
	}
	float2 & operator-=( const float2& pnt )
	{
		x -= pnt.x;
		y -= pnt.y;
		return *this;
	}
	float2 & operator-=( const float & val )
	{
		x -= val;
		y -= val;
		return *this;
	}
	// Multiply
	float2 operator*( const float2& pnt ) const
	{
		return float2( x*pnt.x, y*pnt.y);
	}
	float2 operator*(const float& val) const
	{
		return float2( x*val, y*val);
	}
	float2 & operator*=( const float2& pnt )
	{
		x*=pnt.x;
		y*=pnt.y;
		return *this;
	}
	float2 & operator*=( const float & val ) {
		x*=val;
		y*=val;
		return *this;
	}
	// Divide
	float2 operator/( const float2& pnt ) const
	{
		return float2( pnt.x!=0 ? x/pnt.x : x , pnt.y!=0 ? y/pnt.y : y);
	}
	float2 operator/( const float &val ) const
	{
		if( val != 0)
		{
			return float2( x/val, y/val);
		}
		return float2(x, y);
	}
	float2& operator/=( const float2& pnt )
	{
		pnt.x!=0 ? x/=pnt.x : x;
		pnt.y!=0 ? y/=pnt.y : y;
		return *this;
	}
	float2& operator/=( const float &val )
	{
		if( val != 0 )
		{
			x /= val;
			y /= val;
		}
		return *this;
	}
};

inline float2 operator+( float f, const float2& vec )
{
	return float2( f+vec.x, f+vec.y);
}
inline float2 operator-( float f, const float2& vec )
{
	return float2( f-vec.x, f-vec.y);
}
inline float2 operator*( float f, const float2& vec )
{
	return float2( f*vec.x, f*vec.y );
}
inline float2 operator/( float f, const float2& vec )
{
	return float2( f/vec.x, f/vec.y);
}


//---------------------------------------------------------
class	float4
{
public:
	cl_float	x, y, z, w;

	float4()	{}
	
	float4( float _x, float _y, float _z, float _w = 0.0f)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	float4( const float4 & pnt)
	{
		x = pnt.x;
		y = pnt.y;
		z = pnt.z;
		w = pnt.w;
	}
	void set(float _x, float _y, float _z, float _w = 0.0f)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	
	//------ Operators:
	//Negative
	float4 operator-() const
	{
	return float4( -x, -y, -z, -w );
	}
	//equality
	bool operator==( const float4& pnt )
	{
		return (x == pnt.x) && (y == pnt.y) && (z == pnt.z) && (w == pnt.w);
	}
	//inequality
	bool operator!=( const float4& pnt )
	{
		return (x != pnt.x) || (y != pnt.y) || (z != pnt.z) || (w != pnt.w);
	}
	//Set
	float4 & operator=( const float4& pnt )
	{
		x = pnt.x;
		y = pnt.y;
		z = pnt.z;
		w = pnt.w;
		return *this;
	}
	float4 & operator=( const float& val )
	{
		x = val;
		y = val;
		z = val;
		w = val;
		return *this;
	}
	// Add
	float4 operator+( const float4& pnt ) const
	{
		return float4( x+pnt.x, y+pnt.y, z+pnt.z, w+pnt.w );
	}
	float4 operator+( const float& val ) const
	{
		return float4( x+val, y+val, z+val, w+val );
	}
	float4 & operator+=( const float4& pnt )
	{
		x+=pnt.x;
		y+=pnt.y;
		z+=pnt.z;
		w+=pnt.w;
		return *this;
	}
	float4 & operator+=( const float & val )
	{
		x+=val;
		y+=val;
		z+=val;
		w+=val;
		return *this;
	}
	// Subtract
	float4 operator-(const float4& pnt) const
	{
		return float4( x-pnt.x, y-pnt.y, z-pnt.z, w-pnt.w );
	}
	float4 operator-(const float& val) const
	{
		return float4( x-val, y-val, z-val, w-val);
	}
	float4 & operator-=( const float4& pnt )
	{
		x -= pnt.x;
		y -= pnt.y;
		z -= pnt.z;
		w -= pnt.w;
		return *this;
	}
	float4 & operator-=( const float & val )
	{
		x -= val;
		y -= val;
		z -= val;
		w -= val;
		return *this;
	}
	// Multiply
	float4 operator*( const float4& pnt ) const
	{
		return float4( x*pnt.x, y*pnt.y, z*pnt.z, w*pnt.w );
	}
	float4 operator*(const float& val) const
	{
		return float4( x*val, y*val, z*val, w*val);
	}
	float4 & operator*=( const float4& pnt )
	{
		x*=pnt.x;
		y*=pnt.y;
		z*=pnt.z;
		w*=pnt.w;
		return *this;
	}
	float4 & operator*=( const float & val )
	{
		x*=val;
		y*=val;
		z*=val;
		w*=val;
		return *this;
	}
	// Divide
	float4	operator/( const float4& pnt ) const
	{
		return float4( pnt.x!=0 ? x/pnt.x : x , pnt.y!=0 ? y/pnt.y : y, pnt.z!=0 ? z/pnt.z : z, pnt.w!=0 ? w/pnt.w : w );
	}
	float4	operator/( const float &val ) const
	{
		if( val != 0){
			return float4( x/val, y/val, z/val, w/val );
		}
		return float4(x, y, z, w );
	}
	float4& operator/=( const float4& pnt )
	{
		pnt.x!=0 ? x/=pnt.x : x;
		pnt.y!=0 ? y/=pnt.y : y;
		pnt.z!=0 ? z/=pnt.z : z;
		pnt.w!=0 ? w/=pnt.w : w;
		return *this;
	}
	float4& operator/=( const float &val )
	{
		if( val != 0 ){
			x /= val;
			y /= val;
			z /= val;
			w /= val;
		}
		return *this;
	}
};


inline	float4	operator+( float f, const float4& vec )
{
	return float4( f+vec.x, f+vec.y, f+vec.z, f+vec.w );
}
inline	float4	operator-( float f, const float4& vec )
{
	return float4( f-vec.x, f-vec.y, f-vec.z, f-vec.w );
}
inline	float4	operator*( float f, const float4& vec )
{
	return float4( f*vec.x, f*vec.y, f*vec.z, f*vec.w );
}
inline	float4	operator/( float f, const float4& vec )
{
	return float4( f/vec.x, f/vec.y, f/vec.z, f/vec.w);
}

//---------------------------------------------------------
//	not an opencl type
class	float3
{
public:
	cl_float	x, y, z;

	float3()	{}

	float3( float _x, float _y, float _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}
	float3( const float3 & pnt)
	{
		x = pnt.x;
		y = pnt.y;
		z = pnt.z;
	}
	void set(float _x, float _y, float _z )
	{
		x = _x;
		y = _y;
		z = _z;
	}

	//------ Operators:
	//Negative
	float3 operator-() const
	{
		return float3( -x, -y, -z );
	}
	//equality
	bool operator==( const float3& pnt )
	{
		return (x == pnt.x) && (y == pnt.y) && (z == pnt.z);
	}
	//inequality
	bool operator!=( const float3& pnt )
	{
		return (x != pnt.x) || (y != pnt.y) || (z != pnt.z);
	}
	//Set
	float3 & operator=( const float3& pnt )
	{
		x = pnt.x;
		y = pnt.y;
		z = pnt.z;
		return *this;
	}
	float3 & operator=( const float& val )
	{
		x = val;
		y = val;
		z = val;
		return *this;
	}
	// Add
	float3 operator+( const float3& pnt ) const
	{
		return float3( x+pnt.x, y+pnt.y, z+pnt.z );
	}
	float3 operator+( const float& val ) const
	{
		return float3( x+val, y+val, z+val );
	}
	float3 & operator+=( const float3& pnt )
	{
		x+=pnt.x;
		y+=pnt.y;
		z+=pnt.z;
		return *this;
	}
	float3 & operator+=( const float & val )
	{
		x+=val;
		y+=val;
		z+=val;
		return *this;
	}
	// Subtract
	float3 operator-(const float3& pnt) const
	{
		return float3( x-pnt.x, y-pnt.y, z-pnt.z);
	}
	float3 operator-(const float& val) const
	{
		return float3( x-val, y-val, z-val);
	}
	float3 & operator-=( const float3& pnt )
	{
		x -= pnt.x;
		y -= pnt.y;
		z -= pnt.z;
		return *this;
	}
	float3 & operator-=( const float & val )
	{
		x -= val;
		y -= val;
		z -= val;
		return *this;
	}
	// Multiply
	float3 operator*( const float3& pnt ) const
	{
		return float3( x*pnt.x, y*pnt.y, z*pnt.z );
	}
	float3 operator*(const float& val) const
	{
		return float3( x*val, y*val, z*val );
	}
	float3 & operator*=( const float3& pnt )
	{
		x*=pnt.x;
		y*=pnt.y;
		z*=pnt.z;
		return *this;
	}
	float3 & operator*=( const float & val )
	{
		x*=val;
		y*=val;
		z*=val;
		return *this;
	}
	// Divide
	float3	operator/( const float3& pnt ) const
	{
		return float3( pnt.x!=0 ? x/pnt.x : x , pnt.y!=0 ? y/pnt.y : y, pnt.z!=0 ? z/pnt.z : z );
	}
	float3	operator/( const float &val ) const
	{
		if( val != 0){
			return float3( x/val, y/val, z/val );
		}
		return float3(x, y, z );
	}
	float3& operator/=( const float3& pnt )
	{
		pnt.x!=0 ? x/=pnt.x : x;
		pnt.y!=0 ? y/=pnt.y : y;
		pnt.z!=0 ? z/=pnt.z : z;
		return *this;
	}
	float3& operator/=( const float &val )
	{
		if( val != 0 ){
			x /= val;
			y /= val;
			z /= val;
		}
		return *this;
	}
};


inline	float3	operator+( float f, const float3& vec )
{
	return float3( f+vec.x, f+vec.y, f+vec.z );
}
inline	float3	operator-( float f, const float3& vec )
{
	return float3( f-vec.x, f-vec.y, f-vec.z );
}
inline	float3	operator*( float f, const float3& vec )
{
	return float3( f*vec.x, f*vec.y, f*vec.z );
}
inline	float3	operator/( float f, const float3& vec )
{
	return float3( f/vec.x, f/vec.y, f/vec.z );
}

