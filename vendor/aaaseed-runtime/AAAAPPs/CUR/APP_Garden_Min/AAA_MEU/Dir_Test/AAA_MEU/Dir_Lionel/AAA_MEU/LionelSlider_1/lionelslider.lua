function meu:define_ui()
	local ref = self.ref
	local bu

	self.__bu_alpha_test = {}
	local t_test = self.__bu_alpha_test

	self.media_dir = app.media_dir_rel.."Slider/"
	local bind_back = IMGS.get_bind( self.media_dir.."/slider_back.png" )
	local bind_main = IMGS.get_bind( self.media_dir.."/slider_main.png" )
	local bind_meter = IMGS.get_bind( self.media_dir.."/slider_meter.png" )
	local bind_cursor = IMGS.get_bind( self.media_dir.."/slider_cursor.png" )
	local bind_led_on = IMGS.get_bind( self.media_dir.."/led_on.png" )
	local bind_led_off = IMGS.get_bind( self.media_dir.."/led_off.png" )

	--Simple slider tex with midi
	bu = self:add_slider(	{1,8,	4,1},	"Speed" , self, "speed", 1, -8, 8 )
	 	-- tex slider
		bu:set_meter( false )
	 	bu:set_blink( true )
	 	bu:set_blink_period( 1 )
	 	-- led slider
	 	bu:set_midi( 1, 0 )
	 	bu:set_tex_led( bind_led_on, bind_led_off )

		table.insert( t_test, bu )

	--Simple slider tex with no interaction
	bu = self:add_slider(	{1,9,	4,1},	"Speed_b" , self, "speed_b", 1, -8, 8 )
		bu:set_meter( true )
		bu:set_color_back( { 1, .5, .5, .5 } )
		--bu:set_tex_back( bind_back )
		SLIDER:set_tex_main( bind_main )
		SLIDER:set_tex_meter( bind_meter )
		bu:set_meter_color( { .8, .8, .8, 1 } )
	 	SLIDER:set_tex_cursor( bind_cursor )
	 	bu:disable_uif()

		table.insert( t_test, bu )

	--XY slider
	bu = self:add_slider_two(	{9.5,4,	7,1},	"XY" , self, "XY", 1, -2, 2, .5, 5)
		table.insert( t_test, bu )

	--Double slider
	--bu = self:add_slider_two(	{9.5,2.5,	7,1}, "Double", self, "speed",		1, -4, 4, -8, 8 )
	bu = self:add_slider_two(	{9.5,6,	7,1}, "Double" )	--, self, "speedC",	1, -4, 4, -8, 8 )
		table.insert( t_test, bu )

	--Dial slider
	bu = self:add_slider(	{9.5,10,	4,4}, "Dial" )
		bu:set_dial( true )
		--bu:set_angle( .25 )
		table.insert( t_test, bu )

	bu = self:add_slider(	{9.5,8,	7,1}, "Simple" )
		table.insert( t_test, bu )

	bu = self:add_slider(	{1,1,	8,1}, "Alpha" )
		bu:set_method_on_value_change( self, "update_test_alpha", bu )
end

function meu:update_test_alpha( bu )
	local val = bu:get_value()

	table.apply_method( self.__bu_alpha_test, "set_alpha", val )
end