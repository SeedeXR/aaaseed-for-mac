-- 18K		192.168.1.100
function meu:define_meu_infos( )
	return	{ author = "Mâa",
				help = "Control using a network link a Digital Projection projector,\n"..
						"used by Mâa in Laval.",
				tags = { "device", "input", "output", "utility" }	-- add Hardware
			}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local ix = 1
	local iy = 1
	local SY = 1
	local DY = .5
	local SYM = SY * .8

	bu = self:add_text_info(	{ix,iy,		8,1},		"Received"	)
		ui.bu_received = bu

	iy = iy + 1
	bu = self:add_trig_method(	{ix,iy,		4,SY},		"Power Off",	self, "set_power",	false	):set_text( "Off" )	:set_color_back( "off" )
		--todo revive bu:set_confirmation( true )
		--bu:set_method_on_click( self, "quit" )
	bu = self:add_trig_method(	{ix+4,iy,	4,SY},		"Power On",		self, "set_power",	true	):set_text( "On" )	:set_color_back( "on" )
	iy = iy + SY
	bu = self:add_trig_method(	{ix+4,iy,	4,SY},		"Shutter Off",	self, "set_shutter",false	):set_text( "Off" )	:set_color_back( "on" )
	bu = self:add_trig_method(	{ix,iy,		4,SY},		"Shutter On",	self, "set_shutter",true	):set_text( "On" )	:set_color_back( "off" )
	iy = iy + SY + DY

	bu = self:add_selector(		{ix,iy,		8,SY*2},	"test.pattern" ):set_nb_min_0( 6, 2 )
		bu:set_method_on_value_change( self, "set_test_pattern_from_bu", bu )
		bu:set_item_text( 1, "Off", "White", "Black", "Red", "Green", "Blue",  "Checkerboard", "Crosshatch", "V Burst", "H Burst", "Color Bar", "Plunge" )
	iy = iy + SY*2 + DY

	bu = self:add_selector(		{ix,iy,		8,1},		"input" )		:set_nb_min_0( 6 )
		bu:set_method_on_value_change( self, "set_input_from_bu", bu )
		bu:set_item_text( 1, "HDMI 1", "HDMI 2", "DP 1", "DP 2", "HDBaseT", "3G-SDI" )
	iy = iy + SY + DY

	bu = self:add_selector(		{ix,iy,		8,1},		"pic.mode" )	:set_nb_min_0( 3 )
		bu:set_method_on_value_change( self, "set_pic_mode_from_bu", bu )
		bu:set_item_text( 1, "High Bright", "Presentation", "Video" )
	iy = iy + SY + DY

	self:add_text_info(	{ix,iy,	4,SY},	"Dynamic Black" )
	bu = self:add_trig_method(	{ix+4,iy,	2,SY},		"dynamic black Off",	self, "set_dynamic_black",	false	):set_text( "Off" )
	bu = self:add_trig_method(	{ix+6,iy,	2,SY},		"dynamic black On",		self, "set_dynamic_black",	true	):set_text( "On" )
	iy = iy + SY + DY

	bu = self:add_selector(	{ix,iy,	8,1},	"color.mode" )		:set_nb_min_0( 4 )
		bu:set_method_on_value_change( self, "set_from_bu", "color.mode", bu )
		bu:set_item_text( 1, "ColorMax", "Manual", "Temperature", "Gain/Lifts" )
	iy = iy + SY
	bu = self:add_selector(	{ix,iy,	8,1},	"color.max" )		:set_nb_min_0( 4 ):set_text_visible(false)
		bu:set_method_on_value_change( self, "set_from_bu", "color.max", bu )
		bu:set_item_text( 1, "HDTV", "Peak", "User1", "User2" )
	iy = iy + SY
	bu = self:add_selector(	{ix,iy,	8,1},	"color.temp" )		:set_nb_min_0( 6 ):set_text_visible(false)
		bu:set_method_on_value_change( self, "set_from_bu", "color.temp", bu )
		bu:set_item_text( 1, "3200K", "5400K", "6500K", "7500K", "9300K", "Native" )
	iy = iy + SY
	iy = iy + SY + DY

	iy = 1
	ix = 9
	local SX = 1.5
	self:add_trig(		{ix,iy+SX,	 		SX,SX-.01},	"lens.left" )	:set_method_on_click( self, "do_lens_left" ):set_text( "<-" )
	self:add_trig(		{ix+SX,iy,	 		SX,SX-.01},	"lens.up" )		:set_method_on_click( self, "do_lens_up" ):set_text( "up" )
	self:add_text_info(	{ix+SX,iy+SX,		SX,SX-.01},	"Lens" )
	self:add_trig(		{ix+SX,iy+SX*2,	 	SX,SX-.01},	"lens.down" )	:set_method_on_click( self, "do_lens_down" ):set_text( "down" )
	self:add_trig(		{ix+SX*2,iy+SX,	 	SX,SX-.01},	"lens.right" )	:set_method_on_click( self, "do_lens_right" ):set_text( "->" )

	ix = ix+5
	self:add_trig(		{ix,iy,	 			SX,SX-.01},	"zoom.out" )	:set_method_on_click( self, "do_zoom_out" ):set_text( "+" )
	self:add_text_info(	{ix,iy+SX,			SX,SX-.01},	"Zoom" )
	self:add_trig(		{ix,iy+SX*2,	 	SX,SX-.01},	"zoom.in" )		:set_method_on_click( self, "do_zoom_in" ):set_text( "-" )

	ix = ix+1.5
	self:add_trig(		{ix,iy,	 			SX,SX-.01},	"focus.far" )	:set_method_on_click( self, "do_focus_far" ):set_text( "+" )
	self:add_text_info(	{ix,iy+SX,			SX,SX-.01},	"Focus" )
	self:add_trig(		{ix,iy+SX*2,	 	SX,SX-.01},	"focus.near" )	:set_method_on_click( self, "do_focus_near" ):set_text( "-" )

	ix=9
	iy = iy	+ 5
	self:add_trig(		{ix,iy,	 			2.25,1},	"lens.unlock" )	:set_method_on_click( self, "do_lens_lock", false ):set_text( "Unlock" ):set_color_back( "off" )
	self:add_trig(		{ix+2.25,iy,	 	2.25,1},	"lens.lock" )	:set_method_on_click( self, "do_lens_lock", true ):set_text( "Lock" ):set_color_back( "on" )
	self:add_trig(		{ix+5,iy,	 		3,1}, 		"lens.center" )	:set_method_on_click( self, "do_lens_center" )
	iy = iy + SY + DY

	bu = self:add_selector(	{ix,iy,		8,1},		"laser.mode" )	:set_nb_min_0( 3 ) --:set_text_visible(false)
		bu:set_method_on_value_change( self, "set_from_bu", "laser.mode", bu )
		bu:set_item_text( 1, "Eco", "normal", "Custom" )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		8,1},		"Laser.Power", nil, nil, 20, 20, 100 ):set_value_type_integer(true)
		bu:set_method_on_value_change( self, "set_from_bu", "laser.power", bu )
	iy = iy + SY + DY

	bu = self:add_selector(	{ix,iy,		8,1},		"altitude" )	:set_nb( 3 ) --:set_text_visible(false)
		bu:set_method_on_value_change( self, "set_from_bu", "altitude", bu )
		bu:set_item_text( 1, "On", "Auto", "Quiet" )
		iy = iy + SY

	self:add_trig(			{ix,iy,	 	8,1},		"test" )		:set_method_on_click( self, "do_test" )
end

function meu:do_test( str )
	self:send_dp_str( "*laser.power.info ?" )
end

function meu:init()
	local ref = self.ref
	local bdd = self:get_layer_bdd(1)
	ref.send_string	= param.get_ref( bdd, "send_string" )
	param.set_save( ref.send_string, false )
	ref.send_trig	= param.get_ref( bdd, "send_trig" )
	ref.received	= param.get_ref( bdd, "received" )
end

function meu:send_dp_str( str )
	self:print( "sending to DP : "..str )
	local ref = self.ref
	param.set( ref.send_string, str )
	param.set( ref.send_trig, true )
	MEU.draw( self )
end
function meu:send_dp( what )
	aaa.print_fn()
	--if value == "off" then value = 0
	--elseif
	self:send_dp_str( "*"..what )
end
function meu:send_dp_value( what, value )
	aaa.debug.print_traceback()
--	aaa.print_fn()
	--if value == "off" then value = 0
	--elseif
	self:send_dp_str( "*"..what.." = "..value )
end
function meu:set_from_bu( what, bu )
	if self:is_render() then
		self:send_dp_value( what, bu:get_value() )
	end
end

function meu:set_power( b )						self:send_dp_value( "power",		b and 1 or 0 )	end
function meu:set_shutter( b )					self:send_dp_value( "pic.mute",		b and 1 or 0 )	end
function meu:set_test_pattern(	value	)		self:send_dp_value( "test.pattern",	value )			end
function meu:set_input(			value	)		self:send_dp_value( "input",		value )			end
function meu:do_lens_lock(		b		)		self:send_dp_value( "lens.lock",	b and 1 or 0 )	end
function meu:set_pic_mode(		value	)		self:send_dp_value( "pic.mode",		value )			end
function meu:set_dynamic_black( b		)		self:send_dp_value( "db.on",		b and 1 or 0 )	end

function meu:set_test_pattern_from_bu( bu )
	if self:is_render() then
		self:set_test_pattern( bu:get_value() )
	end
end
function meu:set_input_from_bu( bu )
	if self:is_render() then
		self:set_input( bu:get_value() )
	end
end
function meu:set_pic_mode_from_bu( bu )
	if self:is_render() then
		self:set_pic_mode( bu:get_value() )
	end
end

function meu:do_focus_near()					self:send_dp( "focus.near" )		end
function meu:do_focus_far()						self:send_dp( "focus.far" )			end
function meu:do_zoom_in()						self:send_dp( "zoom.in" )			end
function meu:do_zoom_out()						self:send_dp( "zoom.out" )			end
function meu:do_lens_up()						self:send_dp( "lens.up" )			end
function meu:do_lens_down()						self:send_dp( "lens.down" )			end
function meu:do_lens_left()						self:send_dp( "lens.left" )			end
function meu:do_lens_right()					self:send_dp( "lens.right" )		end
function meu:do_lens_center()					self:send_dp( "lens.center" )		end



function meu:update_ui()
	local ref = self.ref
	local ui = self.ui
	ui.bu_received:set_text( param.get( ref.received ) )
end

function meu:update()
	self:set_ui_slot_at_start( 2 )
end

function meu:draw()
	MEU.draw( self )
end