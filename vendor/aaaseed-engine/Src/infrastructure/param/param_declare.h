
#ifdef AAA_PARAM_DECLARE_H
#error "PARAM_DECLARE_H included more than once."
#endif
#define AAA_PARAM_DECLARE_H 1


#ifndef AAA_PARAM_DEF_H
#	include "param_def.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class c_param_data;

//todo merge with param_data
class c_param_def
{
	friend  c_param_data;
private:
	UINT32					_type_and_flag;
//	C_PCHAR_C				_name_const;	
	o_str					_name;
	REAL					_def;			//	default		//todoq flip it ?
	REAL					_ina;			//	inactive
	REAL					_min;
	REAL					_max;
	aaa::param::fn_update*	_fn_update;		// this is called when value change but don't work on every code path //todo finish, or eventually replace
	C_PCHAR_C*				_symbolic_str;

public:
	static UINT32*			get_nb_pt();
	static UINT32*			get_nb_created_pt();

	FINLINE void			set_name( C_PCHAR_C name )				{ _name.set( name );	}
	CONSTEXPR C_PCHAR_C		get_name()				CONST			{ return _name.get();	}
//	CONSTEXPR C_PCHAR_C		get_name_data()			CONST			{ return _name.//get_data();	}
		
	FINLINE	void			set_type( aaa::param::TYPE CONST t )	{ _type_and_flag = make_type( t, _type_and_flag);	}
	CONSTEXPR UINT32		get_type_and_flag()		CONST			{ return _type_and_flag;			}
																	  
	FINLINE	void			set_def( REAL CONST r )					{ _def = r;		}
	CONSTEXPR REAL			get_def()				CONST			{ return _def;	}
																	  
	FINLINE	void			set_ina( REAL CONST r )					{ _ina = r;		}
	CONSTEXPR REAL			get_ina()				CONST			{ return _ina;	}
																	  
	FINLINE	void			set_min( REAL CONST r )					{ _min = r;		}
	CONSTEXPR REAL			get_min()				CONST			{ return _min;	}
																	  
	FINLINE	void			set_max( REAL CONST r )					{ _max = r;		}
	CONSTEXPR REAL			get_max()				CONST			{ return _max;	}
																	  
	FINLINE	void			set_symbolic_str( C_PCHAR_C* pt )		{ _symbolic_str = pt;	}
	CONSTEXPR C_PCHAR_C*	get_symbolic_str()		CONST			{ return _symbolic_str;	}
																  
	CONSTEXPR aaa::param::fn_update*	get_fn_update()	CONST		{ return _fn_update;	}


	void set_all(	UINT32 CONST type, C_PCHAR_C name,	
						REAL CONST def = 0., REAL CONST ina = 0., REAL CONST min = 0., REAL CONST max = 0.,
						aaa::param::fn_update* CONST fn = nullptr, C_PCHAR_C * CONST  symbolic_str = nullptr );
	c_param_def(	UINT32 CONST type = aaa::param::TYPE_NONE|aaa::param::M_SAVE_NOT, C_PCHAR_C name = nullptr,	
						REAL CONST def = 0., REAL CONST ina = 0., REAL CONST min = 0., REAL CONST max = 0.,
						aaa::param::fn_update* CONST fn = nullptr, C_PCHAR_C * CONST  symbolic_str = nullptr );
	~c_param_def();
//	C_NO_CPY_MOVE(c_param_def)

#define PARAM_DEF_GROUP_EXT ...
#define PARAM_DEF_GROUP_EXT_LEN 3
#define PARAM_DEF_GROUP_NAME(name) AAA_STRING(name...)
#define PARAM_DEF_NONE_EXT ->
#define PARAM_DEF_NONE_EXT_LEN 2
#define PARAM_DEF_NONE_NAME( name ) AAA_STRING(name->)

	//	param_name_is_equal( str, name ) : returns true if the two names match under the
	//	param-name equality rules. NOT case-sensitive on the `str` side, NOT space-sensitive.
	//
	//	  - lower-case `str` matches mixed-case `name` (e.g. "init_full" matches "Init Full").
	//	  - `_` in `str` matches `' '` (space) in `name`.
	//	  - exact match otherwise.
	//	  - terminators recognised on the `name` side : "..." marks a group end, "->" marks
	//	    a none / NONE terminator. Both produce a successful match if `str` is exhausted.
	//
	//	The asymmetry between `str` (lowercase, underscores) and `name` (display form, mixed
	//	case + spaces) is intentional : `str` is the script / command-line input form, `name`
	//	is the human-readable display form.
	static CONSTEXPR	bool	param_name_is_equal( C_PCHAR str, C_PCHAR name )
	{
		//if( !name )
		//	return !str || *str==0;
		//if( str )
		//{
			for( ; ; )
			{
				UINT8 s = *str;		// this the str we ask about (lower case and no space: for "Init Full" param we use "init_full"
				UINT8 p = *name;	// this is the param name
				if( s==p )
				{
					if( s==0 )
						return true;
				}
				else if( (s==p+32 && 'a'<=s && s<='z') || (s=='_' && p==' ') )
				{
				}
				else if( s!=0 )
					return false;
				else
				{
					return		(p=='.' && *(name+3)==0 && *(name+1)=='.' && *(name+2)=='.')	// group termination	
							||	(p=='-' && *(name+2)==0 && *(name+1)=='>');	// none termination
				}
				++str;
				++name;
			}
		//}
		//return *pat == 0;
	}

	static CONSTEXPR INT32 get_param_def_index( C_PCHAR_C str, c_param_def CONST * CONST param_def, INT32 CONST nb )
	{
		for( INT32 index = 0; index < nb; ++index )
		{
			C_PCHAR_C pt_name = param_def[index]._name.get();
			// +32 go from upper case to lower case case 
			if( ( *pt_name==*str || (*pt_name+32)==*str ) && param_name_is_equal( str+1, pt_name+1 ) )
				return index;
		}
		return -1;
	}
};

#define PARAM_DEF_MAKE_INDEX( name ) CONST INT32 PARAM_INDEX_ ## name = c_param_def::get_param_def_index( #name, param, ARRAY_SIZE(param) )

//experiment
//class c_param_def_changeable : public c_param_def
//{
	//FINLINE	void	set_name( C_PCHAR_C name )			{	_name.set( name );	}
	//FINLINE	void	set_def( REAL CONST r )				{	_def = r;				}
	//FINLINE	void	set_ina( REAL CONST r )				{	_ina = r;				}
	//FINLINE	void	set_min( REAL CONST r )				{	_min = r;				}
	//FINLINE	void	set_max( REAL CONST r )				{	_max = r;				}
	//FINLINE	void	set_symbolic_str( C_PCHAR_C* pt )	{	_symbolic_str = pt;		}
//};

using namespace aaa::param;


//todo we need to refine this and c_param because for example REAL cast will clip max 
#define	PARAM_DEF_BASE(					type, name, def,ina, min,max )				c_param_def(	UINT32(type),	(#name),	REAL(def),REAL(ina),	REAL(min),REAL(max)	),
#define	PARAM_DEF_BASE_QUOTE(			type, name, def,ina, min,max )				c_param_def(	UINT32(type),	(name),		REAL(def),REAL(ina),	REAL(min),REAL(max)	),
#define	PARAM_DEF_BASE_FN(				type, name, def,ina, min,max, update )		c_param_def(	UINT32(type),	(#name),	REAL(def),REAL(ina),	REAL(min),REAL(max),	(update)	),
#define	PARAM_DEF_BASE_STR(				type, name, def,ina, min,max, str )			c_param_def(	UINT32(type),	(#name),	REAL(def),REAL(ina),	REAL(min),REAL(max),	nullptr,	(str)	),
//#define	PARAM_DEF_BASE_ALL(			type, name, def,ina, min,max, update, str )	c_param_def(	UINT32(type),	(#name),	REAL(def),REAL(ina),	REAL(min),REAL(max),	(update),	(str)	),

#define	PARAM_DEF_ALL_ZERO(				type, name )					PARAM_DEF_BASE(		(type),					name,	0,0,			0,0 )
#define	PARAM_DEF_ALL_ZERO_FN(			type, name, fn )				PARAM_DEF_BASE_FN(	(type),					name,	0,0,			0,0,	fn )


//	GENERIC
#define	PARAM_DEF_GENE(					type, name, def,ina, min,max )	PARAM_DEF_BASE(		type,					name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_GENE_SAVE_NOT(		type, name, def,ina, min,max )	PARAM_DEF_BASE(		type|M_SAVE_NOT,		name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_GENE_LOCKED(			type, name, def,ina, min,max )	PARAM_DEF_BASE(		type|M_LOCK,			name,	(def),(ina),	(min),(max) )
				
//#define	PARAM_DEF_GENE_INF(				type, name, def,ina )	PARAM_DEF_GENE(		type,					name,	(def),(ina),	-PARAM_INFINI, PARAM_INFINI )
//#define	PARAM_DEF_GENE_INF_SAVE_NOT(	type, name, def,ina )	PARAM_DEF_BASE(		type|M_SAVE_NOT,		name,	(def),(ina),	-PARAM_INFINI, PARAM_INFINI )
//#define	PARAM_DEF_GENE_POS(				type, name, def,ina )	PARAM_DEF_GENE(		type,					name,	(def),(ina),	0, PARAM_INFINI )
				
#define	PARAM_DEF_GENE_ZERO_ONE(		type, name )				PARAM_DEF_GENE(		type,					name,	0,1,			0,1 )
#define	PARAM_DEF_GENE_ONE_ZERO(		type, name )				PARAM_DEF_GENE(		type,					name,	1,0,			0,1 )
				
//#define	PARAM_DEF_GENE_ZERO(			type, name )			PARAM_DEF_GENE_INF(	type,					name,	1,0			)
//#define	PARAM_DEF_GENE_ONE(				type, name )			PARAM_DEF_GENE_INF(	type,					name,	0,1			)
//#define	PARAM_DEF_GENE_ZERO_SAVE_NOT(	type, name )			PARAM_DEF_BASE(		type|M_SAVE_NOT,		name,	1,0,			-PARAM_INFINI, PARAM_INFINI )
				
//#define	PARAM_DEF_GENE_POS_ZERO(		type, name )			PARAM_DEF_GENE_POS(	type,					name,	1,0			)
//#define	PARAM_DEF_GENE_POS_ONE(			type, name )			PARAM_DEF_GENE_POS(	type,					name,	0,1			)


//	BOOL infact bool for windows
#define	PARAM_DEF_BOOL_OFF_STR(			name, str )					PARAM_DEF_BASE_STR(			TYPE_BOOL,	name,	1,0,	0,1,	(str)	)
#define	PARAM_DEF_BOOL_ON_STR(			name, str )					PARAM_DEF_BASE_STR(			TYPE_BOOL,	name,	0,1,	0,1,	(str)	)

#define	PARAM_DEF_BOOL_OFF_SAVE_NOT(	name )						PARAM_DEF_GENE_SAVE_NOT(	TYPE_BOOL,	name,	1,0,	0,1	)
#define	PARAM_DEF_BOOL_ON_SAVE_NOT(		name )						PARAM_DEF_GENE_SAVE_NOT(	TYPE_BOOL,	name,	0,1,	0,1	)
#define	PARAM_DEF_BOOL_LOCKED(			name )						PARAM_DEF_GENE_LOCKED(		TYPE_BOOL,	name,	0,0,	0,1	)

#define	PARAM_DEF_BOOL_OFF(				name )						PARAM_DEF_GENE_ONE_ZERO(	TYPE_BOOL,	name	)
#define	PARAM_DEF_BOOL_ON(				name )						PARAM_DEF_GENE_ZERO_ONE(	TYPE_BOOL,	name	)										


//	INT32
#define	PARAM_DEF_INT32_POS_FN(			name, fn )					PARAM_DEF_BASE_FN(			TYPE_INT32,	name,	1,0,			0,PARAM_MAX_INT32,	(fn)	)

#define	PARAM_DEF_INT32(				name, def,ina, min,max )	PARAM_DEF_GENE(				TYPE_INT32,	name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_INT32_SAVE_NOT(		name, def,ina, min,max )	PARAM_DEF_GENE_SAVE_NOT(	TYPE_INT32,	name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_INT32_LOCKED(			name )						PARAM_DEF_GENE_LOCKED(		TYPE_INT32,	name,	0,0,			PARAM_MIN_INT32,PARAM_MAX_INT32	)

#define	PARAM_DEF_INT32_INF(			name, def,ina )				PARAM_DEF_INT32(			name,	(def),(ina),	PARAM_MIN_INT32,PARAM_MAX_INT32	)
#define	PARAM_DEF_INT32_INF_SAVE_NOT(	name, def,ina )				PARAM_DEF_INT32_SAVE_NOT(	name,	(def),(ina),	PARAM_MIN_INT32,PARAM_MAX_INT32	)
#define	PARAM_DEF_INT32_POS(			name, def,ina )				PARAM_DEF_INT32(			name,	(def),(ina),	0,PARAM_MAX_INT32	)

#define	PARAM_DEF_INT32_ZERO_ONE(		name )						PARAM_DEF_GENE_ZERO_ONE(	TYPE_INT32,	name	)
#define	PARAM_DEF_INT32_ONE_ZERO(		name )						PARAM_DEF_GENE_ONE_ZERO(	TYPE_INT32,	name	)

#define	PARAM_DEF_INT32_ZERO(			name )						PARAM_DEF_INT32(			name,	1,0,			PARAM_MIN_INT32,PARAM_MAX_INT32	)
#define	PARAM_DEF_INT32_ONE(			name )						PARAM_DEF_INT32(			name,	0,1,			PARAM_MIN_INT32,PARAM_MAX_INT32	)
#define	PARAM_DEF_INT32_ZERO_SAVE_NOT(	name )						PARAM_DEF_INT32_SAVE_NOT(	name,	1,0,			PARAM_MIN_INT32,PARAM_MAX_INT32	)

//theses should move to UINT32
#define	PARAM_DEF_INT32_POS_ZERO(		name )						PARAM_DEF_INT32(			name,	1,0,			0,PARAM_MAX_INT32	)
#define	PARAM_DEF_INT32_POS_ONE(		name )						PARAM_DEF_INT32(			name,	0,1,			0,PARAM_MAX_INT32	)

#define	PARAM_DEF_INT32_XY(				name, def,ina, min,max )	PARAM_DEF_INT32(			name##_x,	(def),(ina),	(min),(max) )	\
																	PARAM_DEF_INT32(			name##_y,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_INT32_XYZ(			name, def,ina, min,max )	PARAM_DEF_INT32_XY(			name,		(def),(ina),	(min),(max) )	\
																	PARAM_DEF_INT32(			name##_z,	(def),(ina),	(min),(max) )

#define	PARAM_DEF_INT32_LOCKED_XY(		name )						PARAM_DEF_INT32_LOCKED(		name##_x )	PARAM_DEF_INT32_LOCKED(	name##_y )
#define	PARAM_DEF_INT32_LOCKED_XYZ(		name )						PARAM_DEF_INT32_LOCKED_XY(	name	 )	PARAM_DEF_INT32_LOCKED(	name##_z )


//	UINT32
#define	PARAM_DEF_UINT32(				name, def,ina, min,max )	PARAM_DEF_GENE(				TYPE_UINT32,	name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_UINT32_INF(			name, def,ina			)	PARAM_DEF_GENE(				TYPE_UINT32,	name,	(def),(ina),	0, PARAM_MAX_UINT32	)
#define	PARAM_DEF_UINT32_SAVE_NOT(		name, def,ina, min,max )	PARAM_DEF_GENE_SAVE_NOT(	TYPE_UINT32,	name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_UINT32_LOCKED(		name )						PARAM_DEF_GENE_LOCKED(		TYPE_UINT32,	name,	0,0,			0, PARAM_MAX_UINT32	)

#define	PARAM_DEF_UINT32_ZERO(			name )						PARAM_DEF_UINT32(			name,	1,0,			0,PARAM_MAX_UINT32 )
#define	PARAM_DEF_UINT32_ONE(			name )						PARAM_DEF_UINT32(			name,	0,1,			0,PARAM_MAX_UINT32 )

#define	PARAM_DEF_UINT32_XY(			name, def,ina, min,max )	PARAM_DEF_UINT32(			name##_x,	(def),(ina),	(min),(max) )	\
																	PARAM_DEF_UINT32(			name##_y,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_UINT32_XYZ(			name, def,ina, min,max )	PARAM_DEF_UINT32_XY(		name,		(def),(ina),	(min),(max) )	\
																	PARAM_DEF_UINT32(			name##_z,	(def),(ina),	(min),(max) )

#define	PARAM_DEF_UINT32_LOCKED_XY(		name )						PARAM_DEF_UINT32_LOCKED(	name##_x )	PARAM_DEF_UINT32_LOCKED(	name##_y )
#define	PARAM_DEF_UINT32_LOCKED_XYZ(	name )						PARAM_DEF_UINT32_LOCKED_XY(	name	 )	PARAM_DEF_UINT32_LOCKED(	name##_z )

//	FLOAT
#define	PARAM_DEF_FP32(					name, def,ina, min,max )	PARAM_DEF_GENE(				TYPE_FP32,		name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_FP32_SAVE_NOT(		name, def,ina, min,max )	PARAM_DEF_GENE_SAVE_NOT(	TYPE_FP32,		name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_FP32_LOCKED(			name )						PARAM_DEF_GENE_LOCKED(		TYPE_FP32,		name,	0,0,			PARAM_MIN_FP32,PARAM_MAX_FP32	)

#define	PARAM_DEF_FP32_INF(				name, def,ina )				PARAM_DEF_FP32(				name,	(def),(ina),	PARAM_MIN_FP32,PARAM_MAX_FP32	)
#define	PARAM_DEF_FP32_INF_SAVE_NOT(	name, def,ina )				PARAM_DEF_FP32_SAVE_NOT(	name,	(def),(ina),	PARAM_MIN_FP32,PARAM_MAX_FP32	)
#define	PARAM_DEF_FP32_POS(				name, def,ina )				PARAM_DEF_FP32(				name,	(def),(ina),	0,PARAM_MAX_FP32	)

#define	PARAM_DEF_FP32_ZERO_ONE(		name )						PARAM_DEF_GENE_ZERO_ONE(	TYPE_FP32,		name	)
#define	PARAM_DEF_FP32_ONE_ZERO(		name )						PARAM_DEF_GENE_ONE_ZERO(	TYPE_FP32,		name	)

#define	PARAM_DEF_FP32_ZERO(			name )						PARAM_DEF_FP32(				name,	1,0,			PARAM_MIN_FP32,PARAM_MAX_FP32	)
#define	PARAM_DEF_FP32_ONE(				name )						PARAM_DEF_FP32(				name,	0,1,			PARAM_MIN_FP32,PARAM_MAX_FP32	)
#define	PARAM_DEF_FP32_ZERO_SAVE_NOT(	name )						PARAM_DEF_FP32_SAVE_NOT(	name,	1,0,			PARAM_MIN_FP32,PARAM_MAX_FP32	)

#define	PARAM_DEF_FP32_POS_ZERO(		name )						PARAM_DEF_FP32(				name,	1,0,			0,PARAM_MAX_FP32	)
#define	PARAM_DEF_FP32_POS_ONE(			name )						PARAM_DEF_FP32(				name,	0,1,			0,PARAM_MAX_FP32	)

#define	PARAM_DEF_FP32_LOCKED_XY(		name )						PARAM_DEF_FP32_LOCKED(		name##_x )	PARAM_DEF_FP32_LOCKED(	name##_y )
#define	PARAM_DEF_FP32_LOCKED_XYZ(		name )						PARAM_DEF_FP32_LOCKED_XY(	name	 )	PARAM_DEF_FP32_LOCKED(	name##_z )

//	REAL
#define	PARAM_DEF_REAL(					name, def,ina, min,max )	PARAM_DEF_GENE(				TYPE_REAL,		name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_REAL_SAVE_NOT(		name, def,ina, min,max )	PARAM_DEF_GENE_SAVE_NOT(	TYPE_REAL,		name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_REAL_LOCKED(			name )						PARAM_DEF_GENE_LOCKED(		TYPE_REAL,		name,	0,0,			PARAM_MIN_REAL,PARAM_MAX_REAL	)

#define	PARAM_DEF_REAL_INF(				name, def,ina )				PARAM_DEF_REAL(				name,	(def),(ina),	PARAM_MIN_REAL,PARAM_MAX_REAL	)
#define	PARAM_DEF_REAL_INF_SAVE_NOT(	name, def,ina )				PARAM_DEF_REAL_SAVE_NOT(	name,	(def),(ina),	PARAM_MIN_REAL,PARAM_MAX_REAL	)
#define	PARAM_DEF_REAL_POS(				name, def,ina )				PARAM_DEF_REAL(				name,	(def),(ina),	0,PARAM_MAX_REAL	)

#define	PARAM_DEF_REAL_ZERO_ONE(		name )						PARAM_DEF_GENE_ZERO_ONE(	TYPE_REAL,		name	)
#define	PARAM_DEF_REAL_ONE_ZERO(		name )						PARAM_DEF_GENE_ONE_ZERO(	TYPE_REAL,		name	)

#define	PARAM_DEF_REAL_ZERO(			name )						PARAM_DEF_REAL(				name,	1,0,			PARAM_MIN_REAL,PARAM_MAX_REAL	)
#define	PARAM_DEF_REAL_ONE(				name )						PARAM_DEF_REAL(				name,	0,1,			PARAM_MIN_REAL,PARAM_MAX_REAL	)
#define	PARAM_DEF_REAL_ZERO_SAVE_NOT(	name )						PARAM_DEF_REAL_SAVE_NOT(	name,	1,0,			PARAM_MIN_REAL,PARAM_MAX_REAL	)

#define	PARAM_DEF_REAL_POS_ZERO(		name )						PARAM_DEF_REAL(				name,	1,0,			0,PARAM_MAX_REAL	)
#define	PARAM_DEF_REAL_POS_ONE(			name )						PARAM_DEF_REAL(				name,	0,1,			0,PARAM_MAX_REAL	)

#define	PARAM_DEF_REAL_LOCKED_XY(		name )						PARAM_DEF_REAL_LOCKED(		name##_x )	PARAM_DEF_REAL_LOCKED(	name##_y )
#define	PARAM_DEF_REAL_LOCKED_XYZ(		name )						PARAM_DEF_REAL_LOCKED_XY(	name	 )	PARAM_DEF_REAL_LOCKED(	name##_z )

//DOUBLE
#define	PARAM_DEF_DOUBLE(				name, def,ina, min,max )	PARAM_DEF_GENE(				TYPE_DOUBLE,	name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_DOUBLE_SAVE_NOT(		name, def,ina, min,max )	PARAM_DEF_GENE_SAVE_NOT(	TYPE_DOUBLE,	name,	(def),(ina),	(min),(max) )
#define	PARAM_DEF_DOUBLE_LOCKED(		name )						PARAM_DEF_GENE_LOCKED(		TYPE_DOUBLE,	name,	0,0,			PARAM_MIN_DOUBLE,PARAM_MAX_DOUBLE	)

#define	PARAM_DEF_DOUBLE_INF(			name, def,ina )				PARAM_DEF_DOUBLE(			name,	(def),(ina),	PARAM_MIN_DOUBLE,PARAM_MAX_DOUBLE	)
#define	PARAM_DEF_DOUBLE_INF_SAVE_NOT(	name, def,ina )				PARAM_DEF_DOUBLE_SAVE_NOT(	name,	(def),(ina),	PARAM_MIN_DOUBLE,PARAM_MAX_DOUBLE	)
#define	PARAM_DEF_DOUBLE_POS(			name, def,ina )				PARAM_DEF_DOUBLE(			name,	(def),(ina),	0,PARAM_MAX_DOUBLE	)

#define	PARAM_DEF_DOUBLE_ZERO_ONE(		name )						PARAM_DEF_GENE_ZERO_ONE(	TYPE_DOUBLE,	name	)
#define	PARAM_DEF_DOUBLE_ONE_ZERO(		name )						PARAM_DEF_GENE_ONE_ZERO(	TYPE_DOUBLE,	name	)

#define	PARAM_DEF_DOUBLE_ZERO(			name )						PARAM_DEF_DOUBLE(			name,	1,0,			PARAM_MIN_DOUBLE,PARAM_MAX_DOUBLE	)
#define	PARAM_DEF_DOUBLE_ONE(			name )						PARAM_DEF_DOUBLE(			name,	0,1,			PARAM_MIN_DOUBLE,PARAM_MAX_DOUBLE	)
#define	PARAM_DEF_DOUBLE_ZERO_SAVE_NOT(	name )						PARAM_DEF_DOUBLE_SAVE_NOT(	name,	1,0,			PARAM_MIN_DOUBLE,PARAM_MAX_DOUBLE	)

#define	PARAM_DEF_DOUBLE_POS_ZERO(		name )						PARAM_DEF_DOUBLE(			name,	1,0,			0,PARAM_MAX_DOUBLE	)
#define	PARAM_DEF_DOUBLE_POS_ONE(		name )						PARAM_DEF_DOUBLE(			name,	0,1,			0,PARAM_MAX_DOUBLE	)

#define	PARAM_DEF_DOUBLE_LOCKED_XY(		name )						PARAM_DEF_DOUBLE_LOCKED(	name##_x )	PARAM_DEF_DOUBLE_LOCKED(	name##_y )
#define	PARAM_DEF_DOUBLE_LOCKED_XYZ(	name )						PARAM_DEF_DOUBLE_LOCKED_XY(	name	 )	PARAM_DEF_DOUBLE_LOCKED(	name##_z )

//GAIN BIAS
#define	PARAM_DEF_GAIN(					name )						PARAM_DEF_REAL(				name,	.7, .5,			GAIN_MIN,GAIN_MAX )
#define	PARAM_DEF_BIAS(					name )						PARAM_DEF_REAL(				name,	.7, .5,			BIAS_MIN,BIAS_MAX )

//STR
#define	PARAM_DEF_STR(					name )						PARAM_DEF_ALL_ZERO(		TYPE_STR,				name	)
#define	PARAM_DEF_STR_DEF(				name, def )					PARAM_DEF_BASE_STR(		TYPE_STR,				name,	0,0,			0,0,	(def)	)
#define	PARAM_DEF_STR_LOCKED(			name )						PARAM_DEF_ALL_ZERO(		TYPE_STR|M_LOCK,		name	)
#define	PARAM_DEF_STR_LOCKED_FN(		name, fn )					PARAM_DEF_ALL_ZERO_FN(	TYPE_STR|M_LOCK,		name,	fn )
#define	PARAM_DEF_STR_SAVE_NOT(			name )						PARAM_DEF_ALL_ZERO(		TYPE_STR|M_SAVE_NOT,	name	)

//REF
#define	PARAM_DEF_REF(					name )						PARAM_DEF_ALL_ZERO(		TYPE_REF,				name	)
#define	PARAM_DEF_REF_UPDATE_FN(		name, fn )					PARAM_DEF_ALL_ZERO_FN(	TYPE_REF,				name,	fn )

//POINT
#define	PARAM_DEF_XY_LOCKED(			name )		PARAM_DEF_REAL_LOCKED(		name##_x	)	PARAM_DEF_REAL_LOCKED(	name##_y		)
#define	PARAM_DEF_XYZ_LOCKED(			name )		PARAM_DEF_XY_LOCKED(		name		)	PARAM_DEF_REAL_LOCKED(	name##_z		)
#define	PARAM_DEF_XYZW_LOCKED(			name )		PARAM_DEF_XYZ_LOCKED(		name		)	PARAM_DEF_REAL_LOCKED(	name##_w		)

#define	PARAM_DEF_FP32_LOCKED_XY(		name )		PARAM_DEF_FP32_LOCKED(		name##_x	)	PARAM_DEF_FP32_LOCKED(	name##_y		)
#define	PARAM_DEF_FP32_LOCKED_XYZ(		name )		PARAM_DEF_FP32_LOCKED_XY(	name		)	PARAM_DEF_FP32_LOCKED(	name##_z		)
#define	PARAM_DEF_FP32_LOCKED_XYZW(		name )		PARAM_DEF_FP32_LOCKED_XYZ(	name		)	PARAM_DEF_FP32_LOCKED(	name##_w		)

#define	PARAM_DEF_POINT_XY(				name )		PARAM_DEF_REAL_ZERO(		name##_x	)	PARAM_DEF_REAL_ZERO(	name##_y		)
#define	PARAM_DEF_POINT_XYZ(			name )		PARAM_DEF_POINT_XY(			name		)	PARAM_DEF_REAL_ZERO(	name##_z		)
#define	PARAM_DEF_POINT_XYZW(			name )		PARAM_DEF_POINT_XYZ(		name		)	PARAM_DEF_REAL_ZERO(	name##_w		)

#define	PARAM_DEF_POINT_FP32_XY(		name )		PARAM_DEF_FP32_ZERO(		name##_x	)	PARAM_DEF_FP32_ZERO(	name##_y		)
#define	PARAM_DEF_POINT_FP32_XYZ(		name )		PARAM_DEF_POINT_FP32_XY(	name		)	PARAM_DEF_FP32_ZERO(	name##_z		)
#define	PARAM_DEF_POINT_FP32_XYZW(		name )		PARAM_DEF_POINT_FP32_XYZ(	name		)	PARAM_DEF_FP32_ZERO(	name##_w		)

#define	PARAM_DEF_SCALE_XY(				name )		PARAM_DEF_REAL_ONE(			name##_x	)	PARAM_DEF_REAL_ONE(		name##_y		)
#define	PARAM_DEF_SCALE_XYF(			name )		PARAM_DEF_SCALE_XY(			name		)	PARAM_DEF_REAL_ONE(		name##_factor	)
#define	PARAM_DEF_SCALE_XYZ(			name )		PARAM_DEF_SCALE_XY(			name		)	PARAM_DEF_REAL_ONE(		name##_z		)
#define	PARAM_DEF_SCALE_XYZF(			name )		PARAM_DEF_SCALE_XYZ(		name		)	PARAM_DEF_REAL_ONE(		name##_factor	)
#define	PARAM_DEF_SCALE_XYZF_ZERO(		name )		PARAM_DEF_SCALE_XYF(		name		)	PARAM_DEF_REAL_ZERO(	name##_factor	)

#define	PARAM_DEF_SCALE_FP32_XY(		name )		PARAM_DEF_FP32_ONE(			name##_x	)	PARAM_DEF_FP32_ONE(		name##_y		)
#define	PARAM_DEF_SCALE_FP32_XYZ(		name )		PARAM_DEF_SCALE_FP32_XY(	name		)	PARAM_DEF_FP32_ONE(		name##_z		)
#define	PARAM_DEF_SCALE_FP32_XYZF(		name )		PARAM_DEF_SCALE_FP32_XYZ(	name		)	PARAM_DEF_FP32_ONE(		name##_factor	)

#define	PARAM_DEF_POINT_UV(				name )		PARAM_DEF_REAL_ZERO(		name##_u	)	PARAM_DEF_REAL_ZERO(	name##_v		)
#define	PARAM_DEF_POINT_UVA(			name )		PARAM_DEF_POINT_UV(			name		)	PARAM_DEF_REAL_ZERO(	name##_axe		)
#define	PARAM_DEF_POINT_UVAF(			name )		PARAM_DEF_POINT_UVA(		name		)	PARAM_DEF_REAL_ZERO(	name##_factor	)

#define	PARAM_DEF_POINT_FP32_UV(		name )		PARAM_DEF_FP32_ZERO(		name##_u	)	PARAM_DEF_FP32_ZERO(	name##_v		)
#define	PARAM_DEF_POINT_FP32_UVA(		name )		PARAM_DEF_POINT_FP32_UV(	name		)	PARAM_DEF_FP32_ZERO(	name##_axe		)
#define	PARAM_DEF_POINT_FP32_UVAF(		name )		PARAM_DEF_POINT_FP32_UVA(	name		)	PARAM_DEF_FP32_ZERO(	name##_factor	)

#define	PARAM_DEF_POINT_UV_SAVE_NOT(	name )		PARAM_DEF_REAL_ZERO_SAVE_NOT(	name##_u	)	PARAM_DEF_REAL_ZERO_SAVE_NOT( name##_v		)
#define	PARAM_DEF_POINT_UVA_SAVE_NOT(	name )		PARAM_DEF_POINT_UV_SAVE_NOT(	name		)	PARAM_DEF_REAL_ZERO_SAVE_NOT( name##_axe	)
#define	PARAM_DEF_UVW_SAVE_NOT(			name )		PARAM_DEF_POINT_UV_SAVE_NOT(	name		)	PARAM_DEF_REAL_ZERO_SAVE_NOT( name##_w		)

#define	PARAM_DEF_UV_LOCKED(			name )		PARAM_DEF_REAL_LOCKED(		name##_u	)	PARAM_DEF_REAL_LOCKED(	name##_v		)
#define	PARAM_DEF_UVA_LOCKED(			name )		PARAM_DEF_UV_LOCKED(		name		)	PARAM_DEF_REAL_LOCKED(	name##_axe		)
#define	PARAM_DEF_UVW_LOCKED(			name )		PARAM_DEF_UV_LOCKED(		name		)	PARAM_DEF_REAL_LOCKED(	name##_w		)

#define	PARAM_DEF_SCALE_UV(				name )		PARAM_DEF_REAL_ONE(			name##_u	)	PARAM_DEF_REAL_ONE(		name##_v		)
#define	PARAM_DEF_SCALE_UVF(			name )		PARAM_DEF_SCALE_UV(			name		)	PARAM_DEF_REAL_ONE(		name##_factor	)
#define	PARAM_DEF_SCALE_UVA(			name )		PARAM_DEF_SCALE_UV(			name		)	PARAM_DEF_REAL_ONE(		name##_axe		)
#define	PARAM_DEF_SCALE_UVAF(			name )		PARAM_DEF_SCALE_UVA(		name		)	PARAM_DEF_REAL_ONE(		name##_factor	)

#define	PARAM_DEF_SCALE_FP32_UV(		name )		PARAM_DEF_FP32_ONE(			name##_u	)	PARAM_DEF_FP32_ONE(		name##_v		)
#define	PARAM_DEF_SCALE_FP32_UVF(		name )		PARAM_DEF_SCALE_FP32_UV(	name		)	PARAM_DEF_FP32_ONE(		name##_factor	)
#define	PARAM_DEF_SCALE_FP32_UVA(		name )		PARAM_DEF_SCALE_FP32_UV(	name		)	PARAM_DEF_FP32_ONE(		name##_axe		)
#define	PARAM_DEF_SCALE_FP32_UVAF(		name )		PARAM_DEF_SCALE_FP32_UVA(	name		)	PARAM_DEF_FP32_ONE(		name##_factor	)

#define	PARAM_DEF_ROT_YPR(				name )		PARAM_DEF_REAL_ZERO(		name##_yaw	)	PARAM_DEF_REAL_ZERO(	name##_pitch	)	PARAM_DEF_REAL_ZERO(	name##_roll		)
#define	PARAM_DEF_ROT_YPR_LOCKED(		name )		PARAM_DEF_REAL_LOCKED(		name##_yaw	)	PARAM_DEF_REAL_LOCKED(	name##_pitch	)	PARAM_DEF_REAL_LOCKED(	name##_roll		)

//RECT
#define	PARAM_DEF_RECT_LRTB(		name, DEF )		DEF( name##_left )	DEF( name##_right)	DEF( name##_top )		DEF( name##_bottom )
#define	PARAM_DEF_RECT_LRBT(		name, DEF )		DEF( name##_left )	DEF( name##_right)	DEF( name##_bottom )	DEF( name##_top )

//COLOR
#define	PARAM_DEF_COLOR_RGB(			name )		PARAM_DEF_FP32_ONE(			name##_red	)	PARAM_DEF_FP32_ONE(		name##_green	)	PARAM_DEF_FP32_ONE(		name##_blue		)
#define	PARAM_DEF_COLOR_RGBA(			name )		PARAM_DEF_COLOR_RGB(		name		)	PARAM_DEF_FP32_ONE(		name##_alpha	)
#define	PARAM_DEF_COLOR_RGBA_RED(		name )		PARAM_DEF_FP32_ONE(			name##_red	)	PARAM_DEF_FP32_ZERO(	name##_green	)	PARAM_DEF_FP32_ZERO(	name##_blue		)	PARAM_DEF_FP32_ONE(		name##_alpha	)
#define	PARAM_DEF_COLOR_RGBA_GREEN(		name )		PARAM_DEF_FP32_ZERO(		name##_red	)	PARAM_DEF_FP32_ONE(		name##_green	)	PARAM_DEF_FP32_ZERO(	name##_blue		)	PARAM_DEF_FP32_ONE(		name##_alpha	)
#define	PARAM_DEF_COLOR_RGBA_BLUE(		name )		PARAM_DEF_FP32_ZERO(		name##_red	)	PARAM_DEF_FP32_ZERO(	name##_green	)	PARAM_DEF_FP32_ONE(		name##_blue		)	PARAM_DEF_FP32_ONE(		name##_alpha	)
#define	PARAM_DEF_COLOR_RGBG(			name )		PARAM_DEF_COLOR_RGB(		name		)	PARAM_DEF_FP32_ONE(		name##_grey		)
#define	PARAM_DEF_COLOR_RGBF(			name )		PARAM_DEF_COLOR_RGB(		name		)	PARAM_DEF_FP32_ONE(		name##_factor	) 
#define	PARAM_DEF_COLOR_RGBGA(			name )		PARAM_DEF_COLOR_RGBG(		name		)	PARAM_DEF_FP32_ONE(		name##_alpha	)

#define	PARAM_DEF_COLOR_RGB_BLACK(		name )		PARAM_DEF_FP32_ZERO(		name##_red	)	PARAM_DEF_FP32_ZERO(	name##_green	)	PARAM_DEF_FP32_ZERO(	name##_blue		)
#define	PARAM_DEF_COLOR_RGBA_BLACK(		name )		PARAM_DEF_COLOR_RGB_BLACK(	name		)	PARAM_DEF_FP32_ONE(		name##_alpha	)
#define	PARAM_DEF_COLOR_RGBG_BLACK(		name )		PARAM_DEF_COLOR_RGB_BLACK(	name		)	PARAM_DEF_FP32_ZERO(	name##_grey		) 
#define	PARAM_DEF_COLOR_RGBGA_BLACK(	name )		PARAM_DEF_COLOR_RGBG_BLACK(	name		)	PARAM_DEF_FP32_ONE(		name##_alpha	)

#define	PARAM_DEF_COLOR_RGB_LOCKED(		name )		PARAM_DEF_FP32_LOCKED(		name##_red	)	PARAM_DEF_FP32_LOCKED(	name##_green	)	PARAM_DEF_FP32_LOCKED(	name##_blue		)
#define	PARAM_DEF_COLOR_RGBA_LOCKED(	name )		PARAM_DEF_COLOR_RGB_LOCKED(	name		)	PARAM_DEF_FP32_LOCKED(	name##_alpha	)

//LIST
#define	PARAM_DEF_10_11(	name, DEF )		DEF( name##_10 )				DEF( name##_11 )
#define	PARAM_DEF_10_12(	name, DEF )		PARAM_DEF_10_11( name, DEF )	DEF( name##_12 )
#define	PARAM_DEF_10_14(	name, DEF )		PARAM_DEF_10_12( name, DEF )	DEF( name##_13 )	DEF( name##_14 )
#define	PARAM_DEF_10_15(	name, DEF )		PARAM_DEF_10_14( name, DEF )	DEF( name##_15 )
#define	PARAM_DEF_10_16(	name, DEF )		PARAM_DEF_10_15( name, DEF )	DEF( name##_16 )
#define	PARAM_DEF_10_18(	name, DEF )		PARAM_DEF_10_16( name, DEF )	DEF( name##_17 )	DEF( name##_18 )
#define	PARAM_DEF_10_19(	name, DEF )		PARAM_DEF_10_18( name, DEF )	DEF( name##_19 )
#define	PARAM_DEF_10_20(	name, DEF )		PARAM_DEF_10_19( name, DEF )	DEF( name##_20 )
#define	PARAM_DEF_10_24(	name, DEF )		PARAM_DEF_10_20( name, DEF )	DEF( name##_21 )	DEF( name##_22 )	DEF( name##_23 )	DEF( name##_24 )
#define	PARAM_DEF_10_28(	name, DEF )		PARAM_DEF_10_24( name, DEF )	DEF( name##_25 )	DEF( name##_26 )	DEF( name##_27 )	DEF( name##_28 )
#define	PARAM_DEF_10_31(	name, DEF )		PARAM_DEF_10_28( name, DEF )	DEF( name##_29 )	DEF( name##_30 )	DEF( name##_31 )
#define	PARAM_DEF_10_32(	name, DEF )		PARAM_DEF_10_31( name, DEF )	DEF( name##_32 )
#define	PARAM_DEF_10_36(	name, DEF )		PARAM_DEF_10_32( name, DEF )	DEF( name##_33 )	DEF( name##_34 )	DEF( name##_35 )	DEF( name##_36 )
#define	PARAM_DEF_10_40(	name, DEF )		PARAM_DEF_10_36( name, DEF )	DEF( name##_37 )	DEF( name##_38 )	DEF( name##_39 )	DEF( name##_40 )
#define	PARAM_DEF_10_44(	name, DEF )		PARAM_DEF_10_40( name, DEF )	DEF( name##_41 )	DEF( name##_42 )	DEF( name##_43 )	DEF( name##_44 )
#define	PARAM_DEF_10_47(	name, DEF )		PARAM_DEF_10_44( name, DEF )	DEF( name##_45 )	DEF( name##_46 )	DEF( name##_47 )	
#define	PARAM_DEF_10_48(	name, DEF )		PARAM_DEF_10_47( name, DEF )	DEF( name##_48 )
#define	PARAM_DEF_10_52(	name, DEF )		PARAM_DEF_10_48( name, DEF )	DEF( name##_49 )	DEF( name##_50 )	DEF( name##_51 )	DEF( name##_52 )
#define	PARAM_DEF_10_56(	name, DEF )		PARAM_DEF_10_52( name, DEF )	DEF( name##_53 )	DEF( name##_54 )	DEF( name##_55 )	DEF( name##_56 )
#define	PARAM_DEF_10_60(	name, DEF )		PARAM_DEF_10_56( name, DEF )	DEF( name##_57 )	DEF( name##_58 )	DEF( name##_59 )	DEF( name##_60 )
#define	PARAM_DEF_10_63(	name, DEF )		PARAM_DEF_10_60( name, DEF )	DEF( name##_61 )	DEF( name##_62 )	DEF( name##_63 )	
#define	PARAM_DEF_10_64(	name, DEF )		PARAM_DEF_10_63( name, DEF )	DEF( name##_64 )

#define	PARAM_DEF_2_3(		name, DEF )		DEF( name##_2 )				DEF( name##_3 )
#define	PARAM_DEF_2_4(		name, DEF )		PARAM_DEF_2_3( name, DEF )	DEF( name##_4 )
#define	PARAM_DEF_2_5(		name, DEF )		PARAM_DEF_2_4( name, DEF )	DEF( name##_5 )
#define	PARAM_DEF_2_6(		name, DEF )		PARAM_DEF_2_5( name, DEF )	DEF( name##_6 )
#define	PARAM_DEF_2_7(		name, DEF )		PARAM_DEF_2_6( name, DEF )	DEF( name##_7 )
#define	PARAM_DEF_2_8(		name, DEF )		PARAM_DEF_2_7( name, DEF )	DEF( name##_8 )
#define	PARAM_DEF_2_9(		name, DEF )		PARAM_DEF_2_8( name, DEF )	DEF( name##_9 )

#define	PARAM_DEF_2(		name, DEF )		DEF( name##_1 )				DEF( name##_2 )
#define	PARAM_DEF_3(		name, DEF )		PARAM_DEF_2( name, DEF )	DEF( name##_3 )
#define	PARAM_DEF_4(		name, DEF )		PARAM_DEF_3( name, DEF )	DEF( name##_4 )
#define	PARAM_DEF_5(		name, DEF )		PARAM_DEF_4( name, DEF )	DEF( name##_5 )
#define	PARAM_DEF_6(		name, DEF )		PARAM_DEF_5( name, DEF )	DEF( name##_6 )
#define	PARAM_DEF_7(		name, DEF )		PARAM_DEF_6( name, DEF )	DEF( name##_7 )
#define	PARAM_DEF_8(		name, DEF )		PARAM_DEF_7( name, DEF )	DEF( name##_8 )
#define	PARAM_DEF_9(		name, DEF )		PARAM_DEF_8( name, DEF )	DEF( name##_9 )

#define	PARAM_DEF_0_3(		name, DEF )		DEF( name##_0 )				PARAM_DEF_3( name, DEF )
#define	PARAM_DEF_0_4(		name, DEF )		DEF( name##_0 )				PARAM_DEF_4( name, DEF )
#define	PARAM_DEF_0_5(		name, DEF )		DEF( name##_0 )				PARAM_DEF_5( name, DEF )
#define	PARAM_DEF_0_6(		name, DEF )		DEF( name##_0 )				PARAM_DEF_6( name, DEF )
#define	PARAM_DEF_0_7(		name, DEF )		DEF( name##_0 )				PARAM_DEF_7( name, DEF )
#define	PARAM_DEF_0_8(		name, DEF )		DEF( name##_0 )				PARAM_DEF_8( name, DEF )
#define	PARAM_DEF_0_9(		name, DEF )		DEF( name##_0 )				PARAM_DEF_9( name, DEF )
#define	PARAM_DEF_0_15(		name, DEF )		PARAM_DEF_0_9( name, DEF )	PARAM_DEF_10_15( name, DEF )
#define	PARAM_DEF_0_31(		name, DEF )		PARAM_DEF_0_9( name, DEF )	PARAM_DEF_10_31( name, DEF )
#define	PARAM_DEF_0_47(		name, DEF )		PARAM_DEF_0_9( name, DEF )	PARAM_DEF_10_47( name, DEF )
#define	PARAM_DEF_0_63(		name, DEF )		PARAM_DEF_0_9( name, DEF )	PARAM_DEF_10_63( name, DEF )

#define	PARAM_DEF_04(		name, DEF )									DEF( name##_01 )	DEF( name##_02 )	DEF( name##_03 )	DEF( name##_04 )
#define	PARAM_DEF_07(		name, DEF )		PARAM_DEF_04( name, DEF )	DEF( name##_05 )	DEF( name##_06 )	DEF( name##_07 )
#define	PARAM_DEF_08(		name, DEF )		PARAM_DEF_07( name, DEF )	DEF( name##_08 )
#define	PARAM_DEF_09(		name, DEF )		PARAM_DEF_08( name, DEF )	DEF( name##_09 )

#define	PARAM_DEF_12(		name, DEF )		PARAM_DEF_9( name, DEF )	PARAM_DEF_10_12( name, DEF )
#define	PARAM_DEF_16(		name, DEF )		PARAM_DEF_9( name, DEF )	PARAM_DEF_10_16( name, DEF )
#define	PARAM_DEF_18(		name, DEF )		PARAM_DEF_9( name, DEF )	PARAM_DEF_10_18( name, DEF )
#define	PARAM_DEF_19(		name, DEF )		PARAM_DEF_9( name, DEF )	PARAM_DEF_10_19( name, DEF )
#define	PARAM_DEF_20(		name, DEF )		PARAM_DEF_9( name, DEF )	PARAM_DEF_10_20( name, DEF )
#define	PARAM_DEF_24(		name, DEF )		PARAM_DEF_9( name, DEF )	PARAM_DEF_10_24( name, DEF )
#define	PARAM_DEF_28(		name, DEF )		PARAM_DEF_9( name, DEF )	PARAM_DEF_10_28( name, DEF )
#define	PARAM_DEF_32(		name, DEF )		PARAM_DEF_9( name, DEF )	PARAM_DEF_10_32( name, DEF )
#define	PARAM_DEF_64(		name, DEF )		PARAM_DEF_9( name, DEF )	PARAM_DEF_10_64( name, DEF )
														 
#define	PARAM_DEF_01_16(	name, DEF )		PARAM_DEF_09( name, DEF )	PARAM_DEF_10_16( name, DEF )
#define	PARAM_DEF_01_18(	name, DEF )		PARAM_DEF_09( name, DEF )	PARAM_DEF_10_18( name, DEF )
#define	PARAM_DEF_01_20(	name, DEF )		PARAM_DEF_09( name, DEF )	PARAM_DEF_10_20( name, DEF )
#define	PARAM_DEF_01_24(	name, DEF )		PARAM_DEF_09( name, DEF )	PARAM_DEF_10_24( name, DEF )
#define	PARAM_DEF_01_32(	name, DEF )		PARAM_DEF_09( name, DEF )	PARAM_DEF_10_32( name, DEF )
#define	PARAM_DEF_01_64(	name, DEF )		PARAM_DEF_09( name, DEF )	PARAM_DEF_10_64( name, DEF )

#define	PARAM_DEF_00_03(	name, DEF )		DEF( name##_00 )				DEF( name##_01 )	DEF( name##_02 )	DEF( name##_03 )
#define	PARAM_DEF_00_07(	name, DEF )		DEF( name##_00 )				PARAM_DEF_07( name, DEF )
#define	PARAM_DEF_00_08(	name, DEF )		DEF( name##_00 )				PARAM_DEF_08( name, DEF )
#define	PARAM_DEF_00_09(	name, DEF )		DEF( name##_00 )				PARAM_DEF_09( name, DEF )
#define	PARAM_DEF_00_11(	name, DEF )		PARAM_DEF_00_09( name, DEF )	PARAM_DEF_10_11( name, DEF )
#define	PARAM_DEF_00_15(	name, DEF )		PARAM_DEF_00_09( name, DEF )	PARAM_DEF_10_15( name, DEF )

#define	PARAM_DEF_a_d(		name, DEF )		DEF( name##_a )				DEF( name##_b )		DEF( name##_c )		DEF( name##_d )

//MATRIX
#define	PARAM_DEF_MATRIX_44(			name, DIAGONAL, REGULAR )\
			DIAGONAL(	name##00	)	REGULAR(	name##01	)	REGULAR(	name##02	)	REGULAR(	name##03	)\
			REGULAR(	name##10	)	DIAGONAL(	name##11	)	REGULAR(	name##12	)	REGULAR(	name##13	)\
			REGULAR(	name##20	)	REGULAR(	name##21	)	DIAGONAL(	name##22	)	REGULAR(	name##23	)\
			REGULAR(	name##30	)	REGULAR(	name##31	)	REGULAR(	name##32	)	DIAGONAL(	name##33	)

#define	PARAM_DEF_MATRIX_REAL(			name )		PARAM_DEF_MATRIX_44(	name,	PARAM_DEF_FP32_ONE,		PARAM_DEF_FP32_ZERO		)
#define	PARAM_DEF_MATRIX_REAL_NULL(		name )		PARAM_DEF_MATRIX_44(	name,	PARAM_DEF_FP32_ZERO,	PARAM_DEF_FP32_ZERO		)
#define	PARAM_DEF_MATRIX_REAL_LOCKED(	name )		PARAM_DEF_MATRIX_44(	name,	PARAM_DEF_REAL_LOCKED,	PARAM_DEF_REAL_LOCKED	)

#define	PT_NB_STR( str )	(sizeof(str)/sizeof(CHAR*)-1)
//	SYMBO
#define	PARAM_DEF_SYMBO(				name,	def,ina, max, str )		PARAM_DEF_BASE_STR(		TYPE_SYMBOLIC,					name,	(def),(ina),	0,(max),			(str)	)
#define	PARAM_DEF_SYMBO_LOCKED(			name,	def,ina, max, str )		PARAM_DEF_BASE_STR(		TYPE_SYMBOLIC|M_LOCK,			name,	(def),(ina),	0,(max),			(str)	)
#define	PARAM_DEF_SYMBO_SAVE_NOT(		name,	def,ina, max, str )		PARAM_DEF_BASE_STR(		TYPE_SYMBOLIC|M_SAVE_NOT,		name,	(def),(ina),	0,(max),			(str)	)
#define	PARAM_DEF_SYMBO_SYNO(			name,	def,ina, max, str )		PARAM_DEF_BASE_STR(		TYPE_SYMBOLIC|M_SYNO,			name,	(def),(ina),	0,(max),			(str)	)

#define	PARAM_DEF_SYMBO_PSTR(			name,	def, ina, str )			PARAM_DEF_SYMBO(										name,	(def),(ina),	PT_NB_STR(str),		(str) )
//	the /2 in PARAM_DEF_SYMBO_SYNO_PSTR reflects the current "syno = 2 strings per value"
//	convention. If the syno parity ever generalises to N, this divisor and the M_SYNO comment
//	in param_def.h must move together.
#define	PARAM_DEF_SYMBO_SYNO_PSTR(		name,	def, ina, str )			PARAM_DEF_SYMBO_SYNO(									name,	(def),(ina),	(PT_NB_STR(str)/2), (str) )


#define	PARAM_DEF_SYMBO_PSTR_ZERO(		name,	str )					PARAM_DEF_SYMBO_PSTR(									name,	1,0,			(str) )
#define	PARAM_DEF_SYMBO_PSTR_ONE(		name,	str )					PARAM_DEF_SYMBO_PSTR(									name,	0,1,			(str) )
#define	PARAM_DEF_SYMBO_LOCKED_PSTR(	name,	str )					PARAM_DEF_SYMBO_LOCKED(									name,	1,0,			PT_NB_STR(str), (str) )
#define	PARAM_DEF_SYMBO_SAVE_NOT_PSTR(	name,	str )					PARAM_DEF_SYMBO_SAVE_NOT(								name,	1,0,			PT_NB_STR(str), (str) )

#define	PARAM_DEF_SYMBO_SYNO_PSTR_ZERO(	name,	str )					PARAM_DEF_SYMBO_SYNO_PSTR(								name,	1,0,			(str) )
#define	PARAM_DEF_SYMBO_SYNO_PSTR_ONE(	name,	str )					PARAM_DEF_SYMBO_SYNO_PSTR(								name,	0,1,			(str) )

#define	PARAM_DEF_SYMBO_NEG(			name,	def,ina, min,max, str )	PARAM_DEF_BASE_STR(		TYPE_SYMBO_NEG,					name,	(def),(ina),	(min),(max),		(str)	)
#define	PARAM_DEF_SYMBO_NEG_SAVE_NOT(	name,	def,ina, min,max, str )	PARAM_DEF_BASE_STR(		TYPE_SYMBO_NEG|M_SAVE_NOT,		name,	(def),(ina),	(min),(max),		(str)	)
#define	PARAM_DEF_SYMBO_ZERO(			name,	def,ina, min,max, str )	PARAM_DEF_BASE_STR(		TYPE_SYMBO_ZERO,				name,	(def),(ina),	(min),(max),		(str)	)
#define	PARAM_DEF_SYMBO_MIN_MAX(		name,	def,ina, min,max, str )	PARAM_DEF_BASE_STR(		TYPE_SYMBOLIC,					name,	(def),(ina),	(min),(max),		(str)	)
#define	PARAM_DEF_SYMBO_SYNO_MIN_MAX(	name,	def,ina, min,max, str )	PARAM_DEF_BASE_STR(		TYPE_SYMBOLIC|M_SYNO,			name,	(def),(ina),	(min),(max),		(str)	)


#define	PARAM_DEF_CLASS_BRANCH(			name,	def,ina, class_branch )	PARAM_DEF_BASE_STR(		TYPE_CLASS_BRANCH,				name,	(def),(ina),	0,PARAM_MAX_UINT32,	(class_branch)	)	
#define	PARAM_DEF_FILENAME(				name,	def,ina )				PARAM_DEF_BASE(			TYPE_FILENAME,					name,	(def),(ina),	0,0 )
#define	PARAM_DEF_DIRNAME(				name	)						PARAM_DEF_ALL_ZERO(		TYPE_DIRNAME,					name	)


#define	PARAM_DEF_GROUP_CLOSED(			name,	nb )					PARAM_DEF_BASE_QUOTE(	TYPE_GROUP_CLOSED|M_SAVE_NOT,	PARAM_DEF_GROUP_NAME(name),	(nb),0,		0,0 )
#define	PARAM_DEF_GROUP(				name,	nb )					PARAM_DEF_BASE_QUOTE(	TYPE_GROUP|M_SAVE_NOT,			PARAM_DEF_GROUP_NAME(name),	(nb),0,		0,0 )
																								
#define	PARAM_DEF_NONE(					name )							PARAM_DEF_BASE_QUOTE(	TYPE_NONE|M_SAVE_NOT,			PARAM_DEF_NONE_NAME(name),	0,0,		0,0	)

#define	PARAM_DEF_BIND_1D(				name )							PARAM_DEF_INT32(										name,	1,0,	0,BIND_1D_MAX_NB	)
#define	PARAM_DEF_BIND_1D_CURRENT(		name )							PARAM_DEF_SYMBO_NEG(									name,	1,0,	-1,BIND_1D_MAX_NB,	gstr::current )

#define	PARAM_DEF_BIND_2D_SYMBO(		name,	def,ina,	str )		PARAM_DEF_SYMBO_NEG(									name,	(def),(ina),	-1,PARAM_MAX_INT32,	(str) )	
#define	PARAM_DEF_BANK_2D(				name )							PARAM_DEF_INT32_POS_ZERO(								name	)
#define	PARAM_DEF_BANK_2D_CURRENT(		name )							PARAM_DEF_BIND_2D_SYMBO(								name,	1,0,	gstr::current )	
#define	PARAM_DEF_BIND_2D(				name )							PARAM_DEF_INT32_POS_ZERO(								name	)
#define	PARAM_DEF_BIND_2D_CURRENT(		name )							PARAM_DEF_BIND_2D_SYMBO(								name,	1,0,	gstr::current )
#define	PARAM_DEF_BIND_2D_CURRENT_SEL(	name )							PARAM_DEF_BIND_2D_SYMBO(								name,	0,-1,	gstr::current )	
#define	PARAM_DEF_BANK_BIND_1D_OUT(		name )							PARAM_DEF_INT32_LOCKED(									name	)
#define	PARAM_DEF_BANK_BIND_2D_OUT(		name )							PARAM_DEF_INT32_LOCKED(									name	)
#define	PARAM_DEF_BANK_BIND_3D_OUT(		name )							PARAM_DEF_INT32_LOCKED(									name	)
#define	PARAM_DEF_BIND_2D_ALONE(		name )							PARAM_DEF_INT32_POS_ZERO(								name	)
#define	PARAM_DEF_BIND_2D_ALONE_DEF_INA(name,	def,ina	)				PARAM_DEF_INT32_POS(									name,	(def),(ina)		)

#define	PARAM_DEF_BIND_3D(				name )							PARAM_DEF_INT32_POS_ZERO(								name	)
#define	PARAM_DEF_BIND_3D_CURRENT(		name )							PARAM_DEF_SYMBO_NEG(									name,	1,0,	-1,PARAM_MAX_INT32,	gstr::current )

//extern	C_PCHAR_C	g_str_bind[1];
//#define	PARAM_DEF_IMG_OUT(				name )							PARAM_DEF_INT32_LOCKED(									name	)
//#define	PARAM_DEF_IMG(					name )							PARAM_DEF_INT32_POS_ZERO(								name	)
//#define	PARAM_DEF_IMG_BIND(				name )							PARAM_DEF_SYMBO_NEG(									name,	0,-1,			-1,PARAM_MAX_INT32,	 g_str_bind )
//#define	PARAM_DEF_IMG_DEF_INA(			name,	def,ina	)				PARAM_DEF_INT32_POS(									name,	(def),(ina)		)
//extern	C_PCHAR_C	g_str_same[1];
//#define	PARAM_DEF_IMG_SAME(				name )							PARAM_DEF_SYMBO_NEG(									name,	0,-1,			-1,PARAM_MAX_INT32,	 g_str_same )
#define	PARAM_DEF_IMG_SIZE(				name,	def,ina )				PARAM_DEF_INT32(										name,	(def),(ina),	4, 1024*1024 )
#define	PARAM_DEF_IMG_SIZE_FORMAT()		PARAM_DEF_INT32_XY(	size_asked,			512,256,	4,128*1024	)\
										PARAM_DEF_INT32(	channel_nb,			4,3,		1,4			)\
										PARAM_DEF_SYMBO(	channel_type,		GOL::INTERNAL_TYPE::UINT_16,GOL::INTERNAL_TYPE::UINT_8,	(INT32)GOL::INTERNAL_TYPE::FLOAT_32,GOL::internal_type_str	)
#define PARAM_IMG_SIZE_NB	4