// tests/unit/aaalua_factory_param_stub.cpp
//
// c112 (2026-05-26) -- final link-symbol stubs for the aaalua trio :
//   - c_factory_base ctor / dtor / create_obj / count_concrete + typeinfo (5 syms)
//   - c_param_def ctor (8-arg w/ fn ptr) + dtor (2 syms)
//   - c_param::attach_obj / c_param::get_value_as_real (2 syms)
//   - c_params::get(int) const (1 sym)
//
// Strategy mirrors c108's c_obj_ui stub : FAKED class declarations with
// NO member subobjects + NO inheritance. Symbol mangling is by qualified
// name + parameter list (NOT class layout), so the linker accepts these
// stubs as resolving the references from aaalua_master.cpp.o etc.
//
// CAUTION : audit target is EXCLUDE_FROM_ALL ; never executes. Faked
// classes would crash on real instantiation (no real members) but link
// successfully.
//
// Why no real header includes : pulling the real factory.h / param.h
// triggers o_str/STL member declarations -> c_factory_base ctor body
// would emit o_str::o_str() / std::list ctor refs -> lazy-pull of
// aaa_str.cpp/aaa_mem.cpp from libaaaseed_code_utils.a -> duplicate-
// symbol collisions with test_engine_stubs_no_ostr.cpp's mem stubs.
// The c108 doctrine resolves this : fake the class, skip the cascade.

#include "aaa_type.h"

//	Forward-decls only (no body pulls).
class c_obj_ui;
class c_param;

//	c_factory_base faked declaration -----------------------------------------

class c_factory_base
{
public:
    static  INT32           count_concrete;

    virtual c_obj_ui*       create_obj();
    virtual c_obj_ui*       new_hidden() = 0;
    virtual void            delete_hidden( c_obj_ui* obj ) = 0;

    c_factory_base( C_PCHAR_C cid, c_factory_base* super_factory,
                    C_PCHAR_C human = nullptr,
                    C_PCHAR_C ext = nullptr,
                    C_PCHAR_C props = nullptr );
    virtual ~c_factory_base();
};

INT32 c_factory_base::count_concrete = 0;

c_factory_base::c_factory_base( C_PCHAR_C, c_factory_base*, C_PCHAR_C, C_PCHAR_C, C_PCHAR_C )
{
    //	Empty body : faked class has no members, no subobject ctors emitted.
}

c_factory_base::~c_factory_base()
{
    //	Mirror.
}

c_obj_ui* c_factory_base::create_obj()
{
    return nullptr;
}

//	c_param_def faked declaration --------------------------------------------

class c_param_def
{
public:
    c_param_def( UINT32 type, C_PCHAR_C name, FP32 def, FP32 min, FP32 max, FP32 step,
                 void (*on_change)( c_param* ), C_PCHAR_C const* symbo );
    ~c_param_def();
};

c_param_def::c_param_def( UINT32, C_PCHAR_C, FP32, FP32, FP32, FP32, void (*)(c_param*), C_PCHAR_C const* )
{
}

c_param_def::~c_param_def() = default;

//	c_param faked declaration ------------------------------------------------

class c_param
{
public:
    void   attach_obj( c_obj_ui* CONST obj );
    REAL   get_value_as_real() CONST;
};

void c_param::attach_obj( c_obj_ui* CONST )
{
}

REAL c_param::get_value_as_real() CONST
{
    return REAL{ 0 };
}

//	c_params faked declaration -----------------------------------------------

class c_params
{
public:
    c_param* get( INT32 index ) CONST;
};

c_param* c_params::get( INT32 ) CONST
{
    return nullptr;
}

//	luasocket linkage-mismatch bridge (c113) -------------------------------
//	mime.h + luasocket.h declare luaopen_* WITHOUT `extern "C"`, so the
//	C++ TU `aaalua_util.cpp` emits a C++-mangled ref. The lib (compiled
//	from `.c`) exports C-mangled. Lazy-link should pull only the needed
//	luasocket TUs (luasocket.c.o, mime.c.o) without the whole archive,
//	avoiding the c112 force_load secondary cascade.
//
//	Bridge : pin a C-linkage decl to the lib's C symbol via asm() rename,
//	then provide a C++-linkage forwarder with the name aaalua_util.cpp expects.
//	Weak attrs so a future vendor patch (`extern "C"` in mime.h/luasocket.h)
//	or a real port supersedes automatically.

struct lua_State;

extern "C" int luaopen_socket_core_lib_( lua_State* L ) asm( "_luaopen_socket_core" );
extern "C" int luaopen_mime_core_lib_(   lua_State* L ) asm( "_luaopen_mime_core" );

__attribute__((weak))
int luaopen_socket_core( lua_State* L )
{
    return luaopen_socket_core_lib_( L );
}

__attribute__((weak))
int luaopen_mime_core( lua_State* L )
{
    return luaopen_mime_core_lib_( L );
}
