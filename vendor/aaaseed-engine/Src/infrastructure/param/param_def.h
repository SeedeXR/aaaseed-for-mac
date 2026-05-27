
#ifdef AAA_PARAM_DEF_H
#error "PARAM_DEF_H included more than once."
#endif
#define AAA_PARAM_DEF_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
	
class c_param;

namespace aaa {
	namespace param {

		typedef void fn_update(c_param* param);

		CONSTEXPR	REAL	PARAM_MIN_INT8		= REAL(aaa::LOWEST<INT8>);
		CONSTEXPR	REAL	PARAM_MAX_INT8		= REAL(aaa::BIGGEST<INT8>);
		CONSTEXPR	REAL	PARAM_MAX_UINT8		= REAL(aaa::BIGGEST<UINT8>);

		CONSTEXPR	REAL	PARAM_MIN_INT16		= REAL(aaa::LOWEST<INT16>);
		CONSTEXPR	REAL	PARAM_MAX_INT16		= REAL(aaa::BIGGEST<INT16>);
		CONSTEXPR	REAL	PARAM_MAX_UINT16	= REAL(aaa::BIGGEST<UINT16>);

		CONSTEXPR	REAL	PARAM_MIN_INT32		= REAL(aaa::LOWEST<INT32>);
		// tested on Visual Studio 2022 not to overflow
		CONSTEXPR	REAL	PARAM_MAX_INT32		= REAL(aaa::BIGGEST<INT32> & ~0x7f);	//REAL use 24 bits, so we clamp to the bigger number using higher 24 bits
		CONSTEXPR	REAL	PARAM_MAX_UINT32	= REAL(aaa::BIGGEST<UINT32>);

		CONSTEXPR	REAL	PARAM_MIN_FP32		= REAL(aaa::LOWEST<FP32>);
		CONSTEXPR	REAL	PARAM_MAX_FP32		= REAL(aaa::BIGGEST<FP32>);

		//	A param's min/max are stored in REAL, which is FP32 in the default build, so even
		//	for TYPE_DOUBLE the limits saturate at FP32 representable range. A user supplying
		//	an FP64 limit silently gets clamped to +/- 3.4e38. Same for PARAM_MIN_REAL /
		//	PARAM_MAX_REAL below.	(2023 April, awaiting decision on widening the limit type.)
		CONSTEXPR	REAL	PARAM_MIN_DOUBLE	= REAL(aaa::LOWEST<FP32>);
		CONSTEXPR	REAL	PARAM_MAX_DOUBLE	= REAL(aaa::BIGGEST<FP32>);

		CONSTEXPR	REAL	PARAM_MIN_REAL		= REAL(aaa::LOWEST<FP32>);
		CONSTEXPR	REAL	PARAM_MAX_REAL		= REAL(aaa::BIGGEST<FP32>);


		CONSTEXPR	UINT32	TYPE_MASK = 0x000000ff;	// type live here
		CONSTEXPR	INT32	FLAG_SHIFT = 16;


		enum ACTION : INT32
		{
			PARAM_INC, PARAM_DEC,
			PARAM_INC_LOOP, PARAM_DEC_LOOP,
			PARAM_MUL, PARAM_DIV,
			PARAM_MAX, PARAM_MIN,
			PARAM_DEF, PARAM_INA,
			PARAM_ROUND,
			PARAM_SIGN,
			PARAM_PREV, PARAM_NEXT,
			PARAM_POP, PARAM_PUSH,
			PARAM_OPEN,
			PARAM_NEW, PARAM_FORGET,
			ACTION_NB
		};

		extern C_PCHAR_C	param_action_str[aaa::param::ACTION_NB];

		//todo INT64 BIT64
		//todo HEX
		enum TYPE : UINT32
		{
			TYPE_NONE = 0,
			TYPE_INT8,		TYPE_UINT8,		// unimplemented yet
			TYPE_INT16,		TYPE_UINT16,	// unimplemented yet
			TYPE_INT32,		TYPE_UINT32,
			TYPE_INT64,		TYPE_UINT64,	// unimplemented yet
			TYPE_BOOL,
			TYPE_FP32,
			TYPE_DOUBLE,

			TYPE_SYMBO_NEG, TYPE_SYMBO_ZERO, TYPE_SYMBOLIC,
			TYPE_STR, TYPE_FILENAME, TYPE_DIRNAME, TYPE_REF,	//	like PARAM_STR but no control code (1-31) -> only one line too										
			TYPE_CLASS_BRANCH, TYPE_BIT32,	//	todoq have to be check everywhere
			TYPE_TIMECODE,		//FP32 like float just display is different (subtype /)
			TYPE_GROUP, TYPE_GROUP_CLOSED,
		

			TYPE_MAX_NB,

			//unused		TYPE_LIST		=	0x200,
			M_LOCK = 1 << FLAG_SHIFT,			//	can't be change or saved
			M_SAVE_NOT = 1 << (FLAG_SHIFT + 1),	//	can be changed unlike TYPE_LOCK but most of the time belong to another object who save it
			//	M_SYNO : for TYPE_SYMBOLIC*, the symbol-string array carries SYNONYMS (multiple
			//	display names for the same integer value). For now (2026 May) "synonyms" means
			//	exactly 2 strings per value, so the array is twice as long as max-min+1, hence
			//	the historical "x2" naming and the hardcoded /2 in PARAM_DEF_SYMBO_SYNO_PSTR.
			//	The vocabulary is "syno" because the design is meant to generalise to N strings
			//	per value if the need ever arises.
			M_SYNO = 1 << (FLAG_SHIFT + 2),
//			M_MAA_FLAG = 1 << (FLAG_SHIFT + 3),	//	hack to try stuff
#if AAA_STATE_COMPILE()
			M_STATE_INSENSITIVE = 1 << (FLAG_SHIFT + 4),
#endif //AAA_STATE_COMPILE
		};

		enum TYPE_INTERNAL : UINT32
		{
			TYPE_INTERNAL_NONE = 0,
			TYPE_INTERNAL_UNIMPLEMENT,
			TYPE_INTERNAL_BOOL,

			TYPE_INTERNAL_INT8,		TYPE_INTERNAL_UINT8,
			TYPE_INTERNAL_INT16,	TYPE_INTERNAL_UINT16,
			TYPE_INTERNAL_INT32,	TYPE_INTERNAL_UINT32,
			TYPE_INTERNAL_INT64,	TYPE_INTERNAL_UINT64,

			TYPE_INTERNAL_FP32,		TYPE_INTERNAL_DOUBLE,

			TYPE_INTERNAL_STRING,
			TYPE_INTERNAL_MAX_NB
		};

		struct st_type_info {
			TYPE_INTERNAL	type_internal;
			bool			b_number;
			bool			b_text;
			C_PCHAR_C		name;
			C_PCHAR_C		name_lowercase;
		};

		CONSTEXPR struct st_type_info type_info[TYPE_MAX_NB] =
		{	//	type_internal				b_number b_text	name					name_lowercase
			{	TYPE_INTERNAL_NONE			,false	,false	,"None"					,"none"					},	

			{	TYPE_INTERNAL_UNIMPLEMENT	,true	,false	,"Int8"					,"int8"					},
			{	TYPE_INTERNAL_UNIMPLEMENT	,true	,false	,"UInt8"				,"uint8"				},
			{	TYPE_INTERNAL_UNIMPLEMENT	,true	,false	,"Int16"				,"int16"				},
			{	TYPE_INTERNAL_UNIMPLEMENT	,true	,false	,"Uint16"				,"uint16"				},
			{	TYPE_INTERNAL_INT32			,true	,false	,"Int32"				,"int32"				},
			{	TYPE_INTERNAL_UINT32		,true	,false	,"UInt32"				,"uint32"				},
			{	TYPE_INTERNAL_UNIMPLEMENT	,true	,false	,"Int64"				,"int64"				},
			{	TYPE_INTERNAL_UNIMPLEMENT	,true	,false	,"UInt64"				,"uint64"				},

			{	TYPE_INTERNAL_BOOL			,true	,false	,"Bool"					,"bool"					},	

			{	TYPE_INTERNAL_FP32			,true	,false	,"Fp32"					,"fp32"					},
			{	TYPE_INTERNAL_DOUBLE		,true	,false	,"Double"				,"double"				},

			{	TYPE_INTERNAL_INT32			,true	,false	,"Symbolic_negative"	,"symbolic_negative"	},
			{	TYPE_INTERNAL_UINT32		,true	,false	,"Symbolic_zero"		,"symbolic_zero"		},
			{	TYPE_INTERNAL_UINT32		,true	,false	,"Symbolic"				,"symbolic"				},

			{	TYPE_INTERNAL_STRING		,false	,true	,"String"				,"string"				},
			{	TYPE_INTERNAL_STRING		,false	,true	,"File_name"			,"file_name"			},
			{	TYPE_INTERNAL_STRING		,false	,true	,"Dir_name"				,"dir_name"				},
			{	TYPE_INTERNAL_STRING		,false	,true	,"Reference"			,"reference"			},

			{	TYPE_INTERNAL_UINT32		,true	,false	,"Class_branch"			,"class_branch"			},
			{	TYPE_INTERNAL_UINT32		,true	,false	,"Bit32"				,"bit32"				},
			{	TYPE_INTERNAL_FP32			,true	,false	,"TimeCode"				,"timeCode"				},

			{	TYPE_INTERNAL_NONE			,false	,false	,"Group"				,"group"				},
			{	TYPE_INTERNAL_NONE			,false	,false	,"Group_closed"			,"group_closed"			},
		};

		FINLINE	CONSTEXPR	TYPE			make_type( TYPE CONST type, UINT32 CONST u32 )	{ return TYPE( UINT32(type) | (u32 & ~TYPE_MASK) ); }
		FINLINE	CONSTEXPR	TYPE			get_type(					UINT32 CONST u32 )	{ return TYPE(u32 & TYPE_MASK);						}
		FINLINE	CONSTEXPR	TYPE_INTERNAL	get_type_internal(			UINT32 CONST u32 )	{ return type_info[get_type(u32)].type_internal;	}
		FINLINE	CONSTEXPR	TYPE_INTERNAL	get_type_internal(			TYPE CONST type )	{ return type_info[type].type_internal;				}
		FINLINE	CONSTEXPR	bool			is_type_number(				TYPE CONST type )	{ return type_info[type].b_number;					}
																					// was	return b_type_internal_number[type_to_internal[type_in]];
		FINLINE	CONSTEXPR	bool			is_type_group(				TYPE CONST type )	{ return type==TYPE_GROUP_CLOSED || type==TYPE_GROUP;	}
		FINLINE	CONSTEXPR	bool			is_type_group_closed(		TYPE CONST type )	{ return type == TYPE_GROUP_CLOSED; }
		FINLINE	CONSTEXPR	bool			is_type_text(				TYPE CONST type )	{ return type_info[type].b_text; }

		FINLINE	CONSTEXPR bool	is_type_real( TYPE CONST type_in )
		{
			switch( type_in )
			{
			case TYPE_FP32:
			case TYPE_DOUBLE:
				return true;
			default:
				return false;
			}
		}
		FINLINE	CONSTEXPR bool	is_type_bool( TYPE CONST type_in )
		{
			switch( type_in )
			{
			case TYPE_BOOL:
				return true;
			default:
				return false;
			}
		}

		extern C_PCHAR_C type_str_array[aaa::param::TYPE_MAX_NB];	// still needed for param_spe_draw_one_line
		FINLINE	CONSTEXPR	C_PCHAR_C	get_type_str( UINT32 type )
		{
			type = get_type(type);
			if( type < TYPE_MAX_NB )
				return type_info[type].name;
			else
				return "Unknown type";
		}
		FINLINE	CONSTEXPR	C_PCHAR_C	get_type_str_lowercase( UINT32 type )
		{
			type = get_type(type);
			if( type < TYPE_MAX_NB )
				return type_info[type].name_lowercase;
			else
				return "unknown";
		}

		extern C_PCHAR_C type_internal_str_array[aaa::param::TYPE_INTERNAL_MAX_NB];
		CONSTEXPR	C_PCHAR_C	get_type_internal_str( TYPE_INTERNAL type_internal )
		{
			if( type_internal < TYPE_INTERNAL_MAX_NB )
				return type_internal_str_array[type_internal];
			else
				return "Unknown internal type";
		}

#if	AAA_REAL_IS_DOUBLE()
		CONSTEXPR UINT32 TYPE_REAL = TYPE_DOUBLE;
#else
		CONSTEXPR UINT32 TYPE_REAL = TYPE_FP32;
#endif

		FINLINE	CONSTEXPR	bool	is_save(		UINT32 CONST u32) { return (u32 & M_SAVE_NOT) == 0; }
		FINLINE	CONSTEXPR	UINT32	disable_save(	UINT32 CONST u32) { return  u32 | M_SAVE_NOT; }
		FINLINE	CONSTEXPR	UINT32	enable_save(	UINT32 CONST u32) { return  u32 & ~M_SAVE_NOT; }
		FINLINE	CONSTEXPR	UINT32	set_save(		UINT32 CONST u32, bool CONST b_save)
		{
			if (b_save)
				return enable_save(u32);
			else
				return disable_save(u32);
		}

		FINLINE	CONSTEXPR	bool	is_lock(		UINT32 CONST u32) { return (u32 & M_LOCK) != 0; }
		FINLINE	CONSTEXPR	UINT32	enable_lock(	UINT32 CONST u32) { return  u32 | M_LOCK; }
		FINLINE	CONSTEXPR	UINT32	disable_lock(	UINT32 CONST u32) { return  u32 & ~M_LOCK; }

		FINLINE	CONSTEXPR	bool	is_syno(		UINT32 CONST u32) { return (u32 & M_SYNO) != 0; }
//		FINLINE	CONSTEXPR	UINT32	enable_syno(	UINT32 CONST u32) { return  u32 | M_SYNO; }
//		FINLINE	CONSTEXPR	UINT32	disable_syno(	UINT32 CONST u32) { return  u32 & ~M_SYNO; }



}	//	namespace param
}	//namespace aaa

