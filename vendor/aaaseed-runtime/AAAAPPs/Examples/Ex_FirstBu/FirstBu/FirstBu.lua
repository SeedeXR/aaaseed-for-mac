aaa.lua.global.declare_table( "mfb" )

local bu_vid

function mfb:change_bu( bu )
--	bu:set_rot( false )
--	bu:set_grid_y( .125 )
	bu:set_inertia( true )
	bu:set_ui_top_move( true )
end

local bu_ex

function mfb.draw_background()
	gol.set_texture_dim( 0 )
	gol.set_line_width( 1. )
	gol.color( .5, .5, .5,  1 )

	for i=1,4 do
		aaa.draw_rect_line( (i-3)*2, 0.2, (i-2)*2, 2.2 )
	end
end

function mfb.is_over( bu )
	local blobs = ga:get_blobs()
	for i=1,blobs:get_blob_nb() do
		local blob = blobs.blob[i]
		if bu:is_inside( blob.x, blob.y ) then
			--aaa.print( " touched bu" )
			return true
		end
	end
	return false
end

function mfb.draw_foreground_low()
		--[[louvre gol.set_texture_dim( 0 )
	gol.set_line_width( 1. )
	louvre gol.color( bu_ex:get_value(1), bu_ex:get_value(2), 1,  1 )
	aaa.draw_rect( -.5, -2, .5, -1 )
	--]]
	--aaa.print( "toto" )
	if is_over( bu_vid ) then
		aaa.print( "touch bu 1" )
		bu_vid:get_video():play()
	else
		bu_vid:get_video():stop()
	end
end

function mfb.draw_foreground()
--	draw_foreground_low()
end

function mfb.pages_define_in()
	local	bus = BUS:create( "FirstBu" )
	bus:init_begin()

		local bu = bus:add_slider( "test", {0, 0, 2, .25 } )
		bu:set_meter( false )

	bus:init_end()

	return bus
end

function mfb:test_print( bu, sli )
	aaa.mess.show( "titoto "..bu.." "..sli:get_value() )
end

function mfb:first_bu_pages_define()
	local	ratio_x = 1.77
	local	taille_x = 3 * ratio_x;
	local	taille_y = 3;

	local	noms
	local	bus = BUS:create( "First BU" )
	bus:init_begin_add_to_ga()

	--START

	--bus:inc_page()
	local	bu
	local	param_ref

	bu = bus:add_slider( "Taille", {0, -1, 2, .25 } )
	local bu_slider = bu
	bu:set_meter( true )
	self:change_bu( bu )

	bu = bus:add_selector( "MODULE Dst", {0,.1, 2,1}  )
		bu:set_nb_min_0( 8, 4 )
		bu:set_item_text( 1, "A", "B", "Cam" )
		bu:set_item_text( 8, "d", "c", "toto", "AUdrey" )

	local bu_sel = bu
	bu = bus:add_button( "test 1", {1,1, .25,.25} )
		bu:set_method_on_click( mfb, "test_print", bu, bu_slider )
	bu = bus:add_button( "test 2", {2,2, .25,.25} )
		bu:set_method_on_click( mfb, "test_print", bu, bu_sel )

--[[	OK BUT WE MAKE IT SIMPLER HERE
	bu = bus:add_selector( "MODULE Dst", {0,.1, 8,.2} )
	bu:set_text_draw( true )
	bu:set_nb_min_0( 5 )
	bu:set_item_text( 1, "UI 1", "UI 2", "UI 3", "UI 4", "NO" )
	bu:disable_mobile()


	bu = bus:add_slider( "", {1, 0, 2, .25 } )

	local obj = aaa.obj.get_from_top_by_class( "bdd_clear_screen" )
	param_ref = param.get_ref( obj, "color_grey" )
	bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )

	bu = bus:add_dial( nil, 0, 1, .5, .5 )
	param_ref = param.get_ref( "exlua_color", "global_green" )
	bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )

	bu = bus:add_slider( "Christian", {1, 0, 2, .25 } )
	param_ref = param.get_ref( "APP/APP_ExampleLua/ExLua/fx_a.colt", "global_alpha" )
	bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )

	bu = bus:add_slider( "Maa", {1, 2, 2, .25, 0.125 } )
	--param_ref = param.get_ref( "APP/APP_ExampleLua/ExLua/fx_a.colt", "global_alpha" )
	--bu:set_target_param( param_ref )
	bu:set_min_max( 0, 1 )
	change_bu( bu )
	bu:set_grid_xy( nil, nil )

	bu = bus:add_selector( nil, {1,-2, 2,1.} )
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

	bu = bus:add_selector( nil, {1,-3, 2,1.} )
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
	bu = bus_cur:add_slider_xy( "Example", {	-2, -1, 2, 2. } )
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
--	local ref = aaa.obj.get_no_error( "Sercel_test_curve" )

	bu = bus:add_window( "IN", self:pages_define_in(),	{0,2, 2.5,2} )
	bu:get_bus_down():set_transfo( 8 )

	--VIDEOS:set_dir_media( "Ex" )
	bu_vid = bus:add_video( "1.avi", {0,1, 1,1*9/16} )
	--aaa.box_error( "titi "..bu_vid )

--[[	louvre
	bu = bus:add_text(  "Alt key is very useful here", {0,-1, 1,.25} )
	change_bu( bu )
	bu:set_ui_top_size( true )

	bus:inc_page()

	bu = bus:add_slider( "Toto", {0, 1, 2, .5, 1/4 } )
	change_bu( bu )

	bu = bus:add_dial( "titi", 0, 1, 1, 1 )

	bus:make_bus_loop()

	--the scrolling bu is on the bottom
	bus:add_scroll( -4, -2 )
--]]
	bus:init_end()

	bus:set_function( "draw_before", mfb.draw_background )
	bus:set_function( "draw_after", mfb.draw_foreground )
end

if IS_BUSS_OPEN() then
	mfb:first_bu_pages_define()
end
