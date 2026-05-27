# Windows backend (v2+)

The Mac port is **complete**. The Windows backend is **Task #152** --
the reciprocal cross-port that would bring the same MEU runner +
widget system + Path A catalog UX to a Windows host without dragging
Win-side users back to the vendor engine's layer subsystem.

This page is a brief overview. The authoritative runbook is the
separate
[`docs/windows-backend-howto.md`](../windows-backend-howto.md)
(c144-C), which contains the 27-method mapping table between
`GOL::Backend` (Metal) and the Direct3D 11 implementation
Win-side will need.

---

## Status

| Item                                              | Status                                |
| ------------------------------------------------- | ------------------------------------- |
| Mac port (this repo)                              | **COMPLETE** -- v4 closed             |
| Windows host shell + D3D11 GOL::Backend           | **NOT STARTED** (Task #152)           |
| Lua API parity (`aaa.*`, `aaa.ui.*`, `aaa.io.*`)  | Source code is portable C++ + Lua 5.1 |
| Path A catalog (.metal files)                     | Needs HLSL transpile or DXBC port     |
| MEU runner sub-lib                                | Reusable as-is (hermetic, std::-only) |
| Widget system                                     | Mostly reusable (text rendering needs DWrite swap) |
| NSTextInputClient + IME                           | Replace with Win32 IMM32 / TSF        |

The Mac port is feature-complete (v4 closed per user mandate "no more
versions after v4"). Task #152 is a separate effort gated on a
Win-development machine + the engineering hours to implement the
mapping table from the howto.

---

## Why a Windows backend at all ?

The original AAASeed engine is **Win-native**. The Mac port was the
beachhead because :

1. Apple Silicon is the dominant new-machine architecture for
   creative-coding professionals.
2. The vendor engine's blocked `aaa_mem` / `c_cpu` cone meant a Mac
   port had to invent hermetic Mac sub-libs (see
   [Hermetic Mac sub-libs](memory-doctrine.md#hermetic-mac-sub-libs))
   which produced cleaner, smaller, faster code.
3. The MEU runner + widget system + Path A catalog turned out to be
   **better** than the vendor's layer subsystem for the rendering
   surface AAASeed actually needs. See
   `memory/project_layer_supersession.md`.

A Windows-side rebuild on the Mac port's foundation closes the loop --
Windows users get the new architecture without the layer subsystem's
cost.

---

## What the Win-side runbook covers

The full howto at [`docs/windows-backend-howto.md`](../windows-backend-howto.md)
documents :

- The 27-method `GOL::Backend` interface + its Win-side mapping :
  - MetalBackend -> D3D11Backend (or D3D12 ; the howto compares).
  - MTLDevice -> ID3D11Device.
  - MTLCommandQueue -> ID3D11DeviceContext.
  - MTLRenderPipelineState -> ID3D11VertexShader + ID3D11PixelShader + ID3D11InputLayout + ID3D11BlendState + ID3D11RasterizerState.
  - MTLTexture -> ID3D11Texture2D + ID3D11ShaderResourceView + ID3D11RenderTargetView.
  - MTLBuffer -> ID3D11Buffer.
  - And 20 more.
- The Path A `.metal` -> `.hlsl` transpile pipeline (recommended via
  SPIRV-Cross : Metal -> SPIR-V -> HLSL).
- The Win-native equivalent of every doctrine in
  [memory-doctrine.md](memory-doctrine.md).
- The IME swap : `NSTextInputClient` -> `ITextStoreACP` (Text Services
  Framework) for CJK ; the v4 IME composition state machine is reusable
  as-is on the widget side.

---

## When to reopen Task #152

Per the project_v4_milestone closure : Task #152 reopens when **all
three** of the following hold :

1. A Windows machine + developer hours are available.
2. The howto's mapping table is read end-to-end.
3. A user-surfaced demand exists -- the Mac port already serves the
   primary creative-coding professional audience.

Until then : the Mac DMG is the shipping artifact ; the Windows
runbook is the design spec waiting for an implementor.

---

## Cross-references

- **The full runbook** : [`docs/windows-backend-howto.md`](../windows-backend-howto.md)
- [Architecture](architecture.md)
- [MEU runner](meu-runner.md)
- [Widget system](widget-system.md)
- [Memory doctrine index](memory-doctrine.md)
- Layer supersession
- v4 milestone closure
