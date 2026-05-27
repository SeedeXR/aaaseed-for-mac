# Image — Loading, Conversion, GPU Upload

## Core Classes

### `c_img_base` (`img_base.h/cpp`)
Abstract base for all image objects.
- Dimensions: `_sx`, `_sy` (and `_sz` in 3D variant)
- Data: `_data` pointer, `_b_data_owner` ownership flag
- Format: `_pixel_format`, `_pixel_type`, `_channel_nb`, `_byte_pitch`
- CPU/GPU sync: `_b_changed`, `_state_unique`
- Compressed data: `_compressed_data`, `_compressed_format`, `_compressed_size`
- CPU retention: `_b_data_keep` / `is_free_when_on_board()` — can discard CPU copy after GPU upload
- Thread safety: `_lock` (`aaa::MUTEX`); images are lockable via `std::lock_guard<c_img_2d>`
- Template static: `read<IMG>()` / `reread<IMG>()` — generic load with caching (explicitly instantiated for `c_img_2d` and `c_img_3d`)

### `c_img_2d` (`img.h/cpp`)
Full 2D image.
- Sampling: `get_valid_value_from_uv_*()`, `get_valid_color3r_from_uv_*()` — nearest/linear
- Ellipse (anisotropic) sampling: `get_valid_color*r_from_uv_ellipse()`
- Gradient: `get_valid_gradient_from_uv_*()` — partial derivatives
- Fill: `fill_compo()`, `fill_alpha()`, `fill_rgba()`, `fill_with_colorbar()`
- Transforms: `merge_channel_to_rgba()`, `alpha_inverse()`, `premultiply_alpha()`
- GPU transfer: `move_to_gpu()` / `move_from_gpu()` via `GOL::tex2d`
- Resize: `resize()` uses OpenCV LANCZOS4
- File I/O: `read_tga()`, `read_yuv()`, `read_dds()`, `read_aaatc()`, plus format-specific writers

### `c_img_3d` (`img_3d.h/cpp`)
Volumetric image. Adds `_sz` dimension. Supports VTK, PVM, DDS, raw formats.

### `c_img_master` (`img_master.h`)
Global singleton config object (`c_obj_ui`).
- Library selection: GFLSDK vs FreeImage
- `_b_compress_read/write` — auto-compress on load/save
- `_b_scale_on_load` — rescale large images (with configurable factor and path; checks timestamps to skip reprocessing)
- `_b_net_in/out_active` — network streaming
- `_b_unimplemented_verbose/break` — debug controls

## Pixel Format System (`pixel_format.h`)

- `PIXEL_TYPE` enum: `UINT_8`, `UINT_16`, `FLOAT_16`, `FLOAT_32`, depth variants
- `PIXEL_FORMAT` enum: 70+ formats — `R_8`, `RGB_8`, `RGBA_8`, `BGR_8`, YUV formats, …
- `COMPO` enum: `RED`, `GREEN`, `BLUE`, `ALPHA`, `GREY`, difference channels, …

## Conversion (`image/convert/`)

All pixel format conversion code lives in the `Src/image/convert/` subdirectory. The dispatcher (`c_img_2d::copy_from_src` and the family of `c_img_2d::copy_src_to_<dst>` methods) stays in `image/img_copy.cpp` because they are member methods of `c_img_2d`. Everything else moves under `convert/`.

Namespace `imgcon` — 60+ conversion functions. Pattern:
```cpp
void r8_to_rgb8_sse3( src, src_pitch, dst, dst_pitch, st_img_conv options )
```
SIMD variants suffixed `_sse2`, `_sse3`, `_ssse3`, `_avx2`.

### File layout

The conversion code is split by the kinds of conversion involved, not by the function name:

| File | Scope |
|------|-------|
| `img_convert_generic.cpp` | Per-component, `ch_nb`-driven widenings and narrowings : `memcpy_image`, `uint8_to_uint16`, `uint16_to_uint8`, `uint8_to_fp16`, `uint8_to_fp32`, `uint16_to_fp16`, `uint16_to_fp32`, `fp16_to_uint8`, `fp16_to_fp32`, `fp32_to_uint8`, `fp32_to_fp16`, `binary_to_r8`. Also hosts every per-pixel template family, organized as 5 master templates + thin inline wrappers in `img_convert.h` : R-source replicate `r_to_rgbx<T_SRC, T_DST, int CH_NB, bool B_FORCE_ALPHA>` (wrappers `r_to_rgb`, `r_to_rgba` ; CH_NB == 4 adds an alpha lane, opaque if !B_FORCE_ALPHA else routed from options.alpha_fp32) ; RG-source pass-through `rg_to_rgbx<T_SRC, T_DST, int CH_NB, bool B_FORCE_ALPHA>` (wrappers `rg_to_rgb`, `rg_to_rgba` ; writes R, G, 0 plus optional alpha) plus `rg_to_r<SRC, DST>` (drop G) ; RGB / RGBA-source narrowings `rgbx_to_r<T_SRC, int SRC_CH_NB, T_DST, bool B_SWAP_RB>` reduction template using `aaa::color::rgb_to_grey` (UINT8 src takes the integer fixed-point path, other T_SRC convert to FP32 and use the FP path for precision ; wrappers `rgb_to_r`, `rgba_to_r`) ; `rgbx_to_rg<T_SRC, T_DST, int SRC_CH_NB, bool B_SWAP_RB>` (drop B and optional alpha ; wrappers `rgb_to_rg`, `rgba_to_rg`) ; the full RGB-family cross conversion `rgbx_to_rgbx<T_SRC, T_DST, int SRC_CH_NB, int DST_CH_NB, bool B_SWAP_RB, bool B_FORCE_ALPHA>` covering 3->3, 4->4 with optional alpha override, 3->4 widen, 4->3 narrow (thin wrappers `rgb_to_rgb`, `rgba_to_rgba`, `rgba_to_rgb`, `rgb_to_rgba` ; `rgb_to_rgba` runtime, dispatches on options.b_force_alpha between the 2 specialisations). Runtime helper `imgcon::dispatch_rgba_to_rgba<T_SRC, T_DST>` in img_convert.h picks one of the 4 `rgba_to_rgba` specialisations on (swap, force_alpha) once. Fast paths under `if constexpr` : `T_DST == UINT8 && DST_CH_NB == 4` packs 4 bytes into one UINT32 store via `PACK_RGBA` ; `T_SRC == UINT8 && SRC_CH_NB == 4` reads one UINT32 via UNPACK_RGBA / UNPACK_BGRA ; the U8->U8 4->4 case is even a plain UINT32 copy when neither swap nor force_alpha. Explicit instantiations cascade through `INSTANCE_FOREACH_T_DST` (the precision list lives in this one helper). All template bodies share the `imgcon::call_by_line_striped` skeleton. New "channel-agnostic" widenings (e.g. a future `uint16_to_uint32`) go here. |
| `img_convert_int.cpp` | Pattern-specific conversions where both src and dst are integer (u8 or u16). Includes the family `r8_to_rgb8_*`, `rgb8_to_rgba8_*`, `rgba8_to_rgba8_*`, `rgba8_to_r8_*`, `bgra8_to_rgba8_*`, the *_to_rgb8 downscalers from u16 srcs, etc. |
| `img_convert_int_to_fp.cpp` | Pattern-specific u8 -> fp paths : `rgba8_to_rgba16fp`, `rgba8_to_rgba32fp`, `bgr8_to_rgb16fp`, `bgr8_to_rgb32fp`. |
| `img_convert_fp_to_int.cpp` | Pattern-specific fp -> u8 paths : `rgba32fp_to_rgba8`, `rgba16fp_to_rgba8`, all the `*fp_to_rgb8` downscalers, `rfp16_to_rgb8`, `rfp32_to_rgb8`. |
| `img_convert_fp_to_fp.cpp` | fp -> fp pattern conversions : `rgba32fp_to_rgba16fp`, `rgba16fp_to_rgba32fp`, `rgba16fp_to_rgba16fp`, `rgba32fp_to_rgba32fp`, `bgr16fp_to_rgb32fp`, `bgr32fp_to_rgb16fp`. |
| `img_convert_yuv.cpp` | YUV decoders : `yuyv_to_rgba8`, `i420_to_rgba8`, `nv12_to_rgba8`, `v210_to_bgra8`. Wraps the `bitcon::*_avx2` / `_line_block` workers in `bitmap_convert*.cpp`. |
| `img_convert_macros.h` | Shared inline helpers and macros : `IMGCON_BEGIN_COPY` / `IMGCON_END_COPY*` (used by every `c_img_2d::copy_src_to_<dst>` switch), `adjust_src_dst` (field / flip preprocessing), and `imgcon::call_by_line_striped( options, body )` (factors the stripe parallel skeleton used by every per-pixel template in `img_convert_generic.cpp` : body is a `(INT32 j) NOEXCEPT` lambda receiving the destination line index). **Transitively includes `infrastructure/compute_parallel.h` and `image/img_compo.h`** : files that include `img_convert_macros.h` must not also include those directly (the strict `#error "included more than once"` guards fire on the second include). |
| `bitmap_convert.h/cpp`, `bitmap_convert_yuv.cpp` | `bitcon::` workers : `bitcon::yuyv_to_rgba8_avx2`, etc. |
| `color_space.h/cpp`, `color_space_lua.h/cpp` | `aaa::color` LUT-based RGB / HSV / YUV utilities + Lua bindings. |
| `convert.h`, `a_*.asm` | Legacy VirtualDub DIB conversion stubs + unused x86 YUV assembly. |

Public headers : `image/convert/img_convert.h` (the imgcon namespace), `image/convert/bitmap_convert.h` (the bitcon class), `image/convert/color_space.h` (the aaa::color class). Callers outside `image/convert/` include them via `#include "image/convert/<file>"`. Files inside `convert/` use bare include `#include "<file>"` thanks to same-directory lookup.

### When adding a new conversion

1. Decide which file owns it based on src and dst types (see table above).
2. Add the declaration in `img_convert.h`.
3. Add the case in the matching `c_img_2d::copy_src_to_<dst>` switch in `image/img_copy.cpp`. Prefer the `DISPATCH_*_CASES` macros (see below) when the new converter follows the standard `imgcon::FN<T_SRC, T_DST[, B_SWAP_RB]>(src, src_pitch, dst, dst_pitch, options)` shape ; hand-write the case when there's a SIMD or precision-specific fast path.
4. Flip the matching cell in `info_convert[]` (`pixel_format.cpp`) from `NO` to `Y`.
5. Build all three configs (Debug, Release, Metal) via `out/cmake-build-v145/AAASeed.vcxproj`.

### Dispatcher macros (`img_copy.cpp`)

The `c_img_2d::copy_src_to_<dst>()` switches share a family of helper macros, defined at the top of `img_copy.cpp` and `#undef`'d at the bottom (intentionally not exported). They assume local names `src`, `src_pitch`, `dst`, `dst_pitch`, `options` (so each `copy_src_to_<dst>` declares its parameters with those exact names).

| Macro | Expands to |
|-------|------------|
| `DISPATCH_ARGS_4` | `src, src_pitch, dst, dst_pitch` (for callees with extra middle / trailing args like `ch_nb`, `size_y`, `b_uyvy`) |
| `DISPATCH_ARGS` | `DISPATCH_ARGS_4, options` (the standard imgcon converter 5-tuple) |
| `DISPATCH_ONE(FMT, PREFIX, FN, ...)` | One `case PIXEL_FORMAT::FMT: PREFIX imgcon::FN<__VA_ARGS__>(DISPATCH_ARGS); break;` |
| `DISPATCH_FOREACH_PRECISION_2(FN, T_DST, PREFIX, FMT_PFX)` | 4 `DISPATCH_ONE` calls for `{FMT_PFX##_8, _16, _16FP, _32FP}`, 2 template args (T_SRC, T_DST) |
| `DISPATCH_FOREACH_PRECISION_3(FN, T_DST, PREFIX, FMT_PFX, SWAP)` | Same, 3 template args (T_SRC, T_DST, SWAP) |
| `DISPATCH_R_CASES   (FN, T_DST, PREFIX)` | Cascade `R_*` cases through `FOREACH_PRECISION_2` |
| `DISPATCH_RG_CASES  (FN, T_DST, PREFIX)` | Cascade `RG_*` cases through `FOREACH_PRECISION_2` |
| `DISPATCH_RGB_CASES (FN, T_DST, PREFIX)` | Cascade `RGB_*` cases through `FOREACH_PRECISION_3` with `SWAP = false` |
| `DISPATCH_BGR_CASES (FN, T_DST, PREFIX)` | Cascade `BGR_*` cases through `FOREACH_PRECISION_3` with `SWAP = true` |
| `DISPATCH_RGBA_CASES(FN, T_DST, PREFIX)` | Cascade `RGBA_*` cases through `FOREACH_PRECISION_3` with `SWAP = false` |
| `DISPATCH_BGRA_CASES(FN, T_DST, PREFIX)` | Cascade `BGRA_*` cases through `FOREACH_PRECISION_3` with `SWAP = true` |

`PREFIX` is empty for void callees and `b_alpha_done =` for bool callees (so the alpha-done local declared by `IMGCON_BEGIN_COPY` stays visible at the call site, no `_ALPHA` suffix variant needed). Example :

```cpp
DISPATCH_R_CASES   ( r_to_rgba,   FP16, b_alpha_done = );
DISPATCH_RG_CASES  ( rg_to_rgba,  FP16, b_alpha_done = );
DISPATCH_RGB_CASES ( rgb_to_rgba, FP16, b_alpha_done = );
DISPATCH_BGR_CASES ( rgb_to_rgba, FP16, b_alpha_done = );
```

Splitting RGB/BGR and RGBA/BGRA (instead of one 8-line macro per family) exposes the swap-vs-no-swap intent at the call site instead of hiding it inside the expansion. The precision list `{8, 16, 16FP, 32FP}` lives in exactly two places (`_PRECISION_2` and `_PRECISION_3`). Adding a new precision (e.g. `FP64`) is a one-line edit per arity. Dispatchers with SIMD or precision-specific fast paths (`rgba8`, `rgb8`, `r8`, `rgba16fp`, `rgba32fp`, `rgb16fp`, `rgb32fp`, `rgb16`) keep those special cases hand-written alongside the macro-emitted uniform blocks, with a one-line comment explaining why.

### Runtime dispatch primitives

- **`c_pixel_format::is_move( pf_dst, pf_src )`** — answers "can pf_src be directly converted to pf_dst ?" when both are runtime `PIXEL_FORMAT` values. Routes through the existing `is_move_<dst>( pf_src )` getters. `BGR_8` / `BGRA_8` destinations alias to the `rgb8` / `rgba8` paths (red-blue swap applied at copy time).
- **SIMD tier gating** — SIMD variants are selected via `texture_flux_master::is_convert_to_rgb_avx2()` / `is_convert_to_rgb_sse3()` / `is_convert_to_rgb_sse2()`. These predicates already include the matching `c_cpu::one->is_use_*()` check internally ; **do not double-gate** with both.
- **Templated `<B_SWAP, B_FORCE_ALPHA>` hoist** — kernels like `imgcon::rgba16fp_to_rgba16fp` / `rgba32fp_to_rgba32fp` (in `img_convert_fp_to_fp.cpp`) template on swap and force-alpha booleans and gate every per-pixel branch with `if constexpr`, so each instantiation emits a straight-line loop with no runtime branching on the picking flags.
- **Stripe parallel skeleton** — `imgcon::call_by_line_striped( options, body )` in `img_convert_macros.h` factors the `nb_stripes = MIN(parallel::get_thread_nb_def(), sy)` + per-stripe `y_begin..y_end` loop used by every per-pixel template in `img_convert_generic.cpp`. Per-template bodies now read as : compute pre-loop state, then call `call_by_line_striped` with the per-line work as a `[&](INT32 j) NOEXCEPT { ... }` lambda. Captures src / dst / pitches / per-template state from the enclosing scope. Used by the 5 master templates `r_to_rgbx` / `rg_to_rgbx` / `rgbx_to_r` / `rgbx_to_rg` / `rgbx_to_rgbx` (and the small `r_to_rg` / `rg_to_r`) plus the SIMD widening paths `convert_per_compo_stripe` / `uint16_to_fp16` / `uint16_to_fp32`. Two companion macros in `img_convert_macros.h` build on it : `IMGCON_LINE_PTRS(T_SRC, T_DST)` declares `p_src` / `p_dst` typed line pointers at offset `j * pitch` from raw src / dst ; `IMGCON_PIXEL_LOOP(T_SRC, T_DST, NB)` adds a `for(INT32 i = NB; i > 0; --i)` loop on top, body in braces. The NB argument covers the standard `options.sx` per pixel walk, SIMD block loops, and remainder tails uniformly.
- **Per-pixel templates** in `img_convert_generic.cpp` : `convert_per_compo_stripe<SRC, DST>` (anonymous namespace) factors the scalar fallback of all ch_nb-driven widenings. The exported master template `r_to_rgbx<SRC, DST, CH_NB, B_FORCE_ALPHA>` covers the R-replicate family (one R component read, `c_compo::convert<SRC, DST>` applied, broadcast on the 3 RGB lanes ; CH_NB == 4 also writes alpha from `c_compo::convert<FP32, DST>(options.alpha_fp32 if B_FORCE_ALPHA else 1.0)`) with `r_to_rgb` / `r_to_rgba` thin inline wrappers in `img_convert.h`. `r_to_rg<SRC, DST>` does the 2-lane variant. `rg_to_rgbx<SRC, DST, CH_NB, B_FORCE_ALPHA>` is the RG-source analogue (write R, G, 0 plus optional alpha), wrapped as `rg_to_rgb` / `rg_to_rgba`. `rg_to_r<SRC, DST>` drops G. A fast path under `if constexpr (DST == UINT8 && CH_NB == 4)` in `r_to_rgbx` / `rg_to_rgbx` packs the 4 bytes into one `UINT32` store via the endian-aware `PACK_RGBA` (same technique as `bitcon::r8_to_rgba8_fast`).
- **Explicit instantiation cascades** in `img_convert_generic.cpp` use `INSTANCE_FOREACH_T_DST( FN, ... )` to cascade a per-instantiation macro over the 4 `T_DST` precisions {UINT8, UINT16, FP16, FP32}. The per-family `INSTANCE_X_ALL_DST(...)` macros are 1-liner wrappers around `INSTANCE_FOREACH_T_DST`, so the precision list lives in exactly one place. Convention : the per-instantiation macro must take `T_DST` as its LAST positional arg. An `INSTANCE_EXPAND()` wrapper works around MSVC's traditional preprocessor flattening `__VA_ARGS__` when re-expanded.

## GPU Binding (`bind_img.h/cpp`, `bind_img_*.h`)

- `bind_img_2d.h` — manages multiple 2D texture bindings
- `bind_img_3d.h` — 3D (volume) texture binding
- `bind_img_1d.h` — 1D texture binding; constant `BIND_1D_MAX_NB = 12`
- Functions: `tex_2d_bind()`, `tex_3d_bind()`, `tex_2d_bind_no_gpu_move()`
- Query: `get_bind_cur()`, `get_bind_ui()`
- Lifecycle: `bind_img_init()`, `bind_img_deinit()`

## Color Space (`image/convert/color_space.h/cpp`)

`aaa::color` — static utilities with LUT-based conversion (SHIFT=16 precision).
- RGB->grey weights (Rec.601): RED=54, GREEN=183, BLUE=19 (out of 255)
- YUV lookup tables: `tab_Y[]`, `tab_R_CR[]`, `tab_G_CB[]`, `tab_G_CR[]`, `tab_B_CB[]`
- `yuv_to_rgb_params` struct for configurable YUV standards
- Lua bindings : `image/convert/color_space_lua.h/cpp`

## File Format Loaders

| File | Format |
|------|--------|
| `img_jpg.h/cpp` | JPEG |
| `img_dds.cpp` | DirectDraw Surface (compressed textures) |
| `img_tga.cpp` | TGA |
| `img_sgi.cpp` | SGI RGB |
| `img_yuv.cpp` | Raw YUV video |
| `img_openexr.h/cpp` | OpenEXR HDR |
| `img_freeimage.h/cpp` | FreeImage (multi-format wrapper) |
| `img_xnview.h/cpp` | XnView SDK |
| `img_magic.h/cpp` | ImageMagick |

## Special Effects

- `c_remanence.h` — temporal motion blur: stores frame-difference history,
  parameters `_time`, `_b_color`, `_b_diff`, `_diff_min`, `_diff_factor`

## Networking (`img_net.h/cpp`)

Transmit image blocks over the network when `_b_net_in/out_active` is set.

## Known Patterns / Gotchas

- CPU data can be discarded after GPU upload — check `_b_data_keep` before accessing `_data`
- Locking: use `std::lock_guard<c_img_2d>` around GPU move operations
- Compressed path: pre-loaded `_compressed_data` OR on-the-fly GPU compression via `_b_compress_do`
- Scale-on-load is timestamp-gated to avoid redundant reprocessing
- Avoid `memcpy` in per-pixel inner loops in Debug (`/Od` turns it into a real CRT call ; Release `/O2 /Oi` inlines it). For tiny fixed-size copies inside conversion kernels, expand stores inline or use `cpy_vN<T,S>` from `math/v_base.h`. See root CLAUDE.md for the broader rule.

## Backlog / TODOs

(empty — the `rgbx_to_r` fp-precision item was resolved when the template was generalised to all `T_SRC` : non-UINT8 sources now route through an FP path that converts each component to FP32 and uses the FP `aaa::color::rgb_to_grey`, preserving the full fp range. UINT8 src keeps the integer fixed-point path.)
