
#ifdef AAA_BBOX_H
#error "BBOX_H included more than once."
#endif
#define AAA_BBOX_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_V_H
#	include "math/v.h"
#endif

class	c_bbox final
{
private:
	// TODO - Default initialization values ?
	FP32	_origin[3];
//	FP32	radius_sphere;
	FP32	_radius;
	FP32	_size_half[3];

public:
	FINLINE	FP32			get_origin_coor( INT32 index )	CONST	{	return _origin[index];				}
	FINLINE	FP32 CONST *	get_origin()					CONST	{	return _origin;						}
	template<typename T>
	FINLINE	void			get_origin( T* pt )				CONST	{	cpy_v3( pt, _origin );				}

	FINLINE	FP32			get_radius()					CONST	{	return _radius;						}
	FINLINE	FP32			get_size_half( INT32 index )	CONST	{	return _size_half[index];			}
	FINLINE FP32 CONST *	get_size_half()					CONST	{	return _size_half;					}

	template<typename T>
	FINLINE	void			get_min(	T* pt )				CONST	{	sub_v3( pt, _origin, _size_half );	}
	template<typename T>
	FINLINE	void			get_max(	T* pt )				CONST	{	add_v3( pt, _origin, _size_half );	}
	template<typename T>
	FINLINE	void			get_size(	T* pt )				CONST	{	scale_v3( pt, _size_half, 2. );		}

			void			compute_radius_from_half();
			void			draw();

	FINLINE	void	set_min_max( FP32* min, FP32* max )
					{
						add_then_scale_v3r( _origin,	min, max, .5f );
						sub_then_scale_v3r( _size_half,	max, min, .5f );
						compute_radius_from_half();	
					}

	FINLINE void	extend_v3r( FP32 CONST * point, INT32 CONST nb )
					{
						FP32 min[3];
						FP32 max[3];
						min_max_v3r( min, max, point, nb );
						set_min_max( min, max );
					}

	// TODO - Maa to add min_max_v3f in v.h/cpp
	//FINLINE void	extend_v3f( FP32 CONST * point, INT32 CONST nb )
	//				{
	//					FP32 min[3];
	//					FP32 max[3];
	//					min_max_v3f( min, max, point, nb );
	//					set_min_max( min, max );
	//				}
};

