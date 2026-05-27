function meu:define_meu_infos( )
	return	{ author = "Mâa",
				name_long="\"bird-oid object\"",
				help="Simultate bird/fish herds and much more",
				tags = { "2d", "3D",  "Art", "Draw", "Point", "Procedural" }
			}
end

function meu:boid_define_ui( boid, grea )
	if not boid then return end

	local ref = self.ref
	local bref = boid.ref
	local ui = self.ui
	local bu
	local par

	local layer = aaa.layers.get_layer( self.__layers, 1 )
	local use_bdd_ref = param.get_ref( layer, "use_bdd" )

	local ix,iy = 1,1	
	local SY = 1
	local DY = .1
	local sx_draw = 1.5
	local s_draw = 17 - sx_draw
	local bu
	local SX = 5/2

	--self:print( "use_bdd_ref is "..use_bdd_ref  )
	bu = self:add_button(		{1,		1,		SY,SY},	"Draw",		use_bdd_ref ):set_min_max( 3, 2 ):set_text_rect_ratio( 3 )
	bu = self:add_trig(			{4,		iy,		SX,SY},	"Restart",	bref.restart_trig )
	bu = self:add_trig_method(	{4+SX,	iy,		SX,SY},	"Focus",	boid, "set_focus" )
	iy = iy + 1

--[[
	bu = self:add_trig_method(	{2, iy},		"Save One", self, "save_points_csv" ):set_color_back("save")
		bu:set_text_rect_ratio( 4 )
	bu = self:add_button(	{	2+4, iy },	"Save", self, "b_save", false ):set_color_back("save")
		bu:set_text_rect_ratio( 2 )
	--param.set( use_bdd_ref, self.b_simul and "update_and_draw" or "update" )
]]--
	if self.b_original_target then
		self:add_camera()
	end
	iy = iy + SY*.2
	local multiple_render = ref.multiple_render
	if multiple_render then
		bu = self:add_selector(	{ix,	iy,		4,.8}, 			"Mode" )
			:set_item_text( 1, "Regular", "Both", "Multiple" )
			:set_target_lua( self, "s_multiple_render", 1 )	
			--:set_min_max( 1, 3 )
		bu = self:add_slider(	{ix+4,	iy,		2,.8},			"Mul_size",	ref.multiple, "size_factor", 1/16, 0,1 ):set_color_back( "factor" )
		self:add_button( 		{ix+6,	iy,		2,.8},			"Mul Lua",	self,"b_multiple_lua", 	false )
	end
	iy = iy + SY + DY

	local iy_start = iy

--table.print( boid, "boid", 2 )
	--self:box_debug( "boid_define_ui" )
	self:set_tab_key_def()
	bu = self:add_selector(	{ix,	iy+SY/2,	4,SY}, 			"Dimension" )
		:set_item_text( 1, "1d", "2d", "3D" )
		:set_target_param( bref.dimension )	
		--:set_min_max( 1, 3 )

	bu = self:add_slider(	{ix+4,	iy,			4,SY},			"Accel_Max",				bref.acceleration_max				):set_min_max( 0, 8 )
	bu = self:add_button(	{ix+4,	iy+SY,		4,SY/2},		"Accel_Max_Target_Box",		bref.acceleration_max_target_box 	):set_text("Target_and_Box"):set_text_rect_ratio(3)
	bu = self:add_button(	{ix+4,	iy+SY+SY/2,	4,SY/2},		"Accel_Max_Interaction",	bref.acceleration_max_interaction	):set_text("Interaction"):set_text_rect_ratio(3)

	bu = self:add_slider_two({ix+8,	iy,			4,SY},			"Speed",					nil, bref.speed_min, bref.speed_max	):set_min_max( 0, 8 )
	bu = self:add_slider(	{ix+8,	iy+SY,		4,SY},			"vertical_ratio",			bref.speed_vertical_ratio_max		):set_min_max( 0, 1 )

	bu = self:add_slider(	{ix+12,	iy+1,		4,SY},			"Viscosity",				bref.viscosity						):set_min_max( 0, .99 )

	bu = self:add_slider(	{ix+12,	iy,			4,SY},			"Internal_radius",			bref.internal_radius					):set_min_max( 0, 1 )
	iy = iy + SY*2 + DY

	local ix = 1.1
	local MX = SY
	local DX = .1
	local SXRGB	= 4
	local XRGB	= 14-SXRGB

	local y_start = iy
	bu = self:add_button(		{1,		iy,		SY,SY },		"Move",						bref.move				)
		ui.bu_move = bu
	bu = self:add_slider(		{ix+5,	iy,		2,SY},			"Visibility_angle",			bref.visibility_angle	)--:set_text( "Inf" )
	bu = self:add_button(		{ix+7,	iy,		SY,SY },		"Visibility_use",			bref.visibility_use		):set_text( "Eye" ):set_text_inside(true)
		 self:add_rgbfa_only(	{XRGB,	iy,		SXRGB,SY}, 		"color",					aaa.obj.get_down_by_class( layer, "color" ) )
	bu = self:add_button(		{s_draw,iy,		sx_draw, SY },	"Draw_Force",				bref.draw_force			):set_text( "Draw" )
	iy = iy + SY + DY

	for _,force in PAIRS( boid.forces ) do
		local force_low = string.lower(force)
		local t = bref[force_low]

		bu = self:add_button(		{ix,	iy,		SY,SY},		force,						t.active			):set_text_rect_ratio( 2 )
		bu = self:add_slider(		{ix+3,	iy,		2,SY},		force.."_inf",				t.influence			):set_text( "Inf" )
		if force == "Field" then
			bu:set_min_max( -1, 1 )
		else
			bu:set_min_max( 0, 1 )
		end 
		if t.distance then
			bu = self:add_slider(	{ix+5,	iy,		2,SY},		force.."_distance",			t.distance			):set_text( "Dist" ):set_min_max( 0, 2 )
			bu = self:add_button(	{ix+7,	iy,		SY,SY},		force.."_visibility_use",	t.visibility_use	):set_text( "Eye" ):set_text_inside(true)
		end
		if t.x then
			self:add_sliders_xyz(	{ix+5,iy,		4,SY},		force,						t, 	true,	32,	{ -28, -24, -20, -16, -12, -8, -4, -1, 1, 4, 8, 12, 16, 20, 24, 28 }	)
		end
		if force == "Box" then
			bu = self:add_button( 	{ix+9,iy,		2.5,SY}, 	force.."_type"				)
				bu:set_menu( { "No", "Die", "Wrap Rnd", "Wrap", "Bounce", "Repulse"  } )
				bu:set_text_selector( true )
				bu:set_target_param( t.type )
				bu:set_draw_by_value( 0, "false" )
			bu = self:add_button(	{ix+11.5, iy,	2,SY },		force.."_sphere",			t.sphere		):set_text( "Sphere" )--:set_text_rect_ratio( 3 )

		end
		if t.sx then
			self:add_sliders_sxyzf(	{ix+5-4/3,iy+SY,	4*4/3,SY},	force,					t,	true,	1,	32,	{ 1, 2, 4, 8, 12, 16, 20, 24, 28 }	)
		end
		if t.draw then
			bu = self:add_button(	{s_draw,iy,	sx_draw,SY },	force.."_draw",				t.draw			):set_text( "Draw" )
			if t.draw_factor then
				local cref = COLOR_REF:create( force, boid:get_obj(), force.."_" )
				self:add_rgba(		{XRGB,iy,	SXRGB,1},		force.."_color",			cref			):set_text( force )
				bu = self:add_slider({14,iy,	1.5,SY},		force.."_draw_factor",		t.draw_factor	):set_text_draw( false ):set_min_max( 0, 1 )
				--todo
				--bu = self:add_rgba({10,iy,	3.5,SY},		force.."_rgba"								):set_text( "" )
				--	ui["bu_"..force_low.."_rgba"] = bu
			end
		end
		if t.border_size then
			iy = iy + SY
			bu = self:add_slider(	{12,iy,		2,SY},			force.."_border_size",		t.border_size	):set_text( "Border" )
		end

		if t.radius_external then
			ix = ix + 9
			local SX = 3.5
			bu = self:add_slider_two({ix+DX,iy,	SX,SY},			force.."_radius_external",	nil, t.radius_internal, t.radius_external	)
				bu:set_text("radius"):set_min_max( 0, 8 ):add_values_def_integer()
			bu = self:add_slider(	{ix+DX+SX,iy,SX*.5,SY},		force.."_normal",			t.normal			):set_text( "Normal" )
				bu:set_min_max( -1, 1 ):add_values_def( -.5, .5 )
			ix = ix - 9
		end

		iy = iy + SY
		if t.net_draw then
			local cref = COLOR_REF:create( force.."_net", boid:get_obj(), force.."_net_" )
			self:add_rgba(			{XRGB,iy,	SXRGB,1},		force.."_net_color",		cref			):set_text( "Net" )
			bu = self:add_button(	{15.5,iy,	1.5,SY },		"Net",						t.net_draw		):set_text_rect_ratio( 2 )
			bu = self:add_button(	{2+3, iy,	SY,SY },		"Net_ease",					t.net_ease		):set_text_draw( false )
					--bu:set_text_rect_ratio( 1 )
			bu = self:add_slider_two({2+4,iy,	2,SY},			"Ease",						nil, t.net_ease_in, t.net_ease_out,		0, 1 )
			iy = iy + SY
		end

		iy = iy + SY * .00 + DY
	end

	bu = self:add_button(			{9,	iy-SY*2 },				"Deform",					bref.deform_active		):set_text( "Deform" )--:set_text_rect_ratio( 3 )

	self:set_tab_key( "More" )
	iy = y_start - 2
	self:add_sliders_xyz(			{ix,iy,		8,SY},			"Acc",						bref.acceleration, 	false,	16	)
	self:add_sliders_xyz(			{ix,iy+SY,	8,SY},			"Speed",					bref.speed, 		false,	16	)
	--	add_sliders_xyz(				{ix, iy+2,	8,SY},		"offset",	bref.offset, 		false,	16	)

	iy = iy + 3 * SY

--todo now that we don't init grea with behavior always perhaps this need to be touch up
	if grea then
		local be = grea.behavior
		if be then
			-- go around
			bu = self:add_button(	{ix,iy}, 			"Go_around", 			be.go_around, "b_active",	 		nil				)
			bu = self:add_button(	{ix+5,iy},	 		"GA_Is_Rotation", 		be.go_around, "b_is_rotation",		true			):set_text( "Rotation" )
			bu = self:add_button(	{ix+10,iy},	 		"GA_Clockwise", 		be.go_around, "b_begin_clockwise",	true			):set_text( "Clockwise" )
			iy = iy + SY
			bu = self:add_slider(	{ix+5,iy,	2,SY},	"GA_Pos_min",			be.go_around, "min_pos",	 		nil, -180, 180	):set_text( "Pos min" )
			bu = self:add_slider(	{ix+7,iy,	2,SY}, 	"GA_Pos_max", 			be.go_around, "max_pos",	 		nil, -180, 180	):set_text( "Pos max" )
			bu = self:add_slider(	{ix+9,iy,	2,SY}, 	"GA_Pos_begin", 		be.go_around, "begin_pos",	 		nil, -180, 180	):set_text( "Pos begin" )
			iy = iy + SY
			bu = self:add_slider(	{ix+5,iy,	2,SY},	"GA_Duration", 			be.go_around, "duration",	    	nil, 0, 120		):set_text( "Duration" )
			iy = iy + SY
			bu = self:add_slider(	{ix+5,iy,	2,SY}, 	"GA_Depth_min", 		be.go_around, "depth_min",	 		nil, 0, 20		):set_text( "Depth min" )
			bu = self:add_slider(	{ix+7,iy,	2,SY}, 	"GA_Depth_max", 		be.go_around, "depth_max",	 		nil, 0, 20		):set_text( "Depth max" )
			iy = iy + 1.5*SY

			-- random walk
			bu = self:add_button(	{ix,iy}, 			"Random_walk", 			be.random_walk, "b_active",			nil				)
			bu = self:add_slider(	{ix+5,iy,	2,SY},	"RW_Speed",				be.random_walk, "speed",			nil, 0, 1		):set_text( "Speed" )
			bu = self:add_slider(	{ix+7,iy,	2,SY}, 	"RW_Border", 			be.random_walk, "border",	    	nil, 0, 1		):set_text( "Border" )
			iy = iy + SY
			bu = self:add_button(	{ix,iy}, 			"RW_Is_Stationary",		be.random_walk, "b_is_stationary",	nil				):set_text( "Is stationary" )
			bu = self:add_slider(	{ix+5,iy,	2,SY}, 	"RW_Time_Stationary", 	be.random_walk, "time_stationary",	nil, 0, 60		):set_text( "Time stationary" )
			bu = self:add_slider(	{ix+7,iy,	2,SY}, 	"RW_Time_Moving", 		be.random_walk, "time_moving",		nil, 0, 60		):set_text( "Time moving" )
			iy = iy + 1.5*SY

			-- radius changes
			bu = self:add_slider(	{ix+5,iy,	2,SY}, 	"Radius_min", 			be.radius_changes, "radius_min", 	nil, 0, 5		):set_text( "Radius min" )
			bu = self:add_slider(	{ix+7,iy,	2,SY}, 	"Radius_max", 			be.radius_changes, "radius_max",	nil, 0, 5		):set_text( "Radius max" )
			iy = iy + SY

			-- -- handled in the random walk and go_around, not exposed anymore 
			-- -- y movement
			-- bu = self:add_button({ix, iy }, 			"Y_Movement", 			be.y_movement, "active",			nil				)
			-- bu = self:add_slider({ix+5,iy,	2,SY},	"Y_Speed",				be.y_movement, "speed",				nil, 0, 10		):set_text( "Speed" )
			-- bu = self:add_slider({ix+7.5,iy,	2,SY}, 	"Y_Randomness", 		be.y_movement, "randomness",		nil, 0, 100		):set_text( "Randomness" )
			-- bu = self:add_slider({ix+10 ,iy,	2,SY}, 	"Y_Border", 			be.y_movement, "border",	    	nil, 0, 1		):set_text( "Border" )
			-- iy = iy + SY

			-- legacy
			-- bu = self:add_button({ix, iy }, 			"Rotation", 			boid.behavior, "target_rotate",				false	)
			-- bu = self:add_slider({ix+5,iy,	2,SY},	"Seconds_per_Turn",		boid.behavior, "target_rotation_second_per_turn", 5, 0, 60	)
			-- bu = self:add_slider({ix+7.5,iy,	2,SY}, 	"Rotation_Radius", 		boid.behavior, "target_rotation_radius",	0,		0, 100	)
			-- bu = self:add_slider({ix+10,iy,	2,SY}, 	"Rotation_Adherence", 	boid.behavior, "target_rotation_adherence",	0.1,	0, 10	)
			-- bu = self:add_button({ix+12.5, iy}, 		"Rotation_Clockwise", 	boid.behavior, "target_rotation_clockwise",	true	)
			-- iy = iy + SY
			-- bu = self:add_button({ix, iy }, 			"Noise", 				boid.behavior, "target_noise",				false	)
			-- bu = self:add_slider({ix+5,iy,	2,SY},	"Noise Power",			boid.behavior, "target_noise_power",		0,		0, 10	)
			-- bu = self:add_slider({ix+10,iy,	2,SY},	"Noise Activity",		boid.behavior, "target_noise_activity",		0,		0, 360	)
			-- iy = iy + SY
			-- bu = self:add_button({ix, iy }, 			"Y axis", 				boid.behavior, "target_mvt_y_axis",			false	)
			-- bu = self:add_slider({ix+5,iy,	2,SY},	"Y Noise Power",		boid.behavior, "target_mvt_y_noise_power",	0,		0, 10	)
			-- bu = self:add_slider({ix+10,iy,	2,SY},	"Y Noise Activity",		boid.behavior, "target_mvt_y_noise_activity", 0,	0, 360	)

			-- iy = iy + SY
			-- bu = self:add_slider({ix,iy,		2,SY},	"Movement Scaling",		boid.behavior, "target_mvt_scaling",		1,		0, 10	)
			-- iy = iy + SY
		end
	end

	--BU_TEXTURE:set_bank_line_next( 4 )
	self:add_rendering() 
	ix,iy = 9,iy_start
	self:add_transfo( {ix,iy} )
	iy = iy + 3.2 + DY
	bu = self:add_bu_texture_target_layer(	{ix,iy, 8,5},	"Boid_Tex",	1, true, self:get_layer(1),self:get_layer_mapping(1) )
		ui.bu_tex_boid = bu


	self:set_tab_key( "Def" )
	if ref.def_bind then
		ix,iy = 1,iy_start

		bu = self:add_bu_texture(			{1,iy,	8,5},	"Deformer_Tex",	8 )
			ui.bu_tex_def = bu
	end

	self:set_tab_key( "Master" )
		iy = y_start
		bu = self:add_button(		{ix,iy,		SY,SY},	"Draw Force",		bref.master.draw_force		)
			bu:set_value_load_save(false)
		iy = iy + SY*1.5

		bu = self:add_selector(		{ix+1,iy,	7,SY},	"Draw Net"	)
			:set_item_text( 1, "No", "Allow", "FORCE" )
			:set_target_param( bref.master.draw_net )
			bu:set_value_load_save(false)
		iy = iy + SY

		bu = self:add_button(		{ix,iy,		SY,SY},	"Box Draw Allow",	bref.master.box_draw			)
			bu:set_value_load_save(false)
		iy = iy + SY

		bu = self:add_button(		{ix,iy,		SY,SY},	"Box Draw Force",	bref.master.box_draw_force	)
			bu:set_value_load_save(false)

		if not self.b_original_target then
			--bus_cur:set_bu_pos_load(false)
			bus_cur:set_bu_pos_load_save(false)
		end
	--self:print( "in define "..param.get( bref.repulse.influence) )

end

function meu:define_ui()
	self:boid_define_ui( self.__boid, self.__grea )
end
function meu:do_nothing()	end

function meu:get_boid()		return self.__boid		end

function meu:set_boid_and_layers( boid, layers, grea )
	-- table.print( boid, "boid assigned", 2 )
	local ref = self.ref
	ref.def_image = nil
	ref.def_bind = nil
	self.__layers = layers
	if layers then
		ref.def_image	= aaa.obj.get_down_by_class_no_error( layers, "def_image" )
		if ref.def_image then
			ref.def_bind = param.get_ref( ref.def_image, "image_src" )
		end
	end
	self.__boid = boid
	self.__grea = grea
	self.b_original_target = boid == self.__boid_original

	local save_behavior
	if grea then
		save_behavior = table.copy_deep( grea.behavior )
	end

	if not self.b_original_target then
		self:redefine_bus( false )
		self.save_preset = self.do_nothing
	end

	if grea then
		-- hack. doesn't work if the table isn't exactly nested with depth 2.
		for k, v in PAIRS(save_behavior) do
			for k2, v2 in pairs(v) do
				grea.behavior[k][k2] = v2
			end
		end
		-- copy_deep doesn't work this way since it changes subtable pointers.
		-- grea.behavior = table.copy_deep(save_behavior)
	end
end

function meu:init()
	local ref = self.ref
	--self:box_debug( "meu:init" )
	local boid	= self:get_layer_bdd_if_class( 1, "bdd_boid" )
	if boid then	
		self.__boid_original = BOID:create( self:get_name(), boid )
		--self:box_debug( "self.__boid_original "..self.__boid_original )
		self:set_boid_and_layers( self.__boid_original, self:get_layers()  )
	end
	
	if ref.multiple then
		ref.multiple_render = param.get_ref( ref.multiple, "render" )
		ref.multiple_axe = param.get_ref( ref.multiple, "axe" )
	end
end

function meu:update_ui()
	-- self:print(self.__grea.behavior.go_around.active)
	local bu = self.ui.bu_move
	if bu then
		bu:set_text_color_problem_white( bu:get_value_as_bool() )
	end
end

function meu:update()
--lala	self:read_target()
	local ui = self.ui
	local ref = self.ref
	local bu_tex = ui.bu_tex_def
	if bu_tex then
		local b_changed, bind_2d = bu_tex:get_bind_2d_asked()
		if b_changed then
			param.set( self.ref.def_bind, bind_2d )
		end
	end
end

local tab_mutiple_render = {0,2,1}
function meu:draw()
	local ref = self.ref

	local multiple_render = ref.multiple_render
	local b_multiple_lua
	if multiple_render then
		local s_multiple_render = tab_mutiple_render[self.s_multiple_render]
		param.set( multiple_render, s_multiple_render )
		b_multiple_lua = s_multiple_render>0 and self.b_multiple_lua
		if b_multiple_lua then
			param.set( ref.multiple_axe, 2 )
			gol.set_attrib_uv( {0,0, 0,1, 1,1, 1,0} )
			aaa.bdd.hook_draw_multiple = function ( id )
											--self:print( id )			
											local ph = id + aaa.time.t * (3. + (id%17)/16)
											ph = math.sin( ph )
											ph = ph * ph
											ph = ((ph-.5) * .3 - .05) * math.pi2

											local c = math.cos( ph )
											local s = math.sin( ph )
											local su = .5
											c = c * su
											s = s * su
											local zc = .333
											gol.draw_triangle_fan_uv_3d( 0,0,0,	c,s,zc, 0,0,1, -c,s,zc )
										end
			
		else
			--param.set( ref.multiple_axe, 0 )
		end
	end


	--multiple_render
	if self.b_original_target then
		MEU.draw( self )
	end

	-- remove the drawing hook if necessary
	if b_multiple_lua then
		aaa.bdd.hook_draw_multiple = nil
	end

	-- if self.b_save then
	-- 	self:save_points_csv()
	-- end
end

--todo generalize for all bdd
function meu:save_points_csv()
	if true then return end
	self:print( "save_points_csv" )
	--todo get_always_instance_of_class
	local pl = self.__pl
	if not pl then pl = POINT_LIST:create() self.__pl = pl end
	pl:read_bdd( self.ref.bdd )
	local i = self.__save_index or 0
	i = i + 1
	self.__save_index = i
	local fpath = string.format( "../Export/boid_%04d.csv", i )
	pl:save( fpath, "csv", self.ui.bu_tex_def:get_bind_asked(), 1.7777*8, 8 )
end

