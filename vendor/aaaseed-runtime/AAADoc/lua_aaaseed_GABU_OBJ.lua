-- AAASeed Lua API for the GABU_OBJ base class and its CLASS class registry.
-- Defined in M:/AAA/AAASeed/AAAKernel/lua/GABU_OBJ.lua, loaded at boot from default.lua_master.lua.
--
-- All entries in this file are pure Lua (no C bindings).
--
-- GABU_OBJ is the base class for almost every Lua object in the GaBu and GaBuZoMeu environments.
-- It provides naming, parent/child tree management, printing helpers, dialog (box) helpers,
-- function/method dispatch tables, an active flag, AAASeed object passthrough, and unit testing.
-- All higher-level GaBu classes are declared with CLASS.DECLARE which derives from GABU_OBJ by default.


-- # CLASS

-- CLASS is a free-standing registry table (not a class itself) used to declare new GABU_OBJ subclasses.
-- DECLARE wraps the loop framework's oo.class() with bookkeeping: it registers the class by name in
-- CLASS.__classes_by_name, links it to its super class, exposes it as a Lua global, and seeds a
-- per-class info table accessible through CLASS.__str_class_field (the "__gabu" field on instances).

-- ## DECLARE

-- The single entry point to create a new class. Pass the class name as a string; super defaults to
-- GABU_OBJ; def_table is an optional pre-built definition table (fields and methods become class
-- members). DECLARE returns the new class on success, or nil if a global of the same name already
-- exists, in which case the caller must skip re-declaration. Typical idiom:
--   if CLASS.DECLARE( "MY_CLASS" ) then
--       MY_CLASS:set_class_status_doc( CLASS.STATUS.CORE, "one-liner about MY_CLASS" )
--       function MY_CLASS:my_method( ... ) ... end
--   end

	class_or_nil = CLASS.DECLARE( class_name [, super [, def_table ]] )

-- ## STATUS

-- Symbolic constants for set_class_status_doc(). Used to tag classes by lifecycle / role.
-- Each value is a lowercase string; the constants exist to keep usage spelled consistently.

	CLASS.STATUS.GABU			-- "gabu"          base infrastructure (GABU_OBJ itself)
	CLASS.STATUS.GABUZOMEU		-- "gabuzomeu"     part of the GaBuZoMeu UI framework
	CLASS.STATUS.CORE			-- "core"          stable engine wrapper
	CLASS.STATUS.APP			-- "app"           an APP-level class
	CLASS.STATUS.EXPERIMENTAL	-- "experimental"  in flux, may change or be removed
	CLASS.STATUS.MATH			-- "math"          math primitive
	CLASS.STATUS.TUTORIAL		-- "tutorial"      tutorial-only class
	CLASS.STATUS.UNUSED			-- "unused"        kept for reference, no longer wired in

-- ## QUERY

-- Tests for class identity and GABU_OBJ membership.

	b = CLASS.is_class_name( name )			-- true if a class with this name has been DECLAREd
	b = CLASS.is_class( value )				-- true if value is a class (not an instance)
	b = CLASS.is_gabu_obj( obj [, class] )	-- true if obj is a GABU_OBJ; if class given, also checks obj's class matches


-- # GABU_OBJ

-- All methods below are on instances unless marked otherwise. Static methods (called on the class
-- itself) are noted as such in the comment. Methods prefixed with __ are internal and not listed.

-- ## CLASS INTROSPECTION

-- Each instance carries a hidden "__gabu" field (CLASS.__str_class_field) holding class info:
-- class_name, class, super, classes_by_name (children indexed by name), classes_by_class, created_nb,
-- class_doc, class_status, anonymous_count. The methods below read from that table.

	str = self:get_class_name()				-- name of self's class as a string
	cls = self:get_class()					-- the class object
	cls = self:get_class_super()			-- the parent class
	b = self:is_class_name( name )			-- test class name
	b = self:is_class( class )				-- test class identity
	b = self:is_derived_from_class( class )	-- walks up the chain, returns true if class is an ancestor

	t = self:get_classes_by_name()			-- table of direct subclasses keyed by name
	t = self:get_classes_by_class()			-- table of direct subclasses keyed by the class object

	str = self:get_class_hierarchy_str()	-- "GrandParent:Parent:Class" without the "GABU_OBJ:" root
	doc = self:get_doc()					-- the per-instance doc table (self.doc)
	str = self:get_class_doc()				-- multi-line doc set by set_class_status_doc
	str = self:get_class_status()			-- the CLASS.STATUS string set on the class

	id = self:get_gabu_obj_id()				-- unique integer id for this instance (= created_nb at create time)
	n = self:get_created_nb()				-- number of instances ever created from this class
	self:inc_created_nb()					-- internal: increments the counter on self and all parent classes

	self:print_class_info( level )			-- pretty-print "name -- status" + class_doc, indented by level
	self:parse_class( level, level_max, method [, ...] )	-- call self[method]( level, ... ) then recurse into subclasses up to level_max

-- ## NAME AND LABEL

-- Two independent identifier slots on every instance. The lowercase variants are cached on creation
-- so case-insensitive lookups don't pay a re-lowercasing cost on every call. is_name_match() spans both.

-- ### NAME

-- Names are not unique across classes; uniqueness is at the (class, name) level. Use change_name() to
-- rename post-creation. Both __name and __name_lowercase are set by __init_name() at creation.

	str = self:get_name()					-- original-case name
	str = self:get_name_lowercase()			-- cached lowercase
	self:change_name( name )				-- rename, no-op if same name
	b = self:is_name( name )				-- case-insensitive equality
	b = self:is_name_lowercase( name )		-- when caller already has a lowercase name (faster)
	b = self:is_name_match( pat )			-- string.find on lowercase name then on lowercase label

-- ### LABEL

-- Optional secondary identifier set independently of name. is_name_match scans both.

	self:set_label( label )
	str = self:get_label()
	str = self:get_label_lowercase()		-- nil if no label
	b = self:is_label( label )				-- case-insensitive

-- ## CREATION

-- Three factory functions, all called as static methods (CLASS_NAME.create_instance( CLASS_NAME, ... )).
-- create_instance is the basic form; create_instance_no_name auto-names "anonymous_N";
-- create_instance_with_obj enforces that an AAASeed object reference is provided.

	self = CLASS.create_instance( class, name, obj_ref_or_name )			-- obj_ref_or_name optional
	self = CLASS.create_instance_no_name( class, obj_ref_or_name )
	self = CLASS.create_instance_with_obj( class, name, obj_ref_or_name )	-- errors if obj_ref_or_name is nil
	self:free()								-- default raises a box_error; subclasses must override

-- ## ACTIVE

-- Generic boolean flag. set_active() short-circuits if value is unchanged, set_active_direct skips
-- the check (used by callers that already know the value differs). flip_active toggles and returns
-- the new state.

	b = self:is_active()
	self:set_active_direct( b_on )			-- no change-detection
	self = self:set_active( b_on )			-- chainable, only fires set_active_direct on a change
	b = self:flip_active()					-- returns the new state

-- ## AAASEED

-- Bridge to the engine's C objects. When a GABU_OBJ wraps an AAASeed object, that object's reference
-- is cached on self.ref.obj (set by __init_obj_ref at creation). The two sub-sections expose it: C_OBJ
-- for object-level operations, PARAM for named-parameter access on that object.

-- ### C_OBJ

-- Convenience wrappers when self has a self.ref.obj. All silently no-op if no obj is attached, except
-- check_ref which returns nil + a print_error on a malformed input. check_ref accepts either an object
-- name (string) or an obj ref directly.

	obj = self:get_obj()									-- self.ref and self.ref.obj or nil
	self:update()											-- aaa.obj.update on the ref obj
	self:draw()												-- aaa.obj.draw
	self:update_then_draw()
	self:set_focus()										-- aaa.obj.set_focus_ui (yes, even from set_focus)
	self:set_focus_ui()
	self:set_obj_active( b )								-- param.set on the cached ref.obj_active
	ref = self:check_ref( ref_in )							-- resolve ref_in (string name or obj ref) to a ref; nil + print_error on failure

-- ### PARAM

-- Wrap the param.* C bindings against self.ref.obj. get_param_ref caches the ref on self.ref[name]
-- so repeated lookups are free after the first call.

	r = self:get_param_ref( name )
	v = self:get_param( name )
	self:set_param( name, value )
	self:set_param_and_nosave( name, value )				-- set + param.set_save( r, false )
	self:set_param_save( name, b )
	self:do_param_action_open( name )						-- param.do_action_open

-- ## OUTPUT

-- Logging and debugging surface: instance prints, verbosity-gated prints, modal dialogs, and dump
-- helpers. All instance prints prefix the message with a "ClassName(id)|name : " banner so multi-object
-- logs stay legible.

-- ### PRINT

-- error() prints then calls Lua error() (raises). show* delegate to aaa.debug.

	self:print( ... )
	self:print_inverse( ... )				-- highlighted line
	self:print_debug( ... )					-- debug-only stream
	self:print_error( ... )					-- red / error stream
	self:error( ... )						-- print + error()
	self:print_args( ... )					-- "  args : a1, a2, a3" (skipped if no varargs)
	self:show( val, str )					-- delegate to aaa.debug.show
	self:show_error( str, ... )				-- delegate to aaa.debug.show_error
	self:show_error_ui( str )				-- show_error + aaa.bell()

-- ### VERBOSE

-- Each instance has self.verbose (default 0). The verbose_LEVEL() variants only print when
-- self.verbose >= level, and always log the calling function name first via __print_mark.

	self:set_verbose( s )
	self:verbose_0( ... )					-- always prints, with mark
	self:verbose_1( ... )					-- prints only if verbose >= 1
	self:verbose_2( ... )
	self:verbose_3( ... )
	self:verbose_level( level, fn_level, ... )				-- general form
	self:verbose_0_inverse( ... )			-- same family but using print_inverse
	self:verbose_1_inverse( ... )
	self:verbose_2_inverse( ... )
	self:verbose_3_inverse( ... )
	self:verbose_level_inverse( level, fn_level, ... )

-- ### DIALOG (BOX)

-- Open a modal dialog branded with self's identity and the calling method name. While a box is open
-- GABU_OBJ.b_box_on is true (read-only flag for callers that want to suppress duplicate prompts).
-- box_warning is an alias for box_debug. box_dev also dumps the call stack for debugging.

	ret = self:box_good( ... )				-- green/info dialog
	ret = self:box_debug( ... )				-- yellow/warning dialog (alias self:box_warning)
	ret = self:box_error( ... )				-- red/error dialog
	ret = self:box_look( ... )				-- box_error prefixed with "LOOK ----------------"
	ret = self:box_dev( title, ... )		-- dev dialog with stack trace prepended

-- ### DUMP

-- Inspection helpers built on table.print. dump prints self at depth level (default 3).
-- dump_up walks parents printing "DUMP LEVEL" lines.

	self:dump( [level] )					-- level default 3
	self:print_table( t, str, level_nb )	-- table.print prefixed with self
	self:dump_up( level )					-- prints self then recurses to parent

-- ## UI

-- do_action handles a small set of generic UI commands ("dump", "doc", "name", "values").
-- Subclasses override to add their own and chain to do_uif_command_with_super for fallback.
-- do_key / do_key_special are stubs to override.

	b_used = self:do_action( command )
	b_used = self:do_uif_command_with_super( b_used, uif, class )
	b = self:do_key( key )									-- default returns false
	b = self:do_key_special( key )							-- default returns false
	self:print_do_key( str, key )							-- debug header
	self:print_do_key_special( str, key )

-- ## TREE

-- Each GABU_OBJ can be linked to one parent (__up) and an ordered array of children (__down). This
-- doubly-linked tree is the backbone of the GaBu hierarchy: APPs hold MEUs, MEUs hold UIs, etc. The
-- four sub-sections cover one-step navigation up, the children array itself, batch traversal, and
-- search.

-- ### UP

-- Each node has at most one parent (__up). __set_up is internal and called by add_down on the parent
-- side. get_up_by_class walks up until an ancestor of the right class is found; get_up_before_by_class
-- returns the LAST self before that ancestor (useful to find the immediate child of a known container).

	parent = self:get_up()
	parent = self:get_up_by_class( class )
	child_before = self:get_up_before_by_class( class )

-- ### DOWN

-- Children are stored in a positional array __down with a cached count __down_nb. add_down enforces
-- uniqueness and sets the child's __up. get_down(id) raises (print_error + traceback) on out-of-range;
-- get_down_no_error returns nil silently. Negative ids count from the end (-1 is last).

	self:add_down( gabu_obj )								-- skipped if already present
	self:print_down( [pre] )								-- table.print of __down, optional prefix on the title
	n = self:get_down_nb()									-- 0 if no children
	idx = self:get_down_index( gabu_obj )					-- nil if not found
	t = self:get_down_array_active()						-- new array of active children, nil if empty
	t = self:get_down_array()								-- direct reference to __down (do not mutate)
	b = self:is_down( id )									-- bounds check, id != 0
	d = self:get_down_no_error( id )						-- silent on out-of-range
	d = self:get_down( id )									-- logs error + traceback on out-of-range
	self:sort_down( fn )									-- table.sort on __down
	old_down = self:remove_all_down()						-- clears parent links, returns the old array
	d = self:remove_down( gabu_obj [, b_error] )			-- b_error=false silences "not in down" message

-- ### APPLY (DOWN)

-- Iterate over children calling either a method by name or a free function with extra args.
-- _reverse versions iterate from the end. _active filters on is_active(). test_down* short-circuits
-- and returns the first non-falsey return value.

	self:apply_method_down( method [, ...] )
	self:apply_method_down_active( method [, ...] )	-- same but only on active children
	self:apply_fn_down( fn [, ...] )
	self:apply_fn_down_reverse( fn [, ...] )
	self:apply_fn_down_active( fn [, ...] )
	self:apply_fn_down_start_stop( start, stop, fn [, ...] )
	ret = self:test_down( fn [, ...] )						-- stops at first truthy return
	ret = self:test_down_reverse( fn [, ...] )

-- ### FIND (DOWN)

-- Search helpers on the children array. find_in_array_by_name_lowercase is static (operates on any
-- array of GABU_OBJ-like elements with get_name_lowercase()).

	d = self.find_in_array_by_name_lowercase( t, nb, name )	-- static
	d = self:find_in_down_by_name_lowercase( name )
	d = self:find_in_down_by_id( id )						-- matches on elt.id field
	d = self:find_in_down_by_field_val( field_name, val )	-- matches on elt[field_name] == val
	b = self:have_in_down_val( val )

-- ## DISPATCH

-- Instance-local table storage and the binding-table machinery built on top of it. Dispatch tables
-- (__fn_table) let GaBu code wire callbacks at runtime by name, with frozen args + runtime extras
-- merged at call time. The cross-class apply helpers iterate every DECLAREd class.

-- ### TABLE UTIL

-- get-or-create nested table on self, using rawget so __index of the class isn't followed (the table
-- stays on the instance). With sub_key, a second nested table is materialized.

	t = self:get_table_always( key [, sub_key] )
	t = self:get_table( key )								-- rawget, returns nil if absent

-- ### FUNCTION / METHOD DISPATCH

-- A GABU_OBJ instance can carry a __fn_table mapping keys to bound calls. Bindings are produced by
-- make_table_to_call_fn (free function + frozen args) or make_table_to_call_method (target gabu_obj +
-- method name + frozen args). do_fn(key, ...) merges the frozen args with runtime extras and invokes
-- the binding, returning ok_bool, fn_return. do_table runs a binding directly. Pass nil to a setter
-- to remove the binding.

	t = self:make_table_to_call_fn( fn [, ...] )			-- fn=nil returns nil (uninstall sentinel)
	t = self:make_table_to_call_method( gabu_obj, method_name [, ...] )	-- gabu_obj=nil returns nil
	self = self:set_function( key, fn [, ...] )
	self = self:set_method( key, gabu_obj, method_name [, ...] )
	b, ret = self:do_fn( key [, ...] )						-- false if key not bound
	b, ret = self:do_table( t [, ...] )
	b = self:has_fn( key )
	v = self:get_do_table_return_last()						-- last non-nil return value seen, class-static cache
	self:clear_fn_and_method()								-- wipe the whole __fn_table
	self:do_nothing()										-- explicit no-op for use as a placeholder binding

-- ### METHOD DISPATCH (ACROSS)

-- apply_method calls self[method](self, val, ...) for each val in tab. call_method_protected
-- runs self[method](self) under aaa.lua.pcall_protected, opening a dialog on error if b_dialog.
-- apply_method_to_all_classes iterates every DECLAREd class and calls method_name on it if defined
-- (rawget skips inherited methods).

	self:apply_method( tab, method [, ...] )
	ok = self:call_method_protected( method [, b_dialog] )
	self:apply_method_to_all_classes( method_name [, ...] )

-- ## INFRASTRUCTURE

-- Cross-cutting data services attached to instances: a Lua-source serializer for arbitrary value
-- trees and a small registry of "owned" AAASeed objects used for batch cleanup.

-- ### SERIALIZATION

-- Recursive Lua-source serializer. Handles tables, numbers, strings (quoted), booleans. Other types
-- are silently skipped. serialize_to_str inserts newlines at top level; serialize_to_line stays on one line.

	str = self:serialize_to_str( prefix, val, level )		-- prefix optional, val any, level default 0
	str = self:serialize_to_line( prefix, val, level )		-- flat one-line variant

-- ### REGISTRY

-- A self-managed set of AAASeed objects "owned" by self, stored at self.__gabu_registry_obj.
-- Used to delete a batch of created objects on cleanup.

	b = self:register_obj( obj )							-- error if already registered
	t = self:get_registry_obj()								-- the underlying table (boolean values)
	b = self:is_registry_obj( obj )
	b = self:unregister_obj( obj )							-- error if not registered
	self:delete_registry_objs()								-- aaa.obj.delete on every entry, then clears

-- ## MISC

-- Side helpers that don't fit elsewhere.

-- ### EDIT SOURCE

-- Open the .lua file where the class was declared in the OS-registered editor. Useful from a debug menu.

	self:edit_lua()

-- ### MEDIA HELPER

-- Resolve a filename to an image bind via IMGS.get_bind_sxy, then attach a media descriptor on
-- self.__medias[key]. media has fields bind, bank_2d, bind_2d, sx, sy, ratio.

	media = self:load_media( key, filename, b_async )		-- nil + box_error on missing file

-- ## Internal DOCumentation (DOC)

-- set_class_status_doc is the only public way to attach a status + multi-line doc to a class. Each
-- extra argument becomes a separate line in the resulting class_doc string. Called once at class
-- declaration, typically right after CLASS.DECLARE returns the new class.

	cls:set_class_status_doc( status [, line1, line2, ... ] )	-- called once at class declaration

-- ## UNIT TEST

-- A lightweight self-test scaffold: subclasses define a class-level do_unit_test( class ) method;
-- run_all_unit_tests walks every DECLAREd class and runs the method between __test_unit_begin and
-- __test_unit_end, printing per-test PASS/FAIL and a final summary.

	self:do_test( description, condition )					-- condition truthy = PASS
	self:do_test_error( description, func )					-- expects pcall(func) to fail
	self:run_all_unit_tests()
