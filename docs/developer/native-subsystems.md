# Native peripheral subsystems (c153)

These four sub-libraries port the **non-rendering** Mac-native capabilities from the engine
author's own `mac-port` branch (`gitlab.com/aaa_foundation/aaaseed_exe`, branch `mac-port`) into
this Metal-native tree. Each is a **hermetic** sub-lib per the project's
[hermetic Mac sub-lib doctrine](memory-doctrine.md): pure ObjC++ (`-fno-objc-arc`), `std::` plus
Apple frameworks only — **no engine link, no `o_str`, no `aaa_mem` cascade, no vendor edits**.
They build by default and ship a runnable test pyramid in `tests/native/`.

> The upstream `mac-port` branch keeps OpenGL/GLUT for rendering and uses Metal only for Syphon.
> It adds **no new shaders**. The genuinely-new work it contributes is these feature subsystems.
> The full gap analysis + the remaining roadmap (Audio, Video→Metal zero-copy, context menu,
> multi-window) lives in `second_todo.md` at the repo root.

## Overview

| Sub-lib | Target | Frameworks | Lua surface | Tests |
|---|---|---|---|---|
| `src/audio/` | `aaaseed_audio` | AVFoundation, CoreAudio, Foundation | (backs engine sound) | 10 (+1 mic-skip) |
| `src/video/` | `aaaseed_video` | AVFoundation, CoreVideo, CoreMedia, Metal | (frame source → Metal) | 7 (+1 cam-skip) |
| `src/clipboard/` | `aaaseed_clipboard` | AppKit | — (C-ABI + C++) | 5 |
| `src/midi/` | `aaaseed_midi` | CoreMIDI, CoreFoundation | (backs engine `c_midi`) | 19 (+1 hw-skip) |
| `src/net/` | `aaaseed_net` | Foundation, CoreFoundation, `aaaseed_lua` | `aaa.net.parse_json` | 11 (+1 net-skip) |
| `src/menu/` | `aaaseed_menu` | AppKit | (backs engine context menu) | 6 |
| `src/display/` | `aaaseed_display` | AppKit, QuartzCore, Metal | (host multi-display) | 12 (+1 gated-skip) |
| `src/syphon/` (extension) | `aaaseed_syphon` | Metal, Foundation, CoreFoundation, IOSurface | (extends `aaa.syphon.*`) | 7 |

Full default suite after c153: **87/87 ctest green, 0 regressions** (82 active + 5 explicitly
skipped — hardware MIDI loopback, live network, live mic, live camera, live multi-display — each
logged via `GTEST_SKIP`, never silent).

## Audio — `src/audio/`

CoreAudio device enumeration + AVAudioEngine playback/capture + a **real** energy-based beat
detector (deliberately not the 12-LOC upstream stub).

```cpp
#include "src/audio/audio_mac.h"
#include "src/audio/beat_detector.h"

aaa::audio::Player p;
if (p.open("/path/track.wav")) { p.set_volume(0.8f); p.play(); }

aaa::audio::BeatDetector det;
det.process_block(samples, frame_count, 44100.0);   // feed PCM blocks
if (det.beat_detected_in_last_block()) { /* ... */ }
double bpm = det.estimate_bpm();
```

- Device enumeration (`get_output_device_count` / `get_input_device_count` + names) uses the
  CoreAudio `AudioObject` property API and is crash-safe with zero devices.
- `Player` = AVAudioEngine + AVAudioPlayerNode + AVAudioUnitVarispeed (rate/pitch) + AVAudioFile.
- `Input` installs a tap on the input node and feeds PCM blocks to a callback — the intended
  feeder for `BeatDetector::process_block`.
- `BeatDetector` is **pure std** (framework-free): running-average energy history, noise floor,
  sensitivity multiplier, refractory period; BPM from the median inter-onset interval. Fully
  deterministic and unit-tested on synthetic click tracks (recovers 120 & 90 BPM within ±8).

## Video — `src/video/` (zero-copy Metal, the unified-memory showcase)

The headline of wave 2: a **zero-copy** path from a decoded/captured `CVPixelBuffer` to an
`id<MTLTexture>` via `CVMetalTextureCache`. On Apple Silicon the CPU and GPU share one physical
memory pool, so the IOSurface-backed pixel buffer and the Metal texture wrapping it point at the
**same bytes** — no readback, no staging, no blit.

```cpp
#include "src/video/metal_texture_bridge.h"
#include "src/video/movie_mac.h"

aaa::video::MetalTextureBridge bridge(device);     // one cache per MTLDevice
aaa::video::Movie movie;
if (movie.open("/path/clip.mov")) {
    movie.play();
    CVPixelBufferRef pb = movie.copy_current_pixel_buffer();   // caller CFReleases
    if (pb) {
        id<MTLTexture> tex = bridge.texture_from_pixel_buffer(pb);  // ZERO copy
        // ... sample tex in a shader ...
        CFRelease(pb);
    }
}
bridge.flush();   // once per frame, after the GPU has consumed the texture
```

- **`MetalTextureBridge`** holds the transient `CVMetalTextureRef` alive until the next call /
  `flush()` / dtor (releasing it early would let CoreVideo recycle the underlying image and
  invalidate the vended texture). BGRA8 → `MTLPixelFormatBGRA8Unorm`.
- **`Movie`** = AVPlayer + `AVPlayerItemVideoOutput` configured for BGRA8 Metal-compatible
  IOSurface-backed buffers; `copy_current_pixel_buffer()` returns a +1-retained buffer.
- **`Capture`** = AVCaptureSession; the **static device enumeration never starts a session**, so
  it does not trigger the macOS camera-permission (TCC) dialog and is CI-safe. `start()` does
  prompt; tests gate it behind `AAA_VIDEO_LIVE=1`.
- The headline test synthesizes a known `CVPixelBuffer`, wraps it, and reads the texture bytes
  back — proving the texture aliases the pixel-buffer's unified-memory bytes verbatim. No camera
  or file needed.

## Clipboard — `src/clipboard/`

NSPasteboard UTF-8 string copy/paste. The wave-1 port only handled drag-drop file URLs.

```cpp
#include "src/clipboard/clipboard_mac.h"
aaa::clipboard::copy("hello");            // -> bool
std::string s = aaa::clipboard::paste();  // "" if no string on the pasteboard
```

Also exposes the **byte-compatible C-ABI** `mac_clipboard_copy(const char*, int)` /
`mac_clipboard_paste(char*, int, int*)` that the shared engine source references directly, so the
GaBu text-field paste path links the same symbol on Windows and Mac with no `#ifdef` at the call
site (cross-platform reciprocity).

## MIDI — `src/midi/`

CoreMIDI input/output, replacing the Win32 `midiIn*/midiOut*` byte-stream API 1:1. A shared
`MIDIClientRef` + one input port + one output port; per-instance endpoint selection.

```cpp
aaa::midi::init();
auto n = aaa::midi::get_in_device_count();
aaa::midi::open_in(0, [](const unsigned char* d, std::size_t len){ /* ... */ });
aaa::midi::send_short(0, 0x90, 60, 100);   // note-on ch1 C4 vel100
```

Pure, hardware-free helpers (`encode_short`, `short_message_length`, `is_valid_sysex`) carry the
deterministic logic so the message framing is unit-tested without a MIDI device. The legacy
MIDI-1.0 byte API is deliberately chosen (over the newer UMP/`MIDIEventList` stack) to mirror the
Windows byte model exactly.

## Net — `src/net/`

`NSURLSession` async HTTP + `NSJSONSerialization`→Lua, replacing the CPR/RapidJSON path.

```cpp
aaa::net::set_auth(aaa::net::AUTH_BEARER, "token");
aaa::net::get("https://api.example.com/x", [](long code, std::string body){ /* ... */ });
```

- In-flight requests tracked in an id-keyed, mutex-guarded map; `get_sync` (test-only) blocks on a
  `dispatch_semaphore` with a heap `shared_ptr` result block so a late completion after timeout
  never writes through a dead stack frame.
- `aaa.net.parse_json(jsonString)` returns a recursive Lua table (dict→table, array→1-indexed,
  number/bool/null/string). JSON `true/false` are disambiguated from numbers via
  `CFGetTypeID(...) == CFBooleanGetTypeID()`.

## Syphon directory + vertical-flip — `src/syphon/` (extension)

The wave-1 stack already shipped `ServerMac` (publish) + `ClientMac` (receive a frame from a
**known** server) + the `aaa.syphon.*` Lua bindings. c153 adds **discovery** and a flip helper:

- **`DirectoryMac`** — passive enumeration of currently-advertising servers by observing the shared
  `"aaa.syphon.frame"` Distributed Notification on **both** the local and distributed centers
  ([distnoted dual-center doctrine](memory-doctrine.md)). No advertising of its own, so it never
  contends with a `ClientMac`. Reports distinct server names, per-server frame counts, and the
  latest IOSurfaceID.
- **`flip_rows_in_place`** (`syphon_pixel_util.h`) — the pure, header-only, GPU-free core of the
  `receiver_vertical_flip` feature (many Syphon/OpenGL-origin producers deliver bottom-up frames).
  Unit-tested over packed and padded strides.

```cpp
#include "src/syphon/syphon_directory_mac.h"
aaa::syphon::DirectoryMac dir;
dir.poll(0.1);
for (auto const& name : dir.server_names()) { /* discovered servers */ }
```

## Context menu — `src/menu/`

A native NSMenu right-click context menu built from a platform-neutral C++ model, so the engine's
menu description produces the same structure on Mac as the Win32 menu does on Windows.

```cpp
#include "src/menu/menu_mac.h"
using aaa::menu::Item;

std::vector<Item> items = {
    { "Open",   /*id*/11 },
    Item::make_separator(),
    { "Recent", 12, true, false, { { "a.lua", 20 }, { "b.lua", 21 } } },  // submenu
};
aaa::menu::ContextMenu menu;
menu.set_items(items);
int chosen = menu.popup_at(view, event);   // in-app; chosen == selected command id
```

- Recursive builder (submenus, separators); `setAutoenablesItems:NO` so the model's `enabled`
  flag is authoritative. Selection is recorded via a target object (`NSMenuItem.tag` = command id).
- An NSMenu builds + introspects without a running NSApplication, so the model→NSMenu mapping is
  fully unit-tested headless via path-based accessors (`item_count_at({0,3})`, `label_at`,
  `is_separator_at`, `has_submenu_at`, `id_at`). Honors the upstream nested-submenu recursion fix.

## Multi-display — `src/display/`

Fullscreen-span building blocks: pure geometry + live NSScreen enumeration + borderless aux
windows with per-display CAMetalLayers.

```cpp
#include "src/display/display_layout.h"   // pure, header-only
#include "src/display/display_mac.h"

auto screens = aaa::display::enumerate_screens();
auto bounds  = aaa::display::virtual_bounds(screens);            // spanning canvas
auto slice   = aaa::display::normalized_subrect(screens[1], bounds);  // 0..1 sub-rect

aaa::display::MultiDisplay md;
std::size_t aux = md.enable();              // borderless window per non-primary screen
void* layer = md.aux_metal_layer(0);        // CAMetalLayer* to present that screen's slice
```

- `display_layout.h` is pure std (no frameworks): `virtual_bounds`, `normalized_subrect`,
  `primary_index` — fully deterministic, unit-tested.
- `MultiDisplay` creates all-Spaces, screen-saver-level borderless NSWindows each backed by a
  CAMetalLayer (BGRA8). Its live test is gated (`AAA_DISPLAY_LIVE=1` + ≥2 screens + WindowServer).
- **`SubRectPresenter`** (`display_present_mac.{h,mm}`) is the per-aux-window blit core: it renders
  a `normalized_subrect` of a shared source `MTLTexture` into a destination (an aux drawable's
  texture) via an inline-MSL fullscreen-quad pass that scales to the aux display's resolution. It
  needs no `.metallib` asset (compiles MSL from a source string) and no engine/MetalBackend link,
  so it is unit-tested headless by rendering into an offscreen texture and reading the bytes back.
- **Host integration (c153):** wired into `src/ui/macos/AAASeedMTKView.mm` behind the opt-in
  `AAASEED_MULTIDISPLAY` env var. When enabled with ≥2 screens it sets `framebufferOnly=NO` and,
  after `present_window`, mirrors each aux display's `normalized_subrect` of the primary frame into
  that screen's `CAMetalLayer` drawable via `SubRectPresenter::present_to_drawable`. Default
  behaviour (env unset) is byte-identical — verified: the runtime renders cleanly in both default
  and (graceful-no-op) enabled mode on a 1-display machine.
- **Remaining (tracked in `second_todo.md` §8):** live end-to-end verification on a **≥2-display
  rig** (not available on the dev machine). Semantics are *mirror* (aux shows its sub-rect of the
  primary frame); *true span* (full virtual-canvas-resolution render) is a later engine change.

## Running the native tests

```bash
cmake --preset macos-arm64-debug
cmake --build --preset macos-arm64-debug
ctest --preset macos-arm64-debug -L unit          # includes audio/video/clipboard/midi/net
ctest --preset macos-arm64-debug -L integration   # includes syphon directory discovery
```

Labels follow the [first-label-only CTest quirk](memory-doctrine.md): the category
(`unit`/`integration`) is first; the subsystem is the second label (kept for grep, not `-L`).
