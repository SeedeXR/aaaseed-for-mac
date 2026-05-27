
function AQUA:draw_background()
	bus_cur:draw_debug_str( "Back", -4, 1.3 )
	local function conv( f ) return math.floor(f*100.)*.01 end
end
function AQUA:draw_foreground()	bus_cur:draw_debug_str( "Fore", -4, 1.2 )	end

--todo	generalize
function AQUA:add_slider( text )
	local bu = bus_cur:add_slider( text )
	return bu
end

function AQUA:set_camera_id( id )
	aaa.print_method()
	if id <= 8 then
		self.b_cam_display = false
		param.set( self.ref.camera.edit.camera_index, id-1 )
		if id == 5 then
			self.cam_wall_id = 0
		else
			self.cam_wall_id = nil
		end
	else
		self.b_cam_display = true
		param.set( self.ref.camera.display.camera_index, id-9 )
		self.cam_wall_id = id - 8
	end
end
function AQUA:change_camera( bu )
	self:set_camera_id( bu:get_value() + 1 )
end
function AQUA:set_camera_ui( b_crea )
	local camera = self.ref.camera
	if b_crea then
		if self.b_cam_display then
			aaa.obj.become_ui( camera.display.layers )
		else
			aaa.obj.become_ui( camera.edit.layers )
		end
	else
		aaa.obj.become_ui( camera.back.layers )
	end
	aaa.obj.set_focus_ui( camera.display.layers )
end

-- overload bu fn
local function cam_do_click_down( self, x, y )
	--aaa.print_method()
	SELECTOR.do_click_down( self, x, y )
	aqua:change_camera( self )
end

function AQUA:change_back_color( bu )
	local id = bu:get_value() + 1
	self:print( "switch color to "..id )
	local col = self.back_color[id]
	local obj = aaa.obj.get_from_top_by_class( "bdd_clear_screen" )
	--self:print( "switch color to "..col[1].." "..col[2].." "..col[3] )
	param.set( obj, "color_red",	col[1] )
	param.set( obj, "color_green",	col[2] )
	param.set( obj, "color_blue",	col[3] )
end
local function back_color_do_click_down( self, x, y )
	--self:print( "do_click_down( "..x..", "..y.." )" )
	SELECTOR.do_click_down( self, x, y )
	aqua:change_back_color( self )
end

function AQUA:define_ui_first()
	local bus = BUS:begin_window( "Aqua 1" )
	bus:set_bu_pos_load_save( true )

	local bu
--[[
	--todocam was already commented, but a good point of view
	bu = bus:add_button( "Cam edit", {2.1,3.6, .2,.2} )
	bu:set_ui_top_size( true )
	bu:set_target_param( aaa.ref.camera_edit )
--]]

	BU.set_pos_load_save_default( false )

		--bus:move_prev()
		bu	=	self:add_flag(	"__b_active",		"Active",			true		)

		bu = bus:add_selector( "back color" )
			--bu:set_text_draw( false )
			--bu.text_u = .22
			--bu.text_v = -1.15
			bu:set_nb_min_0( 8, 1 )
			bu.do_mouse_move = back_color_do_click_down
			--bu:set_method_on_click(			aqua, "change_back_color",	bu )
			bu:set_value_load_save( true )

		bu	=	self:add_flag(	"b_fbo",		"FBO",			self.b_slave_onsite		)
			bu:set_text_rect_ratio( 2 )
		bus:move_prev()
		bus:move_right(3)
		bu	=	self:add_flag(	"b_fbo_draw",	"Draw",			true					)
			bu:set_text_rect_ratio( 2 )
		bus:move_prev()
		bus:move_right(3)
		bu	=	self:add_flag(	"b_fbo_out",	"4",			self.b_slave_onsite		)
			bu:set_text_rect_ratio( 2 )

		bus:move_left(6)
		bu = bus:add_but_target_lua( "Back",	nil,	self,	"b_draw_back",	self.b_slave_onsite )
			bu:set_value_load_save( not self.b_slave_onsite )
		bu = bus:add_but_target_lua( "Other",	nil,	self,	"b_draw_other",	false )
			bu:set_value_load_save( true )

		bu = bus:add_but_trig_method( "Camera",	nil,	self,	"set_camera_ui",	true )

		bu = bus:add_selector( "Camera sel",	{nil,nil,	nil,2}	)	--, .8 )
--		bu = bus:add_selector( "Camera sel",	{1,2,		1.8,.4} )	--, .8 )
--		bu = bus:add_selector()
--			bu:set_text( "Camera" )
			bu:set_nb_min_0( 8, 2 )
			bu.do_mouse_move = cam_do_click_down
			bu:set_item_text( 1, "Free" )
			bu:set_item_text( 5, "Wall", "Z", "X", "Top" )
			bu:set_item_text( 9, "PC", "2", "3", "4", "5", "6", "7", "8" )

		bu = bus:add_but_target_lua( "Creatures",	nil,	self.tank, "b_draw_creatures", true )
			bu:set_value_load_save( not self.b_slave_onsite )

		bu = bus:add_but_target_lua( "Fore", 		nil,	self, "b_draw_fore", self.b_slave_onsite )
			bu:set_value_load_save( not self.b_slave_onsite )

		bu = bus:add_but_target_lua( "Draw Screens", nil,	self, "b_draw_screens", false )
			bu:set_text_rect_ratio( 8 )

			bu = bus:add_but_target_lua( "Nb",		nil,	self, "b_draw_screens_nb", false )
				bu:set_text_rect_ratio( 2 )
			bus:move_prev()
			bus:move_right(3)
			bu = bus:add_but_target_lua( "PC",		nil,	self, "b_draw_screens_pc", false )
				bu:set_text_rect_ratio( 2 )
			bus:move_prev()
			bus:move_right(3)
			bu = bus:add_but_target_lua( "Rect",	nil,	self, "b_draw_screens_rect", false )
				bu:set_text_rect_ratio( 2 )
			bus:move_left(6)

		bu = bus:add_trig( "save_bu_pos" ):set_color_back("save")
			bu:set_text_rect_ratio( 8 )
			bu:set_method_on_click( ga, "save_bu_pos" )


		BU.set_pos_load_save_default( nil )

	--	bu = self:add_flag( "b_bo_out",				"FBO PROJ",			self:is_onsite()	)
	--	bu = self:add_flag( "b_gamma",				"FBO Gamma",		true	)
	--	bu = add_slider( "gamma" )
	--		bu:set_min_max( 0, 2 )
	--		bu:set_value( .8 )
	--		aqua.ui.bu_gamma = bu

		bus:move_next()

		bus:move_next()

		self.fxs_back:add_ui()
		self.fxs_other:add_ui()
		self.fxs_fore:add_ui()


	--[[
		function app_fxs_ui()
			local bus = BUS:create( "FXS" )
			bus:set_bu_pos_load_save( false )

			bus:init_begin()
				self.fxs:add_ui()
			bus:init_end()
			return bus
		end
	--]]
		--bu = self:add_flag( "b_fx1",					"FX 1 Draw",		false	)
	--	bu = bus:add_window(  "FXS", app_fxs_ui(),	{0,0, 2,2} )

		bus:move_next()

	bus:end_window()

	return bus
end

function AQUA:define_ui_second()
	local bus = BUS:begin_window( "Aqua 2" )

		local bu

		bus:move_prev()

		bus:add_but_target_param( "Cam See", nil, param.get_ref( self.ref.camera.back.layers, "camera_index_ui" ), 0, 0, 1 )
		bu = bus:add_but_trig_method( "Cam Back", nil,	self, "set_camera_ui", false )
		--[[
			bu = self:add_slider( "size" )
				bu:set_target_param( param.get_ref( self.ref.camera, "ortho_size" ) )
				bu:set_min_max( 1, 20 )
		--]]
		bus:move_next()
		--	axe_DRAW
		bus:add_monitor( "Capture", {1.75,.25, 2,1.5}, 32 )

	bus:end_window()

	return bus
end

function AQUA:add_window_auto( name, bus_down,	i )
	local S = .12
	local bu = bus_cur:add_window( name, bus_down,	{3, 2-i*S*1.2, 2,2} )
	bu:set_size_orig_for_mini( S, S )
	bu:set_active( false )
	return bu
end

function AQUA:define_FXS( fn )
	local bus = BUS:create( "FXS" )
	bus:set_bu_pos_load_save( false )

	bus:init_begin()
		fn()
	bus:init_end()

	return bus
end


function AQUA:master_define_ui()
	local bu
	local bus = BUS:begin_window( "Master" )
	local b_force = true
	bus:set_bu_pos_load_save( false )

		bus:add_wiz_fps( {2.5, 4., 1., .25} )

		bus:move_prev()

		bu = bus:add_but_trig_fn(		"Net local",		nil,	aaa.net.set_local,	0  )
		bu = bus:add_but_trig_fn(		"Net Any",			nil,	aaa.net.set_any,	0 )
		bu = bus:add_but_target_lua(	"Osc Bundle",		nil,	self.osc,		"b_bundle"						)
		bus:move_right(1)
			bu = bus:add_but_target_lua(	"Osc Send",		nil,	self.osc,		"b_send"						)
				bu:set_text_rect_ratio( 8 )
				if b_force then bu:set_value( 0 ) end
			bu = bus:add_but_target_lua(	"Osc Received",	nil,	self.osc,		"b_received"					)
				bu:set_text_rect_ratio( 8 )
				if b_force then bu:set_value( 0 ) end
			bu = bus:add_but_target_lua(	"Master loop no osc",	nil,	self,			"b_master_loop",		false	)
				bu:set_text_rect_ratio( 8 )
				if b_force then bu:set_value( 1 ) end
		bus:move_left(1)

		bu = bus:add_but_target_lua(	"Master",			nil,	self,			"__b_master"					)
		bu = bus:add_but_target_lua(	"Slave",			nil,	self,			"__b_slave"						)
			if b_force then bu:set_value( 1 ) end

	bus:end_window()
	return bus
end

function AQUA:kinect_define_ui()
	local bu
	local bus = BUS:begin_window( "Kinect" )
	bus:set_bu_pos_load_save( false )

		bus:add_wiz_fps( {2.5, 4., 1., .25} )

		bus:move_prev()

		local function add_slider( name, min,max )
			return bus_cur:add_slider_target( name, nil, nil,nil, nil, min,max )
		end
		
		if self.grea_kid then
			for i=1,2 do
				if self.grea_kid[i] then
					bu = bus:add_slider_target( "agitation_ui", {0,0, 20} )
				end
			end
			self.ui.kinect = {}
			--bus:move_next()
				for i = 1,self.kinect_nb do
					local delta = (i-1) * 3
					bus:move_right( delta )
					bu = bus:add_but_target_lua(	"K"..i,	nil,	self,		"b_k"..i,				false	)
						bu:set_text_rect_ratio( 2 )
					self.ui.kinect[i] = bu
					self.grea_kid[i].bu_kinect = bu
					bus:move_left( delta )
					bus: move_prev()
				end
			bus:move_next()
				bu = add_slider( "Blow Left",		0., 3.1	)
					self.grea_kid[1].bu_blow = bu
				bu = add_slider( "Blow Right",		0., 3.1	)
					self.grea_kid[2].bu_blow = bu
		end

	bus:end_window()
	return bus
end


function AQUA:define_app_ui()
	self.tank:define_ui()

	local bus = BUS:create( "Aquarium" )
	bus:set_bu_pos_load_save( true )
	self.ui.bus = bus
	bus:set_active( true )
	bus:init_begin_add_to_ga()

		local bu
		if self.b_kinect then
		else
			bu = self:add_window( "Layers", self:define_ui_first(),		-3,0  )

			local function fxs_add_window()
				self.fxs_back:add_window()
				self.fxs_other:add_window()
				self.fxs_fore:add_window()
			end
			if true then
				fxs_add_window()
			else
				self.bus_fxs = self:define_FXS( fxs_add_window )
				self:add_window( "FXS", self.bus_fxs,	3,1 )
			end

			local Y = 2
			bu = bus:add_window(	"Master",	self:master_define_ui(),	{-1,Y,		2.5,2}	)
			bu = bus:add_window(	"Kinect",	self:kinect_define_ui(),	{ 2,Y,		2,  2}	)

			bus:create_add_bu( BU_BANK,		"BANK", {0, 0,		4, 2.25} )	:set_text_nice()
			--bus:add_wiz_bank(	{0, 0,		4, 2.25} )

		end
	bus:init_end()

	bus:set_method( "draw_before",	self,	"draw_background"	)
	bus:set_method( "draw_after",	self,	"draw_foreground"	)

	ga:register_ui_group( bus )
	ga:set_ui_group_active( false )
--	ga:set_ui_group_active( not self:is_onsite() )

	return bus
end
