--
--BUS
--
if
	CLASS.DECLARE( "BUS",
		nil,
		{
			__image_name = {
				refresh = "Texture/But/Refresh.png",
				home = "Texture/But/Home.png",
				print = "Texture/But/Print.png",
				next = "Texture/But/forward.png",
				prev = "Texture/But/backward.png"
			},
			__stack = STACK:create("BUS"),
			__b_drop_can_receive = false,
			__s_constraint = "center", --	can be "none", "edge", "center"
			__b_move_to_back = true,
			__b_move_to_front = true,
			__window_def_x = -3.,
			__window_def_y = 0,
			__window_def_sx = .6,
			__window_def_sy = 3.,
			__window_def_step_x = 0.25,
			__b_bu_size_save = true,
			__b_need_top_image_by_page_or_domino = false,	-- use to desactivate depreciated code
			
	} ) then
	aaa.lua.global.declare("bus_cur")
	BUS:set_class_status_doc(	CLASS.STATUS.GABU,
								"this is where BU are stored, it give them a coordonnate system, a BU can also have a BUS inside itself.",
								"a whole bunch of functions are provided to navigate up and down THE BUS/BU/BUS/BU... structure",
								"global bus_cur is used to access the current bus used at anytime"
								 )
end

-- PUSH/POP to deal easily with bus_cur
--
function BUS:push()
	BUS.__stack:push(bus_cur)
	bus_cur = self
	if self.verbose >= 3 then
		BUS.__stack:print_fn("push")
	end
end
function BUS:push_for_change()
	if self.__b_in_init == nil then
		self:push()
	end
end
function BUS:pop()
	if self.verbose >= 3 then
		BUS.__stack:print_fn("pop")
	end
	bus_cur = BUS.__stack:pop()
end
function BUS:pop_for_change()
	if self.__b_in_init == nil then
		--local t_dplane = self.dplane[ bu:get_dplane() ]
		self:__do_post_init_end() -- t_dplane )
		self:pop()
	end
end
function BUS:get_stack()
	return BUS.__stack
end
--function BUS.get_cur()		return bus_cur		end


--todo	used APP_channel/Appear could be scrapped
function BUS:apply_bu_page( fn, nb )
	local page = self:get_page()
	nb = nb or self:get_bu_nb()
	for i = 1, nb do
		local bu = self:get_bu(i)
		if bu.__page == page then
			fn(bu)
		end
	end
end

function BUS:create( name, cam_name_symbo )
	if not name then
		self:box_error( "BUS:create() should be :" .. '\nBUS:create( "Name" )' .. "\nDefaulting to NoName." )
		name = "NoName"
	end

	local self = BUS:create_instance( name )
	--if self.verbose >= 1 then self:print_inverse( "create()" ) end
	if self.verbose >= 1 then
		if cam_name_symbo then
			self:print_inverse("\tcreate() with cam " .. cam_name_symbo)
		else
			self:print_inverse("\tcreate()")
		end
	end

	self.__x_offset_exp,self.__y_offset_exp = 0,0
	self.__x_offset_dt = 0
	self.__x_offset_target,self.__y_offset_target = self.__x_offset_exp,self.__y_offset_exp
	self.__x_offset_start,self.__y_offset_start = 0,0

	self.__page_sx = 8.
	self.__page = -42 --to be sure to trigger init on bus_begin()
	self.__page_min = 10000
	self.__page_max = -10000

	self.__b_in_init = nil

	self:init_dplane()

	self:set_active_direct(true)
	--	self.b_save_bu_pos = true

	if cam_name_symbo then
		self.__cam = AAACAM:create(cam_name_symbo, aaa.obj.get_no_error(cam_name_symbo))
	else
		self.__cam = ga.cam
	end

	return self
end

--todo	
function BUS:free_for_meu_test_real_free()
	self:__destroy_all_bu()
end

--todo check if removed from GA.__busses
function BUS:free()
	self:free_for_meu_test_real_free()
	local bu_up = self:get_bu_up()
	if bu_up then
		bu_up:remove_bus_down()
	end
end

--	PAGE
--
function BUS:get_page()
	return self.__page
end
--function BUS:set_page_low(page_index)
--	self.__page = page_index
--	self.__x_offset_exp = -page_index * self.__page_sx
--	self.__x_offset_target = self.__x_offset_exp
--	self:print( "Change page low to "..page_index )
--end
function BUS:set_function_on_set_page(fn)
	self:set_function( "set_page", fn )
end
function BUS:set_page( page_index )
	local last_page = self.__page
	if self.__b_in_init then
		self.__page_min = math.min( self.__page_min, page_index )
		self.__page_max = math.max( self.__page_max, page_index )
		self.__x_offset_exp = -page_index * self.__page_sx
	else
		--arte
		if page_index < self.__page_min then
			page_index = self.__page_max
		elseif self.__page_max < page_index then
			page_index = self.__page_min
		end
		self.__x_offset_dt = ga.__page_change_time
		self.__x_offset_target = -page_index * self.__page_sx
	end
	self.__page = page_index

	if last_page ~= page_index then
		if self.verbose >= 1 then
			self:print( "Change page to " .. page_index )
		end
		self.__bu_xy_def = {x = -4, y = 2}
		self.__bu_xy_pb = {x = 4, y = -2.5}
		self:do_fn( "set_page", self.__page )
	end
end
function BUS:get_page_nb()	return math.max(0, self.__page_max - self.__page_min + 1)	end
function BUS:inc_page()		self:set_page( self.__page + 1 )							end
function BUS:dec_page()		self:set_page( self.__page - 1 )							end

--	DPLANE
--
function BUS:init_dplane()
	local t = { i_depth = 0, __bu = {} } -- default table with all the BU in dplane 0
	self.__dplanes = { t }
end

function BUS:find_dplane_by_depth(depth)
	local dplanes = self.__dplanes
	for i = 1, #dplanes do
		local dplane = dplanes[i]
		if dplane.i_depth == depth then
			return dplane
		end
	end
	return nil
end

--	remove a BU from it's actual dplane
function BUS:remove_bu_from_dplane(bu)
	--	Get BU dplane (0 default)
	local depth = bu:get_dplane()
	--	Get a table with all the BU in this dplane
	local dplane = self:find_dplane_by_depth(depth)
	--	Find the position of the BU in this table
	local index = table.find_key_by_val(dplane.__bu, bu)
	if not index then
		aaa.print_error( "No bu to remove in this dplane " .. depth )
		return false
	end

	if self.verbose >= 1 then
		self:print( "remove " .. bu .. " of index " .. index .. " in dplane " .. depth )
	end
	--Remove the BU from the table by its index
	table.remove( dplane.__bu, index )
	return true
end

--	add a BU in a new dplane
function BUS:add_bu_to_dplane(bu, depth)
	--aaa.print_fn()

	--	Get dplane table asked
	local dplane = self:find_dplane_by_depth(depth)
	--	If there is no BU in this dplane
	if not dplane then
		--	Create the table for this dplane
		--		with the bu already there
		dplane = { i_depth = depth, __bu = {bu} }
		table.insert( self.__dplanes, dplane )
		--	Sort the table of dplane's index
		table.sort(
			self.__dplanes,
			function(a, b)
				return a.i_depth < b.i_depth
			end
		)
	else
		--	Add the BU in the new dplane
		table.insert( dplane.__bu, bu )
	end
	--[[
	--Print all the BU in the dplane
	for k, v in pairs(self.dplane[i_dplane]) do
		self:print(k.." ; "..v)
	end
--]]
end

function BUS:build_dplane_str(depth)
	local str = "dplane : "
	for k, v in pairs(depth) do
		str = str .. v .. " "
	end
	return str
end
--	Print all BU from back to front in the dplane
function BUS:print_dplane(depth, str)
	local s = "back to front : { " .. self:build_dplane_str(depth) .. " }"
	if str then
		s = s .. " " .. str
	end
	self:print(s)
end

--[[
--Reverse all position of the BU in the dplane
function BUS:init_dplane_reverse( dplane )
	dplane = table.reverse(dplane)
	if self.verbose >= 2 then self:print_dplane( dplane, "init_dplane_reverse" ) end
end
--]]
--
--	BU
--
function BUS:get_bu_nb()		return self:get_down_nb()						end
function BUS:get_bu(index)		return self:get_down(index)						end
function BUS:get_bu_up()		return self:get_up_by_class(BU)					end
--2024 July unused
--function BUS:get_bu_index(bu)	return array.find_index_by_val(self.__down, bu)	end

--2024 July unused
--[[
function BUS:get_bu_index_pos(bu)
	local down = self.__down
	--	table.print( self.__down )
	--	local pos_sorted
end
--]]

--2024 July unused
--[[
function BUS:sort_bu_rects()
	local down = table.copy_deep(self.__down)
	local sort_l = {}
	local sort_r = {}
	local sort_b = {}
	local sort_t = {}

	for k in pairs(down) do
		local min_l = math.huge
		local min_r = math.huge
		local min_b = math.huge
		local min_t = math.huge
		local last_picks = {}

		for l in pairs(down) do
			if k < l then
				local rect = down[l]:get_lbrt_cur()
				if rect.l < min_l then
					min_l = rect.l
					sort_l[k] = down[l]
				end
				if rect.r < min_r then
					min_r = rect.r
					sort_r[k] = down[l]
				end
				if rect.b < min_b then
					min_b = rect.b
					sort_b[k] = down[l]
				end
				if rect.t < min_t then
					min_t = rect.t
					sort_t[k] = down[l]
				end
			--table.print( rect )
			end
		end
		print("l : " .. min_l .. ", r : " .. min_r .. ", b : " .. min_b .. ", t : " .. min_t)
	end

	for k in pairs(sort_l) do
		--	print( "sort_l["..k.."] : "..sort_l[k]:get_lbrt_cur().l )
	end
	--	self:print( self.__down[ self:get_bu_index( bu ) ] )
end
--]]

--todo improve
function BUS:found_bu_next( bu_begin, direction )
	aaa.print_fn()
	--self:sort_bu_rects()
	local down = self:get_down_array_active()
	--table.print( down )
	local bu_found
	--table.print( self.__down:get_lbrt_cur() )
	local array = {}

	local function compute_bu_min_distance(bu_begin, bu)
		local a = V2.new_from( bu_begin:get_xy() )
		local b = V2.new_from( bu:get_xy() )
		local asx = bu_begin:get_sx() * 0.5
		local asy = bu_begin:get_sy() * 0.5
		local bsx = bu:get_sx() * 0.5
		local bsy = bu:get_sy() * 0.5
		local function get_edge_points(cx, cy, sx, sy)
			return {
				{ cx - sx, cy - sy }, -- bottom left
				{ cx,      cy - sy }, -- bottom center
				{ cx + sx, cy - sy }, -- bottom right
				{ cx - sx, cy 	   }, -- middle left
				{ cx + sx, cy 	   }, -- middle right
				{ cx - sx, cy + sy }, -- top left
				{ cx,      cy + sy }, -- top center
				{ cx + sx, cy + sy }, -- top right
			}
		end
		local function get_edge_middle_points(cx, cy, sx, sy)
			return {
				{ cx,       cy - sy }, -- bottom center
				{ cx - sx, cy 		}, -- middle left
				{ cx + sx, cy 		}, -- middle right
				{ cx,       cy + sy }, -- top center
			}
		end
		local distances = {}
		local a_points = get_edge_middle_points(a[1], a[2], asx, asy)
		local b_points = get_edge_points(b[1], b[2], bsx, bsy)
		for _, ap in ipairs(a_points) do
			for _, bp in ipairs(b_points) do
				table.insert(distances, V2.dist(ap, bp))
			end
		end
		if #distances > 0 then
			local min = distances[1]
			for i = 2, #distances do
				if distances[i] < min then
					min = distances[i]
				end
			end
			return min
		end
		return nil
	end

	if direction == "right" then
		local r = bu_begin:get_lbrt_cur().r
		--we select all the bu on the right	 
		for _, bu in ipairs(down) do
			local rect = bu:get_lbrt_cur()
			if r < rect.l then
				table.insert( array, bu )
			end
		end
	elseif direction == "left" then
		local l = bu_begin:get_lbrt_cur().l
		for _, bu in ipairs(down) do
			local rect = bu:get_lbrt_cur()
			if l > rect.r then
				table.insert( array, bu )
			end
		end
	elseif direction == "up" then
		local t = bu_begin:get_lbrt_cur().t
		for _, bu in ipairs(down) do
			local rect = bu:get_lbrt_cur()
			if t < rect.b then
				table.insert( array, bu )
			end
		end			
	elseif direction == "down" then
		local b = bu_begin:get_lbrt_cur().b
		for _, bu in ipairs(down) do
			local rect = bu:get_lbrt_cur()
			if b > rect.t then
				table.insert( array, bu )
			end
		end	
	else
		self:print_error( "Unknown direction : "..direction ) 
	end
	
	if #array > 0 then
		local min_dist = math.huge
		for _, bu in ipairs(array) do
			local dist = compute_bu_min_distance( bu_begin, bu )
			if dist < min_dist then
				min_dist = dist
				bu_found = bu
			end
		end
	end

	if bu_found then
	else
		self:print( "No bu found in direction "..direction ) 
	end

	return bu_found
end

function BUS:get_next_bu_pos(b_def)
	local t
	local x, y
	if b_def then
		t = self.__bu_xy_def
		x, y = t.x, t.y
		x = x + 1
		if x > 3 then
			x = -3
			y = y - 1
			if y < -2 then
				y = 2
			end
		end
	else --position when there is a problem
		t = self.__bu_xy_pb
		x, y = t.x, t.y
		x = x - .5
		if x < -3.5 then
			x = 4
		end
	end
	t.x, t.y = x, y
	return x, y
end

function BUS:find_bu_by_key_pair( key1, val1, key2, val2 )
	return self:test_down(
		function(self)
			local v = self[key1]
			if v and v == val1 then
				v = self[key2]
				if v and v == val2 then
					return self
				end
			end
		end
	)
end

--todo move to GABU_OBJ
function BUS:find_bu_by_name_lowercase( bus_name, bu_name )
	--self:print( "going in BUS" )
	if not bus_name then
		local found = self:find_in_down_by_name_lowercase(bu_name)
		if found then
			return found
		end
	elseif self:is_name_lower(bus_name) then	---is_sname/label
		--self:print_inverse( "BUS have right name "..self.." now searching BU named "..bu_name )
		--table.print( self.__down, "bus found", 1 )
		local found = self:find_in_down_by_name_lowercase(bu_name)
		if found then
			--self:print_inverse( "in BUS with right name : we found "..found )
			return found
		end
	end
	--table.print( self.__down, "BUS:find_bu_by_name_lowercase()", 1 )
	local found = self:test_down( function(self) return self:find_bu_by_name_lowercase( bus_name, bu_name ) end )
	--self:print( "going out BUS found " .. found )
	return found
end

function BUS:mark_by_match_pattern( pat )
	--self:print( "mark process with pat " .. pat )
	self:apply_method_down( "mark_by_match_pattern", pat )
end

function BUS:set_move_to_back(b)	self.__b_move_to_back = b		end
function BUS:is_move_to_back()		return self.__b_move_to_back	end

function BUS:set_move_to_front(b)	self.__b_move_to_front = b		end
function BUS:is_move_to_front()		return self.__b_move_to_front	end

function BUS:__move_to( bu, b_front )
	-- aaa.print_fn()
	local fn_name
	if b_front then
		if not self:is_move_to_front() then return end
		fn_name = "move_to_front"
	else
		if not self:is_move_to_back() then return end
		fn_name = "move_to_back"
	end
	if not bu then
		self:print_error( fn_name.."() called with nil" )
		return
	end

	if self.verbose >= 2 then
		self:print( "try "..fn_name.."( " .. bu .. " " .. b_front .. ")" )
	end
	-- find and check if moving is needed
	-- Get the dplane of the BU
	local depth = bu:get_dplane()
	-- Get the table with all BU in this dplane
	local dplane = self:find_dplane_by_depth(depth)

	-- Find the index of the BU in this table
	local index = table.find_key_by_val(dplane.__bu, bu)

	-- If index of the BU is the last of this table then no need to move this BU
	if b_front and index == 1 or index >= #dplane.__bu then
		if self.verbose >= 1 then
			self:print( "no need "..fn_name.."( " .. bu .. " ) from index " .. index)
		end
		return
	end

	-- verbose
	if self.verbose >= 1 then
		self:print( fn_name.."( " .. bu .. " ) from index " .. index )
	end
	if self.verbose >= 3 then
		self:print_dplane(dplane.__bu, "before")
	end

	-- Remove the BU from the table
	table.remove( dplane.__bu, index )

	if self.verbose >= 3 then
		self:print_dplane(dplane.__bu, "after remove")
	end

	if b_front then
		table.insert( dplane.__bu, bu )
--		table.insert( dplane.__bu, dplane.__bu[1] )
	else
		table.insert( dplane.__bu, 1, bu )
	end
	--	ga:set_ui_intercept( nil )
	--	if bu.obj and not aaa.keyboard.is_alt_only() and bu.obj.ref.focus then
	--		aaa.obj.set_focus_ui( bu.obj.ref.draw )
	--		ga:set_ui_intercept( bu.obj.ref.focus )
	--	end

	if self.verbose >= 3 then
		self:print_dplane( dplane.__bu, "now" )
	end
end

function BUS:move_to_front(bu)	return	self:__move_to( bu, true )	end
function BUS:move_to_back(bu)	return	self:__move_to( bu, false )	end


--
--	GENERIC PARSE
--
BUS.doc.init_begin = "() init_end() surround the definition of bu in a bus"
function BUS:init_begin()
	self:push()
	if self.verbose >= 1 then	self:print( "init_begin_add_to_ga()" )	end
	self.__b_in_init = true
	self:set_page( 0 )
	self:set_tab_key(nil)
end

BUS.doc.init_begin_add_to_ga = "() do the same than init_begin() but also add it to the bis the ga should handle\n"..
								"if only init_begin is called the bus is not considered unly hooked also to the ga" 
function BUS:init_begin_add_to_ga()
	self:init_begin()
	ga:add_bus(self)
end

function BUS:__do_post_init_end() -- dplane )
	--self:init_dplane_reverse( dplane )
end

--todo deal with b_skip_load
function BUS:init_end( b_skip_load )
	--aaa.print_fn()
	self.__b_in_init = nil
	--hack	check ok now in add_bu
	--[[
	--todo do it the recent way
	for i = 1, self:get_bu_nb() do
		self.get_bu(i):init_post()
	end
--]]
	if self.verbose >= 1 then
		self:print( "init_end()" )
		if self.verbose >= 3 then
			table.print(self, "BUS")
			self:print( "init_end() AFTER TABLE" )
		end
	end

	self:set_page(0)
	self:set_tab_key( nil )
	self:__do_post_init_end() -- self.dplane[0] )
	if not b_skip_load and not aaa.keyboard.is_shift() then
		--self:print( "load_bu_pos" )
		self:load_bu_pos()
	end
	BUS:pop()
end

function BUS:reset_all_page()
	self:apply_method_down(	"reset"	)
	aaa.mess.show("ALL PAGES RESET" .. "\nAAATouch By Maa\n  and Franz Hildgen\nVersion GaBu v0.5\nmaa@lagraine.com")
end
function BUS:reset_page_current()
	local page = self.__page
	self:apply_fn_down(
		function(self)
			if self.__page == page then
				self:reset()
			end
		end
	)
	--aaa.mess.show( "CURRENT PAGE RESET".."\nAAATouch By Maa\n  and Franz Hildgen\nVersion GaBu v0.5\nmaa@lagraine.com" )
end

function BUS:get_contact_nb_down()
	local nb = 0
	self:apply_fn_down(	function(self)	nb = nb + self:get_contact_nb_down()	end )
	return nb
end
function BUS:clean_contact()
	--self:print( "clean_contact()" )
	self:apply_method_down( "clean_contact" )
end

function BUS:subscribe_to_click(bu, fn)
	if not self.subscriber_click then
		self.subscriber_click = {}
	end
	bu.receive_click_on = fn
	table.insert( self.subscriber_click, bu )
end
function BUS:dispatch_click(bu, click_repeat_nb)
	if self.subscriber_click then
		for key, subs in pairs(self.subscriber_click) do
			subs:receive_click_on( bu, click_repeat_nb )
		end
	end
end

function BUS:find_by_blob_id(blob_id)
	if self.verbose >= 2 then	self:print( "find_by_blob_id( " .. blob_id .. " )" )	end
	return self:test_down(
		function(self)
			return self:find_by_blob_id(blob_id)
		end
	)
end

--todo bus:find_by_blob_id go down
--	I should keep a list of bu in contact
function BUS:deal_with_untouch(blob_id)
	if self.verbose >= 2 then	self:print_inverse("BUS:deal_with_untouch( " .. blob_id .. " )" )	end
	self:push()
		local b_blob_used
		local bu = self:find_by_blob_id(blob_id)
		if bu then
			--self:print( "Blob "..ga:get_blobs().untouch[i].." touch image "..index )
			bu:remove_contact(blob_id)
			b_blob_used = true
		else
			if self.verbose >= 1 then
				self:print("untouch do not correspond to a BU")
			end
			b_blob_used = false
		end
	self:pop()
	return b_blob_used
end

--
--	DROP
--
function BUS:set_drop_receiver( drop_receiver )
	self.__drop_receiver = drop_receiver
end
function BUS:get_drop_receiver()
	local receiver = self.__drop_receiver
	if receiver then
		return receiver
	end
end

--[[
function BUS:find_bus_or_bu_to_drop( x, y )
	self:print( "BUS:find_bus_or_bu_to_drop "..x.." "..y )
	--if self:can_drop() then
		self:print( "Can drop " )
		local found
		self:push()
			local bu = self:find_bu_active_by_pos( x,y )
			if bu then --and bu:can_drop() then
				--found = bu
				self:print( "Find bu "..bu )
				local bus_down = bu:get_bus_down()
				if bus_down and bu.__b_bus_down_active_ui and (not bu:is_mini()) then
					local nx,ny = bu:convert_xy_bus_up_to_local( x,y )
					if self.verbose >= 2 then self:print( bu.." find_bus_to_drop "..x.." "..y ) end
					nx, ny = bus_down:do_transfo( nx, ny )
					local found_down  = bus_down:find_bus_or_bu_to_drop( nx,ny )
					if found_down then
						self:print( "Find down "..found_down )
						found = found_down
					end
				end
			end
			if not found and self:can_drop() then
				found = self
			end
		self:pop()
		return found
	--end
end
--]]

function BUS:find_bu_active_by_pos( x,y )
	--aaa.debug.print_traceback()
	--self:print( "hack is ".. self.__b_hack_maa )
	--aaa.print_fn( )
	if not self:is_active() then
		return nil
	end
	--	done in is_inside() now
	--x =  x - self.__x_offset_exp
	--y =  y - self.__y_offset_exp
	if self.__b_hack_maa then --todomaa find why but we need this for bu_list to have click_down and uif ok
		--self:print( "hack Maa" )
		x = x - self.__x_offset_exp	
		y = y - self.__y_offset_exp
	end
	--self:print( self.__x_offset_exp, self.__y_offset_exp )
	--local dplanes_index = self.__dplanes_index
	--local bu_front

	local dplanes = self.__dplanes
	-- For all dplanes from the the front to the back reverse from draw (table is sort)
	for i = #dplanes, 1, -1 do
		-- Get the table of the BU in this dplane
		local tbu = dplanes[i].__bu
		-- For all BU in the table in reverse
		for j = #tbu, 1, -1 do
			local bu = tbu[j]
			if bu then
				--self:print( "bu is "..bu )
				if bu:is_active() then
					if bu:is_visible() then
						-- if aaa.keyboard.is_alt_only() then
						-- 	--]]
						-- 	---[[ maa ui_top check if commenting is a good idea ?
						-- 	--hack a problem here when descending is this (get_bus_down()) enough ?
						-- 	if bu:is_ui_top() or ((not aaa.keyboard.is_alt()) and bu:get_bus_down()) then
						-- 	else
						-- 		bu = nil
						-- 	end
						-- else
						if bu:is_ui_active() then
							--self:print( "test "..bu )
							if bu:is_inside( x,y ) then
								--self:print( "bu inside is "..bu )
								if self.verbose >= 1 then
									self:print( "BUS:find_bu_active_by_pos( "..x.." , "..y.." ) found " .. bu )
								end
								-- if aaa.keyboard.is_alt_only() and (not bu:is_ui_top() and not bu:get_bus_down()) then
								--  	bu = nil
								-- end
								-- self:print( " return " .. bu )
								return bu
							end -- if bu:is_inside
						end -- if bu
					else -- bu:is_visible()
						--todo visible test seems to work but not print when else ?!?!?!?
						--if self.verbose >= 1 then
						--	self:print_debug("BUS:find_bu_active_by_pos( "..x.." , "..y.." ) skipping unvisible " .. bu)
						--end
					end	-- bu:is_visible()
				end	-- bu:is_active()
			else -- if bu
				self:print_error( "find_bu_active_by_pos() have a nil bu here" )
			end -- if bu
		end -- for j
	end -- for i

	return nil
end

function BUS:find_bu_by_pos( x,y )
	--aaa.print_method()
	self:push()
	local bu = self:find_bu_active_by_pos( x,y )
	if bu then
		--aaa.print_fn()
		local nx,ny = bu:convert_xy_bus_up_to_local( x,y )
		bu.__crosshair_u = nx
		bu.__crosshair_v = ny
		if self.verbose >= 2 then
			self:print( bu .. " find_bu_by_pos " .. x .. " " .. y )
		end

		local bu_down
		local bus_down = bu:get_bus_down()
		if bus_down and bu.__b_bus_down_active_ui and (not bu:is_mini()) then
			nx, ny = bus_down:do_transfo( nx,ny )
			bu_down = bus_down:find_bu_by_pos( nx,ny )
		end
		if bu_down then
			bu = bu_down
		end
		if self.verbose >= 1 then
			self:print( "BUS:find_bu_by_pos() found " .. bu )
		end
	else
		if self.verbose >= 2 then
			self:print( "BUS:find_bu_by_pos() do not correspond to a BU" )
		end
	end
	self:pop()

	return bu
end

function BUS:deal_with_touch( blob )
	if self.b_mouse_only and blob.id ~= -1 then
		return nil
	end
	if self.verbose >= 1 then
		self:print( "now process " .. blob .. " x " .. blob.x .. ", y " .. blob.y )
	end

	self:push()

		local bu = self:find_bu_active_by_pos( blob.x, blob.y )
		if bu then
			if self.verbose >= 2 then
				self:print( bu .. " touched by " .. blob )
			end

			local bu_down
			local bus_down = bu:get_bus_down()
			if bus_down and bu.__b_bus_down_active_ui and (not bu:is_mini()) then
				local b = BLOB:create( blob )
				b.x, b.y = bu:convert_xy_bus_up_to_local( b.x,b.y )
				b.x, b.y = bus_down:do_transfo( b.x,b.y )
				bu_down = bus_down:deal_with_touch( b )
				if b.__b_consumed then
					blob.__b_consumed = true --propagate up consumed
				end
			end
			if bu_down then
				bu = bu_down
			else
				if aaa.keyboard.is_alt_only() then
					if not bu:is_ui_top() then
						bu = nil
					end
					--ow that we found a bu at the lowest level we go up until we found a BU which should react with Alt
					-- repeat
					-- 	if bu:is_ui_top() then
					-- 		break
					-- 	end
					-- 	local bus = bu:get_bus_up()
					-- 	if not bus then
					-- 		break
					-- 	end
					-- 	bu = bu:get_bu_up()
					-- until bu==nil
				end
				if bu then
					bu:add_contact(blob)
				end
			end
		else
			if self.verbose >= 2 then
				self:print("touch do not correspond to a BU")
			end
		end

	self:pop()

	return bu
end

function BUS:__update_bus_private()
	if self.verbose >= 3 then	self:print("__update_bus_private()")	end

	if BUS.__b_clean_contact then
		self:clean_contact()
	end
	-- this is here when we move from page to page
	--	Horizontal
	if self.__x_offset_exp ~= self.__x_offset_target then
		if self.__b_page_no_inertia then
			self.__x_offset_exp = self.__x_offset_target
		else
			--self:print( "update() self.__x_offset_target" )
			local delta = self.__x_offset_target - self.__x_offset_exp
			if delta ~= 0. then
				self.__x_offset_dt = math.max(self.__x_offset_dt - aaa.time.dt, 0)
				local d = self.__x_offset_dt / ga.__page_change_time
				d = math.pow((1 - d), .5)
				delta = delta * d
				self.__x_offset_exp = self.__x_offset_exp + delta
			end
		end
	end

	--	Vertical
	if self.__y_offset_target then	
		local delta = self.__y_offset_target - self.__y_offset_exp
		if delta ~= 0. then
			--self:print( "update() self.__y_offset_target "..self.__y_offset_exp.." "..self.__y_offset_target )
			delta = delta * .2
			self.__y_offset_exp = self.__y_offset_exp + delta
		end
	end

	--self:print( "update() "..self:get_bu_nb() )
	--self:print( table.show( self, "BUS" ) )
	self:apply_method_down_active( "update" )


	if BUS.__b_need_top_image_by_page_or_domino then
		--todo sercel out for bug

		--done here can previous code can reorder
		--no more ordering should happen from here
		--	find the top domino, will be use to stop the other videos
		--	find the image, we use it for the second screen
		self.top_image_by_page = {}

		--local bu_front
		local dplanes = self.__dplanes
		-- For all dplanes from the the smallest to the highest (table is sort)
		for i = 1, #dplanes do
			-- Get the table of the BU in this dplane
			local tbu = dplanes[i].__bu
			-- For all BU in the table
			for j = 1, #tbu do
				local bu = tbu[j]
				if bu then
					if bu:is_active() then
						if bu.domino ~= nil then
							self.domino[bu.__page].top = bu
						end
						if bu:get_texture_bind_2d() and bu:get_y() > 2.2 then
							self.top_image_by_page[bu.__page] = bu
						end
					end
				else
					self:print_error("private_update() have a nil bu here")
				end
			end
		end

		--TODO UTRAM a regler ailleurs
		local bu = self.top_image_by_page[self.__page]
		if bu then
			if bu:get_texture_bind_2d() and aaa.lua.global.get("second_screen_set") then
				--self:print( "up "..self:get_y() )
				second_screen_set(bu:get_texture_bind_2d())
			end
		end
	end
end

--todo add a mecanism for first interation to keep it on top
function BUS:update()
--	if not self or not self:is_active() then return end

	if GA.b_spy then aaa.spy.push_range( "update-" .. self, 2 ) end
		self:push()
		if self.verbose >= 3 then
			self:print("update()")
		end

			--self:print( "t "..self.update_before )
			if self.__fn_table then
				self:do_fn("update_before")
				self:__update_bus_private()
				self:do_fn("update_after")
			else
				self:__update_bus_private()
			end

		self:pop()
	if GA.b_spy then aaa.spy.pop_range() end
end

--
--	CONSTRAINT
--
function BUS:constraint_inside( bu, x,y, sx,sy )
	local cons = self.__s_constraint
	if cons and cons ~= "none" then
		if cons == "center" then
			return bu:__constraint_point_inside( x,y )
		elseif cons == "edge" then
			return bu:__constraint_inside_at_edge( x,y, sx,sy )
		else
			self:print("constraint type " .. cons .. " is unknowed")
		end
	end
	return x,y
end
function BUS:set_constraint(s_constraint)
	self.__s_constraint = string.lower(s_constraint)
end

--
--	TRANSFO
--
BUS.doc.set_transfo = "( size_x [size_y, [,center_x [,center_y ]]] ) define the coordonate system of the BUS\n"..
					"x and y passed to the bus from a containing BU will be transformed as x * size + center\n"..
					"if size_y is nil, size_x will be used     center default is 0\n"..
					"e.g. set_transfo(8) will define a -4,4 coordonate system\n"..
					"note that the default coordonate system of a BU (not a BUS) is -.5,.5 on both axis"
function BUS:set_transfo( sx,sy, cx,cy )
	sy = sy or sx
	self.__trs = {}
	self.__trs.sx = sx
	self.__trs.sy = sy
	self.__trs.over_sx = 1. / sx
	self.__trs.over_sy = 1. / sy
	self.__trs.cx = cx or 0
	self.__trs.cy = cy or 0
end
function BUS:get_transfo()
	return self.__trs
end
function BUS:get_transfo_sxy()
	local trs = self.__trs
	if trs then
		--self:print( "size "..trs.sx.." "..trs.sy.." center "..trs.cx.." "..trs.cy )
		return trs.sx, trs.sy
	end
	return nil
end
function BUS:do_transfo( x,y )
	local trs = self.__trs
	if trs then
		--self:print_debug( "we got a transfo here" )
		x = x * trs.sx + trs.cx
		y = y * trs.sy + trs.cy
	end
	return x,y
--	return x-self.__x_offset_exp, y-self.__y_offset_exp
end
function BUS:do_transfo_inverse( x,y )
	--x,y = x+self.__x_offset_exp, y+self.__y_offset_exp
	local trs = self.__trs
	if trs then
		--self:print_debug( "we got a transfo inverse here" )
		x = (x - trs.cx) * trs.over_sx
		y = (y - trs.cy) * trs.over_sy
	end
	return x,y
--	return x+self.__x_offset_exp, y+self.__y_offset_exp
end
function BUS:gol_transfo()
	local trs = self.__trs
	if trs then
		if GA.b_spy then aaa.spy.push_range( "gol_transfo()", 5 ) end
			gol.push_scale_translate_2d( trs.over_sx, trs.over_sy, -trs.cx+self.__x_offset_exp, -trs.cy+self.__y_offset_exp)
		if GA.b_spy then aaa.spy.pop_range() end
	else
		gol.push_scale_translate_2d( 1,1, self.__x_offset_exp,self.__y_offset_exp )
	end
end

function BUS:gol_transfo_inverse()
	local trs = self.__trs
	if trs then
		--gol.translate( trs.cx, trs.cy )
		--gol.scale( trs.sx, trs.sy )
		gol.pop_matrix()
	else
		gol.pop_matrix()
	end
end


--
--	VIDEO
--
function BUS:stop_video_all(bu_not_this_one)
	if self.verbose >= 3 then	aaa.print_method()	end
	self:apply_fn_down(
		function(self)
			if self:is_video() and self ~= bu_not_this_one then
				if self:is_active() then
					self:get_video():stop()
				end
			end
		end
	)
end
function BUS:hide_and_stop_video()
	if self.verbose >= 3 then	aaa.print_method()	end
	self:apply_fn_down(
		function(self)
			if self:is_video() then
				if self:is_active() then
					self:get_video():stop()
					self:set_active(false)
				end
			end
		end
	)
end
function BUS:show_hide_video(b)
	self:apply_fn_down(
		function(self)
			if self:is_video() then
				--if self:is_active() then
				self:set_active(b)
			--end
			end
		end
	)
end
function BUS:hide_video()
	if self.verbose >= 3 then	aaa.print_method()	end
	self:show_hide_video(false)
end
function BUS:show_video()
	if self.verbose >= 3 then	aaa.print_method()	end
	self:show_hide_video(true)
end
function BUS:set_video_loop(b)
	self:apply_fn_down(
		function(self)
			if self:is_video() then
				self:set_video_loop(b)
			end
		end
	)
end
function BUS:place_video_at_start()
	self:apply_fn_down(
		function(self)
			if self:is_video() then
				self:place_video_at_start()
			end
		end
	)
end
--
--	DRAW
--

--
--	DRAW
--
function BUS:__draw_gdebug_inactive( b_cam )
	if self.verbose >= 3 then	self:print("__draw_gdebug_inactive()")	end

	if b_cam then	self.__cam:update()
	else			self:gol_transfo()
	end
	--if self.__b_grid_draw	then
	--	self:draw_grid()
	--end

	--self:do_fn( "draw_before" )

	--	Draw from the back to front
	local dplanes = self.__dplanes
	-- For all dplanes from the the smallest to the highest (table is sort)
	for i = 1, #dplanes do
		-- Get the table of the BU in this dplane
		local tbu = dplanes[i].__bu
		-- For all BU in the table
		for j = 1, #tbu do
			local bu = tbu[j]
			if not bu:is_active() then
				bu:transfo_and_draw_gdebug()
			end
		end
	end
	--[[
	for i=1,self:get_bu_nb() do
		local bu = self:get_bu_back_first( i )
		if bu then
			if not bu:is_active() then
				bu:transfo_and_draw_gdebug()
			end
		else
			self:print_error( "__draw_gdebug_inactive() have a nil bu here" )
		end
	end
--]]
	--self:do_fn( "draw_after" )
	if b_cam then
	else
		self:gol_transfo_inverse()
	end
	--param.set( carre, "active", 0 )
	--param.set( key, "active", 0 )
end

function BUS:__draw_gdebug_coor()
	local d = .25

	local bu = self:get_bu_up()
	if not bu then				gol.color_blue()
	elseif bu:is_contact() then	gol.color_green()
	else						gol.color_cyan()
	end

	aaa.draw_null(0, 0, 0, d * 2)
	aaa.draw_null(1, 1, 0, d * 2)
	gol.draw_line_strip_2d(0, 0, 1, 1, 1 - d, 1, 1, 1 - d, 1, 1)
end

function BUS:__draw_bu_all()
	local b_draw_min = false --BU:is_draw_min()
	--if true then return end
	local dplanes = self.__dplanes
	-- For all dplanes from the the smallest to the highest (table is sort)
	for i = 1, #dplanes do
		-- Get the table of the BU in this dplane
		local dplane = dplanes[i]
		-- For all BU in the table
		local tbu = dplane.__bu
		if self.verbose >= 3 then
			table.print( tbu, "tbu "..i, 1 )
		end
		for j = 1,#tbu do
			local bu = tbu[j]
			if bu then
				if bu:is_active() then
					if bu:is_visible() then
						--bu:print( "draw" )
						if b_draw_min then 	--and bu:is_draw_skip_for_speed () then
						else
	--[[	was using page
							local d = self:get_page() - bu.__page
							--bu:print( self:get_page().." "..bu.__page )
							--self:print( ga.b_draw_1page.." "..ga.b_draw_3page )
							if d==0 then
								bu:__transform_and_draw()
							elseif ga.b_draw_1page then
							elseif not ga.b_draw_3page or inside( d, -1, 1 ) then
								--self:print( "toto" )
								bu:__transform_and_draw()
							end
	--]]
							if bu:is_video() then bu:update_video()	end
							bu:__transform_and_draw()
							if BU.__b_gdebug_draw_inactive and bu:is_gdebug() then
								gol.finish()
							end
						end

					else
						if bu:is_gdebug() and bu.__b_gdebug_draw_invisible then
							bu:__transform_and_draw_gdebug()
						end
					end
				else
					if bu:is_gdebug() and bu.__b_gdebug_draw_inactive then
						bu:__transform_and_draw_gdebug()
					end
				end
			else
				self:print_error( "__draw_bu_all() have a nil bu here" )
			end
		end
	end
end
function BUS:draw( b_cam )
	-- if not self or not self:is_active() then
	-- 	return
	-- end

	if GA.b_verbose_render then self:print( "draw() " .. ga.__update_index .. " begin" ) end
	if GA.b_spy then aaa.spy.push_range( self .. ".draw()", 8 ) end
	self:push()
	
		if b_cam 	then	self.__cam:update()
		else				self:gol_transfo()
		end
		
		if self.__b_grid_draw	then	self:draw_grid()			end
		
		--self:print( "will draw_before" )
		local fn_table = self.__fn_table
		if fn_table				then	self:do_fn( "draw_before" )	end
		
		if self.__b_gdebug_coor then	self:__draw_gdebug_coor()	end
		
			self:__draw_bu_all()
		
		if fn_table				then	self:do_fn( "draw_after" )	end
		
		if not b_cam			then	self:gol_transfo_inverse()	end
	
		if ga:is_visualization() then
			--gol.push_attrib()
			gol.set_default()
			--gol.set_line_smooth( true )
			--gol.set_line_width( 1. )
			gol.color_cyan()
			aaa.draw_str_xy( "Page " .. self:get_page(), 0, 0, .2, .2 )
			--gol.pop_attrib()
		end

	self:pop()
	if GA.b_spy then aaa.spy.pop_range() end
	if GA.b_verbose_render then self:print( "draw() " .. ga.__update_index .. " end" ) end
end

--
--	LOAD/SAVE bu pos
--
function BUS:set_fname( fname )
	self.__fname = fname
end
function BUS:get_fname()
	local fname = self.__fname
	if not fname then
		fname = self:get_name()
	end
	return fname
end
function BUS:set_dir(dir)
	--aaa.debug.print_traceback()
	--self:box_debug( "set dir to "..dir )
	self.__dir = dir
end
function BUS:get_dir()				return self.__dir	end

function BUS:set_bu_pos_load(b)		self.__b_bu_pos_load = b		end
function BUS:is_bu_pos_load()		return self.__b_bu_pos_load		end

function BUS:set_bu_pos_save(b)		self.__b_bu_pos_save = b		end
function BUS:is_bu_pos_save()		return self.__b_bu_pos_save		end

function BUS:set_bu_size_save(b)	self.__b_bu_size_save = b		end
function BUS:is_bu_size_save()		return self.__b_bu_size_save	end

function BUS:set_bu_pos_load_save(b)
	self:set_bu_pos_load(b)
	self:set_bu_pos_save(b)
end

function BUS:__build_pages_list()
	local tab = {}
	--self:print( self.__page_min, self.__page_max )
	local page_min = self.__page_min
	local page_max = self.__page_max
	self:apply_fn_down(
		function(self)
			--self:print( i.." "..self.__page.." "..page_min.." "..page_max )
			if inside( self.__page, page_min, page_max ) and (self:is_pos_load_save() or self:is_value_load_save()) then --avoid extreme data
				--self:print( "\tinside " )
				local name = self:get_name_lowercase()
				if name then
					local list = tab[self.__page]
					if not list then
						tab[self.__page] = {}
						list = tab[self.__page]
					end
					self.__name_sort = name
					table.insert( list, self )
				end
			end
		end
	)
	--table.print( tab, "toto", 2, 4 )
	--	for _, t in pairs(tab) do
	--		table.sort( )
	--	end
	return tab
end

--	used for serialization of position
--		page_x start at 0
--		i start at 1
--todo cache the last bu or change strategy to avoid too much search
function BUS:get_bu_by_page_name( page_x, name )
	local bu
	if bus_cur.__tmp_tab then
		local pbu = bus_cur.__tmp_tab[page_x]
		if pbu then
			if type(name) == "string" then
				name = string.lower(name)
				bu = GABU_OBJ.find_in_array_by_name_lowercase( pbu, #pbu, name )
			else
				bu = pbu[name]
			end
		end
	else
		self:print( "__tmp_tab is empty is it normal ?" )
	end
	if not bu then
		if name:sub(1,6) == "proto_" then
			--todo
			--we try probably to load position when MUS is not populated yet
			--self:print_debug( "no bu for name "..name.." this is normal for now (2024 Feb)" )
			--self:print_debug( "  Dir_protos have its odities which will vanish when we get rid of it" )
		else
			--table.print( self.__down, "get_bu_by_page_name()", 2 )
			self:print_error( "get_bu_by_page_name() no bu for name "..name )
			--aaa.debug.print_traceback()
		end
	end
	return bu
end

-- BEGIN load/save fns
-- THIS FUNCTIONS ARE USED FOR SAVING and so LOADING
--
--
-- deal with save_pos_value_more( self, file ) which use "BUS.do_bu_method"
--  not calling tranform_name_out
local  function tranform_name_in( name )
	if name == "uidir" then
		return "ui6"
	else
		return name
	end
end
local  function tranform_name_out( name )
	if name == "ui6" then
		return "uidir"
	else
		return name
	end
end
--this is what saved and then called to set the normal position infact
-- we use other fns for page 0 and use now a rect argument but we keep these for compatiblity with old files (TANKs in particular)
function BUS.set_bu_pos( page_x, name, x,y, sx,sy, angle, value )
	local rect
	if type(page_x)=="string" and type(name)=="table" then --called with args ( name, rect, value )
		page_x, name, rect, value = 0, page_x, name, x
	else
		rect = {x,y, sx,sy, angle}
	end
	name = tranform_name_in( name )
	--	get bu
	local bu = bus_cur:get_bu_by_page_name( page_x, name )
	if bu then
		--	position
		if bu:is_pos_load_save() then
			rect[1] = rect[1] + page_x * bus_cur.__page_sx
			bu:set_pos_orig( rect )
		end
		-- value
		bu:bus_set_value( value )
	else
		--aaa.box_debug( "can't find BU "..name )
	end
end
function BUS.set_bu_pos_value( name, rect, value )
	tranform_name_in( name )
	BUS.set_bu_pos( name, rect, value )
end
function BUS.set_bu_pos_mini( page_x, name, x,y, sx,sy, angle, b_mini )
	local rect
	if type(page_x)=="string" and type(name)=="table" then --called with args ( name, rect, value )
		page_x, name, rect, b_mini = 0, page_x, name, x
	else
		rect = {x,y, sx,sy, angle}
	end
	name = tranform_name_in( name )
	local bu = bus_cur:get_bu_by_page_name( page_x, name )
	if bu and bu:is_pos_load_save() then
		rect[1] = rect[1] + page_x * bus_cur.__page_sx
		bu:set_pos_mini( rect, b_mini )
	end
end
function BUS.set_bu_pos_value_mini( name, rect, b_mini )
	name = tranform_name_in( name )
	BUS.set_bu_pos_mini( name, rect, b_mini )
end

function BUS.set_bu_mini( page_x, name, b )
	name = tranform_name_in( name )
	local bu = bus_cur:get_bu_by_page_name( page_x, name )
	if bu and bu:is_pos_load_save() then
		bu:__set_window_state( b and "mini" or "normal" ) 
	end
end
function BUS.set_bu_value( name, value )
	--aaa.print_fn()
	local bu = bus_cur:get_bu_by_page_name( 0, name )	-- we don't store page anymore (Maa 2021 april)
	if bu then
		-- if bu:is_name("finger") then
		-- 	aaa.print_fn()
		-- 	aaa.box_debug( "vakue is "..value ) 
		-- end
		bu:bus_set_value( value )
	end
end

function BUS.do_bu_method( name, method, ... )
	name = tranform_name_in( name )
	local bu = bus_cur:get_bu_by_page_name( 0, name )	-- we don't store page anymore (Maa 2021 april)
	if bu then
		--table.print( bu, "for "..bu, 2 )
		bu[method]( bu, ... )
	end
end
--
--
-- END load/save fns


function BUS:make_bu_pos_name( dirname )
	--self:box_debug( "BUS:make_bu_pos_name()\n"..dirname )
	--aaa.print_fn()

	local str = self:get_fname()
	if not str then
		self:print( "have no name can't build BU positions name" )
		return nil
	end
	str = str .. ".bus"
	
	local dir = dirname or self:get_dir()
	if not dir then
		--self:box_debug( "BUS:make_bu_pos_name() not dir case" ) 
		--	we want it in the App dir
		if app then
			dir = app:get_dir_absolute()
		end
	end
	--aaa.print_fn()
	if dir then
		str = dir .. str
	end
	--self:box_debug( "BUS:make_bu_pos_name()\n"..dirname.."\n"..str )
	--self:print_inverse( "bu_pos name is "..str )
	return str
end

function BUS:load_bu_pos( dirname )
	-- if self:get_name_lowercase() == "buga" then
	-- 	aaa.debug.print_traceback()
	-- end

	if not self:is_bu_pos_load() then
		return
	end

	local filename = self:make_bu_pos_name(dirname)
	if not filename then
		self:print( "can't load BU positions because no filename" )
		return
	end

	if GA.b_spy then aaa.spy.push_range( "load_bu_pos", 14 ) end
		if self.verbose >= 1 then
			self:print( "will load BU Position from " .. filename )
		end
		--self:print( "will load BU Position from "..filename )
		if aaa.file.is_exist(filename) then
			self.__tmp_tab = self:__build_pages_list()
			self:push()
			aaa.dofile(filename)
			self:pop()
			self.__tmp_tab = nil -- we don't need it anymore
		end
	if GA.b_spy then aaa.spy.pop_range() end
end

--tdo make it faster avoiding str concatenation
-- write the per-BU lines (header included) to an already-open file handle.
-- The caller opens and closes the file.
function BUS:save_bu_pos_to_file( file )
	file:write( "-- BU position value for " .. self,  "\n" )
	--file:write( "-- BUS.set_bu_pos( page, index, s, y, size_x, size_y, angle [,value] )\n" )

	local tab = self:__build_pages_list()

	local function make_name( bu, i )
		local str = bu:get_name_lowercase()
		str = (str == "ui6") and "uidir" or str
		if str then	str = '"' .. str .. '"'
		else		str = "" .. i
		end
		return str
	end

	-- we loop on a table of table, each table correspond to one page
	local b_save_size = self:is_bu_size_save()
	for page_index = 0, #tab do
		local tbu = tab[page_index]
		if tbu then
			local page_x_offset = - page_index * self.__page_sx
			table.sort( tbu, function(a,b) return a.__name_sort < b.__name_sort end )

			--file:write( "-- page " .. page_index .. "\n" )
			for i = 1, #tbu do
				local bu = tbu[i]
				--	SAVE VALUE
				local str_value
				if bu:is_value_load_save() then
					str_value = bu:get_values_as_str() or nil
					if str_value=="" then
						str_value = nil
					end
				end

				if bu:is_pos_load_save() then
					local name = make_name( bu, i )

					--GET POS REGULAR
					local x,y, sx,sy, angle = bu:get_pos_normal()
					x = x + page_x_offset

					--HEADER POS REGULAR + x,y"
					local str
					if page_index == 0 then	str = "BUS.set_bu_pos_value( "            ..name..", {"..x..","..y
					else					str = "BUS.set_bu_pos( "..page_index..", "..name..", "..x..","..y
					end
					--ADD sx,sy
					if b_save_size then	str = str..", "..(sx or 1)..","..(sy or 1)	--avoid nil value to be printed
					else				str = str..", nil,nil"
					end
					--ADD angle
					--we save angles even if we currently don't use it or it will erase the values
					if page_index == 0 then
						if angle ~= 0 then	str = str..", ".. angle.." }"
						else				str = str.. "}"
						end
					else					str = str..", "..(angle or 0)
					end
					--ADD values
					if str_value then
						str = str..", "..str_value
					end
					--SAVE LINE
					--self:print( str )
					file:write( str .. " )\n" )

					--SAVE mini if exist
					x,y, sx,sy, angle = bu:get_pos_mini()
					if x then
						x = x + page_x_offset
						local str
						if page_index == 0 then	str = "BUS.set_bu_pos_value_mini( "..name..", {"
						else					str = "BUS.set_bu_pos_mini( "..page_index..", "..name..", "
						end
						--ADD x,y, sx,sy
						str = str..x..","..y..", "..sx..","..sy
						if page_index == 0 then
							if angle ~= 0 then	str = str..", ".. angle.." }"
							else				str = str.."}"
							end
						else					str = str..", "..angle
						end
						if bu:is_mini() then
							str = str..", "..true
						end
						--self:print( str )
						file:write(str .. " )\n")
					end

				else	-- save only value here
					if str_value then
						local name = make_name( bu, i )
						--self:print( bu.." have name "..name )
						--self:box_debug( "name is \""..name.."\"" )
						file:write( "BUS.set_bu_value( "..name..", "..str_value.." )\n")
					--else
					--	self:box_error(bu .. "\nhave no get_value\nin BUS " .. self)
					end
				end
				bu:save_pos_value_more( file )
				-- local plugged = bu:get_table( "__plugged_in" )
				-- if plugged then
				-- 	local midi = plugged.midi
				-- 	if midi then
				-- 		local ch,ctl = midi.ch,midi.ctl
				-- 		local str = ch.."-".. ctl
				-- 		self:box_good( bu.." will save midi connection "..str )
				-- 	end
				-- end
			end
		end
	end
end

function BUS:save_bu_pos( dirname )
	if not self:is_bu_pos_save() then
		return
	end

--	self:box_error( "save_bu_pos()" )
--	self:print_inverse( "save_bu_pos()" )
--	aaa.debug.print_traceback()
--	aaa.print_fn()

	local filename = self:make_bu_pos_name(dirname)
	if not filename then
		self:print( "can't save BU position because no filename" )
		return
	end

	if self.verbose >= 1 then
		self:print( "will save BU Position" )
		local str = "save_bu_pos("
		if dirname then	str = str .. " \""..dirname.."\" )"
		else 			str = str .. ")"
		end
		str = str .. " -> "..filename
		self:print_inverse( str )
		--aaa.box_good( self.." save bu_pos "..filename )
	--self:print_inverse( "will save BU Position value" )
	end

	local file = aaa.file.open( filename, "w" )
	if file then
		self:save_bu_pos_to_file( file )
		file:close()
	end

	-- recurse into bus_down: each one writes its own .bus file
	local tab = self:__build_pages_list()
	for page_index = 0, #tab do
		local tbu = tab[page_index]
		if tbu then
			for i = 1, #tbu do
				local bu = tbu[i]
				local bus_down = bu:get_bus_down()
				if bus_down then
					bus_down:save_bu_pos()
				end
			end
		end
	end
end

--
--	UTIL
--
function BUS:trim_page_begin()
	self.__page_min = self.__page_min + 1
end
function BUS:trim_page_end()
	self.__page_max = self.__page_max - 1
end

function BUS:make_bus_loop()
	local bu
	--set icon before all
	self:set_page(self.__page_max + 1)
	bu = self:add_image( "refresh_at_max", {0,0, 4,4}, BUS.__image_name.refresh )
	bu:set_ui_active(false)
	bu:set_border(false)
	bu:disable_mobile()
	self:trim_page_end() -- trim the end
	--set icon after
	self:set_page(self.__page_min - 1)
	bu = self:add_image( "refresh_at_min", {0,0, 4,4}, BUS.__image_name.refresh )
	bu:set_ui_active(false)
	bu:set_border(false)
	bu:disable_mobile()
	self:trim_page_begin() -- trim the beginning
end

function BUS.static_reset_page_current()
	bus_cur:reset_page_current()
end
function BUS.static_set_page(index)
	bus_cur:set_page(index)
end

function BUS:draw_debug_str( text, x,y )
	gol.set_default()
	BU:set_text_line_attr()
	gol.color_yellow()
	aaa.draw_str_xy( text, x,y, .1,.1 )
end

--
--	GRID
--
function BUS:set_grid_draw(b)
	self.__b_grid_draw = b
end
--todo use BUS dimension
function BUS:draw_grid()
	--if true then return end
	gol.set_texture_dim(0)
	if MAAEB and MAAEB.draw then
		MAAEB.draw(3.5, 3.5, 0, .5)
	end

	local sx, sy = self:get_transfo_sxy()
	local al = .05

	gol.color_cyan(al * 2.)
	aaa.draw_lines_hori_nb(sy * 2, .25, .5, 0, sx)
	aaa.draw_lines_vert_nb(sx * 2, .25, .5, 0, sy)
	gol.color_cyan(al * 3.5)
	aaa.draw_lines_hori_nb(sy, .5, 1, 0, sx)
	aaa.draw_lines_vert_nb(sx, .5, 1, 0, sy)
	gol.color_cyan(al * 5.)
	aaa.draw_lines_hori_nb(sy - 1, 1, 1, 0, sx)
	aaa.draw_lines_vert_nb(sx - 1, 1, 1, 0, sy)
end

