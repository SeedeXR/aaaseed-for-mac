function meu:define_meu_infos( )
	return { author = "Mâa", date="2024",
			tags = { "Core", "Tutorial", "Unfinished" },
			help = "example to define a window attach to a MEU"
			}
end

function meu:get_preset_nb()	return 0	end


function meu:define_ui_window_1( col_nb )
	local ix,iy = 1,1
	local SX = 8
	local SY = 1
	local DY = SY * .2

	local bu

	bu = self:add_selector( {ix,iy, 	SX,SY},		"Channel"	)
		bu:set_nb( 10 )
		bu:set_item_text_from_nb()
	iy = iy + SY

	-- we need have of prev + half of current
	bu = self:add_selector( {ix,iy, 	SX,SY*2},	"Slice"		)
		bu:set_nb_min_0( 8,2 )
		bu:set_value(0)
		bu:set_item_text_from_nb()
	iy = iy + SY*2 + DY

	bu = self:add_selector( {ix,iy, 	SX,SY},		"Control"	)
		bu:set_nb(8)
		bu:set_item_text_from_nb() 
	iy = iy + SY + DY

	iy = 15
	bu = self:add_button(  {ix,iy,	 	4,SY},		"Test1"		)
	bu = self:add_button(  {ix+4,iy,	4,SY},		"Test2"		)

	ix,iy = 9,1
	self:add_transfo( 		{ix,iy,		8,2.4} )
	iy = iy + 2.4 + DY
	bu = self:add_slider(	{ix,iy,		8,SY},		"Value", self, "value",	1,	0,8	)
		:add_values_def( .5,1,2,3,4,6 )
	iy = iy + SY + DY

	for i = 1,col_nb-2 do
		bu = self:add_slider(	{9+i*8,1,	8,SY},	"Value_A"..i, self, "value"..i,	1,	0,8	)
		bu = self:add_slider(	{9+i*8,9,	8,SY},	"Value_B"..i, self, "value"..i,	1,	0,8	)
		bu = self:add_slider(	{9+i*8,17,	8,SY},	"Value_C"..i, self, "value"..i,	1,	0,8	)
	end

end

function meu:define_window_1()
	local name = self:get_name()
	local f = 1/4
	local rect = {0,0, 8*f,4.5*f}	
	local bu = self:add_window_ga( 	rect, name, 5, "define_ui_window_1" )
	return bu
end

function meu:define_ui()
	local ref	= self.ref
	local ui 	= self.ui
	local bu
	local par

	self:add_camera()

	--self:add_bu_texture_target_unit()

	--self:add_rendering()
	--self:add_nb_uv( {1, 12.5}, self:get_layer_bdd(1) )

	local ix = 1
	local iy = 1
	local SY,DY = 1,.2

	-- self:set_tab_key( "Base" )
	bu = self:add_slider(	{ix,iy,		8,SY},	"Value", self, "value",	1,	0,8	)
	 	:add_values_def( .5,1,2,3,4,6 )
	iy = iy + SY + DY

	bu = self:define_window_1()
	ui.bu_win_1 = bu

	self:set_tab_key( "More" )
	bu = self:add_button(  {ix+4,iy+4,	 8,SY}, "MEU Window Test", false )
		bu:set_value_load_save( false )	
		bu:set_method_on_value_change( self, "update_window_presence", 1, bu  )
	--ui.bu_win = bu
end

function meu:update_window_presence( id, bu )
	local ui 	= self.ui
	local val = bu:get_value_as_bool()
	aaa.print_method()
	local bus_ctx = ga:get_bus_ctx()
	local buss = bus_ctx:get_regular()
	table.print( buss, buss.."", 1 )
	table.print( buss.__down, "down", 1 )
	local bus = buss:get_bus(-1)
	table.print( bus, bus.."", 1 )
	if val then
		bus:add_bu(ui.bu_win_1)
	else
		bus:remove_bu(ui.bu_win_1)
	end
end

function meu:init()
end

function meu:draw()
	local alpha = self:get_alpha()
	gol.color_white( alpha )
	MEU.draw( self )
end

