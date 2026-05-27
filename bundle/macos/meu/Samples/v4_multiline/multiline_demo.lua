--  bundle/macos/meu/Samples/v4_multiline/multiline_demo.lua
--
--  Sample MEU : "v4_multiline" -- v4 canonical demo for the
--  multi-line `aaa.ui.text_area` widget primitive (c150-A).
--
--  Demonstrates the new multi-line text widget that c150-A
--  landed alongside the full `NSTextInputClient` protocol on
--  `AAASeedInputView`. Where `aaa.ui.text_input` (v2 Phase 3)
--  is a single-line scratch field, `aaa.ui.text_area` accepts
--  arbitrary text with embedded newlines :
--
--    aaa.ui.text_area( label, value, visible_lines, width_chars,
--                      max_length )
--
--  Key differences from `text_input` :
--    - newlines INSIDE the value are preserved (single-line
--      `text_input` rejects newline keystrokes).
--    - the widget renderer reserves vertical space for
--      `visible_lines` lines of glyphs (default 8 in this demo).
--    - the cursor moves through lines on arrow-up / arrow-down.
--    - IME composition events (c150-A `NSTextInputClient` path)
--      route through the focused `text_area` exactly like they
--      do through `text_input`.
--
--  Motivating use cases : MEU description fields, embedded
--  shader code snippets, structured notes attached to a preset.
--
--  See docs/AUTHORING_MEUS_ON_MAC.md section 3.9
--  "Multi-line text_area + IME (v4)" for the full reference.

--  Persisted state. The initial text contains embedded newlines
--  to demonstrate that the round-trip preserves them ; the user
--  can edit + add lines, then save the result to disk via the
--  v3 save_file_dialog binding.
local notes = "Welcome to AAASeed v4 !\n"
            .. "\n"
            .. "Type here. Newlines work.\n"
            .. "Tab into the field with mouse-click for now.\n"
            .. "Arrow keys move between lines.\n"

--  Most recent save destination -- shown on the HUD line so a
--  screenshot captures the live save result. "(none)" when no
--  save has been attempted ; "(cancelled)" when the user backed
--  out of the dialog.
local last_save = "(none)"

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "v4 Multi-Line Demo", 16, 16, 420, 360 )

    --  Multi-line text area. Signature :
    --    ( label, value, visible_lines, width_chars, max_length )
    --  Returns the new string after this frame's keystrokes,
    --  same return-and-reassign idiom as `text_input` + slider.
    --
    --  visible_lines = 8 reserves room for 8 glyph rows ;
    --  width_chars  = 48 reserves room for 48 characters per row
    --                 (the renderer wraps glyphs beyond that) ;
    --  max_length   = 1024 caps the buffer at 1024 chars.
    notes = aaa.ui.text_area( "notes", notes, 8, 48, 1024 )

    --  Save button -- writes the text-area contents out via the
    --  v3 save_file_dialog. The actual disk write is left as a
    --  synthetic acknowledgement so this sample stays read-only
    --  ; the real save path would write `notes` to `path` via
    --  io.open / write / close. (v3's aaa.io.save_preset is
    --  widget-state-only ; arbitrary text needs a future
    --  aaa.io.write_file binding.)
    if aaa.ui.button( "Save notes..." ) then
        local path = aaa.io.save_file_dialog(
            "Save notes", "notes.txt", { "txt" } )
        if path then
            last_save = path
            aaa.log( "multiline_demo : would save " .. path )
        else
            last_save = "(cancelled)"
            aaa.log( "multiline_demo : save cancelled" )
        end
    end

    aaa.ui.end_panel()

    --  Count lines for the HUD : Lua's gsub counts replacements,
    --  so `gsub(notes, "\n", "\n")` returns (replacement_string,
    --  num_replacements) -- one newline per replacement, plus
    --  one for the final line that has no trailing newline.
    local _, newline_count = notes:gsub( "\n", "\n" )
    aaa.draw_hud_text( "lines : " .. ( newline_count + 1 )
                       .. " | last save : " .. last_save )

    --  Drive `aaa_curl_noise_real` underneath so the panel sits
    --  over a moving field -- helps visual QA of the text-area
    --  background alpha + cursor blink against motion.
    aaa.use_shader( "aaa_curl_noise_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.draw_fullscreen_quad()
end

aaa.log( "multiline_demo.lua : v4 multi-line text_area demo loaded" )
