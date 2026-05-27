#include "param_def.h"
#include "param_declare.h"

C_PCHAR_C	aaa::param::param_action_str[ aaa::param::ACTION_NB ] =
{
	"Inc",		"Dec",
	"Inc Loop",	"Dec Loop",
	"Mul",		"Div",
	"Max",		"Min",
	"Default",	"Inactive",
	"Integer",
	"Sign",
	"Previous",	"Next",
	"Pop",		"Push",
	"Open",
	"New",		"Forget",
};

C_PCHAR_C aaa::param::type_str_array[aaa::param::TYPE_MAX_NB] =
{
	"None",

	"Int8",
	"UInt8",
	"Int16",
	"Uint16",
	"Int32",
	"UInt32",
	"Int64",
	"UInt64",

	"Bool",

	"Fp32"	,
	"Double",

	"Symbolic_negative",
	"Symbolic_zero",
	"Symbolic",

	"String",
	"File_name",
	"Dir_name",
	"Reference",

	"class_branch",
	"Bit32",
	"TimeCode",
	"Group",
	"Group_closed",
};

C_PCHAR_C aaa::param::type_internal_str_array[aaa::param::TYPE_INTERNAL_MAX_NB] =
{
	"NONE",
	"UNIMPLEMENTED",
	"BOOL",

	"INT8",		"UINT8",
	"INT16",	"UINT16",
	"INT32",	"UINT32",
	"INT64",	"UINT64",

	"FP32",		"DOUBLE",

	"STRING"
};




#define	DEF_BASE(	type, def, ina, min, max )			UINT32(type),	REAL(def), REAL(ina),	REAL(min), REAL(max)
#define	DEF_GENE(	type, def, ina, min, max )			DEF_BASE(		type,			def, ina,	min, max )
#define DEF_REAL(	def, ina, min, max )				DEF_GENE(		TYPE_REAL,		def, ina,	min, max )
#define DEF_REAL_ONE_ZERO()								DEF_GENE(		TYPE_REAL,		1, 0,	0, 8192 )
#define DEF_INT(	def, ina, min, max )				DEF_GENE(		TYPE_INT32,		def, ina, min, max )


//below are experiments
#if 0

//__COUNTER__
INT32 tmp = 0; 
#define DO( name, pt, def, ... ) constexpr INT32 name##_INDEX = tmp; tmp = name##_INDEX + 1;
#include "param_def_macro.h"
#undef DO

#define DO( name, pt, def, ... ) c_param_def( #name, def ),
constexpr c_param_def	defs[3] = {
	#include "param_def_macro.h"
};
#undef DO

#define NO *nullptr
//#define DO( name, pt_name, def, ... ) defs[name##_INDEX]._pt = strcmp( #pt_name, str_no ) == 0 ? nullptr : &(pt_name);
#define DO( name, pt_name, def, ... ) defs[name##_INDEX]._pt = &(pt_name);
void assign_pt()
{
	static const str_no = "NO";
	#include "param_def_macro.h"
}
#undef DO
#undef NO

#endif

#if 0
#include <array>
#include <string>

struct param {
    std::string name;
    int value;
};

#define DO( name, pt, def, ... ) c_param_def( #name, def ),
constexpr std::array<c_param_def, 3>	defs = {
	#include "param_def_macro.h"
};
#undef DO

#define DO( name, pt, def, ... ) constexpr INT32 name ## _INDEX = []() { \
    INT32 index = 0; \
    for (const auto& p : defs) { \
        if (p.name == #name) { \
            return index; \
        } \
        ++index; \
    } \
    return -1; \
}()
#include "param_def_macro.h"
#undef DO

#define NO *nullptr
//#define DO( name, pt_name, def, ... ) defs[name##_INDEX]._pt = strcmp( #pt_name, str_no ) == 0 ? nullptr : &(pt_name);
#define DO( name, pt_name, def, ... ) defs[name##_INDEX]._pt = &(pt_name);
void assign_pt()
{
	static const str_no = "NO";
	#include "param_def_macro.h"
}
#undef DO
#undef NO



#endif

#if 0
class obj_ui {
public:
    struct param {
        std::string name;
        INT32 *ptr;
        INT32 index;
    };

    void add_param(const std::string &name, INT32 &var)
	{
        INT32 index = params.size();
        params[name] = index;
        param_info.push_back({name, &var, index});
    }

    std::map<std::string, INT32> params;
    std::vector<param> param_info;
};

INT32 main() {
    obj_ui ui;
    INT32 x = 0;
    INT32 y = 0;
    ui.add_param("x", x);
    ui.add_param("y", y);

    return 0;
}
#endif