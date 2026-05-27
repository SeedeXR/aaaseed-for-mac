local bu_louvre_1
local bu_louvre_2

local function exlua_draw_ontop( bu )
	bu:draw()
	param.set( "BU_ex_bdd_text", "text",  "iiilll "..bu.." ".."Jojo la frite qui pue la graisse de veille cuisson aleatoire".." Jojo la frite qui pue la graisse de veille cuisson aleatoire   " )
	aaa.layers.draw_layer_all( "BU_top_example" )
end

local function draw_dev_curve_edit( bu )
	--bu:draw()
	gol.scale( .125, .25 )
	aaa.layers.draw_layer_all( "dev_curve_edit" )
	gol.scale( 8, 4 )
end

local function exlua_pages_define_2()
	local ratio_x	= 1.77
	local taille_x	= 3 * ratio_x;
	local taille_y	= 3;

	local noms

	local bus = BUS:create( "ExLua" )
	bus:init_begin_add_to_ga()

	--START

	--bus:inc_page()
--[[
	BUI:set_meter( false )
--]]
	local	bu
	local	param_ref

	bu = bus:add_slider( "Taille", {	0, 0, 2, .5, 1/8 } )
	bu:set_midi( 1,0 )
	bu:set_meter( true )

	bu = bus:add_slider( nil, 1, 0, 2, .5, 1/8 )
	bu:set_midi( 1,1 )
	local obj = aaa.obj.get_from_top_by_class( "bdd_clear_screen" )
	param_ref = param.get_ref( obj, "color_grey" )
	bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )

	bu = bus:add_dial( nil, 0, 1, 1, 1 )
	bu:set_midi( 1, 0 )
	param_ref = param.get_ref( "exlua_color", "global_green" )
	bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )

	local i_start = bus:get_bu_nb()+1
	local dir_name = app.media_dir_rel.."Kjay"
	bus:add_dir( dir_name, true, true, false, true )
	for i = i_start , bus:get_bu_nb()-4 do	-- - 4 because there is 4 boutons here
		local bu = bus:get_bu(i)
		--louvre bu.draw_custom = exlua_draw_ontop
		--no more draw_custom
	end

	bus:inc_page()

	bu = bus:add_slider( "Toto", {0, 1, 2, .5, 1/4 } )
	bu:set_midi( 1, 0 )

	bus:make_bus_loop()

	--the scrolling bu is on the bottom
	bus:add_scroll( -4, -2 )

	bus:init_end()
end

local function change_bu( bu )
--	bu:set_rot( false )
--	bu:set_grid_y( .125 )
	bu:set_inertia( false )
	bu:set_ui_top_move( true )
end

local bu_ex

local function draw_background()
	gol.set_texture_dim( 0 )
	gol.set_line_width( 1. )
	gol.color( .5, .5, .5,  1 )

	for i=1,4 do
		aaa.draw_rect_line( (i-3)*2, 0.2, (i-2)*2, 2.2 )
	end
end
local function is_over( bu )
	local blobs = ga:get_blobs()
	for i=1,blobs:get_blob_nb() do
		local blob = blobs.blob[i]
		if bu:is_inside( blob.x, blob.y ) then
			--aaa.print( " touched buu" )
			return true
		end
	end
	return false
end
local function draw_foreground_low()
		--[[louvre gol.set_texture_dim( 0 )
	gol.set_line_width( 1. )
	louvre gol.color( bu_ex:get_value(1), bu_ex:get_value(2), 1,  1 )
	aaa.draw_rect( -.5, -2, .5, -1 )
	--]]
	--aaa.print( "toto" )
	if bu_louvre_1 then
		if is_over( bu_louvre_1 ) then
			aaa.print( "touch bu 1" )
			bu_louvre_1:get_video():play()
		else
			bu_louvre_1:get_video():stop()
		end
	end
	if bu_louvre_2 then
		if is_over( bu_louvre_2 ) then
			aaa.print( "touch bu 1" )
			bu_louvre_2:get_video():play()
		else
			bu_louvre_2:get_video():stop()
		end
	end
end

local function draw_foreground()
	draw_foreground_low()
end

local function exlua_pages_define_in()
	local	bus = BUS:create( "ExLua_in" )
	bus:init_begin()

	local bu
	local ref = aaa.obj.get( "Sercel_test_curve" )
	if ref then
		bu = bus:add_bu( BU:create( "test", {0,-1, 2,.25} ) )
			change_bu( bu )
			bu:attach_obj( ref, 0,0, 16,2, "Sercel_test_curve_bdd" )
			bu:set_ui_top_size( true )
		--bu.draw_custom = draw_dev_curve_edit
		--no more draw_custom
	end
	bus:init_end()

	return bus
end

local function exlua_pages_define()
	local	ratio_x = 1.77
	local	taille_x = 3 * ratio_x;
	local	taille_y = 3;

	local	noms
	local	bus = BUS:create( "ExLua" )
	bus:init_begin_add_to_ga()

	--START

	--bus:inc_page()
--[[
	BUI:set_meter( false )
--]]
	local	bu
	local	param_ref

--[[	OK BUT WE MAKE IT SIMPLER HERE
	bu = bus:add_selector(  "MODULE Dst",	{0,.1, 8,.2} )
	bu:set_text_draw( true )
	bu:set_nb_min_0( 5 )
	bu:set_item_text( 1, "UI 1", "UI 2", "UI 3", "UI 4", "NO" )
	bu:disable_mobile()

	bu = bus:add_slider( "Taille", {0,0, 2,.25 } )
	bu:set_midi( 1, 0 )
	bu:set_meter( true )
	change_bu( bu )

	bu = bus:add_slider( nukm 1,0, 2,.25 )
	bu:set_midi( 1, 1 )
	local obj = aaa.obj.get_from_top_by_class( "bdd_clear_screen" )
	param_ref = param.get_ref( obj, "color_grey" )
	bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )

	bu = bus:add_dial( nil, 0,1, .5,.5 )
	param_ref = param.get_ref( "exlua_color", "global_green" )
	bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )

	bu = bus:add_slider( "Christian", {1,0, 2,.25 } )
	param_ref = param.get_ref( "APP/APP_ExampleLua/ExLua/fx_a.colt", "global_alpha" )
	bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )

	bu = bus:add_slider( "Maa", {1,2, 2,.25, 0.125 } )
	bu:set_midi( 1, 3 )
	--param_ref = param.get_ref( "APP/APP_ExampleLua/ExLua/fx_a.colt", "global_alpha" )
	--bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )
	bu:set_grid_xy( nil, nil )

	bu = bus:add_selector( nil,	{1,-2, 2,1} )
	--param_ref = param.get_ref( "APP/APP_ExampleLua/ExLua/fx_a.colt", "global_alpha" )
	--bu:set_target_param( param_ref )
	bu:set_text( "Selector" )
	bu:set_text_draw( true )
	bu:set_nb_min_0( 7, 3 )
	bu:set_item_text_from_nb( )
	bu:set_item_text( 10, "X", "XI", "XII", "XIII", "XIV" )
	bu:set_item_text( 20, "Fuck" )
	--bu:set_min_max( 0, 1 )
	change_bu( bu )
	--bu:set_grid_xy( nil, nil )

	bu = bus:add_selector( nil,	{1,-3, 2,1} )
	--param_ref = param.get_ref( "APP/APP_ExampleLua/ExLua/fx_a.colt", "global_alpha" )
	--bu:set_target_param( param_ref )
	bu:set_text( "Src" )
	bu:set_text_draw( true )
	bu:set_nb_min_0( 3 )
	bu:set_item_text( 1, "A", "B", "Cam" )
	--bu:set_min_max( 0, 1 )
	change_bu( bu )
	--bu:set_grid_xy( nil, nil )
--]]

--[[	louvre
	bu = bus_cur:add_slider_xy( "Example", {	-2,-1, 2,2. } )
	bu:set_text_draw( true )
	change_bu( bu )
	bu_ex = bu
--]]

--[[	marche mais complique pour debutant
	local ref = aaa.obj.get_no_error( "eo_tube3_curve_a" )
	if ref then
		bu = bus:add_bu( BU:create( "test_a", {0,-1, 2,.25} ) )
		change_bu( bu )
		bu:attach_obj( ref, -4,0, 8,2, "eo_tube3_curve_bdd_a" )
		bu:set_ui_top_size( true )
		--bu.draw_custom = draw_dev_curve_edit
		--no more draw_custom
	end

	local ref = aaa.obj.get_no_error( "eo_tube3_curve_b" )
	if ref then
		bu = bus:add_bu( BU:create( "testb", {0,-1, 2,.25} ) )
		change_bu( bu )
		bu:attach_obj( ref, -4,0, 8,2, "eo_tube3_curve_bdd_b" )
		bu:set_ui_top_size( true )
		--bu.draw_custom = draw_dev_curve_edit
		--no more draw_custom
	end
--]]
	if true then
	--	local ref = aaa.obj.get_no_error( "Sercel_test_curve" )
		local ref
		ref = aaa.obj.get( "Sercel_test_curve" )
		if ref then
			bu = bus:add_bu( BU:create( "test", {0,-1, 2,.25} ) )
			change_bu( bu )
			bu:attach_obj( ref, 0,0, 16,2, "Sercel_test_curve_bdd" )
			bu:set_ui_top_size( true )
			--bu.draw_custom = draw_dev_curve_edit
			--no more draw_custom
		end
	end

	bu = bus:add_window( "IN", exlua_pages_define_in(),	{0,2, 2.5,2} )
	bu:get_bus_down():set_transfo( 8 )

	VIDEOS:set_dir_media( "Arte" )

	bu_louvre_1 = bus:add_video( "EM/som/1.avi", {0,1, 1,1*9/16} )
	bu_louvre_2 = bus:add_video( "EM/SAT.avi",  {-1,1, 1,1*9/16} )
--[[	louvre
	bu = bus:add_text(  "Alt key is very useful here", {0,-1, 1,.25} )
	change_bu( bu )
	bu:set_ui_top_size( true )

	bus:inc_page()

	bu = bus:add_slider( "Toto", {0,1, 2,.5, 1/4 } )
	change_bu( bu )

	bu = bus:add_dial( "titi", 0,1, 1,1, )

	bus:make_bus_loop()

	--the scrolling bu is on the bottom
	bus:add_scroll( -4, -2 )
--]]
	bus:init_end()

	bus:set_function( "draw_before", draw_foreground )
end

if IS_BUSS_OPEN() then
	aaa.activate( { "Sercel_Test" } )
	exlua_pages_define()
end
