-- AAASeed Lua API for the SHADING class.
-- Defined in M:/AAA/AAASeed/AAAKernel/lua/SHADING.lua, loaded at boot from default.lua_master.lua.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- SHADING wraps the engine's C shader object (vertex / geometry / fragment / compute pipeline).
-- It exposes the four shader stages, their named uniforms, source-file editing, reload triggers, and
-- a save flag per uniform. The class is declared via CLASS.DECLARE (see lua_aaaseed_GABU_OBJ for CLASS).
-- The bulk of the API is uniform read/write per stage; the consecutive-write family alone defines
-- hundreds of methods built from a regular naming pattern, documented as a single section below.


-- # SHADING

-- ## NOTATION

-- This file uses uppercase placeholders to describe families of methods that share a naming pattern.
-- Substitute them mentally to get the actual method name:
--   STAGE  in  vert / geom / frag / comp           (the four shader stages)
--   TYPE   in  float / int / vec4                  (the three uniform types)
--   AXIS   in  x / y / z / w                       (vec4 component, in this order)
-- Examples:
--   self:set_STAGE_float( id, val )       means set_vert_float, set_geom_float, set_frag_float, set_comp_float
--   self:set_STAGE_vec4_AXIS( id, v )     means 16 methods: set_vert_vec4_x, ..., set_comp_vec4_w
-- When a family is restricted to a subset (e.g. only vert/frag/comp), the prose says so explicitly.

-- ## OVERVIEW

-- Id numbering convention: on the Lua side (and on the C shading object's params), uniform ids
-- start at 1, matching Lua's array convention. So self:set_STAGE_float( 1, v ) writes the first
-- float slot. Inside the GLSL shader, the corresponding arrays (aaa_vu_float, aaa_fu_vec4, etc.)
-- are indexed from 0 as in C, so Lua id N maps to GLSL index N-1.
--
-- A SHADING instance carries a self.ref table pointing at the C shader object plus a per-stage
-- sub-table (self.vert, self.geom, self.frag, self.comp) holding the param refs for each uniform
-- slot:
--   self.ref.obj            -- the C shader object
--   self.ref.active         -- bool param
--   self.ref.valid          -- bool param
--   self.ref.reload_all     -- trig param
--   self.ref.comp_active    -- bool param (compute stage on/off)
--   self.STAGE              -- per-stage table { reload, bind, fname, valid, float[], int[], vec4[] }
--   self.frag.src / .out / .auto    -- extra fragment-only refs (full source, output, mouse_time_auto)
--
-- Stage uniform counts (max id for the typed accessors) are class-level tables:
--   SHADING.vert_nb = { float = 8,  int = 4, vec4 = 12 }    -- aliases SHADING.vertex_nb
--   SHADING.geom_nb = { float = 4,  int = 4, vec4 = 4  }    -- aliases SHADING.geometry_nb
--   SHADING.frag_nb = { float = 24, int = 4, vec4 = 8  }    -- aliases SHADING.fragment_nb
--   SHADING.comp_nb = { float = 24, int = 4, vec4 = 8  }    -- aliases SHADING.compute_nb

-- ## CREATION

-- create wraps GABU_OBJ.create_instance_with_obj (errors if obj_ref is nil) then calls set_obj()
-- which builds all per-stage ref tables. set_obj is also called manually if you swap the underlying
-- C object.

	self = SHADING:create( name, obj_ref )
	ref = self:set_obj( obj )								-- (re)build all per-stage refs from obj

-- ## STATE

-- is_active reads the "active" param. is_valid reads "valid" plus, if compute is enabled, the
-- compute stage's own valid flag.

	b = self:is_active()
	b = self:is_valid()										-- valid AND (not comp_active OR comp.valid)

-- ## RELOAD AND EDIT

-- Triggers reload of one stage or all stages. edit_shader opens the .vert/.geom/.frag/.comp source
-- file in the registered OS editor; edit_shader(nil) opens all four. trig_reload_include is
-- class-level (static): it reloads every shader's #include files via the global shader_master
-- object cached at SHADING.master.ref.

	self:trig_reload()										-- reload all 4 stages (reload_all trig)
	self:trig_reload_STAGE()								-- 4 methods: trig reload of one stage
	self:edit_shader( [STAGE] )								-- STAGE in "vert"/"geom"/"frag"/"comp" or nil for all
	self:edit_shader_STAGE()								-- 4 methods: edit one stage
	str = self:get_fname_STAGE()							-- 4 methods: currently used source filename
	SHADING.trig_reload_include()							-- static: reload all shaders #include files

-- ## REFS

-- Direct typed ref accessors. id is the LOGICAL uniform id (1..n_max for the stage/type combo).
-- For vec4, two flavours coexist: get_ref_STAGE_vec4_AXIS(id) returns a single-component ref,
-- while get_ref_STAGE_vec4(id, idb) takes a 1..4 component index (1=x, 2=y, 3=z, 4=w).
--
-- The fragment stage exposes three extra params: full source, output, and the mouse_time_auto flag.

	r = self:get_ref_STAGE_float( id )						-- 4 methods
	r = self:get_ref_STAGE_int( id )						-- 4 methods
	r = self:get_ref_STAGE_vec4_AXIS( id )					-- 16 methods (4 stages * 4 axes)
	r = self:get_ref_STAGE_vec4( id, idb )					-- 4 methods, idb in 1..4

	r = self:get_ref_frag_src()								-- full fragment source param (used by hot-edit)
	r = self:get_ref_frag_out()								-- fragment output ref
	r = self:get_ref_frag_auto()							-- mouse_time_auto flag

-- ## INTEGER AND FLOAT

-- One-slot read/write for float and int uniforms.

	self:set_STAGE_float( id, val )							-- 4 methods
	v = self:get_STAGE_float( id )							-- 4 methods
	self:set_STAGE_int( id, val )							-- 4 methods
	v = self:get_STAGE_int( id )							-- 4 methods

-- ## VECTOR (vec4)

-- Whole-vec4 write plus per-component accessors. set_STAGE_vec4(id, x, y, z, w) writes 1 to 4
-- components depending on how many args are non-nil (x only, x+y, x+y+z, x+y+z+w). Helpers below
-- build common patterns: cos_sin packs angle pairs in turn-units, tex_size packs size + 1/size.
-- There is no whole-vec4 getter; read components individually via the AXIS accessors.

	self:set_STAGE_vec4( id, x, y, z, w )					-- 4 methods; trailing nil args allowed
	self:set_STAGE_vec4_AXIS( id, v )						-- 16 methods
	v = self:get_STAGE_vec4_AXIS( id )						-- 16 methods
	self:clear_STAGE_vec4( id )								-- 4 methods: write (0, 0, 0, 0)
	self:set_STAGE_vec4_cos_sin( id, a1 [, a2] )			-- 4 methods; a2 nil -> (cos a1, sin a1, 1, 0). Angles in turn units.
	self:set_STAGE_vec4_tex_size( id, sx, sy )				-- 4 methods: writes (sx, sy, 1/sx, 1/sy)

-- ## CONSECUTIVE WRITE

-- A family of methods writes a run of N consecutive values starting at slot START within a stage's
-- float or int uniform array. Naming is regular:
--
--   self:set_STAGE_TYPE_START( v1 )
--   self:set_STAGE_TYPE_START_END( v1, v2, ..., vN )
--
-- where TYPE is float or int (vec4 is not in this family), START is the first slot id, and END is
-- the last slot id (inclusive). N = END - START + 1, capped at 8 for floats and 4 for ints (the
-- underlying param.set_real_<N> primitives go up to 8 / 4). The single-arg form (no _END) is just
-- N=1 at slot START.
--
-- Restrictions:
--   - vert / frag / comp have full coverage for both float and int.
--   - geom only has set_geom_int_START_END single-set per slot (no consecutive int run, no
--     consecutive float family at all).
--
-- The full enumeration of names is mechanical and not listed here; START ranges over the stage's
-- max id for the type, and END ranges over START .. min(START+max_run-1, max_id). A few
-- representative examples:

	self:set_frag_float_1( v1 )								-- write float[1]
	self:set_frag_float_1_2( v1, v2 )						-- write float[1..2]
	self:set_frag_float_1_8( v1, v2, v3, v4, v5, v6, v7, v8 )	-- write float[1..8]
	self:set_vert_float_3_6( v3, v4, v5, v6 )				-- write float[3..6]
	self:set_comp_int_2_4( v2, v3, v4 )						-- write int[2..4]
	self:set_vert_int_4( v4 )								-- write int[4] only

-- A small fragment-only get family also exists for floats starting at slot 1, returning N values:

	v1, ..., vN = self:get_frag_float_1_END()				-- END in 2..8, N = END; also _1 alone returns 1 value
	v = self:get_frag_float_N()								-- N in 2..8, single value

-- ## PARAM SAVE FLAG

-- Toggle the persistence (save) flag on a contiguous range of c_param objects belonging to the
-- underlying c_shading. Each uniform slot of the shading object is a regular AAASeed c_param; the
-- save flag controls whether the param value is serialized with the project. Pass i_beg = nil to
-- start at 1; i_end = nil defaults to the stage's max id. b is the save bool (false = ephemeral).
-- For vec4 the range is in vec4 ids (not the flat 4*id index); the helper internally translates.
-- All return self for chaining.

	self:set_save_STAGE_TYPE( b [, i_beg [, i_end ]] )		-- 12 methods (4 stages * 3 types)

-- ## SPECIAL

-- One-off helper for a specific use-case: write 6 vec3 values from a flat 18-element table into the
-- vertex stage's first 6 vec4 slots (the 4th component is left untouched). Forwards to the C
-- aaa.shading.set_vert_vec4_spe binding for speed.

	self:set_vert_vec3_6_table18( t )						-- t = { x1,y1,z1, x2,y2,z2, ..., x6,y6,z6 }

-- ## MASTER

-- Class-level table holding the global shader_master ref and helpers that act on every shader at
-- once. Rebuilt at class declaration time.

	SHADING.master.ref.obj				-- the global shader_master c_obj
	SHADING.master.ref.include_reload	-- trig param to reload all shader #includes
	SHADING.trig_reload_include()		-- set the trig + force aaa.obj.update on the master

-- ## GENERIC ACCESS

-- String-keyed accessors that select stage and type at call time: sha_type in "vert"/"geom"/
-- "frag"/"comp", val_type in "float"/"int"/"vec4". For vec4 with these calls, id is the FLAT
-- index (1..vec4_max*4), since they index the underlying array directly.

	r = self:get_ref( sha_type, val_type, id )				-- example: self:get_ref( "frag", "int", 3 )
	v = self:get_val( sha_type, val_type, id )
	self:set_val( sha_type, val_type, id, val )
	self:set_save( sha_type, val_type, id, b )				-- toggle the save flag on the param
