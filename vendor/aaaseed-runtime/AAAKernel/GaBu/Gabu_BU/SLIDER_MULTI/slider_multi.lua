if CLASS.DECLARE( "SLIDER_MULTI", BUI, {
	__uv_transfo = { fu=1, fv=1, ou=0, ov=0 },
	__elt_size = .06
	} ) then
		SLIDER_MULTI:set_class_status_doc(	CLASS.STATUS.GABU,
									"a BUI for numeric multiple values"  )
end



-- function SLIDER_MULTI:change_bu_value( vals )
-- 	local x,y = vals[1],vals[2]
-- 	bu:set_xy( x,y )
-- end

--todo redo all this with sizes of elt too
--todo see with GA:add_to_undo_history
-- function SLIDER_MULTI:add_to_undo_history( bu, prev_xy, next_xy )
-- 	self:print( "add_to_undo_history()" )

-- 	--table.print( prev_xy, "prev_xy", 2 )
-- 	--table.print( next_xy, "next_xy", 2 )

-- 	if not prev_xy then
-- 		return
-- 	end

-- 	local prev_x,prev_y = prev_xy[1],prev_xy[2]
-- 	local next_x,next_y = next_xy[1],next_xy[2]
-- --	print(prev_x, prev_y)
-- --	print(next_x, next_y)
-- 	if prev_x ~= next_x and prev_y ~= next_y then
-- 		local elt
-- 		if false then
-- 			local t = bu:__make_change_table()
-- 		else
-- 			elt = { undo = ga:make_table_to_call_method( bu, "change_bu_value", prev_xy ),
-- 					redo = ga:make_table_to_call_method( bu, "change_bu_value", next_xy )
-- 					}
-- 		end
-- 		self:__add_elt_to_undo_history( elt )
-- 	end
-- end

-- function SLIDER_MULTI:add_to_undo_history( bu )
-- 	if not bu then
-- 		self:print_debug( "SLIDER_MULTI:add_to_undo_history() no bu" )
-- 		return 
-- 	end

-- 	local verbose = ga:get_undo_redo():get_verbose()
-- 	--aaa.print_fn()
-- 	--aaa.debug.print_traceback()
-- 	local storage = bu.__undo_redo_storage
-- 	if verbose > 1 then
-- 		table.print( storage, "undo_redo_storage", 2 )
-- 	end

-- 	local prev = storage.pos_prev
-- 	local x,y = bu:get_xy()
-- 	local sx,sy = bu:get_sxy()
-- 	local undo = self:make_table_to_call_method( bu, "set_xy_sxy", prev[1],prev[2],	prev[3],prev[4]	)
-- 	local redo = self:make_table_to_call_method( bu, "set_xy_sxy", x,y,				sx,sy			)
-- 	ga:add_undo_redo( undo,redo )
-- 	bu.__undo_redo_storage = nil
-- end


--[[
--todo we should generalize this notion of last touched
local function __intercept_do_click_down( bu, x,y )
	local ur = ga:get_undo_redo()
	local verbose = ur:get_verbose()
	if verbose > 0 then aaa.print_fn() end

	local bu_multi = bu:get_bu_up()
	bu_multi:set_bu_last( bu )

	bu_multi:set_bu_cur()
	--oo.getclass(bu).draw( bu, x,y )
	ur:store_position_in_bu(bu)
end


local function __intercept_do_click_up( bu, x,y )
	local verbose = ga:get_undo_redo():get_verbose()
	if verbose > 0 then aaa.print_fn() end

	ga:get_undo_redo():add_pos_to_undo_history( bu )
	--bu.__prev_xy = nil
end
--]]

local function __intercept_do_click_down( bu, x,y )
	oo.getsuper(SLIDER_MULTI).do_click_down( bu, x,y )
	local bu_multi = bu:get_bu_up()
	bu_multi:set_bu_last( bu )
end

function SLIDER_MULTI:create_elt( id, bus, rect, b_state )	
	--aaa.print_fn()
	if rect and rect[3] and rect[4]==nil then
		self:box_debug( "sy is nil ......" )
	end

	bus:push()
		local bu = BUTTON:create( tostring(id), rect )
		if b_state==nil then
			b_state = false
		end
		bu:set_value( b_state )

		local x,y, sx,sy = unpack(rect)
		if not sx then
			local s = self.__elt_size
			sx,sy = self:get_sxy()
			sx,sy = s/sx, s/sy
		end
	
		bu:set_mobile( true )
		bu:set_inertia( false )
		bu:set_ui_top_size( true )
		--bu:set_ui_top( true )

		bu.do_click_down = __intercept_do_click_down
	--	bu.do_click_up 	 = __intercept_do_click_up

		bus:add_bu( bu )
		bu:__set_rect_inertia( {x,y, sx,sy} )

		bu:set_text_inside(false) --to force draw of a checkmark
		--todo refine encapsulate
		bu.__b_inside_main_rect_only = true
	bus:pop()

	return bu
end


function SLIDER_MULTI:__init_elt_nb( name, nb, b_state )
	nb = nb or 8
	local bus_down = BUS:create( "Multi_".. name )
	bus_down:set_transfo( 1,1, .5,.5 )	-- here we set the coor system
--	local bus_down = self:get_bus_down()

	bus_down:init_begin()
		local elts = {}
		local x,y = nil,.5
		for i = 1,nb do
			x = (i-.5)/nb
			local elt = self:create_elt( i, bus_down, {x,y}, b_state )
			--	there is an implicit size of 2 by 2 unit
			table.insert( elts, elt )
		end
	bus_down:init_end()

	self.__elt_nb = nb
	self.__elts = elts

	self:transform_in_window( bus_down )
	--hack this 4 lines to make it "fix" so ALt + key will not move it
	self:set_mobile( false )
	self:set_ui_top_size( false )
	self:set_ui_top( false )
	self.__positions.b_window = false

	--self:set_text_xy( -.25, .46 )
	self:set_text_factor( .1 )
end

function SLIDER_MULTI:create( name, rect, nb )
	local self = SLIDER_MULTI:create_instance( name, rect )
	self:__init_elt_nb( name, nb )
	return self
end

function SLIDER_MULTI:get_elt_nb()			return self.__elt_nb						end

function SLIDER_MULTI:get_elt( i )			return self.__elts[i]						end
function SLIDER_MULTI:get_elt_table()		return self.__elts							end
function SLIDER_MULTI:is_elt_active( i )	return self:get_elt(i):get_value_as_bool()	end

function SLIDER_MULTI:set_bu_last( bu )		self.__bu_last = bu		end
function SLIDER_MULTI:get_bu_last()			return self.__bu_last	end

function SLIDER_MULTI:set_elt_nb( nb )	--no undo_redo here
	local more = nb - self:get_elt_nb()
	if more ~= 0 then
		self:add_elt( more )
	end
end
function SLIDER_MULTI:set_elt_nb_ui( nb )
	local more = nb - self:get_elt_nb()
	if more ~= 0 then
		self:add_elt_ui( more )
	end
end

function SLIDER_MULTI:add_elt( add_nb )	--no undo_redo here
	--todo deal with name/number
	local nb = self.__elt_nb
	local id_first
	if add_nb < 0 then	-- remove bu
		while add_nb < 0 do
			if nb <= 1 then	break end

			local bu = self:get_bu_last()
			local id
			if bu then
				--self:print( "bu_last" )
				id = table.find_key_by_val( self.__elts, bu )
			else
				id = nb
			end
			if not id_first then
				id_first = id
				self:print( "id_first is "..id_first )
			end
			self:get_bus_down():remove_bu( self.__elts[id] )
			table.remove( self.__elts, id )
			self:set_bu_last( self.__elts[id-1] )
			nb = nb - 1
			add_nb = add_nb + 1
		end
	else	-- add_bu
		while add_nb > 0 do
			local bu = self:get_bu_last() or self.__elts[nb]
			local x,y, sx,sy = 0,0
			if bu then
				--self:print( "we have bu" )
				x,y = bu:get_xy()
				sx,sy = bu:get_sxy()
			end
			local elt = self:create_elt( nb+1, self:get_bus_down(), {(x+(sx or 0)),y, sx,sy}, false )
			self:__sync_element( elt )
			nb = nb + 1
			self.__elts[nb] = elt
			self:__update_elt_text_xy_f_ratio( elt )
			self:set_bu_last( elt )
			add_nb = add_nb - 1
		end

	end
	self.__elt_nb = nb
	if id_first then 
		for id=id_first,self:get_elt_nb() do
			local elt = self:get_elt( id )
			local new_name = tostring(id)
			--self:print( "is is "..id.." will change name from "..elt:get_name().." to "..new_name  )
			elt:change_name( new_name )
			--todo name should be enough
			elt:set_text( new_name )
			--self:print( "name is now "..elt:get_name() )
		end
	end
end
function SLIDER_MULTI:add_elt_ui( add_nb )
	--todo deal with name/number
	local nb_prev = self.__elt_nb
	self:add_elt( add_nb )
	local nb_next = self.__elt_nb

	--todo deal with the number but not the values
	if nb_next ~= nb_prev then
		local undo = self:make_table_to_call_method( self, "set_elt_nb", nb_prev	)
		local redo = self:make_table_to_call_method( self, "set_elt_nb", nb_next	)
		ga:add_undo_redo( undo,redo )
	end
end

function SLIDER_MULTI:set_elt_center_xy( i, x,y )
	local bu = self:get_elt( i )
	if bu then bu:set_xy( x,y ) end
end
function SLIDER_MULTI:sort_elt( fn )
	table.sort( self.__elts, fn )
end
--hack see below
function SLIDER_MULTI:get_elt_xy( i )
	local bu = self:get_elt( i )
	return bu:get_xy()
end
function SLIDER_MULTI:sort_elt_by_x()
	self:sort_elt( function(a,b) return a:get_x() < b:get_x() end )
end

--[[
function SLIDER_MULTI:get_elt_xy( i )
	local bu = self:get_elt( i )
	local x,y	= bu:get_xy()
	local sx,sy = bu:get_sxy()
	--here we rely on the canonical sizes of 1
	x = (x-sx*.5) / (1.-sx)
	y = (y-sy*.5) / (1.-sy)
	return	clamp_01(x), clamp_01(y)
end
function SLIDER_MULTI:set_elt_xy( i, x, y )
	local bu = self:get_elt( i )
	local sx,sy = bu:get_sxy()
	--here we rely on the canonical sizes of 1
--	x = x * (1.-sx)
--	y = y * (1.-sy)
	bu:set_xy( x, y )
end
--]]

function SLIDER_MULTI:get_elt_sxy( i )
	local bu = self:get_elt( i )
	return bu:get_sxy()
end
function SLIDER_MULTI:set_elt_sxy( sx,sy, i )
	if i then
		--aaa.debug.print_traceback()
		local elt = self:get_elt( i )
		--self:print( sx.." "..sy )
		elt:set_sxy( sx,sy )
	else
		self:apply_to_elt( "set_sxy", sx,sy )
	end
end

function SLIDER_MULTI:place_elts( elt )
	local nb = self:get_elt_nb()
	local sq = math.floor( math.sqrt( nb - 1 ) ) + 1
	local id = 0
	local S = .3
	local SH = .5 - S*.5
	for j=1,sq do
		for i=1,sq do
			id = id + 1
			if id <= nb then
				self:set_elt_center_xy( id, (i-1)/(sq-1)*S+SH, (sq-j)/(sq-1)*S+SH )
			end
		end
	end
end

function SLIDER_MULTI:define_more( name, sx, sy )
	local bus = BUS:create( name.."_more" )
	bus:init_begin()
		bus:set_constraint( "edge" )
		bus:set_bu_pos_load_save( false )

		local bu
		local delta = sy*.05
		local SY = sy /  14.
		local SX = sx - 2. * delta
		local le = -sx*.5 + delta
		local ri = sx*.5 - delta
		local to = sy*.5 - delta
		local dy = .5

		local x = le + sx*.5
		local y = to
		bu = bus:add_trig( "dec", {x,y, sx,sy} )
			bu:set_text( "-" )
			bu:set_method_on_click( self, "add_elt_ui", -1 )
		y = y + sy
		bu = bus:add_trig( "inc", {x,y, sx,sy} )
			bu:set_text( "+" )
			bu:set_method_on_click( self, "add_elt_ui", 1 )
	bus:init_end()

	return bus
end

function SLIDER_MULTI:set_uv_transfo( fu,fv, ou,ov )
	self.__uv_transfo = { fu=fu or 1, fv=fv or 1, ou=ou or 0, ov=ov or 0 }
end
function SLIDER_MULTI:get_uv_transfo()
	local t = self.__uv_transfo
	return t.fu,t.fv, t.ou,t.ov
end

function SLIDER_MULTI:get_values()
	local tab = {}
	for i=1,self:get_elt_nb() do
		local u,v = self:get_elt_xy( i )
		local su,sv = self:get_elt_sxy( i )
		local elt = self:get_elt( i )
		--elt:print( elt:__get_balue().__value.." "..elt:get_value().." "..elt:is_value_type_bool() )
		tab[i] = { u,v, elt:get_value(), su,sv }
		local att = elt.__att
		if att then
			tab[i][3] = att
		end
	end
	return tab
end
function SLIDER_MULTI:get_values_transformed()
	local tab = self:get_values()
	local fu,fv, ou,ov = self:get_uv_transfo()
	V3.add_mul_table( tab, tab, -ou,-ov,  1/fu,1/fv )
	return tab
end
function SLIDER_MULTI:set_values( tab )
	local fu,fv, ou,ov = self:get_uv_transfo()
	--self:print( "change elt_nb from "..self:get_elt_nb().." to "..#tab )
	--table.print( tab, "set_values", 3 )
	self:add_elt( #tab - self:get_elt_nb() )
	for i=1,#tab do
		local pt = tab[i]
--		table.print( pt, "elt "..i, 2 )
		local elt = self:get_elt( i )
		elt:set_xy( pt[1], pt[2] )
		local att = pt[3]
		if att then
			if type(att)=="table" then
				elt.__att = att
			else
				elt:set_value( att )
			end
		end
		if pt[4] and pt[5] then
			elt:set_sxy( pt[4], pt[5] )
		end
	end
end
function SLIDER_MULTI:set_values_transformed( tab )
	local fu,fv, ou,ov = self:get_uv_transfo()
	for i=1,#tab do
		local pt = tab[i]
		local u, v = pt[1], pt[2]
		self:set_elt_center_xy( i, u*fu+ou, v*fv+ov )
	end
end
function SLIDER_MULTI:cpy_point_to_bdd( bdd, ou,ov, fu,fv )
	ou = ou or 0
	ov = ov or 0
	fu = fu or 1
	fv = fv or 1
	aaa.bdd.set_lua_cur( 	bdd )
	for i=1,self:get_elt_nb() do
		local u,v = self:get_elt_xy( i )
		aaa.bdd.set_point( nil, i, (u+ou)*fu, (v+ov)*fv )
	end
end

function SLIDER_MULTI:__update_elt_text_xy_f_ratio( elt )
	local t = self.__text_xy_f_ratio
	if t then
		elt:set_text_xyf( t[1],t[2],t[3] )
		local ratio = t[4]
		if ratio then
			elt:set_text_rect_ratio(ratio)
		end
	end
end
function SLIDER_MULTI:set_elt_text_xy_f_ratio( x,y, f, ratio )
	self.__text_xy_f_ratio = {x,y, f, ratio}
	for i=1,self:get_elt_nb() do
		self:__update_elt_text_xy_f_ratio( self:get_elt(i) )
	end
end

function SLIDER_MULTI:__sync_element( elt )
	local b_elt_draw_check_mul = self.__b_elt_draw_check_mul
	if b_elt_draw_check_mul then
		elt:set_method_on_click_double( elt, "flip_value", 1 ) --we have to pass id or flip_value will receive click.x or click.y
	end
	elt:set_check_mul( b_elt_draw_check_mul )
end
function SLIDER_MULTI:set_select_on_click_double()
	self.__b_elt_draw_check_mul = true
	for i=1,self:get_elt_nb() do
		self:__sync_element( self:get_elt( i ) )
	end
end

--todo use table apply
function SLIDER_MULTI:apply_to_elt( method, ... )
	for _,elt in IPAIRS(self.__elts) do
		elt[method]( elt, ... )
	end
end
function SLIDER_MULTI:flip_elts_active_all()	self:apply_to_elt( "flip_value" )		end
function SLIDER_MULTI:set_elts_active_all()	self:apply_to_elt( "set_value", true )	end
function SLIDER_MULTI:clear_elts_active_all()	self:apply_to_elt( "set_value", false )	end

function SLIDER_MULTI:do_key( key )
	self:print_do_key( "SLIDER_MULTI", key )

	local b_key_used
	--local id = self:get_id_interacting()
	if key==127 then -- delete
		self:add_elt_ui( -1 )
		b_key_used = true
	end
	return b_key_used or oo.getsuper(SLIDER_MULTI).do_key( self, key )
end

function SLIDER_MULTI:do_key_special( key )
	self:print_do_key_special( "SLIDER_MULTI", key )
	
	local b_key_used
	--local id = self:get_id_interacting()
	if	key==276 then -- insert
		self:add_elt_ui( 1 )
		b_key_used = true
	end
	return b_key_used or oo.getsuper(SLIDER_MULTI).do_key_special( self, key )
end


-- function SLIDER_MULTI:update()
-- 	oo.getsuper(SLIDER_MULTI).update(self)
-- 	for i=1,self.__elt_nb do
-- 		local bu = self.__elts[i]
-- 		local x, y = bu:get_xy()
-- 		bu.transaction_x:set_value_ui( x, 1 )
-- 		bu.transaction_y:set_value_ui( y, 1 )
-- 	end
-- end

function SLIDER_MULTI:get_uif_zones()
 	local zones = {}
 	BU.add_uif_zones_base( self, zones )
 	return zones
end
