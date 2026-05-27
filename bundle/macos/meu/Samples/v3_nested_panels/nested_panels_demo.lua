--  bundle/macos/meu/Samples/v3_nested_panels/nested_panels_demo.lua
--
--  Sample MEU : "v3_nested_panels" -- v3 canonical demo for deep
--  collapsing-panel nesting (c149-A).
--
--  Phase 4 of v2 (c148) introduced one-level collapsing panels :
--  a `begin_collapsing_panel` could live INSIDE `begin_panel` but
--  not inside another `begin_collapsing_panel`. Deeper nesting
--  was deferred to v3 per the ship-gate doctrine.
--
--  c149-A's widget renderer extension landed the depth tracking
--  required for deep nesting. Each `begin_collapsing_panel` now
--  pushes a depth counter ; the header chrome (indent + chevron
--  + click region) auto-adjusts to the depth so the visual
--  hierarchy reads correctly. The runner still rejects malformed
--  begin/end pair-ups (regression-guarded by the c149-A widget
--  unit tests).
--
--  Layout for this demo :
--
--    begin_panel "Nested Panels Demo"
--      begin_collapsing_panel "Effects"      depth 1
--        begin_collapsing_panel "Color"      depth 2
--          slider "intensity"
--        end_collapsing_panel
--        begin_collapsing_panel "Bloom"      depth 2
--          slider "radius"
--          slider "threshold"
--        end_collapsing_panel
--      end_collapsing_panel
--      begin_collapsing_panel "Atmosphere"   depth 1 (sibling)
--        slider "fog density"
--      end_collapsing_panel
--    end_panel
--
--  See docs/AUTHORING_MEUS_ON_MAC.md section 3.8 "Collapsing
--  panels + hot reload" (v3 nesting subsection) for the depth-
--  rules + idiomatic-collapse-pattern reference.

--  Persisted slider values -- one per leaf widget.
local intensity = 1.0
local fog_density = 0.3
local bloom_radius = 4.0
local bloom_threshold = 0.6

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Nested Panels Demo", 16, 16, 380, 360 )

    --  Top-level "Effects" panel -- contains two nested sub-panels.
    --  begin_collapsing_panel still returns `expanded` ; emit child
    --  widgets ONLY when expanded. Skipping end_collapsing_panel on
    --  the collapsed branch is REQUIRED (and matches the idiomatic
    --  `if begin_collapsing_panel then ... end_collapsing_panel end`).
    if aaa.ui.begin_collapsing_panel( "Effects", 24, 56, 340, 200 ) then
        --  Nested "Color" sub-panel inside "Effects". This is the
        --  v3 capability that v2 Phase 4 rejected -- depth 2.
        --  The widget renderer auto-indents the chevron + header.
        if aaa.ui.begin_collapsing_panel( "Color", 40, 96, 312, 80 ) then
            intensity = aaa.ui.slider(
                "intensity", intensity, 0.0, 2.0 )
            aaa.ui.end_collapsing_panel()
        end

        --  Sibling "Bloom" sub-panel inside "Effects" -- same depth
        --  as "Color". Two sliders (radius + threshold) drive the
        --  bloom shader's spatial + luminance knobs.
        if aaa.ui.begin_collapsing_panel( "Bloom", 40, 180, 312, 80 ) then
            bloom_radius = aaa.ui.slider(
                "radius", bloom_radius, 1.0, 16.0 )
            bloom_threshold = aaa.ui.slider(
                "threshold", bloom_threshold, 0.0, 2.0 )
            aaa.ui.end_collapsing_panel()
        end

        aaa.ui.end_collapsing_panel()
    end

    --  Top-level "Atmosphere" panel -- sibling of "Effects", depth 1.
    --  Sibling collapsing panels at the same depth are unchanged
    --  from Phase 4 ; the new v3 capability is the nested
    --  begin_collapsing_panel INSIDE another, which "Effects" above
    --  exercises.
    if aaa.ui.begin_collapsing_panel( "Atmosphere", 24, 264, 340, 80 ) then
        fog_density = aaa.ui.slider(
            "fog density", fog_density, 0.0, 1.0 )
        aaa.ui.end_collapsing_panel()
    end

    aaa.ui.end_panel()

    --  HUD echo so a screenshot captures the full nested state in
    --  one frame. string.format keeps the line readable.
    aaa.draw_hud_text( string.format(
        "i=%.2f r=%.1f t=%.2f fog=%.2f",
        intensity, bloom_radius, bloom_threshold, fog_density ) )

    --  Drive `aaa_bloom_real` with the live values. Threshold goes
    --  in float slot 0 (gain) ; intensity goes in float slot 1
    --  (a deliberate inversion of the v2_collapse_reload mapping
    --  to highlight that the float-slot table is MEU-author-chosen
    --  per shader, not a global convention).
    aaa.use_shader( "aaa_bloom_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_float( 0, bloom_threshold )
    aaa.set_uniform_float( 1, intensity )
    aaa.draw_fullscreen_quad()
end

aaa.log( "nested_panels_demo.lua : v3 deep-nested collapsing panels demo loaded" )
