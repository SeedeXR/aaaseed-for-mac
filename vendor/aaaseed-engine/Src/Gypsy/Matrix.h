
#ifdef Gypsy_Matrix_H
#error "Matrix_H included more than once."
#endif
#define Gypsy_Matrix_H 1


// Matrix.h Class for doing operations on matrices and vectors
// Copyright (c) 1999 ID8 Media
// Written by Edwin P. Berlin, Jr., Secret Software

#include <math.h>

#ifndef DLLEXPORT
//maa#define DLLEXPORT		__declspec(dllexport)
#define DLLEXPORT		__cdecl
#endif

//Useful macro for MatrixToEuler - never yields 0
#define FSGN(x)	((x) >= 0 ? 1.f : -1.f)

////////////
// Constants
//#undef PI							//In case it was defined in another file
//#define PI	3.14159265358979

#define RADIANS_TO_DEGREES		(float)(180. / PI)
#define DEGREES_TO_RADIANS		(float)(PI / 180.)


#ifndef __cplusplus

//If you want to link Gypsy.dll with C code, this will let the
//Vector and Matrix members of the Node struct compile
typedef struct {
	float x, y, z;
} Vector;

typedef struct {
	float xx, xy, xz;		//This row is the right vector
	float yx, yy, yz;		//This row is the up vector
	float zx, zy, zz;		//This row is the forward vector
} Matrix;

#else

///////////////
// Vector class

class Vector {
public:
	float x, y, z;

	//////////////
	//Constructors

	///////////////////////////
	//Uninitialized constructor
	FINLINE Vector() {}
	////////////////////////
	//Initialize from floats
	FINLINE Vector(float ax, float ay, float az) :
				  x(ax), y(ay), z(az) {}
	/////////////////////////
	//Initialize from doubles
	FINLINE Vector(double ax, double ay, double az) :
				  x((float)ax), y((float)ay), z((float)az) {}

	///////////
	//Constants

	/////////////
	//Zero Vector
	static const Vector Zero;
	static const Vector XAxis;
	static const Vector YAxis;
	static const Vector ZAxis;

	///////////
	//Functions

	///////////////////////////////////////////
	//Return the vector with unit length in the
	//same direction as this vector
	FINLINE Vector Normalize()
	{
		Vector R = *this;
		float norm = (float)sqrt(x * x + y * y + z * z);
		if(norm != 0.f) R /= norm;
		return R;
	}

	//////////////////////////////////
	//Return the length of this vector
	FINLINE float Length()
	{
		return (float)sqrt(x * x + y * y + z * z);
	}

	/////////////////
	//Vector addition
	//Operator +=
	FINLINE void operator+=(const Vector V)
	{
		//this += V
		x += V.x;
		y += V.y;
		z += V.z;
	}

	/////////////////
	//Vector addition
	//Operator +
	FINLINE Vector operator+(const Vector V)
	{
		Vector R;
		//R = this + V
		R.x = x + V.x;
		R.y = y + V.y;
		R.z = z + V.z;
		return R;
	}

	////////////////////
	//Vector subtraction
	//Operator -=
	FINLINE void operator-=(const Vector V)
	{
		//this -= V
		x -= V.x;
		y -= V.y;
		z -= V.z;
	}

	////////////////////
	//Vector subtraction
	//Operator -
	FINLINE Vector operator-(const Vector V)
	{
		Vector R;
		//R = this - V
		R.x = x - V.x;
		R.y = y - V.y;
		R.z = z - V.z;
		return R;
	}

	/////////////
	//Dot product
	//Operator * (Vector)
	FINLINE float operator*(const Vector V)
	{
		//Result = this dot V
		return x * V.x + y * V.y + z * V.z;
	}

	/////////////////////////////
	//Multiply vector by a scalar
	//Operator *= (Scalar)
	FINLINE void operator*=(const float s)
	{
		//this *= s
		x *= s;
		y *= s;
		z *= s;
	}

	/////////////////////////////
	//Multiply vector by a scalar
	//Operator * (Scalar)
	FINLINE Vector operator*(const float s)
	{
		Vector R(x * s, y * s, z * s);
		return R;
	}

	///////////////////////////
	//Divide vector by a scalar
	//Operator /= (Scalar)
	FINLINE void operator/=(const float s)
	{
		//this /= s
		float d = 1.f / s;
		x *= d;
		y *= d;
		z *= d;
	}

	///////////////////////////
	//Divide vector by a scalar
	//Operator / (Scalar)
	FINLINE Vector operator/(const float s)
	{
		float d = 1.f / s;
		Vector R(x * d, y * d, z * d);
		return R;
	}

	//////////////////////
	//Vector cross product
	//Operator ^ (Cross product)
	FINLINE Vector operator^(const Vector V)
	{
		Vector R;

		R.x = y * V.z - V.y * z;
		R.y = z * V.x - V.z * x;
		R.z = x * V.y - V.x * y;

		return R;
	}
};

///////////////
// Matrix class

//MAA	class DLLEXPORT Matrix {
class Matrix {
public:
	//Rotation matrix
	//The columns are the X, Y, and Z axes
	float xx, xy, xz;		//This row is the right vector
	float yx, yy, yz;		//This row is the up vector
	float zx, zy, zz;		//This row is the forward vector

	//////////////
	//Constructors

	///////////////////////////
	//Uninitialized constructor
	FINLINE Matrix() {}
	////////////////////////
	//Initialize from floats
	FINLINE Matrix(float axx, float axy, float axz,
				  float ayx, float ayy, float ayz,
				  float azx, float azy, float azz) :
			xx(axx), xy(axy), xz(axz),
			yx(ayx), yy(ayy), yz(ayz),
			zx(azx), zy(azy), zz(azz) {}
	/////////////////////////
	//Initialize from doubles
	FINLINE Matrix(double axx, double axy, double axz,
				  double ayx, double ayy, double ayz,
				  double azx, double azy, double azz) :
			xx((float)axx), xy((float)axy), xz((float)axz),
			yx((float)ayx), yy((float)ayy), yz((float)ayz),
			zx((float)azx), zy((float)azy), zz((float)azz) {}
	/////////////////////////////
	//Initialize from row vectors
	FINLINE Matrix(Vector RowX, Vector RowY, Vector RowZ) :
			xx(RowX.x), xy(RowX.y), xz(RowX.z),
			yx(RowY.x), yy(RowY.y), yz(RowY.z),
			zx(RowZ.x), zy(RowZ.y), zz(RowZ.z) {}

	///////////
	//Constants

	/////////////////
	//Identity matrix
	static const Matrix Identity;

	//////////////
	//Initializers

	////////////////////////////////////////////////////////
	//These functions return rotation matrices about 1 angle
	//Rotations are all CCW looking down the axis of rotation
	//(in the minus direction)
	static Matrix RotX(float theta);	//Rotation matrix about X axis
	static Matrix RotY(float theta);	//Rotation matrix about Y axis
	static Matrix RotZ(float theta);	//Rotation matrix about Z axis

	/////////////////////////////////////////////////////////
	//These functions return rotation matrices about 3 angles
	//Matrices are composed as: RotY * RotX * RotZ
	//Since these are applied right first, this is a rotation
	//about Z, then X, then Y
	//Pay attention to the order of the arguments
	static Matrix RotYXZ(float thetay, float thetax, float thetaz);
	//Matrices are composed as: RotZ * RotX * RotY
	//Since these are applied right first, this is a rotation
	//about Y, then X, then Z
	//Pay attention to the order of the arguments
	static Matrix RotZXY(float thetaz, float thetax, float thetay);
	//Matrices are composed as: RotZ * RotY * RotX
	//Since these are applied right first, this is a rotation
	//about X, then Y, then Z
	//Pay attention to the order of the arguments
	static Matrix RotZYX(float thetaz, float thetay, float thetax);

	///////////
	//Functions

	////////////////////////////////////////////////////////////////////
	//Converts a pure rotation matrix into Euler angles in the order YXZ
	//reading right to left.
	//Assumes the matrix is a pure rotation with no scale or shear.
	//There is an ambiguity of the sign of cos(thetax) which we resolve
	//by simply forcing cos(thetaz) >= 0
	//There is an additional ambiguity when cos(thetax) = 0.  This is the
	//gymbal lock condition in which thetay and thetaz are not independent.
	//We resolve this special case by assuming thetay = 0 when cos(thetax) = 0.
	//These are fundamental issues caused by the fact that Euler angles are
	//inherently ill-defined.
	//Pay attention to the order of the arguments
	void MatrixToEulerYXZ(float *ty, float *tx, float *tz);

	////////////////////////////////////////////////////////////////////
	//Converts a pure rotation matrix into Euler angles in the order ZYX
	//reading right to left.
	//Assumes the matrix is a pure rotation with no scale or shear.
	//There is an ambiguity of the sign of cos(thetay) which we resolve
	//by simply forcing cos(thetay) >= 0
	//There is an additional ambiguity when cos(thetay) = 0.  This is the
	//gymbal lock condition in which thetax and thetaz are not independent.
	//We resolve this special case by assuming thetax = 0 when cos(thetay) = 0.
	//These are fundamental issues caused by the fact that Euler angles are
	//inherently ill-defined.
	//Pay attention to the order of the arguments
	void MatrixToEulerZYX(float *tz, float *ty, float *tx);

	////////////////////////////////////
	//Return the transpose of the matrix
	FINLINE Matrix Transpose()
	{
		Matrix R(xx, yx, zx,
				 xy, yy, zy,
				 xz, yz, zz);
		return R;
	}

	//////////////////////////////////
	//Return row vectors of the matrix
	FINLINE Vector RowX() {	return Vector(xx, xy, xz); }
	FINLINE Vector RowY() {	return Vector(yx, yy, yz); }
	FINLINE Vector RowZ() {	return Vector(zx, zy, zz); }

	///////////
	//Operators

	/////////////////
	//Matrix multiply
	//Operator *=
	FINLINE void operator*=(const Matrix B)
	{
		Matrix A(*this);
		//Result = A B

		//Each column of Result = A times each column of B
		xx = A.xx * B.xx + A.xy * B.yx + A.xz * B.zx;
		yx = A.yx * B.xx + A.yy * B.yx + A.yz * B.zx;
		zx = A.zx * B.xx + A.zy * B.yx + A.zz * B.zx;

		xy = A.xx * B.xy + A.xy * B.yy + A.xz * B.zy;
		yy = A.yx * B.xy + A.yy * B.yy + A.yz * B.zy;
		zy = A.zx * B.xy + A.zy * B.yy + A.zz * B.zy;

		xz = A.xx * B.xz + A.xy * B.yz + A.xz * B.zz;
		yz = A.yx * B.xz + A.yy * B.yz + A.yz * B.zz;
		zz = A.zx * B.xz + A.zy * B.yz + A.zz * B.zz;
	}

	/////////////////
	//Matrix multiply
	//Operator * (Matrix)
	FINLINE Matrix operator*(const Matrix B)
	{
		Matrix R;
		//Result = this B

		//Each column of Result = A times each column of B
		R.xx = xx * B.xx + xy * B.yx + xz * B.zx;
		R.yx = yx * B.xx + yy * B.yx + yz * B.zx;
		R.zx = zx * B.xx + zy * B.yx + zz * B.zx;

		R.xy = xx * B.xy + xy * B.yy + xz * B.zy;
		R.yy = yx * B.xy + yy * B.yy + yz * B.zy;
		R.zy = zx * B.xy + zy * B.yy + zz * B.zy;

		R.xz = xx * B.xz + xy * B.yz + xz * B.zz;
		R.yz = yx * B.xz + yy * B.yz + yz * B.zz;
		R.zz = zx * B.xz + zy * B.yz + zz * B.zz;

		return R;
	}

	/////////////////////////////
	//Matrix times a right vector
	//Operator * (Vector)
	FINLINE Vector operator*(const Vector V)
	{
		Vector R;
		//Result = this V

		//Each column of Result = A times each column of B
		R.x = xx * V.x + xy * V.y + xz * V.z;
		R.y = yx * V.x + yy * V.y + yz * V.z;
		R.z = zx * V.x + zy * V.y + zz * V.z;

		return R;
	}
};

#ifndef _USRDLL
//Client program must import static constant members of the above classes
//extern __declspec(dllimport) const Vector Vector::Zero;
//extern __declspec(dllimport) const Vector Vector::XAxis;
//extern __declspec(dllimport) const Vector Vector::YAxis;
//extern __declspec(dllimport) const Vector Vector::ZAxis;
//extern __declspec(dllimport) const Matrix Matrix::Identity;
//This doesn't work because of a bug in the compiler.
//Instead, we'll export and import constant classes
extern __declspec(dllimport) const Vector VectorZero;
extern __declspec(dllimport) const Vector VectorXAxis;
extern __declspec(dllimport) const Vector VectorYAxis;
extern __declspec(dllimport) const Vector VectorZAxis;
extern __declspec(dllimport) const Matrix MatrixIdentity;
#else
extern DLLEXPORT const Vector VectorZero;
extern DLLEXPORT const Vector VectorXAxis;
extern DLLEXPORT const Vector VectorYAxis;
extern DLLEXPORT const Vector VectorZAxis;
extern DLLEXPORT const Matrix MatrixIdentity;
#endif

#endif


