if CLASS.DECLARE( "MEU_CTX", nil, {
	__proto_dir_name 	= "AAA_PROTO",
} ) then
end

MEU_CTX:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
						"centralize the operations on a group of MEU\n"..
						"GP use it."
					)

function MEU_CTX:create( name )
	if MEU.verbose >= 1 then
		aaa.print_fn()
	end

-- CREATE
	local self = MEU_CTX:create_instance( name )
	self.__protos_by_type = {}
	self.__inst_last_by_type = {}	--	free it at some point to avoid keeping old tables

	MEU_CTX.cur = self	-- we deal only with one mow because only one now

	return self
end


--
--	NAME
--
function MEU_CTX:split_meu_type_inst( name )
	--aaa.print( "MEU_CTX.split_meu_type_inst( \""..name.."\" )" )
	local _, _, meu_type, inst_key = string.find( name, "([%w]*)_([%w_%.-]+)" )
	--aaa.print( "MEU_CTX:split_meu_type_inst() : "..name.." -> "..meu_type..", "..inst_key  )
	return meu_type, inst_key
--	return meu_type, tonumber( inst_key )
end
function MEU_CTX:build_inst_name( meu_type, inst_key )
	return meu_type.."_"..inst_key
end

MEU_CTX.__name_pattern = "[^%w_.-]"
--todo test name validity 
function MEU_CTX:is_name_valid( name )
	local str
	if name == nil then
		str = "MEU name is nil"
	elseif name:find(self.__name_pattern) then
		if name:find( ' ' ) then
			str = "MEU name\n"..name.."\nshould not contain white space."
		else
			str = "MEU name\n"..name.."\nshould contain only alphanumeric characters, underscore and dot."
		end
	end
	if str then
		ga:add_dialog_message( str )
		return false, str
	end
	return true
end
function MEU_CTX:make_name_valid( name )
	return string.gsub( name, self.__name_pattern, "" )	
end

--
--	PROTO
--
function MEU_CTX:get_protos_table()					return self.__protos_by_type							end
function MEU_CTX:get_proto_from_type( meu_type )	return self.__protos_by_type[string.lower(meu_type)]	end
function MEU_CTX:register_proto( proto )
	local meu_type = proto:get_meu_type()
	local proto_existing =  self:get_proto_from_type(meu_type)
	if proto_existing then
		if proto_existing ~= proto then
			self:box_error( meu_type.." already have a proto :\n"..proto_existing.."\ndifferent from\n"..proto.."\nskipping registration" )
			--todo we should clean here
			return
		end
	else
		self.__protos_by_type[string.lower(meu_type)] = proto
		proto.__instances = {}
		proto.__instances_nb = 0
	end
	return true
end
function MEU_CTX:unregister_proto( proto )
	proto.__instances = {}
	proto.__instances_nb = 0
	local meu_type = proto:get_meu_type_lower()
	self.__protos_by_type[meu_type] = nil
end


function MEU_CTX:free_proto( proto, b_check_no_instance )
	--aaa.print_fn()
	if not CLASS.is_gabu_obj( proto ) then
		if type(proto) == "table" then
			table.print( proto, "wrong proto" )
		end
		self:print_error( "free_proto() proto is not a Gabu_Obj : skipping free" )
		return
	end

	-- REMOVE INSTANCE
	if b_check_no_instance then
		local nb = self:get_instances_nb( proto )
		if nb > 0 then
			self:print_error( "free_proto() : "..proto.." with "..nb.." instances !!!" )
			self:apply_instances( proto, "free" )
		end
	else
		self:apply_instances( proto, "free" )
	end
	-- REMOVE PROTO
	proto:free()
	self:unregister_proto( proto )
end

function MEU_CTX:register_meu( meu, proto )
	--aaa.print_fn()
	if proto then
		if self:register_proto( proto ) then
			meu.__proto = proto
		else
			proto = nil -- to avoid to register the meu
		end
	else
		proto = meu.__proto
	end

	if proto then
		local instances = proto.__instances
		local inst_key = meu.__inst_key
		if not instances[inst_key] then
			instances[inst_key] = {}
		end
		--aaa.print_fn()	
		table.insert( instances[inst_key], meu )
		proto.__instances_nb = proto.__instances_nb + 1
	end
end

function MEU_CTX:unregister_meu( meu )
	local proto = meu:get_proto()
	--aaa.print_method()
	local instances = proto.__instances
	local inst_key = meu.__inst_key
	if instances then
		--table.print( instances, "before", 2 )
		local tab = instances[inst_key]
		--table.print( tab, "tab before", 1 )
		if tab then
			local meu_removed = array.remove_by_val( tab, meu )
			if meu_removed then
				--aaa.print_fn()
				proto.__instances_nb = proto.__instances_nb - 1
			end
			if #tab == 0 then
				instances[inst_key] = nil
			end
		else
			table.print( instances, "instances table of table" )
			self:print_error( proto.." is a MEU proto with no instances table for "..inst_key.." !!!" )
		end
		--instances[inst_key] = nil
		--table.print( instances, "after", 2 )
	else
		--aaa.debug.print_traceback()
		self:print_error( proto.." is a MEU proto with no instances table !!!" )
	end

	--if meu == proto then
	--	self:free_proto( proto, true )
	--end
end

function MEU_CTX:get_instances( meu_type, inst_key )
	local proto = self:get_proto_from_type( meu_type )
	if proto then
		local instances = self:get_instances_table( proto )
		return instances[inst_key]
	end
end

function MEU_CTX:free_protos( b_check_no_instance )
	for _,proto in PAIRS( self.__protos_by_type ) do
		if not proto.__b_freed then	-- needed because of free order in the case of self proto
			self:free_proto( proto, b_check_no_instance )
		end
	end
	self.__protos_by_type = {}
	self.__b_proto__kernel_created = false
end

MEU_CTX.doc.__get_kernel_proto_dirs = "() return a table with all the folder from the AAAkernel to parse for loading proto."
function MEU_CTX:__get_kernel_proto_dirs()
	local dirs = aaa.dir.get_dirs( self.__proto_dir_name, "MEU_PROTO*" )
	--table.print( dirs, "Protos dirs" )

	local path_start = self.__proto_dir_name.."/"  
	--path_start = aaa.dir.get_dir_kernel().."/"..path_start
	for i,name in ipairs( dirs ) do
		dirs[i] = path_start..name.."/"
	end
	--table.print( dirs, "Protos dirs" )
	--aaa.box_debug( "toto" )
	return dirs
end

function MEU_CTX:create_protos_kernel()
	self:free_protos()
	if self.__b_proto__kernel_created then
		local str = "MEU_CTX:create_protos_kernel() re enter"
		aaa.debug.print_traceback( str )
		aaa.box_debug( str )
		return
 	end

	--self:box_error( "MEU_CTX:create_protos_kernel()" )
	aaa.dir.push_def( aaa.dir.get_dir_kernel() )
		self:add_protos_in_dir( self:__get_kernel_proto_dirs() )
	aaa.dir.pop_def()
	self.__b_proto__kernel_created = true
end


function MEU_CTX:load_layers_or_module( dirname, name_for_c_obj )
	local obj, err = app:load_layers_or_module( dirname, name_for_c_obj )
	if obj then
		if MEU.verbose >= 1 then aaa.print( aaa.obj.get_class( obj ).." in "..dirname.." loaded" ) end
	else
--		aaa.print_error( "layer in "..dirname.." not loaded : "..err )
	end
	return obj
end

function MEU_CTX:load_proto( dir, meu_type )
	if MEU.verbose >= 1 then aaa.print( "\tStart load proto for MEU "..meu_type ) end
	
	local dirname = dir..meu_type.."/"
	local layers_or_module = self:load_layers_or_module( dirname, "PROTO_"..meu_type )
	
	if layers_or_module then
		if MEU.verbose > 0 then aaa.print( "\tfound layers_or_module " ) end
		local proto = MEU:create( layers_or_module, meu_type, "proto", "proto_"..meu_type, dirname )
		if proto then
			--proto:check_tag() --2024 Sep done when calling get_meu_infos_cleaned() which is more dynamic  
			if MEU.verbose > 0 then
				self:print_debug( "\tproto loaded : "..proto )
			end
			proto:__define_bus(true)
			return proto
		else
			self:print_error( "\tCould not load proto for MEU "..meu_type )
		end
	else
		--aaa.debug.print_traceback()
		if MEU.verbose > 0 then
			self:print_debug( "\tNo Proto for MEU "..meu_type )
		end
	end
end

function MEU_CTX:add_protos_in_dir( dir_or_table )
	if type(dir_or_table)=="nil" then
		return
	elseif type(dir_or_table)=="table" then
		--table.print( dir_or_table, "dir_or_table" )
		for i=1,#dir_or_table do
			self:add_protos_in_dir( dir_or_table[i] )
		end
		return
	end

	--aaa.print_fn()
	--local dir = string.remove_trailing_slash( dir_or_table )
	local dir = dir_or_table	-- it seems trailing slash are ok here but if removed cause problems later 
	if not aaa.dir.is_exist( dir ) then
		self:print_error( "add_protos_in_dir( \""..dir.."\" ) dir don't exist" )
		return
	end

	local str_post = "add_protos_in_dir ( \""..dir.."\" )"
	self:print_inverse( "Begin "..str_post )
	local nb_err = 0
	local nb_proto = 0

	local t_dir = aaa.dir.get_dirs( dir )
	--table.print( t_dir, "dirs in "..dir )
	if MEU.verbose > 0 then
		self:print( tostring( t_dir ) )
	end
	if t_dir then
		for _, meu_type in pairs( t_dir ) do
			--aaa.print( meu_type )
			if self:load_proto( dir, meu_type ) then
				nb_proto = nb_proto + 1
			else
				nb_err = nb_err + 1
			end
		end
	end

	local str = "\tLoaded "..nb_proto.." protos"
	if nb_err > 0 then
		str = str.." with "..nb_err.." errors"
		self:print( str )
		self:print_debug(   "End   "..str_post )
	else
		self:print_inverse( "End   "..str_post..": "..str )
	end
end

function MEU_CTX:apply_protos( method_name, ... )
	--aaa.print( "update_protos_all()" )
	table.apply_method( self:get_protos_table(), method_name, ... )
end

MEU_CTX.doc.update_protos_all = "() this where all the protos are updated"
function MEU_CTX:update_protos_all()
	self:apply_protos( "__update_lua" )
end


--
--	INSTANCE
--
function MEU_CTX:get_instances_table( proto )
	return proto.__instances
end
function MEU_CTX:get_instances_nb( proto )
	--self:print( "__instances_nb is "..proto.__instances_nb )
	--table.print( self:get_instances_table(proto), "instances table", 1 )
	return proto.__instances_nb
	-- local nb = 0
	-- for k,tab in PAIRS( self:get_instances_table(proto) ) do
	-- 	--todo should the proto be in the list of instance like now
	-- 	for _,inst in IPAIRS( tab ) do
	-- 		if not inst:is_proto() then
	-- 			nb = nb + 1
	-- 		end
	-- 	end
	-- end
	-- return nb
end
function MEU_CTX:get_instances_array( proto )
	local insts = {}
	--table.apply_method( tab, method_name, ... )
	for k,tab in PAIRS( self:get_instances_table(proto) ) do
		--todo should the proto be in the list of instance like now
		for _,inst in IPAIRS( tab ) do
			if not inst:is_proto() then
				table.insert( insts, inst )
			end
		end
	end
	return insts
end

--local nb = 0
function MEU_CTX:apply_instances( proto, method_name, ... )
	local t_instance = self:get_instances_table(proto)
	--table.apply_method( tab, method_name, ... )
	-- if not proto:is_proto_and_isolated() then
	-- 	self:print( "proto "..proto )
	-- 	table.print( t_instance, "instances from proto", 2)
	-- end
	for k,tab in PAIRS( t_instance ) do
		--todo should the proto be in the list of instance like now
		for _,inst in IPAIRS( tab ) do
			if not inst:is_proto_and_isolated() then
				--	self:print( "Apply "..method_name.." to instance "..inst.."  key is "..k )			
				inst[method_name]( inst, ...)
				--nb = nb + 1
			end
		end
	end
	--self:print( "apply to "..nb.." instances" )
end


-- function MEU:have_instance()
-- 	local proto = self.__proto
-- 	--table.print( proto.__instances, "proto.instances" )
-- 	for name, inst in pairs(proto.__instances) do
-- 		if name ~= "proto" then
-- 			--self:print( "yes we have an instance" )
-- 			return true
-- 		end
-- 	end
-- 	--self:print( "No instances" )
-- 	return false
-- end

function MEU_CTX:check_name_keep_type( name_new, name_old, mess_begin )
	local type_src = self:split_meu_type_inst(  string.lower( name_old ) )
	local type_dst = self:split_meu_type_inst( name_new )
	local str 
	if type_dst and type_dst ~= "" then
		type_dst = string.lower( type_dst )
		if type_src ~= type_dst then
			str = mess_begin.." MEU of type \n"..type_src.."\nusing a different type name\n"..type_dst
		end
	else
		str = mess_begin.." MEU of type\n"..type_src.."\nusing no type"
	end
	if str then
		ga:add_dialog_message( str )
		return false
	end
	return true
end
