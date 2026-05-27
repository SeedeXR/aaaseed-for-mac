
#ifdef AAA_VERTEX_DATA_H
#error "VERTEX_DATA_H included more than once."
#endif
#define AAA_VERTEX_DATA_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


class c_vertex_data
{
private:
	INT32	_nb_point;
	INT32	_nb_normal;
	INT32	_nb_uv;

	// quickly done for bdd_tri
	REAL*	_point_original;		//	for bdd_tri store what come from the file
	REAL*	_point_transformed;		//	for bdd_tri store transformed (translated/scaled) point
	REAL*	_point;					//	keep track of which buffer to use set_transformed which it
	REAL*	_point_deformed;		//	for bdd_tri store deformed point
	bool	_b_transformed;

	REAL*	_normal;
	REAL*	_uv;

public:
	c_vertex_data();
	~c_vertex_data();

		//	AAA_ERR alloc( INT32 CONST nb );
			AAA_ERR	alloc_point( INT32 CONST nb );
			AAA_ERR	alloc_point_transformed();
			AAA_ERR	alloc_point_deformed();
			AAA_ERR	alloc_normal( INT32 CONST nb );
			AAA_ERR	alloc_uv( INT32 CONST nb );
			AAA_ERR alloc( INT32 CONST nb, bool CONST b_normal, bool CONST b_uv );

			void	dealloc_point();
			void	dealloc_normal();
			void	dealloc_uv();
			void	dealloc();

			void	set_transformed( bool CONST in );

	FINLINE	REAL*	get_point()					{	return _point;				}
	FINLINE	REAL*	get_point_original()		{	return _point_original;		}
	FINLINE	REAL*	get_point_transformed()		{	return _point_transformed;	}
	FINLINE	REAL*	get_point_deformed()		{	return _point_deformed;		}
	FINLINE	INT32	get_point_nb() CONST		{	return _nb_point;			}

	FINLINE	REAL*	get_normal()				{	return _normal;				}
	FINLINE	INT32	get_normal_nb()	CONST		{	return _nb_normal;			}

	FINLINE	REAL*	get_uv()					{	return _uv;					}
	FINLINE	INT32	get_uv_nb()	CONST			{	return _nb_uv;				}
};

