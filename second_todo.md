# second_todo.md — Upstream `aaaseed_exe-mac-port` → `aaaseed-for-mac` native-port gap catalog

> **c154 status (2026-05-30):** QT STUDIO INTEGRATION DONE — all 8 c153 native sub-libs are now
> reachable from the intuitive QML UI via a `nativeDevices`/`nativeDisplay` bridge
> (`src/ui/qt/aaa_native_bridge.{h,cpp}`): a new **Devices** tab (MIDI/audio/video/Syphon/display/
> clipboard) + a new **Display** menu (engine output : Intuitive preview vs Native window, with
> multi-display span). **Permissions fix:** the shipping Studio DMG (`aaaseed_app_qt`) now consumes
> the canonical `Info.plist.in` (camera+mic usage strings) — previously it auto-generated a plist
> WITHOUT them, a latent TCC-kill on its Qt-Multimedia camera access. +3 Qt tests (unit/integration/
> regression). **Full ctest 90/90 green, 0 regressions ; existing 54 Qt::Test + Studio panels
> untouched.** Integration + data-flow + debug doc: `docs/developer/qt-native-integration.md`.
> REMAINING: §8 live ≥2-display verification (hardware), §4 `bdd_syphon` widget, §6 engine SIMD resync.

> **c153 status (2026-05-30):** DONE — §1 Audio, §2 Video (zero-copy Metal), §3 MIDI, §4 Syphon
> directory+flip, §5 Net, §6 Clipboard, §7 Context menu, §8 Multi-window incl. the per-display
> present primitive — **8 native hermetic sub-libs + 80 tests, 75 pass / 5
> hardware-or-network-skipped, full ctest 87/87 green, 0 regressions**. §8 host integration is
> wired into `AAASeedMTKView` (opt-in, single-display verified). REMAINING (tracked): §8 live
> verification on a ≥2-display rig (code-complete; no such hardware here), §4's BDD-widget
> surface, and the §6-engine SIMD resync. See per-section `[x]/[ ]` markers below.


> Created continuation 153 (2026-05-30). Source of truth for the **second wave** of porting work:
> bringing the engine author's own macOS branch (`aaaseed-originals/aaaseed_exe-mac-port`,
> GitLab `aaa_foundation/aaaseed_exe` branch `mac-port`) across into our Metal-native repo.
>
> **Companion file:** `memory/todo.md` (wave-1 work, mostly `[x]`). Cross-linked.
> **Doctrine reminder:** `memory/project_context.md` line 87 marked Mac-only feature subsystems
> "out of scope until v2". This file reactivates a subset by explicit user request
> (continuation 153). Each item below is native-Metal / native-Apple-framework — **no stubs
> claimed done**, full test pyramid (unit + integration + regression) required per
> `memory/session_start.md` rule 5.

---

## 0. Method & provenance (zero-hallucination basis)

- Upstream tree: `aaaseed-originals/aaaseed_exe-mac-port` — the engine author's OWN macOS port.
  It **keeps OpenGL/GLUT for rendering** (`Renderer_AAA_mac.cpp` is an event-wiring stub;
  `Runner_mac.cpp` drives `glfwPollEvents`). It uses **Metal only for Syphon interop**.
- Diff vs upstream `main` (Windows): **~50 new `*_mac` files, 915 differing files**. The 915
  are overwhelmingly **cross-platform SIMD refactors** (`sse2neon` → `simde`, AVX2/SSE
  alignment, LF line-endings) — engine-internal, NOT Mac-GPU work. They are tracked in §6 as a
  separate "engine resync" concern, not feature ports.
- **The upstream branch adds ZERO new `.metal` / `.glsl` shaders** (verified: `find … -name
  '*.metal' / '*.glsl'` returns nothing outside ThirdParty). The runtime GLSL shader corpus
  lives in the *content* repo `aaaseed-main`, unchanged by this branch. → "new shaders" in the
  request resolves to **0 new shader files**; our repo already ports 169 MSL shaders. The real
  GPU-leverage opportunities are in **video decode/capture → Metal texture zero-copy** (§2) and
  the **Syphon receiver path** (§4).

### Gap matrix (upstream feature → our repo status)

| # | Upstream subsystem | Upstream files / LOC | Our repo status | Native-port target |
|---|---|---|---|---|
| 1 | **Audio (CoreAudio)** | `media/sound/{snd_player,sound,sound_input}_coreaudio`, `BeatDetector.h` — 6 files, 932 LOC | **ABSENT** | AVAudioEngine + CoreAudio device enum; BeatDetector |
| 2 | **Video (AVFoundation)** | `media/video/mov_avfoundation`, `media/video/capture/capture_avfoundation` — 4 files, 1371 LOC | **ABSENT** | AVPlayer/AVCaptureSession → `CVMetalTextureCache` zero-copy MTLTexture |
| 3 | **MIDI (CoreMIDI)** | `obj_ui/com/midi_mac.{h,mm}` — 2 files, 320 LOC | **ABSENT** | CoreMIDI client/in/out ports |
| 4 | **Syphon receive + directory** | `bdd_spe/bdd_syphon.{h,cpp}` + `SyphonServerMac` — 565+418 LOC | **PARTIAL** (server+client+Lua present; no receiver-discovery / multi-server / per-instance BDD) | Extend `src/syphon`: directory enum, multi-server, receiver path |
| 5 | **Net requests (HTTP+JSON)** | `net_requests/net_requests_{,_json}_mac.mm` — 2 files, 481 LOC | **ABSENT** | NSURLSession async + NSJSONSerialization → Lua table |
| 6 | **Clipboard copy/paste** | `infrastructure/clipboard_mac.mm` — 37 LOC | **PARTIAL** (drag-drop only) | NSPasteboard string copy/paste |
| 7 | **Native context menu** | `system/darwin/SystemContextMenu_mac.mm`, `MacMenuBar.mm` | **ABSENT** (Qt QML only) | NSMenu right-click context menu |
| 8 | **Multi-window fullscreen span** | `system/darwin/MultiWindow_mac.{h,mm}` — 686 LOC | **ABSENT** (single NSWindow) | Multi-`NSScreen` borderless windows + per-display Metal layer |
| — | Core event routing | `core/{Controller,Renderer,Runner}_AAA_mac` | N/A (our event path is `AAASeedInputView`/`MTKView` delegate) | No port — different architecture, behavior already covered |

---

## 1. Audio — CoreAudio / AVAudioEngine  `[x]` DONE c153 (tier 2)

**Landed:** hermetic `src/audio/` (`aaaseed_audio`) — device enumeration via the CoreAudio
AudioObject property API (output/input counts + names, crash-safe at 0 devices); `Player`
(AVAudioEngine + AVAudioPlayerNode + AVAudioUnitVarispeed + AVAudioFile : open/play/pause/stop,
volume/pan/rate clamped, position/duration); `Input` (AVAudioEngine inputNode `installTapOnBus`,
returns false with no mic). `beat_detector.{h,mm}` — a **real** energy-based onset detector (NOT
the upstream 12-LOC stub): running-average energy history + noise floor + sensitivity +
refractory period, BPM from median inter-onset interval; PURE std (framework-free). Tests:
`audio_beat_detector_test.cpp` (5 pass — recovers 120 & 90 BPM from synthetic click tracks within
tolerance, silence→no beats, reset) + `audio_mac_test.mm` (5 pass + 1 gated mic skip). All
CoreAudio/AVFoundation symbols SDK-verified.


**Feature:** sound-file playback (volume/pan/position/rate) + audio input capture + device
enumeration + beat detection, mirroring the Windows DirectSound surface.

**Upstream API to mirror** (param-driven `c_sound_*` factory, no direct Lua):
- `c_sound_coreaudio` — device enum (`s_device_in/out_names/ids`, 32 max), input/output selection.
- `c_sound_player_coreaudio` — `play/pause/stop`, `volume`, `pan`, `position`, `frequency`.
- `c_sound_input_coreaudio` — AudioQueue triple-buffered capture.
- `BeatDetector.h` — **upstream is a 12-LOC stub**; do NOT inherit the stub — implement real
  onset/energy beat detection or omit and mark explicitly.

**Native plan:** hermetic `src/audio/` sub-lib (std:: + AVFoundation/AudioToolbox only, per
`[[hermetic-mac-sublibs]]`). `AVAudioEngine` + `AVAudioPlayerNode` for playback; `AVAudioEngine`
input node tap for capture; `AudioObjectGetPropertyData` for device enum.

**Tests:** unit (device-enum returns ≥1 output device; player state machine; beat-detector on
synthetic click track) · integration (play a fixture WAV, assert position advances) · regression
(beat-detector BPM within tolerance on a fixture loop). Add `perf`-labelled budget for the
input-tap callback latency.

**Cross-platform:** Windows path untouched (`#elif AAA_OS_MAC()` only).

---

## 2. Video — AVFoundation → Metal zero-copy  `[x]` DONE c153 (tier 2)  ← unified-memory showcase, verified

**Landed:** hermetic `src/video/` (`aaaseed_video`). **`MetalTextureBridge`** (the headline) —
`CVMetalTextureCache`-backed zero-copy wrap of a BGRA8 `CVPixelBuffer` into an
`MTLPixelFormatBGRA8Unorm` `id<MTLTexture>` with NO CPU copy ; holds the transient
`CVMetalTextureRef` alive until next call/`flush()`/dtor (documented ownership). **`Movie`**
(AVPlayer + AVPlayerItem + `AVPlayerItemVideoOutput`: open/play/pause/stop/rate/volume/
position/duration + `copy_current_pixel_buffer`). **`Capture`** (AVCaptureSession: static
device enum that does NOT trigger the camera-permission dialog, BGRA8 Metal-compatible output,
delegate stashes latest frame, `copy_latest_pixel_buffer`). Tests: `video_metal_bridge_test.mm`
(3 pass — **zero-copy round-trip: synthesized pixel buffer → MTLTexture → getBytes readback
confirms the texture aliases the unified-memory bytes verbatim**, plus nil-input / nil-device
guards) + `video_mac_test.mm` (4 pass + 1 gated camera skip). All CoreVideo/AVFoundation/CoreMedia
symbols SDK-verified ; `devicesWithMediaType:` deprecation is intentional (simplest portable enum).


**Feature:** (a) movie playback (`AVPlayer`), (b) camera/device capture (`AVCaptureSession`)
with format enumeration + dynamic frame-size handling.

**Upstream API to mirror:**
- `c_movie_avfoundation` — `open_specific`, `play/pause/stop`, `set/get_position`, `set_rate`,
  `set_volume`.
- `c_capture_avfoundation` — `open_specific`, `run/stop/update`, `ask_frame`; `do_enum`,
  `get_device_name/count_static`, `get_format_supported_nb/text`; delegate resizes flux buffer
  on size mismatch (upstream note: old code clobbered 1920×1080 with 720×288 — keep the guard).

**Native plan — THIS is where unified memory matters:** decode/capture → `CVPixelBuffer` →
`CVMetalTextureCacheCreateTextureFromImage` → `MTLTexture` with **zero copy** on Apple-Silicon
unified memory (no CPU readback). Feed straight into the existing MetalBackend / MEU sampler
path. `AVPlayerItemVideoOutput` (`copyPixelBufferForItemTime:`) for playback;
`AVCaptureVideoDataOutput` delegate (`kCVPixelFormatType_32BGRA` or `420v`) for capture.

**Tests:** unit (CVMetalTextureCache round-trip: known pixel buffer → MTLTexture → sample back,
assert bytes) · integration (decode a short fixture .mov, render frame N to MTKView,
golden-compare) · regression (frame-hash of decoded fixture frame stable across runs). `perf`
budget on the per-frame zero-copy bind (must be allocation-free). Camera integration test must be
**skippable** when no capture device is present (CI has none) — gate on `AVCaptureDevice` count
and `GTEST_SKIP()` with a log line (no silent skip, per `[[regression-guard-tests]]`).

**Cross-platform:** Windows DirectShow/MediaFoundation path untouched.

---

## 3. MIDI — CoreMIDI  `[x]` DONE c153

**Landed:** hermetic `src/midi/` (`aaaseed_midi`) — CoreMIDI client/in/out ports, device
enumeration, `open_in(cb)`, `send_short`/`send_sysex`, pure `encode_short` / `short_message_length`
/ `is_valid_sysex` helpers. Tests: `tests/native/midi_mac_test.mm` — 19 pass (encode/length/sysex
framing + lifecycle idempotency + device-count + out-of-range guards); 1 skipped (hardware
loopback, logged). All CoreMIDI symbols SDK-verified. Legacy MIDI-1.0 byte-stream API chosen to
mirror the Win32 `midiIn*/midiOut*` model 1:1.


**Feature:** MIDI hardware in/out, replacing Win32 `midiIn*/midiOut*`.

**Upstream API to mirror** (C extern, backs high-level `c_midi`):
`midi_mac_init/deinit`, `..._get_in/out_device_nb`, `..._get_in/out_device_name`,
`..._open_in/out(c_midi*)`, `..._send_short`, `..._send_sysex`, `midi_read_proc` callback.

**Native plan:** hermetic `src/midi/` — shared `MIDIClientRef` + `MIDIInputPortRef` +
`MIDIOutputPortRef`; per-instance endpoint selection; `MIDIReceived`/`MIDISend`;
`MIDIPacketList` parse.

**Tests:** unit (device enum non-crashing on 0 devices; short-message encode round-trip;
sysex framing) · integration (virtual-source → virtual-destination loopback via
`MIDISourceCreate`, assert bytes) · regression (packet parser stable). No GPU; no perf budget.

**Cross-platform:** Windows untouched.

---

## 4. Syphon — receiver + directory + multi-server  `[x]` DONE c153 (directory + flip); receiver core was already shipped

**Landed:** `src/syphon/syphon_directory_mac.{h,mm}` (`DirectoryMac`) — passive server discovery by
observing the shared `"aaa.syphon.frame"` DN on both centers (dual-center per
[[distnoted-dual-center]]); enumerates distinct server names, per-server frame counts, latest
surface id. `src/syphon/syphon_pixel_util.h` — pure header-only `flip_rows_in_place` (the testable
core of `receiver_vertical_flip`). Tests: `tests/native/syphon_directory_mac_test.mm` — 7 pass
(2-server discovery, clear, empty-name guard, 4 flip cases). The receiver path (`ClientMac`,
`consume_metal_texture`) + multi-server send (lua registry keyed by name) **already existed**
(c124). **Still open:** BDD-side per-instance UI surface (`bdd_syphon` widget) — belongs to the
UI/widget owner; tracked here, not a Metal/IOSurface-plumbing gap.


**Feature:** our `src/syphon` already does **server publish + client consume + Lua** via
IOSurface + Distributed Notifications. Upstream `bdd_syphon` adds the **per-instance BDD UI
surface** and a richer model: server **directory discovery** (`aaa_syphon_directory_list` →
uuid+name), **multi-server** (per-BDD named servers), and a fuller **receiver** (`receiver_index`,
`receiver_name`, `vertical_flip`, `receiving_count_out`).

**Native plan:** extend `src/syphon` (do NOT rewrite — additive, preserve existing tests):
- Directory enumeration over the Distributed-Notification advertise channel.
- Multiple named `ServerMac` instances keyed by name.
- Receiver wiring already exists in `syphon_client_mac` — expose `vertical_flip` + index/name
  selection through `syphon_lua_mac`.
- BDD-side integration belongs to UI layer (§ defer to BDD/widget owner) — track separately;
  the *Metal/IOSurface plumbing* is the v1 deliverable here.

**Tests:** extend existing `syphon_*_test` — add directory-enum test (publish 2 named servers,
assert both discovered), vertical-flip correctness (golden), multi-server isolation. Keep
`syphon_round_trip_perf_test` green (no regression).

**Cross-platform:** Spout path on Windows untouched.

---

## 5. Net requests — NSURLSession + JSON→Lua  `[x]` DONE c153

**Landed:** hermetic `src/net/` (`aaaseed_net`) — `net_mac.{h,mm}` async `get`/`post` via
NSURLSession (id-keyed mutex-guarded in-flight map), `AuthMode` NONE/BASIC/BEARER + pure
`make_auth_header` (self-base64), `get_sync` (semaphore, test-only, shared_ptr result block so a
late completion never writes a dead frame); `net_json_lua.{h,mm}` — `aaa.net.parse_json` →
recursive Lua table (dict/array/number/bool-via-`CFBooleanGetTypeID`/null/string). Tests:
`tests/native/{net_mac_test.mm (6 pass + 1 skipped live-network, gated on AAA_NET_LIVE),
net_json_lua_test.cpp (5 pass)}`. All Foundation symbols SDK-verified.


**Feature:** async HTTP GET/POST + JSON parse, replacing the CPR-based path.

**Upstream API to mirror:** `net_requests::get/post` (async, NSURLSession + dispatch),
`set_auth_mode` (NONE/BASIC/BEARER), `parse_json_to_table` (NSJSONSerialization → Lua table,
recursive). Response map keyed by request id, mutex-guarded.

**Native plan:** hermetic `src/net/` — `NSURLSession` data tasks on a serial queue; completion
posts into an id-keyed result map; JSON via `NSJSONSerialization`. JSON→Lua table conversion
goes through the existing Lua shim layer.

**Tests:** unit (JSON→table: dict/array/nested/null/number/bool round-trip; auth-header
formatting) · integration (GET against a localhost `NSURLSessionDataTask` to a loopback
`NSHTTPServer`/stub, assert 200 + body; **skip if no loopback**) · regression (parser stable on
fixture JSON). No GPU.

**Cross-platform:** Windows CPR path untouched.

---

## 6. Clipboard copy/paste — NSPasteboard  `[x]` DONE c153

**Landed:** hermetic `src/clipboard/` (`aaaseed_clipboard`) — `aaa::clipboard::copy/paste`
(std::string) + the byte-compatible C-ABI `mac_clipboard_copy`/`mac_clipboard_paste` (matches the
upstream branch so the shared engine call site links the same symbol cross-platform). Tests:
`tests/native/clipboard_mac_test.mm` — 5 pass (ASCII + multibyte UTF-8 round-trip, empty/clear,
C-ABI truncation guard, bad-args). Side-effect-free (RAII pasteboard guard restores prior content).


**Feature:** UTF-8 string copy/paste (we only have drag-drop file URLs today).

**Native plan:** add `mac_clipboard_copy/paste(std::string)` to `src/ui/macos`
(`NSPasteboardTypeString`). Small, hermetic.

**Tests:** unit (copy then paste round-trips a UTF-8 string incl. multibyte). No integration/perf
needed beyond unit; regression-guard that we don't read arbitrary pasteboard types.

---

## 7. Native context menu — NSMenu  `[x]` DONE c153 (tier 3)

**Landed:** hermetic `src/menu/` (`aaaseed_menu`) — `aaa::menu::Item` platform-neutral model
(label/id/enabled/separator/children) + `ContextMenu` that builds a recursive NSMenu (submenus via
`setSubmenu`, separators, `setAutoenablesItems:NO` so the model's enabled flag wins) with a target
object recording the selected command id. Path-based introspection accessors make the model→NSMenu
mapping fully testable headless. Honors the upstream nested-submenu recursion fix (commit
caca41cd). Tests: `tests/native/menu_mac_test.mm` — 6 pass (pure count, top-level, submenu +
separator, nested-submenu recursion, enabled flag, out-of-range safety). Live
`popUpContextMenu:withEvent:forView:` path is in-app (not headless-testable).


**Feature:** right-click context menu, Mac-native (today only Qt QML has one). Upstream routes
`callback_event_menu` → `SystemContextMenu_mac`.

**Native plan:** `src/ui/macos` NSMenu builder driven by the engine's menu-item model;
recursive submenu lookup (upstream fixed a recursion bug here — see commit `caca41cd`; honor it).

**Tests:** unit (menu-model → NSMenu item count incl. nested submenus). Integration via the
in-app harness (open menu, assert items). 

**Cross-platform:** Windows menu path untouched.

---

## 8. Multi-window fullscreen span — multi-NSScreen  `[x]` DONE c153 (tier 3) — native core; host present-loop wiring tracked below

**Landed:** hermetic `src/display/`. `display_layout.h` — PURE, header-only geometry:
`virtual_bounds` (bounding canvas over all screens), `normalized_subrect` (a screen's 0..1 slice
of the canvas, for sampling the shared render), `primary_index`. `display_mac.{h,mm}` —
`enumerate_screens()` (live NSScreen frames) + `MultiDisplay` (one borderless, all-Spaces,
screen-saver-level NSWindow per non-primary screen, each backed by a `CAMetalLayer`). Tests:
`display_layout_test.cpp` (6 pass — side-by-side / negative-origin bounds, normalized subrect,
degenerate guards, primary selection, empty) + `display_mac_test.mm` (2 pass: enumeration
consistency + out-of-range safety ; 1 gated `MultiDisplayAuxGated` skip — needs AAA_DISPLAY_LIVE +
≥2 screens + WindowServer).

**Present primitive ALSO landed (c153):** `display_present_mac.{h,mm}` — `SubRectPresenter`
renders a normalized sub-rect of a shared source `MTLTexture` into a destination (an aux drawable's
texture) via an inline-MSL fullscreen-quad pass with nearest/scaling sampling: the exact
per-aux-window blit the engine loop calls each frame. Tests: `display_present_test.mm` (4 pass —
identity preserves columns, right-half→green, left-half→red, nil-arg guard ; CI-safe offscreen
readback, no window). A real shader UV-range bug was caught + fixed by the identity test.

**Host integration LANDED (c153):** wired into `src/ui/macos/AAASeedMTKView.mm`. Opt-in via the
`AAASEED_MULTIDISPLAY` env var (default + multi-monitor behaviour is byte-identical when unset —
zero regression). When enabled with ≥2 screens it sets `framebufferOnly=NO`, then after
`present_window` mirrors each aux display's `normalized_subrect` of the primary frame into that
screen's `CAMetalLayer` via `SubRectPresenter::present_to_drawable` (presenter cmd buffers run
after the engine's on the same queue, so the frame is fully written before sampling). Added
`MetalBackend::native_texture(TextureId)` (additive bridge accessor) + `present_to_drawable`
overload. **Verified:** runtime renders 60 frames clean in default mode and 40 frames clean with
`AAASEED_MULTIDISPLAY=1` on this 1-display machine (gracefully no-ops); full ctest 87/87.

**Only truly-open item:** live end-to-end on a **≥2-display rig** — cannot be exercised on this
1-display dev machine. All components (geometry, aux windows, per-display layers, present
primitive, host wiring) are complete + unit-tested; the multi-monitor branch is code-complete
pending hardware verification. Semantics are *mirror* (each aux shows its sub-rect of the
primary-rendered frame) ; *true span* (rendering the full virtual-canvas resolution) would be a
later engine-resolution change.


**Feature:** one borderless window per non-primary display, shared render output blitted to each
display's sub-rect; handles "Displays have separate Spaces".

**Native plan:** our architecture is single `NSWindow`+`MTKView`. Native version = N borderless
`NSWindow`s each backed by a `CAMetalLayer`, the MetalBackend presents each display's sub-rect.
This is a **significant architecture change** to the UI host — scope carefully; likely its own
multi-session effort.

**Tests:** unit (virtual-rect math: N screens → combined rect + per-screen sub-rects). Integration
needs ≥2 displays (CI has 1) → must `GTEST_SKIP()` with a logged reason.

**Cross-platform:** Windows multi-monitor path untouched.

---

## Engine resync note (the 915 differing files)  `[ ]`

Upstream `main` advanced under our wave-1 vendoring: `sse2neon` retired in favor of `simde`,
AVX2/SSE alignment fixes, `image/convert` refactor, LF line-endings. These are **cross-platform
engine changes**, not Mac-GPU ports. If/when we re-vendor `vendor/aaaseed-engine` to a newer
upstream snapshot, fold these in per `VENDORING.md`. Out of scope for the feature ports above;
recorded so the next agent doesn't mistake SIMD churn for portable feature work.

---

## Execution order (by leverage × tractability × no-regression risk)

1. ~~**§6 Clipboard**~~ — DONE c153.
2. ~~**§5 Net** + **§3 MIDI**~~ — DONE c153 (ran as parallel background agents).
3. ~~**§4 Syphon directory + flip**~~ — DONE c153 (receiver core pre-existed).
4. ~~**§1 Audio**~~ — DONE c153 (tier 2). real BeatDetector (did NOT inherit the 12-LOC stub).
5. ~~**§2 Video**~~ — DONE c153 (tier 2). zero-copy bridge verified by byte-readback.
6. ~~**§7 Context menu**~~ — DONE c153 (tier 3).
7. ~~**§8 Multi-window**~~ — DONE c153 (tier 3); native core + host wiring in `AAASeedMTKView` (opt-in, single-display verified).
8. **§4 BDD-widget surface** — `bdd_syphon` per-instance UI; UI/widget owner.
9. **§8 live ≥2-display verification** — code-complete; needs a multi-monitor rig (unavailable on this dev machine).

Each item: land native impl + 3-tier tests + Lua/UI wiring + doc + memory note, then flip `[ ]`→`[x]` here and cross-link the commit.
