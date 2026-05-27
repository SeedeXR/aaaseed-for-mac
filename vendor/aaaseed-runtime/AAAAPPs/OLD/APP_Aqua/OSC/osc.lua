if not aaa.lua.global.get( "AQUA" ) then return end

--todo make an OSC object
function AQUA:init_osc()
	self.osc = {}
	local osc = self.osc
	osc.count = 0
	osc.b_verbose = false
	osc.b_send = true
	osc.b_received = true
	osc.b_bundle = true
	osc.dst = 2
end

function AQUA:send_osc( what, ... )
	local osc = self.osc
	if not osc then return end

	osc.count = osc.count + 1
	local tag = "/"..what
	if osc.b_verbose then
		--self:print( "end_osc() "..eo.osc.count )
		if osc.b_send then
			aaa.print( osc.count.." : osc_send( "..tag..string:make_from_list( ... ).." )" )
		else
			aaa.print( "DONT : osc_send( "..tag..string:make_from_list( ... ).." )" )
		end
	end

	if osc.b_send then
		if osc.b_bundle then
			aaa.net.osc_send( osc.dst, tag, ... )
		else
			aaa.net.osc_send_no_bundle( osc.dst, tag, ... )
		end
	end
	--aaa.sleep( osc_delay )
end


--[[
function AQUA:add_osc_trig_str( text, what, str )
	local bu =  bus_cur:add_teig()
	bu:set_text( text )
	bu:set_method_on_click(		self, "send_osc", what, str )
	return bu
end
function AQUA:click_osc_bool( what, bu )
	self:send_osc( what, bu:get_value_as_bool() )
end
function AQUA:add_osc_bool( text, what )
	local bu =  bus_cur:add_button( text )
	bu:set_method_on_click(		aqua, "click_osc_bool", what, bu )
	return bu
end
function AQUA:click_osc_float( what, bu )
	self:send_osc( what, bu:get_value() )
end
function AQUA:add_osc_slider( text, what )
	local bu =  bus_cur:add_slider( text )
	bu.what = what
	return bu
end

function AQUA:define_ui_osc()
	local osc = self.osc
	if not osc then return end
	osc.bu = {}

	local bus = BUS:begin_window( "Osc" )

	local bu

	--self:add_osc_trig_str( "Play", "music/play/", "titre album" )
	--self:add_osc_trig_str( "Stop", "music/stop/", "none" )
	--self:add_osc_bool( "BEAR", "bear/" )
	bu = self:add_osc_slider( "Hans", "hans" )
	table.insert( osc.bu, bu )
	--bu = self:add_osc_slider( "Plafonier", "ceiling" )
    --bu:set_value( 1 )
	bu = self:add_osc_trig_str( "- plafonnier", "ceiling/less")
	table.insert( osc.bu, bu )
	bu = self:add_osc_trig_str( "+ plafonnier", "ceiling/more")
	table.insert( osc.bu, bu )


	bus:end_window()

	bus:set_method(	"draw_after",	self, "draw_osc" )

	return bus
end
--]]

function AQUA:receive_osc_aqua()
	if not aqua or not aqua.osc.b_received then return end

	--if GA.b_spy then aaa.spy.push_range( "OSC Receive aqua", 1 ) end

	repeat
		local tab = aaa.net.osc_take_by_start( "/aqua" )
		if not tab then break end

		--aaa.print( "received Osc " )

		local i = 1
		while true do
			--aaa.print( i )
			local sub_tab = tab[i]
			if not sub_tab then break end

			--aaa.print( "received Osc "..i )
			local tags = sub_tab.tags
			local args = sub_tab.args

			if tags[2] == "restart" then
				if not self:is_master() then
					self.__ggrea = GGREA:create( self:get_name() )
				end
			elseif tags[2] == "add_grea" then
				if not self:is_master() then
					self.__ggrea:add_grea( args[1], args[2], args[3], args[4] )
				end
			else

			end
			i = i + 1
		end
	until false

	--aaa.spy.pop_range()
end	