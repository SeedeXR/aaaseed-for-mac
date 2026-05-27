
#ifdef AAA_PARAM_DATA_H
#error "PARAM_DATA_H included more than once."
#endif
#define AAA_PARAM_DATA_H 1


#ifndef AAA_PARAM_DEF_H
#	include "param_def.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class c_param_def;
class c_factory_group;

class c_param_data final
{
private:
	o_str					_name;
	UINT32					_type_and_flag;
	REAL					_def;	//	default		//todoq flip it
	REAL					_ina;	//	inactive
	REAL					_min;
	REAL					_max;
	aaa::param::fn_update*	_fn_update;
	//	_symbolic_str is two-headed, depending on get_type() :
	//	  TYPE_SYMBOLIC*       : C_PCHAR_C* array of symbol strings (paired if M_SYNO is set).
	//	                         Read via get_symbolic_str() / get_symbolic_str_direct(i).
	//	  TYPE_CLASS_BRANCH    : reinterpreted as c_factory_group*.
	//	                         Read via get_factory_group(). No sister set_factory_group()
	//	                         exists ; assignment goes through set_symbolic_str(C_PCHAR_C*).
	C_PCHAR_C*				_symbolic_str;

public:
	static	UINT32*	get_nb_pt();			
	static	UINT32*	get_nb_created_pt();		
	static	UINT32*	get_nb_created_copy_pt();

	c_param_data();
	c_param_data( c_param_def CONST * CONST def );
	c_param_data( c_param_data CONST * CONST pd );

#if 1
	C_NO_CPY_MOVE(c_param_data)
#else
	//	copy constructor
	c_param_data( CONST c_param_data& obj );
	//	copy assignment operator
	c_param_data& operator=( CONST c_param_data& obj );
#endif

	~c_param_data();

			void						set( c_param_def CONST * CONST in );

	FINLINE	void						set_type( aaa::param::TYPE CONST type )	{ _type_and_flag = aaa::param::make_type( type, _type_and_flag ); }
	FINLINE	aaa::param::TYPE			get_type()						CONST	{ return aaa::param::get_type(					_type_and_flag ); }
	FINLINE aaa::param::TYPE_INTERNAL	get_type_internal()				CONST	{ return aaa::param::get_type_internal(			_type_and_flag ); }
	FINLINE	C_PCHAR_C					get_type_str()					CONST	{ return aaa::param::get_type_str(				_type_and_flag ); }
	FINLINE	C_PCHAR_C					get_type_str_lowercase()		CONST	{ return aaa::param::get_type_str_lowercase(	_type_and_flag ); }

	FINLINE	bool			is_group()						CONST				{ return aaa::param::is_type_group(			get_type()		);	}
	FINLINE	bool			is_group_closed()				CONST				{ return aaa::param::is_type_group_closed(	get_type()		);	}
	FINLINE	bool			is_type_text()					CONST				{ return aaa::param::is_type_text(			get_type()		);	}
	FINLINE	bool			is_type_number()				CONST				{ return aaa::param::is_type_number(		get_type()		);	}
	FINLINE	bool			is_type_real()					CONST				{ return aaa::param::is_type_real(			get_type()		);	}
	FINLINE	bool			is_type_bool()					CONST				{ return aaa::param::is_type_bool(			get_type()		);	}
	FINLINE	bool			is_save()						CONST				{ return aaa::param::is_save(				_type_and_flag	);	}
	FINLINE	bool			is_lock()						CONST				{ return aaa::param::is_lock(				_type_and_flag	);	}
	FINLINE	bool			is_syno()						CONST				{ return aaa::param::is_syno(				_type_and_flag	);	}
	FINLINE void			set_save( bool CONST b_save )						{ _type_and_flag = aaa::param::set_save( _type_and_flag, b_save );	}

#if AAA_STATE_COMPILE()						
	FINLINE	bool			is_state_sensitive()			CONST				{ return (_type_and_flag & aaa::param::M_STATE_INSENSITIVE) == 0;	}
#endif	//AAA_STATE_COMPILE														  
	FINLINE	void			or_type_maa(	UINT32 CONST mask )					{ _type_and_flag |= mask;	}
	FINLINE	void			and_type_maa(	UINT32 CONST mask )					{ _type_and_flag &= mask;	}
	FINLINE	UINT32			get_type_maa()					CONST				{ return _type_and_flag;	}
																				  
	FINLINE	void			set_def(		REAL CONST def )					{ _def = def;	}
	FINLINE	REAL			get_def() 						CONST				{ return _def;	}
	FINLINE	void			set_ina(		REAL CONST ina )					{ _ina = ina;	}
	FINLINE	REAL			get_ina()						CONST				{ return _ina;	}
																				  
	FINLINE	void			set_min(		REAL CONST min )					{ _min = min;	}
	FINLINE	REAL			get_min()						CONST				{ return _min;	}
	FINLINE	void			set_max(		REAL CONST max )					{ _max = max;	}
	FINLINE	REAL			get_max()						CONST				{ return _max;	}
																				  
	FINLINE	void			get_min_max(	REAL& min, REAL& max )		CONST	{ min = _min;	max = _max;		}
																				  
			void			set_limits(		REAL def, REAL ina, REAL min, REAL max ); 
																				  
	FINLINE	void			set_name(		o_str CONST & name )				{ _name.set( name );			}
	FINLINE	void			set_name(		C_PCHAR_C name )					{ _name.set( name );			}
	FINLINE	o_str CONST &	get_name()									CONST	{ return _name;					}
	FINLINE	bool			is_name_equal(	o_str CONST & name )		CONST	{ return _name.is_equal(name);	}
																				  
	FINLINE	C_PCHAR_C*		get_symbolic_str()							CONST	{ return _symbolic_str;	}
#if AAA_DEBUG()
private:
			void			build_symbolic_index_valid(	INT32& index )	CONST;
public:
#endif //AAA_DEBUG
	FINLINE	C_PCHAR_C		get_symbolic_str_direct(	INT32 CONST index	)	CONST	{	return _symbolic_str ? _symbolic_str[index] : nullptr; }
	FINLINE	C_PCHAR_C		get_symbolic_str(			INT32 index	)			CONST	{
#if AAA_DEBUG()
																							build_symbolic_index_valid( index );
#endif //AAA_DEBUG
																							return get_symbolic_str_direct( index );
																						}

	FINLINE	c_factory_group*	get_factory_group()	CONST								{	return (c_factory_group*)_symbolic_str; }
			INT32				get_value_from_symbo_str( C_PCHAR str )		CONST;

	FINLINE aaa::param::fn_update*		get_fn_update()			CONST					{ return _fn_update;	}
};


