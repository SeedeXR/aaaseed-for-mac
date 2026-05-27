// aaalua_file_virtual_stub.cpp
//
// Continuation 65 -- stubs for the 4 c_file_virtual statics referenced
// from aaa_str.cpp::find_vfile_data + aaa_file.cpp::FOPEN/FCLOSE. The
// real c_file_virtual.cpp is 214 LOC but pulls
// infrastructure/layer/layers.h + obj_ui/bdd/util/bdd.h + ... -- the
// full c_layer / c_bdd subsystem cascade.
//
// Stub strategy : b_active = false makes is_active() return false ;
// the c_file::FOPEN code path then takes the "no vfile, fall back to
// real fopen" branch. get_data returns a reference to a static-zero
// st_vfile which is never actually read (find_vfile_data only consults
// it under b_active=true).
//
// Lives in its own TU to avoid the namespace/class mem name conflict
// that test_engine_stubs_no_ostr.cpp's namespace-mem wrapper would
// trigger if c_file_virtual.h were pulled into the same TU.

#include "file/file_virtual.h"

bool   c_file_virtual::b_active  = false;
bool   c_file_virtual::b_reading = false;
FILE*  c_file_virtual::file_cur  = nullptr;
o_str  c_file_virtual::dirname{};   //  default-constructed empty o_str

namespace
{
    //  Default-zero st_vfile : get_data hands this out for every key.
    c_file_virtual::st_vfile g_empty_vfile{ nullptr, 0 };
}

c_file_virtual::st_vfile const& c_file_virtual::get_data( C_PCHAR_C /*key*/ )
{
    return g_empty_vfile;
}
