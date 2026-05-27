# GaBu_Util — Standalone utility classes

Utility classes with no UI dependencies. Usable from MEU scripts and APP code.

## Runtime loading mechanism

GaBu_Util is one of the two GaBu folders executed **every frame** by the engine (the other is `GaBu_BU/`). The execution order within GaBu_Util is determined by `default.layers_bind`.

Each subfolder follows the same pattern:
- `<NAME>.lua` : the Lua class source
- `fx.aaa_layers_all` : a minimal C++ layer containing a `lua_wrap` object that loads the `.lua`

The `lua_wrap` settings are always:
```
doit = ON ;
run_only_when_compiled = ON ;
skip_rest = ON ;
filename = "<NAME>.lua" ;
```

`default.layers_bind` maps bind indices (Bind_00 through Bind_68, with gaps) to `<FOLDER>/fx`. `default.layerss_param` declares the module as `gabu_util`, active, covering layers 15 to 68. The engine walks the bind list in index order each frame, so classes loaded early (MATH at Bind_00, QUATERNION at Bind_01) are available to all later classes.

`default.cam` holds the default camera for the module (ortho, not usually relevant to util classes).

## Class convention

Every class uses `CLASS.DECLARE( "NAME" )`, which derives from `GABU_OBJ`. The standard constructor is `NAME:create(...)`. Instance methods use `self:method()` syntax. Some files declare multiple related classes (e.g. `PHASOR.lua` declares PHASOR, PHASOR_A, PHASOR_ASR). A few files define free functions instead of classes (MATH, LINE, FNAME).

## Class catalog

### Core data structures

| Directory | Class(es) | Description |
|---|---|---|
| `STACK/` | `STACK` | LIFO stack. `push(v)`, `pop()`, `get_top()`, `set_top(v)`, `get_size()`, `get_from_bottom(i)`, `erase()`, `set_size_min(n)`. Used by BUS and APP for push/pop context. |
| `QUEUE/` | `QUEUE` | High-performance circular-buffer FIFO. `push(v)`, `pop()`, `get_front(i)`, `get_first()`, `get_last()`, `get_nb()`, `is_empty()`, `reserve(n)`, `clear()`, `insert(i,v)`. Auto-grows on push. O(1) push/pop. |
| `MATH/` | (free functions) | `math.step_linear(val,a,b)`, `math.step_interval_linear(val,a,b,c,d)`, `math.step_interval_linear_table(val,tab)`. Extensions beyond `aaa_math.lua`. |

### Media and rendering

| Directory | Class(es) | Description |
|---|---|---|
| `FBO/` | `FBO` | Wraps the C++ FBO object. `create(name, fbo, layers)`. Pixel size, channel config (nb, type), attachments 1..8 + `"depth"` (active, bind, tex_on_cpu, mipmap_generate, channel_type). `get_bdd_clear_screen()` returns a `BDD_CLEAR_SCREEN`. `update()`, `draw()`, `set_focus()`, `is_valid()`, `is_attach()`. |
| `IMG/` | `IMG`, `IMGS` | `IMG`: image handle (bind + dimensions). Draw methods: `draw_sxy`, `draw_sx`, `draw_sy`, `draw_xy_sxy`, `draw_xyz_sxy`, `draw_xyz_sxy_rotz`, `draw_lb_sxy`, `draw_lt_sxy`. Aspect ratios via `get_rx/ry()`. Crop support. Multi-language versions via `switch_version(la)`. `IMGS`: global singleton managing bind allocation and image loading. `get_bind(fname)`, `get_img(fname)`, `get_img_from_bind(bind)`, `assign_bind(fname)`. Bind allocation starts at `BIND_FREE_START = 1024`. |
| `IMG_SEQ/` | `IMG_SEQ`, `IMG_SEQ_PLAYER` | `IMG_SEQ`: numbered image sequence. Load from files (`load`), directory (`load_from_dir`), or array. Access by index, random (`get_bind_rnd`), or cycle phase (`get_bind_in_cycle`, `get_bind_in_cycle_triangle`). `IMG_SEQ_PLAYER`: playback wrapper with fps, loop, reverse, time/phase seeking. |
| `IMG_READER/` | `IMG_READER` | Async image loading. |
| `VIDEO/` | `VIDEO` | Video file playback. Transport: `start`, `stop`, `play`, `toggle`, `restart`, `set_time`, `set_time_cano`, `set_loop`, `set_time_factor`. Scrubbing: `scrub(t)`, `scrub_cano(v)`. Audio: `set_sound(vol,pan)`, `set_volume(v)`, `set_pan(v)`. Capture: `set_capture_open_run(b)`, `restart_capture()`. Luma keying: `set_luma(b,min,max,inv)`. Multi-language via `switch_version(la)`. |
| `VIDEOS/` | `VIDEOS` | Video pool manager. `get_new(fname)`, `get(fname)`, `find(fname)`, `get_sound(fname)`. Bind range: `BIND_VIDEO_START=512`, `BIND_VIDEO_NB=128`. Directory management via `set_dir`, `set_dir_media`. |
| `TEXT/` | `TEXT` | Text rendering. Font types: line (MAA/GLUT), texture, buffer, outline, polygon, extrude, SDF. Config: `set_font(v)`, `set_face_size(v)`, `set_face_dpi(v)`, `set_justification(v)`, `set_alignment(v)`, `set_alignment_vert(v)`, `set_interligne(v)`, `set_line_length(v)`. Color: `set_color(r,g,b,a)`, `set_color_sdf(r,g,b,a)`. Metrics: `get_width()`, `get_height()`. |
| `FBO_AND_OUT/` | `FBO_AND_OUT` | Combines an FBO with an output path. |
| `BDD_CLEAR_SCREEN/` | `BDD_CLEAR_SCREEN` | Screen-clear rendering object used by FBO. |

### Animation and timing

| Directory | Class(es) | Description |
|---|---|---|
| `PHASOR/` | `PHASOR`, `PHASOR_A`, `PHASOR_ASR` | Envelope generators with random timing. `PHASOR_A` (attack-only): cycles wait then attack, phase in [0,1] during attack. `PHASOR_ASR`: wait, attack, sustain, release cycle. Both return `phase, id_rnd [, b_done]` from `set_time(t)`. Configurable ranges: `set_wait_range`, `set_attack_range`, etc. Random id pool via `set_id_rnd_nb(n)`. |
| `TIMER/` | `TIMER` | Performance profiling. `start()`, `store()`, `get_now()`, `get_ms_str()`, `get_micro_sec_str()`. |
| `SEQ/` | `SEQ` | Sequencer unit. Duration, phase [0,1], time tracking. Ease-in support. Playlist integration (`add_playlist`, `update_playlist`). Module usage control (`add_mu_used_by_name`, `enable_mu_used`, `disable_mu_used`). Callback attachment via `set_methods(target, method_update, method_active)`. |
| `SEQS/` | `SEQS` | Sequence manager. Creates and navigates SEQ collections. Playback: `begin`, `run`, `flip`, `restart`. Navigation: `set_seq(id_or_name)`, `inc_seq`, `dec_seq`. Looping, preroll, time-factor support. UI builders: `define_buttons`, `define_bus_seqs`, `define_window_seqs`. |
| `CHANGER/` | `CHANGER`, `TCHANGER`, `CHANGERS` | Value interpolation over time. `CHANGER`: interpolates a value from start to stop over time_len, calling `target:method(v)`. Easing via `set_gain_bias`. `TCHANGER`: trigger (fires a method at a time offset). `CHANGERS`: queue of CHANGER/TCHANGER, auto-removes completed ones. |

### Geometry and drawing

| Directory | Class(es) | Description |
|---|---|---|
| `POINT_LIST/` | `POINT_LIST`, `SEGMENT_LIST` | Point cloud. Add/get points (xyz, xy). Geometry: distances, barycenter, min/max. Transforms: `mul_xyz`, `add_xyz`, `force_z`, `mul_add`. Drawing: `draw_lines_from_points`, `draw_lines_begin_end`, `draw_lines_random_with_method`. Processing: `process_points_with_method/fn`. Catmull-Rom curve generation. CSV save with colors. |
| `LINE/` | (free functions) | `aaa.line.draw_segment(x0,y0,x0n,y0n,x1,y1,x1n,y1n)`, `aaa.line.draw(table,size,b_loop)`. |
| `LINE_STRIP/` | `LINE_STRIP`, `LINE_STRIP_TABLE` | `LINE_STRIP`: indexed strip with open/closed flag. Curve conversion: `convert_to_catmull_rom`, `convert_to_bezier`, `convert_to_bezier_by3`, `convert_to_midpoint`. Normal offset/deform. Decimation. `LINE_STRIP_TABLE`: collection of strips sharing a POINT_LIST. Drawing: `draw`, `draw_point`, `draw_point_colored`, `draw_order`. Sorting: `sort_xy`, `sort_hilbert`. Random/Gaussian decimation. |
| `SPACE/` | `SPACE`, `SPACE_BEAR` | 3D space container. Navigation: `enter()`, `quit()`. Hooks: `define_ui`, `init_buildings`, `update_buildings`, `draw`. BUS attachment. SPACE_BEAR is an example subclass. |
| `PLOT/` | `PLOT`, `TBUF`, `SCOPE`, `SCOPE_3D` | `PLOT`: AxiDraw plotter control. Pen up/down, movement in step/aaa/canonical coords, command queue, simulation mode. Three coordinate systems with conversions. `TBUF`: time-windowed buffer (extends QUEUE). `SCOPE`: 1D oscilloscope (value + velocity + acceleration traces). `SCOPE_3D`: 3D point trace with optional textured ribbon. |
| `QUATERNION/` | `QUATERNION` | 3D rotation quaternion. `create(w,x,y,z)`, `make_rotate(from,to)`, `rotation_arc`, `arcball`. Operations: `multiplyRight`, `normalize`, `scale`, `slerp`. Conversion: `matrix_transform` (to 4x4), `get_euler`. |

### Input and interaction

| Directory | Class(es) | Description |
|---|---|---|
| `KEYBOARD/` | (keyboard) | Additional keyboard handling beyond `aaa_keyboard.lua`. |
| `OSC/` | (OSC) | Open Sound Control message helpers. |
| `OSC_MESS/` | `OSC_MESS` | OSC message builder. |
| `NOVATION/` | (Novation) | Novation Launchpad / Launch Control MIDI controller support. |

### Data, I/O, and formatting

| Directory | Class(es) | Description |
|---|---|---|
| `DATAGRID/` | `DATAGRID` | 2D data table backed by a BDD object. `read` loads data, access via `get_data`, `get_field_max/min`, `get_field_all_value`, `get_field_at_id`. Interpolation lookup: `get_id_prev_next_inter(key,val)`. |
| `LOG/` | `LOG` | File logger. `add(str,...)`, `add_error(str,...)`, `add_alive(str,...)`. Periodic heartbeat via `update()`. Read/write to file. Uses QUEUE internally. |
| `FNAME/` | (free function) | `FNAME.split_ext(fname)` returns name, ext (without dot). |
| `LANG/` | `LANG` | Language/localization support. |
| `Media/` | `MEDIA` | Media path resolution and directory management. |
| `PLAYLIST/` | `PLAYLIST` | Media playlist management. |
| `SNDS_PLAYER/` | `SNDS_PLAYER` | Sound playback helper. |
| `SMTP/` | `SMTP` | Email sending (plain SMTP). |
| `SMTPS/` | `SMTPS` | Email sending (SMTP over TLS). |

### Visual utilities

| Directory | Class(es) | Description |
|---|---|---|
| `COLOR_REF/` | `COLOR_REF` | Wraps color parameters (RGB/RGBA/HSV) from a C++ object. `create(name, obj_ref, param_prefix)`. HSV mode toggle via `set_hsv_mode(b)`. |
| `PIXEL_SIZE/` | `PIXEL_SIZE` | Resolution presets. `get_sxy(id_or_str)` returns sx,sy. `get_ui_str()` for BU menus. Presets: 512, VGA(640x480), PAL(767x575), XGA(1024x768), SXGA(1280x1024), FWXGA(1366x768), 720p(1280x720), HD(1920x1080), WUXGA(1920x1200), WQHD(2560x1440), 2160p(3840x2160), 4320p(7680x4320), plus custom (DPool, LVFloor, LVStageLed). |
| `FX/` | `FX` | Single effect wrapper. Wraps a MEU-like C++ object tree. `create(name, obj, b_active)`. Window/UI creation: `create_window`, `add_ui_window`. `set_bind_2d(bind)` for output. `draw()`, `update()` hooks. |
| `FXS/` | `FXS` | Collection of FX objects. |
| `MARKS/` | `MARKS` | Annotation marks for visual overlay. |
| `SCOPE/` | `SCOPE` | (see PLOT/ section, SCOPE is declared in PLOT.lua) |

### Simulation and 3D

| Directory | Class(es) | Description |
|---|---|---|
| `BOID/` | `BOID` | Boid/flocking simulation. Forces: Box, Target, Repulse, Flocking, Steering, Field, Repulse_By_Other. `create(name, obj)`, `build_all_ref()`, `trig_restart()`. |
| `FACE_INFO/` | `FACE_INFO` | Face/mesh data extraction. |
| `PUPPET_BODY/` | `PUPPET_BODY` | Puppet skeleton body. |
| `PUPPET_PART/` | `PUPPET_PART` | Puppet skeleton part/limb. |
| `GridRef/` | `GridRef` | Grid reference management. |
| `BDD_FBX/` | `BDD_FBX` | FBX geometry import helper. |
| `Pixar/` | `PIXAR` | Pixar USD/format integration. |

### Misc

| Directory | Class(es) | Description |
|---|---|---|
| `param_util/` | (param utilities) | Additional param read/write helpers beyond `aaa_param.lua`. |
| `PC/` | `PC` | PC-specific platform utilities. |
| `LV/` | `LV` | LV (LED Video) display support. |
| `VM_IOS/` | `VM_IOS` | iOS virtual machine bridge (legacy). |
| `MAA_EB/` | `MAA_EB` | Maa's EB (internal tool integration). |

## Bind loading order (from default.layers_bind)

The order matters for dependencies (earlier classes are available to later ones):

```
00: MATH          01: QUATERNION       03: param_util    04: STACK
05: QUEUE         06: CHANGER          07: PHASOR        09: COLOR_REF
10: BDD_CLEAR_SCREEN  11: PIXEL_SIZE   12: FBO           13: DATAGRID
14: BDD_FBX       15: BOID             17: GridRef       18: LANG
19: KEYBOARD      20: PC               21: LOG           22: TEXT
24: MARKS         26: FNAME            27: Media         29: IMG
30: IMG_SEQ       31: IMG_READER       33: VIDEO         34: VIDEOS
35: PLAYLIST      36: SNDS_PLAYER      37: SMTP          38: SMTPS
40: FBO_AND_OUT   41: MAA_EB           43: PIXAR         44: POINT_LIST
45: LINE_STRIP    46: PLOT             47: TIMER         48: SCOPE
50: OSC           51: OSC_MESS         53: SPACE         54: FX
55: FXS           57: SEQ              58: SEQS          59: FACE_INFO
61: NOVATION      63: VM_IOS           66: LV            67: PUPPET_PART
68: PUPPET_BODY
```

Gaps (02, 08, 16, 23, 25, 28, 32, 39, 42, 49, 52, 56, 60, 62, 64, 65) are reserved for future classes.

## Usage pattern

```lua
-- Create an FBO in MEU:init()
local fbo = FBO:create( "my_fbo", fbo_obj_ref )
fbo:set_pixel_size( 512, 512 )
fbo:set_channel_nb_type( 4, 3 )    -- 4ch fp16

-- Use PHASOR_A in MEU:update()
local ph = PHASOR_A:create( "ph", 0.5, 1.5, 0.2, 0.8 )
local phase = ph:inc_time( aaa.time.dt )
-- phase drives animation

-- TIMER for profiling
local t = TIMER:create( "perf" )
t:start()
-- ... expensive code ...
t:store()
aaa.print( t:get_ms_str() )

-- Draw an image
local img = IMGS.get_img( "my_texture.png" )
img:draw_sxy( 2.0, 1.5 )

-- Video playback
local vid = VIDEOS:get_new( "clip.mp4" )
vid:start()
vid:set_loop( true )

-- Value interpolation
local changers = CHANGERS:create( "fx" )
changers:add( 0.0, 1.0, my_obj, "set_alpha", 0, 1 )   -- fade in over 1s
changers:change()                                        -- call in update()

-- Oscilloscope
local scope = SCOPE:create( "debug" )
scope:add( some_value )
scope:draw( 4, 2 )
```

## Documentation generation

The existing `AAADoc/tools/lua_to_md.pl` pipeline can generate API reference docs for GaBu_Util classes. For each class worth documenting, create a `lua_aaaseed_<CLASS>.lua` file in `AAADoc/` following the same convention as `lua_aaaseed_GABU_OBJ.lua`:

- Use `-- #` / `-- ##` / `-- ###` section markers
- All entries are pure Lua (no `-- lua` tag needed, same as GABU_OBJ/SHADING docs)
- Add the new file to `AAADoc/tools/regen_all.sh`
- Run `regen_all.sh` to produce the `.md`

Priority candidates for documentation: FBO, IMG/IMGS, VIDEO/VIDEOS, TEXT, PHASOR, SEQ/SEQS, POINT_LIST, LINE_STRIP, CHANGER, QUEUE, STACK.
