
#ifdef AAA_PARAMS_H
#error "PARAMS_H included more than once."
#endif
#define AAA_PARAMS_H 1


#ifndef	AAA_PARAM_H
#	include "infrastructure/param/param.h"
#endif
#ifndef _UTILITY_
#	include <utility>		//	for std::index_sequence used by the unrolled v<N> templates
#endif

#if AAA_STATE_COMPILE()
#	ifndef AAA_STATE_H
#		include "infrastructure/obj/state.h"
#	endif
#	ifndef _MAP_
#		include <map>
#	endif
class	c_param_state_base;
#endif //AAA_STATE_COMPILE


class c_params final
{
public:
	c_params();
	~c_params();
private:
	c_param*	_param_array;		//	pointer to an array of param (new c_param[]). first is param_header
	INT32		_param_nb_allocated;
	INT32		_param_nb_used;
#if AAA_STATE_COMPILE()
	std::map< p_param, c_param_state_base* >*	_state_map;
#endif //AAA_STATE_COMPILE

public:
	//PARAM
private:
			c_param*	alloc_param( INT32 CONST nb );
			void		dealloc_params();
//			void		set_param_header_low(	c_param* param )				;
//			void		set_param_header(		c_param* param, c_obj_ui* obj )	;

public:
			void		init_for_obj(		c_obj_ui* obj,	INT32 nb	);	//	we pass nb because of trax
			void		err_param_init_pt(	INT32 nb_in					);

			void		remove_refs();

							FINLINE	void	set_pt_null(	INT32& h		)							CONST;
									void	set_pt_null_vn(	INT32& h,		INT32 nb	)				CONST;

	template	<class T>	FINLINE	void	set_pt(			INT32& h,		T& pt		)				CONST;
	template	<class T>	FINLINE	void	set_pt(			INT32& h,		T* pt		)				CONST;
	template	<class T>	FINLINE	void	set_pt_no_inc(	INT32 CONST h,	T* pt		)				CONST;
	template	<class T>			void	set_pt_vn(		INT32& h,		T* pt,		INT32 nb	)	CONST;

	//	Compile-time-N variants. The fold expression on std::index_sequence guarantees
	//	full unrolling at every optimisation level (Debug included), unlike a runtime
	//	for-loop. Same machine code as the legacy hand-rolled set_pt_v2..v5 etc.
	template	<INT32 N, class T>	FINLINE	void	set_pt_v(			INT32& h,		T* CONST pt	)	CONST;
	template	<INT32 N>			FINLINE	void	set_pt_null_v(		INT32& h	)					CONST;
	template	<INT32 N>			FINLINE	void	set_unused_v(		INT32 CONST h,	bool CONST b )	CONST;

	//	Backward-compat aliases : previous fixed-arity API, callers do not need changes.
	template	<class T>	FINLINE	void	set_pt_v2(		INT32& h,		T* CONST pt	)	CONST	{ set_pt_v<2>(h, pt); }
	template	<class T>	FINLINE	void	set_pt_v3(		INT32& h,		T* CONST pt	)	CONST	{ set_pt_v<3>(h, pt); }
	template	<class T>	FINLINE	void	set_pt_v4(		INT32& h,		T* CONST pt	)	CONST	{ set_pt_v<4>(h, pt); }
	template	<class T>	FINLINE	void	set_pt_v5(		INT32& h,		T* CONST pt	)	CONST	{ set_pt_v<5>(h, pt); }

	FINLINE	void	set_pt_null_v2(	INT32& h )								CONST	{ set_pt_null_v<2>(h);		}
	FINLINE	void	set_pt_null_v3(	INT32& h )								CONST	{ set_pt_null_v<3>(h);		}
	FINLINE	void	set_pt_null_v4(	INT32& h )								CONST	{ set_pt_null_v<4>(h);		}
	FINLINE	void	set_pt_null_v5(	INT32& h )								CONST	{ set_pt_null_v<5>(h);		}

	FINLINE	void		set_unused(   INT32 CONST h, bool CONST b )				CONST;
	FINLINE	void		set_unused_2( INT32 CONST h, bool CONST b )				CONST	{ set_unused_v<2>(h, b);	}
	FINLINE	void		set_unused_3( INT32 CONST h, bool CONST b )				CONST	{ set_unused_v<3>(h, b);	}
	FINLINE	void		set_unused_4( INT32 CONST h, bool CONST b )				CONST	{ set_unused_v<4>(h, b);	}
	FINLINE	void		set_unused_5( INT32 CONST h, bool CONST b )				CONST	{ set_unused_v<5>(h, b);	}
	FINLINE	void		set_unused_6( INT32 CONST h, bool CONST b )				CONST	{ set_unused_v<6>(h, b);	}
	FINLINE	void		set_unused_n( INT32 CONST h, bool CONST b, INT32 nb )	CONST;

	FINLINE	void		set_max_no_inc(			INT32 CONST h,	REAL CONST in		)	CONST;
	FINLINE	void		attach_obj(				INT32& h,		c_obj_ui* CONST pt	)	CONST;
	FINLINE	void		attach_obj_no_inc(		INT32 CONST h,	c_obj_ui* CONST pt	)	CONST;



//			void		set_value( C_PCHAR param_name, C_PCHAR param_value, bool b_str );
//			void		set_to_ina();
//			void		set_to_def();
	//HEADER
			c_param*	get_param_header()				CONST	{	return _param_array;			}
//			void		set_param( CONST p_param pt, INT32 CONST nb );
	FINLINE	c_param*	get_param_begin()				CONST	{	return _param_array+1;			}
	FINLINE	INT32		get_param_nb_allocated()		CONST	{	return _param_nb_allocated;		}
	FINLINE	INT32		get_param_nb_used()				CONST	{	return _param_nb_used;			}
			void		set_param_nb_used(	INT32 nb	);
			c_param*	get(				INT32 index	)	CONST;
			void		swap_param(			INT32 a,	INT32 b	)	CONST;
			void		swap_param_expand(	INT32 a,	INT32 b	)	CONST;

			CHAR CONST * 	param_read_from_mem(	CHAR CONST * src,	INT32 nb	);
	FINLINE	CHAR CONST * 	param_read_from_mem(	CHAR CONST * src	)			{	return param_read_from_mem(	src, get_param_nb_allocated() );	}
			CHAR*			param_write_to_mem(		CHAR * dst,			INT32 nb	)	CONST;
	FINLINE	CHAR*			param_write_to_mem(		CHAR * dst			)	CONST	{	return param_write_to_mem(	dst, get_param_nb_allocated() );	}

			c_param*	get_param_by_name(			C_PCHAR_C name_in )	CONST;
			INT32		get_param_index_by_name(	C_PCHAR_C name_in ) CONST;

//			bool		is_param_belong(	c_param CONST *	param	) CONST;
			INT32		get_param_index(	c_param CONST *	param	) CONST;

#if AAA_STATE_COMPILE()
private:
	FINLINE	void		store_state(			c_param*	param	);
	FINLINE	void		store_state();
	FINLINE	void		retrieve_state();
	FINLINE	void		retrieve_state_inter();
	FINLINE	void		remove_state();
public:
			void		state_do_action();
#endif //AAA_STATE_COMPILE

			void		cpy_from(		c_params* src, INT32 index_begin, INT32 nb ) CONST;
			void		cpy_from_skip(	c_params* src, INT32 nb_skip_at_begin, INT32 nb_skip_at_end ) CONST;
};

FINLINE	void	c_params::set_pt_null(			INT32& h	) CONST
{
	get(h++)->set_pt_null();
}

template	<class T>
FINLINE	void	c_params::set_pt(				INT32& h,	T& pt	) CONST
{
	get(h++)->set_pt( &pt );
}
template	<class T>
FINLINE	void	c_params::set_pt(				INT32& h,	T* CONST pt	) CONST
{
	get(h++)->set_pt( pt );
}
template	<class T>
FINLINE	void	c_params::set_pt_no_inc(		INT32 CONST h,	T* CONST pt	) CONST
{
	get(h)->set_pt( pt );
}
//	Generic v<N> implementation. The (..., expr) fold expression over an
//	index_sequence forces explicit unrolling of N calls at template instantiation
//	time, so the generated code is identical to a hand-rolled v2/v3/... at every
//	optimisation level. No runtime loop construct is emitted.
template < INT32 N, class T >
FINLINE	void	c_params::set_pt_v( INT32& h, T* CONST pt ) CONST
{
	static_assert( N >= 1, "N must be >= 1" );
	if( pt )
	{
		c_param* p = get(h);
		h += N;
		[&]<size_t... I>( std::index_sequence<I...> )
		{
			(..., p[I].set_pt( pt + I ));
		}( std::make_index_sequence<N>{} );
	}
	else
		set_pt_null_v<N>( h );
}

template < INT32 N >
FINLINE	void	c_params::set_pt_null_v( INT32& h ) CONST
{
	static_assert( N >= 1, "N must be >= 1" );
	c_param* p = get(h);
	h += N;
	[&]<size_t... I>( std::index_sequence<I...> )
	{
		(..., p[I].set_pt_null());
	}( std::make_index_sequence<N>{} );
}

template < INT32 N >
FINLINE	void	c_params::set_unused_v( INT32 CONST h, bool CONST b ) CONST
{
	static_assert( N >= 1, "N must be >= 1" );
	c_param* p = get(h);
	[&]<size_t... I>( std::index_sequence<I...> )
	{
		(..., p[I].set_unused( b ));
	}( std::make_index_sequence<N>{} );
}

template	<class T>
		void	c_params::set_pt_vn(			INT32& h,	T* pt,	INT32 nb	) CONST
{
	if( pt )
	{
		c_param* p = get(h);
		h += nb;
		p->set_pt( pt );
		for( ; --nb>0; )
			(++p)->set_pt( ++pt );
	}
	else
		set_pt_null_vn( h, nb );
}

FINLINE	void	c_params::attach_obj_no_inc(	INT32 CONST h,	c_obj_ui* CONST pt	) CONST
{
	get(h)->attach_obj( pt );
}
FINLINE	void	c_params::attach_obj(			INT32& h,	c_obj_ui* CONST pt	) CONST
{
	attach_obj_no_inc( h, pt );
	++h;
}
FINLINE	void	c_params::set_max_no_inc(		INT32 CONST h,	REAL CONST in	) CONST
{
	get(h)->set_max( in );
}

FINLINE	void	c_params::set_unused( INT32 CONST h, bool CONST b ) CONST
{
	get(h)->set_unused( b );
}

FINLINE	void	c_params::set_unused_n( INT32 CONST h, bool CONST b, INT32 nb ) CONST
{
	c_param* p = get(h);
	p->set_unused( b );
	for( ; --nb>0; )
		(++p)->set_unused( b );
}
