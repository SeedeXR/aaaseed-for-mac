CLASS.DECLARE( "SPACE" )

function SPACE:is_wspace()	return false end
function SPACE:create( name )
	local self = SPACE:create_instance( name )
	return self
end

function SPACE:define_ui()			end
function SPACE:update()				end
function SPACE:init_buildings()		end
function SPACE:update_buildings()	end
function SPACE:draw()				end

function SPACE:enter()
	self:print_inverse( "enter()                            enter()" )
	if self.__bus_creator_fn then
		self:print_inverse( "create bud with bus_creator_fn" )
		local bus = self.__bus_creator_fn()
		if bus then
			self:set_bus( bus )
			self.__bus_creator_fn = nil
		end
	end
	if self.ui and self.ui.bus then
		self.ui.bus:set_active( true )
		if app and app.skel then
			app.skel:set_multitouch( true )
		end
	end
end
function SPACE:quit()
	self:print_inverse( "quit()                            quit()" )
	if self.ui and self.ui.bus then
		self.ui.bus:set_active( false )
		if app and app.skel then
			app.skel:set_multitouch( false )
		end
	end
end
function SPACE:add_bus_creator( fn )
	if fn then
		self.__bus_creator_fn = fn
	else
		self:print( "can't add_bus_creator() with a nil fm" )
	end
end
function SPACE:set_bus( bus )
	if bus then
		if not self.ui then self.ui = {} end
		self.ui.bus = bus
		bus:set_active( false )
	else
		self:print( "can't set_bus() with a nil bus" )
	end
end

function SPACE:get_cam_layerss()	return nil 	end

CLASS.DECLARE( "SPACE_BEAR", SPACE )

function SPACE_BEAR:create( name )
	local self = SPACE_BEAR:create_instance( name )
	self.ui = {}
	self:define_ui()
	return self
end

function SPACE_BEAR:update()
	-- si besoin du temps pour limiter : aaa.time.t en second
	if self.ui.bu_minus:get_value_as_bool() then
		self:print( "- plafonnier pushed" )
		app:send_osc("ceiling/less", 1)
	end
	if self.ui.bu_plus:get_value_as_bool() then
		self:print( "- plafonnier pushed" )
		app:send_osc("ceiling/more", 1)
	end
	local v = self.ui.bu_hans:get_value()
	v = math.floor( v * 100 ) * .01
	if v ~= self.last_hans then
		self:print( "hans "..v )
		self.last_hans = v
		app:send_osc("hans", v * 0.99)
	end
end

function SPACE_BEAR:define_ui()
	local bus = BUS:create( "Bear" )

	self.ui.bus = bus
	bus:set_active( true )
	bus:init_begin_add_to_ga()

		--	x, y, size_x, size_y, angle(en tours)
		bu = bus:add_slider( "Hans", {	-3, 0, 2, 1, 0.25 } )
		self.ui.bu_hans = bu

		function change_bu( bu )
			bu:set_text_factor( .1 )
		end

		local S = 1.5
		bu = bus:add_trig( "- plafonnier",	{1,1, S,S} )
		self.ui.bu_minus = bu
		change_bu( bu )

		bu = bus:add_trig( "+ plafonnier",	{2.8,1, S,S} )
		self.ui.bu_plus = bu
		change_bu( bu )

	bus:init_end()

	return bus
end

