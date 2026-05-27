
local function draw_dev_curve_edit( bu )
	--bu:draw()
	gol.scale( .125, .25 )
	aaa.layers.draw_layer_all( "dev_curve_edit" )
	gol.scale( 8, 4 )
end

local function change_bu( bu )
	bu:set_rot( false )
	bu:set_grid_xy( nil, .25 )
	bu:set_inertia( false )
	bu:set_ui_top_move( true )
end

local function sliders_pages_define()
	local ratio_x = 1.77
	local taille_x = 3 * ratio_x;
	local taille_y = 3;

	local noms

	local bus = BUS:create( "sliders" )
	bus:init_begin_add_to_ga()

	--START
	BUI:set_meter( true )
	local	XB = 2.
	local	SX = 1.5
	local	Y = -2.5
	local	SY = 3
	local bu = SLIDER.create_midi_vert(	XB,SX,	Y,SY,		8, 1, 0 )


--restore
--	bu = DIAL.create_midi(			XB, SX,	 Y+SY+.2, SY+.5,		8, 1,	2, 1 )
	bu = SLIDER.create_midi_hori(		XB, SX,	 Y+SY+.2, SY*.5,	8,	2, 0 )
	BUI:set_meter( false )

	--bus:inc_page()
--[[
	BUI:set_meter( false )
--]]
	local bu
	local param_ref

	bu = bus:add_slider( "Taille", {0, 0, 2, .25 } )
	bu:set_midi( 1, 0 )
	bu:set_meter( true )
	change_bu( bu )

	bu = bus:add_slider( "Christian", {0, -1, 2, .25 } )
	bu:set_midi( 1, 2 )
	--todo this is bad it infer where the APP dir is
	local dir = aaa.dir.get_dir_start()
	param_ref = param.get_ref( dir.."/AAAAPPs/Examples/Ex_Sliders/ExLua/fx_a.colt", "global_alpha" )
	bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )

	bu = bus:add_slider( "Maa", {1, 2, 2, .25, 0.125 } )
	bu:set_midi( 1, 3 )
	--param_ref = param.get_ref( "AAAAPPs/ExampleLua/ExLua/fx_a.colt", "global_alpha" )
	--bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )
	bu:set_grid_xy( nil, nil )


--[[
	bus:inc_page()

	bu = bus:add_slider( "Toto", {0, 1, 2, .5, 1/4 } )
	bu:set_midi( 1, 0 )
	change_bu( bu )
--]]
	bus:make_bus_loop()

	--the scrolling bu is on the bottom
	bus:add_scroll( -4, -2 )

	bus:init_end()
end
if IS_BUSS_OPEN() then
	sliders_pages_define()
end

