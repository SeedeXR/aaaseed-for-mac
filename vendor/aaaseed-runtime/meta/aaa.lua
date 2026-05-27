---@meta
--
-- LSP meta file declaring the AAASeed extensions on top of the standard
-- Lua libraries (math, table, string, ...). Loaded via Lua.workspace.library
-- in `.luarc.json` so the lua-language-server stops flagging these as
-- `undefined-field`.
--
-- Only the EXTENSIONS are declared here; the standard fields stay covered by
-- the built-in stdlib metadata.

-- math extensions
math.pi2				= 0		-- 2 * pi (TAU)
math.pi2_over			= 0		-- 1 / (2*pi)
math.pi_over			= 0		-- 1 / pi

function math.bias(...) end
function math.gain(...) end
function math.gain_bias(...) end
function math.hws(...) end
function math.is_nan(...) end
function math.get_turbulence(...) end
function math.convert_hilbert_xy_to_d(...) end
function math.dist_v(...) end
function math.do_bezier_xyz(...) end
function math.do_catmull_rom(...) end
function math.do_catmull_rom_xyz(...) end
function math.fn_linear(...) end
function math.fn_linear_all(...) end
function math.get_angle(...) end
function math.get_angle_01(...) end
function math.get_length(...) end
function math.get_length_squared(...) end
function math.dist_v2r(...) end
function math.random_centered(...) end
function math.random_real(...) end
function math.rotate_ab_rad(...) end
function math.rotate_ab_turn(...) end
-- gauss, randomf, randomf_pow, step_linear, step_interval_linear,
-- step_interval_linear_table are defined in Lua (LINE_STRIP, PHASOR,
-- GaBu_Util/MATH/math.lua) so no stubs are needed here.

-- table extensions (debug + functional helpers commonly added by aaa_table)
function table.print(...) end
function table.show(...) end
---@return table
function table.copy(...) end
---@return table
function table.copy_simple(...) end
---@return table
function table.copy_simple_check(...) end
---@return table
function table.copy_shallow(...) end
---@return table
function table.copy_deep(...) end
---@return table
function table.deepcopy(...) end
---@return integer
function table.count_item(...) end
function table.append(...) end
function table.clear(...) end
---@return table
function table.create(...) end
function table.apply_fn(...) end
function table.apply_method(...) end
---@return table
function table.build_array_with_unique_value(...) end
---@return table
function table.build_table_header(...) end
---@return table
function table.from_csv(...) end
function table.find_key_by_val(...) end
function table.is_empty(...) end

-- string extensions (added by aaa_string)
function string.enforce_trailing_slash(...) end
function string.is_trailing_slash(...) end
function string.is_trailing_slash_dialog(...) end
function string.remove_trailing_slash(...) end
function string.wrap(...) end
function string.split(...) end
function string.make_from_list(...) end

-- C-injected `param` namespace, the AAASeed parameter access API.
-- Overloaded: param.set(ref, val) or param.set(obj, name, val), same for get.
param = param or {}
function param.set(...) end
function param.set_save(...) end
function param.set_and_save(...) end
function param.set_and_save_no(...) end
function param.get(...) end
function param.get_bool(...) end
function param.get_int(...) end
function param.get_ref(...) end
function param.get_value_def(...) end
function param.is_valid(...) end
