# Flex — NVIDIA Flex Physics (`Src/flex/`)

NVIDIA Flex particle-based physics simulation layer. Wraps the NvFlex SDK.

## Build Toggle

`AAA_USE_FLEX()` macro (defined in `flex_sdk.h`) gates all Flex code. When 0, only forward declarations for `NvFlexLibrary` and `NvFlexSolver` are provided and the entire subsystem compiles out.

## Files

| File | Responsibility |
|---|---|
| `flex_sdk.h` | Includes `NVidia/NvFlex.h`; defines `AAA_USE_FLEX()` toggle |
| `Structs.h` | All plain structs and enums: creation info, solver options, simulation/fluid/cloth/diffuse/collision params. Also defines `GOL::st_flex_solver` (UBO layout). |
| `Solver.h/.cpp` | `c_flex_solver` — core solver object. Owns all GPU buffers, drives the NvFlex tick. |
| `Particle_Host.h/.cpp` | `c_particle_host` — CPU-side particle data (position, velocity, phase). |
| `Spring_Host.h/.cpp` | `c_spring_host` — cloth/constraint spring data. |
| `Triangle_Host.h/.cpp` | `c_triangle_host` — triangle mesh data for cloth and collision. |
| `Rigid_Host.h/.cpp` | `c_rigid_host` — rigid body aggregate data. |
| `Emitter_Shapes.h/.cpp` | Shape-based particle emitter. |
| `Emitter_Cloth.h/.cpp` | Cloth particle emitter. |
| `Emitter_Image.h/.cpp` | Image-driven particle emitter. |
| `Collision_Shapes.h/.cpp` | `c_flex_collision_shapes` — collision geometry (planes, spheres, meshes). |
| `Buffer_Gl.h/.cpp` | `c_flex_buffer_gl` — GL-interop buffer (CUDA/GL shared). |
| `Buffer_Cuda_Host.h/.cpp` | `c_flex_buffer_cuda_host` — CUDA host-mapped buffer. |
| `Buffer_Cuda_Device.h/.cpp` | `c_flex_buffer_cuda_device` — CUDA device buffer. |
| `Utils.h/.cpp` | Utility helpers. |

## Key Classes

### `c_flex_solver` (inherits `c_obj`)
Central object. Created via `c_flex_solver::create()` (not constructor directly — constructor is private, takes `NvFlexLibrary*`).

- `reset()` — reinitialise solver with new creation params
- `reset_from_host()` — build solver from host data (particles, springs, triangles, rigids)
- `update()` — tick the simulation one step
- `bind_buffers_gl_draw()` / `unbind_buffers_gl_draw()` — bind GPU buffers for rendering
- `bind_buffers_gl_compute()` / `unbind_buffers_gl_compute()` — bind for compute shaders
- `dispatch_gl_compute()` — dispatch custom compute pass

### Buffers
Three buffer types reflecting the CUDA/GL interop model:
- `c_flex_buffer_gl` — primary render buffer, shared between CUDA and GL
- `c_flex_buffer_cuda_host` — readable from CPU (position readback, etc.)
- `c_flex_buffer_cuda_device` — GPU-only CUDA buffer

## Structs / Enums

All in `Structs.h`. Key enums:
- `e_flex_time_mode` — `FIXED`, `REALTIME_DIRECT`, `REALTIME_LOOPED`
- `e_flex_particle_phase_type` — `NONE`, `SOLID`, `FLUID`
- `e_flex_spring_type` — `BEND`, `SHEAR`, `STRETCH`, `TETHER`
- `e_flex_spring_length_type` — `LENGTH_FACTOR`, `LENGTH_ABSOLUTE`

`GOL::st_flex_solver` is a struct placed in the GOL namespace because it maps directly to a GPU UBO layout.

## Include Guard Convention

Files in `Src/flex/` use the **FLEX prefix exception** — do NOT change these to standard guards:
- Guard macro: `AAA_FLEX_` + filename uppercased + `_H`
- Error message: `"FLEX/` + filename uppercased + `_H included more than once."`

Example: `Solver.h` → guard `AAA_FLEX_SOLVER_H`, message `"FLEX/SOLVER_H included more than once."`

This is intentional and documented in the root `CLAUDE.md`.
