 -- AAALUA	this to search easy the script editor (Visual STudio Code in general)
-- inside brackets mean optional: [,optional_arg]
-- a -- lua tag at end of line means defined in a Lua script (aaa_*.lua); no tag means C binding.
-- Files where every entry is pure Lua (e.g. lua_aaaseed_GABU_OBJ.lua) drop the tag entirely.
--
-- INCOMPLETE: this file documents the C bindings + the aaa_*.lua scripts + aaa_jit_dump.lua.
-- GABU_OBJ + CLASS have their own file (lua_aaaseed_GABU_OBJ.lua / .md).
-- SHADING has its own file (lua_aaaseed_SHADING.lua / .md).
-- Class wrappers (TRANSFO, OPENCL) and GaBu/* subfolders are not yet covered.
-- To enumerate the runtime API surface, see AAAKernel/Dev/AAAUser/Guest/default.lua_master.lua
-- and walk each loaded file.


-- # CORE / BOOTSTRAP
-- ## TIME
	aaa.time -- table defined by the C and completed with lua scripts based on the time stuff from c_application (key 0 in flatland)
 			 -- look at aaa_time.lua for more and details
	aaa.time.t 			-- is updated at every frame and take in account realtime flag, time_factor, restart...
	aaa.time.dt 		-- is the time since the previous frame
	aaa.time.restart()	-- reset the time as the t key (get_time_now() is not affected)
 
	t = aaa.time.get_time_now()	-- used to time things (e.g. MEU) return a time in sec from launch of program

	-- TIME pure-lua helpers (defined in aaa_time.lua)
	aaa.time.init_ref()					-- lua	wires aaa.time.ref
	aaa.time.init()						-- lua	resets the lua time state
	aaa.time.update()					-- lua	updates aaa.time.t, aaa.time.dt
	aaa.time.restart()					-- lua	(also exposed in C side) reset the time
	str	= aaa.time.get_str_hms()		-- lua	hh:mm:ss for current or given time
	str	= aaa.time.get_str_hms_underscore()	-- lua	hh_mm_ss
	str	= aaa.time.get_str_hms_semicolon()	-- lua	hh;mm;ss
	str	= aaa.time.get_str_ymd_underscore()	-- lua	yyyy_mm_dd
	str	= aaa.time.get_str_fps()		-- lua	formatted fps string
	v	= aaa.time.triangle( freq )		-- lua	triangle wave 0..1..0 at given freq

 -- ## MINIMUM TO START
 aaa.lua.get_table_by_caller()	-- return the same unique table for the same caller
 								--	use it to have persistent storage


-- todo comment better
ccalls	-- table used to accumulate the calls
		-- filled with ccalls[fn_name] = 1
		-- process controlled by lua master
		-- do ccalls = {} to restart

	OBJ means obj_ref or obj_name

-- ## HOOK
	--	no more aaa.hook_lua_start()
	--	filename arg need to be refined later
	aaa.hook_env_load_before	( filename )
		--	if existing called just before environment is loaded
		--	nothing loaded yet except net, pref, pref_start, bdd_ui_pref_master, lua_master, lua_master.lua_wrap
		--	this the first time lua is called
		--	GOL and opencl (if allowed) start after
	aaa.hook_env_load_after		( filename )
		-- called just after environment is loaded
	aaa.hook_env_save_before	( filename )
		-- called just before environment is saved
	aaa.hook_env_save_after		( filename )
		-- called just after environment is saved
	aaa.hook_main_loop_begin 	()
		-- just before giving control to the main loop
	aaa.hook_quit 				( b_shutdown, b_shutdown_reboot )
		-- just before quitting
		-- last chance to do something
	aaa.flatland.hook_draw_info ( sx, sy )	-- sx,sy size of viewport in pixel
	aaa.flatland.hook_draw_focus( sx, sy )	-- sx,sy size of viewport in pixel
	-- called after the background in drawn and before the rest ( stereo / text / ... )

	aaa.menu.hook( id )

-- ## Window title
	-- the window title (texte) of the main AAASeed window is made of 2 parts:
	--		prefix  |  title
	-- these functions set each part 
	aaa.window.set_main_title_prefix( prefix )
	aaa.window.set_main_title( title )

-- ## AAASeed Variables
	aaa.set( name, value )
	value	= aaa.get( name )

-- ## SCRIPT and LUA
	str		= aaa.script.get_dir_name	( )
	str		= aaa.script.get_name		( )
	str		= aaa.script.get_name_full 	( )

	aaa.lua.set_file_force_recheck( b )	-- quick hack to make sure script are reloaded (usually it is tested every 12 iterations )

-- ##TOS
	aaa.lua.add_package_path( str )		-- add str to package.path with printed messages
	aaa.lua.add_package_cpath( str )	-- add str to package.cpath with printed messages

-- ## INFO
	aaa.info.get_ref()				-- lua	get info obj ref
	path	= aaa.info.get_exe_path()			-- lua
	name	= aaa.info.get_exe_name_pure()		-- lua

-- ## JIT
	aaa.jit.on()					-- lua	enable LuaJIT
	aaa.jit.off()					-- lua	disable LuaJIT
	aaa.jit.set( b_on )				-- lua	enable/disable LuaJIT

	-- aaa.jit.dumper: LuaJIT verbose-trace dumper module (vendored from Mike Pall's -jv).
	-- Prints one line per generated trace; useful to see what gets compiled or where the JIT punts.
	aaa.jit.dumper.on( [outfile] )	-- lua	attach trace dump; outfile = filename, "-" for stdout, env LUAJIT_VERBOSEFILE, or stderr by default
	aaa.jit.dumper.off()			-- lua	detach trace dump and close file
	aaa.jit.dumper.start( [outfile] )	-- lua	alias for on(); used by the -j command line option


-- # AAAseed OBJECT (c_obj)
	-- create an object of cid (class id: e.g. layer),
	-- return nil when failed
	-- if there is fileame iut read the oject from the file, if not it create an empty one
	-- if there is a root_obj it set the root of the created object to it,
	-- if not the root is set to what the c side has as current defaut root.
	obj_ref = aaa.obj.create_by_cid( cid [,filename_or_nil [,root_obj_ref] ] )

	object_ref	= aaa.obj.get_root( obj_ref or obj_name )
	aaa.obj.set_root( obj_ref or filename or name_symbo, root_ref )

	obj_ref = aaa.obj.clone( src [,root_obj] )						-- in fact defined in aaa_obj.lua 

	aaa.obj.update_then_draw(	obj )	-- for MODULE LAYERS LAYER ( and in fact all c_obj_ui )
	aaa.obj.update(				obj )
	aaa.obj.draw(				obj )

	-- aaa.obj.attach( obj_ref or obj_name, param_name )
	-- aaa.obj.detach( obj_ref or obj_name, param_name )

	aaa.obj.set_focus	( obj )	-- use aaa.obj.set_focus_ui( obj ) which force flatland on too

	b = aaa.obj.is_focus( obj )
	aaa.obj.become_ui	( obj )

	b			= aaa.obj.is_class			( obj_ref or obj_name, class_name )		--	done in lua
	name		= aaa.obj.get_class			( obj_ref or obj_name )					-- return class name as string

	name		= aaa.obj.get_name_ui 		( obj_ref or obj_name )
	name		= aaa.obj.get_name_human 	( obj_ref or obj_name )
	name		= aaa.obj.get_name 			( obj_ref or obj_name )
				  aaa.obj.set_name			( obj_ref or obj_name, name )	-- if no name it erase it
	name		= aaa.obj.get_name_symbo	( obj_ref or obj_name ) -- return nil for empty name
				  aaa.obj.set_name_symbo	( obj_ref or obj_name, name )	-- if no name it erase it
	filename	= aaa.obj.get_filename		( obj_ref )		-- return nothing so nil if no filename
				  aaa.obj.set_filename		( obj_ref, filename )
--	dir_name	= aaa.obj.get_dir			( obj_ref )

	-- OBJ GET
-- return nil if not found
 	array		= aaa.obj.get_intances_by_class( class_name )	
	object_ref	= aaa.obj.get_by_name		( name )	-- search from the top of the obj_ui hierarchy by search_name then symbolic_name (name_symbo)
--	object_ref	= aaa.obj.get_ref			( name )	-- idem obj_by_name
	object_ref	= aaa.obj.get_by_name_symbo	( name )	-- search from the top of the obj_ui hierarchy by symbolic_name (name_symbo)
	object_ref	= aaa.obj.get_by_class_and_name_symbo( class_name, name )	-- search from the top of the obj_ui hierarchy by symbolic_name (name_symbo)
	object_ref	= aaa.obj.get_by_id			( id )		-- return object 

	-- if the first argument is a string it is search by filename first (which is fast 2024 June) then by name symbo which can be slow
	object_ref	= aaa.obj.get 				( id or name_symbo or name )	-- regroup all the fns in one use this one except if you have a good reason
	object_ref	= aaa.obj.get_no_error		( id or name_symbo or name )	-- idem but don't trigger error if not found

	object_ref	= aaa.obj.get_down			( obj_ref or filename or name_symbo, name_symbo )	-- search down the tree, return nil if not found
	object_ref	= aaa.obj.get_down_no_error	( obj_ref or filename or name_symbo, name_symbo )	-- idem but don't trigger error if not found
	object_ref	= aaa.obj.get_up			( obj_ref or filename or name_symbo, name_symbo )	-- search only up
	object_ref	= aaa.obj.get_up_no_error	( obj_ref or filename or name_symbo, name_symbo )

	object_ref	= aaa.obj.get_from_top_by_class				( class_name )	--	search from AAASeed root the top
	object_ref	= aaa.obj.get_from_top_by_class_no_error	( class_name )
	object_ref	= aaa.obj.get_down_by_class					( id or name_symbo or name,	class_name )	--	search only down
	object_ref	= aaa.obj.get_down_by_class_no_error		( id or name_symbo or name,	class_name )
	object_ref	= aaa.obj.get_by_class						( id or name_symbo or name,	class_name )	--	search only down, then up one level, ...
	object_ref	= aaa.obj.get_by_class_no_error				( id or name_symbo or name,	class_name )
	object_ref	= aaa.obj.get_up_by_class					( id or name_symbo or name,	class_name )	--	search only down, then up one level, ...
	object_ref	= aaa.obj.get_up_by_class_no_error			( id or name_symbo or name,	class_name )	--	search only up

	-- todo make fns to return table of objects for the obj fns too
	--	same than get_obj fns but find only branchs (the level directly below, branch have the obj_ref as root)
	object_ref	= aaa.obj.get_branch_by_name_symbo			( obj_ref or obj_name, name_symbo )
	object_ref	= aaa.obj.get_branch_by_name_symbo_no_error	( obj_ref or obj_name, name_symbo )
	object_ref	= aaa.obj.get_branch_by_class				( obj_ref or obj_name, class_name )
	object_ref	= aaa.obj.get_branch_by_class_no_error		( obj_ref or obj_name, class_name )
	array		= aaa.obj.get_branchs_by_class				( obj_ref or obj_name, class_name )	-- warning : the order is random, e.g: Don't except layer from layers to be in the right order
 	array		= aaa.obj.get_branchs						( obj_ref or obj_name )	-- warning : the order is the one of the c implementation

	aaa.obj.save( obj, fpath=nil )
	aaa.obj.load( obj, fpath=nil )

	aaa.obj.save_tree_to_file( obj, fpath )		-- documented here but avoid
 		-- use by MEU to save its layers or module, in C does a locking mechanism (why ?) and deal differently with dir in case of module
		-- but appart of that like save below
	aaa.obj.load_tree_from_file( obj, fpath )	-- documented here but avoid

	aaa.save()			--	same as CTRL S in the interaction
 						-- refine doc and precise functionality
	-- OBJ for more hardcore dev
	-- todo this need to be cleaned
	aaa.obj.set_searchable_by_filename( b ) -- On by default if off object don't sign_in/out of the name registry

	aaa.obj.delete( object_ref )
	aaa.wipe_focus()				--	to be called when we obj.delete(), so pointer to deleted stuff in the focus fifo will be flushed
	aaa.obj.update_params( obj	)	--	call the C param_init_pt ( maa guess it was useful to update the ui, adjustable grid for example)

	str = aaa.obj.enum_command( object_ref )
 	b = aaa.obj.do_command( object_ref, command_string )

	-- OBJ pure-lua helpers (defined in aaa_obj.lua)
	aaa.obj.get_lua_wrap( ref )			-- lua
	aaa.obj.edit_lua( ref )				-- lua

	-- OBJ not really use for now, have to be cleaned on c side
-- maa should develop name signification
	aaa.obj.get_with_ud( id or name_symbo or name )
	aaa.obj.find_first_by_class_name( class_name )

--	b = aaa.obj.set_target( dst_obj_ref, slot, target_obj_ref )
--	obj_ref = aaa.obj.get_target( src_obj_ref )

	object_ref	= aaa.get_multiple_cur()
	object_ref	= aaa.get_caller()

	b			= aaa.obj.is_ref			( obj_ref )		--	check the validity of the obj_ref
	b			= aaa.obj.is_ref_no_error	( obj_ref )		--	idem

	index = aaa.find_in_bind( obj_ref or obj_name, name )

-- # PARAM
--	param are designated thru a param reference got with get_ref
--		or using an obj_ref or obj_name followed by a param_name
--			param.get( param_ref )
--		or	param.get( obj_ref or obj_name, param_name )
--		noted param.get( PARAM ) below
--	param is a global table referencing aaa.param, it is defined in aaa_util.lua for backward compatibility

	b			= aaa.param.is_ref			( param_ref )	--	check the validity of the param_ref
	b			= aaa.param.is_ref_no_error	( param_ref )	--	idem

	aaa.obj.set_param_from		( dst as obj_ref or obj_name, src as obj_ref or obj_name )
	aaa.obj.set_param_from		( dst as obj_ref or obj_name, src as obj_ref or obj_name, index_begin )
	aaa.obj.set_param_from		( dst as obj_ref or obj_name, src as obj_ref or obj_name, index_begin, nb )
	aaa.obj.set_param_from_skip	( dst as obj_ref or obj_name, src as obj_ref or obj_name, nb_to_skip_at_begin, nb_to_skip_at_end )

	aaa.obj.clear_param	( obj_ref or obj_name )	-- set all the param of an object in the state they are at creation

	param_ref = param.get_ref			( obj_ref or obj_name, param_name )
	param_ref = param.get_ref_no_error	( obj_ref or obj_name, param_name )	--	just return nil if it can't get a ref, don't trig an error
	param.set( PARAM, value [,comment] )	-- if value is nil, numeric param are untouched (were set to 0 pre June 2018)

	string	= param.get_type( PARAM )	--	for now
										--	"int32",		"uint32",		"symbolic_negative",	"symbolic_zero",	"symbolic",
										--	"bool",			"fp32",			"double",
										--	"string",		"file_name",	"dir_name",		"reference",
										--	"class_branch",	"bit32",		"time_code",
										--	"group",		"group_closed",
										--	"none",
										--	or "type_no" if not in the range
	bool	= param.is_type_real 		( PARAM )	-- true for fp32 and double
	bool	= param.is_type_bool 		( PARAM )
	bool	= param.is_type_number 		( PARAM )
	bool	= param.is_type_text 		( PARAM )

	value	= param.get_comment			( PARAM )
	param.set_comment					( PARAM, str )		-- comment is displayed after param

	value	= param.get					( PARAM )
	value	= param.flip				( PARAM )			-- inverse the value of a boolean
 															-- on other type: 0 is false on other value are true
 															--  will then set to 1 or 0

	param.set_real 	(	param_ref, v1 )
	--	these are fast, unprotected fns (they write to consecutive location in memory, successive params, in fact)
	--		to be used by a coder who know what he does
	param.set_real_2(	param_ref, v1, v2 )
	param.set_real_3(	param_ref, v1, v2, v3 )
	param.set_real_4(	param_ref, v1, v2, v3, v4 )
	param.set_real_5(	param_ref, v1, v2, v3, v4, v5 )
	param.set_real_6(	param_ref, v1, v2, v3, v4, v5, v6 )
	param.set_real_7(	param_ref, v1, v2, v3, v4, v5, v6, v7 )
	param.set_real_8(	param_ref, v1, v2, v3, v4, v5, v6, v7, v8 )

	v1								=	param.get_real(	  PARAM )
	--	these are fast, unprotected fns (they write to consecutive location in memory)
	--		to be used by coder who know what they do
	v1, v2							=	param.get_real_2( PARAM )
	v1, v2, v3						=	param.get_real_3( PARAM )
	v1, v2, v3, v4					=	param.get_real_4( PARAM )
	v1, v2, v3, v4, v5				=	param.get_real_5( PARAM )
	v1, v2, v3, v4, v5, v6			=	param.get_real_6( PARAM )
	v1, v2, v3, v4, v5, v6, v7		=	param.get_real_7( PARAM )
	v1, v2, v3, v4, v5, v6, v7, v8	=	param.get_real_8( PARAM )

	value	= param.get_int32			( PARAM )
	value	= param.get_uint32			( PARAM )

	bool	= param.get_bool			( PARAM )

	value	= param.get_str				( PARAM )
	value	= param.get_str_lower		( PARAM )	-- same forcing lowercase
	value	= param.get_str_upper		( PARAM )	-- same forcing uppercase

	min		= param.get_min				( PARAM )
	max		= param.get_max				( PARAM )
	def		= param.get_def				( PARAM )
	ina		= param.get_ina				( PARAM )

			param.set_min				( PARAM, value_min )
			param.set_max				( PARAM, value_max )
			param.set_def				( PARAM, value_def )
			param.set_ina				( PARAM, value_ina )

	value	= param.get_name			( PARAM )

	b		= param.is_save				( PARAM )
	value	= param.set_save			( PARAM, b )

	obj 	= param.get_obj				( PARAM )	-- obj it belongs to
	obj 	= param.get_obj_attached	( PARAM )	-- obj attached

	b 		= param.do_action_open		( PARAM )	-- return TRUE if the object used it or FALSE if not


	-- index start at 0 and are the c index of the param in the obj
	--		useful to use cpy_param_fns
	index	= param.get_nb				( obj_ref or obj_name, param_name )
	index	= param.get_index			( obj_ref or obj_name, param_name )
	index	= param.get_index_no_error	( obj_ref or obj_name, param_name )	--	just return nil if it can't find the param, don't trig an error

	-- todo we change in C (2023 April) so we should follow code path
--	param.infini	-- the int 32 value used in param C side as PARAM_INFINI, careful with rounding of value

	-- PARAM pure-lua helpers (defined in aaa_param.lua)
	param.set_and_save_no( ref, val )			-- lua	set + mark non-saved
	param.set_and_save( ref, val, b_save )		-- lua	set + save flag toggle
	param.set_v2( ref, v )						-- lua	set 2 consecutive params from a v2 table
	param.set_v3( ref, v )						-- lua	idem for v3
	param.set_v4( ref, v )						-- lua	idem for v4
	ref	= param.get_ref_xyz( obj_ref, name_pre )	-- lua	get refs to <name>_x/_y/_z params
	ref	= param.get_ref_xyz_packed( obj_ref, name_pre )		-- lua
	ref	= param.get_ref_uva( obj_ref, name_pre )	-- lua	get refs to <name>_u/_v/_a
	ref	= param.get_ref_uva_packed( obj_ref, name_pre )		-- lua
	v1,v2 = param.get_v2( ref )					-- lua	read 2 consecutive params
	t	  = param.get_v2_packed( ref )			-- lua	idem returns table {v1,v2}
	v1,v2,v3 = param.get_v3( ref )				-- lua
	t	  = param.get_v3_packed( ref )			-- lua
	v1,v2,v3,v4 = param.get_v4( ref )			-- lua
	t	  = param.get_v4_packed( ref )			-- lua

-- # AAA UTIL (lua, defined in aaa_util.lua)
	aaa.activate( t )							-- lua	enable a feature/object
	aaa.deactivate( t )							-- lua
	aaa.init_ref_first()						-- lua	called early in script init
	aaa.flatland.set_draw( b )					-- lua
	aaa.flatland.set_draw_focus( b )			-- lua
	aaa.flatland.is_draw_focus()				-- lua
	aaa.flatland.set_draw_info( b )				-- lua
	aaa.flatland.set_focus_offset( x, y )		-- lua
	aaa.flatland.set_color_blind( b )			-- lua
	aaa.set_check_memory( b )					-- lua
	aaa.set_lua_silent( b )						-- lua
	b	= aaa.pc.is_maa()						-- lua	PC identity helpers
	b	= aaa.pc.is_dev()						-- lua
	b	= aaa.is_edit()							-- lua
	aaa.set_edit( b )							-- lua
	aaa.update_util()							-- lua
	-- quit / shutdown / reboot helpers
	aaa.set_shutdown_reboot( b_on )				-- lua
	aaa.set_shutdown_on_exit( b_on )			-- lua
	aaa.set_quit_do_save( b_on )				-- lua
	aaa.quit_save()								-- lua
	aaa.quit_no_save()							-- lua
	aaa.quit()									-- lua
	aaa.shutdown()								-- lua
	aaa.shutdown_no_save()						-- lua
	aaa.reboot()								-- lua
	aaa.reboot_no_save()						-- lua
	aaa.kill_explorer()							-- lua
	aaa.launch_explorer( dir )					-- lua
	aaa.obj.set_focus_ui( ref )					-- lua	set focus + force flatland on
	aaa.os.open_dir( dir )						-- lua	open dir in OS file explorer

-- # DATAGRID
	-- todo move it to bdd, generalize
	--	col and line start at 1 of course
	aaa.obj_set_real			( obj, col, line, val	)
	aaa.obj_set_str 			( obj, col, line, str	)
	val		= aaa.obj_get_data	( obj, col, line		)	-- return nil or number or str depending on the content
	-- the next two should probably now be removed, obj_get_data() is better
	val		= aaa.obj_get_real	( obj, col, line		)
	str		= aaa.obj_get_str	( obj, col, line		)	--	return "" if no str


-- # UI
-- ## KEYBOARD (lua, defined in aaa_keyboard.lua)
	aaa.keyboard.init_ref()					-- lua
	b	= aaa.keyboard.is_ctrl()			-- lua
	b	= aaa.keyboard.is_shift()			-- lua
	b	= aaa.keyboard.is_alt()				-- lua
	b	= aaa.keyboard.is_ctrl_only()		-- lua
	b	= aaa.keyboard.is_shift_only()		-- lua
	b	= aaa.keyboard.is_alt_only()		-- lua
	b	= aaa.keyboard.is_no_modifier()		-- lua
	aaa.keyboard.consume_ascii()			-- lua	consume the ascii key event
	c	= aaa.keyboard.get_ascii()			-- lua
	c	= aaa.keyboard.get_ascii_down()		-- lua	without consuming
	aaa.keyboard.consume_special()			-- lua	consume special key event (F-keys, arrows, etc.)
	k	= aaa.keyboard.get_special()		-- lua
	k	= aaa.keyboard.get_special_down()	-- lua
	b	= aaa.keyboard.is_key_fn( key, nb )	-- lua	check if key is F<nb>
	aaa.keyboard.update()					-- lua

-- ## MOUSE
 	-- pixel are relative to the window and go from 0 (left/top) to window size-1 (right/bottom)
	aaa.mouse.set_xy_pixel( x,y )
	x,y = aaa.mouse.get_xy_pixel()
	-- render are relative to the render area and go from 0 (left/bottom) to 1 (right/top)
	x,y = aaa.mouse.get_xy_render()
	rx,ry = aaa.mouse.convert_xy_pixel_to_render( x,y )

	aaa.mouse.set_grabbed( b )		-- true to inhibit real action
	aaa.mouse.cancel_move()			--	use by GABU to stop interaction in AAASeed when it use it (avoid cam move for example)

	aaa.mouse.init_ref()			-- lua	wires aaa.mouse.ref to bind objects
	aaa.mouse.is_but_left()			-- lua
	aaa.mouse.is_but_middle()		-- lua
	aaa.mouse.is_but_right()		-- lua
	aaa.mouse.is_wheel_up()			-- lua
	aaa.mouse.is_wheel_down()		-- lua
	aaa.mouse.update()				-- lua	updates aaa.mouse.* state

-- ## EVENT	
	EVENT.hook( e_type, e_sub_type, a,b,c,d,e )	--	see EVENT.lua for example, flag to activate it in lua_master

-- ## ACTION
	aaa.do_action( action )	--	these will change action is not really defined

-- ## WINDOW (lua)
	aaa.force_window( x,y, sx,sy )			-- lua
	aaa.push_window()						-- lua
	aaa.push_console()						-- lua
	aaa.force_fullscreen()					-- lua
	aaa.set_fullscreen( mode )				-- lua
	mode = aaa.get_fullscreen_mode()		-- lua
	b	 = aaa.is_fullscreen()				-- lua
	aaa.set_monitor_off()					-- lua
	aaa.set_monitor_on()					-- lua
	aaa.set_window_size( x, y )				-- lua
	x,y	 = aaa.get_window_size()			-- lua
-- ## SCREEN (lua)
	nb	 = aaa.screen.get_nb()				-- lua
	l,b,r,t = aaa.screen.get_lbrt( screen_id )		-- lua
	sx,sy   = aaa.screen.get_sxy( screen_id )		-- lua
	scale   = aaa.screen.get_scale( screen_id )		-- lua
	vp      = aaa.screen.get_viewport_cano( screen_id )		-- lua
	aaa.screen.set_camera( cam, screen_id )			-- lua

-- ## FLATLAND
	str	= aaa.flatland.get_mess( i )	--	get line from the console 1 is last. 2...
	r,g,b,b_inverse = aaa.flatland.get_mess_color( i )
	nb	= aaa.flatland.get_mess_line_nb()
	b	= aaa.flatland.is_in_focus( x, y )
	b	= aaa.flatland.is_in_info( x, y )
	b	= aaa.flatland.is_in( x, y )

-- ## MENU
	main_id = aaa.menu.get_main( )
	aaa.menu.set_main( main_id )

	tree_param_id = aaa.menu.get_tree_param( )
	aaa.menu.set_tree_param( tree_param_id )

	param_list_id = aaa.menu.get_param_list( )

	id = aaa.menu.create( )
	aaa.menu.set_cur( id )
	nb = aaa.menu.get_item_nb()		--	use this +1 when you add a menu to have something to remove it

	aaa.menu.insert_item		( item_pos, name, out_value [,check] )	-- item_pos in [1,item_nb]
	aaa.menu.add_item			(			name, out_value [,check] )

	aaa.menu.insert_separator	( item_pos )							-- item_pos in [1,item_nb]
	aaa.menu.add_separator		()

	aaa.menu.insert_menu_sub	( item_pos, name, id )					-- item_pos in [1,item_nb]
	aaa.menu.add_menu_sub		( name, id )

	aaa.menu.change_item		( item_pos, name, out_value [,check] )	-- item_pos in [1,item_nb]
	aaa.menu.remove_item		( item )

-- # INPUT / OUTPUT
-- ## MIDI
	-- channel_id    in [1,16]
	-- control_index in [0,511]
	-- note_index    in [0,127]
 	-- value is in [0,1]
	value	= aaa.midi.get_control	( channel_id, control_index )
	value	= aaa.midi.get_velocity	( channel_id, note_index )
	aaa.midi.set_control			( channel_id, control_index, value )

	aaa.midi.send_note_on			( obj, channel_id, note_index, velocity )	-- velocity in [0,127]
	aaa.midi.send_note_off			( obj, channel_id, note_index, velocity )
	aaa.midi.send_note_pressure		( obj, channel_id, note_index, velocity )
	aaa.midi.send_control_change	( obj, channel_id, control_index, value )	-- value in [0,127]
	aaa.midi.send_program_change	( obj, channel_id, program_index )

	aaa.midi.load_global( filename )
	aaa.midi.save_global()				-- use the name used for load_global
	aaa.midi.save_global( filename )
	aaa.midi.add_patch_global( channel_id, control_index, channel_dst, control_index_dst )
	-- no more for now
	--	aaa.midi.hook_set_control( channel, control, value )
-- add tin

-- ## MESSAGE
	--	each str will be separated by a linefeed
	aaa.print(			str [,str ...] )	--	yellow for now	we should add a choice
	aaa.print_inverse(	str [,str ...] )	--	inverse / reverse
	aaa.print_error(	str [,str ...] )
	aaa.print_debug(	str [,str ...] )

	aaa.bell()	-- old school bell from bell character
	
	aaa.box_error_lua(	err_str	[,str ...] )	-- used for pcall error (pcall called from lua)
	aaa.box_error(		str [,str ...] )
	aaa.box_warning(	str [,str ...] )
	aaa.box_good(		str [,str ...] )
-- ## DIALOG
	ret = aaa.box_dev( title, str [,str ...] )	-- return nil if Cancel, 1 if OK, 2 if Other
	ret = aaa.box_ask( title, str )				-- return nil if cancel, 1 if ok

	-- this is the lua standard mechanism but it will like any Lua error
	--		eventually do the traceback and trigger AAASeed dialog / editor focus
	error( str )

-- ## CLIPBOARD
	aaa.clipboard.move_to( str )	-- send str to the os clipboard

-- # MEDIA
-- ## AUDIO
	aaa.audio.init_ref()			-- lua	inits aaa.audio.ref with refs to obj master and params 'volume', 'pan'
	aaa.audio.set_volume( vol )		-- lua	setter for aaa.audio.ref.volume
	aaa.audio.set_pan( pan )		-- lua	setter for aaa.audio.ref.pan

-- ## VIDEO
	aaa.video.set_bind_filename( bind, name )	--	just assign name to bind
	name	= aaa.video.get_bind_filename( bind )	--	return name of bind


-- # INFRASTRUCTURE
-- ## MATERIAL
	-- todo ? aaa.material_front_set_alpha( alpha )
	material_ref = aaa.material.get_front_cur()
	material_ref = aaa.material.get_front_ui()
	material_ref = aaa.material.get_back_cur()
	material_ref = aaa.material.get_back_ui()

-- ## MODULES
	modules_ref	= aaa.modules.set_cur( [modules_ref] )	-- todo
	modules_ref	= aaa.modules.get_cur()
	modules_ref	= aaa.modules.get_ui()

	nb			= aaa.modules.get_module_nb_max()
	aaa.modules.set_module_ui( modules_ref, id )	-- id starting at 1
	module_ref = aaa.modules.get_module( modules_ref, id )			-- if no indes return module_ui
	module_ref = aaa.modules.new_module( modules_ref, id, filename )	-- if no indes it use module_index_ui

-- ## MODULE
	module_ref	= aaa.module.get_cur()
	module_ref	= aaa.module.get_ui()
-- ## LAYERS
	layers_ref	= aaa.layers.get_cur()	--	return nothing if no bdd
	layers_ref	= aaa.layers.get_ui()

	--	this a typical sequence to deal with a Layers in lua
	--	with no arg layers cur is used by default by each call
	--		could be dangerous with multiple ...
	-- MEU should use the equivalent MEU fns 
	aaa.layers.draw_begin(			[layers_ref] )	--	should always be match by a draw_after
 													--	Trax, FBO, Camera, Viewport, Transfo
	aaa.layers.draw_layer_all(		[layers_ref] )	--	draw all layers in layers
-- todo		aaa.layers.draw_layer( 		id )		--	1 to 26
	aaa.layers.draw_end(			[layers_ref] )	--	should always be match by a draw_before
	aaa.layers.skip_rest(			[layers_ref] )	--	todo limit to current layers and donc pas
	
	aaa.layers.draw_fbo_and_cam(	[layers_ref] )	-- experimental (just update the fbo / camera part)

	aaa.layers.push_attrib()
	aaa.layers.pop_attrib()

	layer_ref	= aaa.layers.get_layer(		layers_ref, id )	--	id 1 to 26 ( A to Z )
	camera_ref	= aaa.layers.get_camera(	layers_ref, index )	--	index 0 to 15
	camera_ref	= aaa.layers.get_camera(	layers_ref )		--	camera used by the layers, set when executing draw_begin()

	traxs_ref	= aaa.layers.get_traxs		( layers_ref )			-- with no arg layers cur is used by default
	trax_ref	= aaa.layers.get_trax		( layers_ref, id )		-- id in [1,26]
	values_ref	= aaa.layers.get_values		( layers_ref )			-- with no arg layers cur is used by default
	value_ref	= aaa.layers.get_value_ref	( layers_ref, id )		-- id start at 1 for value_00
	val 		= aaa.layers.get_value		( layers_ref, id )
-- ## LAYER
	layer_ref	= aaa.layer.get_cur()
	layer_ref	= aaa.layer.get_ui()
	bdd_ref 	= aaa.layer.get_bdd(		LAYER )
	bdd_ref		= aaa.layer.get_bdd_prev(	LAYER )
	aaa.layer.push_attrib()
	aaa.layer.pop_attrib()
	aaa.layer.set_bdd_external( LAYER, bdd_ref )
	aaa.layer.update_draw_shader_bdd()					-- tmp

	-- LAYER pure-lua helpers (defined in aaa_layer.lua)
	ref	= aaa.layer.get_rendering( layer_ref )			-- lua
	ref	= aaa.layer.get_mapping( layer_ref )			-- lua
	ref	= aaa.layer.get_model( layer_ref )				-- lua
	ref	= aaa.layer.get_model_no_error( layer_ref )		-- lua
	ref	= aaa.layer.get_color( layer_ref )				-- lua	c_color ref
	ref	= aaa.layer.create_color( layer_ref, name )		-- lua	encapsulating COLOR_REF
	ref	= aaa.layer.get_shading( layer_ref )			-- lua	c_shading ref
	ref	= aaa.layer.create_shading( layer_ref, name )	-- lua	encapsulating SHADING
	ref	= aaa.layer.get_tex_unit( layer_ref, id_unit )	-- lua	id_unit defaults to 0
	ref	= aaa.layer.get_tex_unit_mapping( layer_ref, id_unit )	-- lua
	ref	= aaa.layer.get_bank_bind_2d_ref( layer_ref, id_unit )	-- lua	bank/bind 2d param ref
	tab	= aaa.layer.build_bank_bind_2d_ref_table( layer_ref, id_unit_begin, id_unit_end )	-- lua
	aaa.layer.set_bind_2d( layer_ref, bind )			-- lua

-- ## CAMERA
	xc, yc, zc	= aaa.camera.world_to_cam(		xw,	yw,	zw	)
	xw, yw, zw	= aaa.camera.cam_to_world(		xc,	yc, zc	)
	xs, ys, zs	= aaa.camera.world_to_screen(	xw,	yw,	zw	)
	xw, yw, zw	= aaa.camera.screen_to_world(	xs,	ys,	zs	)
	xs, ys, zs	= aaa.camera.cam_to_screen(		xc,	yc,	zc	)
	xc, yc, zc	= aaa.camera.screen_to_cam(		xs,	ys,	zs	)

	cam_ref	= aaa.camera.get_cur()
	cam_ref	= aaa.camera.get_ui()
	cam_ref	= aaa.camera.get_ui_or_find()	-- search camera when no camera ui
	aaa.camera.set_cur(	cam_ref )
	aaa.camera.set_cur()			--	set camera cur to NULL
	aaa.camera.set_ui(	cam_ref )
-- ## VIEWPORT (lua)
	ref	= aaa.viewport.get_ref_by_name( name )	-- lua
	v	= aaa.viewport.get( name )				-- lua
	rx	= aaa.viewport.get_ratio_x( name )		-- lua
	ry	= aaa.viewport.get_ratio_y( name )		-- lua
-- ## FBO
	aaa.fbo.unbind()		--	Needed for Garden APPs
	fbo_ref	= aaa.fbo.get_cur()
	fbo_ref	= aaa.fbo.get_prev()
-- ## SHADING
	shading_ref = aaa.shading.get_cur()
	-- todo extend this with more generic fns with variable data size, stride....
	aaa.shading.set_vert_vec4_spe( shading_ref, user_data_array_of_18_fp32 )
	aaa.shading.set_vert_vec4_spe( shading_ref, table_of_18_fp32 )
	aaa.shading.set_vert_vec4_spe( shading_ref, v1, v2, ... , v18 )

-- ## STEREO (lua)
	aaa.stereo.init_ref()					-- lua
	b = aaa.stereo.is_active()				-- lua
	b = aaa.stereo.is_field_for_update()	-- lua
	aaa.stereo.update()						-- lua

-- ## LIGHTS
	lights_ref	= aaa.lights.get_cur()
	lights_ref	= aaa.lights.get_ui()
	light_ref	= aaa.lights.get_light( id )	-- id in [1,8]

-- ## DEFORMER
	deformer	= aaa.deformer.get_prev()
	deformer	= aaa.deformer.get_cur()
	deformer	= aaa.deformer.get_ui()
	-- todo test
	x,y,z = aaa.deformer.apply_xyz( deformer, x,y,z )

-- ## MULTIPLE
	id, x, y, z = aaa.get_multiple_id_xyz( multiple ) -- only for multiple


-- # BDD
-- ## GENERIC
--	BDD means obj_ref or obj_name but for a bdd
--	BDDN means BDD, or "nothing" (the argument is not passed: lua use the nb of argument to detect this )
--		in the nothing case the default lua bdd is used

	bdd	= aaa.bdd.get_cur()			-- return ref or nil
	bdd	= aaa.bdd.get_ui()			-- return ref or nil
	aaa.bdd.set_ui( bdd )

	aaa.bdd.update		( BDD )
	aaa.bdd.restart		( BDD )
	aaa.bdd.set_lua_cur	( BDD )	--	set a default bdd, return true if non nil bdd set
								--		pass nil or nothing set no bdd (NULL in C++)

	aaa.bdd.set_color(	index, r, g, b, a )	--	implemented only on bdd_voxel for now
-- ## BDD POINT (implemented bdd by bdd, error message when not implemented)
	nb			=	aaa.bdd.get_point_dataset_nb	(	BDDN )
					aaa.bdd.set_point_dataset		(	BDDN,	id )	-- id start at one

	nb			=	aaa.bdd.get_point_nb			(	BDDN )
	x,y,z		=	aaa.bdd.get_point				(	BDDN,	index )	-- return nothing if index is invalid
	x,y,z,id	=	aaa.bdd.get_point_and_id		(	BDDN,	index )
	x,y,z,id	=	aaa.bdd.get_point_and_id_local	(	BDDN,	index )	-- avoid need to call  poid.split_id() after, this is the case with get_point_and_id
	id,x,y,z	=	aaa.bdd.get_id_and_point		(	BDDN,	index )	--	will be removed ? goss for id,x,y = get ...
--	id,x,y,z	=	aaa.bdd.get_id_and_point		(	bdd, -1 )		-- or any negative number start a process
--	id,x,y,z	=	aaa.bdd.get_id_and_point		(	bdd )			-- get the next one
	table 		= 	aaa.bdd.get_points				(	BDDN )			--	table = { {x,y,z} [,{x,y,z}... ] }
	table 		= 	aaa.bdd.get_points_with_id		(	BDDN )			--	table = { {x,y,z,id} [,{x,y,z,id} ...] }

	-- return true if the bdd provided all the point
	b = aaa.bdd.get_points_to_table( table, table_id, obj, point_index, point_nb, point_stride )
	b = aaa.bdd.get_points_to_table( table, table_id, obj, point_index, point_u_nb, point_u_stride, point_v_nb, point_v_stride )

	--	only in bdd_mocap/voronoi for now
	nb						=	aaa.bdd.get_segment_nb	(	BDDN )
	xa,ya,za,	xb,yb,zb	=	aaa.bdd.get_segment		(	BDDN,	index )

	x,y,z, speed_x, speed_y, speed_z	= aaa.bdd.get_random_point_and_speed( BDD, time )
	x,y,z, speed_x, speed_y, speed_z	= aaa.bdd.get_random_point_and_speed( BDD )	-- for current time

	aaa.bdd.set_point( BDD, index, x, y, z )				-- return true if done, nil otherwise
															-- index start at 1, if z omited considered as 0
															-- implemented in bdd_polygon/boid/voxel/point for now
	aaa.bdd.erase_points( BDDN )

	aaa.bdd.hook_draw_multiple
	-- if nil the rendering multiple do its job as usual
	-- if it a function then it is expected to be s function(index) which is called for each multiple rendering

-- ## BDD BOID
	-- hack for now but ensure we always talk about the same boid
	--	index start at 1
	b = aaa.bdd.set_point_by_id		( BDD, index, x, y ,z )			--	idem but point is selected by id (only boid with id by_index set will work)
																--	return true when success
	b = aaa.bdd.set_point_by_id		( BDD, index, x, y )				--	z = 0
	x,y,z = aaa.bdd.get_point_by_id	( BDD, index )				--	only boid for now,
	b = aaa.bdd.set_box_by_id		( BDD, index, x, y ,z )			--	b = true when success or nil

-- poids and poid
-- these are hooks: C notation but in fact a lua function
	poid.hook_set_universe( universe_id )                                       -- universe_id is int
	r = poid.hook_is_repulse( boids_id_a, boids_id_b )                          -- return 1. if interaction between poids has to happen; a==b on self-interaction
	r = poid.hook_do_contact( boid_id_a, boid_id_b )                            -- return 1. if contact has to happen
	poid.hook_do_kill( boids_id, boid_id )
	poid.hook_do_create( boids_id, boid_id )

	x,y,z = get_pos()
	x,y,z = get_pos_to_draw()
	x,y,z = get_speed()

	-- todo check and add
	-- ADD_POID_FN( set_pos );
	-- ADD_POID_FN( set_speed );
	-- ADD_POID_FN( get_box_size );
	-- ADD_POID_FN( get_pos_a );
	-- ADD_POID_FN( get_pos_b );
	-- ADD_POID_FN( create_at );
	-- ADD_POID_FN( kill_all );
	-- ADD_POID_FN( kill_a );
	-- ADD_POID_FN( kill_b );
	-- ADD_POID_FN( kill_by_id );
	-- ADD_POID_FN( set_fix_a );
	-- ADD_POID_FN( set_fix_b );
	-- ADD_POID_FN( split_id );

	poid.create_at( ref_or_name, x, y ,z )
	poid.create_at( ref_or_name, x, y ,z, speed_x, speed_y, speed_z )

	poid.kill_all( ref_or_name )
	poid.kill_by_id( id )	-- experimental works only for bdd_boid, bdd_part(id_generator = index_mode )

	bdd_boid_id, id = poid.split_id( id )
	
-- ## BDD_VOXEL
	--	experimental for now
	--	suppress first arg to use default bdd ( set by aaa.bdd.set_lua_cur )
	aaa.bdd.add_point_line_y(	BDDN,	x, y, z,	dy,	nb, img_col )
	aaa.bdd.set_line_y(			BDDN,	id,	 b_active, b_forced )
	aaa.bdd.begin_render(		BDDN )
	aaa.bdd.end_render(			BDDN )
	--	need default lua bdd set
	--[[	doc from C++...
	{	"set_target",			1,	ID_TARGET			},
	{	"clear",				1,	ID_CLEAR			},
	{	"fill",					1,	ID_FILL				},
	{	"scale",				1,	ID_SCALE			},
	{	"cpy",					2,	ID_COPY				},
	{	"copy",					2,	ID_COPY				},
	{	"add",					2,	ID_ADD				},
	{	"sub",					2,	ID_SUB				},
	{	"mul",					2,	ID_MUL				},
	{	"mix",					2,	ID_MIX				},
	{	"interpolate",			2,	ID_INTERPOLATE		},
	{	"min",					2,	ID_MIN				},
	{	"max",					2,	ID_MAX				},
	{	"image_color_clear",	0,	ID_IMG_COLOR_CLEAR	},
	]]--
	aaa.bdd.render_op0			(	opname,						[args, ...] )
	aaa.bdd.render_op1			(	opname, buf1,				[args, ...] )
	aaa.bdd.render_op2			(	opname,	buf1, buf2,			[args, ...] )
	aaa.bdd.render_op3			(	opname,	buf1, buf2, buf3,	[args, ...] )
	-- todo	aaa.bdd.mul_add_render(		r,g,b, [,a] [or,og,ob] [,oa] )
	aaa.bdd.render_plane_axe	(	axe,	y_begin, y_end )
	aaa.bdd.render_sphere		(	x,y,z,				size, delta_full, delta_grad )
	aaa.bdd.render_boule		(	x,y,z,				size, inside_cano=1	)
	aaa.bdd.render_cube			(	x,y,z,				size, inside_cano=1	)
	aaa.bdd.render_segment		(	xa,ya,za, xb,yb,zb,	size, inside_cano=1	)
	aaa.bdd.render_cylinder_y	(	x,y,z,				size, delta_full, delta_grad )

	aaa.bdd.set_color_map		(  	index, bind, u, v, fu, fv )		-- bind nil or negative to clear the color map

-- ## BDD_PART
	bdd.add_particle_at( bdd, x, y ,z )
	bdd.add_particle_at( bdd )

-- ## BDD_GRID_ADJUSTABLE
	--	 work only on bdd_grid_adjustable for now
			aaa.bdd.set_control_point	(	BDD, index_u,index_v, x,y [,z] )
	x,y,z =	aaa.bdd.get_control_point	(	BDD, index_u,index_v )
	u,v	= aaa.bdd.get_uv_from_xy(	BDD, x, y )
	u,v	= aaa.bdd.get_uv_from_xy(	BDD, x, y, z )		--	z unused for now

-- ## BDD_BOXES only for now
	control_id	= aaa.bdd.get_control_id_from_xy( BDD, x, y )
	control_id	= aaa.bdd.get_control_id_from_xy( BDD, x, y, z )
-- ## BDD bdd_tube_path
	len					=	aaa.bdd.get_len(				BDD )

-- ## BDD_TUBE_PATH
	nb = bdd.get_ring_nb					(	BDD )
	cur, next, inter = bdd.get_ring_from_s	(	BDD,	s )
	bdd.init_ring                           (	BDD )					--	init all_rings
	bdd.init_ring							(	BDD, index )
	bdd.set_ring_center_offset				(	BDD, index, o_u, o_v )
	ou, ov = bdd.get_ring_center_offset		(	BDD, index )
	bdd.set_ring_radius						(	BDD, index, ru, rv )
	ru, rv = bdd.get_ring_radius			(	BDD, index )
	bdd.set_ring_angle_offset				(	BDD, index, angle_offset )
	bdd.set_ring_angle_range				(	BDD, index, angle_range )
	bdd.set_ring_enabled					(	BDD, index, b )
	bdd.set_ring_marker						(	BDD, index, fn_str, id )				--	trigger aaa.fn_str( id/id_marker, index/id_ring, x, y, z )
	bdd.set_ring_texture					(	BDD, index, bind_index )				--	-1 or nil to go back to regular way
	bdd.set_ring_uv							(	BDD, index, u, u_next, v_min, v_max, v_next_min, v_next_max  )
	bdd.set_ring_uv							(	BDD, index, u, u_next, v_min, v_max )	-- like previous but v_next_min = v_min and v_next_max = v_max
	bdd.set_ring_uv							(	BDD, index, u, u_next )					--	consider v_min = 0, vmax = 1
	x, y, z = bdd.get_ring_center			(	BDD, index )
	x, y, z = bdd.get_ring_normal			( 	BDD, index )

-- ## BDD_CURVE_3D, BDD_LINE_3d, BDD_MOCAP
	-- dataset_id == node_id	--	s in [0,1] at least for curve_3d
	len					=	aaa.bdd.get_len					(	BDD,	node_id			)
	x,y,z				=	aaa.bdd.get_point_s				(	BDD,	node_id, s		)
			tx,ty,tz	=	aaa.bdd.get_tangent_s			(	BDD,	node_id, s		)
	x,y,z,	tx,ty,tz	=	aaa.bdd.get_point_tangent_s		(	BDD,	node_id, s		)
-- ## BDD_CURVE_3D and BDD_MOCAP only
	x,y,z				=	aaa.bdd.coor_to_world			(	BDD,	node_id, x,y,z	)
-- ## BDD_CURVE_3D and BDD_LINE_3d only
	x,y,z				=	aaa.bdd.get_point_s_curve_fast	(	BDDN,	node_id, s		)
	table				=	aaa.bdd.get_points_s			(	BDDN,	node_id, ds [, s_begin [, s_end ] ] )	-- sample points along curve from s_begin (default 0) to s_end (default 1) with step ds

	nb = aaa.bdd.get_control_point_nb	( 	BDD, dataset_id )
							aaa.bdd.set_control_point		(	BDD, index_u, x,y [,z] )
	x,y,z				=	aaa.bdd.get_control_point		(	BDD, index_u )
	aaa.bdd.push_control_point_back		(	BDD, dataset_id, x,y,z )
	aaa.bdd.pop_control_point_front		(	BDD, dataset_id )
	aaa.bdd.clear_control_points		(	BDD, dataset_id )

	aaa.bdd.clear_datasets				(	BDD )
-- ## BDD_LINE_3D only (quite specialized to transfert boid to curve in script for aquariuns
	len = aaa.bdd.push_control_point_back_len_max( BDD, dataset_id, x,y,z, len_max ))

-- ## BDD_CURVE_EDIT
	val = aaa.bdd.get_curve_val( BDD, control_id, phase )

-- ## BDD_IMG_CONTOUR and BDD_BLOB BDD_BLOB_TRACKING (derived fron bdd_blob_base)
	aaa.bdd.get_blob_nb( sbdd )
	x,y				= aaa.bdd.get_blob_xy(			bdd, id )	-- start at 1
	x,y, sx,sy		= aaa.bdd.get_blob_xy_sxy(		bdd, id )	-- start at 1
	id, x,y			= aaa.bdd.get_blob_id_xy(		bdd, id )	-- start at 1
	id, x,y, sx,sy	= aaa.bdd.get_blob_id_xy_sxy(	bdd, id )	-- start at 1
	--	t is an existing table, nb = bdd.get_blob_polyline_to_table( t, bdd, id )
	nb				= aaa.bdd.get_blob_polyline_to_table( t,	bdd, id )	-- start at 1

-- ## BDD_BLOB
	aaa.bdd.clear_blob	(	BDD )
	aaa.bdd.add_blob	(	BDD, id, x, y, sx, sy )
-- ## BDD_BLOB_TRACKING
	b	= aaa.bdd.get_ghost(	BDD, index )

-- ## BDD_FIELD_GENE only
	aaa.bdd.add_sphere 					( BDD, x,y,z, radius )
-- ##DEF_DISTANCE_FIELD only
	aaa.bdd.update_fish_length 			( BDD, len )

-- ## DEF_WALL_INTERACTION
	aaa.bdd.reset_walls 				( BDD )
	aaa.bdd.add_human 					( BDD, x,y,z, radius )
	aaa.bdd.end_humans 					( BDD )

-- ## BDD_FLEX only

	-- todo no more jitter ? jotter of what
	-- aaa.bdd.add_rigid_box( bdd_ref, x,y,z, rx,ry,rz, nb_x,nb_y,nb_z, radius, vx,vy,vz, mass, b_rigid, stifness, what_jitter )

	aaa.bdd.flex.create_begin()
	aaa.bdd.flex.create_end()
	nb = aaa.bdd.flex.get_particle_nb(	BDD )
	aaa.bdd.flex.set_default( b_self_collide, b_fluid, group_idx [, tag] )	-- the others fns can set it too as an option
	aaa.bdd.flex.append_particle(	BDD, x,y,z, vx,xy,xz, ux,uy,
										mass,
										[,b_self_collide, b_fluid, group_idx [, tag]] )
	aaa.bdd.flex.append_box(		BDD, x,y,z, rx,ry,rz, vx,vy,vz, nb_x,nb_y,nb_z, radius,
										mass, b_rigid, stiffness
										[,b_self_collide, b_fluid, group_idx [, tag]] )
	aaa.bdd.flex.append_cylinder( 	BDD, x,y,z, rx,ry,rz, vx,vy,vz, nb,nb_z, radius,
										mass, b_rigid, stiffness
										[,b_self_collide, b_fluid, group_idx [, tag]] )
	aaa.bdd.flex.append_sphere(		BDD, x,y,z, rx,ry,rz, vx,vy,vz, nb, radius,
										mass, b_rigid, stiffness,
										[,b_self_collide, b_fluid, group_idx [, tag]] )
 	aaa.bdd.flex.append_cloth_grid(	BDD, x,y,z, sx,sy, mass, axis, pin_mode, nb_u,nb_v,
										b_stretch_use,	stretch_stiff,	stretch_rest,
										b_bend_use,		bend_stiff,		bend_rest,
										b_shear_use,	shear_stiff,	shear_rest,
										[,b_self_collide, b_fluid, group_idx [, tag]] )

	 aaa.bdd.flex.append_spring( 	BDD, from, to, stiffness, rest_length_factor )
	 aaa.bdd.flex.append_triangle( 	BDD, i1,i2,i3, nx,ny,nz )

-- ## BDD_BULLET only
	aaa.bdd.set_gravity					( bdd, gravity_x, y, z )												-- sets gravity of the active world

	ind = aaa.bdd.create_box_shape		( bdd, sx, sy, sz )														-- create a btBoxShape and return its index in c_bdd_bullet::_box_shapes
	ind = aaa.bdd.create_sphere_shape	( bdd, size )															-- create a btSphereShape and return its index in c_bdd_bullet::_sph_shapes
	ind = aaa.bdd.create_cylinder_shape	( bdd, sx, sy, sz )														-- create a btCylinderShape and return its index in c_bdd_bullet
	ind = aaa.bdd.create_transform		( bdd, px, py, pz )														-- create a btTransform and return its index in c_bdd_bullet::_transforms
	ind = aaa.bdd.create_rigid_body		( bdd, mass, ind_trans, ind_shape, shape_type, [local_inertia_xyz] )	-- create a btRigidBody and return its index in c_bdd_bullet::_rigid_bodies

	bid = aaa.bdd.add_object_bid		( bdd, ind_trans, ind_shape, ind_body, shape_type )						-- create a c_bullet_obj (only rigid for now),
 																													-- shape type : 1 = sphere, 2 = box
																													-- add it to the current dynamic world	and c_bdd_bullet::_objects
																													-- returns its bid (bullet id)
	bid = aaa.bdd.delete_obj_bid		( bdd, bid ) 															-- remove obj with bid from c_bdd_bullet::_objects (map) and push_back to c_bdd_bullet::_objects_free
	_b_is_bid = aaa.bdd.is_bid			( bdd, bid )															-- checks if bid exists

	aaa.bdd.lock_move_rotate_bid		( bdd, bid, axes_xyz, angles_xyz )
	aaa.bdd.lock_move_rotate_all_bid	( bdd, axes_xyz, angles_xyz )
	aaa.bdd.set_mass_bid				( bdd, bid, mass )
	aaa.bdd.set_pos_bid					( bdd, bid, pos )
	px, py, pz = aaa.bdd.get_pos_bid	( bdd, bid )
	aaa.bdd.set_size_bid				( bdd, bid, sx, sy, sz )
	sx, sy, sz = aaa.bdd.get_size_bid	( bdd, bid )
	aaa.bdd.set_rot_z_bid				( bdd, bid, rot_z )
	rz = aaa.bdd.get_rot_z_bid			( bdd, bid )
	aaa.bdd.set_damping					( bbd, bid, damping )
	aaa.bdd.set_dynamic					( bbd, bid, bool )
	aaa.bdd.set_restitution				( bdd, bid, restitution )
	aaa.bdd.pick						( bdd, from_x, from_y, 0, to_x, to_y, to_z )
	m0, m1, [...] = aaa.bdd.get_matrixes( bdd )

-- ## BDD dataset bdd_opencl_part
	aaa.bdd.update_dataset				(	BDD, dataset_id )		--	no dataset_id means all
	aaa.bdd.draw_dataset				(	BDD, dataset_id )		--	no dataset_id means all
	aaa.bdd.set_dataset_float			(	BDD, id, v1 [,vn...] )	--	id start at 1


-- # NETWORK
-- ## NET
	aaa.net.lua_send(			link_index, channel, name [,name...] )
	aaa.net.send_magic_packet(	link_index, mac_raw )	-- mac_raw should be "xx-xx-xx-xx-xx-xx" or "xx:xx:xx:xx:xx:xx"
	aaa.net.send_text8(			link_index, name [, name...] )
	aaa.net.hook_receive_text8( str )			-- hook to reveive BLK_TEXT8
	aaa.net.restart(			link_index )	-- same as ask for close, update(), ask for start, update()

	-- NET pure-lua helpers (defined in aaa_net.lua)
	aaa.net.set_active( b_active )				-- lua
	b	= aaa.net.is_active()					-- lua
	b	= aaa.net.is_remote()					-- lua
	id	= aaa.net.get_host_id()					-- lua
	aaa.net.update()							-- lua
	b	= aaa.net.is_ip_str( ip_str )			-- lua	checks "x.x.x.x" string format
	ref	= aaa.net.get_link_ref( link_index )	-- lua
	aaa.net.restart_link( link_index )			-- lua
	aaa.net.restart_with_ip_port( tab )			-- lua	tab fields: link_index, ip="192.168.0.1", port, etc.
	aaa.net.set_local( link_index )				-- lua
	aaa.net.set_any( link_index )				-- lua
	aaa.net.start_with( tab )					-- lua
	aaa.net.stop_link( link_index )				-- lua
	aaa.net.stop_link_all()						-- lua
	aaa.net.lua_send_fn_global( link_index, channel, fn_name )	-- lua
	-- OSC pure-lua helpers
	aaa.net.osc_dump_tag_arg( index, tags, args )	-- lua	debug helper to print osc payload
	aaa.net.osc_test( start )					-- lua	smoke test
	aaa.net.osc_apply_method( start, obj, method )	-- lua

-- ## NET REQUESTS (HTTP)
	-- Typical HTTP request verbs
	-- when bo headers default used is { {"accept", "application/json"} }
	id = aaa.net.requests.get(   	url	)
	id = aaa.net.requests.post(  	url, payload [,headers] )
	id = aaa.net.requests.put(   	url, payload [,headers] )
	id = aaa.net.requests.patch(	url, payload [,headers] )
	id = aaa.net.requests.download( url, path	)
	id = aaa.net.requests.delete(	url	)

	-- Authentication, applies to all subsequent requests unless disable_auth() or clear_auth() is called
	aaa.net.requests.set_basic_auth(  username, password )
	aaa.net.requests.set_digest_auth( username, password )
	aaa.net.requests.set_ntlm_auth(   username, password )
	aaa.net.requests.set_oauth(			token	)
	aaa.net.requests.disable_auth()
	aaa.net.requests.enable_auth() -- Only necessary to be called if disable_auth() was called previously
	aaa.net.requests.clear_auth()

	-- Checking requests, obtaining responses, parsing JSON to Lua tables
	-- Returns table of request_ids and whether the request has completed or not (1 or 0)
	t = aaa.net.requests.get_request_list()
	-- Returns false if request isn't done, response object as table otherwise
	result = aaa.net.requests.get_request_result_if_done(	request_id	)
 	-- Converts a JSON in string form into a Lua table
	t = aaa.net.requests.json_to_table(response_body)
		

--## OSC
	--	osc_index (x) is in [0,3] and use the osc_net_dst_0/1/2/3 param to choose the net_link
	--			(pref with F10 / chapter Midi Osc Net)
	--	e.g. aaa.net.osc_send( dst, "/eo", 2, "no", toto )
	aaa.net.osc_send(				osc_index, tag, ... )
	aaa.net.osc_send_no_bundle(		osc_index, tag, ... )
	aaa.net.osc_flush(				osc_index )
	tab = aaa.net.osc_take_by_start( str )
		--	tab is a table of table, where each table is made of 2 tables { { tags, args }, [ { tags, args }, ... ] }
		--	tags table contain the field separated by / in the osc packet
		--	args table contain the value of the fields


-- # FILES
-- ## FILE AND DIR
	str = string.remove_trailing_slash( str )	-- defined in aaa_string
	-- trigger a load of file
	aaa.file.read_async( slot, filename )	--	experimental for now, slot in [1,16]

	str	= aaa.file.read_text( filename )
	b	= aaa.file.is_exist	( filename )
	b	= aaa.file.create	( filename )					-- create an empty file and the dir(s) in the path if needed
															-- return true if creation successful
	ext			=	aaa.file.get_ext		( filename )	-- return nothing if no extension found
	dir_name	=	aaa.file.get_dir_name	( filename )
	file_name	=	aaa.file.get_file_name	( filename )
	name_pure	=	aaa.file.get_name_pure	( filename )	--	remove dir and last extension ( only after the last '.' )

	int64	= aaa.file.get_modification_date	( filename )
	-- return nothing if ok: err_str is nil in this case
	err_str = aaa.file.remove( filename )
	err_str = aaa.file.move( src, dst )					-- will fail if dst already exist ? common function with aaa.dir.move()
	err_str = aaa.file.copy( src, dst, b_overwrite )
	err_str = aaa.file.copy( src, dst )					-- b_overwrite is false in this case
	err_str = aaa.file.rename( src, dst )				-- rename file or dir, same fn in aaa.dir
	table	= aaa.file.list_drive()	-- return table with all connected drives
									-- return nil if no drive	-- { { drive= "C:"; type = "usb_key", b_usb = true } ; {...}; }

	b = aaa.dir.is_exist( dir )	-- return bool
	b = aaa.dir.create	( dir )	-- return true if creation successful
	b = aaa.dir.remove	( dir )	-- return true if remove successful
	aaa.dir.move = aaa.file.move
	aaa.dir.rename = aaa.file.rename

	aaa.dir.set_def( name )			-- prefer push_def/pop_def for nested usage
	aaa.dir.push_def( name )
	name 	= aaa.dir.pop_def()
	name	= aaa.dir.get_def()

	name	= aaa.dir.get_dir_start()			-- dir where the exe started
	name	= aaa.dir.get_dir_kernel()			-- dir where AAASeed set to get the data AAAKernel
 												--  was get_dir_app but now APP (lua notion) can be anywhere
												--  use app:get_dir_absolute()
	name	= aaa.dir.get_dir_user()
	name	= aaa.dir.get_dir_net()
	name	= aaa.dir.get_dir_tracker()			-- implemented in lua

	table	= aaa.dir.get_files( dir, pattern )	-- return a table of string or nil when no files matches
 												--  in pattern you can use any combination of wildcard characters that is supported by the host operating system.
	table	= aaa.dir.get_files( dir )			--  same but pattern is "*.*"
	table	= aaa.dir.get_dirs(  dir, pattern )	-- same for directories
	table	= aaa.dir.get_dirs(  dir )

	path = aaa.info.get_exe_path()
	name = aaa.info.get_exe_name_pure()

	-- FILE / DIR pure-lua helpers (defined in aaa_file.lua)
	err	= aaa.file.copy_dir( src, dst, b_overwrite )		-- lua	deep copy of a directory
	str	= aaa.file.make_name_number( pre, i, post, format )	-- lua	build "<pre><N><post>" with format on N
	b	= aaa.file.is_exist_name_number( pre, i, post, format )		-- lua
	str	= aaa.file.get_format_exist_name_number( pre, i, post )		-- lua
	fh	= aaa.file.open_no_error( fname, how )			-- lua	idem aaa.file.open without error popup
	fh	= aaa.file.open( fname, how )					-- lua	wrapper around io.open with logging
	aaa.file.save_text( fname, text )					-- lua	write text to fname (creates dir if needed)
	aaa.file.do_if_exist( filename )					-- lua	dofile if exists
	aaa.file.do_if_exist_protected( filename, b_diag )	-- lua	pcall'd variant
	str	= aaa.dir.get_dir_media()						-- lua	media dir path
	str	= aaa.dir.make_media_path( path_in_media )		-- lua	get_dir_media() .. path_in_media
	-- aaa.dir.get_dir_tracker() already documented above; defined in aaa_file.lua
	str	= aaa.dir.get_dir_snapshot()					-- lua

-- ## FILE DIALOG
 	-- todo move to aaa.file.do_dialog_open/save and aaa.dir.do_dialog
	-- filter use the window convention (e.g. filter = "Avi\0*.*\0Movie\0*.avi;*.mpg;*.mpeg;*.mov;*.qt;*.wmv;*.mp4\0Tous\0*.*\0\0")
	--  or predefined values (unsensitive to case): "image" or "video" for now
	-- these function return nil when "cancel" is choosen
	name			= aaa.file.do_dialog_open(   title [,filter] )			--	return nil when canceled
	table_of_name	= aaa.file.do_dialog_open(   title  ,filter  ,true )	--	return an array of names or nil when canceled
	name			= aaa.file.do_dialog_save(   title [,filter] )			--	return nil when canceled
	name			= aaa.file.do_dialog_folder( title )					--	return nil when canceled

	-- todo must refine quickly extended for Loyal
	-- todo comment lua_id
	
	aaa.edit_dialog( lua_id, param [,obj=nil [,b_simple=false, [,title=nil, [,b_integer=false]]]]  )	-- dialog value will be updated with aaa.dialog hook
	--	aaa.dialog_hook( lua_id, action, value )
	aaa.dialog_hook( lua_id, "open" )			-- called before dialog init event is processed
	aaa.dialog_hook( lua_id, "close", status )	-- called after dialog is ended, status is "ok" or "cancel" for now (2024 Jan)
	aaa.dialog_hook( lua_id, "change_value", value )


-- # SYSTEM / DIAGNOSTICS
-- ## PC
	-- useful in watchdog mode when we can do it thru params
	b = aaa.os.shutdown( b_reboot )	-- return true if success, don't do exit
	aaa.os.exit( b_shutdown, b_reboot )
	aaa.os.quit( b_confirm, b_save, b_shutdown, b_reboot )

	b = aaa.os.open_editor( fname [,line_to_go] )
	str_err_or_nil = aaa.os.execute_shell( command, arg )	-- return true if c did it with no error
	str_err_or_nil = aaa.os.execute_process( command, arg )	-- same but launch using create process
	str = aaa.os.get_exe_start_time()	-- str depend on date and time till the second, useful to get_unique_str

	aaa.os.pop_console()
	aaa.os.push_console()
	aaa.os.pop_window()
	aaa.os.push_window()
	aaa.os.set_window_topmost()
	aaa.os.set_window_notopmost()

	aaa.sleep( millisec )	-- call the C Sleep function

-- ## POWER (lua, defined in aaa_power.lua)
-- aaa.power.ref holds param refs to the C "power_master" object; the helpers below read/write through them.
	aaa.power.update()										-- lua	refresh aaa.power.b_plugged / b_charging / battery_level / time_left / time_max
	str	= aaa.power.get_str()								-- lua	one-line "Power : Plugged Charging 87.3% 42 mins left"
	aaa.power.print()										-- lua	print get_str()
	aaa.power.set_battery_cpu( min, max )					-- lua	set CPU state range when on battery and trigger
	aaa.power.set_plugged_cpu( min, max )					-- lua	set CPU state range when plugged and trigger
	aaa.power.set_brightness_battery( bright )				-- lua	set display brightness when on battery and trigger
	aaa.power.set_brightness_plugged( bright )				-- lua	set display brightness when plugged and trigger

-- ## SPY	implement NSight for now
--	color_index -> red=1, green=2, blue=4 and combine -> 0 black, 7 white, 5 magenta...
	aaa.spy.add_mark	( str )
	aaa.spy.push_range	( str, color_index )
	aaa.spy.pop_range	( str )

-- ## DEBUG (lua, defined in aaa_debug.lua)
	aaa.debug.debug()								-- lua	enter the debug REPL
	t	= aaa.debug.get_fn_info( level )			-- lua	debug.getinfo wrapper, returns table
	str	= aaa.debug.get_fn_name( level )			-- lua	function name at stack level
	v	= aaa.debug.get_obj( level )				-- lua	get nearest method-receiver
	o,m	= aaa.debug.get_obj_method( level )			-- lua
	str	= aaa.debug.get_obj_method_name( level )	-- lua	"obj:method" composite name
	t	= aaa.debug.make_traceback_table()			-- lua
	aaa.debug.handle_error( err_str )				-- lua	central error sink
	aaa.lua.pcall_protected( fn, info, b_dialog )	-- lua	pcall wrapper with traceback + dialog
	aaa.lua.dofile_protected( filename, b_dialog )	-- lua
	aaa.debug.print_traceback( str_in )				-- lua
	aaa.debug.print_arg( a )						-- lua
	aaa.debug.print_args( ... )						-- lua
	aaa.debug.show( ... )							-- lua	value, str, caller, alive_time
	aaa.debug.show_warning( ... )					-- lua
	aaa.debug.show_error( ... )						-- lua
	str	= aaa.debug.get_arg_as_str( start )			-- lua
	str	= aaa.debug.get_fn_arg_as_str()				-- lua
	str	= aaa.debug.get_method_arg_as_str()			-- lua
	aaa.debug.print_fn( str_or_bool )				-- lua	if str: prepend, if true: print_traceback after
	aaa.debug.print_fn_inverse( str_or_bool )		-- lua
	aaa.debug.print_method( str_or_bool )			-- lua
	aaa.debug.print_local()							-- lua
	aaa.debug.box_debuginfo( info )					-- lua
	aaa.empty()		-- dont't do anything just to test speed of fn calls
					-- useful to debug too

-- ## WATCHDOG
	watchdog.set_loop_time( loop_time_in_sec )
	watchdog.net_send( INT32 link_dst, str1, str2, ... )		-- send a LUA_WATCHDOG BLOCK
	watchdog.trig_exit()										-- exit watchdog
	time = watchdog.get_time()
