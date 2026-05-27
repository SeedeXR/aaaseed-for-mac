# CODE STYLE

## Preamble

AAASeed is written with portability, longevity, and stability in mind.\
Please keep these goals in mind when contributing.\

AAASeed follows a strict zero-warning policy: the project must compile without any warnings, and all warnings must be resolved before committing changes.

------------------------------------------------------------------------

# Naming

-   AAASeed code is written by default using **lowercase with
    underscores (snake_case)**.
-   Uppercase is also used (see below), but unless necessary, it must
    never be mixed with lowercase. **No CamelCase.**
-   We name from general to specific:

``` cpp
INT8 capture_dshow_device_id;
void convert_img_rgb_to_rgba_fast();
```

-   Common abbreviations:

``` cpp
img  // image
tex  // texture
src  // source
dst  // destination
```

------------------------------------------------------------------------

## Prefixes

We use prefixes for certain types.

-   Class members start with a leading underscore (`_`).
-   When refactoring old code, migrate to the `_member` convention.

Example:

``` cpp
bool        b_flip_red_blue;    // b_ for bool
o_str       o_name;             // o_ for o_str (our string class)
c_capture   capture;            // c_ for class
st_info     info;               // st_ for struct

class c_example : public c_base
{
    INT8    _val8;
    bool    _b_clamp;
    o_str   _o_name;
};
```

Additional prefixes:

``` cpp
s_         // INT32 used as enum (symbolic value), e.g. s_state
p_ or pt_  // pointer
h_ or hd_  // handle
```

------------------------------------------------------------------------

## Types and Portability

For portability, we use our own types defined in:

`Src/code_utils/aaa_type.h`

You are encouraged to include this file first whenever possible.

Key types:

``` cpp
INT8   / UINT8     // 8-bit integer
INT16  / UINT16    // 16-bit integer
INT32  / UINT32    // 32-bit integer
INT64  / UINT64    // 64-bit integer

FP32               // 32-bit floating point (single precision)
DOUBLE             // 64-bit floating point (double precision)
REAL               // 32 or 64 bits depending on configuration
                   // FP32 by default (controlled by AAA_REAL_IS_DOUBLE())
```

Keyword wrappers (defined in `aaa_type.h`):

``` cpp
CONST
CONSTEXPR
FINLINE
RESTRICT
NOEXCEPT
```

------------------------------------------------------------------------

## Uppercase Conventions

Types, template parameters, enum values, defines, `const`, and
`constexpr` are written in UPPERCASE.

``` cpp
CONSTEXPR INT32 VALUE_MAX = 32;
```

------------------------------------------------------------------------

## AAA Defines

AAASeed defines:

-   Start with `AAA`
-   Use function-like macros

All warnings must be resolved before committing.

``` cpp
#if AAA_DEBUG()
    // debug case
#endif

#if AAA_ENDIAN_LITTLE()
#else
#endif
```

------------------------------------------------------------------------

## Source File Character Set (ASCII Only)

**All characters inside `.cpp` / `.h` files must be plain 7-bit ASCII.** This applies to both string literals and comments.

Why: AAASeed's debug output sink (`DBG_PRINT_STRING`, `ERR_PRINT_STRING`, `debug_break` messages, console, log file) writes bytes through to a Windows console that treats them as cp-1252. A source file saved as UTF-8 will emit multi-byte sequences for any non-ASCII glyph, and those bytes render as garbage like `ÔÇö` (the UTF-8 encoding of `—` interpreted as three cp-1252 chars). Comments are compiled away but still display wrong in editors that expect cp-1252, and they mix awkwardly with code review tools.

Common offenders to avoid and their ASCII replacements:

| Avoid | Codepoint | Use instead |
|-------|-----------|-------------|
| `—` em dash | U+2014 | `--` or ` - ` |
| `–` en dash | U+2013 | `-` |
| `’` `‘` curly apostrophes | U+2019, U+2018 | `'` |
| `“` `”` curly quotes | U+201C, U+201D | `"` |
| `…` ellipsis | U+2026 | `...` |
| `→` arrow | U+2192 | `->` |
| `•` bullet | U+2022 | `*` or `-` |
| accented letters in identifiers / comments | — | transliterate to ASCII |

`.md` documentation files are exempt -- markdown is rendered by tools that handle UTF-8 correctly and is never piped to the runtime debug output.

Rule of thumb: if you'd type it on a US keyboard without Alt codes, it's fine. Autocorrect or paste-from-Word sneaks non-ASCII punctuation in silently -- watch for it.

------------------------------------------------------------------------

## Debug Guards

Runtime assertions use `#if AAA_DEBUG()` + `debug_break()` (declared in `err.h`).
Never use `__debugbreak()` directly or hardcode numeric range limits.

``` cpp
#if AAA_DEBUG()
    if( f < T(aaa::LOWEST<INT32>) || f > T(aaa::BIGGEST<INT32>) )
        debug_break( "f out of INT32 range -- cast to INT32 is UB" );
#endif
```

`debug_break_if( condition, fmt, ... )` is the variant that takes a bool:

``` cpp
#if AAA_DEBUG()
    debug_break_if( ptr == nullptr, "ptr must not be null here" );
#endif
```

------------------------------------------------------------------------

## Type Range Constants (`aaa::LOWEST` / `aaa::BIGGEST`)

Use the project templates instead of raw `std::numeric_limits` or magic numbers.
Defined in `aaa_type.h`:

``` cpp
template<typename T> CONSTEXPR T aaa::LOWEST  = std::numeric_limits<T>::lowest();
template<typename T> CONSTEXPR T aaa::BIGGEST = std::numeric_limits<T>::max();
```

Examples:

``` cpp
aaa::BIGGEST<INT32>   // INT32_MAX
aaa::LOWEST<INT32>    // INT32_MIN  (== lowest() for integers)
aaa::BIGGEST<FP32>    // FLT_MAX
aaa::LOWEST<FP32>     // -FLT_MAX  (lowest() for floats, not FLT_MIN)
```

------------------------------------------------------------------------

# Indentation

-   Tabs are used (not spaces).
-   Braces must be vertically aligned.

Correct:

``` cpp
for( INT32 i = 0; i < OUT_BANK_NB; ++i )
{
    _channel_ui[i] = i + 1;
    _control_ui[i] = 1;
}
```

Avoid:

``` cpp
for( INT32 i = 0; i < OUT_BANK_NB; ++i ) {
    _channel_ui[i] = i + 1;
    _control_ui[i] = 1;
}
```

Single-line blocks may omit braces:

``` cpp
for( INT32 i = 0; i < OUT_BANK_NB; ++i )
    _control_ui[i] = 1;
```

Always place statements on a new line for easy breakpoint placement.

------------------------------------------------------------------------

# Includes

Include inside header files must be guarded:

``` cpp
#ifndef __AAA_TOTO_H__
#   include "toto.h"
#endif
```

AAASeed include all start with

``` cpp
#ifdef AAA_TOTO_H
#error "TOTO_H included more than once."
#endif
#define AAA_TOTO_H 1
```

The guard macro is `AAA_` + filename uppercased + `_H`. The error message is the filename uppercased + `_H` only (no extra `AAA_` prefix). Examples: `toto.h` → guard `AAA_TOTO_H`, message `"TOTO_H"`; `aaa_toto.h` → guard `AAA_AAA_TOTO_H`, message `"AAA_TOTO_H"`.

Multiple inclusion triggers an error to ensure early detection and
faster compilation.

------------------------------------------------------------------------

# Classes

-   Data members should be private (or protected).
-   Avoid public data members.
-   Use `FINLINE` accessors when needed.

``` cpp
FINLINE void set_member( INT32 CONST val )   { _member = val; }
FINLINE INT32 get_member() CONST             { return _member; }
```

------------------------------------------------------------------------

# Class Hierarchy

## c_obj --- Base Class

-   Base class for most objects.
-   Stores an ID.
-   Tracks instance creation and destruction.

Implementation:\
[Src/infrastructure/obj/obj.h](Src/infrastructure/obj/obj.h)

## c_obj_ui --- for UI Object (object with param accesible in Flatand)

-   Base class for objects with UI parameters.
-   Inherits from `c_obj`.
-   Provides plenty of mehods in particular to load/save.
-   Each subclass must define its factory (use the macros, declare it concrete or abstract). 
-   c_obj_active_ui : is a c_obj_ui with an active parameter.
-   c_bdd : the base class for bdd.

Implementation:\
[Src/infrastructure/obj/obj_ui.h](Src/infrastructure/obj/obj_ui.h)
