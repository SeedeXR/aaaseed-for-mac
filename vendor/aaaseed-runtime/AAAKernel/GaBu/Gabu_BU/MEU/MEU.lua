if CLASS.DECLARE( "MEU", nil, {
	__LABEL_FILENAME	= "aaa.lua",
	__no_app_root_dir	= "AAA/MEU/"
	} ) then

	aaa.lua.global.declare( "meu" )
	MEU.__b_meu_send = false
	MEU.__verbose_file = 0
end

MEU:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
							"Module Editable Unit with its compact form MU (Module Unit)\n"..
							"encapsulate a block of functionality. This is the basic block\n"..
							"when using APP_GPs (Application \"GARDEN PARTY\") holding a MU graph\n"..
							"that the user edit and manipulate.\n"..
							"It regroup all the User interface and the code for a block of functionality.",
							"Once a MEU prototype is defined, instances can be created."
						)

function MEU:show_error( str, ... )
	--self:print_debug( str )
	self.__b_show_error = true
	oo.getsuper(MEU).show_error( self, str, ... )
end

						
function MEU:get_bus()					return self.__bus						end
function MEU:set_bus( bus )				self.__bus = bus						end

MEU.doc.get_mu = { "() return the MU corresponding to the MEU.", "mu have a symetric MU:get_meu() method." }
function MEU:get_mu()					return	self.__mu						end
function MEU:get_gp()					return	self.__mu:get_gp()				end
function MEU:get_meu_used()				return	self							end

MEU.doc.set_mu_value = {	"( val ) set the value of the MU slider, for val > 0 the MU is rendered,",
							"for graphic MU it correspond to the alpha of the MU." }
function MEU:set_mu_value( val )		self.__mu:set_value( val )				end
MEU.doc.get_mu_value = 	{	"() get the value of the MU slider." } 
function MEU:get_mu_value( val )		return self.__mu:get_value()			end

--todo do like BU BUS GP
function MEU:get_mus_down()				return self.__mus_down					end
function MEU:add_mus_down( mus )		self.__mus_down	= mus					end

function MEU:save_bu_pos( dirname )		self:get_bus():save_bu_pos( dirname )	end

--2025 Jan removed with introduction of MEU_CTX 
--function MEU.c_init()
	--tododesk
 	--MEU.create_protos()
--end

function MEU:__get_c_root_def()
	return self:__get_obj_main()
end

function MEU:create_obj_by_cid( cid, name )
	local obj = aaa.obj.create_by_cid( cid, name )
	aaa.obj.set_root( obj, self:__get_c_root_def()  )
	self:register_obj( obj )
	return obj
end
function MEU:clone_obj( src )
	local obj = aaa.obj.clone( src, self:__get_c_root_def() )
	self:register_obj( obj )
	return obj
end
function MEU:delete_obj( obj )
	aaa.obj.delete( obj )
	self:unregister_obj( obj )
end
function MEU:get_obj_by_name_symbo( name )
	local obj = aaa.obj.get_down( self.ref.__obj_main, name )
	return obj
end


--
-- UTILS
--
function MEU:call_method_checked( method, b_dialog )	-- method is always "init" for now 2025 Feb
	local b_retcode, err_str
	if method then

		if self[method] then	
			-- store start time
			local start = watchdog.get_time()
			if self.verbose > 0 then self:print( "\tbefore calling "..method.."()" ) end
			aaa.spy.push_range( method, 14 )

				-- leftover for debug it is nil now
				-- local method_in_debug = self[method.."_in_debug"]
				-- if method_in_debug then
				-- 	method_in_debug(self)
				-- 	b_retcode = true
				-- else			
					if false and aaa.pc.is_maa() then
						self[method](self)
						b_retcode = true
					else
						--self:print_inverse( "call_method_protected for "..method )	
						b_retcode, err_str = self:call_method_protected( method, b_dialog )
					end
					--self:print_inverse( "\t\t\t\t"..self.__b_inited )
				--end

			aaa.spy.pop_range()
			if self.verbose > 0 then self:print( "\tafter calling "..method.."()" ) end
			
			-- debug stuff when it is too long
			local delta = watchdog.get_time() - start
			if delta > 3. then
				delta = delta * 1000
				local str = method .. " called in "..delta.." millisecs" 
				if false then
					self:box_debug( str )
				else
					self:print_debug( str )
				end
			end
		else
			b_retcode = true
			--if b_message then
			--	self:print_debug( "\tno "..method.."() to call" )
			--end
		end
	else
		self:print_error( "call_protected_checked() method is nil" )
		b_retcode = false
	end
	return b_retcode, err_str
end


--
--	FREE
--
MEU.__verbose_free = 0
MEU.doc.free = { "() free the MEU." }
function MEU:free()
	--aaa.print_fn()
	-- remove from ui slot
	local gp = GP.cur
--	local i_slot = gp:find_ui_slot_by_meu( self )
	local i_slot = self:get_ui_slot()
	if i_slot then
		gp:remove_ui_slot( i_slot )
	end

	if self.__b_freed then
		self:print_error( "MEU:free() already freed : skipping rest of free()" )
		aaa.debug.print_traceback()
		return
	end
	self.__b_freed = true

	MEU_CTX.cur:unregister_meu( self )

	-- delete c_obj_ui created by cid or cloned and still here
	self:delete_registry_objs()

	-- now for the main obj
	if self.__b_dont_free_obj then
		self:print( "Don't free()" )
	else
		if MEU.__verbose_free > 0 then self:print_inverse( "free() begin" ) end

			local ref = self.ref
			if ref then
				local obj = ref.__obj_main
				if obj then
					--self:print( "delete "..aaa.obj.get_name(obj) )
					app:delete_obj( obj )
				end
				ref.__obj_main		= nil
				ref.__layers_main	= nil
				ref.__module_main	= nil
				--self.ref			= nil
			else
				self:print_error( "MEU:free() no ref" )
			end

		--self:box_debug( "MEU:free()" )
		if MEU.__verbose_free > 0 then self:print_inverse( "free() end" ) end
	end

	self:free_bus_and_bu()

	--todo eventually free other structure
	self.__presets	= nil
	self.ui			= nil
	self.__bu_ui	= nil
	
	--we keep it because it is need (__unregister) or for info
	--self.__meu_type			= nil
	--self.__inst_key			= nil	
	--self:set_label( nil )
	--self.__meu_render_count	= nil
	--self.__render_index		= nil
	--self:__set_dir( nil, false )
			
	--oo.getsuper(MEU).free( self )
end

--
--	NAME
--
function MEU:__update_name()
	local name = self:get_name()
	self.__meu_type, self.__inst_key = MEU_CTX.cur:split_meu_type_inst( name )

	local bu = self:get_bu_title()
	if bu then -- if or we could run into problem at init() time
		-- local label = self:get_label()
		-- if label then
		-- 	str = string.upper(label)..": "..str
		-- end
		-- self:print_inverse( str )
		bu:set_text( name )
	end
end
function MEU:__init_name( name )
	oo.getsuper(MEU).__init_name( self, name )
	self:__update_name()
end
function MEU:change_name( name )
	local old_name = self:get_name()
	--self:print( old_name.." to "..name )
	if old_name ~= name then
		MEU_CTX.cur:unregister_meu( self )
		oo.getsuper(MEU).change_name( self, name ) 
		self:__update_name()
		MEU_CTX.cur:register_meu( self )
	end
end


--
-- LUA
--
function MEU:edit_lua()
	-- if self:is_proto() then	--todo refine
	-- 	self:print( "MEU:edit_lua() meu is a Proto so edit" )
	-- 	local name = self.__lua_script_fullname
	-- 	if name then
	-- 		aaa.obj.edit_lua( name )
	-- 	else
	-- 		aaa.obj.edit_lua( self.ref.__obj_main )
	-- 	end
	-- else
	local proto = self:get_proto()
	--self:box_debug( "MEU:edit_lua() proto is "..proto.." "..proto:get_class_name() )
	if proto:is_class(MEU) then
		self:print( "MEU:edit_lua() is class MEU" )
		aaa.obj.edit_lua( proto.ref.__obj_main )
	else
		self:print( "MEU:edit_lua() is class "..proto:get_class_name() )
		GABU_OBJ.edit_lua( self )
		-- else
		-- 	self:print( "MEU:edit_lua() No proto ----> do nothing" )
		-- end
	end
end

function MEU:get_lua_filename()
	local param_ref = self.ref.lua_filename 
	if param_ref then
		return param.get( param_ref )
	end
end

function MEU:add_script( name )
	--self:box_good( "MEU:get_dir_at_creation() "..self:get_dir_at_creation() )
	--self:box_good( "MEU:build_dirname_new() "..self:build_dirname_new() )
	--self:box_good( "MEU:build_dirname() "..self:build_dirname() )
	--self:box_good( "MEU:get_dir_new() "..self:get_dir_new() )

	--local info = debug.getinfo( 2, "S" )
	--self:box_good( "info.source "..info.source )643
	--self:box_good( "SUB "..info.source:sub(3) )
	--self:box_good( "sub match "..info.source:sub(3):match( "(.*/)" ) )
	--self:box_good( "dir "..info.source:sub(2):match( "(.*/)" ) )
	--self:box_good( "file "..info.source:sub(2):match( "(.*/)" )..name )

	local l_name = self.__lua_wraps_names
	local l		 = self.__lua_wraps
	if not l_name then
		l_name = {}
		self.__lua_wraps_names = l_name
		l = {}
		self.__lua_wraps = l
	end
	if l[name] then
		self:print( "script \""..name.."\" already added, skipping" )
	else
		local ind = #l + 1
		l_name[name] = ind
		local filename = self:get_dir_absolute()..name
		--self:box_good( "script added name : "..name )

		l[ind] = LUA_WRAP:create( name, filename )
	end
end

function MEU:update_script()
	local l	= self.__lua_wraps
	if l then
		for i=1,#l do
			--self:print( "i is "..i.." "..l[i].." "..l[i]:__get_obj_main() )
			l[i]:update_then_draw()
		end
	end
end

function MEU:__init_new_lua( ref_lua, b_proto )
	local ref = self.ref
	if b_proto then
		if not ref_lua then
			self:print_error( " proto have no lua no lua" )
		else
			ref.lua	= ref_lua

			if ref.__module_main then	param.set( ref.__module_main, "module_lua",		false )
			else						param.set( ref.__layers_main, "lua",			false )
			end

			ref.lua_filename	=	param.get_ref( ref_lua, "filename"	)
			ref.lua_text		=	param.get_ref( ref_lua, "text" 		)
	--		ref.lua_edit_trig	=	param.get_ref( ref_lua, "edit_trig"	)
		--self:print( param.get(ref.lua_filename).." "..param.get(ref.lua_text) )
		--todo  true should be a strategy of optimisation set by a flag
		
		--we what to be sure it is executed there because we eventually did it before to accept sub class of MEU definition in proto
			param.set( ref_lua, "run_only_when_compiled", false )	
				self:__update_lua( true )
			param.set( ref_lua, "run_only_when_compiled", true )
		end
	else
		if ref_lua then
			param.set( ref_lua, "doit", false )
			param.set( ref_lua, "doit_trig", false )
			param.set( ref_lua, "run_only_when_compiled", false )
			param.set( ref_lua, "filename", "" )
		end
		ref.lua = self:get_proto().ref.lua
	end
end

function MEU:__update_lua( b_first )
	--self:print( "__update_lua()" )
	
	if self.__b_freed then
		if self:is_proto() then
		else
			self:print_debug( "should not happen on an instance." )
		end
	else
		local r = self.ref.lua
		if r then
			_G.meu = self
			--self:box_error( self..": __update_lua()" )
			--self:print( "\t	really __update_lua()" )
			aaa.obj.update( r )
		else
			if self.__b_no_lua_ok then	--hack avoiding FX printings errors
			else
				if b_first then
					self:print_debug( "proto have no lua. It could be an error ot not." )
				end
			end
		end
	end
end

--
--	NEW
--
function MEU:__dbg( mess, name, obj )
	if( name:sub(1,3) == "Out" ) then
		local str
		if self.ref and self.ref.lua then
			str = self:get_lua_filename()
		else
			local lua =	aaa.obj.get_branch_by_class_no_error( obj or self:__get_obj_main(), "lua_wrap" )
			if lua then
				local filename	=	param.get( lua, "filename"	)
				str = "no ref lua_filename\nbut filename\n"..filename
			else
				str = "no lua_wrap"
			end
		end

		self:box_debug( mess.."\n"..str )
	end
end

MEU.doc.create = "( obj_or_false, meu_type, inst_key, label, path ) is the internal method used to create a MEU\n"..
					"obj_or_false is ref on a c_obj_ui or false if we don't use a c_obj_ui (2025 Jan experimental for MUS:create_mu_app())\n"..
					" meu_type and inst_key will form the name\n"..
					" label is eventually used just for display to simplify the UI\n"..
					" path will be stored as the location of MEU in the filesystem and so the MUS?MU hierarchy too"					
function MEU:create( obj_or_false, meu_type, inst_key, label, path )
	if self.verbose >= 1 then
		aaa.print_fn()
	end

	local obj = obj_or_false and obj_or_false or nil
	-- some type of MEU like MEU_DIR, MEU_GRIDSEL, MEU_REF, MEU_TRAX...
	--  have an explicit class so here we try to find it and use it if found

	-- but for that we have to run the script so if the class is defined in the script it will be created
	local ref_lua =	aaa.obj.get_branch_by_class_no_error( obj_or_false, "lua_wrap" )
	if ref_lua then
		_G.meu = {}
		aaa.obj.update( ref_lua )
		param.set( ref_lua, "doit_trig", true ) -- so next time it will run
	end

	local class_name = "MEU_"..string.upper( meu_type )
	local class = aaa.lua.global.get( class_name )
	if class then
	--	aaa.print( "class_name is "..class_name )
	--	aaa.box_debug( "class is "..class )
		if self.verbose >= 1 then
			self:print( "in MEU:__create_low() for type "..meu_type.." found class: "..class )
		end
	else
		class = MEU
	end

-- CREATE
	local name = MEU_CTX.cur:build_inst_name( meu_type, inst_key )
--	MEU:__dbg( "before", name, obj )
	local self = class.create_instance( class, name, obj )
	-- self.ref table id created in create_instance,
	-- only if we pass an obj (as a ref or as a c_obj_ui name_symbo which will found an obj ) 
	--table.print( self, "Mew MEU", 2 )
--	self:__dbg( "after", name, obj )

-- some NAME done thru __init_name() butwe need todo label here
	if label then
		self:set_label( label )
	end

	--todo clarify these too
	self.__meu_render_count	= 0	-- add one every time this MEU is rendered
	self.__render_index 	= 0 -- every time a frame is rendered at the GP level and internal Gp counter is incremented
								--  this "frame counter" is passed to the MEU when rendered
								--  this is a way to know if the MEU have been rendered or not in the current frame

	self:__set_dir( path, true )

	self.__b_init_needed	= false	-- todo clean and document better 
	self.__presets			= {}	-- use to deal with presets
	self.ui					= {}	-- public table used to store bu
	self.__bu_ui			= {}	-- used for bu access, preset ...

	if obj_or_false ~= false then
		if aaa.obj.is_ref_no_error( obj ) then
			local class_str = aaa.obj.get_class( obj )
			local ref = self.ref
			-- for the moment (2025 Jan) MEU can only be build around a c_ayers or c_module
			-- but in practise most of thr MEU are done with a layers
			if 		class_str == "layers"	then
				ref.__layers_main	= obj
				ref.__obj_main		= obj
			elseif	class_str == "module"	then
				ref.__module_main	= obj
				ref.__obj_main		= obj
			else
				self:box_error( "obj passed to this MEU is not a layers or a module, but a ", class_str )
			end
		else
			self:box_error( "no obj passed to this MEU" )
		end
	end

--	self:__dbg( "after" )

	self:__init_new( ref_lua )
	--self.verbose = 0


	-- if class:get_class_name() == "MEU_DIR" then
	-- 	table.print( MEU_DIR, "MEU_DIR", 2 )
	-- 	table.print( MEU_DIR_BASE, "MEU_DIR_BASE", 2 )
	-- 	table.print( self, "self", 2 )
	-- 	self:box_debug( "Maa Debug" ) 
	-- end

	return self
end



function MEU:init()	end


function MEU:__init_new( ref_lua )
	--self.verbose = 1
	if self.verbose > 0 then aaa.print( "MEU:__init_new() Begin "..self ) end
	meu = self	-- assign global the meu  script refer to this meu

	local ref = self.ref
	if self:is_proto_and_isolated() then	-- proto isolated means that this is a "pure" proto (not an instance also)
		aaa.obj.set_root( ref.__obj_main, ga.__c_node_proto_isolated )
		if self.verbose > 0 then self:print( "__init_new() is an isolated Proto" ) end
		--	this is a proto loaded from the proto dir
		MEU_CTX.cur:register_meu( self, self )
		self:__init_new_lua( ref_lua, true )
	else
		local meu_type = self:get_meu_type()
		local proto = MEU_CTX.cur:get_proto_from_type( meu_type )
		-- if not proto then
		-- 	self:box_debug( "not proto in MEU:__init_new() for type "..meu_type )
		-- 	proto = MEU_CTX.cur:load_proto( MEU.__proto_dir_base, meu_type )	--hack was just meu_type
		-- 	if proto then
		-- 		--test it because proto are loaded at start of app from proto dirs
		-- 		self:box_debug( "this should not happened : try to load proto for "..meu_type.." from dir "..MEU.__proto_dir_base )
		-- 	end
		-- end

		if not proto then	-- we don't have a proto so it will be a proto too
			aaa.obj.set_root( ref.__obj_main, ga.__c_node_proto )
			if self.verbose > 0 then self:print( "__init_new() is its own Proto" ) end
			--	this one have no proto, this is an error or this instance is also its own proto (not from proto dir)
			MEU_CTX.cur:register_meu( self, self )
			self:__init_new_lua( ref_lua, true )
		else
			aaa.obj.set_root( ref.__obj_main, ga.__c_node_inst )
			if self.verbose > 0 then self:print( "__init_new() is only an instance" ) end
			--	we use the proto
			MEU_CTX.cur:register_meu( self, proto )
			self:__init_new_lua( ref_lua, false )
			local mt = getmetatable(self)
			-- if we want the instance to use the prototype we have to set the instance metable
			--	but for subclass or MEU (TRAX, DIR....) this is already done as the meu is created as an instance of the subclass
			if getmetatable(self) == MEU then
				--self:print( "metatable is : "..mt )
				--self:box_debug( "METATABLE" )
				local mt = {}
				--todo	check if it ok for fn could it have side effects
				if true then
					mt.__index = proto
				else
				--	ok too
					mt.__index = function( tab, key )	return tab.__proto[key]	end
				end
				setmetatable( self, mt )
			end
			--self:__update_lua()
		end
		-- called in init() called in re/define_bus()
		--	self:__build_refs()
	end

	if self.verbose > 0 then aaa.print( "MEU:__init_new() End" ) end
	--self.verbose = 0
end

MEU.doc.init_mu = "() called at the end when a mu is created so the MEU class or instance can specialzed mu."
function MEU:init_mu( mu ) end

function MEU:set_mu( tab_or_name, val )
	if type(val)=="boolean" then
		val = val and 1 or 0
	end
--	aaa.print_fn()
	if type(tab_or_name)=="string" then
		local mu = self:get_mu_by_name_no_error( tab_or_name )
		if mu then
			mu:set_value( val )
			if val <= 0. then
				-- we stop at this frame, not the next one
				-- value are observed at the beginning
				--hack we doit in this case but maa don't really undersatnd and what happen when val is 0 ?
				mu:set_render_skip_next( true )
			end
		else
			self:print_debug( "in MEU:set_mu() don't find MU with name "..tab_or_name )
		end
	else
		for _,elt in PAIRS(tab_or_name) do
			self:set_mu( elt, val )
		end
	end
end

--
-- FOCUS
function MEU:set_focus( b_flatland_force )
	aaa.print_fn()
	--aaa.debug.print_traceback()
	if b_flatland_force or aaa.flatland.is_draw_focus() then
		--aaa.print_fn()
		local obj = self:__get_obj_main()	--todo what happened when it is not the case

		if obj then
			aaa.obj.set_focus_ui( obj )
			--todoaqua refine for module
	--todocam
	--		if obj == self:get_layers() then
	--			MEU.cam = aaa.layers.get_camera( obj )
	--		end
		end
	end
end
function MEU:set_focus_on_obj()
	--aaa.print_fn()
	local obj = self:__get_obj_main()	--todo what happened when it is not the case

	if obj then
		aaa.obj.set_focus_ui( obj )
	end
end



--	FILE DIR NAME
--
--todo move in obj ? 
function MEU:__set_dir( path, b_create )
	path = string.enforce_trailing_slash( path )
	--self:box_debug( self.__dir.."\n"..path )
	--self:box_debug( ":set_dir() change dir from\n"..self.__dir.." to \n"..path )
	if b_create then
		self.__dir_at_creation	= path		--todo clean (2024 Sep) not used except in same debug test
	end
	if string.lower(path) == "dir" then
		aaa.debug.print_traceback( "dir name is dir" )
	end
	self.__dir = path
end

function MEU:__change_obj_and_branched_filename( obj, src_dir, dst_dir, b_verbose )
	local branches = aaa.obj.get_branchs( obj )
	--table.print( branches, "branches", 2 )
	for i,v in ipairs( branches ) do
		self:__change_obj_and_branched_filename( v, src_dir, dst_dir, b_verbose )
	end

	local fname = aaa.obj.get_filename( obj )
	if fname then
		if b_verbose then self:print( " -- "..fname ) end
		local str,nb = string.gsub( fname, src_dir, dst_dir )
		if nb == 1 then
			if b_verbose then self:print( "will rename to : "..str ) end
			aaa.obj.set_filename( obj, str )
		else
			self:print_debug( "nb is "..nb.." and str "..str )
		end
	else
		if b_verbose then self:print( " : No filename" ) end
	end
end

function MEU:change_filename( src_dir, dst_dir, b_verbose )
	local obj = self:__get_obj_main()
	self:__change_obj_and_branched_filename( obj, src_dir, dst_dir, b_verbose )
	self:__set_dir( dst_dir.."/", false )
end

function MEU:get_dir_up()
	return self:get_mu():get_dir_up()
end
function MEU:get_dir_at_creation()
	local path = self.__dir_at_creation
	if path then
		--self:print( "MEU:get_dir_at_creation() path is "..path )
		return path
	end
	aaa.debug.print_traceback()
	self:print_error( "get_dir_at_creation() self have no path, this should not happen" )
--	debug.debug()
end
function MEU:get_dir()			return self.__dir									end
function MEU:get_dir_no_slash()	return string.remove_trailing_slash( self.__dir )	end

function MEU:build_dirname( dir_name )
	if dir_name then
		self:box_debug( "should not happen" )
		return dir_name.."/"..self:get_dir_up()
	end
	return self:get_dir()
	--return self:get_dir_new()
end
--todo implement useful for monAqua and dior (MEU:add_script) 
function MEU:get_dir_absolute()
	--was aaa.dir.get_dir_app().."/"..self:build_dirname()
	--app:get_dir() return a relative path 
	--return app:get_dir().."/"..self:build_dirname()
	--return self:build_dirname()
	local dirname = app:get_dir_absolute()..self:get_dir_up()
	return dirname
end

--
--	SAVE
--

-- function meu:save_to_dir_direct( dirname, b_complete )
-- 	--aaa.print_fn()
-- 	--table.print( self, self.." save_to_dir_direct", 1 ) 
-- 	MEU.save_to_dir_direct( self, dirname, b_complete )
-- end

function MEU:save_to_dir_direct( dirname, b_complete )
	--aaa.print_fn()
	--self:print( "save_to_dir_direct()" )
	if self.__verbose_file > 0 then
		aaa.print_fn()
	end

	if self.__b_init_needed then
		aaa.debug.print_traceback()
		self:box_error( "Will not save a MEU not inited correctly" )
	end

	if self.verbose >= 1 then aaa.print_method() end

	local ref = self.ref

	--	LAYERS
	local err = app:save_layers_or_module( ref.__obj_main, dirname, b_complete )
	if err then
		return false
	end

	--	LABEL
	--self:print( dirname.." saved" )
	if not self:is_proto() then
		local label = self:get_label()
		if label then
			--self:print( "need to save label : "..label.."\"" )
			local text = "MU.label_next = \""..label.."\""
			aaa.file.save_text( dirname..MEU.__LABEL_FILENAME, text )
		end
	end

	--	PRESET
	if self:get_instances_nb()>0 then	-- we don't save proto when there is no instance
		self:store_preset( 0, dirname )
	end

	--aaa.print_method()
	--self:box_debug( "save_to_dir_direct" )
	if b_complete then
		self:save_presets( 1, dirname )
	--	LUA Script
		if self:is_proto() and ref.lua then
			local filename = dirname..self:get_lua_filename()
			local text = param.get( ref.lua_text )
			--self:box_error( "Will save "..filename )
			aaa.file.save_text( filename, text )
		end
	end

	return true
end
function MEU:save_to_dir( dirname, b_complete )
	if self.verbose >= 1 then aaa.print_method() end
	self:save_to_dir_direct( dirname, b_complete )
end

function MEU:save( dir_name )
	--self.__verbose_file = 0
	if self.__verbose_file > 0 then
		aaa.print_fn()
	end
	--if true then
	--	self:print( "MEU:save( \""..dir_name.."\" ) " )
	--end

	--self:print_inverse( "Bingo MEU:save() with __b_init_needed = "..self.__b_init_needed )
	--if self.__b_init_needed then
	--	table.print( self, "MEU", 2 )
	--end
	--	this is just to check and debug, should go when done


	if dir_name then
		--self:print( "MEU:save "..self:build_dirname() )
		--local mus = self:get_mu():__get_mus_up()
		--self:print( "mus is "..mus )
		--self:print( "\tdir is "..mus:get_dir() )
		--HACK
		--tododesk
		--hard really bad this name harcoded
		local app_dir = app:get_dir_absolute()
		self:print_debug( "dir_name ".. dir_name .. " -- app_dir " .. app_dir )
		local dirname_new = app_dir..self:get_dir_new()
		local dirname_old = self:get_dir_at_creation()
		if dirname_old ~= dirname_new then
			self:print_error( "    new is  "..self:get_dir_new() )
			self:print_error( "    old was "..dirname_old )
			return
		end
		local label = self:get_label()
		if label then
			self:print_debug( "label is "..label.." with name "..self:get_name() )
		end
	end

	self:save_to_dir_direct( self:build_dirname(dir_name), dir_name~=nil )
end


function MEU:open_dir()
	return self:get_mu():open_dir()
	-- local dir = self:build_dirname()
	-- local str = "explorer "..self:build_dirname()
	-- self:print( "open_dir : "..dir )
	-- --self:box_debug( dir )
	-- aaa.os.execute_process(  "explorer", dir )
	-- --aaa.os.execute_shell( "explorer", dir )
end


--
--	SAVE
-- 
-- function MEU:__save_insts( dir_name )
-- 	--self:print_inverse( "__save_insts()" )
-- 	self:apply_instances( "save", dir_name )
-- --	table.apply_fn( self:__get_instances_table(), MEU.save, dir_name )	--	the proto carries all
-- end

function MEU:save_all( dir_name )
	aaa.print_fn()
--	aaa.box_debug( "dir_name is " .. dir_name )
	-- we don't see this
	--local gp = GP.cur
	--gp.ui.bu_save_all:set_text( "Saving" )

	MEU:print_inverse( "---	save_all( "..dir_name.." ) begin" )
		MEU_CTX.cur:apply_protos( "apply_instances", "save", dir_name )
	MEU:print_inverse( "---	save_all() end" )

	GP.cur.ui.bu_save_all:begin_fx()
end

--todo do when cleaner
-- function MEU:save_insts_presets()
-- 	--self:print_inverse( "save_insts_presets()" )
-- 	table.apply_method( self.__instances, "save_presets", 1 )
-- end
-- function MEU.save_all_presets()
-- 	MEU:print_inverse( "---	save_all_presets() begin" )
-- 		table.apply_fn( MEU_CTX.cur:get_protos_table(), MEU.save_insts_presets )
-- 	MEU:print_inverse( "---	save_all_presets() end" )
-- end

function MEU.save_as_all_inst()
	aaa.print( "save_as_all_inst()" )
	local dir_name = aaa.file.do_dialog_save( "Save Meus", "" )

	if dir_name then
		aaa.print( "Picked "..dir_name )
		MEU:save_all( dir_name )
	end
end


--[[
--buggy for now, real architecture change needed, for no confusion Maa removed it
function MEU:load()
	self:print( "loaded" )
	local filename = self:build_filename()
	local complete_name = filename..".aaa_layers_all"
	if aaa.file.is_exist( complete_name ) then
		aaa.obj.set_searchable_by_filename( false )
		local err = aaa.obj.load_tree_from_file( self:__get_obj_main(), filename )
		aaa.obj.set_searchable_by_filename( true )

		if err then
			self:print_error( "err "..err.." loading file : "..complete_name )
		else
			self:load_presets()
			self:print( filename.." loaded" )
		end
	end
end
--]]

function MEU:__do_close( gp, bu )
	gp:remove_meu_from_ui_slot( self )
	--bu:print( "toto close" )
	bu:set_value( 0 )	--	we need to do that because we close and the close will stay on
						--	that's even why we have this fn
						--	we should find other way
						--	and it don't seem to be enough : when meu are back, the close but flash
						--todo solve it, go deeper
end

function MEU:get_bu_title()
	local ui = self.ui --we had to protect from not existing ui
	return ui and ui.__bu_title
end

function MEU:__load_presets_and_recall_0()
	self:load_presets()	--todoaqua avoid provoking error in aqua
	self:recall_preset( 0 )
end

function MEU:free_bus_and_bu()
	local bu = self.__bu_win
	if bu then
		local bus = bu:get_bus_up_no_error()
		self:print_debug( "have a bus "..bus )
		if bus then
			bus:remove_bu( bu )
		end
		bu:free()
		self.__bu_win = nil
	end

	self:free_bu_texture()

	local bus = self:get_bus()
	if not bus then return end

	self.__bu_group = nil
	self:__erase_bu_ui()
	--bus:free_for_meu_test_real_free()
	bus:free()
	bus.__meu_owner = nil
	self:set_bus()

end

function MEU:redefine_bus( b_init, b_skip_load )
	self:print_inverse( "redefine_bus()" )

	--hack
	--self:print( "before redefine ".. self.__b_meu_send  )
	local b_send_save = self.ui.__bu_meu_send:get_value_as_bool()
	local tab_value = self.ui.__bu_tab:get_value()
	self:print( "tab_value is "..tab_value )

	--self:print( "SO "..b_send_save )
	--hack comprendre un jour
	--if		b_send_save==1 then b_send_save = true
	--elseif	b_send_save==0 then b_send_save = false
	--end
	--self:print( "stored "..b_send_save )
	-- self.__b_meu_send = false
	self:unregister_update_ui()
	-- if MEU Bus is seen
	--todo extend to several BU_MEU using it and not only one
	local bu_up = self:get_bus():get_bu_up()

	local b_need_mus = MUS.get_cur() == nil	--made to go further in calling redefine for meu dir
	if b_need_mus then
		self:get_mu():__get_mus_up():push()
	end

		self:free_bus_and_bu()

		self:__define_bus( b_init, b_skip_load )

	if b_need_mus then
		MUS:pop()
	end

	local bus = self:get_bus()
	if bu_up then
		bu_up:add_bus_down( bus )
	end

	if tab_value then	
		self.ui.__bu_tab:set_value(tab_value)
	end
	--self:print( "after redefine send_save set to ".. b_send_save  )
	self.ui.__bu_meu_send:set_value( b_send_save )
	--self:print( "after redefine send_save        ".. self.__b_meu_send  )
	--self.__b_meu_send = b_send_save
end

