
if not aaa.lua.global.get( "ROMANITE" ) then return end

local L_APP = ROMANITE

--todo make an OSC object
function L_APP:init_osc()
	self.osc = {}
	local osc = self.osc
	osc.count = 0
	osc.b_verbose = true
	osc.b_send = true
	osc.b_received = true
	osc.b_bundle = false
	osc.dst = 2
end

--todo there is duplication of OSC code: follow string:make_from_list to have a first shot at it
--todo factorize
function L_APP:add_osc_ui()
	local osc = self.osc
	if not osc then return end

	osc.bu = {}

end

function L_APP:send_osc( what, ... )
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

	--aaa.debug.print_traceback()
	--self:box_debug( "Send "..tag )
	if osc.b_send then
		if osc.b_bundle then
			aaa.net.osc_send( osc.dst, tag, ... )
		else
			aaa.net.osc_send_no_bundle( osc.dst, tag, ... )
		end
	end
	--aaa.sleep( osc_delay )
end
function L_APP:send_aaaseed_info( text, ... )
	self:send_osc( "AAASeed/info/"..text,  app.osc.count+1, ... )
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

--]]

function L_APP:receive_osc()
	if not self.osc.b_received then return end

	repeat
		local tab = aaa.net.osc_take_by_start( "/romanite" )
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
			elseif tags[2] == "example" then
			else

			end
			i = i + 1
		end
	until false

end	