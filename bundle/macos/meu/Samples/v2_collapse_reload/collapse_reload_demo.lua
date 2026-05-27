--  bundle/macos/meu/Samples/v2_collapse_reload/collapse_reload_demo.lua
--
--  Sample MEU : "v2_collapse_reload" -- v2 Phase 4 canonical demo.
--
--  Demonstrates two new widget primitives c148-A landed :
--
--    1. `aaa.ui.begin_collapsing_panel( title, x, y, w, h ) -> expanded`
--       + `aaa.ui.end_collapsing_panel()` -- a panel section that can
--       be collapsed (header-only) or expanded (full content visible)
--       by clicking the header. Returns `expanded` (boolean) ; the
--       MEU only emits widgets inside the panel when `expanded`.
--       Pairs 1:1 like `begin_panel` / `end_panel`.
--
--    2. `aaa.ui.hot_reload_button( label? )` -- a button widget that
--       triggers `Runner::reload()` on click. Returns `true` on the
--       frame the click was registered (rising-edge ; same debounce
--       as `aaa.ui.button`). The MEU can use the return value to
--       log / track reload events for QA. `label` is optional ;
--       default is "Reload MEU".
--
--  The demo composes both new primitives with the Phase 1 slider to
--  drive `aaa_bloom_real` end-to-end : two collapsing sections
--  ("Effect" + "Fog") each contain a slider ; the bottom of the main
--  panel has a hot-reload button. Sections start expanded ; clicking
--  the header collapses them. Sliders inside collapsed sections are
--  not drawn (no input, no value change) -- a deliberate property of
--  the immediate-mode collapsing-panel primitive : "if it isn't drawn,
--  it doesn't exist this frame".
--
--  Spirit of vendor/aaaseed-runtime/AAAKernel/GaBu/'s
--  `add_bu_param_group_expandable`, reimagined as a Mac-native
--  immediate-mode primitive that returns its expanded state every
--  frame. See docs/AUTHORING_MEUS_ON_MAC.md section 3.8
--  "Collapsing panels + hot reload".

--  Slider-driven values. Persisted across frames so the slider
--  positions are sticky.
local effect_intensity = 1.0
local fog_density = 0.3

--  Tracks the most recent reload event so the HUD can echo it.
local last_reload_frame = 0

function aaa.on_frame( w, h, frame )
    --  Outer container. The collapsing panels nest INSIDE this with
    --  their own x/y/w/h (relative to the window, not the parent
    --  panel) -- one-level nesting is supported by Phase 4 ; deeper
    --  nesting is v3 work.
    aaa.ui.begin_panel( "Collapsing + Reload Demo", 16, 16, 360, 280 )

    --  "Effect" collapsing section -- contains intensity slider.
    --  begin_collapsing_panel returns `expanded` ; emit child widgets
    --  ONLY when expanded == true.
    if aaa.ui.begin_collapsing_panel( "Effect", 16, 56, 328, 80 ) then
        effect_intensity = aaa.ui.slider(
            "intensity", effect_intensity, 0.0, 2.0 )
        aaa.ui.end_collapsing_panel()
    end

    --  "Fog" collapsing section -- contains density slider. Same
    --  pattern. The two sections layout vertically per the y coords ;
    --  the runner does not auto-stack collapsing panels in Phase 4
    --  (manual positioning, like `begin_panel`).
    if aaa.ui.begin_collapsing_panel( "Fog", 16, 144, 328, 80 ) then
        fog_density = aaa.ui.slider( "density", fog_density, 0.0, 1.0 )
        aaa.ui.end_collapsing_panel()
    end

    --  Hot-reload button at the bottom of the main panel. Clicking
    --  it triggers Runner::reload() which closes + re-opens the
    --  lua_State and re-runs this script. Lua locals (e.g.
    --  effect_intensity) reset to their declared initial values --
    --  this is the canonical way to "reset all sliders" mid-session.
    if aaa.ui.hot_reload_button( "Reload MEU" ) then
        last_reload_frame = frame
        aaa.log( "collapse_reload_demo : hot reload triggered at frame "
                 .. frame )
    end

    aaa.ui.end_panel()

    --  HUD echo so a screenshot captures the live state.
    aaa.draw_hud_text( string.format(
        "intensity=%.2f density=%.2f last_reload=%d",
        effect_intensity, fog_density, last_reload_frame ) )

    --  Drive the bloom shader with the two slider values. The bloom
    --  shader binds float slot 0 to gain (intensity) and slot 1 to a
    --  density falloff. The collapsing panels above don't impose any
    --  extra marshalling -- the slider return values plug DIRECTLY
    --  into set_uniform_float as in Phase 1.
    aaa.use_shader( "aaa_bloom_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_float( 0, effect_intensity )
    aaa.set_uniform_float( 1, fog_density )
    aaa.draw_fullscreen_quad()
end

aaa.log( "collapse_reload_demo.lua : v2 Phase 4 collapsing + reload demo loaded" )
