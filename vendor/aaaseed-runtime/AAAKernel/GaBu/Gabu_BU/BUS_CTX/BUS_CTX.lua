if CLASS.DECLARE( "BUS_CTX" ) then
	--todo recomment
	BUS_CTX:set_class_status_doc(	CLASS.STATUS.GABU,
								"When App are defined, under the GA control (see GA:add_app()),",
								"several BUSS are open one after another (regular/top/top_top)",
								"In fact BUSS regroup several Bus and maintain an order of prioritity for their update and/or draw",
								"bus are added to BUSS using the ga (e.g. ga:add_bis(bus) )" )
	function BUS_CTX:set_cur()	BUS_CTX.__BUS_CTX_cur = self	end
	function BUS_CTX:get_cur()	return BUS_CTX.__BUS_CTX_cur	end
end

function BUS_CTX:create( name )
	local self = BUS_CTX:create_instance( name )

	self:set_cur()	--todo move it to GABU_OBJ

	self.__regular	= BUSS:create( name.."_regular" )
	self.__top 		= BUSS:create( name.."_top"     )
	self.__top_top	= BUSS:create( name.."_top_top" )	-- seems to be unused

	self.__from_top    = { self.__top_top, 	self.__top, self.__regular }
	self.__from_bottom = { self.__regular,  self.__top, self.__top_top }

	return self
end

function BUS_CTX:get_regular()		return self.__regular 	end
function BUS_CTX:get_top()			return self.__top 		end
--function BUS_CTX:get_top_top()	return self.__top_top 	end

function BUS_CTX:is_open_regular()	return self.__regular	and self.__regular:is_open()	end
function BUS_CTX:is_open_top()		return self.__top 		and self.__top    :is_open()	end
function BUS_CTX:is_open_top_top()	return self.__top_top	and self.__top_top:is_open()	end

--todo app concept de cur should be enough
function BUS_CTX:__open_buss( buss )
	local cur = self.__buss_cur
	if cur then
		cur:close()
	end

	self.__buss_cur = buss
	if buss then
		buss:open()
	end
end

function BUS_CTX:open_regular()		self:set_cur()	return self:__open_buss(self.__regular)	end
function BUS_CTX:open_top()			self:set_cur()	return self:__open_buss(self.__top)		end
function BUS_CTX:open_top_top()		self:set_cur()	return self:__open_buss(self.__top_top)	end

--function BUS_CTX:get_buss_cur()	return self.__buss_cur 		end

function BUS_CTX:close()			return self:__open_buss(nil)	end

--todoapp
--2024 July unused
--[[
function GA:add_app( app_class, name, ... )
	self:__open_buss( self.__buss )
	APP.CREATE_INST( app_class, name, ... )

	self:__open_buss( self.__buss_top )
	app:do_buss_top_open()
	ga:add_ga_and_bu_windows()

	self:__open_buss( self.__buss_top_top )
	app:do_buss_top_top_open()

	self:__open_buss( nil )
end
--]]

function BUS_CTX:find_bu_by_name_lowercase( bus_name, bu_name )
	local bu
	for _,buss in ipairs( self.__from_bottom ) do
		bu = buss:find_bu_by_name_lowercase( bus_name, bu_name )
		if bu then
			--self:print( "find_bu_by_name_lowercase return "..bu )
			return bu
		end
	end
end

function BUS_CTX:save_bu_pos()
	self:apply_method_from_bottom( "save_bu_pos" )
end

function BUS_CTX:find_bu_by_pos( x,y )
	--aaa.print_fn()
	local bu
	for _,buss in ipairs( self.__from_top ) do
		bu = buss:find_bu_by_pos( x,y )
		if bu then return bu end
	end
end
function BUS_CTX:find_bu_active_by_pos( x,y )
	--aaa.print_fn()
	local bu
	for _,buss in ipairs( self.__from_top ) do
		bu = buss:find_bu_active_by_pos( x,y )
		if bu then return bu end
	end
end

function BUS_CTX:__add_bus_low( bus, buss, fn_name )
	if bus:get_up() then
		self:print_debug( "Adding to buss a bus with have already an up" )
		self:print_debug( "Skipping but Mâa will take care of this message")
		return
	end
	if buss then
		if fn_name then buss:print_inverse( fn_name.."( "..bus.." )" ) end
		buss:add_down( bus )
	end
end
--tdo redi this verbose stuff
function BUS_CTX:add_bus( bus )		self:__add_bus_low( bus, self.__cur or self.__regular,	self.verbose >= 1 and "BUS_CTX:add_bus" or nil )		end
function BUS_CTX:add_bus_top( bus )	self:__add_bus_low( bus, self.__top,				    self.verbose >= 1 and "BUS_CTX:add_bus_top" or nil )	end

function BUS_CTX:remove_bus( bus )
--	local buss = self.__buss_cur or self.__buss
--	if buss then
		if self.verbose >= 1 then self:print_inverse( "BUS_CTX:remove_bus( "..bus.." )" ) end
		for _,buss in ipairs( self.__from_bottom ) do
			if buss:remove_down( bus, false ) then
				return true
			end
		end
		self:print_error( "BUS_CTX:remove_bus() Can't remove_down "..bus )
		return false
		--self:box_debug( "Debug pause ")
--	end
end

function BUS_CTX:apply_method_from_bottom( method_name, ... )
	for _,buss in ipairs( self.__from_bottom ) do
		buss:apply_method_down( method_name, ... )
	end
end
function BUS_CTX:apply_method_from_bottom_active( method_name, ... )
	for _,buss in ipairs( self.__from_bottom ) do
		buss:apply_method_down_active( method_name, ... )
	end
end


function BUS_CTX:deal_with_untouch( blobs )
	for _,buss in ipairs( self.__from_top ) do
		if buss:deal_with_untouch( blobs ) then
			break
		end
	end
end

function BUS_CTX:deal_with_touch( blobs )
	local touch_nb = blobs:get_touch_nb()
	local touch_used = 0
	for _,buss in ipairs( self.__from_top ) do
		touch_used = touch_used + buss:deal_with_touch( blobs )
	end
	return touch_used == touch_nb
end

-- function BUS_CTX:attach_app( app )
-- 	self.__app_to_attach = app
-- end

function BUS_CTX:update()
	self:set_cur()
	self:apply_method_from_bottom_active( "update" )
	-- if self.__app then
	-- 	self.__app:push_update_pop()
	-- end
end

function BUS_CTX:draw()
	self:set_cur()
	
	--app is (only one in june 2021)) attached to only buss_regular
	--self:print( "field __app is "..self.__app )
	-- if self.__app then
	-- 	self.__app:push_draw_pop()
	-- end

	self:apply_method_from_bottom_active( "draw" )

	-- if self.__app_to_attach and (not self.__regular:is_open()) then
	-- 	self.__app = self.__app_to_attach
	-- 	self.__app_to_attach = nil
	-- end


end