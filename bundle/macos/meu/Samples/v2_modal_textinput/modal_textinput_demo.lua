--  bundle/macos/meu/Samples/v2_modal_textinput/modal_textinput_demo.lua
--
--  Sample MEU : "v2_modal_textinput" -- v2 Phase 3 canonical demo.
--
--  Demonstrates the new modal-dialog + text-input combo that c148-A
--  landed. The motivating workflow is "rename preset" : the user
--  clicks a button in the main panel, a modal pops up over the scene
--  asking for a new name, the user types it, clicks OK or Cancel,
--  the modal closes and the main panel sees the updated name (or not,
--  if cancelled).
--
--  Phase 1 + 2 widgets exercise the immediate-mode shape but stay
--  IN-PANEL. Phase 3 introduces TWO new ideas :
--
--    1. Modal mode -- `aaa.ui.show_modal( id )` flips a runner-side
--       flag ; `aaa.ui.begin_modal( id, w, h ) -> open, ok, cancel`
--       returns three booleans : `open` is true while the modal is
--       displaying, `ok_clicked` / `cancel_clicked` are rising-edge
--       on the frame the user accepts / dismisses. The widget layer
--       (c148-A) renders the modal CENTERED on the window with a
--       darkened backdrop ; input outside the modal is consumed by
--       the backdrop so the main panel can't be clicked.
--
--    2. Keyboard text input -- `aaa.ui.text_input( label, value,
--       max_length )` accepts characters from AAASeedInputView (c148-A
--       added the responder-chain text-forwarding path) and returns
--       the updated string each frame. Single-line, ASCII-only,
--       no IME -- those are v3 work (NSResponder chain integration).
--
--  Spirit of vendor/aaaseed-runtime/AAAKernel/GaBu/'s rename-dialog
--  pattern, reimagined as a Mac-native immediate-mode primitive.
--  See docs/AUTHORING_MEUS_ON_MAC.md section 3.7 "Modals + text input".

--  Persisted state. `preset_name` is the committed name shown in the
--  main panel ; `pending_name` is the editable buffer the modal shows
--  while the user is typing. On OK the pending value commits to
--  `preset_name` ; on Cancel it is discarded.
local preset_name = "untitled"
local pending_name = ""

function aaa.on_frame( w, h, frame )
    --  Main panel -- shows the current preset name + a button that
    --  opens the rename modal.
    aaa.ui.begin_panel( "Modal + Text Demo", 16, 16, 340, 200 )

    aaa.draw_hud_text( "preset: " .. preset_name )

    --  Rising-edge button click ; on the frame it returns true, copy
    --  the current preset name into the pending buffer (so the modal
    --  starts pre-filled), then ask the runner to show the modal.
    if aaa.ui.button( "Rename preset..." ) then
        pending_name = preset_name
        aaa.ui.show_modal( "rename_preset" )
    end

    aaa.ui.end_panel()

    --  Modal popup. Renders OVER the main panel + scene when open ;
    --  ignored when closed. The widget layer owns the darkened
    --  backdrop ; this Lua only sees the OK / Cancel signals.
    --
    --  Signature : ( id, w, h ). Returns three booleans : `open`,
    --  `ok_clicked`, `cancel_clicked`. Pair with `aaa.ui.end_modal()`
    --  IFF `open` was true.
    local open, ok_clicked, cancel_clicked = aaa.ui.begin_modal(
        "rename_preset", 280, 140 )
    if open then
        --  Text input. Signature : ( label, value, max_length ).
        --  `max_length` is optional ; default 64. Returns the new
        --  string after this frame's keystrokes. Same return-and-
        --  reassign idiom as the slider / picker.
        pending_name = aaa.ui.text_input( "name", pending_name, 32 )

        --  Rising-edge accept : commit + close. The widget layer
        --  closes the modal automatically when begin_modal returns
        --  ok_clicked OR cancel_clicked the same frame.
        if ok_clicked then
            preset_name = pending_name
            aaa.log( "modal_textinput : renamed to " .. preset_name )
        elseif cancel_clicked then
            aaa.log( "modal_textinput : rename cancelled" )
        end

        aaa.ui.end_modal()
    end

    --  Background scene -- a simple gradient via the additive scale
    --  shader so the modal backdrop has something to darken.
    aaa.use_shader( "ps_Maa_add_scale" )
    aaa.set_uniform_int( 0, 1 )
    aaa.draw_fullscreen_quad()
end

aaa.log( "modal_textinput_demo.lua : v2 Phase 3 modal + text-input demo loaded" )
