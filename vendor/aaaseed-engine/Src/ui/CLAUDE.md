# UI Layer — Events, Dialogs, Menus, Font, Preferences

## Event System (`AAA_event.h`, `event/`)

`c_event` — abstract base for all input events.

```cpp
UINT32  _type;           // 4-char code, e.g. 'KYBD', 'WIND'
UINT32  _sub_type;       // subtype code
INT32   _value_nb;       // how many values are set
INT32   _value[5];       // payload: indices 0–4 are ALL valid
```

**`_value[5]` has 5 elements (indices 0–4).** `set_value_5()` sets all five.
`_value_nb` tracks how many are actually populated for a given event.

Helpers: `set_value_1()` … `set_value_5()`, `set_event_1()` … `set_event_4()`.

Concrete subclasses:
- `c_event_keyboard` — key code, modifiers, x/y at keypress
- `c_event_mouse` — x/y, `mouse::BUTTON` enum, `mouse::STATE` enum
- `c_event_midi` — channel, note, velocity
- `c_event_window` — focus, move, resize, close

**`CREATE_EVENT_TYPE(name, cod4)` macro** — registers the type using the same ODR `b_trick_`
pattern as the factory system (static global bool forces construction at startup).

`c_event::process()` — sends to network (if connected), then calls Lua `EVENT.hook` callback.
`is_to_send()` is virtual per subclass and controls network broadcasting.

## Dialog System (`dialog_wrapper.h`, `dialog_number.h`, `dialog_str.h`)

### `c_dialog_wrapper` (base)
- HWND-based modal/modeless param editor
- Static list: `std::vector<c_dialog_wrapper*> wrappers`, protected by `aaa::MUTEX_RECURSIVE`
- Lua support: `_b_lua_callback`, `_lua_id` — execute Lua on param change

**`wrappers` holds all open dialogs regardless of subclass.** When iterating from a subclass
method, always use `dynamic_cast` to filter — a C-style cast is UB if a different subclass
is present:
```cpp
for( auto const & p : wrappers )
{
    if( auto* p_str = dynamic_cast<c_dialog_wrapper_str*>(p) )
        p_str->some_method();
}
```

### `c_dialog_number` (numeric param editor)
- Edits REAL or INT32 params with a slider/spinner
- `_value_initial` — stored on open for cancel/revert
- **CRITICAL — cancel fall-through**: `IDCANCEL` intentionally falls through to the `IDOK` case.
  It restores the display to `_value_initial`, calls `output_edit()` to commit that initial value
  (resetting any live preview), then closes with `do_end(0, "cancel")`.
  `IDC_REVERT` (stay-open reset) does NOT fall through.
  **Do NOT add a `break` between `IDCANCEL` and `IDOK`.**

### `c_dialog_str` (string/list param editor)
- Text field or dropdown/listbox for file paths, symbolic enums
- Drag/drop reordering: `_drag_index_start`, `_drag_index`
- Selection tracking: `_sel_begin`, `_sel_end`, `_sel_prev_begin`, `_sel_prev_end`

## Input State

### Keyboard modifier state (`keyboard.h`)
`modifier` — static class:
- `flags` (INT32), `b_double_alt`
- `make()`, `is_ctrl_on()`, `is_shift_on()`, `is_alt_on()`, `is_none()`
- `update()` — polls current keyboard state
- `is_double_alt()` — Alt-Alt sequence detection

### `c_keyboard`
- `KEYBOARD_COUNTRY` enum: `FRANCE`, `UK`, `MAC_FR`, `MAC_US`, `CUSTOM`
- Numpad flying mode: `b_numpad_flying`, `b_numpad_flying_sliding`
- Key routing flags: `b_alphabet_for_trax/ui`, `b_ascii_for_trax`, `b_key_special_for_trax`
- Captured keys: `key_alphabet`, `key_ascii`, `key_special`, `key_double_alt`

### `c_mouse`
- Static: `sensitivity`, `multiplier`, `wheel_factor`, `b_grabbed`, `cur`, `time_last_move`
- Instance: `_x_pixel`, `_y_pixel`, `_b_but_state[]`, `_but_time_down[]`, `_but_time_up[]`
- `set_grabbed()` — confines cursor to window

## Menus (`aaa_menu.h/cpp`, `seedmenu.h/cpp`)

`menu` namespace:
- `create(callback)` — new menu from `PT_MENU_FN`
- `insert/add/change/remove_item()` — with value and check state
- `insert/add_separator()`
- `insert/add/change_menu_sub()` — submenus
- `attach()` / `detach()` / `detach_all()` — bind/unbind to mouse buttons
- `b_allow_ui` — enable/disable all menus globally

## Font / Text (`alphabet.h/cpp`, `alphabet_maa.h`, `alphabet_roman.h`, `font.h`)

`aaa::alphabet` namespace with two styles:
- `aaa::alphabet::maa` — modern bitmap font (X_SIZE=0.8, X_SPACE=0.4)
- `aaa::alphabet::roman` — serif font (Y_SPACE=0.2)

**`COLOR_TYPE` enum** — 15 types: `ITEM`, `TITLE`, `INFO`, `ERROR`, `ON/OFF`, `NODE` (4 levels), unused variants.
Static arrays: `alphabet_color[COLOR_TYPE_MAX_NB][4]`, `color_char[4]`.

Draw API: `draw_str_init()`, `draw_str_begin()`, `draw_str_end()`, `draw_cr()`.
Low-level: `GOL::make_raster_font()`, `GOL::draw_string()`.

## 2D Overlay (`flatland.h/cpp`, `flatland_lua.h/cpp`)

`c_flatland` — singleton HUD renderer (`c_obj_ui`, static `cur`).
Static boolean flags (all have `_lua` variants for Lua access):
`b_draw_focus`, `b_draw_info`, `b_draw_curve`, `b_draw_fps`, `b_draw_frame`,
`b_draw_timecode`, `b_draw_mess`, `b_draw_clock`, `b_draw_logo`

Viewports: `viewport_focus`, `viewport_info`, `viewport_curve` — lazy-created.
Rendering params: `ui_line_width`, `b_ui_gradient`, `b_ui_blend`, background colors/regions.
Font sizing: `char_size_x_asked`, `char_size_y_asked`, `mess_char_size_factor_x/y`.

## Preferences (`pref.h/cpp`, `pref_start.h/cpp`)

`c_pref` — singleton preference object (`c_obj_ui`, static `cur`).
- `_b_shift_out`, `_b_ctrl_out`, `_b_alt_out` — keyboard output states
- `init_masters()`, `load_masters()`, `save_masters()`

## Symbolic Strings (`strsymbo.h/cpp`)

`gstr` — static string arrays for symbolic param enums:
- `xyz[3]`, `rot_order[6]`, `uva[3]`, `dim[4]`, `dim_prim[3]`
- `current[2]`, `borrow[3]`, `borrow_once[4]`
- `p2[17]` (powers of 2), `hint[3]`, `test_func[8]`, `draw_number[8]`, `front_and_back[3]`
- Macros: `PARAM_DEF_AXE_X/Y/Z()` — axis param definitions

## Value Storage (`obj_value.h/cpp`)

`c_obj_value` — generic 32×4 value block (`ITEM_NB=32`, `GROUP_NB=4`, `_values[128]`).
- `get_from_index()` / `set_to_index()` — modulo-wrapped access
- `get_sort_index_from_min/max()` — min/max search (used by animation tracks)

## File Dialogs (`file_dlg.h`)

`aaa::file` namespace: `get_filename_open()`, `get_filename_save()`, `get_folder()`.

## UI Master Control (`ui_master.h`)

`master` namespace — `NO_ALLOW_FORCE` enum (NO=0, ALLOW=1, FORCE=2).
`process_master_switch(NO_ALLOW_FORCE, bool b)` → logic: `ALLOW ? b : (FORCE ? true : false)`.
