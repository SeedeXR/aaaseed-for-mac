# Math — Utilities

## Two Parallel Vector Libraries

### Raw-pointer library (`v_base.h`, `v.h`)
Low-level ops on raw `REAL*` arrays.
- `v_base.h` — `cpy_v2/3/4`, `cpy_v16`, `set_v2/3/4`, `clear_v2/3/4`, stride/interleaving, N-element variants.
  The `cpy_vN` family are 2-type templates dispatching with `if constexpr( std::same_as<T, S> )` : same-type calls take a `memcpy(sizeof(T)*N)` fast path (`/O2 /Oi` recognises it as intrinsic and emits movq / movd / movups) ; same-type Debug expands inline stores instead, avoiding the real CRT call `/Od` would emit. Cross-type calls expand to `dst[i] = T( src[i] )` for per-element conversion. Any new fixed-size N-element helper called per-pixel should follow this pattern. See root CLAUDE.md for the broader "no memcpy in per-pixel paths in Debug" rule.
- `v.h` — higher-level: `dot_v2/3`, `norm_v3`, `normalized_v3`, `cross_v3`, `lerp`, 100+ functions
- Pre-decrement write idiom used throughout — correct, writes exactly N elements (see root CLAUDE.md)

### Template library (`T*.h` files) — SEPARATE STYLE
`TVector.h`, `TMatrix22/33/44.h`, `TMatrixAffine2.h`, `TQuaternion.h`, `TBox.h`, `TRect.h`,
`TArea.h`, `TRay.h` — struct-convention, different naming, separate from the rest of the codebase.
**Do not confuse with the raw-pointer library.** `VECTRAIT<T>` provides type traits.

## Matrix Math (`aaa_matrix.h`, `aaa_matrix.cpp`)

Wraps GLM (OpenGL Mathematics).
- Functions: `ortho()`, `frustum()`, `lookat_safe()`, `translate()`, `rotate()`, `scale()`
- Rotation order enum: `ORDER_XYZ`, `ORDER_YZX`, `ORDER_ZXY`, `ORDER_XZY`, `ORDER_YXZ`, `ORDER_ZYX`
- Optional AVX2 dispatch: `AAA_USE_AVX2()` compile flag
- `matrix.h` — also provides low-level 4×4 ops on raw float arrays (separate from GLM wrapper)

## Quaternions

Two implementations exist (legacy/alternate):
- `c_quaternion` (`quaternions.h`) — axis-angle, `create_from_axis_angle()`, `create_matrix()`
- `c_quaternions` — `set_axis_angle()`, `normalize()`, `dot()`, `get_conjugate()`, `rotate_vector()`, `matrix()`

## `AAA_REAL_IS_DOUBLE()` Dispatch

Compile-time macro switches `REAL` between `float` and `double`.
All math that operates on `REAL*` arrays branches on this macro.

## XNA / SIMD Paths

`AAA_USE_XNA()` is **enabled** (`v.h` defines it to `1`). Most vector functions runtime-dispatch
to `*_xna` variants (in `v_xna.h/cpp`) via `c_vector_lib::s_vector_lib` checks.
A few specific functions have their XNA path disabled with `#if 0` where the XNA
implementation doesn't exist yet — these are noted individually, not system-wide.

## 1D Functions (`fn1d/`)

Class hierarchy all rooted in `c_fn1d` (inherits `c_obj`):
- `c_fn1d_periodic` — periodic base
- `c_fn1d_periodic_table` — table-based periodic
- `c_fn1d_periodic_table_rand` — random table
- `c_fn1d_fm` / `c_fn1d_fm_add` (`fn1d_fm.h/cpp`) — FM (frequency modulation) additive
- `c_fn1d_phase_pertub` (`fn1d_pp.h/cpp`) — phase perturbation

Used by `c_bdd_mesh` for mesh perturbation/regularisation.

## Noise & Turbulence

- `noisdist.h/cpp` — `c_fn1d_periodic_turb_dist`: periodic turbulence+distance noise
- `noisturb.h/cpp` — turbulence generation

## Random Numbers (`rand.h`, `rand.cpp`)

Hierarchy rooted in `c_rand` (abstract):
- `c_rand_lin_portable` — portable linear congruential
- `c_rand_lin` — platform-optimised linear
- `c_rand_license` — licensed algorithm
- `c_rand_gauss` — Gaussian distribution
- `c_rand_gauss_slick` — fast Gaussian approximation
- `c_rand_exp` — exponential distribution
- `c_rand_min`, `c_rand_max`, `c_rand_flip` — composite selectors

`aaa_rand.h/cpp` — global random number access.

## Math Utilities (`aaa_math.h`, `aaa_math.cpp`)

`c_math` — lookup tables for sin, tan, 1/x functions.
- `SQRT()`, `POW()`, `ATAN()`, `ATAN2()`, `ATAN2_TURN()`
- `POW2()`, `POW2_EQUAL_OR_SUP()`, `INT_IS_POW2()`
- Constants: `PI`, `PI_TIME_2`, `PI_BY_2`, `DEG_OVER_1`

## Specialised Files

- `TMath.h` — template sin/cos/exp with float/double specialisations
- `billboard.h/cpp` — billboard/sprite orientation math
- `gainbias.h` — gain and bias modulation
- `hilbert.h` — Hilbert space-filling curve
- `spots.h/cpp` — spot/lighting computation
- `curves.h/cpp` — `c_bernstein_polynomial`, `c_curve_3d`
- `voronoi_diagram_generator.h/cpp` — Voronoi diagram (Fortune's algorithm); `c_struct_server<T>` for dynamic allocation

## Lua Binding

`math_lua.h/cpp` — exposes math module to Lua.
