--  bundle/macos/meu/Samples/v4_ime_text/ime_text_demo.lua
--
--  Sample MEU : "v4_ime_text" -- v4 canonical demo for the IME
--  composition + commit surface (c150-A).
--
--  Demonstrates the new `NSTextInputClient` protocol path that
--  c150-A wired into `AAASeedInputView`. With a CJK keyboard
--  input source active (System Settings -> Keyboard -> Input
--  Sources -> Pinyin / Hiragana / Hangul), typing into the
--  `aaa.ui.text_input` field below routes through the macOS
--  Text Input System :
--
--    1. Each keystroke produces a `marked text` event with an
--       underline-rendered pending composition (e.g. typing "k"
--       then "o" on a Japanese Romaji IME shows the two-letter
--       pending composition with a styled underline).
--    2. Pressing Enter (or Space, depending on the IME's commit
--       gesture) commits the composition : the marked text
--       clears, the committed string is inserted at the cursor,
--       and the `aaa.ui.text_input` return value updates.
--    3. Pressing Escape clears the pending marked text without
--       committing.
--
--  Real-world CJK input verification requires a Mac with a CJK
--  input source active -- the same shape as drag-drop or
--  interactive Space-press checks from c143-C / c145-A / c149-A.
--  For autonomous test coverage this sample ALSO exposes a
--  synthetic API that drives the same code path from Lua :
--
--    - `aaa.ime.set_marked_text( text, sel_start, sel_len )` :
--      injects a pending composition (the underline-rendered
--      marked text). Same effect as the IME calling
--      `setMarkedText:selectedRange:replacementRange:` on the
--      input view.
--    - `aaa.ime.commit_marked_text()` : commits whatever marked
--      text is currently pending into the focused text input.
--      Same effect as the IME calling `insertText:` with the
--      finalised composition.
--
--  See docs/AUTHORING_MEUS_ON_MAC.md section 3.9
--  "Multi-line text_area + IME (v4)" for the full reference.

--  Persisted state. `user_name` is the committed string echoed
--  on the HUD ; the IME composition lives in the runner's
--  marked-text buffer between commits, not as Lua state.
local user_name        = ""
local synthetic_demo   = false
local synthetic_status = "(idle)"

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "v4 IME Text Input Demo", 16, 16, 380, 240 )

    --  Text input -- CJK ok when an IME is active. Composition
    --  renders with an underline ; commit on Enter (or the IME's
    --  own commit gesture).
    user_name = aaa.ui.text_input( "your name (CJK ok)", user_name, 64 )

    aaa.draw_hud_text( "name : " .. user_name
                       .. " | synth : " .. synthetic_status )

    --  Button to kick off the synthetic IME round-trip. Once
    --  pressed, the next 90 frames inject a 3-step pretend
    --  composition into the text_input above so a screenshot
    --  shows the marked-text underline + commit transition
    --  without a CJK keyboard.
    if aaa.ui.button( "Run synthetic IME demo (type then commit)" ) then
        synthetic_demo   = true
        synthetic_status = "running"
    end

    aaa.ui.end_panel()

    --  Synthetic IME drive : pretend the IME composed "k" then
    --  "ko" then committed. Mirrors the shape a Japanese Romaji
    --  IME would produce when typing "ko" + Enter, but driven
    --  entirely from Lua so the test harness can verify the
    --  marked-text + commit code path without a real IME.
    if synthetic_demo then
        local step = frame % 90
        if step == 0 then
            aaa.ime.set_marked_text( "k", 0, 1 )
            synthetic_status = "marked : k"
        elseif step == 30 then
            aaa.ime.set_marked_text( "ko", 0, 2 )
            synthetic_status = "marked : ko"
        elseif step == 60 then
            aaa.ime.commit_marked_text()
            synthetic_status = "committed"
            synthetic_demo   = false
        end
    end

    --  Background fill so the sample has visible motion under
    --  the marked-text underline. `aaa_noise_real` is the c135-A
    --  Perlin + Simplex + FBM revival.
    aaa.use_shader( "aaa_noise_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.draw_fullscreen_quad()
end

aaa.log( "ime_text_demo.lua : v4 IME composition + commit demo loaded" )
