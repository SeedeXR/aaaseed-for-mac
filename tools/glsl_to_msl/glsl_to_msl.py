#!/usr/bin/env python3
# tools/glsl_to_msl/glsl_to_msl.py
#
# Mechanical GLSL -> MSL substitution helper for the AAASeed Path A
# shader port. Doctrine : pure Apple Metal, no SPIR-V / Vulkan IR (see
# memory/feedback_nvidia_to_metal.md and philosophy.md Part 2.86).
#
# What this tool DOES :
#   - Recognises a narrow vocabulary of "simple" fragment shaders (texture
#     sample + uniform vec4 / float arithmetic + per-pixel fragment
#     output via gl_FragColor or gl_FragData).
#   - Emits a complete .metal file with #include <metal_stdlib>, the
#     VertexOut struct, a full-screen-triangle vs_main pass-through, and
#     an fs_main with the substituted body.
#   - Detects unsupported constructs (fixed-function GL : gl_Vertex,
#     gl_ModelViewMatrix, gl_NormalMatrix, ftransform, gl_Color ;
#     C++-preprocessor macros : AAA_DEFINE_*, flight(), blend() ; modern
#     GLSL in/out varyings) and emits `// TODO_PORT:` markers in the
#     output WITHOUT silently dropping the source.
#
# What this tool does NOT do :
#   - Translate vertex shaders. Vertex stages are still hand-ported.
#   - Resolve the C++ macro injection that GLSL_shader.cpp does at
#     register time on Windows. Those shaders need Group C handling.
#   - Touch SPIR-V or any Vulkan tooling.
#   - Wire the runtime uniform-upload path. Each emitted .metal has its
#     constant-struct binding ; the engine asset pipeline still has to
#     match the layout. This is a porting helper, not a runtime layer.
#
# Usage :
#   python3 glsl_to_msl.py <input.frag> <output.metal>
#   python3 glsl_to_msl.py --report <input.frag>    # diagnose only
#
# Exit codes :
#   0 = clean port (no TODO_PORT markers)
#   1 = port emitted but contains TODO_PORT markers (manual review needed)
#   2 = hard refusal : shader uses constructs the tool can't safely handle

import argparse
import re
import sys
from pathlib import Path


# ----------------------------------------------------------------------
# Regex tables -- ordered ; first match wins per category.
# ----------------------------------------------------------------------

# Constructs the tool flatly refuses to translate. Hit any of these and
# we emit a hard refusal (exit 2) so the user knows to hand-port.
HARD_REFUSE_PATTERNS = [
    (r"\bAAA_DEFINE_[A-Z_]+\s*\(",
        "C++ preprocessor macro injection (Group C). Not in this tool's scope."),
    (r"\bflight\s*\(",
        "Engine-injected `flight(...)` macro. Group C territory."),
    (r"\bblend\s*\(",
        "Engine-injected `blend(...)` macro. Group C territory."),
    (r"\bST_AAA_BV\b",
        "Engine BV (banana / vertex) struct. Needs the engine prelude."),
]

# Constructs that get translated to a `// TODO_PORT:` marker -- we emit
# the line unchanged so a human can finish the port, but we never claim
# the file is clean.
TODO_PORT_PATTERNS = [
    (r"\bgl_Vertex\b",          "GLSL fixed-function gl_Vertex. Add a vertex attribute."),
    (r"\bgl_Normal\b",          "GLSL fixed-function gl_Normal. Add a vertex attribute."),
    (r"\bgl_Color\b",           "GLSL fixed-function gl_Color. Add a vertex attribute."),
    (r"\bgl_ModelViewMatrix\b", "GLSL FF matrix. Use a uniform mvp matrix."),
    (r"\bgl_ProjectionMatrix\b","GLSL FF projection. Use a uniform proj matrix."),
    (r"\bgl_NormalMatrix\b",    "GLSL FF normal matrix. Derive from inverse(transpose(mvp))."),
    (r"\bftransform\s*\(\s*\)", "GLSL ftransform. Use `uniforms.mvp * gl_Vertex` explicitly."),
    (r"\bgl_Position\b",        "GLSL FF gl_Position. Set VertexOut.position in vs_main."),
    #	NOTE : entries previously here for `gray()` and `compute_gray()`
    #	moved to the inline prelude in `src/shaders/msl/aaa_prelude.mslh`
    #	(continuation 32). Tool now prepends the prelude on every emit,
    #	so calls to those helpers resolve at MSL compile time.
]

# Pure mechanical substitutions inside the function body. Order matters
# (longest match first to avoid eating prefixes).
BODY_SUBSTITUTIONS = [
    # Type names
    (r"\bvec2\b",   "float2"),
    (r"\bvec3\b",   "float3"),
    (r"\bvec4\b",   "float4"),
    (r"\bivec2\b",  "int2"),
    (r"\bivec3\b",  "int3"),
    (r"\bivec4\b",  "int4"),
    (r"\buvec2\b",  "uint2"),
    (r"\buvec3\b",  "uint3"),
    (r"\buvec4\b",  "uint4"),
    (r"\bmat2\b",   "float2x2"),
    (r"\bmat3\b",   "float3x3"),
    (r"\bmat4\b",   "float4x4"),

    # GLSL -> MSL builtin renames
    (r"\bdFdx\b",   "dfdx"),
    (r"\bdFdy\b",   "dfdy"),
    (r"\bfwidth\b", "fwidth"),    # same name -- keep for clarity
    (r"\bmod\b",    "fmod"),
    # `discard;` and `discard ;` in GLSL fragment shaders -> MSL has
    # `discard_fragment();`. Whole-word match avoids touching tokens
    # like `discard_low` if any. Continuation 34.
    (r"\bdiscard\b(?=\s*;)",  "discard_fragment()"),

    # Fragment output : MSL has no `gl_FragColor` -- the fragment
    # function returns a float4. Tool emits a local `_aaa_fragout`
    # float4 in fs_main and returns it. Any `gl_FragColor`,
    # `gl_FragColor.xyz =`, `gl_FragColor.a +=`, etc. all map to the
    # local. Whole-word substitution (no `\s*=` suffix) covers swizzle
    # assignments too. Continuation 33 fix surfaced by ps_Maa_MixTwo.
    (r"\bgl_FragColor\b", "_aaa_fragout"),
]


# ----------------------------------------------------------------------
# Source preprocessing
# ----------------------------------------------------------------------

def strip_comments( source: str ) -> str:
    """
    Removes C-style block (`/* ... */`) and line (`// ... \\n`) comments.
    Tool's helper / use-site regexes would otherwise fire on commented-out
    code (e.g. ps_Maa_MixTwo.frag has a fully commented `doit()` helper
    that previously poisoned the tool's classification -- continuation
    33 fix).
    Preserves the line count for diagnostic reporting -- block comments
    are replaced with their newline content so line numbers in the
    `todos` / `hard_refuse` reports stay correct.
    """
    out = []
    i = 0
    n = len( source )
    while i < n:
        ch = source[ i ]
        if ch == "/" and i + 1 < n:
            nxt = source[ i + 1 ]
            if nxt == "/":
                #	Line comment : eat until newline (keep the newline).
                j = source.find( "\n", i )
                if j == -1:
                    break
                i = j
                continue
            if nxt == "*":
                #	Block comment : eat until "*/", preserving newlines so
                #	subsequent line numbers stay aligned.
                j = source.find( "*/", i + 2 )
                if j == -1:
                    #	Unterminated block comment ; bail.
                    break
                #	Keep the newlines that were inside the block.
                inner = source[ i + 2 : j ]
                out.append( "\n" * inner.count( "\n" ) )
                i = j + 2
                continue
        out.append( ch )
        i += 1
    return "".join( out )


# ----------------------------------------------------------------------
# Analysis
# ----------------------------------------------------------------------

def analyse( source: str ) -> dict:
    """
    Returns a dict with :
      hard_refuse : list of (line_no, line, reason) -- if non-empty, abort
      todos       : list of (line_no, line, reason) -- emit as // TODO_PORT
      tex_slots   : list of (slot_index, glsl_name)   -- detected samplers
      uses_aaa_fu_float : bool
      uses_aaa_fu_vec4  : bool
      uses_gl_TexCoord  : bool
      uses_gl_FragData  : list of int     -- indices found
      uses_gl_FragColor : bool
    """
    info = {
        "hard_refuse"        : [],
        "todos"              : [],
        "tex_slots"          : [],
        "uses_aaa_fu_float"  : False,
        "uses_aaa_fu_vec4"   : False,
        "uses_aaa_fu_int"    : False,
        "uses_gl_TexCoord"   : False,
        "uses_gl_FragData"   : [],
        "uses_gl_FragColor"  : False,
    }

    #	NOTE : file-scope helper detection / TODO_PORT removed in
    #	continuation 33. Helpers are now handled by the helper-mutation
    #	pass in emit_msl : their signatures get extra binding parameters
    #	and call sites get the matching arguments. See _mutate_helper
    #	and _rewrite_call_sites below.

    for lineno, line in enumerate( source.splitlines(), start = 1 ):
        for pattern, reason in HARD_REFUSE_PATTERNS:
            if re.search( pattern, line ):
                info[ "hard_refuse" ].append( ( lineno, line.rstrip(), reason ) )

        for pattern, reason in TODO_PORT_PATTERNS:
            if re.search( pattern, line ):
                info[ "todos" ].append( ( lineno, line.rstrip(), reason ) )

        # Sampler arrays of the form  `uniform sampler2D NAME[N]`
        m = re.search( r"\buniform\s+sampler2D\s+(\w+)\s*\[\s*(\d+)\s*\]", line )
        if m:
            name = m.group( 1 )
            count = int( m.group( 2 ) )
            for i in range( count ):
                info[ "tex_slots" ].append( ( i, f"{name}[{i}]" ) )

        # Singleton samplers `uniform sampler2D NAME;`
        m = re.search( r"\buniform\s+sampler2D\s+(\w+)\s*;", line )
        if m and "[" not in m.group( 0 ):
            info[ "tex_slots" ].append( ( len( info[ "tex_slots" ] ), m.group( 1 ) ) )

        if "aaa_fu_float[" in line:
            info[ "uses_aaa_fu_float" ] = True
        if "aaa_fu_vec4[" in line:
            info[ "uses_aaa_fu_vec4" ] = True
        if "aaa_fu_int[" in line:
            info[ "uses_aaa_fu_int" ] = True
        #	Non-`aaa_`-prefixed variants used by older engine shaders.
        #	The Windows engine GLSL_shader.cpp injection aliases these
        #	to the same per-frame uniform array, so on Mac we route them
        #	through the same MSL struct binding. Set the same flag so
        #	the struct + fs_main param emit. Body substitution below
        #	rewrites the non-prefixed access to the canonical form.
        #	Continuation 35.
        if re.search( r"\bfu_float\s*\[", line ) and "aaa_fu_float[" not in line:
            info[ "uses_aaa_fu_float" ] = True
        if re.search( r"\bfu_vec4\s*\[", line )  and "aaa_fu_vec4[" not in line:
            info[ "uses_aaa_fu_vec4" ] = True
        if re.search( r"\bfu_int\s*\[", line )   and "aaa_fu_int[" not in line:
            info[ "uses_aaa_fu_int" ] = True
        if "gl_TexCoord[" in line:
            info[ "uses_gl_TexCoord" ] = True
        for m in re.finditer( r"gl_FragData\s*\[\s*(\d+)\s*\]", line ):
            idx = int( m.group( 1 ) )
            if idx not in info[ "uses_gl_FragData" ]:
                info[ "uses_gl_FragData" ].append( idx )
        if "gl_FragColor" in line:
            info[ "uses_gl_FragColor" ] = True

    info[ "uses_gl_FragData" ].sort()
    return info


def report( source_path: Path, info: dict ) -> None:
    """Pretty-prints what the tool found, regardless of exit path."""
    print( f"\n# glsl_to_msl report for {source_path}" )
    if info[ "hard_refuse" ]:
        print( "## HARD REFUSE -- cannot translate :" )
        for ln, txt, reason in info[ "hard_refuse" ]:
            print( f"  {source_path}:{ln} : {reason}" )
            print( f"      | {txt}" )
    if info[ "todos" ]:
        print( "## TODO_PORT -- manual review needed :" )
        for ln, txt, reason in info[ "todos" ]:
            print( f"  {source_path}:{ln} : {reason}" )
            print( f"      | {txt}" )
    if info[ "tex_slots" ]:
        print( "## Textures detected :" )
        for slot, name in info[ "tex_slots" ]:
            print( f"  slot {slot} : {name}" )
    if info[ "uses_aaa_fu_float" ]:
        print( "## uses aaa_fu_float[] -- emit AaaFuFloats constant struct binding" )
    if info[ "uses_aaa_fu_vec4" ]:
        print( "## uses aaa_fu_vec4[] -- emit AaaFuVec4s constant struct binding" )
    if info[ "uses_aaa_fu_int" ]:
        print( "## uses aaa_fu_int[] -- emit AaaFuInts constant struct binding" )
    if info[ "uses_gl_FragData" ]:
        print( f"## uses gl_FragData[{info['uses_gl_FragData']}] -- MRT output" )
    print()


# ----------------------------------------------------------------------
# Body translation
# ----------------------------------------------------------------------

def translate_body( body: str, info: dict ) -> str:
    """Apply mechanical substitutions + emit TODO_PORT markers in-line."""
    out_lines = []
    for line in body.splitlines():
        # Inject TODO_PORT comment above any line that hits a known
        # fixed-function pattern. We do NOT remove the GLSL : human
        # decides what to do.
        for pattern, reason in TODO_PORT_PATTERNS:
            if re.search( pattern, line ):
                out_lines.append( f"    // TODO_PORT: {reason}" )
                break

        translated = line
        for pattern, replacement in BODY_SUBSTITUTIONS:
            translated = re.sub( pattern, replacement, translated )

        # GLSL has .s/.t/.p/.q swizzles (texture-coord style) AND .r/.g/.b/.a
        # (color style) on top of .x/.y/.z/.w. MSL accepts .x/.y/.z/.w and
        # .r/.g/.b/.a but NOT .s/.t/.p/.q. Translate them to .x/.y/.z/.w.
        # The substitution is positional inside swizzle clusters : `.stp`
        # becomes `.xyz`, `.t` becomes `.y` etc.
        def _swizzle_st_to_xy( match ):
            cluster = match.group( 1 )
            mapped  = cluster.translate( str.maketrans( "stpq", "xyzw" ) )
            return "." + mapped
        translated = re.sub( r"\.([stpq]+)\b", _swizzle_st_to_xy, translated )

        # GLSL `texture2D(t, uv)` -> MSL `t.sample(_aaa_samp, uv)`.
        # The replacement requires a sampler in scope -- vs/fs prelude
        # binds one at slot 0 named `_aaa_samp`.
        translated = re.sub(
            r"texture2D\s*\(\s*([^,]+)\s*,\s*([^)]+?)\s*\)",
            r"\1.sample(_aaa_samp, \2)",
            translated,
        )
        # Modern GLSL `texture(t, uv)` (no `2D` suffix) -- same MSL form.
        # Word-boundary anchor avoids matching `texture2D` again or any
        # `*texture` partial. Continuation 33.
        translated = re.sub(
            r"\btexture\s*\(\s*([^,]+)\s*,\s*([^)]+?)\s*\)",
            r"\1.sample(_aaa_samp, \2)",
            translated,
        )
        # `textureSize(t, lod)` -> `int2(t.get_width(lod), t.get_height(lod))`.
        # The lod arg is preserved verbatim.
        translated = re.sub(
            r"textureSize\s*\(\s*([^,]+)\s*,\s*([^)]+?)\s*\)",
            r"int2(\1.get_width(\2), \1.get_height(\2))",
            translated,
        )

        # 2-arg `atan(y, x)` -> `atan2(y, x)` (MSL has both names, only
        # 2-arg form differs from GLSL). Single-arg `atan(x)` stays as
        # `atan(x)` -- MSL accepts that. Continuation 34, surfaced by
        # ps_Maa_by_n. The `(?:[^(),]|\([^)]*\))+` arg form allows one
        # level of nested parens inside each arg (e.g. `atan(a-b, c*d)`).
        translated = re.sub(
            r"\batan\s*\(\s*((?:[^(),]|\([^)]*\))+?)\s*,\s*((?:[^(),]|\([^)]*\))+?)\s*\)",
            r"atan2(\1, \2)",
            translated,
        )

        # Sampler-array reference `NAME[0]` -> `_aaa_tex_0`. Engine
        # convention : flatten to one binding per slot, name suffixed
        # with the index. Matches the aaa_tex2d[0] use sites in the
        # shaders we've ported by hand.
        for slot, name in info[ "tex_slots" ]:
            # Only fires if the original glsl name had array syntax.
            if "[" in name:
                base = name.split( "[" )[ 0 ]
                idx  = name.split( "[" )[ 1 ].rstrip( "]" )
                pattern = rf"\b{re.escape(base)}\s*\[\s*{re.escape(idx)}\s*\]"
                translated = re.sub( pattern, f"_aaa_tex_{idx}", translated )

        # gl_TexCoord[N].SWIZZLE -> in.uv.SWIZZLE. We translate the
        # base reference and let the swizzle ride through. Continuation
        # 33 : tightened from the previous "swallow the swizzle"
        # version which broke single-component swizzles like
        # `gl_TexCoord[0].x` (didn't match) and lost the .x in the
        # translation.
        translated = re.sub(
            r"gl_TexCoord\s*\[\s*\d+\s*\]",
            "in.uv",
            translated,
        )
        # `in.uv.st` -> `in.uv.xy` (sampler-style swizzles already mapped
        # generically by the .stpq pass above, but the leading `s` after
        # `in.uv.` collides with the rule -- explicit cleanup for the
        # most common pattern.). Already handled by the generic .stpq
        # → .xyzw pass earlier.

        # aaa_fu_float[N] -> _aaa_floats.values[N] (constant struct).
        translated = re.sub(
            r"aaa_fu_float\s*\[\s*(\d+)\s*\]",
            r"_aaa_floats.values[\1]",
            translated,
        )
        # aaa_fu_vec4[N] -> _aaa_vec4s.values[N].
        translated = re.sub(
            r"aaa_fu_vec4\s*\[\s*(\d+)\s*\]",
            r"_aaa_vec4s.values[\1]",
            translated,
        )
        # aaa_fu_int[N] -> _aaa_ints.values[N].
        translated = re.sub(
            r"aaa_fu_int\s*\[\s*(\d+)\s*\]",
            r"_aaa_ints.values[\1]",
            translated,
        )
        #	Non-prefixed `fu_float[N]` / `fu_vec4[N]` / `fu_int[N]` ->
        #	same canonical `_aaa_*` form. The substitution must NOT
        #	collide with already-translated `_aaa_floats.values[N]`
        #	tokens -- we anchor on word boundaries so `_aaa_floats` is
        #	safe. Continuation 35.
        translated = re.sub(
            r"\bfu_float\s*\[\s*(\d+)\s*\]",
            r"_aaa_floats.values[\1]",
            translated,
        )
        translated = re.sub(
            r"\bfu_vec4\s*\[\s*(\d+)\s*\]",
            r"_aaa_vec4s.values[\1]",
            translated,
        )
        translated = re.sub(
            r"\bfu_int\s*\[\s*(\d+)\s*\]",
            r"_aaa_ints.values[\1]",
            translated,
        )

        out_lines.append( translated )

    return "\n".join( out_lines )


def extract_main_body( source: str ) -> str:
    """
    Extracts the body of GLSL `main()` -- the contents between its
    opening `{` and matching closing `}`. Returns "" if main not found.
    """
    m = re.search( r"\bvoid\s+main\s*\(\s*(?:void)?\s*\)\s*\{", source )
    if not m:
        return ""
    start = m.end()
    depth = 1
    i = start
    while i < len( source ) and depth > 0:
        ch = source[ i ]
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
        i += 1
    if depth != 0:
        return ""
    return source[ start : i - 1 ]


def extract_helpers_list( source: str ):
    """
    Extracts file-scope helper function definitions (not `main`) as a
    LIST of source-text blocks. Continuation 33 split out from the
    pre-existing `extract_helpers` so emit_msl can iterate per-helper
    for the mutation pass.
    """
    out_blocks = []
    pos = 0
    while True:
        m = re.search(
            r"^\s*(?!void\s+main\b)(?:float|int|uint|bool|vec[234]|ivec[234]|uvec[234]|mat[234])\s+(\w+)\s*\(([^)]*)\)\s*\{",
            source[ pos: ],
            flags = re.MULTILINE,
        )
        if not m:
            break
        abs_start = pos + m.start()
        body_open = pos + m.end()
        depth = 1
        i = body_open
        while i < len( source ) and depth > 0:
            ch = source[ i ]
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
            i += 1
        if depth != 0:
            break
        out_blocks.append( source[ abs_start : i ] )
        pos = i
    return out_blocks


def extract_helpers( source: str ) -> str:
    """Back-compat wrapper -- returns helpers as one concatenated string."""
    return "\n\n".join( extract_helpers_list( source ) )


# ----------------------------------------------------------------------
# Helper-mutation pass
# ----------------------------------------------------------------------
#
# GLSL allows file-scope helper functions to reference uniforms /
# samplers / varyings directly because those are global in GLSL. MSL has
# no globals at that scope -- bindings live as parameters of the
# fragment / vertex function. So every helper that uses a binding needs
# to receive it as an explicit parameter, and every call site must
# pass it through. This pass mutates the translated helper bodies +
# their call sites accordingly.
#
# Continuation 33 implementation. Tested against ps_Maa_diff,
# ps_Maa_MixTwo, ps_Maa_threshold (helper-free path), and the existing
# catalog members which had no helpers.

#	Token name in the translated MSL -> (helper-param syntax, call-site
#	arg syntax-from-main, call-site arg syntax-from-helper).
#	Order in this list = canonical parameter order appended to helper
#	signatures. Keep stable across emits so two ports of the same
#	shader produce identical output.
_HELPER_BINDING_TABLE = [
    #	tex_N : populated per-slot at scan time, not here
    ( "_aaa_samp",   "sampler _aaa_samp",                  "_aaa_samp",  "_aaa_samp"  ),
    ( "in.uv",       "float2 _aaa_uv",                     "in.uv",      "_aaa_uv"    ),
    ( "_aaa_floats", "constant AaaFuFloats& _aaa_floats",  "_aaa_floats","_aaa_floats"),
    ( "_aaa_vec4s",  "constant AaaFuVec4s&  _aaa_vec4s",   "_aaa_vec4s", "_aaa_vec4s" ),
    ( "_aaa_ints",   "constant AaaFuInts&   _aaa_ints",    "_aaa_ints",  "_aaa_ints"  ),
]


def _scan_helper_needs( translated_body: str, info: dict ):
    """
    Inspect the body string AFTER body-substitution and return :
      tex_slots_used   : sorted list of int slots (flattened-array refs
                         from `aaa_tex2d[N]` -> `_aaa_tex_N`)
      singleton_texs   : list of (name, slot) for non-array textures
                         like `tex0`/`tex1` -- the body uses the bare
                         GLSL name and fs_main has it bound under that
                         same name. Continuation 35.
      bindings_used    : list of (token, helper_param, arg_from_main,
                         arg_from_helper) in canonical order.
    """
    tex_slots = sorted( {
        int( m.group( 1 ) )
        for m in re.finditer( r"\b_aaa_tex_(\d+)\b", translated_body )
    } )
    #	Singleton textures : info["tex_slots"] holds (slot, glsl_name).
    #	For slots whose name is NOT array-style ("name[N]") the helper
    #	may reference the bare name (e.g. `tex0`). We add it to the
    #	helper's params under the same name.
    singleton_texs = []
    for slot, name in info.get( "tex_slots", [] ):
        if "[" in name:
            continue
        if re.search( r"\b" + re.escape( name ) + r"\b", translated_body ):
            singleton_texs.append( ( name, slot ) )

    bindings = []
    for tok, hp, am, ah in _HELPER_BINDING_TABLE:
        if re.search( re.escape( tok ).replace( r"\.", r"\." ), translated_body ):
            bindings.append( ( tok, hp, am, ah ) )
    return tex_slots, singleton_texs, bindings


def _mutate_helper( helper_src: str, info: dict ):
    """
    Given the FULL `RETURN_TYPE NAME(orig_params) { body }` source of a
    helper (already body-substituted), return :
      (name, mutated_src, tex_slots_used, bindings_used)
    Inserts the needed bindings as extra parameters after the helper's
    own params, and rewrites `in.uv` -> `_aaa_uv` inside the body.
    """
    sig_match = re.match(
        r"(?P<lead>\s*)(?P<ret>\w+)\s+(?P<name>\w+)\s*\((?P<params>[^)]*)\)\s*\{",
        helper_src,
    )
    if not sig_match:
        return None, helper_src, [], []

    name        = sig_match.group( "name" )
    ret_type    = sig_match.group( "ret"  )
    orig_params = sig_match.group( "params" ).strip()
    body_start  = sig_match.end()

    #	Walk the matching closing brace.
    depth = 1
    i = body_start
    while i < len( helper_src ) and depth > 0:
        c = helper_src[ i ]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
        i += 1
    body = helper_src[ body_start : i - 1 ]
    tail = helper_src[ i - 1 : ]   # the closing '}' + anything after

    #	What bindings does the body need ?
    tex_slots, singleton_texs, bindings = _scan_helper_needs( body, info )

    if not tex_slots and not singleton_texs and not bindings:
        #	Helper uses no bindings -- leave it alone.
        return name, helper_src, [], [], []

    #	Build the extra-params list. Array textures first (canonical
    #	order), then singleton textures (in slot order), then bindings.
    extra = []
    for slot in tex_slots:
        extra.append( f"texture2d< float > _aaa_tex_{slot}" )
    for tex_name, _slot in singleton_texs:
        extra.append( f"texture2d< float > {tex_name}" )
    for tok, hp, am, ah in bindings:
        extra.append( hp )

    new_params = orig_params + ( ", " + ", ".join( extra ) if orig_params else ", ".join( extra ) )

    #	Inside the helper body, `in.uv` (which makes sense in main) must
    #	become the local param name `_aaa_uv`.
    if any( tok == "in.uv" for tok, _, _, _ in bindings ):
        body = re.sub( r"\bin\.uv\b", "_aaa_uv", body )

    new_src = (
        sig_match.group( "lead" )
        + ret_type + " " + name + "( " + new_params + " )\n{"
        + body
        + tail
    )
    return name, new_src, tex_slots, singleton_texs, bindings


def _rewrite_call_sites( source: str, helper_name: str,
                         tex_slots: list, singleton_texs: list,
                         bindings: list,
                         arg_position: str ) -> str:
    """
    For each call to `helper_name(...)` in `source`, append the
    binding-arg list. `arg_position` is "main" or "helper" -- selects
    which arg-name to use for `in.uv` (literal `in.uv` from main,
    `_aaa_uv` from another helper).
    Only matches calls with no nested parens (simple-args case). Nested
    cases will fall through and produce an MSL "wrong arity" error,
    which is easier to debug than a wrong substitution.
    """
    if not tex_slots and not singleton_texs and not bindings:
        return source

    extra_args = [ f"_aaa_tex_{s}" for s in tex_slots ]
    for tex_name, _slot in singleton_texs:
        extra_args.append( tex_name )
    for tok, hp, am, ah in bindings:
        extra_args.append( am if arg_position == "main" else ah )
    extras_str = ", ".join( extra_args )

    pattern = re.compile( r"\b" + re.escape( helper_name ) + r"\s*\(([^()]*)\)" )
    def _repl( m ):
        args = m.group( 1 ).strip()
        if args:
            return f"{helper_name}( {args}, {extras_str} )"
        else:
            return f"{helper_name}( {extras_str} )"
    return pattern.sub( _repl, source )


# ----------------------------------------------------------------------
# Emit MSL
# ----------------------------------------------------------------------

PRELUDE = """// {basename}.metal -- mechanical port from {basename}.frag
// Generated by tools/glsl_to_msl/glsl_to_msl.py . Doctrine : pure Apple
// Metal, no SPIR-V / Vulkan IR (memory/feedback_nvidia_to_metal.md).
//
// Manual review required for any `// TODO_PORT:` marker below.

#include <metal_stdlib>
using namespace metal;

struct VertexOut
{{
    float4 position [[position]];
    float2 uv;
}};
"""

UNIFORM_STRUCTS = """
//	Engine convention : aaa_fu_float[N] / aaa_fu_vec4[N] / aaa_fu_int[N]
//	arrays uploaded per frame. Fixed-size structs match what the Windows
//	engine writes ; the runtime asset pipeline must produce the same
//	layout on Mac.
struct AaaFuFloats { float  values[ 16 ]; };
struct AaaFuVec4s  { float4 values[ 16 ]; };
struct AaaFuInts   { int    values[ 16 ]; };
"""

VS_MAIN = """
//	Pass-through vertex stage. Oversized triangle covers the full screen ;
//	uv runs [0,1] over the visible quadrant.
vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    float2 verts[ 3 ] = { float2( -1.0, -3.0 ),
                          float2( -1.0,  1.0 ),
                          float2(  3.0,  1.0 ) };
    float2 uvs  [ 3 ] = { float2( 0.0, 2.0 ),
                          float2( 0.0, 0.0 ),
                          float2( 2.0, 0.0 ) };
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = uvs[ vid ];
    return out;
}
"""


#	Path of the Mac-side MSL prelude (relative to this script). Tool
#	inline-prepends it to every emit so generated .metal files are
#	self-contained for MTL::Device::newLibrary -- no `#include` headers
#	to resolve at compile time.
_THIS_DIR     = Path( __file__ ).resolve().parent
_PRELUDE_PATH = _THIS_DIR / ".." / ".." / "src" / "shaders" / "msl" / "aaa_prelude.mslh"


def _load_prelude() -> str:
    if not _PRELUDE_PATH.is_file():
        #	Tool still runs without the prelude file present (rare
        #	bootstrap case) ; just emits nothing for it.
        return ""
    return _PRELUDE_PATH.read_text()


def emit_msl( source_path: Path, info: dict, body: str, helpers: str ) -> str:
    basename = source_path.stem
    chunks = [ PRELUDE.format( basename = basename ) ]

    #	Inline-prepend the engine MSL prelude (gray() / compute_gray() /
    #	constants). Continuation 32 -- replaces the TODO_PORT markers
    #	that previously fired on calls to these helpers.
    prelude_src = _load_prelude()
    if prelude_src:
        chunks.append( "\n//	---- inline engine MSL prelude (from aaa_prelude.mslh) ----\n" )
        chunks.append( prelude_src )
        chunks.append( "\n" )

    if info[ "uses_aaa_fu_float" ] or info[ "uses_aaa_fu_vec4" ] or info[ "uses_aaa_fu_int" ]:
        chunks.append( UNIFORM_STRUCTS )

    #	Helper-mutation pass (continuation 33). Each helper :
    #	  1. Body is body-substituted (same rules as main : type renames,
    #	     texture sample, uniform indexing, etc.).
    #	  2. Body is scanned for binding references. If any are present,
    #	     extra parameters are appended to the helper signature and
    #	     `in.uv` is rewritten to `_aaa_uv` (the new local name).
    #	  3. Call sites in MAIN's body and in OTHER helpers are rewritten
    #	     to pass the matching arguments.
    #	The order matters : pass 2 must happen before pass 3 so we know
    #	WHAT extra args to append per call.
    helper_blocks = extract_helpers_list( info[ "source_for_emit" ] )
    helper_mutations = []   #	list of (name, mutated_src, tex_slots, singleton_texs, bindings)
    for raw_block in helper_blocks:
        translated = translate_body( raw_block, info )
        name, mutated_src, tex_slots, singleton_texs, bindings = _mutate_helper( translated, info )
        helper_mutations.append( ( name, mutated_src, tex_slots, singleton_texs, bindings ) )

    if helper_mutations:
        chunks.append( "\n// ---- helper functions (translated + mutated) ----\n" )
        for name, mutated_src, tex_slots, singleton_texs, bindings in helper_mutations:
            chunks.append( mutated_src + "\n\n" )

    chunks.append( VS_MAIN )

    # Build fs_main signature with detected bindings.
    args = [ "VertexOut in [[stage_in]]" ]
    next_tex_slot = 0
    for slot, name in info[ "tex_slots" ]:
        if "[" in name:
            args.append( f"texture2d< float > _aaa_tex_{slot} [[texture({slot})]]" )
        else:
            args.append( f"texture2d< float > {name} [[texture({slot})]]" )
        next_tex_slot = max( next_tex_slot, slot + 1 )
    if info[ "tex_slots" ]:
        args.append( "sampler _aaa_samp [[sampler(0)]]" )

    buffer_slot = 0
    if info[ "uses_aaa_fu_float" ]:
        args.append( f"constant AaaFuFloats& _aaa_floats [[buffer({buffer_slot})]]" )
        buffer_slot += 1
    if info[ "uses_aaa_fu_vec4" ]:
        args.append( f"constant AaaFuVec4s&  _aaa_vec4s  [[buffer({buffer_slot})]]" )
        buffer_slot += 1
    if info[ "uses_aaa_fu_int" ]:
        args.append( f"constant AaaFuInts&   _aaa_ints   [[buffer({buffer_slot})]]" )
        buffer_slot += 1

    #	Translate main body once and rewrite helper call sites with the
    #	extra binding args. The rewrite uses arg-from-main (literal
    #	`in.uv`, `_aaa_floats`, etc.).
    body_t_main = translate_body( body, info )
    for name, _mutated, tex_slots, singleton_texs, bindings in helper_mutations:
        if name:
            body_t_main = _rewrite_call_sites(
                body_t_main, name, tex_slots, singleton_texs, bindings,
                arg_position = "main" )

    # Return type : single float4 unless MRT (gl_FragData).
    if info[ "uses_gl_FragData" ]:
        struct_body = "\n".join(
            f"    float4 color{i} [[color({i})]];"
            for i in info[ "uses_gl_FragData" ]
        )
        chunks.append( "\nstruct FragOut\n{\n" + struct_body + "\n};\n" )
        chunks.append( "\nfragment FragOut fs_main( " + ", ".join( args ) + " )\n{\n" )
        chunks.append( "    FragOut _aaa_out;\n" )
        # Route gl_FragData[N] = X to _aaa_out.colorN = X.
        body_t = re.sub( r"gl_FragData\s*\[\s*(\d+)\s*\]\s*=",
                         r"_aaa_out.color\1 =", body_t_main )
        chunks.append( "    " + body_t.replace( "\n", "\n    " ) + "\n" )
        chunks.append( "    return _aaa_out;\n}\n" )
    else:
        chunks.append( "\nfragment float4 fs_main( " + ", ".join( args ) + " )\n{\n" )
        chunks.append( "    float4 _aaa_fragout = float4( 0.0 );\n" )
        chunks.append( "    " + body_t_main.replace( "\n", "\n    " ) + "\n" )
        chunks.append( "    return _aaa_fragout;\n}\n" )

    return "".join( chunks )


# ----------------------------------------------------------------------
# CLI
# ----------------------------------------------------------------------

def main() -> int:
    ap = argparse.ArgumentParser( description = "Mechanical GLSL -> MSL helper (AAASeed Path A)." )
    ap.add_argument( "input", type = Path, help = "Input .frag file." )
    ap.add_argument( "output", nargs = "?", type = Path, default = None,
                     help = "Output .metal file. Omit with --report to only diagnose." )
    ap.add_argument( "--report", action = "store_true", help = "Print diagnostic only, no emit." )
    args = ap.parse_args()

    if not args.input.is_file():
        print( f"error: input not a file : {args.input}", file = sys.stderr )
        return 2

    raw_source = args.input.read_text()
    #	Strip comments before any analysis -- commented-out helpers /
    #	commented-out fixed-function GL must not trigger TODO_PORT or
    #	HARD_REFUSE. Continuation 33 fix.
    source = strip_comments( raw_source )
    info = analyse( source )
    info[ "source_for_emit" ] = source
    report( args.input, info )

    if info[ "hard_refuse" ]:
        print( "Hard refusal : tool cannot safely translate this shader.", file = sys.stderr )
        return 2

    if args.report:
        return 1 if info[ "todos" ] else 0

    if args.output is None:
        print( "error: provide an output path or --report", file = sys.stderr )
        return 2

    body = extract_main_body( source )
    if body == "":
        print( "error: could not extract `main()` body", file = sys.stderr )
        return 2

    helpers = extract_helpers( source )
    msl = emit_msl( args.input, info, body, helpers )
    args.output.write_text( msl )
    print( f"Wrote {args.output} ({len(msl)} bytes)." )
    return 1 if info[ "todos" ] else 0


if __name__ == "__main__":
    sys.exit( main() )
