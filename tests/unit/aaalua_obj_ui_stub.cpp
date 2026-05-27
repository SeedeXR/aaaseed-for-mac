// aaalua_obj_ui_stub.cpp
//
// c108 link-bucket port -- defines the c_obj_ui base class symbols
// referenced by the aaalua_*.cpp trio (master / wrap / util) and the
// other aaalua TUs that link into aaaseed_aaalua_trio_link_audit_tests.
//
// The c107 audit found 37 unique c_obj_ui::* undefined symbols out of
// 90 total. Real obj_ui.cpp is 2364 LOC and pulls module / param_focus /
// param_draw / bdd_cell_draw / namer / net_blk / OscReceivedElements /
// spy / root / aaa_dir -- a multi-session cascade. We instead define
// stub bodies in THIS TU using a deliberately-faked `c_obj_ui` class
// declaration (no member subobjects, no inheritance) so that :
//   * the mangled symbol names match (mangling is name-based for
//     non-virtual member fns and static data) ;
//   * the ctor body does NOT pull `c_params::c_params()` /
//     `c_obj::c_obj()` etc. as new undefined symbols, which would
//     simply move the problem one layer deeper.
//
// IMPORTANT : this file is for the aaaseed_aaalua_trio_link_audit_tests
// target ONLY. Other test targets pull in their own c_obj_ui handling
// (header-only smokes) or rely on engine TUs not being in their link.
// DO NOT add this .cpp to any other target.
//
// Scope (37 symbols) :
//   ctor : c_obj_ui(c_factory_base*)
//   dtor : ~c_obj_ui() (pure virtual definition)
//   static data : ref_obj_nb, ref_param_nb
//   static method : the_factory()
//   virtual methods : set_active, update_lua, update_then_draw,
//       update_then_draw_lua, prepare_for_ui, param_init,
//       param_init_pt_static, param_init_with, param_do_action,
//       do_command, enum_command, cell_draw_obj, osc_process_message,
//       save_to_file, save_to_this_file, save_do_before, save_do_after,
//       load_from_file, load_from_existing_file, load_do_before,
//       load_do_after, get_comment, get_file_version_save
//   non-virtual : obj_new_private, make_script_filename,
//       get_name_str, get_obj_sub_by_index, print_string, dbg_print,
//       err_print, ui_print, box_err
//
// Once Mac ports the real obj_ui.cpp (multi-session, blocked on the
// param_draw + bdd_cell_draw + module subsystems), retire this file.

#include <cstdarg>
#include <cstdio>

#include "aaa_type.h"

//  Forward decls -- the real types from the engine. We only need the
//  names for mangling purposes ; layout does not matter because the
//  stub class below has no members and no base, so no subobject
//  ctor/dtor calls are emitted.
class c_factory_base;
class c_obj_ui_fwd;     // alias for our stub class' return type
class c_param;
class c_param_def;
class c_root;
class o_str;
namespace osc { class ReceivedMessage; }
namespace aaa { namespace param { enum ACTION : INT32; } }

template < class T > class c_factory_abstract;

//  Match the engine's AAA_ERR typedef. Already in aaa_type.h but kept
//  explicit here for grep-ability.
typedef INT32 AAA_ERR;

//  --------------------------------------------------------------------
//  Faked c_obj_ui class declaration.
//
//  Empty layout : no inheritance, no members. Mangled names of member
//  fns are identical to the real class (mangling is by qualified name
//  + parameter list, not by class layout). Ctor and dtor have no
//  subobject init lists, so they emit no upstream symbol references.
//
//  CAUTION : if a future call site needs to take the address of a
//  c_obj_ui object (sizeof, pointer arithmetic over an array, vtable
//  introspection), this fake will produce wrong code. The stub is
//  for LINK ONLY -- the audit executable never runs.
//  --------------------------------------------------------------------
class c_obj_ui
{
public:
    c_obj_ui( c_factory_base* );
    virtual ~c_obj_ui() = 0;

    static c_factory_abstract<c_obj_ui>& the_factory();

    static UINT32 ref_obj_nb;
    static UINT32 ref_param_nb;

    virtual void    set_active( bool );
    virtual void    update_lua();
    virtual void    update_then_draw();
    virtual bool    update_then_draw_lua();
    virtual void    prepare_for_ui();
    virtual void    param_init();
    virtual void    param_init_pt_static();
            void    param_init_with( c_param_def const*, INT32 );
    virtual bool    param_do_action( c_param*, aaa::param::ACTION );
    virtual bool    do_command( char const* );
    virtual bool    enum_command( o_str& );
    virtual void    cell_draw_obj( REAL );
    virtual void    osc_process_message( osc::ReceivedMessage const& );
    virtual AAA_ERR save_to_file( o_str const& );
    virtual AAA_ERR save_to_this_file( o_str const& );
    virtual AAA_ERR save_do_before( o_str const& );
    virtual AAA_ERR save_do_after( o_str const& );
    virtual AAA_ERR load_from_file( o_str const& );
    virtual AAA_ERR load_from_existing_file( o_str const& );
    virtual AAA_ERR load_do_before( o_str const& );
    virtual AAA_ERR load_do_after( o_str const& );
    virtual o_str*  get_comment() const;
    virtual INT32   get_file_version_save() const;

            void    obj_new_private( c_obj_ui* );
            bool    make_script_filename( o_str&, o_str const&, char const*, o_str* );

    char const*     get_name_str() const;
    c_obj_ui*       get_obj_sub_by_index( INT32 ) const;

    void            print_string( char const* fmt, ... ) const;
    void            dbg_print(    char const* fmt, ... ) const;
    void            err_print(    char const* fmt, ... ) const;
    void            ui_print(     char const* fmt, ... ) const;
    void            box_err(      char const* fmt, ... ) const;
};

//  --------------------------------------------------------------------
//  Static data definitions.
//  --------------------------------------------------------------------
UINT32 c_obj_ui::ref_obj_nb   = 0;
UINT32 c_obj_ui::ref_param_nb = 0;

//  --------------------------------------------------------------------
//  the_factory() returns a reference to a static c_factory_abstract<>.
//  We cannot construct a real one (would pull factory.cpp + registry
//  cascade). Return a reference to a deliberately-uninitialised char
//  buffer cast through the template type. The audit target never
//  executes -- it only links.
//  --------------------------------------------------------------------
c_factory_abstract<c_obj_ui>& c_obj_ui::the_factory()
{
    //  alignas to a generous alignment so the reinterpret cast does not
    //  trip UBSAN if ever exercised. Storage is class-static.
    alignas( 16 ) static char buf[ 256 ] = {};
    return *reinterpret_cast<c_factory_abstract<c_obj_ui>*>( buf );
}

//  --------------------------------------------------------------------
//  ctor / dtor -- empty bodies. No subobject init lists, so no upstream
//  ctor calls emitted.
//  --------------------------------------------------------------------
c_obj_ui::c_obj_ui( c_factory_base* /*factory*/ )
{
}

c_obj_ui::~c_obj_ui()
{
}

//  --------------------------------------------------------------------
//  Virtual methods -- no-op / identity-return.
//  --------------------------------------------------------------------
void    c_obj_ui::set_active( bool )                              {}
void    c_obj_ui::update_lua()                                    {}
void    c_obj_ui::update_then_draw()                              {}
bool    c_obj_ui::update_then_draw_lua()                          { return false; }
void    c_obj_ui::prepare_for_ui()                                {}
void    c_obj_ui::param_init()                                    {}
void    c_obj_ui::param_init_pt_static()                          {}
void    c_obj_ui::param_init_with( c_param_def const*, INT32 )    {}
bool    c_obj_ui::param_do_action( c_param*, aaa::param::ACTION ) { return false; }
bool    c_obj_ui::do_command( char const* )                       { return false; }
bool    c_obj_ui::enum_command( o_str& )                          { return false; }
void    c_obj_ui::cell_draw_obj( REAL )                           {}
void    c_obj_ui::osc_process_message( osc::ReceivedMessage const& ) {}

AAA_ERR c_obj_ui::save_to_file(           o_str const& ) { return 0; }
AAA_ERR c_obj_ui::save_to_this_file(      o_str const& ) { return 0; }
AAA_ERR c_obj_ui::save_do_before(         o_str const& ) { return 0; }
AAA_ERR c_obj_ui::save_do_after(          o_str const& ) { return 0; }
AAA_ERR c_obj_ui::load_from_file(         o_str const& ) { return 0; }
AAA_ERR c_obj_ui::load_from_existing_file(o_str const& ) { return 0; }
AAA_ERR c_obj_ui::load_do_before(         o_str const& ) { return 0; }
AAA_ERR c_obj_ui::load_do_after(          o_str const& ) { return 0; }

o_str*  c_obj_ui::get_comment()           const { return nullptr; }
INT32   c_obj_ui::get_file_version_save() const { return 0; }

//  --------------------------------------------------------------------
//  Non-virtual / const-qualified helpers.
//  --------------------------------------------------------------------
void    c_obj_ui::obj_new_private( c_obj_ui* )                                            {}
bool    c_obj_ui::make_script_filename( o_str&, o_str const&, char const*, o_str* )       { return false; }

char const* c_obj_ui::get_name_str()                              const { return ""; }
c_obj_ui*   c_obj_ui::get_obj_sub_by_index( INT32 )               const { return nullptr; }

//  Variadic print methods -- discard the arguments. va_start/va_end
//  the args so the compiler does not warn about unused varargs (and
//  so a hostile call site that passes large arg lists does not leak
//  stack on architectures where va_list is non-trivial).
void c_obj_ui::print_string( char const* /*fmt*/, ... ) const
{
    std::va_list ap; va_start( ap, this ); va_end( ap );
}
void c_obj_ui::dbg_print( char const* /*fmt*/, ... ) const
{
    std::va_list ap; va_start( ap, this ); va_end( ap );
}
void c_obj_ui::err_print( char const* /*fmt*/, ... ) const
{
    std::va_list ap; va_start( ap, this ); va_end( ap );
}
void c_obj_ui::ui_print( char const* /*fmt*/, ... ) const
{
    std::va_list ap; va_start( ap, this ); va_end( ap );
}
void c_obj_ui::box_err( char const* /*fmt*/, ... ) const
{
    std::va_list ap; va_start( ap, this ); va_end( ap );
}
