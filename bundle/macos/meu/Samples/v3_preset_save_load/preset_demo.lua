--  bundle/macos/meu/Samples/v3_preset_save_load/preset_demo.lua
--
--  Sample MEU : "v3_preset_save_load" -- v3 canonical demo for the
--  preset save/load surface (c149-A).
--
--  Demonstrates the v3 preset bindings that complete the
--  "persist parameters across sessions" half of the authoring loop :
--
--    1. `aaa.io.save_file_dialog( title, default_name, ext_list )
--       -> path_or_nil` -- a Lua-callable wrapper around the
--       c131-B NSSavePanel adapter. Same return contract as
--       `aaa.io.open_file_dialog` but for SAVING : the user picks
--       a destination, Cocoa prompts on overwrite, the call
--       returns the chosen path or nil on cancel.
--
--    2. `aaa.io.save_preset( path ) -> bool` -- serializes the
--       current widget-state map (all `aaa.ui.*` widget values
--       the runner has captured this frame) to a Lua-table file
--       at `path`. Returns true on success ; false on I/O error.
--       The on-disk format is a single `return { ... }` chunk
--       so the file is hand-editable + version-control-friendly.
--
--    3. `aaa.io.load_preset( path ) -> bool` -- reverse of save :
--       reads the Lua-table file at `path`, restores widget state
--       to the saved values. Widgets whose IDs no longer exist in
--       the running MEU are skipped (forward-compatible).
--       Returns true on success ; false on I/O / parse error.
--
--  Together with the modal + text-input surface from v2 Phase 3,
--  these bindings let an artist tweak sliders mid-session, save
--  the result to a named .lua preset, and reload it on a future
--  session (or on a teammate's machine).
--
--  See docs/AUTHORING_MEUS_ON_MAC.md section 4 "aaa.io.* file I/O
--  API (v3)" for the full binding reference.

--  Persisted widget state. The Lua locals act as the source of
--  truth this frame ; the runner's widget-state map mirrors them
--  for `aaa.io.save_preset` to serialize.
local intensity = 1.0
local hue = { r = 1.0, g = 0.5, b = 0.2, a = 1.0 }

--  Most recent operation status -- shown on the HUD line so a
--  screenshot captures the live save/load result.
local status_msg = "Ready"

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Preset Save/Load Demo", 16, 16, 360, 280 )

    aaa.draw_hud_text( status_msg )

    --  Phase 1 slider drives the intensity uniform.
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )

    --  Phase 2 HSV picker drives the color uniform.
    hue.r, hue.g, hue.b, hue.a = aaa.ui.hsv_color_picker(
        "main_color", hue.r, hue.g, hue.b, hue.a )

    --  Save button -- opens NSSavePanel, then serializes widget
    --  state to the chosen file. Same return contract as open :
    --  nil on cancel, path string on accept. The default_name is
    --  pre-filled in the Cocoa text field ; the user can change
    --  it before clicking Save.
    if aaa.ui.button( "Save preset..." ) then
        local path = aaa.io.save_file_dialog(
            "Save preset", "my_preset.lua", { "lua" } )
        if path then
            if aaa.io.save_preset( path ) then
                status_msg = "Saved : " .. path
                aaa.log( "preset_demo : saved " .. path )
            else
                status_msg = "Save FAILED : " .. path
                aaa.log( "preset_demo : save failed " .. path )
            end
        else
            aaa.log( "preset_demo : save cancelled" )
        end
    end

    --  Load button -- opens NSOpenPanel, then deserializes the
    --  chosen file into the runner widget-state map. On success
    --  the next `aaa.ui.slider` / `aaa.ui.hsv_color_picker`
    --  calls will return the restored values, so the Lua locals
    --  catch up automatically through the return-and-reassign
    --  idiom.
    if aaa.ui.button( "Load preset..." ) then
        local path = aaa.io.open_file_dialog(
            "Load preset", { "lua" } )
        if path then
            if aaa.io.load_preset( path ) then
                status_msg = "Loaded : " .. path
                aaa.log( "preset_demo : loaded " .. path )
            else
                status_msg = "Load FAILED : " .. path
                aaa.log( "preset_demo : load failed " .. path )
            end
        else
            aaa.log( "preset_demo : load cancelled" )
        end
    end

    aaa.ui.end_panel()

    --  Drive `aaa_bloom_real` with the live values. The bloom
    --  shader takes a float-slot-0 intensity gain + a vec4-slot-0
    --  tint. After a successful `load_preset` the slider position
    --  + picker color snap to the restored values, and the bloom
    --  output updates the same frame.
    aaa.use_shader( "aaa_bloom_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_float( 0, intensity )
    aaa.set_uniform_vec4( 0, hue.r, hue.g, hue.b, hue.a )
    aaa.draw_fullscreen_quad()
end

aaa.log( "preset_demo.lua : v3 preset save/load demo loaded" )
