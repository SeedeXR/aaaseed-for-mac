--	TODO
--		no error message for instances on no script
--if not MU then
--	return
--end
-- MUS perhaps should be a BUS instead of having a BUS
if CLASS.DECLARE( "MUS", nil, {
	__mu_def_sx		= 14 / 32,
	__mu_def_sy		=  3 / 32,
	__stack			= STACK:create( "MUS" ),
	__alpha			= 1,	--todo cleatr status of this alpha
	__b_mu_save		= true,
	__b_render		= true,
	__cur			= nil,
	__b_name_unique	= false,
	} ) then
	MUS:set_class_status_doc(	CLASS.STATUS.GABUZOMEU,
								"MUS handle the rendering of a group of MUs, rendering its associated MEU",
								"there is one MUS at the top level of an APP (APP_GP infact) and one for any MEU_DIR",
								"when rendering you can get the current MUS using MUS.get_cur()" )
end

-- STACK
--
function MUS:push()
	MUS.__stack:push( MUS.__cur )
	MUS.__cur = self
--	aaa.print( "MUS push "..self )
end
function MUS:pop()
--	local prev = MUS.__cur
	local cur = MUS.__stack:pop()
	MUS.__cur = cur
--	aaa.print( "pop "..prev.." now "..cur )
end
function MUS.get_cur()
	return MUS.__cur
end

function MUS:create( name, b_root, dir, mu_grid )
	-- table.print( self, "self", 1 )
	-- self:print( "is1 "..self )
	-- self:print( "is2 "..aaa.__build_str(self) )
	-- self:print( "is3 "..aaa.__build_table_str(self) )
	-- self:print( self == MUS )
		
	local str = "MUS:create( "..name.." )"
	aaa.print_fn_inverse( str )
	local self = MUS:create_instance( name )
	self.__mu_down = {}
	self.__mu_by_name = {}
	self.__mu_grid = mu_grid or .0125
	self.__mu_rendered = {}
	self.__mu_rendered_nb = 0	-- we explicitly keep count to reuse table and avoid garbage collection
	self.__mu_to_render = {}
	self.__mu_to_render_nb = 0	-- we explicitly keep count to reuse table and avoid garbage collection  
	if not dir then
		self:box_error( "No dir" )
	else
		self:__set_dir( dir )
		--aaa.box_error( "MUS:create() with dir\n"..dir.."\n"..self:get_dir_proto() )
		MEU_CTX.cur:add_protos_in_dir( self:__get_dir_proto( b_root ) )
		--table.print( MEU_CTX.cur:get_protos_table(), "Proto table" )
	end

	self.render_level_stack = STACK:create( name.."_render_level" )
	return self
end

-- FILE STUFF
--
function MUS:__set_dir(dir)		self.__dir = dir 	end
function MUS:get_dir()			return self.__dir	end

function MUS:get_dir_MEU()		return "AAA_MEU/"	end
function MUS:get_dir_inst()
	return	self:get_dir()..MUS:get_dir_MEU()	
end

function MUS:__get_dir_proto( b_search_up )
	local dir = self:get_dir()
	--self:print_debug( "__get_dir_proto() started at "..dir )
	local proto_dir_name = MEU_CTX.cur.__proto_dir_name
	local dir_proto_path = dir..proto_dir_name.."/"			-- in app dir or MEU dir
	local dirs = {}
	local id = 0
	if aaa.dir.is_exist(dir_proto_path) then
		id = id + 1
		dirs[id] = dir_proto_path
	end 

	if b_search_up then
		local dir_kernel = aaa.dir.get_dir_kernel()
		local dir_start = aaa.dir.get_dir_start()
		while true do
			--
			dir = aaa.file.get_dir_name( string.remove_trailing_slash( dir ) )
			--aaa.box_debug( dir.."\n->\n"..dir2.."\n"..aaa.dir.get_dir_kernel() )
			if dir==dir_kernel or dir=="" then
				break
			end
			dir_proto_path = dir.."/"..proto_dir_name.."/"
			--self:print_debug( "try with "..dir_proto_path )
			if aaa.dir.is_exist(dir_proto_path) then
				id = id + 1
				dirs[id] = dir_proto_path
			end
			if dir==dir_start then
				break
			end
		end
	end

	if id > 0 then
		table.print( dirs, "dirs found by __get_dir_proto()", 2 )
		return dirs
	end
end

function MUS:is_mus_root()
	return self:get_up():is_class(GP)
end
function MUS:get_dir_up()
	local dir = self:get_dir_MEU()
	local up = self:get_up()
	if not up:is_class(GP) then
		self:print( "up is "..up )
		--local mu = up:get_mu()
		local mu_dir = up:get_dir_up()
		self:print( "adding mu_dir : "..mu_dir )
		dir = mu_dir..dir
	end
	return dir
end
function MUS:get_def_fname()	return self:get_dir_inst().."/AAA_MUS_def.lua"	end

function MUS:update_mu_path( mus_dst_dir )
	mus_dst_dir = mus_dst_dir..self:get_dir_MEU()
	aaa.print_fn()
	for _, mu in PAIRS( self.__mu_down ) do
		--can't the mus up already changed
		--local src_dir = mu:get_dir_fpath()
		local meu = mu:get_meu() 
		local src_dir = meu:get_dir()
		self:print_debug( "MUS:update_mu_path()" )
		self:print_debug( "src     is "..src_dir )

		local dst_dir = mus_dst_dir..mu:get_name().."/"	
		self:print_debug( "    to dst "..dst_dir )

		--done when we move
		--local err = MU.__move_copy_files( src_dir, dst_dir, true )
		--if err then
		--		self:print_error( err.." while copying\n"..src_dir.."\nto\n"..dst_dir )
		--	else

		self:print_debug( meu.." will change_filename\n"..src_dir.."\nto\n"..dst_dir )
		meu:change_filename( src_dir, dst_dir )
	end
end

function MUS:get_mu_sxy_def()
	return self.__mu_def_sx,self.__mu_def_sy
end
function MUS:set_mu_sxy_def( sx,sy )
	self.__mu_def_sx,self.__mu_def_sy = sx,sy
end

MUS.doc.get_pos_free = "( index ) find a position for a new MU"
function MUS:get_pos_free( index )
	local sx,sy = self:get_mu_sxy_def()
	local x,y
	if self:is_mus_root() then
		x,y = app:get_gp():get_xy_new()
	else
		index = index or #(self.__mu_down)	--todo we should deal only with the one with no position
										--		and so call it only when no position saved
		local up = self:get_up()
		local fn = up.get_rect_free
		local r = fn and fn( up )
		
		if r then
			local ix = index % 4
			local iy = (index - ix) / 4
			x =	(ix+.5) * sx
			y =	r.b + .125 + (iy+.5) * sy * .6 + index * .01
		else
			x,y = 0,0
		end
	end
	return	x,y, sx,sy
end

-- un/register to be done by MUS (2025 Jan) 
function MUS:__register_mu( mu )	-- called from  MU:__rename() and MUS:__add_mu()
	local name = mu:get_name_lowercase()
	if self.__mu_by_name[ name ] then
		self:box_error( "MUS:__register_mu() there is already a MU/MEU named \""..name.."\"\n\tCan't register this one" )
		return false
	else
		self.__mu_by_name[name] = mu
		--table.print( self.__mu_by_name, 1 )
		--self:box_debug( name )

		-- do it at GP level (to be removed ?)
		local gp = app:get_gp()
		gp:__register_mu( mu )

		return true
	end
end
function MUS:__unregister_mu( mu, b_definitive )
	-- do it at GP level (to be removed ?)
	local gp = app:get_gp()
	gp:__unregister_mu( mu, b_definitive )

	local name = mu:get_name_lowercase()
	if self.__mu_by_name[ name ] then
		self.__mu_by_name[ name ] = nil
	else
		self:box_debug( "MUS:__unregister_mu() there no MU/MEU named \n"..name.."\n" )
	end
end

function MUS:is_mu_by_name( name_or_array )
	local mu
	if type(name_or_array)=="table" then	
		for _,name in ipairs(name_or_array) do
			if self:is_mu_by_name( name ) then
				return true
			end
		end
	end
	local key = string.lower(name_or_array)
	return self.__mu_by_name[key]~=nil
end

MUS.doc.get_mu_down_by_name_no_error =
	"( name, mus_to_ignore ) return the first element with the requested name.\n"..
	"The search start with the MEUs/MUs directly in this MUS,\n"..
	"  then recursively on branches (folder/MEU_DIR).\n"..
	"mus_to_ignore, an optional argument, specifies a MUS not to search in,\n"..
	" used by MEU:get_mu_by_name_no_error() when we search up recursively."
MUS.doc.get_mu_by_name_no_error =
	"( name_or_array, mus_to_ignore ) return the first element with the requested name.\n"..
	"The search start with the MEUs/MUs directly in this MUS,\n"..
	"  then recursively on branches (folder/MEU_DIR) down the MUS,\n"..
	"  then it call the MUS up passing itself as the ignore argument\n"..
	"If the first argument is an array of string, names are used one by one until a MU is found or not.\n"..
	"mus_to_ignore, an optional argument, specifies a MUS not to search in.\n"..
	"no_error version of these methods don't print or trigger any error, use it for perform test."
MUS.doc.get_mu_by_name				= MUS.doc.get_mu_by_name_no_error
MUS.doc.get_meu_by_name				= MUS.doc.get_mu_by_name_no_error
MUS.doc.get_meu_by_name_no_error	= MUS.doc.get_mu_by_name_no_error
--todo comment
function MUS:get_mu_down_by_name_no_error( name, mus_to_ignore )
	--aaa.print_fn()
	local name = string.lower(name)
	--self:print( "search "..name )
	--table.print( self.__mu_by_name, "search "..name.." in "..self..".__mu_by_name", 1 )
	local mu_by_name = self.__mu_by_name
	local mu = mu_by_name[ name ]
	if not mu then
		for _,mu_down in PAIRS(mu_by_name) do
			local meu = mu_down:get_meu()
			local mus = meu:get_mus_down()
			if mus and mus ~= mus_to_ignore then
				if meu.__b_freed then
					self:print_debug( "skip "..mus..":get_mu_down_by_name_no_error( "..name.." ) because holding meu was freed" )
				else
					mu = mus:get_mu_down_by_name_no_error( name )
					if mu then
						break
					end
				end
			end
		end
	end
	return mu
end

function MUS:get_mu_by_name_no_error( name_or_array, mus_to_ignore )
	--aaa.print_fn()
	--aaa.debug.print_traceback()
	-- if table is passed we try with all elements
	if type(name_or_array)=="table" then
		local mu
		for _,name in ipairs(name_or_array) do
			mu = self:get_mu_by_name_no_error( name, mus_to_ignore )
			if mu then
				break
			end
		end
		return mu
	end

	-- now we have a name
	local name = string.lower( name_or_array )
	local mu = self:get_mu_down_by_name_no_error( name, mus_to_ignore )

	if not mu then
		local b_debug = false
		local up = self:get_up()
		if not up then
			self:box_error( "no mu and no up" )
		elseif not up:is_class(GP) then	-- we can't go up if this is the top dir\
			-- not a GP so a MEU
			if up.__b_freed then
				local str = "MUS:get_mu_by_name_no_error()"..up.." is freed"
				if b_debug then
					self:box_error(str)
				else
					self:print_debug(str.."\nMaa should make it better" )
				end
			else
				--table.print( up, up.."is up", 1 )
				local mus_up = up:get_mu():__get_mus_up()
				if not mus_up then
					local str = "MUS:get_mu_by_name_no_error()\nno mus up for up "..up.."\nname is "..name
					if b_debug then
						self:box_error(str)
					else
						self:print_debug(str.."\nMaa should make it better" )
					end
				else
				--self:print( "no mu: search up in "..up )
					mu = mus_up:get_mu_by_name_no_error( name, self )
				end
			end
		end
		--self:print( "no mu: return nil" )
	 	--self:box_debug( "MUS:get_mu_by_name_no_error() Can't find MU "..name_or_array.." Searching down\n" )
	end
	
	return mu
end

function MUS:get_mu_by_name( name_or_array, mus_to_ignore )
	local mu = self:get_mu_by_name_no_error( name_or_array, mus_to_ignore )
	if not mu then
		table.print( self.__mu_by_name, self..".__mu_by_name", 1 )
		if type(name_or_array)=="string" then
			self:box_error( "Can't find mu with name\n"..string.lower( name_or_array ) )
		else
			table.print( name_or_array, "name_table", 1 )
			self:box_error( "Can't find mu in this table" )
		end
	end
	return mu
end

function MUS:get_meu_by_name_no_error( name_or_array, mus_to_ignore )
	local meu
	local mu = self:get_mu_by_name_no_error( name_or_array, mus_to_ignore )
	if mu then
		meu = mu:get_meu()
		if meu and meu.__b_freed then
			self:print_debug( "we found a MEU but it was freed : "..meu )
			meu = nil
		end
	end
	return meu
end
function MUS:get_meu_by_name( name_or_array, mus_to_ignore )
	local meu
	local mu = self:get_mu_by_name( name_or_array, mus_to_ignore )
	if mu then
		meu = mu:get_meu()
		if meu and meu.__b_freed then
			self:print_debug( "we found a MEU but it was freed : "..meu )
			meu = nil
		end
	end
	return meu
end

function MUS:__add_mu( mu )
	if self:__register_mu( mu ) then
		table.insert( self.__mu_down, mu )
	
		mu:__set_mus_up( self )
		self.__bus_mus:add_bu( mu )

		mu:set_grid_xy( self.__mu_grid, self.__mu_grid )	--todo for now we have the same in GP and MEU_folder
		local sxd,syd = self:get_mu_sxy_def()
		mu:set_sxy_min( sxd  ,syd   )	-- we use default size as a minimum
		mu:set_sxy_max( sxd*4,syd*8 )	--  and max too but square
	end
end
function MUS:remove_mu( mu, b_definitive )
	array.remove_by_val( self.__mu_down, mu )
	self:__unregister_mu( mu, b_definitive )
	mu:__set_mus_up( nil )
	local bus = mu:get_bus_up()
	if bus then -- needed when redef for MEU_DIR
		bus:remove_bu( mu )
	end
end
function MUS:free_mu_all()
	local mu_nb = #(self.__mu_down)
	for i = mu_nb,1, -1 do
		self.__mu_down[i]:free()
		self.__mu_down[i] = nil
	end
end
function MUS:free()
	self:free_mu_all()
	self.__bus_mus:free()
	self.__bus_mus = nil		
	self.__mu_by_name = nil
	self.__mu_rendered = nil
	self.__mu_to_render = nil

	--todo remove render rects
end

function MUS:__move_mu( mu, dir_src, dir_dst, x,y, x_local,y_local )
	aaa.print_fn()

	local verb = "moving"
	local err = MU.__move_files( dir_src, dir_dst )
	if err then
		self:print_error( err.." while "..verb.."\n"..dir_src.."\nto\n"..dir_dst )
		return false
	end

	local meu = mu:get_meu()
	meu:change_filename( dir_src, dir_dst )

	local mus_src = mu:__get_mus_up()
	if mus_src ~= self then
		-- this just move the mu in the structure
		mus_src:remove_mu( mu, false )
		self:add_mu_at( mu, x,y, x_local,y_local )
	end

	return true
end

function MUS:move_mu( mu, name_asked, x,y, x_local,y_local )
	local pre = "MUS:move_mu() "
	self:print_debug( pre..mu )
	aaa.print_fn()

	local mus_down = mu:get_meu_used():get_mus_down() 
	if self == mus_down then
		local str = "Can't move MEU inside itself"
		ga:add_dialog_message( str )
		return false
	end
	--todo check for the whole hierarchy

	local new_name = name_asked or mu:get_name()
	local b = self:check_by_name( new_name, "move" )
	if b then
		local dir_prev = app:get_dir_absolute()..mu:get_dir_up()
		dir_prev = string.remove_trailing_slash( dir_prev )
		self:print_debug( pre.."src_dir is "..dir_prev )

		--app:get_dir_absolute()..self:get_dir_up()..mu:get_dir_up()	
		local dir_next = app:get_dir_absolute()..self:get_dir_up()..new_name
		self:print_debug( pre.."dst_dir is "..dir_next )

		local mus_prev = mu:__get_mus_up()
		local x_prev,y_prev = mu:get_pos_orig_xy()

		b = self:__move_mu( mu, dir_prev, dir_next, x,y, x_local,y_local )

		aaa.print_fn()
		if b and name_asked==nil then
			local x_next,y_next = mu:get_xy()
			local undo = self:make_table_to_call_method( mus_prev, "__move_mu", mu, dir_next, dir_prev, x_prev,y_prev )
			local redo = self:make_table_to_call_method( self,     "__move_mu", mu, dir_prev, dir_next, x_next,y_next )
			ga:add_undo_redo( undo,redo )
			mu.__undo_redo_storage = nil
		end
	end
	return b
end

function MUS:add_mu_at( mu, x,y, mu_x_local,mu_y_local )
	aaa.print_fn()
	if not mu then
		return
	end

	-- we scalew relative to min size
	local sx,   sy   = mu:get_sxy() 
	local sx_m, sy_m = mu:get_sxy_min()
	local fx,   fy   = sx/sx_m, sy/sy_m

	self:__add_mu( mu )

	--self:print( "Mus def size is "..sxd.." "..syd )
	--self:print( "add_mu_at( "..mu.." ) sxyd is now "..sxd.." "..syd )

	sx_m, sy_m = mu:get_sxy_min()
	--mu:set_sxy_direct( sx,sy )
	mu:set_sxy( fx*sx_m,fy*sy_m )

	--self:print_debug( "    sxy is now "..mu:get_sx().." "..mu:get_sy() )

	-- if x then
	--  	-- x,y = mu:convert_xy_top_to_local( x,y )
	--  	-- x = x - ox
	--  	-- y = y - oy
	-- 	-- x,y = mu:convert_xy_local_to_bus_up( x,y )
	-- else
	--  	x,y = 0,0
	-- end

	--self:print_debug( "will call mu:set_xy( "..x..", "..y.." )" )
	
	mu:set_xy( x,y )

	x,y = mu:get_xy( x,y )
	if mu_x_local and mu_y_local then
		local x0,y0 = mu:convert_xy_local_to_bus_up( 0,0 )
		local x1,y1 = mu:convert_xy_local_to_bus_up( 1,1 )
		x = x - (x1-x0) * mu_x_local
		y = y - (y1-y0) * mu_y_local
	end
	
	local sx,sy = mu:get_sxy()
	if not self:is_mus_root() then
		x,y = mu:__constraint_inside_at_edge( x,y, sx,sy )
	end
	mu:set_xy( x,y )

	self:print_debug( " end sxy is now "..mu:get_sx().." "..mu:get_sy() )
	-- debug stuff
--	local sx,sy = self:get_mu_sxy_def()
--	self:print_debug( self..":add_mu_at( "..mu.." ) sxy at the end "..sx.." "..sy )

end

local b_mu_pre_2024 = true
if b_mu_pre_2024 then
	function MUS:__add_mu_special( meu_type, inst_key, x,y, ox,oy )
		local new_mu
		local dir_last = self:get_dir()
		self:__set_dir( "tmp/" )

			self:push()	--todo refine to avoid all this special case
				new_mu = self:create_mu( meu_type, inst_key )
			self:pop()

			if new_mu then
				self:add_mu_at( new_mu, x,y, ox,oy )
				new_mu:cpy_cur_to_all( "MUS:__add_mu_special" )
			end
		self:__set_dir( dir_last )
		return new_mu
	end

	function MUS.__prepare_tmp_dir( meu_type, inst_key )
		--self:print( inst_key )
		local name = MEU_CTX.cur:build_inst_name( meu_type, inst_key )
		local dir = "tmp/AAA/"..name
		local b = aaa.dir.remove( dir )	--we erase first not to accumulate data from old file
		MUS:print( "deleted "..dir.." with "..( b and "success" or "failure" ) )
		dir = dir.."/"
		MUS:print( "return "..dir )
		return dir
	end
end

--[[ called by unused make_ref
function MUS:create_mu_ref( mu, x,y, ox,oy )
	aaa.print_fn()
	if not b_mu_pre_2024 then
		self:print_error( "MUS:create_mu_ref() removed from now 2024 Jan")
	else
		aaa.print_fn()
		local meu = mu:get_meu()
		--self:print( meu_type )
		if meu:is_proto_and_isolated() then
			self:add_dialog_message( "can't create ref\non an isolated proto" )
			return
		end
		
		local meu_type = "Ref"
		local inst_key = meu:get_name()
		local name = "Ref_"..inst_key
		meu = MEU_CTX.cur:get_proto_from_type( meu_type )

		--hack using save is a quick way to do it for now
		--	temporary change the inst_key name (no "proto")
		local dir = MUS.__prepare_tmp_dir( meu_type, inst_key )

		--	don't save label
		--todofast this is strange
		local label_tmp, inst_key_tmp = meu.__label, meu.__inst_key
		meu.__label, meu.__inst_key	= label, label
			meu:save_to_dir_direct( dir, true )
			--mu:save_to_dir( dir )
		meu.__label, meu.__inst_key	= label_tmp, inst_key_tmp

		return self:__add_mu_special( meu_type, inst_key, x,y, ox,oy )
	end
end
--]]

function MUS:check_by_name( name, verb )
	--table.print( mus.__mu_by_name, mus..".__mu_by_name" )
	local str
	if not MEU_CTX.cur:is_name_valid( name ) then				-- check name validity
		str = "name is not valid"
	elseif self:is_mu_by_name( name ) then	-- check no Meu with the same name
		str = "already exist"
	end
	if str then
		str = "Can't "..verb.." MEU \n"..name.."\n"..str
		ga:add_dialog_message( str )
		return false
	end
	return true
end

--MUS:get_dir_up()
function MUS:copy_mu( mu, x,y, ox,oy )
	aaa.print_method()

	local mus_src = mu:__get_mus_up()
	local dir_src, name_src = mu:get_path_and_name()
	local b_mus_dif = self ~= mus_src
	
	local dir_dst, name_dst

	if self == mus_src then
		local meu = mu:get_meu()
		local nb = meu:get_instances_nb() 
		name_dst = name_src..(nb+1)
		--name_dst = name_src.."Inst"
		dir_dst = dir_src
	else
		name_dst = name_src
		dir_dst =  string.remove_trailing_slash( self:get_dir_inst() )
	end
	--self:print( "dst will be "..dir_dst )

	local mu_new
	if self:check_by_name( name_dst, "copy" ) then
		mu_new = mu:__copy_low( self, dir_dst,name_dst, {x,y} )
	end

	if mu_new then
		if self ~= mus_src then
			local sx,   sy   = mu:get_sxy() 
			local sx_m, sy_m = mu:get_sxy_min()
			local fx,fy = sx/sx_m, sy/sy_m
			sx_m, sy_m = mu_new:get_sxy_min()
			--mu:set_sxy_direct( sx,sy )
			mu_new:set_sxy( fx*sx_m, fy*sy_m )
		end
		return true
	end
end

MUS.doc.__create_mu_from_meu = "(meu, sx,sy) this is the only function calling MU:create()"
function MUS:__create_mu_from_meu( meu, sx,sy )
	if self.verbose >= 2 then aaa.print_method() end

	if MUS.__b_name_unique then
		if app:get_mu_by_name_no_error( meu:get_name() ) then
			table.print( meu, "meu", 2 )
			aaa.debug.print_traceback()
			self:box_error( meu.." is already registered, can't create again." )
			return
		end
	end

	self:push()

		local bus = self.__bus_mus
		bus:push()
			local x,y, nsx,nsy = self:get_pos_free( 0 )
			if not sx then
				sx,sy = nsx,nsy
			end
			local mu = MU:create( {x,y, sx,sy}, meu )
		bus:pop()
		
		self:__add_mu( mu )
		--mu:arrange_new( self, x,y );

		meu:__define_bus( true )
		--to let MEU init and specialize MU
		meu:init_mu( mu )


	self:pop()

	return mu
end

function MUS:__check_instance_exist( meu_type, inst_key )
	local inst_name = MEU_CTX.cur:build_inst_name( meu_type, inst_key )
	if self:is_mu_by_name( inst_name ) then
		self:box_error( "meu "..inst_name.." already exist : skipping creation of MEU and MU" )
		return true
	end
	return false
end

-- create MU and so associated MEU
function MUS:create_mu( meu_type, inst_key, label )
	--	aaa.print_method()
	--	PREPARE NAME
	--	inst_key are string
	--inst_key = inst_key or label
	inst_key = tostring( inst_key )
	local inst_name = MEU_CTX.cur:build_inst_name( meu_type, inst_key )
	if self:__check_instance_exist( meu_type, inst_key ) then
		self:print_error( "in create_mu() have already an instance\n"..inst_name )
		return nil
	end

	-- we get the directory from inst_name
	local inst_dir = self:get_dir_inst()..inst_name
	if not aaa.dir.is_exist( inst_dir ) then
		self:print_debug( "No dir "..inst_dir.." : did not create a mu "..inst_name )
		return nil
	end

	aaa.spy.push_range( inst_name, 14 )

		inst_dir = inst_dir.."/"
		--self:print( "inst_dir is "..inst_dir )
	-- we run a script if it is there
		aaa.file.do_if_exist_protected( inst_dir..MEU.__LABEL_FILENAME )
	-- where we get a symbolic name for interface eventually
		if not label then
			-- for compatibility with old version where label where sname
			if MU.sname_next then
				label = MU.sname_next
				MU.sname_next = nil
			end
			label = MU.label_next
			if label then
				--self:box_debug( self.." got a label "..label )
				MU.label_next = nil
			end
		end
		--if label then
		-- it had sense with sname but not with label
			-- if app:get_mu_by_name_no_error( sname ) then
			-- 	local new_name
			-- 	repeat
			-- 		new_name = meu_type.."_42"..math.random( 10000 )
			-- 	until not app:get_mu_by_name_no_error( new_name )
			-- 	self:box_error( "Can't create a MEU with an already existing sname "..sname.."\n"..
			-- 					"sname is replaced by unique "..new_name )
			-- 	sname = new_name
			-- end
		--end
	--	we have now all the names we need
		--aaa.debug.print_local()
		--self:print( "create_mu( "..meu_type..", "..inst_key..(label and (", "..label.." )") or " )" ) )

	--LOAD LAYERS OT MODULE
		--	try instance loaded in AAASeed already (regular layers)
		--	load by name in a dir
		local name_inst = MEU_CTX.cur:build_inst_name( meu_type, inst_key )
		local obj = MEU_CTX.cur:load_layers_or_module( inst_dir, name_inst )
		if not obj then
			self:print_error( "Can't find layers or module in "..inst_dir )

		--	search by symbol on AAASeed could be use for using stuff from other app
			local name_symbo = "MEU_"..inst_name
			obj = aaa.obj.get_no_error( name_symbo )
			if not obj then
				self:print_error( "no c_obj_ui found with name_symbo \""..name_symbo.."\"" )
				aaa.debug.print_traceback()
				local b = aaa.box_ask( "Open Dir ?", "Can't find layers or module in\n"..inst_dir..
										"\nand no c_obj_ui found with name_symbo\n"..name_symbo..
										"\nShould we open this dir for you ?" )
				if b then
					aaa.os.execute_process( "explorer", inst_dir )
				end
			end
		end

	--CREATE MU
		local mu
		if obj then
			local meu = MEU:create( obj, meu_type, inst_key, label, inst_dir )
			if not meu then
				self:box_error( "Can't create meu "..inst_name.." or "..meu_type )
			else
				mu = self:__create_mu_from_meu( meu )
			end
		end

	aaa.spy.pop_range()
	return mu
end

--[[
function MUS:create_mu_app( inst_key, module_name )
	--aaa.print_method()
--PREPARE NAME
	local meu_type = "APP"
	--	inst_key are string
	--inst_key = inst_key or label
	inst_key = tostring(inst_key)
	local inst_name = MEU_CTX.cur:build_inst_name( meu_type, inst_key )
	if self:__check_instance_exist( meu_type, inst_key ) then
		self:print_error( "in create_mu_app() have already an instance\n"..inst_name )
		return nil
	end

	--	we have now all the names we need
	--aaa.debug.print_local()
	--self:print( "create_mu( "..meu_type..", "..inst_key.." )" )
--CREATE MU
	local mu
	local meu = MEU:create( false, meu_type, inst_key, nil,	module_name )
	if not meu then
		self:box_error( "Can't create meu "..inst_name.." or "..meu_type )
	else
		mu = self:create_mu_from_meu( meu, .5, .1 )
	end
	return mu
end
--]]

function MUS:create_one_mu( name )
	--aaa.print_method()
	local function make_str( str )
		return "MUS:create_one_mu() in dir :\n"..self:get_dir_up().."\n"..str	
	end

 	if		name == ".git" then
	elseif 	name == "nil" then
		self:print_debug( make_str( "dir \"nil\" won't ne processed" ) )
	elseif 	name == "AAA_no" then
		self:print_debug( make_str( "dir AAA_no is there to be skipped" ) )
	else
		--aaa.print( " --> "..name )
		local meu_type, inst_key = MEU_CTX.cur:split_meu_type_inst( name )
		--aaa.print_method( "Bis" )
		if meu_type and inst_key then
			--aaa.print( "     "..meu_type.." "..inst_key )
			return self:create_mu( meu_type, inst_key )
		else
			if not inst_key then
				self:box_error( make_str( name.." can't be split in MEU type and instancem\n name could in form "..name.."_instanceName" ) )
			else
				self:box_error( make_str( name.." can't be split in MEU type and instancem\n name could be wrong" ) )
			end
		end
	end
end

function MUS:create_mu_from_dir( dir )
	if self.verbose > 0 then aaa.print_fn() end
	dir = dir or self:get_dir_inst()

	--self:box_debug( "Here" ) 

	local t_dir = aaa.dir.get_dirs( dir )
	--self:print( tostring( t_dir ) )
	--table.print( t_dir, "dirs in "..dir )
	if not t_dir then
		return
	end

	for v, name in pairs( t_dir ) do
		if self.verbose > 2 then
			self:print( "MUS:create_mu_from_dir( \""..dir.."\" ): "..v.." name -> "..name  )
		end
		self:create_one_mu( name )
	end
end

function MUS:create_mu_several( meu_type, nb )
	if not nb then nb = 1 end
	for i=1,nb do
		self:create_mu( meu_type, i )
	end
end

function MUS:import_mu( dir_src )
	aaa.print_fn()
	local dname	= aaa.file.get_dir_name	( dir_src )
	local fname	= aaa.file.get_file_name( dir_src )
	self:print( "import from dir "..dname.." with fname "..fname )

	local dir_dst = self:get_dir_inst()
	self:print( "Dst dir " .. dir_dst )

	if self:check_by_name( fname, "import" ) then
		-- get proto dir
		--	local proto = dialog_table.proto
		--	local dir_proto = proto:get_dir()
		--	self:print( "dir_proto is "..dir_proto )

		-- build new dir
		--	local gp = app:get_gp()
		--	local mus = gp:get_mus_down()
		--	local dir_new = mus:get_dir()..mus:get_dir_MEU().."/"..name
		--	self:print( "dir_inst is "..dir_new )
			
		dir_dst = dir_dst.."/"..fname
		--todo could we use aaa.file.copy
		local err = aaa.file.copy_dir( dir_src, dir_dst )
		if err then
			local str = "While copying\n"..dir_src.." to\n"..dir_dst.."\nwill remove destination and cancel import."
			aaa.dir.remove( dir_dst )
			ga:add_dialog_message( str )
			return 
		end

		local meu_type, inst_key = MEU_CTX.cur:split_meu_type_inst( fname )
		local mu = self:create_mu( meu_type, inst_key )
		--local mu = mus:create_one_mu( dir_new )		
		if mu then
			mu:arrange_new( self )
		-- 	if rect then
		-- 		mu:set_sxy( rect[3],rect[4] )
		-- 		mu:set_xy(  rect[1],rect[2] )
		-- 	end

		end
	end
end

function MUS:set_alpha( alpha )
	self.__alpha = alpha
end

--todo now that mu can change size we need to adjust this
local function sort_bt_lr(a,b)
	local ya, yb = a:get_y(), b:get_y()
	return ya==yb and a:get_x()<b:get_x() or ya<yb
end



--todomona suspend this for now, we should solve it or remove
MUS.__b_do_restore = false
--build a sorted table with all the mu to render in a rect
--	return true if there is at least one mu to render
MUS.doc.build_mu_to_render_from_rect = "( rect ) gather the MUs to render in rect and sort in the right order"
function MUS:build_mu_to_render_from_rect( rect )

	-- the protos MUS never render
	if not self.__b_render then return false end
	if not rect then
		--self:print_debug( "No rect" )
		return false
	end

	--select if it is (OM or needed) and inside the rect
	local tab = self.__mu_to_render
	local nb = 0
	for _, mu in PAIRS( self.__mu_down ) do
		if mu:is_render() and mu:is_inside_lbrt( rect ) then
			nb = nb + 1 
			tab[nb] = mu
		end
	end
	self.__mu_to_render_nb = nb

	--if self:get_name():sub(5,5) == "K"  then
	--	self:print( "build_mu_to_render_from_rect() " )
	--	table.print( self.__mu_down )
	--	table.print( tab, "selected" )
	--end
	
	if nb==0 then return false end

	if MUS.__b_do_restore then
		self.__meu_last_rendered_by_type = {}
		self.__meu_last_rendered_by_type_id = 0
	end

	--	sort vertically from bottom to top then left to right
	table.sort( tab, sort_bt_lr )
	--table.print( tab, "mu_to_render_raw", 1 )
	if self.verbose >= 2 then self:print( "build_mu_to_render_from_rect() --> "..nb.." mu" ) end

	return true
end

function MUS:__force_mu_in_rect( rect )
	if self.verbose >= 2 then self:print( "force_mu_in_rect()" ) end

	local mu_array = self.__mu_down
	--table.print( mu_array, "mu_array in __force_mu_in_rect()", 1 )
	local nb = #mu_array
	for i = 1,nb do
		local mu = mu_array[i]
		--self:print( "testing "..mu ) 
		if mu:is_inside_lbrt( rect ) then
		else
			--self:print( "force_mu_in_rect() "..mu.." is outside" ) 
			-- inside so we add it
			if self.verbose >= 3 then self:print( "\force_mu_in_rect() "..mu.." is outside" ) end
			local x,y = mu:get_xy()
			local sx,sy = mu:get_sxy()
			local f = .1
			if x < rect.l then
				x = rect.l + sx*f
			elseif rect.r < x then
				x = rect.r - sx*f
			end
			if y < rect.b then
				y = rect.b + sy*f
			elseif rect.t < y then
				y = rect.t - sy*f
			end
			mu:set_xy( x,y )
		end
	end

end

function MUS:do_render_pass_rect( rect, alpha, spy_name, spy_color_id )
	if self.verbose >= 2 then self:print( "__render_pass_lrbt() begin" ) end
	if GA.b_spy then aaa.spy.push_range( spy_name, spy_color_id ) end

	--aaa.obj.update_then_draw( fbo_layers )
	local mu_array = self.__mu_to_render
	local nb = self.__mu_to_render_nb
	if nb>0 then
		-- we have the sorted table with all the mu
		-- now we select only what is in the rect
		local level_stack = self.render_level_stack
		level_stack:erase()
		local level_0 = { mu={}, id=0 }
		level_stack:push( level_0 )
		local level_cur = level_0
		--local modifiers_id = {}
		local i_render_last
		--table.print( mu_render, "mu_to_render", 1 )

		for i = 1,nb do
			local mu = mu_array[i]
			if mu:is_inside_lbrt( rect ) then
				-- inside so we add it
				if self.verbose >= 3 then self:print( "\tdo_render_pass_rect() add "..mu ) end
				table.insert( level_cur.mu, mu )
				--table.print( level_cur.mu, self.." ite "..i )
				
				--this is for MuBegin/End
				--todo check and document
				local meu = mu:get_meu()	--opt build a meu list directly and move skip_frame to MEU
				if meu.modify_render_chain then
					if meu:modify_render_chain( level_stack ) then
						level_cur = level_stack:get_top()
					end
				end

			end
		end

--		table.print( modifiers_id, "modifiers_id", 1 )

		local to_render = level_0.mu		
		nb = #to_render
		if nb>0 then
			--table.print( to_render, "to_render for "..spy_name, 1 )
			self:push()
				local i = 1
				while i <= nb do
					local mu = to_render[i]
					--self:print( i.." -> "..mu )
					mu:render( self.__alpha * alpha  )
					-- if self.__skip_mu_nb then
					-- 	--self:print( "skipping "..self.__skip_mu_nb )
					-- 	i = i + self.__skip_mu_nb
					-- 	self.__skip_mu_nb = nil
					-- end
					i = i + 1
				end
			self:pop()
			--table.print( self.__mu_rendered, "mu_rendered", 2 )
		end
	end

	--table.print( mu_render, "mu_render", 1 )
	if GA.b_spy then aaa.spy.pop_range() end
end

function MUS:do_render( spy_color )
	-- build table with the zone (rect) to draw
	--  while computing the englobing rect
	local global_rect
	local zone_to_draw = {}
	for i,bu in ipairs(self.__render_rect) do
		local alpha = bu:get_value()
		if alpha > 0 then
			local lbrt = bu:get_lbrt()
			if not global_rect then		global_rect = table.copy_simple( lbrt )
			else						aaa.lbrt.include_lbrt( global_rect, lbrt )
			end
			bu.__mus_data = { lbrt=lbrt, alpha=alpha }
			table.insert( zone_to_draw, bu ) 
		end
	end

	self:begin_render()
	if not self:build_mu_to_render_from_rect( global_rect ) then
		return
	end

	--todo sort the rectangles but how ?
	for i,bu in ipairs(zone_to_draw) do	
		--table.print( lbrt, "lbrt" )
		local data = bu.__mus_data
		self:do_render_pass_rect( data.lbrt, data.alpha, bu:get_name(), spy_color )
	end
end

function MUS:register_meu_last_rendered_by_type( meu )
	if MUS.__b_do_restore then
		local id = self.__meu_last_rendered_by_type_id + 1
		self.__meu_last_rendered_by_type_id = id
		--self:print( "register_meu_last_rendered_by_type : ".." "..meu:get_meu_type().." "..meu.." "..id )
		local t = self.__meu_last_rendered_by_type
		t[ meu:get_meu_type() ] = { meu=meu, id=id }
	end
end

function MUS:restore_meu_last_rendered()
	if MUS.__b_do_restore then
		--aaa.print_method()
		local t = self.__meu_last_rendered_by_type
		if t then
			for _,v in pairs_sorted( t, function(t,a,b) return t[a].id < t[b].id end ) do
				local meu = v.meu
				--meu:print( "\trender for restore" )
				meu:set_restoring( true )
				meu:render()
				meu:set_restoring( false )
			end
		end
	end
end

function MUS:begin_render()
	--we keep count because we reuse table to avoid GC
	self.__mu_to_render_nb = 0
	self.__mu_rendered_nb = 0
end

function MUS:print_mu_rendered()
	for i = 1,self.__mu_rendered_nb do
		local elt = self.__mu_rendered[i]
		self:print( elt:get_name() )
	end
end

MUS.doc.add_mu_rendered = "( mu, b_rendered, b_skip_path_draw ) called by MU:render() and others to build the list of MU rendered"
function MUS:add_mu_rendered( mu, b_rendered, b_skip_path_draw )
	if self.verbose >= 3 then self:print( "\render_mu()"..mu ) end
	--self:print( "render_mu()"..mu )
	-- skip_frame can avoid rendering
	-- data used by draw_render_chain()
	--mu.__b_rendered = b_rendered
	--opt avoid table alloc
	local nb = self.__mu_rendered_nb + 1
	self.__mu_rendered_nb = nb
	local t = self.__mu_rendered[nb]
	if not t then
		t  = {}
		self.__mu_rendered[nb] = t
	end
	t[1] = mu
	t[2] = b_rendered
	t[3] = b_skip_path_draw
end

local __render_chain_phase = 0
function MUS:draw_render_chain()
	--aaa.show( MU.b_draw_hidden, "MU.b_draw_hidden" )
	--if true then return end
	__render_chain_phase = __render_chain_phase + .04	--refrence is 25 fps so we can feel speed or slow down

	local nb = self.__mu_rendered_nb
	--self:print( nb )
	if nb==0 then return end

	local t = self.__mu_rendered
	--table.print( self.__mu_rendered, "with "..nb )
	for i=1,nb do
		t[i][1].__update_path_count = 0
	end

	local a = wrap_01( aaa.time.t * 1. )
	local b = wrap_01( a + .9 )
	local f = .5 + math.sin(aaa.time.t*10) * .5
	local green = .5 + f * .5
	gol.color4( 1, green, 0, .8 )	-- orange to yellow
	local prev = nil
	local x ,y
	local xp,yp
	local offset = 0

	local bus = self.__bus_mus
	local ox,oy = bus.__x_offset_exp,bus.__y_offset_exp
	--local ox,oy = 0,0

	for i=1,nb do
		local elt = t[i]
		local mu = elt[1]
		local meu = mu:get_meu_used()
		local b_fbo = meu:is_class( MEU_FBO )

		local count = mu.__update_path_count

		x,y	= mu:get_xy()
		x,y = x+ox,y+oy
		local limit = 8
		local dx
		if count <= limit then
			dx = count
		else
			dx = limit + math.log( count-limit )	--todo do better but not urgent
		end
		x = x + (dx-1) * .05
		local sx = mu:get_sx()
		if elt[2] then
			local s = sx * .2
			if b_fbo then
				s = s * 2
			else
				s = s * ( 1. + f )
			end
			local a  = 1 - f*.5
			if i==1 or i==nb  then
				--gol.color_red( a )
				gol.color4( 1, green, 0, a )	-- orange to yellow
				if b_fbo then
					aaa.draw_disk_arc_axe_z( x,y,0, s, 0, 1, 3 )
				else
					local dy = s*3/8
					if nb == 1 then
						aaa.draw_disk_arc_axe_z( x,y-dy, 0, s, 0,  .5, 2 )
						aaa.draw_disk_arc_axe_z( x,y+dy, 0, s, .5, .5, 2 )
					else
						if i == 1 then	aaa.draw_disk_arc_axe_z( x,y-dy, 0, s, 0,  .5, 2 )
						else			aaa.draw_disk_arc_axe_z( x,y+dy, 0, s, .5, .5, 2 )
						end
					end
				end
			else
				gol.color4( 1, green, 0, a )	-- orange to yellow
				if b_fbo then
					aaa.draw_disk_arc_axe_z( x,y,0, s, 0, 1, 3 )
				else
					aaa.draw_disk_arc_axe_z( x,y,0, s, .125, 1, 4 )
				end
			end
			gol.set_line_width( 1.5 )
		else
			gol.color_red(.6)
			gol.set_line_width( BU.__draw_text_line_width * 1.5 )
			aaa.draw_mul_line( x,y, sx*.1,sx*.1 )
			--aaa.draw_circle_axe_z( x, y, 0, sx*.2, 16 )
		end

		if prev and not elt[3] then
			if prev[2] and elt[2] then
				gol.color4( 1, green, 0, .8 )
			else
				gol.color_red()
			end
			gol.set_line_width( BU.__draw_text_line_width * 1.5 )
			if true then
				aaa.draw_line_2d_stipple( xp,yp, x,y, 1, __render_chain_phase, false )
			else			
				local x0,y0 = xp,yp
				local x1,y1 = x,y
				if prev==mu then
					local dy = 0.035
					y0 = y0 + dy
					y1 = y1 + dy
				else
					--self:print( "draw link with "..meu.." "..x1.." "..y1 )
					local dx,dy = xp-x, yp-y
					local n = math.sqrt(dx*dx+dy*dy)
					local f = 0.03
					dx,dy = f*dx/n, f*dy/n
					x0 = x0-dx
					y0 = y0-dy
					x1 = x1+dx
					y1 = y1+dy
				end
				local xa = interpolate( x0,x1, a )
				local ya = interpolate( y0,y1, a )
				local xb = interpolate( x0,x1, b )
				local yb = interpolate( y0,y1, b )
				if a<b then
					aaa.draw_line( xa,ya, xb,yb )
				else
					aaa.draw_line( x0,y0, xb,yb )
					aaa.draw_line( xa,ya, x1,y1 )
				end
			end
		end

		prev = elt
		xp,yp = x,y
		mu.__update_path_count = count + 1
	end
	
end

function MUS:__draw_on_top( b_draw_render_chain )
	gol.set_depth( false )
	gol.set_texture_dim( 0 )

	if b_draw_render_chain and GP.__b_draw_render_chain then
		self:draw_render_chain()
	end
end

function MUS:is_render()		return self.__b_render	end
function MUS:set_render( b )	self.__b_render = b		end

function MUS:set_mu_save( b )		self.__b_mu_save = b	end
function MUS:save_to_dir( dirname, b_complete )
	--aaa.file.save_text( self:get_def_fname(), text )
	--self:box_debug( "MUS:save_to_dir()\n"..dirname.."\n"..self:get_def_fname() )
	dirname = dirname .. self:get_dir_MEU()
	if self.__b_mu_save then
		self:apply_method_to_mu( "save_inside_dir", dirname, b_complete )
	end
	local up = self:get_up()
	--if up then 
		up:save_bu_pos( dirname )
	--end
end
function MUS:save_top_level()
	local fname = self:get_def_fname()
	local str = "tab.render_rect_nb = "..#(self.__render_rect).."\n"
	aaa.file.save_text( fname, str )
end
--todo we should refine name
function MUS:apply_method_to_mu( method, ... )
	local ret = false
	self:push()
		local t = self.__mu_down
		for i = 1, #t do
			local mu = t[i]
			--mu:print( i )
			local l_ret = mu[method]( mu, ... )
			--self:print( mu.."ret is "..l_ret )
			ret = ret or l_ret
		end
	self:pop()
	--self:print( "ret is "..ret )
	return ret
end
function MUS:apply_fn_to_mu( fn )
	self:push()
		local t = self.__mu_down
		for i = 1, #t do
			--self:print( "mu "..i.." "..t[i] )
			fn( t[i] )
		end
	self:pop()
end

function MUS:hide_mu_unused()
	aaa.show( "hide_mu_unused()", self ) 
	self:apply_fn_to_mu( function(mu) mu:set_hide( mu:get_value()<=0 ) end )
end

function MUS:mark_by_match_pattern( pat )
	return self:apply_method_to_mu( "mark_by_match_pattern", pat )
end
function MUS:mark_by_type( meu_type )
	return self:apply_method_to_mu( "mark_by_type", meu_type )
end

function MUS:__attach_bus( bus )
	self.__bus_mus = bus
	--self:box_debug( "Dir is "..self:get_dir_inst() )
	--bus:set_dir( self:get_dir_inst() )
	bus:set_drop_receiver( self )
end

function MUS:add_define( bus )
	self:__attach_bus( bus )

	self:push()
		self:create_mu_from_dir()
--		self:add_mu_bus()
	self:pop()
end

function MUS:begin_define()
	self:push()

	local filename = self:get_def_fname()
	local def = { render_rect_nb = 3 }
	if aaa.file.is_exist(filename) then
		-- table.print( def )
		app:dofile( filename, def )
		-- table.print( def )
		-- self:box_debug( filename )
		--aaa.lua.dofile_protected( filename, b_dialog )
	end

	local bus = BUS:create( self:get_name() )
	bus:set_fname( MUS:get_dir_MEU().."AAA_MUS" )
	self:__attach_bus( bus )

	bus:init_begin()
	bus:set_bu_pos_load_save(true)

	self.__render_rect = {}
	for i=1,def.render_rect_nb do
		self:insert_render_rect_at( bus, i )
	end
	self:create_mu_from_dir()

	return bus
end
function MUS:end_define()
	--	add rest

	self.__bus_mus:init_end()
--	self:add_mu_bus()
	self:pop()
end

function MUS:__name_render_rect()
	for i,bu in ipairs(self.__render_rect) do
		local name = "Render_"..i
		if name ~= bu:get_name() then	-- avoid message
			bu:change_name( name )
		end
	end
end
function MUS:insert_render_rect_at( bus, i, rect )
	rect = rect or {} 
	bus:push()
		rect[1] = rect[1] or 0
		rect[2] = rect[2] or i/4
		rect[3] = rect[3] or 1
		rect[4] = rect[4] or 1

		local bu = BU_RECT:create( "Render_"..i, rect )
			bus:add_bu( bu )
			bu:set_dplane( -42 )
			bu:__set_mus_up( self )
			table.insert( self.__render_rect, i, bu )
	bus:pop()
	self:__name_render_rect()
end
function MUS:insert_render_rect( bu )
	local i_dst = array.find_index_by_val( self.__render_rect, bu )
	if i_dst then
		local x,y, sx,sy = bu:get_xy_sxy()
		local d = .25
		self:insert_render_rect_at( self.__bus_mus, i_dst+1, {x-d,y+d, sx,sy} )
	end
end
function MUS:remove_render_rect( bu )
	local rects = self.__render_rect
	if #rects > 1 then
		self.__bus_mus:remove_bu( bu )
		array.remove_by_val( rects, bu )
		self:__name_render_rect()
	end
end
function MUS:inc_render_rect( bu, inc )
	local rects = self.__render_rect
	local i_src = array.find_index_by_val( rects, bu )
	if i_src then
		local i_dst = i_src + inc
		if inside( i_dst, 1, #rects ) then
			rects[i_src],rects[i_dst] = rects[i_dst],rects[i_src]
			self:__name_render_rect()
			return true
		end
	end
	return false
end
function MUS:get_render_rect_by_number( id )
	return self.__render_rect[id]
end
