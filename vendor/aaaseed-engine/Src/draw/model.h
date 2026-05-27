
#ifdef AAA_MODEL_H
#error "MODEL_H included more than once."
#endif
#define AAA_MODEL_H 1


#ifndef	AAA_APP_H
#	include "infrastructure/layer/app.h"
#endif
#ifndef AAA_V_H
#	include "math/v.h"
#endif


class	c_model final : public c_obj_ui
{
	FACTORY_DECLARE(c_model,c_obj_ui);
public:
	static	c_model*	def;
	static	c_model*	cur;
	static	c_model*	ui;
private:
	REAL	_size_ui[4];
	REAL	_size[3];
	INT32	_axe;
	REAL	_resolution;
protected:
public:
			void	build_sum_up( o_str& o );
	virtual	void	param_init_pt();
	FINLINE	void	update()							{
															::scale_v3( _size, _size_ui, _size_ui[3]*g_app->get_global_size_factor() );
															c_model::cur = this;
														}
	FINLINE	REAL	get_resolution()	CONST			{	return _resolution * g_app->get_global_resolution(); }
	FINLINE UINT32	compute_from_resolution( UINT32 nb )
														{	return MIN( nb, UINT32( nb*get_resolution() ) );	}

	FINLINE	INT32	get_axe()		CONST				{	return _axe; }
	FINLINE	void	get_axes( INT32 &i_u, INT32 &i_v, INT32 &i_axe ) CONST
														{
															i_axe = _axe;
															axe_build_index( i_u, i_v, i_axe );
														}
	FINLINE	void	get_axes_vert( INT32 &i_u, INT32 &i_v, INT32 &i_axe ) CONST
														{
															i_axe = _axe;
															axe_build_index_vert( i_u, i_v, i_axe );
														}
	FINLINE	REAL	get_size_index(INT32 index)	CONST	{	return _size[index];	}
	template< class T >
	FINLINE	void	get_size_v3( T *s )	CONST			{	::cpy_v3( s, _size );	}
	template< class T >
	FINLINE	void	get_size_xyz_v3( T *s )	CONST		{
															switch( _axe )
															{	
															case 0:	::cpy_shift1_v3r( s, _size );	break;
															case 1:	::cpy_shift2_v3r( s, _size );	break;
															case 2:	::cpy_v3(		  s, _size );	break;
															}
														}
	template< class T >
	FINLINE	void	scale_v3( T* s ) CONST				{	::mul_v3( s, _size );			}
	template< class T, class S >
	FINLINE	void	get_size_scaled_v3( T *s, S f )	CONST
														{	::scale_v3( s, _size, f );	}
	template< class T >
	FINLINE	void	get_size_half_v3( T* s )	CONST	{	::scale_v3( s, _size, T(.5) );	}
	template< class T >
	FINLINE	void	get_size_v2( T* s )	CONST			{	::cpy_v2( s, _size );	}
	template< class T >
	FINLINE	void	get_size_half_v2( T* s )	CONST	{	::scale_v2( s, _size, T(.5) );	}
	template< class T >
	FINLINE	void	scale_v2( T* s ) CONST				{	::mul_v2( s, _size );			}

	FINLINE	void	set_size_factor( REAL s )			{	_size_ui[3] = s;			}
	FINLINE	void	set_size_v3r(		REAL* s)		{	::cpy_v3( _size_ui, s );		}
	FINLINE	bool	is_diff_size_v3r(	REAL* s ) CONST	{	return	::is_diff_v3( s, _size );	}

};

