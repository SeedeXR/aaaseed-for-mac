--	APP
--
if CLASS.DECLARE( "APP", nil,
	{
		verbose				= 0,
		__stack = STACK:create( "APP" ),
		__t_bu_edit 		= {},
	}	) then
	aaa.lua.global.declare( "app" )
	APP.apps = {}
	APP.media_dir_rel = aaa.dir.get_dir_media()
	APP.__b_bu_send = false
	APP:set_class_status_doc(	CLASS.STATUS.GABU,
								"APP means application, a kind of program for GaBuZo",
								"the cur app can be found using the global variable app" )
end

function APP.DECLARE( class_name, super, def_table, fn_init_class, ... )
	local class = CLASS.DECLARE( class_name, super, def_table, fn_init_class, ... )
	if class then
		APP_FACTORY.add( class_name, class, nil )
	end
	return class
end

function APP.CREATE_INST( app_class, name, ... )
	name = name or string.lower( app_class:get_class_name() )
--	table.print( APP.apps, "APP.apps" )
--	aaa.box_warning( "Will try to open APP "..name )
	if #(APP.apps) > 0 then
		local app_opened = APP.apps[1]
		aaa.debug.print_traceback()
		aaa.box_error(	"APP "..app_opened.." already open by\n      "..app_opened.__script_opening.."\n"..
						"\nso will not open "..name.." from\n      "..aaa.script.get_name().."\n"..
						"\nIn flatland switch off the modules you don't want\n      and keep the one you want"..
						"\n      you can also change param active"..
						"\n      in the default.layerss_param files"
						 )
		return
	end

	GA.force_verbose_def()
--	aaa.box_warning( "Open APP "..name )

	local self = app_class:create_instance( name )
	-- memorise so we can guard and do a nice message when we try to open a second one
	self.__script_opening = aaa.script.get_name()
	table.insert( APP.apps, self )

	self.ref					= {}
	self.ui						= {}
	self.__prefix				= name.."_"
	self.__need_midi_update		= {}	--	keep a list of BUI which need update but because they are midi related should be change

	self.__time_last			= aaa.time.t
	self.__time_interval		= 0

	self.__module				= aaa.module.get_cur()
	--self.__dir				= aaa.obj.get_dir( self.__module )
	--self:print( aaa.obj.get_name(self.__module ).." Titi "..aaa.file.get_dir_name( aaa.obj.get_name(self.__module ) ) )
	self.__dir_absolute			= aaa.file.get_dir_name( aaa.obj.get_name(self.__module ) ).."/"

	--self:print( "type "..type(name).." -> "..name )
	aaa.lua.global.set( name, self )

--	self:print_inverse( "dir is "..self:get_dir() )
	self:print_inverse( "dir absolute  is "..self:get_dir_absolute() )

	-- aaa:box_debug( aaa.obj.get_name(self.__module).."\n"
	-- 				..aaa.obj.get_dir(self.__module).."\n"
	-- 				..aaa.obj.get_filename(self.__module) )

	-- if app then
	-- 	aaa.box_debug( app.." dir is "..app:get_dir() )
	-- end

	self:clear_init_done()

	if not self:init_app( ... ) then
		self:box_debug( self..":init_app() not completed" )
	end

	self:set_init_done()

	if aaa.lua.global.get( "ga" ) then	-- going to global needed for reviving m42
		local bus_ctx = BUS_CTX:get_cur()
		--if bus_ctx then
			--tododesk should not be here
			--bus_ctx:attach_app( self )
		--end
		self:ask_bus_ctx(bus_ctx)
	end

	self:register_inst( name )

	self:define_app_ui()

	return self
end

function APP:ask_bus_ctx( ctx )
	self.__bus_ctx_to_attach = ctx
end
function APP:update_bus_ctx()
	-- local bus_ctx = self.__bus_ctx_to_attach
	-- if bus_ctx and (not  bus_ctx:is_open_regular()) then
	-- 	self.__bus_ctx = bus_ctx
	-- 	self.__bus_ctx_to_attach = nil
	-- end
end

--	FREE
--
--todo free __bus_ctx 
function APP:free()
	if self.t_activate then
		aaa.deactivate( self.t_activate )
	end
	self.__need_midi_update = nil
	self.__ref = {}

	GABUIN.stop_cam_cur()
end

function APP.free_app_cur()
	if not app then return end

	app:add_log( "Free" )
	if app.free == APP.free then
		app:box_error( "This app have no specific free method" )
	end

	app:print_inverse( "free() begin" )
		app:free()
	app:print_inverse( "free() end" )

	app = nil
	--todo refine
	APP.apps = {}
end


--	INIT
--
function APP:register_inst( name )
	if APP.verbose > 0 then
		self:box_debug( "register_inst( "..name.." )" )
	end
end

function APP:get_gp()	end	-- avoid some error when using only an APP without GP, it is override by APP_GP

function APP:get_module()
	return self.__module
end

function APP:init_app()
	self:set_active_direct( true )

	if aaa.lua.global.get( "ga" ) then	-- going to global needed for reviving m42
		ga:init_by_user()
	end

	if self.t_activate then
		aaa.activate( self.t_activate )
	end

	self:print_inverse( "APP "..self:get_name().." created." )

	aaa.window.set_main_title_prefix( self:get_dir_absolute().."  |  "  )

	self:init_ref()
	
	return self
end
function APP:init_ref()	end

function APP:is_init_done()			return self.__b_init_done		end
function APP:clear_init_done()
	--todo deal really with free
	app = self
	self.__b_init_done = false
end
function APP:set_init_done()		self.__b_init_done = true		end

function APP:define_app_ui()		end

--todo hook save
function APP:hook_quit( b_shutdown, b_shutdown_reboot ) -- send as int
	local str = "Quit"
	local name = self:get_name()
	if name then str = name.." "..str end
	self:add_log( str )
	if b_shutdown then
		local str
		if b_shutdown_reboot then
			str ="\tRequiring Shutdown and Reboot"
		else
			str ="\tRequiring Shutdown"
		end
		self:add_log( str )
	end
end


--	PUSH / POP
--
function APP:push()
	APP.__stack:push( app )
	app = self
	if self.verbose >= 2 then APP.__stack:print_fn( "push" ) end
end
function APP:pop()
	if self.verbose >= 2 then APP.__stack:print_fn( "pop" ) end
	app = APP.__stack:pop()
end
function APP:get_stack() return APP.__stack end


function APP.push_do_method_pop( self, method )
	if not self then
		aaa.debug.print_traceback()
		APP:print( "try to apply method "..method.." on nil app")
		return
	end

	if self:is_init_done() then
		--aaa.print_fn()
		self:push()
		--self:print( "oyo "..method )
			app[method]( self )
		self:pop()
	end
end
function APP:push_update_pop()
	self:push_do_method_pop( "update" )
end
function APP:push_draw_pop()
	self:push_do_method_pop( "draw" )
--	ga:reset_draw( true )
end



-- FILE
--
--function APP:get_dir()			return self.__dir			end
function APP:get_dir_absolute()		return self.__dir_absolute	end
function APP:get_prefix()			return self.__prefix		end
function APP:make_relative_path( path )
	local app_dir = self:get_dir_absolute()
	return string.gsub( path, app_dir, "" )
end

-- TARGET / MIDI
--
--	APP keep a list of element connected which need always an update
-- todo rename link / connection ...
function APP:register_for_midi_update( bu )
	self.__need_midi_update[bu] = bu
end
function APP:unregister_for_target_update( bu )
--	self:print( "unregister_for_target_update( "..bu.." )" )
	self.__need_midi_update[bu] = nil
end

function APP:update_target_needed()
	for k, bu in PAIRS( self.__need_midi_update ) do
		bu:update_target_midi()
	end
end


--	UPDATE
--
function APP:update_before()	end
function APP:update_after()
	local seqs = self.seqs
	if seqs then seqs:updraw( true ) end
end
function APP:update()

	self:update_bus_ctx()

	if self.__bus_ctx then
	--	self.__bus_ctx:update()
	end
	--aaa.debug.print_traceback( "APP:update()" )

	--self:print( "APP:update()" )
	self:update_before()
	self:update_log()

	if not aaa.b_ios then
		--todo refine we send same packet every frame
		local watch = aaa.lua.global.get( "watchdog" )
		local time = aaa.time.t
		local dt = time - self.__time_last
		self.__time_last = time
		self.__time_interval = self.__time_interval + dt
		if watch and self.__time_interval > 2 then
			self.__time_interval = 0
			--self:print( "APP:update() with watch" )
			watch.net_send( 0, "watchdog.aaaseed_is_alive( \""..aaa.net.machine.."\", \""..aaa.net.ip[1].."\" )" )
		end

		if not aaa.is_edit() then
			local t = self.__method_on_time
			if t then
				--	test on second is here to avoid a double reboot if reboot is fast
				if aaa.time.hour == t.hour and aaa.time.minute == t.minute and aaa.time.second <= 15 then
					self:add_log( "Shutdown asked because of hour" )
					t.table[t.method]( t.table, t.arg1, t.arg2, t.arg3, t.arg4 )
				end
			end
		end
	end
	self:update_target_needed()

	self:update_after()
end


--	DRAW
--
function APP:draw_before()
--	aaa.debug.print_traceback( "APP:draw_before()" )
end
function APP:draw_after()	end
function APP:draw()
	
--	aaa.debug.print_traceback( "APP:draw()" )

	--self:print( "APP:draw()" )
	self:draw_before()
	self:draw_after()

	if self.__bus_ctx then
		ga:reset_draw( true )
		self.__bus_ctx:draw()
	end
end

function APP:get_capture_suv_meter()
	self:print_error( "APP:get_capture_suv_meter() default called, using 8 x 4.5 HD format" )
	return 8, 4.5 -- return HD format by default and avoid test to call it
end


--	TARGET PC
--

-- GA handle this but it is read through APP
--  so we can specialize
function APP:is_onsite()	return ga:is_onsite()	end

function APP:lock( b_lock )
	-- is done after gp init
	ga:set_ui_group_active( not b_lock )
	ga:set_ui_group_active( not b_lock, "top" )
	aaa.set_edit( not b_lock )
	aaa.flatland.set_draw( false )
	if b_lock then
		param.set( aaa.ref.pref, "window_pop_trig",		1 )
	end
end
function APP:lock_onsite()
	self:lock( self:is_onsite() )
end


--	MASTER
--
function APP:is_master() 		return self.__b_master												end
function APP:set_master( b )	self.__b_master = b		self:print_inverse( "Master is now "..b )	end
function APP:is_slave()			return self.__b_slave												end
function APP:set_slave( b )		self.__b_slave = b		self:print_inverse( "Slave is now "..b )	end


--	WINDOW
--
function APP:add_window( title, bus_down,	x,y )
	local bu = bus_cur:add_window( title, bus_down,	{x,y, 2,2} )
	return bu
end


--	RESSOURCES
--
function APP:get_imgs()		return self.imgs end

function APP:create_obj_by_cid( cid, name )
	local obj = aaa.obj.create_by_cid( cid, name )
	aaa.obj.set_root( obj, GA.__c_node_rest  )
	self:register_obj( obj )
	return obj
end
function APP:clone_obj( src, root )
	local obj = aaa.obj.clone( src, root or GA.__c_node_rest )
	self:register_obj( obj )
	return obj
end
function APP:delete_obj( obj )
	aaa.obj.delete( obj )
	self:unregister_obj( obj )
end

-- infact a load_layers
function APP:create_layers( filename )
	aaa.obj.set_searchable_by_filename( false )
	local layers
	local err	--todo deal with err

	--self:box_debug( "APP:create_layers(\n"..filename )
	local layers_fname = (filename and filename:sub(-1) == "/") and (filename.."fx") or filename

	if not layers_fname then
		err = "No filename"
	elseif not aaa.file.is_exist( layers_fname..".aaa_layers_all" ) and not aaa.file.is_exist( layers_fname..".layers_param" ) then
		err = "No file .aaa_layers_all or .layers_param found"
	else
		layers = self:create_obj_by_cid( "layers", layers_fname )
		if not layers then
			aaa.print_error( "err loading layers : "..layers_fname )
			err = "create_obj_by_cid() failed"
		end
	end
	--free store object for deletion on free APP:free
	aaa.obj.set_searchable_by_filename( true )
	return layers, err
end

function APP:load_layers_or_module( dirname, name_for_c_obj )
	local layers_or_module
	local err	--todo deal with err

	--self:box_debug( "APP:load_layers_or_module(\n"..dirname )
	--hack asymmetry on set_obj_searchable_by_filename done at the time of AquaLoyal
	if aaa.file.is_exist( dirname.."fx.aaa_layers_all" ) or aaa.file.is_exist( dirname.."fx.layers_param" ) then
		aaa.obj.set_searchable_by_filename( false )
		layers_or_module = self:create_obj_by_cid( "layers", dirname.."fx" )
		if layers_or_module then
			aaa.obj.set_root( layers_or_module, ga.__c_node )
		else
			aaa.print_error( "err loading layers : "..dirname.."fx" )
			self:box_debug( "err loading layers :\n"..dirname.."fx" )
		end
		aaa.obj.set_searchable_by_filename( true )
	else
		--"local module_fname = filename:sub(-1) == "/" and (filename.."default.layerss_param") or filename
		if aaa.file.is_exist( dirname.."default.layerss_param" ) then
		--todo problem for FishTank 2023 December Maa
		--	aaa.obj.set_searchable_by_filename( false )
			--self:box_debug( "APP:load_layers_or_module(\n"..dirname )
			--local dir_last = aaa.dir.get_def()
			--local dir_new = dir_last.."/"..dirname
			--todo deal wuth error between push and pop
			--aaa.dir.push_def( dir_new )	--todo do it in C ?
			--self:box_debug( "change dir from\n"..dir_last.."\nto\n"..dir_new )
				layers_or_module = self:create_obj_by_cid( "module", dirname.."default.layerss_param" )
				if not layers_or_module then
					aaa.print_error( "err loading module : "..dirname.."default.layerss_param" )
				else
					--self:box_debug( "APP:create_module(\n"..dirname.."default.layerss_param" )
				end
		--todo problem for FishTank 2023 December Maa
		--	aaa.obj.set_searchable_by_filename( true )
			--aaa.dir.pop_def()
		end
	end

	if layers_or_module then
		aaa.obj.set_name( layers_or_module, name_for_c_obj )
	end
	--free store object for deletion on free APP:free
	return layers_or_module, err
end

function APP:save_layers_or_module( layers_or_module, dirname ) --, b_full )
	--aaa.print_fn( "WTF" )

	if not layers_or_module then
		self:print_error( "layers_or_module is nil ???" )
		aaa.debug.print_traceback()
		return
	end

	--self:box_debug( dirname )
	local err
	if not dirname then
		dirname = ""
	end

	local classname = aaa.obj.get_class( layers_or_module )
	--self:print( "APP:save_layers_or_module() "..classname.." "..aaa.obj.get_name(layers_or_module) )
	local fname

	--hack asymmetry on set_obj_searchable_by_filename done at the time of AquaLoyal
	if classname=="layers" then
		fname = dirname:sub(-1) == "/" and dirname.."fx"					or dirname
		aaa.obj.set_searchable_by_filename( false )
			err = aaa.obj.save_tree_to_file( layers_or_module, fname )	--, b_full )
		aaa.obj.set_searchable_by_filename( true )
	else
		fname = dirname:sub(-1) == "/" and dirname.."default.layerss_param"	or dirname
		--self:box_debug( "filename is "..dirname )
		--local module_fname = dirname:sub(-1) == "/" and ( dirname.."default.layerss_param" ) or dirname

		--todo we solved it on load
		--local dir_last = aaa.dir.get_def()
		--self:box_debug( "dir def was "..dir_last )
		--self:box_debug( aaa.dir.get_cur() )
		--local dir_new = dir_last.."/"..dirname
		--self:box_debug( "new dir is "..dir_last )
		--self:box_debug( module_fname )
		--aaa.dir.push_def( dir_new )	--todo do it in C ?

		--aaa.dir.pop_def()
		err = aaa.obj.save_tree_to_file( layers_or_module, fname )	--, b_full )
	end

	if err then
		self:print_error( "err "..err.." saving : "..fname )
	end
	return err
end


--	ACTION on TIME
--
function APP:set_method_on_time( hour, minute, table, method, arg1, arg2, arg3, arg4 )
	local t = {}
	t.hour = hour
	t.minute = minute
	t.table = table
	t.method = method
	t.arg1 = arg1
	t.arg2 = arg2
	t.arg3 = arg3
	t.arg4 = arg4
	self.__method_on_time = t
end
function APP:set_shutdown_no_save_on_time( hour, minute )
	self:set_method_on_time( hour, minute, aaa, "shutdown_no_save" )
end
function APP:set_reboot_no_save_on_time( hour, minute )
	self:set_method_on_time( hour, minute, aaa, "reboot_no_save" )
end


--	UTIL
--
function APP:dofile( filename, def )
	if not aaa.file.is_exist( filename ) then
		local str = "APP:dofile() : Non existing File "..filename
		self:box_dev( str )
		self:add_log( str )
		return false, str
	end
	--find better than tab here
	--todo find a better name/wat avoid this or permanent global
	aaa.lua.global.declare( "tab" )	-- use to load/save and preset
	tab = def
	local b_ok = aaa.lua.dofile_protected( filename )	--this set field in the tab
	--local b_ok = aaa.dofile( filename )
	aaa.lua.global.undeclare( "tab" )
	return b_ok
--	return aaa.dofile( filename )
end

--todo move to FX ?
function APP:add_flag( name, text, b_on )
	local bu = bus_cur:add_button( text )
	bu:set_value(b_on)	--needed it always to set to boolean
	bu:set_target_lua( self, name )
	return bu
end


--	LANG
--
--todo really finish it (use chanel N5)
--todo some in GA
function APP:get_lang()	return self.__lang	end
function APP:init_lang( ... )
	local lang = LANG:create( self:get_name(), ... )
	self.__lang = lang
end
function APP:init_lang_def()
	self:init_lang( "french", "english" )
end
function APP:set_lang( la )
	--self:print( "set_lang( "..la.." )" )
	local cur = self.__lang:get_cur()
	--self:print( "cur is "..cur )
	if la ~= cur then
		IMGS.switch_version( la )
		--self:print( "APP:set_lang() now change VIDEOS to "..la )
		VIDEOS:switch_version( la )
		self.__lang:set_cur( la )
	end
end
--todo complete
function APP:flip_lang()
	local la = (self.__lang:get_cur() ~= "english") and "english" or "french"
	self:set_lang( la )
end


--	LOG
--
function APP:create_log( filename, interval_sec, b_read )
	local name = self:get_name()
	self.__log = LOG:create( name, filename, interval_sec )
	if b_read == nil then
		b_read = true
	end
	if b_read then
		self:read_log()
	end
	local str = "LOG started"
	if name then str = name.." "..str end
	self:add_log( str )
end
function APP:create_log_in_dir( dir, interval_sec, b_read )
	local fname  = aaa.time.year_start.."_"..aaa.time.month_start.."_"..aaa.time.day_start
	self:create_log( dir.."AAA_LOG/AAA_"..fname..".log", interval_sec, b_read )
end

function APP:get_log()				return self.__log			end
function APP:read_log()				local log = self.__log		if log then log:read() end				end
function APP:write_log()			local log = self.__log		if log then log:write() end				end
function APP:add_log( str )			local log = self.__log		if log then	log:add( str ) end			end
function APP:add_log_error( str )	local log = self.__log		if log then log:add_error( str ) end	end
function APP:update_log()			local log = self.__log		if log then log:update() end			end


--	NET
--
--	todo clean this because it lokk wrong (there since at least 2020)
function APP:send_no_print( str )
	aaa.net.lua_send( 0, 0, "if "..self:get_name().." then "..str.." end" )
end
function APP:send( str )
	self:print( str )
	self:send_no_print( str )
end
function APP:send_fn( fn_name )
	local str = self:get_name().."."..fn_name
	self:send( "if "..str.." then "..str.."() end" )
end
--todo do a reception fn to compact transmition too
function APP:__send_method_low( b_verbose, method_name, args )
	local str =	"local t = aaa.lua.global.get(\"app\")\n"..
				"local fn = t and t."..method_name.."\n"..
				"if fn then fn(t"
	if args~=nil then str = str..","..args end
	str = str..") end"
	if b_verbose then self:print(str) end
	aaa.net.lua_send( 0, 0, str )
end
function APP:send_method( method_name, args )
	self:__send_method_low( false, method_name, args )
end
function APP:send_method_verbose( method_name, args )
	self:__send_method_low( true, method_name, args )
end

function APP:is_bu_send()
	return self.__b_bu_send
end
function APP:flip_bu_send()
	self.__b_bu_send = not self.__b_bu_send
	self:print_inverse( "bu_send is now "..self.__b_bu_send )
	aaa.show( self.__b_bu_send, "BU_SEND" )
end


--	OSC
--
function APP:print_osc_tags_args( tags, args )
	if tags	then
		local str = "receive_osc tags : "
		for _, v in ipairs(tags) do
			str = str.." "..v
		end
		self:print( str )
	end
	if args	then
		local str = "                    "
		for _, v in ipairs(args) do
			str = str.." "..v
		end
		self:print( str )
	end
end
function APP:receive_osc_print( start )
	repeat
		--self:print( "receive_osc()" )
		local tab = aaa.net.osc_take_by_start( start )
		if tab then
			--table.print( tab, "tags args", 3 )
			for i, v in ipairs(tab) do
				--table.print( v.args, "args", 3 )
				self:print_osc_tags_args( v.tags, v.args )
			end
		else
			break
		end
	until false
end

--	QUIT
--
function aaa.hook_quit( b_shutdown, b_shutdown_reboot ) -- send as int
	if not app then return end
	app:hook_quit( b_shutdown, b_shutdown_reboot )
end


-- 	EDIT
--
function APP:__is_bu_already_edit( bu )
	for k, v in pairs( self.__t_bu_edit ) do
		if v == bu then return true end
	end
	return false
end

function APP:__set_bu_edit( bu )
	--self:print( "__set_bu_edit "..bu )
	local t_bu_edit = self.__t_bu_edit
	if not self:__is_bu_already_edit( bu ) then
		table.insert( t_bu_edit, bu )
	end
end

function APP:__get_edit_bu_active( keyboard )
	--todo change it (see rest.lua GABU.update_key() )
	local t_bu_edit = self.__t_bu_edit

	if keyboard == "real" then
		return t_bu_edit[1]
	end

	local keyboard_type = keyboard:get_keyboard_type()

	local bu_front
	for k, v in pairs( t_bu_edit ) do
		if v:is_editable_active() and v:get_edit_type() == keyboard_type then
			bu_front = v
		end
	end
	return bu_front
end


-- KEYBOARD
--
function APP:do_key( key )
	self:print_do_key( "APP", key )

	local b_key_used = false
	if key == 117 then	-- 'u'
		self:flip_bu_send()
		b_key_used = true
	end

	return b_key_used or oo.getsuper(APP).do_key( self, key )
end
function APP:do_key_special( key )
	self:print_do_key_special( "APP", key )

	local b_key_used = false
	if self.seqs then
		b_key_used = self.seqs:do_key_special( key )
	end

	return b_key_used or oo.getsuper(APP).do_key_special( self, key )
end


-- FLIP / FBO
--
function APP:set_fbo_double_last( bind )	self.__fbo_double_last = bind		end
function APP:get_fbo_double_last()			return self.__fbo_double_last		end

-- SAVE
--
--todo 2023 Sept created, but should refine
function APP:save_top_level() 
end