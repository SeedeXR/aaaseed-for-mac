# Studio UI — Data Flow Mindmap

## Overview

```
Studio (aaa::ui::studio::Studio)
│
├── Node Graph ──────────────────────────────────────────────
│     nodes[]          StudioNode {id, label, pos, shader_name, script_path}
│     links[]          NodeLink   {id, from_pin → to_pin}
│     add_node()       → StudioImpl::next_node_id++ (monotonic, starts at 1)
│     remove_node()    → erases node + all links referencing it
│     link_nodes()     → StudioImpl::next_link_id++ (monotonic, starts at 1)
│
├── Code Editor ─────────────────────────────────────────────
│     editor_text()    → std::string buffer
│     set_editor_text()→ replaces buffer
│     on_run_script()  → fires callback with current text (Cmd+R)
│
├── Console ─────────────────────────────────────────────────
│     log(level, text) → ConsoleEntry {INFO|WARN|ERR|LUA}
│     draw_console()   → scrollable ImGui table; colour by level
│
├── Camera ──────────────────────────────────────────────────
│     camera()         → const CameraState& {pos, look, up, fov, near, far}
│     set_camera()     → copies CameraState into impl
│
├── Performance Ring ────────────────────────────────────────
│     push_perf_sample(ms) → PerfRing[256] circular write
│     draw_perf_panel()    → ImGui::PlotLines over ring data
│
├── Preferences ─────────────────────────────────────────────
│     set_font_scale()          → ImGui::GetIO().FontGlobalScale
│     apply_gabuzoumeu_theme()  → ImGui::StyleColors patch
│
└── GPU / ImGui Lifecycle ───────────────────────────────────
      init(ns_view, mtl_device)
        ImGui::CreateContext()
        ImGui_ImplMetal_Init(device)
        ImGui_ImplOSX_Init(view)
        font atlas → GPU texture
      new_frame(w, h, dt)
        io.DisplaySize = (w, h)
        io.DeltaTime   = dt > 0 ? dt : steady_clock-derived
        rpd ← MTLRenderPassDescriptor (textures from mtk.currentDrawable)
        ImGui_ImplMetal_NewFrame(rpd)
        ImGui_ImplOSX_NewFrame(view)
        ImGui::NewFrame()
      render(cmd_encoder, cmd_buffer)
        draw all 10 panels
        ImGui::Render()
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), cb, encoder)
        # null encoder/cb branch → ImGui::EndFrame() (no GPU work)
```

## Event flow

```
NSApplication
  → NSEvent (key/mouse/scroll)
    → AAASeedMTKView's local NSEvent monitor (installed at Studio init)
        → Studio::handle_ns_event(ev)
            → returns WantCaptureMouse || WantCaptureKeyboard
        → returns nil to NSEvent dispatcher → event consumed by ImGui
        → returns ev → event continues to AAASeedInputView for engine
    → imgui_impl_osx's own internal NSEvent monitor (installed by
      ImGui_ImplOSX_Init) updates ImGui IO state independently
```

## MEU Runner ↔ Studio

```
aaa::meu::Runner
  → runs Lua MEU script each frame
  → (v2) calls aaa.studio.add_node() / aaa.studio.log()
  → (v2) Studio receives node mutations live

GOL::MetalBackend
  → owns MTL::Device + MTL::CommandQueue
  → get_device()         → MTL::Device* (used by Studio::init)
  → get_active_encoder() → void* cast of MTL::RenderCommandEncoder*
                           (used by Studio::render each frame)
  → begin_window_render_pass() / present_window()
     must bracket Studio::render()
```

## Frame Sequence

```
drawInMTKView:
  [1] lazy init Studio (first frame only)
  [2] studio->new_frame(w, h, dt)       ← before render pass
  [3] backend->begin_window_render_pass(drawable, desc)
  [4]   ... engine draw calls ...
  [5]   studio->render(active_encoder)  ← ImGui on top
  [6] backend->present_window(drawable)
```
