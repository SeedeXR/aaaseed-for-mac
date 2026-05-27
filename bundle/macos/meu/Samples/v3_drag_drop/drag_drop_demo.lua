--  bundle/macos/meu/Samples/v3_drag_drop/drag_drop_demo.lua
--
--  Sample MEU : "v3_drag_drop" -- v3 canonical demo for the new
--  drag-drop + file-dialog file-I/O surface (c149-A).
--
--  Demonstrates the v3 file-I/O bindings that completed the in-app
--  authoring loop on Mac :
--
--    1. `aaa.io.drop_file( path )` -- synthetic test injection that
--       simulates an NSWindow drag-drop event. The real production
--       path is the AAASeedMTKView registerForDraggedTypes + the
--       prepareForDragOperation / performDragOperation responder
--       chain c149-A wired into the host view. From a Lua MEU you
--       only consume the result : when a .lua is dropped onto the
--       window, the runner's drag-drop hook calls Runner::load_script
--       and re-runs the chunk. From inside an MEU you can call
--       `aaa.io.drop_file( path )` to deterministically test that
--       hook (used by the c149-A integration tests).
--
--    2. `aaa.io.open_file_dialog( title, ext_list ) -> path_or_nil`
--       -- a Lua-callable wrapper around the c131-B NSOpenPanel
--       adapter (`src/ui/macos/aaa_file_dialog.{h,mm}`). Blocking
--       sheet dialog ; returns the chosen path string or nil if the
--       user cancelled. `ext_list` is a Lua array of extension
--       strings (no leading dot) -- e.g. { "lua" } or
--       { "lua", "plua", "meu" }.
--
--  Two ways to "load another MEU" while one is running :
--    - Drag a `.lua` from Finder onto the window (handled by the
--      host, not by this script).
--    - Click "Browse for MEU script..." (this script).
--
--  See docs/AUTHORING_MEUS_ON_MAC.md section 4
--  "aaa.io.* file I/O API (v3)" for the full binding reference.

--  HUD state -- echoes the most recently loaded path so a
--  screenshot captures the live drag-drop result. Survives one
--  frame ; resets to "(none yet)" on `Runner::reload()`.
local last_loaded = "(none yet)"

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Drag-Drop Demo", 16, 16, 360, 200 )

    aaa.draw_hud_text( "Last loaded : " .. last_loaded )

    --  Browse-for-MEU button. Opens a Cocoa NSOpenPanel filtered to
    --  `.lua` files only. On accept the chosen path is recorded in
    --  the HUD ; on cancel the dialog returns nil and last_loaded
    --  is unchanged.
    --
    --  Note : in a real app this would also call something like
    --  `runner.load_script( path )` to swap the MEU in-place, but
    --  the Lua sandbox does not expose that entry point (it lives
    --  on the C++ Runner). Drag-drop onto the window IS that
    --  canonical "load another MEU" path ; this button just lets
    --  the artist preview the dialog flow without leaving the .app.
    if aaa.ui.button( "Browse for MEU script..." ) then
        local path = aaa.io.open_file_dialog(
            "Choose a .lua MEU script",
            { "lua" } )
        if path then
            last_loaded = path
            aaa.log( "drag_drop_demo : selected " .. path )
        else
            aaa.log( "drag_drop_demo : browse cancelled" )
        end
    end

    aaa.ui.end_panel()

    --  Background scene -- the noise shader gives drag-drop a
    --  visual context (otherwise an empty window is hard to
    --  distinguish from "did the .app crash ?").
    aaa.use_shader( "aaa_noise_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.draw_fullscreen_quad()
end

aaa.log( "drag_drop_demo.lua : v3 drag-drop + open_file_dialog demo loaded" )
