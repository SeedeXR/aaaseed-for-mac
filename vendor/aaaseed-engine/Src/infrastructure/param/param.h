
#ifdef AAA_PARAM_H
#error "PARAM_H included more than once."
#endif
#define AAA_PARAM_H 1


#ifndef AAA_PARAM_MORE_H
#	include "param_more.h"
#endif
#ifndef AAA_PARAM_DATA_H
#	include "param_data.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif


class c_obj_ui;
class c_param_def;
class c_params;
class c_dialog_wrapper;

class c_param final
{
	friend c_params;			// because c_params handle list and in particular deal with swap_param()
	friend c_dialog_wrapper;	// avoid editing twice the same value using get_pt_const_void
	friend c_trax;				// for dangerous access with pt_real, get_pt_const_real3 //todo remove ?

private:
	static	c_obj_ui*	obj_loading;	// set when load_list is used

	void*			_pt;
	c_param_data*	_data;
	c_param_more*	_more;
	UINT32			_flags;		//store flag and index of param in an array

//
//	FLAGS
//
	//	_flags packed UINT32 layout :
	//
	//	  bit  0..9  : 10-bit param index (0..1023). 0 = header, 1 = first real param.
	//	               M_ID = 0x3ff is also used as out-of-range sentinel (see get_id).
	//	  bit 10..19 : reserved.
	//	  bit 20..30 : PARAM_FLAGS bits (DATA_OWNER, REFERENCED, LOCK, LAST, UNUSED,
	//	               EXPAND_OUT, EXPAND_IN, EXPAND_LIST, DBG_DISPLAY, UPDATE_TRIG,
	//	               STATE_SENSITIVE if AAA_STATE_COMPILE).
	//	  bit 31     : reserved.
	static	CONSTEXPR	UINT32	M_ID =	0x3ff;

	// from bit 20 we store flag
	enum PARAM_FLAGS : INT32
	{
		S_DATA_OWNER	= 20,
		S_REFERENCED	,
		S_LOCK			,
		S_LAST			,
		S_UNUSED		,	// not currently used (usually depend on a previous param switch so param_draw will display it differently 

		S_EXPAND_OUT	,	//todo we could move these to more
		S_EXPAND_IN		,
		S_EXPAND_LIST	,
		S_DBG_DISPLAY	,	//todo we could move this to more
		S_UPDATE_TRIG	,

#if AAA_STATE_COMPILE()
		S_STATE_SENSITIVE,
#endif
	};

	static	CONSTEXPR	UINT32	M_DATA_OWNER		=	UINT32(1)<<S_DATA_OWNER;
	static	CONSTEXPR	UINT32	M_REFERENCED		=	UINT32(1)<<S_REFERENCED;
	static	CONSTEXPR	UINT32	M_LOCK				=	UINT32(1)<<S_LOCK;
	static	CONSTEXPR	UINT32	M_LAST				=	UINT32(1)<<S_LAST;
	static	CONSTEXPR	UINT32	M_UNUSED			=	UINT32(1)<<S_UNUSED;

	static	CONSTEXPR	UINT32	M_EXPAND_OUT		=	UINT32(1)<<S_EXPAND_OUT;
	static	CONSTEXPR	UINT32	M_EXPAND_IN			=	UINT32(1)<<S_EXPAND_IN;
	static	CONSTEXPR	UINT32	M_EXPAND_LIST		=   UINT32(1)<<S_EXPAND_LIST;

	static	CONSTEXPR	UINT32	M_EXPAND_ALL		=	M_EXPAND_OUT | M_EXPAND_IN | M_EXPAND_LIST;

	static	CONSTEXPR	UINT32	M_DBG_DISPLAY		=	UINT32(1)<<S_DBG_DISPLAY;
	static	CONSTEXPR	UINT32	M_UPDATE_TRIG		=	UINT32(1)<<S_UPDATE_TRIG;

#if AAA_STATE_COMPILE()
	static	CONSTEXPR	UINT32	M_STATE_SENSITIVE	=	UINT32(1)<<S_STATE_SENSITIVE;
#endif


public:
	static	c_obj_ui*	get_obj_loading()	{	return obj_loading; }
	//	Used by c_fifo::_store to match the current focus state against stored entries
	//	(two header params reference the same controlled data, i.e. the same obj).
	//	Null-safe : returns false if either side is null.
	static bool		is_same_pt(c_param CONST * CONST p1, c_param CONST * CONST p2)
											{ return p1 && p2 && p1->_pt == p2->_pt; }

//	static CONSTEXPR	UINT32	M_DBG_DISPLAY_ALL	= 0xffe00000;
#if	AAA_DEBUG()
private:
	static	INT32	s_check;
public:
	static	void		push_check()		{	++s_check;	}
	static	void		pop_check()			{	--s_check;	}

			void		check( void CONST * CONST pt ) CONST;
#endif
private:
			void			alloc_more();
	FINLINE	c_param_more*	get_more_always()
	{
		if( !_more )
			alloc_more();
		return _more;
	}
	CONSTEXPR	bool	is_data_owner()				CONST	{	return (_flags & M_DATA_OWNER) != 0;	}
				void	set_data_owner();
	CONSTEXPR	void	set_data_owner( bool CONST b )
	{
		if(b)
			_flags |= M_DATA_OWNER;
		else
			_flags &= ~M_DATA_OWNER;
	}

			void		set_id( INT32 index );
			c_param*	get_header()						{	return this - get_id();						}
public:
			INT32		get_id();
	FINLINE	bool		is_header()					CONST	{	return (_flags & M_ID) == 0;				}

	FINLINE	void		clear_lock()						{	_flags &= ~M_LOCK;							}
	FINLINE	void		set_lock()							{	_flags |= M_LOCK;							}
	FINLINE	bool		is_lock()					CONST	{	return (_flags & M_LOCK) != 0;				}

	FINLINE	void		clear_referenced()					{	_flags &= ~M_REFERENCED;					}
	FINLINE	void		set_referenced()					{	_flags |= M_REFERENCED;						}
	FINLINE	bool		is_referenced()				CONST	{	return	(_flags & M_REFERENCED) != 0;		}

	FINLINE	void		set_last()							{	_flags |= M_LAST;							}
	FINLINE	void		set_last( bool CONST b )			{
																if(b)
																	_flags |= M_LAST;
																else
																	_flags &= ~M_LAST;
															}
	FINLINE	bool		is_last()					CONST	{	return (_flags & M_LAST)!=0;				}

	FINLINE	void		set_unused()						{	_flags |= M_UNUSED;							}
	FINLINE	void		set_unused( bool CONST b )			{
																if(b)
																	_flags |= M_UNUSED;
																else
																	_flags &= ~M_UNUSED;
															}
	FINLINE	bool		is_unused()					CONST	{	return (_flags & M_UNUSED)!=0;				}

#if AAA_STATE_COMPILE()		
	FINLINE	void		clear_state_sensitive()				{	_flags &= ~M_STATE_SENSITIVE;				}
	FINLINE	void		set_state_sensitive()				{	_flags |= M_STATE_SENSITIVE;				}
	FINLINE	bool		is_state_sensitive()		CONST	{	return	(_flags & M_STATE_SENSITIVE) != 0;	}
#endif //AAA_STATE_COMPILE

	FINLINE	void		clear_update_trig()					{	_flags &= ~M_UPDATE_TRIG;				}
	FINLINE	void		set_update_trig()					{	_flags |= M_UPDATE_TRIG;				}
	FINLINE	bool		is_update_trig()			CONST	{	return	(_flags & M_UPDATE_TRIG) != 0;	}

public:
	static CONSTEXPR	INT32	UI_STR_LINE_MAX		= 3;

	static	bool		b_verbose;
	static	bool		b_list_load_verbose;

	static	DOUBLE		start_value;
	static	DOUBLE		step_value;

	static	void		c_init();
	static	void		c_deinit();

	static	UINT32*		get_nb_pt();
	static	UINT32*		get_nb_created_pt();
	static	UINT32*		get_nb_created_copy_pt();

	//c_param( c_param_def* );
	c_param();
//	c_param( CHAR* name_in, INT32 type_in );
//	c_param( CHAR* name_in, bool b_in = true );
	~c_param();

//param
#if 1
	C_NO_CPY_MOVE(c_param)
#else 
	//	copy constructor
	c_param( CONST c_param& obj );
	//	copy assignment operator
	c_param& operator=( CONST c_param& obj );
#endif

	template	<class T>
	FINLINE	void	set_pt( T* CONST pt )
	{
#if	AAA_DEBUG()
		check( (void*)pt );
#endif
		_pt = (void*)pt;
	}
	FINLINE	void	set_pt_null()
	{
		_pt = nullptr;
	}
	FINLINE	void	cpy_pt( c_param* src )
	{
		auto pt = src->_pt;
		if( pt )
			set_pt( pt );
		else
			set_pt_null();	// avoid message for null pt
	}

private:	//access to pt is isolated because eventually param will set/get value only thru function
			//todo infact this is a detail of implementation that not other object should deal with
			//  we should remove the access from these objects
	FINLINE	void CONST *	get_pt_const_void()		CONST	{	return _pt;				}	
	FINLINE	bool			is_pt()					CONST	{	return _pt != nullptr;	}
//	FINLINE	void*			get_pt()						{	return _pt;				}
// 
	FINLINE o_str*			get_pt_o_str()			CONST	{	return (o_str*)_pt;								}
	FINLINE o_str CONST &	get_const_o_str()		CONST	{	return *((o_str CONST *)get_pt_const_void());	}
//hack what we do in trax should be checked what happen in fn which copy FP32 to DOUBLE for example
	FINLINE	REAL CONST *	get_pt_const_real3()	CONST	{	return (REAL CONST *) get_pt_const_void();		}
	//dangerous but used for trax
	FINLINE	REAL *			get_pt_real3()			CONST	{	return (REAL *) _pt;							}

public:
	FINLINE bool			is_changeable()					{	return is_pt() && !is_lock();				}	

			void		set_def( REAL CONST in );
			void		set_ina( REAL CONST in );
			void		set_min( REAL CONST in );
			void		set_max( REAL CONST in );
			void		set_limits(	REAL CONST def, REAL CONST ina, REAL CONST min, REAL CONST max );

	FINLINE	REAL		get_def()		CONST				{ return _data->get_def();		}
	FINLINE	REAL		get_ina()		CONST				{ return _data->get_ina();		}
	FINLINE	REAL		get_min()		CONST				{ return _data->get_min();		}
	FINLINE	REAL		get_max()		CONST				{ return _data->get_max();		}
			
private:
	FINLINE INT32		get_int32()		CONST				{ return *((INT32*)		get_pt_const_void());	}
	FINLINE UINT32		get_uint32()	CONST				{ return *((UINT32*)	get_pt_const_void());	}
	FINLINE bool		get_bool()		CONST				{ return *((bool*)		get_pt_const_void());	}
	FINLINE FP32		get_fp32()		CONST				{ return *((FP32*)		get_pt_const_void());	}
	FINLINE DOUBLE		get_double()	CONST				{ return *((DOUBLE*)	get_pt_const_void());	}
	//hack	get_real is not used a lot and should eventually disappear
//	FINLINE REAL		get_real()		CONST				{ return *((REAL *)	get_pt_const_void());	}

	FINLINE	void		get_min_max( REAL& min, REAL& max )	CONST	{	_data->get_min_max( min, max ); }
	template<class T>
	FINLINE	T			clamp( T val ) CONST
						{
							REAL min, max;
							get_min_max( min, max );
							return CLAMP( val, min, max );
						}

	FINLINE	void		set_int32_direct(	INT32	CONST val	);
	FINLINE	void		set_uint32_direct(	UINT32	CONST val	);
	FINLINE	void		set_fp32_direct(	FP32	CONST val	);
//	FINLINE	void		set_real_direct(	REAL	CONST val	);
	FINLINE	void		set_double_direct(	DOUBLE	CONST val	);
	FINLINE	void		set_str_direct(		C_PCHAR_C     val	);
	FINLINE	void		set_str_direct(		o_str CONST & val	);
	FINLINE	void		set_bool_direct(	bool	CONST val	);

//	FINLINE	void		set_int32(			INT32	CONST val )		{	set_int32_direct(	clamp( val ) );	}
//	FINLINE	void		set_uint32(			UINT32	CONST val )		{	set_uint32_direct(	clamp( val ) );	}
//	FINLINE	void		set_fp32(			FP32	CONST val )		{	set_fp32_direct(	clamp( val ) );	}
//	FINLINE	void		set_real(			REAL	CONST val )		{	set_real_direct(	clamp( val ) );	}
//	FINLINE	void		set_double(			DOUBLE	CONST val )		{	set_double_direct(	clamp( val ) );	}

	FINLINE	void		set_data(		c_param_data* CONST data )
																{
																	if( _data )	//use only once
																		debug_break( "in c_param data already set" );
																	_data = data;
																}
	//dangerous for testing only
	FINLINE	c_param_data*	get_data_unsafe_maa()				{	return _data; }

	FINLINE	bool		is_group_closed()				CONST	{	return _data->is_group_closed();				}
	FINLINE	bool		is_group()						CONST	{	return _data->is_group();						}

			void		print_debug(	C_PCHAR_C signature, C_PCHAR_C mess ) CONST;
			void		print_err(		C_PCHAR_C signature, C_PCHAR_C mess ) CONST;
public:
	FINLINE	bool		is_save()						CONST	{	return _data->is_save();						}
			void		set_save( bool CONST b_save );

			void		set_type( UINT32 CONST type_in );

	FINLINE	aaa::param::TYPE			get_type()					CONST	{ return _data->get_type();					}
	FINLINE	aaa::param::TYPE_INTERNAL	get_type_internal()			CONST	{ return _data->get_type_internal();		}
	FINLINE	C_PCHAR_C					get_type_str()				CONST	{ return _data->get_type_str();				}
	FINLINE	C_PCHAR_C					get_type_str_lowercase()	CONST	{ return _data->get_type_str_lowercase();	}

	FINLINE	bool		is_type_text()					CONST	{ return _data->is_type_text();			}
	FINLINE	bool		is_type_number()				CONST	{ return _data->is_type_number();		}
	FINLINE	bool		is_type_real()					CONST	{ return _data->is_type_real();			}
	FINLINE	bool		is_type_bool()					CONST	{ return _data->is_type_bool();			}

	FINLINE	void		clear_obj()								{	if(_more)	_more->clear_obj();			}
	FINLINE	void		set_obj(	c_obj_ui* CONST obj )		{
																	if(obj)
																		get_more_always()->set_obj(obj);
																	else
																		clear_obj();
																}
	FINLINE	bool		is_obj()						CONST	{	return _more ? _more->is_obj() : false;			}	
	FINLINE	c_obj_ui*	get_obj()						CONST	{	return _more ? _more->get_obj() : nullptr;		}	
			c_obj_ui*	get_obj_owner();

	FINLINE	void		set_flag_error()						{	get_more_always()->set_flag_error();			}
	FINLINE	void		clear_flag_error()						{	get_more_always()->clear_flag_error();			}
	FINLINE	void		set_flag_error( bool CONST b_error )	{
																	if( b_error )
																		set_flag_error();
																	else
																		clear_flag_error();
																}
	FINLINE	bool		is_flag_error()					CONST	{	return _more ? _more->is_flag_error() : false;	}

			void		attach_param(	c_param* CONST pt,	INT32 CONST nb );
	//FINLINE	bool	is_param_attached()				CONST	{	return _more ? _more->is_param_attached() : false;		}
	FINLINE	INT32		get_param_attached_nb()			CONST	{	return _more ? _more->get_param_attached_nb() : 0;		}
	FINLINE	c_param*	get_param_attached()			CONST	{	return _more ? _more->get_param_attached() : nullptr;	}


			void		attach_obj(		c_obj_ui* CONST obj );
	FINLINE	c_obj_ui*	get_obj_attached()				CONST	{	return _more ? _more->get_obj_attached() : nullptr;		}


	FINLINE	o_str*		get_comment()					CONST	{	return _more ? _more->get_comment() : nullptr;			}
	FINLINE	o_str*		get_comment_always()					{	return get_more_always()->get_comment_always();			}
	FINLINE	C_PCHAR_C	get_comment_str()				CONST	{	return _more ? _more->get_comment_str() : nullptr;		}
	FINLINE	void		clear_comment()							{
																	if( _more )
																		_more->clear_comment();	
																}
			void		set_comment( C_PCHAR_C str );
			void		set_comment( o_str CONST & o );
//			void		set_comment( o_str* o );
			void		set_comment( o_str CONST & o,	bool CONST b_error );
			void		set_comment( C_PCHAR_C str,		bool CONST b_error );

	FINLINE	void		set_comment_bool(	bool CONST b )		{	set_comment( b ? "ON" : nullptr );				}
	FINLINE	void		set_comment_on_off(	bool CONST b )		{	set_comment( b ? "ON" : "OFF" );				}
	FINLINE	void		set_comment_int32(	INT32 CONST val )	{	get_more_always()->set_comment_int32( val );	}
	FINLINE	void		set_comment_real(	REAL CONST val )	{	get_more_always()->set_comment_real( val );		}
	FINLINE	void		set_comment_double(	DOUBLE CONST val )	{	get_more_always()->set_comment_double( val );	}

			bool		is_pluggable()	CONST	;	

	FINLINE	C_PCHAR*	get_symbolic_str()								CONST	{	return (C_PCHAR*)	_data->get_symbolic_str();				}
	FINLINE	C_PCHAR		get_symbolic_str_direct( INT32 CONST index )	CONST	{	return (C_PCHAR)	_data->get_symbolic_str_direct(index);	}
	FINLINE	C_PCHAR		get_symbolic_str(		 INT32 CONST index )	CONST	{	return (C_PCHAR)	_data->get_symbolic_str(index);			}

	FINLINE	c_factory_group* get_factory_group()	CONST	{	return _data->get_factory_group();	}

			void			set_name( o_str CONST & name_in );
			void			set_name( C_PCHAR_C name_in );
	FINLINE	o_str CONST & 	get_name()						CONST	{	return _data->get_name();				}

			void		clear();
//			void		set( void* pt, INT32 type, CHAR * str, REAL def=0., REAL ina=0., REAL min=0., REAL max=0. );
			void		set( CONST c_param_def* CONST p_st_param );

			void		send_change();

	static	INT32		sprint_ui( CHAR* dst, INT32 len, C_PCHAR_C name, INT32 CONST type, CP_CVOID pt, C_PCHAR_C* CONST symbolic = nullptr );
	//todo refine for lua
// GET
			INT32		get_value_as_int32()						CONST;
			UINT32		get_value_as_uint32()						CONST;
			bool		get_value_as_bool()							CONST;
#if	!AAA_REAL_IS_DOUBLE()
			REAL		get_value_as_real()							CONST;
#endif
			DOUBLE		get_value_as_double()						CONST;
			void		get_value_as_str(				o_str& o )	CONST;
// SET
			void		set_value_str_direct(			o_str CONST & o	);
			void		set_value_str_direct(			C_PCHAR_C value );

			void		set_value_str(					o_str CONST & o	);
			void		set_value_str(					C_PCHAR_C value );
			void		set_value_str_from_double(		C_PCHAR_C format, DOUBLE CONST value );

			AAA_ERR		set_value_from_str(				C_PCHAR_C str	);
/*
#if	!AAA_REAL_IS_DOUBLE()
			void		set_value_num_from_real(		REAL	CONST value	);
#endif
*/
			void		set_value_num_from_double(		DOUBLE	CONST value	);
//			void		set_value_num_from_int32(		INT32	CONST value	);
//			void		set_value_num_from_uint32(		UINT32	CONST value	);
			void		set_value_num_from_bool(		bool	CONST value	);

			void		cpy_value_from_param_type_same(	c_param CONST * CONST src	);

			void		set_value_to_def();
			void		set_value_to_ina();

			void		change_value_for_ui(			DOUBLE CONST in );
	FINLINE bool		is_ina()	CONST	;

			bool		do_action(						aaa::param::ACTION	CONST action );

			INT32		get_display_line_nb();

			bool		sprint(							CHAR* str, INT32 CONST len );
public:
			c_param*	draw(							CHAR sy_cont );

			void		save_fprint(					CHAR*&			dst	);
			CHAR*		write_to_mem(					CHAR*			dst	)	CONST;
			CHAR CONST*	read_from_mem(					CHAR CONST *	src	);

#if AAA_STATE_COMPILE()
			bool		need_state_store();
#endif
	FINLINE bool		need_save()	CONST;

	FINLINE	AAA_ERR		save(	CHAR* &dst	);


//CONNEXION
	FINLINE	bool			is_out()						CONST	{ return _more ? _more->is_out()		: false;	}
	FINLINE	INT32			get_out_nb()					CONST	{ return _more ? _more->get_out_nb()	: 0;		}
	FINLINE	c_connex*		get_out( INT32 CONST index )	CONST	{ return _more ? _more->get_out(index)	: nullptr;	}
	FINLINE	c_connex::LIST*	get_out()						CONST	{ return _more ? _more->get_out()		: nullptr;	}
																	  
	FINLINE	bool			is_in()							CONST	{ return _more ? _more->is_in()			: false;	}
	FINLINE	INT32			get_in_nb()						CONST	{ return _more ? _more->get_in_nb()		: 0;		}
	FINLINE	c_connex*		get_in( INT32 CONST index )		CONST	{ return _more ? _more->get_in(index)	: nullptr;	}
	FINLINE	c_connex::LIST*	get_in()						CONST	{ return _more ? _more->get_in()		: nullptr;	}


//	BORROW : install a c_connex::LIST* allocated and owned by another param (typically a
//	trax_header, mirrored into a c_traxs UI proxy slot). The receiving _more does NOT own the
//	list, and unborrow_in/unborrow_out MUST be called before the receiver is destroyed,
//	otherwise the destructor will SAFE_DELETE a list it does not own.
//	See Src/infrastructure/param/CLAUDE.md "trax_header borrow" for the design rationale and
//	the pending cleanup deferred to the trax/traxs review.
	FINLINE	void		borrow_in(	c_connex::LIST* CONST in  )
	{
#if AAA_DEBUG()
		if( !in )
			debug_break( "%s() borrow with null, use unborrow_in() instead", __FUNCTION__ );
#endif
		get_more_always()->borrow_in( in );
	}
	FINLINE	void		borrow_out(	c_connex::LIST* CONST out )
	{
#if AAA_DEBUG()
		if( !out )
			debug_break( "%s() borrow with null, use unborrow_out() instead", __FUNCTION__ );
#endif
		get_more_always()->borrow_out( out );
	}
	FINLINE	void		unborrow_in()								{ if(_more) _more->borrow_in ( nullptr ); }
	FINLINE	void		unborrow_out()								{ if(_more) _more->borrow_out( nullptr ); }
															  
public:														  
	FINLINE AAA_ERR		add_in(		c_connex * CONST con )	{ return get_more_always()->add_in( con );	}	//par move to plug vocabulary
	FINLINE AAA_ERR		add_out(	c_connex * CONST con )	{ return get_more_always()->add_out( con );	}
	FINLINE void		remove_in(	c_connex * CONST con )	{ if(_more) _more->remove_in( con );		}	// should be called only when there is one
	FINLINE void		remove_out( c_connex * CONST con )	{ if(_more) _more->remove_out( con );		}	// should be called only when there is one

	FINLINE	c_obj_ui*	find_obj_owner()
	{
		return _more ? _more->find_obj_owner() : nullptr;
	}

			void		disconnect();
//
//	PARAM EXPAND
//
	FINLINE	INT32		get_expand_out_nb()		CONST	{	return	( _flags & M_EXPAND_OUT	 )	? get_out_nb() : 0;				}
	FINLINE	bool		is_expand_out()			CONST	{	return	( _flags & M_EXPAND_OUT	 )	!= 0;							}
	FINLINE	INT32		get_expand_in_nb()		CONST	{	return	( _flags & M_EXPAND_IN	 )	? get_in_nb() : 0;				}

	FINLINE	bool		is_expand_in()			CONST	{	return	( _flags & M_EXPAND_IN	 )	!= 0;							}
	FINLINE	INT32		get_expand_list_nb()	CONST	{	return	( _flags & M_EXPAND_LIST )	? get_param_attached_nb() : 0;	}
	FINLINE	bool		is_expand_list()		CONST	{	return	( _flags & M_EXPAND_LIST )	!= 0;							}

	FINLINE	bool		is_expand()				CONST	{	return	( _flags & M_EXPAND_ALL  )	!= 0;							}
//	FINLINE	INT32		get_expand_nb()			CONST	{	return is_expand() ? get_expand_out_nb() + get_expand_in_nb() + get_expand_list_nb() : 0; }

	FINLINE	void		enable_expand_out		()		{	_flags |= M_EXPAND_OUT;		}
	FINLINE	void		enable_expand_in		()		{	_flags |= M_EXPAND_IN;		}
	FINLINE	void		enable_expand_list		()		{	_flags |= M_EXPAND_LIST;	}
	FINLINE	void		enable_expand_all		()		{	_flags |= M_EXPAND_ALL;		}
						
	FINLINE	void		disable_expand_out		()		{	_flags &= ~M_EXPAND_OUT;	}
	FINLINE	void		disable_expand_in		()		{	_flags &= ~M_EXPAND_IN;		}
	FINLINE	void		disable_expand_list		()		{	_flags &= ~M_EXPAND_LIST;	}
	FINLINE	void		disable_expand_all		()		{	_flags &= ~M_EXPAND_ALL;	}

	FINLINE	void		set_expand_list( bool CONST b )
						{
							if( b )
								enable_expand_list();
							else
								disable_expand_list();
						}

			void		flip_expand_out			();
			void		flip_expand_in			();
			void		flip_expand_list		();
			void		flip_expand_all			();
//
//	PARAM DBG DISPLAY
//
//param
	FINLINE	bool		is_dbg_display()		CONST	{	return (_flags & M_DBG_DISPLAY) != 0; }
			void		enable_dbg_display();
			void		disable_dbg_display();
			void		flip_dbg_display();
	
			DOUBLE		update_start_value();

public:
	static			void	set_verbose(			bool CONST b_in );
	static			void	flip_verbose();
	static			void	set_list_load_verbose(	bool CONST b_in );
	static			void	flip_list_load_verbose();
	static			AAA_ERR	load_file_type(			C_PCHAR_C filename,		CHAR* CONST title, INT32* CONST version );
	static			AAA_ERR	list_save_to_file(		C_PCHAR_C filename,		C_PCHAR_C param_name, INT32 CONST version, c_param* CONST param, INT32 CONST nb, bool CONST b_default_save_always );
	static			AAA_ERR	list_load_from_file(	o_str CONST & filename,	c_obj_ui* obj );

	//danger infrastructure at risk
	//	should be c_obj_node
	static			void	builder_push(			c_obj_ui* one );
	static			void	builder_pop();
private:
	static	FINLINE	AAA_ERR	load_open(				o_str CONST & filename );
	static	FINLINE	void	load_close();
	static			AAA_ERR	list_load(				c_obj_ui* obj );
	static	FINLINE	AAA_ERR	list_load_from_stream(	c_obj_ui* obj );
	static	FINLINE	AAA_ERR	load_begin(				CHAR* title, INT32* version );
	static	FINLINE	void	load_end();

	static	FINLINE	FILE*	save_open(				C_PCHAR_C filename );
	static	FINLINE	AAA_ERR	save_close(				FILE* file );
	static	FINLINE	AAA_ERR	save_begin(				FILE* file, C_PCHAR_C title, INT32 version );
	static	FINLINE	AAA_ERR	save_end(				FILE* file );
public:
	static			bool	action(					aaa::param::ACTION CONST action );
};

typedef c_param* p_param;



