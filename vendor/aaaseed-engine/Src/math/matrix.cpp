#include "matrix.h"


/*
void	transform_vector( GLfloat u[4], CONST GLfloat v[4], CONST GLfloat m[16] )
{
	GLfloat v0=v[0], v1=v[1], v2=v[2], v3=v[3];
#define M(row,col)  m[col*4+row]
	u[0] = v0 * M(0,0) + v1 * M(1,0) + v2 * M(2,0) + v3 * M(3,0);
	u[1] = v0 * M(0,1) + v1 * M(1,1) + v2 * M(2,1) + v3 * M(3,1);
	u[2] = v0 * M(0,2) + v1 * M(1,2) + v2 * M(2,2) + v3 * M(3,2);
	u[3] = v0 * M(0,3) + v1 * M(1,3) + v2 * M(2,3) + v3 * M(3,3);
#undef M
}
*/

/*
//todo was in map perhaps useful to be checked (2010)
void	transpose_matrix44( GLdouble src[16], GLdouble dst[16])
{
	for( INT32 i = 0; i < 4; ++i )
	{
		for( INT32 j = 0; j < 4; ++j )
		{
			dst[i*4+j] = src[j*4+i];
		}
	}
}
*/

/*
//todo was in map perhaps useful to be checked (2010)
void	transpose33_matrix44( GLdouble src[16], GLdouble dst[16] )
{
	for( INT32 i = 0; i < 4; ++i )
	{
		for( INT32 j = 0; j < 4; ++j )
		{
			if( i < 3 && j < 3 )
				dst[i * 4 + j] = src[j * 4 + i];
			else
				dst[i * 4 + j] = src[i * 4 + j];
		}
	}
}
*/

/*
//todo was in map perhaps useful to be checked (2010)
GLboolean	invert_matrix( GLdouble src[16], GLdouble inverse[16])
{
	double t;
	INT32 swap;
	GLdouble tmp[4][4];

	matrix44_identity( inverse );

	for( INT32 i = 0; i < 4; ++i )
	{
		for( INT32 j = 0; j < 4; ++j )
		{
			tmp[i][j] = src[i * 4 + j];
		}
	}

	for( INT32 i = 0; i < 4; ++i )
	{
		// look for largest element in column.
		swap = i;
		for( INT32 j = i + 1; j < 4; ++j )
		{
			if( fabs( tmp[j][i] ) > fabs( tmp[i][i] ) )
			{
				swap = j;
			}
		}

		if( swap != i )
		{
			// swap rows.
			for( INT32 k = 0; k < 4; ++k )
			{
				t = tmp[i][k];
				tmp[i][k] = tmp[swap][k];
				tmp[swap][k] = t;

				t = inverse[i * 4 + k];
				inverse[i * 4 + k] = inverse[swap * 4 + k];
				inverse[swap * 4 + k] = t;
			}
		}

		if( tmp[i][i] == 0 )
		{
			// no non-zero pivot.  the matrix is singular, which
			//	shouldn't happen.  This means the user gave us a bad
			//	matrix.
			return GL_FALSE;
		}

		t = tmp[i][i];
		for( INT32 k = 0; k < 4; ++k )
		{
			tmp[i][k] /= t;
			inverse[i * 4 + k] /= t;
		}
		for( INT32 j = 0; j < 4; ++j )
		{
			if( j != i )
			{
				t = tmp[j][i];
				for( INT32 k = 0; k < 4; ++k )
				{
					tmp[j][k] -= tmp[i][k] * t;
					inverse[j * 4 + k] -= inverse[i * 4 + k] * t;
				}
			}
		}
	}
	return GL_TRUE;
}
*/
