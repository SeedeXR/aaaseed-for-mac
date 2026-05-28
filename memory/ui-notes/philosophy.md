# Studio UI — Philosophy

## Immediate Mode = GaBuZoMeu BU System

The Studio is built on Dear ImGui — a retained-nothing, immediate-mode UI toolkit.
Every frame the entire UI is re-declared from C++ state. There is no scene graph,
no binding layer, no diffing, no reconciliation. The draw code *is* the state
description. This is the GaBuZoMeu authoring principle applied to tooling:

> **Everything is a BU (Basic Unit).** Panels are composed from primitives.
> The compositor frame drives the editor the same way it drives the scene.

## Palette Contract

All Studio colours come from the GaBuZoMeu palette constants defined at the top
of `aaa_studio_metal.mm`:

| Constant | Hex | Meaning |
|----------|-----|---------|
| `k_teal` | `#1abc9c` | Background accent, active wire |
| `k_dark_blue` | `#1a1e2e` | Window background |
| `k_light_teal` | `#a8e6cf` | Selected node / highlighted text |
| `k_red` | `#e74c3c` | Error, disabled, dangerous action |
| `k_green` | `#2ecc71` | Active, enabled, running |
| `k_yellow` | `#f1c40f` | Realtime data, latency, fps counter |
| `k_magenta` | `#9b59b6` | Selected item (secondary selection) |

Never use hard-coded colour literals outside of these constants.

## Proximity-First UX

Panels are arranged so the most-used workflows are spatially adjacent:

```
┌────────────────┬──────────────┬──────────────┐
│  Node Graph    │  Code Editor │  MEU Inspector│
│  (left 45%)   │  (centre 35%)│  (right 20%) │
├────────────────┴──────────────┴──────────────┤
│  Console (bottom left)  │  Perf (bottom right)│
└─────────────────────────────────────────────┘
```

The node you click populates the code editor and MEU inspector automatically.
No menu navigation required to go from "I see a node" to "I am editing its script."

## Everything Scriptable

Every action reachable via the Studio UI must eventually be reachable via the
MEU Lua scripting surface (v2/v3 milestones). The Studio is a convenience layer
on top of the scripting engine, not a separate authority. This ensures:

1. The CLI / headless CI path and the interactive authoring path share one truth.
2. Macros are free — record your Studio session as a Lua script.
3. The engine can be driven without a display (shader regression, automated
   golden-frame captures, etc.) using the same API surface.

## Zero Hallucination Policy

All code in this module was verified against the actual codebase:

- `GOL::MetalBackend::get_active_encoder()` was added explicitly (c148) after
  confirming that `get_native_encoder()` did not exist.
- `MTL::Device*` bridge-cast uses `reinterpret_cast<void*>` + `(__bridge ...)`;
  metal-cpp layout compatibility with ObjC `id<MTLDevice>` is documented in
  Apple's metal-cpp headers.
- No API is assumed without grep verification.
