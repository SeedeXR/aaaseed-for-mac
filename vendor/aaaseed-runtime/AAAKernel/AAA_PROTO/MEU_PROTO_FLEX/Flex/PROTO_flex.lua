function meu:define_meu_infos( )
	return { author = "Mâa mrvux",
			tags = { "3D", "Draw", "Core", "CoreGraphic", "Procedural", "point", "VJ" },
			help = "Encapsulate use of c_bdd_flex using Nvidia Flex Physic Simulator\n"..
					"Used to do fluids, soft and rigid bodies\n"..
					"really fun but it required shader expertise"
			 }
end

function meu:define_ui()
	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	local ix = 1
	local iy = .8
	local SY = 1
	local SX = 8
	local DY = .1
	local IY_BEGIN = 4.5

	local bdd = ref.bdd

-- add iterations

	self:add_camera()

--	bu = self:add_slider(	{ix,iy,	8,SY}, "Size", 			sha:get_ref_frag_float(1), nil, 0, -1, 1 )
	-- bu = self:add_selector(	{ix+4,iy,	4,SY*2},	"Nb info" )
	-- 	bu:set_nb( 3, 2 )
	-- 	bu:set_item_text( 1, "16K", "64K", "256K", "1M", "4M", "16M" )
	-- 	bu:set_item_data( 1, 128*128, 256*256, 512*512, 1024*1024, 2048*2048, 4096*4096 )

	local SYP = 3.6
	local SYB = (SYP-DY)*3/4
	self:add_particle_nb_ui(	{ix,iy,	8,SYB},	ref.bdd, "particle", true )
	iy = iy + SYB + DY

	SYB = (SYP-DY)/4
	local SX1 = 1.5
	self:add_trig_method(	{ix,iy,		SX1,SYB},	"Build", 		self,	"flex_build_particles" )
		:set_color_back( "reset" )
	bu = self:add_button( 	{ix+SX1,iy,	4-SX1,SYB},	"Init type" )
		bu:set_menu{ 	"Cloth",		"RCube Maa", 	"Sphere Maa",		"Rope Maa",			"Nigids",			"Rope",				"Mixed",
						"et 8x4",
						"Crash", 		"Sphere Maa PB",	"Baballe",			"Net",
						"Baballe Mini", "Baballe Maa",		"Baballe Maa Bis",	"Bandeau"
				   }:set_nb_min_0( 4, 4 )
		bu:set_text_selector(true)
	ui.bu_init_type = bu:get_selector()
	ui.bu_init_type:set_method_on_value_change( self,	"flex_build_particles" )

	--todo generalize
	local S = 4/3
	local x = ix+4 
	self:add_button(		{x,	iy,		S,SYB},	"Update",		self, "b_update", 				true )
	self:add_button(		{x+S,iy,	S,SYB},	"Run",			ref.bdd, "solver_run", 			true )
	self:add_button(		{x+S*2,iy,	S,SYB},	"Comp",			ref.bdd, "force_field_compute", true )
	bu = self:add_selector(	{9,iy,		8,SYB},	"Draw_modes"	)
		ui.bu_draw = bu
		bu:set_selection_multiple( true )
		bu:set_item_text(	1, "Particle", "Velocity", "Spring", "Transform", "Triangle", "Acceleration" )
		bu:set_sel_multiple_item( 1, true )
	
	iy = iy + SYB
	DY = .2

	self:add_shading_ui_multiple( {9,2} )
		
	bu = self:add_slider(	{15,2,		2,SY},	"Comp_int_1",	self, "sha_comp_int_1",		0,	0,32 )
		bu:set_value_type_integer( true )
		bu:set_min_max_strict( true )

	iy = IY_BEGIN
	iy = iy + SY + DY

	self:set_tab_key_def()
	iy = IY_BEGIN

	iy = iy + SY + DY
--	self:add_mapping_by_side_only()
	bu = self:add_rgbf(		{1,iy,		8,SY}	)
		self.ui.bu_color = bu
	iy = iy + SY
	self:add_blending(		{1,iy} )
	iy = iy + SY + DY
	--todo replace by compute floats/int/vec..
	self:add_slider(		{ix,iy,		8,SY},	"Density Y",	self, "sha_density_y",	0,	-1,1 )
	iy = iy + SY + DY
	self:add_slider(		{ix,iy,		6,SY},	"Time Divider",	self, "time_divider",	1,	0,32 )
	self:add_button(		{ix+6,iy,	2,SY},	"Time Wrap",	self, "b_time_wrap",	false	):set_text( "Wrap" )


	self:add_rendering(		{1,nil,		8,SY*2} )

	--todomaa generalise at loww levels
	local function add_xyz( rect,	text, param_name_pre, norm, values_def, pre )
		local tab = {	x = param.get_ref( bdd, param_name_pre.."x" ),
						y = param.get_ref( bdd, param_name_pre.."y" ),
						z = param.get_ref( bdd, param_name_pre.."z" ) }
		--table.print( tab, "add_xyz" )
		self:add_sliders_xyz(	rect,	text,	tab, false, norm, values_def, pre )
	end
	iy = iy + SY*3

	ix,iy = 9, IY_BEGIN
	self:add_slider(	{ix,iy,		8,SY},	"Part Size", 		self, "part_size", 1, 0, 4	 )
	iy = iy + SY
	self:add_button(	{ix,iy,		SY,SY},	"Use Texture",		self, "b_sha_tex_use", 	false )
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},	"Hardness",			self, "sha_hardness",	1,	0,8 )
	self:add_slider(	{ix+4,iy,	4,SY},	"Round",			self, "sha_round", 		1,	0,1 )
	iy = iy + SY + DY

	self:add_button(	{ix,iy,		SY,SY},	"density Compute",	bdd, "density", false	)
	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},	"Density Size",		self, "density_size",	0, 0,1 )
	iy = iy + SY + DY

	
	bu = self:add_button({ix,iy,	SY,SY},	"Smoothing Enable",	bdd, "smoothed_position", 	false	)
		bu:set_text_visible(false)
	self:add_slider(	{ix+1,iy,	7,SY},	"Smoothing", 		bdd, "smoothing",	1, 0,10	 )
	iy = iy + SY + DY


	local fall = {}
	self.fall = fall
	self:add_button( 	{ix,iy,		SY,SY},	"Fall", 			fall, "b_active", 	false	)
	self:add_slider(	{ix+3,iy,	5,SY},	"Fall Age",			fall, "age",		8, 0, 32 ):set_text( "Age"):add_values_def( 1,4,5,8,10,16 )
	self:add_slider(	{ix,iy+SY,	8,SY},	"Fall Speed",		fall, "speed",		0, 0, 10 ):set_text( "Speed" ):add_values_def( 1,2,4 )
	self:add_slider(	{ix,iy+SY*2,8,SY},	"Fall Ease Time",	fall, "ease_time", 	0, 0, 16 ):set_text( "Ease Time" ):add_values_def( 1,2,3,4,5,8,10 )
	iy = iy + SY*3 + DY


	self:set_tab_key( "Emit" )
	ix,iy = 1,IY_BEGIN
	SY = .94
	self:add_slider(	{ix,iy,		8,SY},	"Emit NB Max", 		bdd, "emitter_emit_nb",			512,   	0, 65536	)
	iy = iy + SY
	self:add_button(	{ix+1,iy,	SY,SY},	"Emit per second", 	bdd, "emitter_emit_per_second", 		true	)
		:set_text_rect_ratio(6)
	iy = iy + SY
	self:add_button( 	{ix+1,iy,	SY,SY},	"Emit Image",		bdd, "emitter_image", 			true	)
	iy = iy + SY + DY

	self:add_button( 	{ix,iy,		SY,SY},	"Coverage Enable", 	bdd, "emitter_image_coverage_enabled", 	false	)
		:set_text_rect_ratio(6.5)
	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},	"Coverage", 		bdd, "emitter_image_coverage",		.5,	0, 1	)
	iy = iy + SY + DY

	--todo move to rgb
	bu = add_xyz(		{ix,iy,		8,SY},	"Color_Min",		"emitter_image_color_min_", 		.1, {0, 1 }	)
	iy = iy + SY
	bu = add_xyz(		{ix,iy,		8,SY},	"Color_Max",		"emitter_image_color_max_", 		1., {0, 1 }	)
	iy = iy + SY + DY
	bu = add_xyz(		{ix,iy,		8,SY},	"Color_Selector",	"emitter_image_color_selector_", 	1., {0, 1 }	)
	iy = iy + SY + DY

	self:add_slider(	{ix,iy,		4,SY},	"Emit 1 Rate", 		bdd, "emitter_1_rate",			1,   	0, 512	)
	self:add_slider(	{ix+4,iy,	4,SY},	"Emit 2 Rate", 		bdd, "emitter_2_rate",			1,   	0, 512	)
	iy = iy + SY + DY
	self:add_slider(	{ix,iy,		8,SY},	"Mass", 			bdd, "mass",					1  	)
	iy = iy + SY + DY

	ix,iy = 9,IY_BEGIN
	
	local function add_emit_geo(i)
		local pre = "emitter_"..i.."_"
		bu = add_xyz(	{ix,iy,		8,SY},	"Pos_"..i,			pre.."pos_",		8, {-4, 4 }, "Emit_"	)
		iy = iy + SY	
		bu = add_xyz(	{ix,iy,		8,SY},	"Size_"..i,			pre.."size_",		8, {-4, 4 }, "Emit_"	)
		iy = iy + SY	
		bu = add_xyz(	{ix,iy,		8,SY},	"Vel_"..i,			pre.."vel_",		8, {-4, 4 }, "Emit_"	)
		iy = iy + SY	
		bu = add_xyz(	{ix,iy,		8,SY},	"Jitter_Pos_"..i,	pre.."jitter_pos_",	8, {-4, 4 }, "Emit_"	)
		iy = iy + SY	
		bu = add_xyz(	{ix,iy,		8,SY},	"Jitter_Vel_"..i,	pre.."jitter_vel_",	8, {-4, 4 }, "Emit_"	)
		iy = iy + SY + DY
	end						
	add_emit_geo(1)
	add_emit_geo(2)


	self:set_tab_key( "Sim" )
	ix,iy = 1,IY_BEGIN
	SY = 0.85
	bu = add_xyz(		{ix,iy,		8,SY},	"Gravity",	"gravity_", 	9.81,	{-4, 4 }	)
	iy = iy + SY + DY

	self:add_slider(	{ix,iy,		8,SY},	"Friction Dynamic",			bdd, "friction_dynamic",	0.03125,	0,4 )
	self:add_slider(	{ix,iy+SY,	8,SY},	"Friction Static",			bdd, "friction_static",		0.015,		0,4 )
	self:add_slider(	{ix,iy+SY*2,8,SY},	"Friction Particle",		bdd, "friction_particle",	0.0625,		0,4 )
	iy = iy + SY*3 + DY
	self:add_slider(	{ix,iy,		8,SY},	"Restitution",		    	bdd, "restitution",		    0.2,		0,4 )
	self:add_slider(	{ix,iy+SY,	8,SY},	"Adhesion",	            	bdd, "adhesion",	        1/16,		0,1 )
	iy = iy + SY*2 + DY
	self:add_slider(	{ix,iy,		8,SY},	"Shock Propagation",		bdd, "shock_propagation",	0,			0,1	)
	self:add_slider(	{ix,iy+SY,	8,SY},	"Solid Pressure",   		bdd, "solid_pressure",	  	0,			0,1	)
	self:add_slider(	{ix,iy+SY*2,8,SY},	"Free Surface Drag",		bdd, "free_surface_drag",   0,			0,1	):add_values_def( .1, .25, .5, .75 )
	iy = iy + SY*3 + DY

	self:add_slider(	{ix,iy,		8,SY},	"Collide Distance",			bdd, "collision_distance",    		0,	0,1	)
	self:add_slider(	{ix,iy+SY,	8,SY},	"Collide Margin Particle",	bdd, "collision_margin_particle",   0,	0,1	)
	self:add_slider(	{ix,iy+SY*2,8,SY},	"Collide Margin Shape",		bdd, "collision_margin_shape",		0,	0,1	)
	iy = iy + SY + DY

	ix,iy = 9,IY_BEGIN
	SY = 0.85
	self:add_slider(	{ix,iy,		8,SY},	"Radius", 			bdd, "radius",					.05,	0,.5	):add_values_def( .05, .1, .125, .25, .5, .75 )
	self:add_slider(	{ix,iy+SY,	4,SY},	"Rest Solid", 		bdd, "rest_factor_solid",		.55,	.5,.7	):add_values_def( .6, .65, .75 )
	self:add_slider(	{ix+4,iy+SY,4,SY},	"Rest Fluid", 		bdd, "rest_factor_fluid",		.55,	.5,.7	):add_values_def( .6, .65, .75 )
	iy = iy + SY*2 + DY
	self:add_slider(	{ix,iy,		8,SY},	"Dissipation", 		bdd, "dissipation",				0,		0,4		):add_values_def( .125, .25, .5, .75 )
	self:add_slider(	{ix,iy+SY,	8,SY},	"Damping", 			bdd, "damping",					1,		0,4		):add_values_def( .125, .25, .5, .75 )
	iy = iy + SY*2 + DY
	self:add_slider(	{ix,iy,		8,SY},	"Cohesion", 		bdd, "cohesion",				1,		0,4		):add_values_def( .125, .25, .5, .75 )
	self:add_slider(	{ix,iy+SY,	8,SY},	"Surface Tension", 	bdd, "surface_tension", 		0.125,	0,4		):add_values_def( .125, .25, .5, .75 )
	self:add_slider(	{ix,iy+SY*2,8,SY},	"Viscosity", 		bdd, "viscosity",				32,		0,200	):add_values_def( 1, 2, 4, 8, 32, 50, 64, 100, 128 )
	iy = iy + SY*3 + DY
	self:add_slider(	{ix,iy,		8,SY},	"Buoyancy", 		bdd, "buoyancy",				1,		0,1		):add_values_def( .1, .25, .5, .75 )
	self:add_slider(	{ix,iy+SY,	8,SY},	"Vorticity", 		bdd, "vorticity_confinement",	128,	0,512	)
	iy = iy + SY*2 + DY

-- COLLIDERS
	self:set_tab_key( "Colli" )
	ix,iy = 1,IY_BEGIN
	self:add_button(	{ix,iy,		SY,SY},	"Floor", 		    bdd, "floor", 			true	)
	self:add_slider(	{ix,iy+SY*1,8,SY},	"Floor Y", 			bdd, "floor_y",			.0,   	-8, .8	)
	iy = iy + SY * 3
	for i=1,7 do
		self:add_button({ix,iy,		SY,SY},	"Plane " .. i,		bdd, "plane_".. i, 		true	)
		--bu = add_xyzw({ix + 5,iy,	8,SY},	"",	"plane_" .. i .. "_plane_", 	0,	{-4, 4 }	)
		iy = iy + SY * 1
	end
	ix,iy = 9,IY_BEGIN
	self:add_button(	{ix,iy,		SY,SY},	"Colliders",		bdd, "colliders",		true	)
		:set_text_rect_ratio(6.5)
	iy = iy + SY

	--self:add_slider(	{ix,iy,		4,SY},	"Wrap X Min",		self, "sha_wrap_x_min",	-5,	-100,100 )
	--self:add_slider(	{ix,iy,		4,SY},	"Wrap X Max",		self, "sha_wrap_x_max",	-5,	-100,100 )
	--self:add_slider(	{ix+4,iy,	4,SY},	"Round",			self, "sha_round", 		1,	0,1 )
	-- for i=1,2 do
	-- 	self:add_button(	{ix+1,iy,	SY,SY },	"Collider "..i,	bdd, "collider_"..i,	true	)
	-- 		:set_text_rect_ratio(6.5)
	-- 	iy = iy + SY
	-- end
	iy = iy + DY
	bu = self:add_selector(	{ix,iy,	8,1}, "Colliders Switch" )
		bu:set_nb( 8,1 )
		bu:set_item_text_from_nb()
		bu:set_selection_multiple( true )
		bu:set_text_visible( false )
		ui.bu_collider_active = bu
	iy = iy + 1 + DY
	--iy = iy + SY
	--bu = add_xyz( 	{ix,iy,	SX/3,SYM},	SX/3,0,	"Position",	"collider_1_pos",	8, {-400, 400 }, "Col"	)
	--iy = iy + SY
	--bu = add_xyz( 	{ix,iy,	SX/3,SYM},	SX/3,0,	"Rotation",	"collider_1_rot",	8, {-1, 1 }, "Col"	)
	--iy = iy + SY
	--bu = add_xyz( 	{ix,iy,	SX/3,SYM},	SX/3,0,	"Size",		"collider_1_size",	8, {-400, 400 }, "Col"	)


	self:set_tab_key( "SYS" )
	SY = 1.2
	ix,iy = 1,IY_BEGIN+SY
	self:add_slider(	{ix,iy,	8,SY},		"Max Contact",			bdd, "particle_contact_nb_max",		6,		0,32 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Max Neighbour",		bdd, "particle_neighbour_nb_max",	96, 	0,194 )
	iy = iy + SY

	ix,iy = 9,IY_BEGIN
	self:add_button( 	{ix,iy,	SY,SY},		"Relaxation Local", 	bdd, "relaxation_mode", 			false	)
		:set_text_rect_ratio(6.5)
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Relaxation Factor", 	bdd, "relaxation_factor",			1,		0,1	)
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Substeps",				bdd, "substep_nb",					3,		1,32 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Iterations",			bdd, "substep_iteration",			3,		1,32 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Speed Max",			bdd, "speed_max",					100000,	10,10000000 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Accel Max",			bdd, "acceleration_max",			50,		2,100000 )
	iy = iy + SY
	self:add_slider(	{ix,iy,	8,SY},		"Sleep Threshold",		bdd, "sleep_threshold",				0,		0,10 )


	self:set_tab_key( "TEX" )
	ix,iy = 1,IY_BEGIN
	local tex_nb = 4
	local SYT = 5
	self:add_bu_texture_target_unit( 	{ix,iy, 	4,SYT}, "Particle",	1, false )
	self:add_bu_texture_target_unit( 	{ix+4,iy, 	4,SYT}, "Tex_Emit",	2, false )
	self:add_bu_texture(				{ix+8,iy,	4,SYT}, "Tex_"..5,	5, false )
	self:add_bu_texture(				{ix+12,iy,	4,SYT},	"Tex_"..6,	6, false )
	iy = iy + SYT
	self:add_bu_texture_target_unit(	{ix,iy,		4,SYT}, "Tex_"..3, 3, false )	-- for now a Distance field
	self:add_bu_texture_target_unit(	{ix+4,iy,	4,SYT},	"Tex_"..4, 4, false )
	self:add_bu_texture(				{ix+8,iy,	4,SYT}, "Tex_"..7, 7, false )
	self:add_bu_texture(				{ix+12,iy,	4,SYT},	"Tex_"..8, 8, false )


	self:set_tab_key( "Clo" )
	ix,iy = 1,IY_BEGIN
	SY = 0.85

	bu = add_xyz(			{ix,iy,		8,SY},	"Center",	"center_", 	8, {-1,0,1 }	)
	iy = iy + SY + DY
	bu = self:add_selector(	{ix,iy,		8,SY},	"pin" )	--notext not save in preset
		:set_nb_min_0( 4 )
		:set_item_text( 1, "None", "AllBorder", "AllCorner", "TopBorder" )
		par = param.get_ref( ref.bdd, "pin" )
		bu:set_target_param( par )
		bu:set_text_draw( false )

	iy = iy + SY + DY
	bu = self:add_selector(	{ix,iy,		8,SY},	"mass_mode" )	--notext not save in preset
		bu:set_nb_min_0( 2 )
		bu:set_item_text( 1, "Constant", "Density")
		local parmass = param.get_ref( ref.bdd, "mass_mode" )
		bu:set_target_param( parmass )
		bu:set_text_draw( false )
	iy = iy + SY
--	self:add_slider(	{ix,iy,		8,SY},	"Mass_Constant", 	bdd, "mass",			1  	)
--	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},	"Mass_Density",		bdd, "mass_density",	1 )
	iy = iy + SY + DY

	local function add_spring_ui( name )
		local loname = string.lower( name )
		self:add_button( 	{ix,iy,			SY,SY},		loname.."_use", 			bdd, loname.."_use", 			true	)
			:set_text_visible(false)
		self:add_slider(	{ix+SY,iy,		8-SY,SY},	loname.."_stiffness",		bdd, loname.."_stiffness",		1, 	0,2 )
			:set_text( name )
		iy = iy + SY
		self:add_button( 	{ix+SY,iy,		SY,SY},		loname.."_length_use", 		bdd, loname.."_length_use", 	true	)
			:set_text_visible(false)
		self:add_slider(	{ix+SY*2,iy,	8-SY*2,SY},	loname.."_length_factor",	bdd, loname.."_length_factor",	1,	0,2 )
			:set_text( "Lenght" )
		iy = iy + SY + DY
	end
	add_spring_ui( "Stretch" )
	add_spring_ui( "Bend" )
	add_spring_ui( "Shear" )
	add_spring_ui( "Tether" )

	ix,iy = 1,IY_BEGIN
	ix = ix + SX
	bu = add_xyz(		{ix,iy,		8,SY},	"Wind",		"wind_", 	8,	{-1,0,1 }	)
	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},	"Drag",		bdd, "drag",	.1,	0,1 )
	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},	"Lift",		bdd, "lift",	.1,	0,1 )
	iy = iy + SY + DY

	self:add_selector(	{ix,iy,		8,SY},	"Draw Cloth" )	--notext not save in preset
		:set_nb_min_0( 4 )
		:set_item_text( 2, "Hori", "Vert", "Grid" )
		:set_target_lua( self, "s_draw_cloth" )
		:set_text_draw( false )
	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},	"Pin Border",	bdd, "pin_border_size",	1,	1,32 )
	iy = iy + SY
	self:add_button(	{ix,iy,		SY,SY},	"Triangle",		bdd, "triangle_use", 	true	)

	self:set_tab_key( "Fld" )
	ix,iy = 1,IY_BEGIN
	self:add_button(	{ix,iy,		SY,SY},		"PullBack Active",	self, "b_sha_pullback", 		true	)
		:set_text_visible(false)
	self:add_slider(	{ix+SY,iy,	8-SY,SY},	"PullBack Dist",	self, "sha_pullback_dist",		4,		0,20 )
	iy = iy + SY
	self:add_slider_two({ix,iy,		SX,SY},		"DF Range", 		self, "sha_df_min", "sha_df_max",	0,.25,	-1,2 )
--	self:add_slider(	{ix,iy,		8,SY},		"Field Max Dist",	self, "sha_field_dist_max",	0.05,	0,1 )
	iy = iy + SY
	self:add_button(	{ix,iy,		SY,SY},		"Field  Active",	self, "b_sha_field", 			true	)
		:set_text_visible(false)
	self:add_slider(	{ix+SY,iy,	8-SY,SY},	"Field Str",		self, "sha_field_strenght",		-100, 	-500,500 )
	iy = iy + SY
	self:add_slider(	{ix+SY,iy,	8-SY,SY},	"Field Gamma",		self, "sha_field_gamma",		1,		.1,20 )
	iy = iy + SY + DY
	self:add_slider(	{ix+SY,iy,	8-SY,SY},	"Field Interpolate",self, "sha_field_interpolate",	1,		0,1 )
	
	self:set_tab_key( "Ball" )
	ix,iy = 1,IY_BEGIN
	self:add_slider(	{ix,iy,		8,SY},		"Ball Size", 		self, "ball_size",		4,		1,32	)
	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},		"Bone Size", 		self, "bone_size",		10, 	1,32	)
	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},		"Bone Width", 		self, "bone_width", 	345, 	1,4096	)
	iy = iy + SY
	self:add_slider(	{ix,iy,		8,SY},		"Bone Height", 		self, "bone_height",	144, 	1,4096	)
	iy = iy + SY
	self:add_button(	{ix,iy,		SY,SY},		"Wrap X",			self, "b_sha_wrap_x", 	true	)
	iy = iy + SY
	self:add_button(	{ix,iy,		SY,SY},		"Wrap Y",			self, "b_sha_wrap_y", 	false	)
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},		"Wrap X Min",		self, "sha_wrap_x_min",	-4.7,	-100,100 )
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},		"Wrap X Max",		self, "sha_wrap_x_max",	4.7,	-100,100 )
	iy = iy + SY
    self:add_slider(	{ix,iy,		4,SY},		"Wrap Y Min",		self, "sha_wrap_y_min",	0,		-100,100 )
	iy = iy + SY
	self:add_slider(	{ix,iy,		4,SY},		"Wrap Y Max",		self, "sha_wrap_y_max",	0,		-100,100 )

end

function meu:get_preset_nb() 	return 32	end

function meu:restart()
	param.set( self.ref.restart_trig, true )
end

function meu:init()
	self.layer_attr_id			= 1
	self.layer_reset_accel_id	= 2
	self.layer_filter_img_id	= 3
	self.layer_emit_id			= 4
	self.layer_flex_id			= 5
	self.layer_draw_attrib		= 6
	local layer_shading_ids		= { self.layer_flex_id, 7, 8, 9 }

	local ref = self.ref
	self.layer_shading_cur = self.layer_flex_id
	self:add_shading( layer_shading_ids, nil )

	ref.layer_flex = self:get_layer( self.layer_flex_id  )
	local bdd = self:get_layer_bdd( self.layer_flex_id  )
	ref.bdd				= bdd
	ref.radius			= param.get_ref( bdd, 	"radius" )

	ref.point_nb 		= param.get_ref( bdd,	"particle_nb_max" )
	ref.point_nb_used 	= param.get_ref( bdd,	"particle_nb_used" )
	ref.free_nb 		= param.get_ref( bdd,	"free_indices_counter_out" )

	ref.relaxation_mode = param.get_ref( bdd,	"relaxation_mode" )
	ref.restart_trig	= param.get_ref( bdd,	"restart_trig" )
	ref.bdd_draw		= param.get_ref( bdd,	"draw" )
	ref.bdd_draw_mode	= param.get_ref( bdd,	"draw_mode" )

	ref.use_bdd			= param.get_ref( ref.layer_flex, "use_bdd" )

	self.draw_count = 0
	self.count_first = 0
	--todo this should be added to bdd_flex allocated / used
	--ref.point_nb_used	= param.get_ref( bdd, "point_nb_used" )

	self.phase = 0
end

--todo generalize with MEU:add_particle
function meu:set_point_nb( nb, nb_used )
	local ref = self.ref
	param.set( ref.point_nb,		nb		)
	param.set( ref.point_nb_used,	nb_used	)
end

function meu:update()
	local ref = self.ref
	local ui = self.ui
	local bdd = ref.bdd

	--unclear but need for LVDream
	if self.count_first then
		self.count_first = self.count_first + 1
		if self.count_first > 10 then
			--restrict cases it happens
			--todo merge with draw_countr
			if param.get_str_lower( bdd, "init_mode" ) == "lua" then
				self:flex_build_particles()
			else
				self:restart()
			end
			self.count_first = nil
		end
	end

	self:update_particle_nb()

	if self.time_divider > 0 then
		self.phase = self.phase + aaa.time.dt / self.time_divider
	end

	local t_active = self.ui.bu_collider_active:get_values()
	for i=1,8 do
	 	param.set( bdd, "collider_"..i, t_active[i] )
	end

	param.set(	self:get_layer_ref_table(5).use_bdd,
				self.b_update
					and (self.b_draw and "update_and_draw" or "update" )
					or (self.b_draw and "draw" or "none" )
			)

	for i=5,8 do
		self:bind_texture_to_unit( i )
	end
end

function meu:draw_cloth( s_draw )
	local ref = self.ref
	local bdd = ref.bdd
--	local nb_u = param.get( bdd, "nb_u" )
--	local nb_u = param.get( bdd, "nb_v" )
	GA.__flex_bdd_last = bdd	--hack do it clean later
	--self:print( GA.__flex_bdd_last )
end

function meu:set_up_shader()

	local ref = self.ref
	local sha = self:get_shading()

	local sha_last = self.__sha_last
	if sha ~= sha_last then	--first time or change of shader
		--self:box_debug( "set_up_shader() "..sha )
		self.__sha_last = sha
		--we change save status of params
		sha:set_save_comp_float(	false,	6,	8	)
		sha:set_save_comp_float(	false,	1,	1	)
		sha:set_save_comp_int(		false,	1,	1	)	--todo check to extend
		sha:set_save_comp_float(	false,	5,	5	)
		sha:set_save_comp_float(	false,	10,	14	)

		sha:set_save_vert_float(	false,	1,	1	)
		sha:set_save_vert_int(		false,	1,	1	)

		sha:set_save_frag_float(	false,	1,	2	)
		sha:set_save_frag_int(		false,	1,	2	)
	end
	
	--tried for shy fountain
	local b_restart = param.get_bool( ref.restart_trig )
	if b_restart then
		self:print( "we got a reset here" )
	end
	sha:set_comp_int_4(	b_restart and 1 or 0 )


	local ph = self.phase
	if self.b_time_wrap then
		ph = wrap_01(ph)
	end
	sha:set_comp_float_1( ph )

	local fall = self.fall
	-- integrate velocity comnpute shader
 	if fall.b_active then
		sha:set_comp_float_6_8( fall.age, fall.speed,	1/math.max(fall.ease_time,.0001) )
	else
		sha:set_comp_float_6_8( 32, 0, 1 )
	end
	-- generic value
	sha:set_comp_int_1(		self.sha_comp_int_1 )
	sha:set_comp_float_5(	self.sha_density_y	)

	sha:set_comp_float_10_15(	self.b_sha_pullback and self.sha_pullback_dist or 10000,
								self.sha_df_min, self.sha_df_max,
								self.b_sha_field and self.sha_field_strenght or 0,
								self.sha_field_gamma, self.sha_field_interpolate )		


	-- Baballe stuff
	sha:set_comp_int_2(		self.b_sha_wrap_x and 1 or 0		)
	sha:set_comp_int_3(		self.b_sha_wrap_y and 1 or 0		)

	if self.b_sha_wrap_x then
		sha:set_comp_float_3( self.sha_wrap_x_min )
		sha:set_comp_float_4( self.sha_wrap_x_max )
	end
	--only set for install on site, 5 conflicts with density
	if self.b_sha_wrap_y then
		sha:set_comp_float_5( self.sha_wrap_y_min )
		sha:set_comp_float_6( self.sha_wrap_y_max )
	end

	-- particle shape
	local s =  self.part_size * param.get( ref.radius )
	sha:set_vert_float( 1, s )
	sha:set_frag_int_1(		self.b_sha_tex_use and 1 or 0		)
	sha:set_frag_float_1_2(	self.sha_hardness, 		self.sha_round	)
end

local table_mode = { particle=1, velocity=2, spring=3, transform=4, triangle=5, acceleration=6	}

function meu:draw_flex( what )
	--self:print( "draw_flex "..what )
	local ref = self.ref
	local sha = self:get_shading()

	if what then
		what = string.lower(what)
		local i_mode = table_mode[what]
		-- this where to set shader
		if i_mode then	sha:set_vert_int_1( i_mode )
						--self:print_debug( " is "..i_mode )
		else			self:print_error( what.." is not reconized mode" )
		end
		param.set( ref.bdd_draw_mode, what )
	end

	if self.layer_shading_cur ~= self.layer_flex_id then
		--self:print( "draw layer_shading "..self.layer_shading_cur )
		self:draw_layer( self.layer_shading_cur )
	end
	--todo avoid update each time
	--self:print( "use_bdd is "..param.get_str( ref.use_bdd ) )

	--self:print( "draw layer_flex_id "..self.layer_flex_id )
	self:draw_layer( self.layer_flex_id )
	self.b_can_init_now = true
end

function meu:simul_and_draw_flex()
	local ref = self.ref
	local ui = self.ui

	self.layer_shading_cur = self:get_shading_layer_id()
	param.set( self.ref.use_shading, self.layer_shading_cur == self.layer_flex_id and "owner" or "current" )

	self:set_up_shader()

	local items_on = ui.bu_draw:get_items_text_on()
	--table.print( items_on )

	local nb = #items_on
	local b_draw = nb > 0
	
	if b_draw then
		param.set( ref.use_bdd, self.b_update and "update_and_draw" or "draw" )
		self:draw_flex( items_on[1] )
		if nb >= 2 then
			param.set( ref.use_bdd, "draw" )
			for i=2,nb do
				self:draw_flex( items_on[i] )
			end
		end	
	else
	 	param.set( ref.use_bdd, "update" )
		self:draw_flex()
	end

end

function meu:draw()
	local ref = self.ref
	local bdd = ref.bdd
	self:draw_layers_begin()
		self:draw_layer(	self.layer_attr_id )
		self:draw_layer(	self.layer_reset_accel_id )
		self:draw_layer(	self.layer_filter_img_id )
		self:draw_layer(	self.layer_emit_id )

		self:draw_layer(	self.layer_attr_id )

		--todo 1 we get nothing, 2 we see net but don't update from force field, 3 is ok ????
		if self.draw_count then
			self.draw_count = self.draw_count + 1
			if self.draw_count >= 3 then
				if param.get_str_lower( bdd, "init_mode" ) == "lua" then
					self:flex_build_particles()
				end
				self.draw_count = nil
			end
		end

		self:simul_and_draw_flex()

		-- --self:print( self.s_draw_cloth )
		-- if self.s_draw_cloth ~= 0 then
		-- 	--self:draw_layer( self.layer_draw_attrib )
		self:draw_cloth( self.s_draw_cloth )
		-- else
		-- 	GA.__flex_bdd_last = nil	--hack do it clean later
		-- end
	self:draw_layers_end()
end


function meu:flex_build_particles()
	--avoid messages saying buffers too small
	if not self.b_can_init_now then 
		return
	end

	local ref = self.ref
	local ui = self.ui
	local bdd = ref.bdd

	local stiff = param.get( bdd, "stretch_stiffness" )
	local rest_factor = param.get( bdd, "stretch_length_factor" )
	
	local bend_stiff = param.get( bdd, "bend_stiffness" )
	local bend_rest_factor = param.get( bdd, "bend_length_factor" )

--	local stiff = 1.0
--	local rest_factor = 1.0
	local vx,vy,vz = 0,0,0

	local ux,uy = 0,0

	local rx,ry,rz = 0,0,0
	local box_nb = 5

	local mass_one = 1.0
	local mass_zero = 0.0

	local axis = 1;
	local pin_mode = 1

	local use_spring = true

	local b_self_collide = true
	local b_fluid = false
	local group_index = 0

	--print ref.s_scene_type
	--print(tostring(self.s_init_type)
	local str = ui.bu_init_type:get_item_text()
	if not str then
		return false
	end
	local type = string.lower(str)
	self:print( "Build "..type )
	--print group_index

	local flex = aaa.bdd.flex
	if not flex then return end --avoid error when no flex
	 
	local function set_floor( b )
		--self:box_debug( "Floor to "..b )
		self:set_bu_value( "floor", b )
	end 
	local function set_gravity_and_floor( b )
		set_floor( b )
		self:set_bu_value( "gravity_y", b and -9.81 or 0 )
	end 
	flex.set_default( b_self_collide, b_fluid, group_index )
	flex.create_begin( bdd )
	if type == "net" or type == "bandeau"  then
		local b_stretch = true
		local b_bend = false
		set_gravity_and_floor(false)
		local sx,sy = 9.,2.5
		local v_step = 20
		local u_step = math.floor(v_step*sx/sy)	--nb of interval, there is nb-1 lines, with nb*sub_nb+1 point
		local sub_nb = 2 --min is 1
		self.__flex_net = { u_step=u_step, v_step=v_step, sub_nb=sub_nb, sx=sx, sy=sy, dx=sx/u_step, dy=sy/v_step }

		local nb_u,nb_v = u_step*sub_nb+1,v_step*sub_nb+1
		local function append( x,y,z, m )
			flex.append_particle( bdd , x,y,z,  vx,vy,vz, ux,uy, m )
		end
		local b_frame = type == "net"
		local index = 0
		local margin = 1
		for iv = 1,v_step-1 do
			local y = sy * ( iv / v_step - .5 )
			for iu=1,nb_u do
				local u = (iu-1) / (nb_u-1)
				local x = sx * ( u - .5 )
				append( x,y,0,  (iu<=sub_nb+margin or iu>=nb_u-sub_nb*	margin or ( b_frame and (iv<=margin or iv==v_step-margin))) and 0 or 1 )
				if iu>1 then
					if b_stretch then
						flex.append_spring( bdd, index-1, index, stiff, rest_factor )
					end
					if b_bend and iu>2 then
						flex.append_spring( bdd, index-2, index, bend_stiff, bend_rest_factor )
					end
				end
				index = index + 1
			end
		end
		for iu = 1,u_step-1 do
			local i_prev,i_prev_prev
			local i_cur = index
			local x = sx * ( iu / u_step - .5 )
			local iv_cord = 0
			for iv=1,nb_v do
				local b_end = iv==1 or iv==nb_v
				local b = b_end or (((iv-1) % sub_nb) ~= 0 )
				if b then
					local u = (iv-1) / (nb_v-1)
					local y = sy * ( u - .5 )
					append( x,y,0, ( b_frame and (iu<=margin or iu==u_step-margin or iv<=sub_nb*margin or iv>=nb_v-sub_nb*margin)) and 0 or 1 )
					i_cur = index
					index = index + 1
				else
					i_cur = iv_cord * nb_u + iu * sub_nb
					iv_cord = iv_cord + 1
				end
				if iv>1 then
					if b_stretch then
						flex.append_spring( bdd, i_prev, i_cur, stiff, rest_factor )
					end
					if b_bend and iv>2 then
						flex.append_spring( bdd, i_prev_prev, i_cur, bend_stiff, bend_rest_factor )
					end
				end
				i_prev_prev = i_prev
				i_prev = i_cur
			end
		end
	elseif type == "cloth" then
		flex.append_cloth_grid( bdd, -1.0,2.0,0.0, 3.0,3.0, mass_one, 2, pin_mode, 32, 32, use_spring, 1.0, 1.0, use_spring, 1.0, 1.0, use_spring, 1.0, 1.0 )
	elseif type == "rigids" then
		set_gravity_and_floor(true)
		for i = 1,32 do
			local x = math.sin( i * 1.2 ) * 1
			local y = 3 + i/16*2
			local z = math.cos( i * 2.2 ) * 1
			flex.append_box( bdd, 	x,y,z, rx,ry,rz, vx,vy,vz,
					box_nb,box_nb,box_nb,	0.05, mass_one, true, stiff )
			flex.append_sphere( bdd, -x,y,z, rx,ry,rz, vx,vy,vz,
			 		box_nb,					0.05, mass_one, true, stiff )
		end
	elseif type == "cube maa" then
		set_gravity_and_floor(true)
		--todo why cube 10 have a neg x velocity at start ???
		for i = 10,12 do
			local x = math.sin( i * 1.2 ) * 2
			local y = 3 + i/16*2
			local z = math.cos( i * 2.2 ) * 2
			flex.append_box( bdd, 	x,y,z, rx,ry,rz, vx,vy,vz,
					box_nb,box_nb*i,box_nb,	0.05, mass_one, true, stiff )
			--ok rotation are not done
			--rx = rx + 10
			--ry = ry + 20
		end
	elseif type == "sphere maa" or type == "sphere maa pb" then
		set_gravity_and_floor(true)
		flex.set_default( b_self_collide, false, group_index )
		local b_pb = type == "sphere maa pb"
		local f = b_pb and .5 or .1
		for i = 1,b_pb and 16 or 32 do
			local x = math.sin( i * 1.2 ) * 2
			local y = 3 + i/16*2
			local z = math.cos( i * 2.2 ) * 2
			flex.append_sphere( bdd, -x,y,z, rx,ry,rz, vx,vy,vz,
			 		box_nb*(1+i*f),					0.05, 1, true, stiff )
			--rx = rx + .1
			--ry = ry + .2
		end
	elseif type == "rope maa" then
		set_gravity_and_floor(true)
			local i_off = 0
			local ir = 1
			local t = 3
			--stiff = 1.0
			--rest_factor = .5
			for x = -4,4,.1 do
				--stiff = stiff - .01
				--rest_factor = rest_factor + .01
				--rope
				local nb = 32
				local d = .05
				local z = 0
				flex.append_particle(		bdd , x,t,z,	 vx,vy,vz,	ux,uy,	ir%4~=0 and mass_one or mass_zero )
				for i=1,nb do
					z = z + .05
					flex.append_particle(	bdd , x,t,z, 	vx,vy,vz,	ux,uy,	mass_one )
				end
				for idx = 0,nb-1 do
					flex.append_spring(		bdd, i_off+idx, i_off+idx+1,	stiff, rest_factor )
				end
				x = x + .2
				i_off = i_off + nb + 1
				if ir > 1 then
					flex.append_spring(		bdd, i_off-16, i_off-1-nb-16,	stiff, rest_factor*.5 )
				end
				ir = ir + 1
		end
	elseif type == "rope" then
		set_gravity_and_floor(true)
		local i_off = 0
		local x = 0
		for x = -4,4,.1 do
			--rope
			local nb = 64
			local d = .03
			local z = 0
			local y = 4
			
			for i=1,nb+1 do
				flex.append_particle( bdd , x,y,z,  vx,vy,vz, ux,uy, i==1 and 0 or 1  )
				y = y - 3/nb
				--z = z + .05
			end
			for idx = 0,nb-1 do
				flex.append_spring( bdd, i_off+idx, i_off+idx+1, stiff, rest_factor )
			end
			i_off = i_off + nb + 1
		end
	elseif type == "net 8x4" then
		set_floor(false)
		local i_off = 0
		local sx,sy = 8,4
		local nb_u,nb_v = 129,65
		local function append( x,y,z, m )
			flex.append_particle( bdd , x,y,z,  vx,vy,vz, ux,uy, m )
		end
		local d = .00
		for iy = 1,7 do
			local y = iy * .5 
			local o_ph = math.pi * ( iy + 1)
			for i=1,nb_u do
				local u = (i-1)/(nb_u-1)
				local z = math.cos( o_ph + u * math.pi2  * 8) * d
				local x = sx * u
				append( x,y,z,  (i==1 or i==nb_u) and 0 or 1 )
			end
			for idx = 0,nb_u-2 do
				flex.append_spring( bdd, i_off+idx, i_off+idx+1, stiff, rest_factor )
			end
			i_off = i_off + nb_u
		end
		for ix = 1,15 do
			local x = ix * .5 		
			local o_ph = math.pi * ix
			for i=1,nb_v do
				local u = (i-1)/(nb_v-1)
				local z = math.cos( o_ph + u * math.pi2  * 4) * d
				local y = sy * u
				append( x,y,z,  (i==1 or i==nb_v) and 0 or 1 )
			end
			for idx = 0,nb_v-2 do
				flex.append_spring( bdd, i_off+idx, i_off+idx+1, .5, .125 )
			end
			for i=1,7 do
				flex.append_spring( bdd, i_off+i*8, nb_u*(i-1)+ix*8, -1, 0 )
			end
			i_off = i_off + nb_v
		end
	elseif type == "mixed" then
		set_gravity_and_floor(true)
		flex.append_particle( bdd , 0.0,3.0,0.0,  vx,vy,vz, ux, uy, mass_zero )
		for px=0.5,1.5,0.5 do
			flex.append_particle( bdd , px,3.0,0.0,  vx,vy,vz, ux, uy, mass_one )
		end
	
		for idx =0, 2 do
			flex.append_spring( bdd, idx, idx+1, stiff, rest_factor )
		end
	
		local nb = flex.get_particle_nb( bdd )
	
		flex.append_box( bdd, 2.0,3.0,0.0, rx,ry,rz, vx,vy,vz, box_nb,box_nb,box_nb, 0.05, mass_one, true, stiff )
	
		flex.append_spring( bdd, nb-1, nb, stiff, rest_factor )
	
		for i=1,4 do
			flex.append_sphere( bdd, 2., 3 + i, 0., rx,ry,rz, vx,vy,vz,
							box_nb*(1+i*.5), 0.05, mass_one, true, stiff )
		end
		flex.append_cloth_grid( bdd, 3,1,0, 3,3, mass_one, axis, pin_mode, 32, 32, use_spring,1,1, use_spring,1,1, use_spring,1,1 )
		
		flex.append_cloth_grid( bdd, -1,2,0, 3,3, mass_one, 2,   pin_mode, 32, 32, use_spring,1,1, use_spring,1,1, use_spring,1,1 )
	elseif type == "crash" then
		set_gravity_and_floor(true)
		--start with *21 not less
		flex.append_sphere( bdd, 0, 7, 0., rx,ry,rz, vx,vy,vz,
				box_nb*21, 0.05, mass_one, true, stiff )
		--crash only if next line is in and after not before
		flex.append_cloth_grid( bdd, 3,1,0, 3,3, mass_one, axis, pin_mode, 32, 32,
				use_spring,1,1, use_spring,1,1, use_spring,1,1 )

	elseif type == "baballe" then
		--set_gravity_and_floor(true)
		--start with *21 not less

		local bone_x = 36
		local bone_y = 5
		local circle_res = 31
		local rig_stiff = 1.0
		local layer_z_nb = 3
		local gid = 0
		local rad = 0.04
		local mass = 0.02

		for i=1,8 do
			flex.append_box( bdd, -5.9 + i * 2.,3.0,0.0, rx,ry,rz, vx,vy,vz, bone_x,bone_y,layer_z_nb, rad, mass_one, true, rig_stiff, true, false, gid)
			--gid = gid + 1;
			flex.append_box( bdd, -4.0 + i * 2.,6.0,0.0, rx,ry,rz, vx,vy,vz, bone_x,bone_y,layer_z_nb, rad, mass_one, true, rig_stiff , true, false, gid)
			--gid = gid + 1;
		end
		--flex.append_spring( bdd, nb-1, nb, stiff, rest_factor )

		for i=1,4 do
			flex.append_cylinder( bdd, -3.9 + i * 2., 4.5, 0.0, rx,ry,rz, vx,vy,vz,	circle_res, 3, rad, mass, true, rig_stiff , true, false, gid)
			--gid = gid + 1;

			flex.append_cylinder( bdd, -3.0 + i * 2., 8., 0.0, rx,ry,rz, vx,vy,vz,	circle_res, 3, rad, mass, true, rig_stiff , true, false, gid)
			--gid = gid + 1;
		end
	elseif type == "baballe maa" then
		local bone_x = 36
		local bone_y = 5
		local circle_res = 16
		local rig_stiff = 1.0
		local layer_z_nb =3
		local rad = 0.02
		local mass = mass_one * 100.

		for v=-4,4,.5 do
			--flex.append_box(		bdd, v,3,0,		rx,ry,rz,	vx,vy,vz,	2,32,layer_z_nb,			rad,		mass, true, rig_stiff )
			--gid = gid + 1;
			--flex.append_box(		bdd, v,6,0,		rx,ry,rz,	vx,vy,vz,	4,128,layer_z_nb,			rad,		mass, true, rig_stiff )
			--gid = gid + 1;
		end
		--flex.append_spring( bdd, nb-1, nb, stiff, rest_factor )

		--for v=-4,4,.8 do
			flex.append_cylinder(	bdd, 3,4.5,0,	rx,ry,rz,	vx,vy,vz,	circle_res*2,layer_z_nb,	rad,		mass, true, rig_stiff )
			flex.append_cylinder(	bdd, 1,4.5,0,	rx,ry,rz,	vx,vy,vz,	circle_res*2,layer_z_nb,	rad,		mass, true, rig_stiff )
			--gid = gid + 1;
			--flex.append_cylinder(	bdd, v,2,0,		rx,ry,rz,	vx,vy,vz,	circle_res, layer_z_nb,		rad,		mass, true, rig_stiff )
			--gid = gid + 1;
		--end	
	elseif type == "baballe maa bis" then
		local bone_x = 36
		local bone_y = 5
		local circle_res = 8
		local rig_stiff = 1.0
		local layer_z_nb = 3
		local rad = 0.15
		local mass = mass_one * 100.
		local baballe_id = 0
		local nonos1_id = 1
		local nonos2_id = 2

		self:set_bu_value( "radius", rad )
		--param.set( bdd, rad )

		for v=-5,5,3 do
			flex.append_box(		bdd, v,22,0,	rx,ry,rz,	vx,vy,vz,	16,4,layer_z_nb,			rad,		mass, true, rig_stiff, true, false, 0, nonos1_id )
			flex.append_box(		bdd, v,20,0,	rx,ry,rz,	vx,vy,vz,	16,4,layer_z_nb,			rad,		mass, true, rig_stiff, true, false, 0, nonos1_id )
			flex.append_box(		bdd, v,18,0,	rx,ry,rz,	vx,vy,vz,	16,4,layer_z_nb,			rad,		mass, true, rig_stiff, true, false, 0, nonos1_id )
			flex.append_box(		bdd, v,15,0,	rx,ry,rz,	vx,vy,vz,	16,4,layer_z_nb,			rad,		mass, true, rig_stiff, true, false, 0, nonos1_id )
			--gid = gid + 1;
			flex.append_box(		bdd, v,8,0,		rx,ry,rz,	vx,vy,vz,	16,4,layer_z_nb,			rad,		mass, true, rig_stiff, true, false, 0, nonos2_id )
			flex.append_box(		bdd, v,7,0,		rx,ry,rz,	vx,vy,vz,	16,4,layer_z_nb,			rad,		mass, true, rig_stiff, true, false, 0, nonos2_id )
			--gid = gid + 1;
		end
		--flex.append_spring( bdd, nb-1, nb, stiff, rest_factor )

		for v=-9,4,3 do
			flex.append_cylinder(	bdd, v,10,0,	rx,ry,rz,	vx,vy,vz,	circle_res*2,layer_z_nb,	rad,		mass, true, rig_stiff, true, false, 0, baballe_id )
			flex.append_cylinder(	bdd, v+2,5,0,	rx,ry,rz,	vx,vy,vz,	circle_res*2,layer_z_nb,	rad,		mass, true, rig_stiff, true, false, 0, baballe_id )
			flex.append_cylinder(	bdd, v+2,5,0,	rx,ry,rz,	vx,vy,vz,	circle_res*2,layer_z_nb,	rad,		mass, true, rig_stiff, true, false, 0, baballe_id )
			flex.append_cylinder(	bdd, v,12,0,	rx,ry,rz,	vx,vy,vz,	circle_res*2,layer_z_nb,	rad,		mass, true, rig_stiff, true, false, 0, baballe_id )
			--gid = gid + 1;
			--flex.append_cylinder(	bdd, v,2,0,		rx,ry,rz,	vx,vy,vz,	circle_res, layer_z_nb,		rad,		mass, true, rig_stiff )
			--gid = gid + 1;
		end	
	elseif type == "baballe mini" then
		local bone_x = 36
		local bone_y = 5
		
		local rig_stiff = 1.0
		local layer_z_nb = 1
		local f = .5
		local rad = 0.01 * f
		local mass = mass_one * 100.
		local baballe_id = 0
		local nonos1_id = 1
		local nonos2_id = 2
		local function get_bind( index )	return 53 + index	end
		local nonos_spacing = 16

		--size of the bone in pixel
		local nonos_width = self.bone_width
		local nonos_height = self.bone_height

		local nonos_ratio = nonos_width / nonos_height

		local function get_dim_xy()
			local dim_y = self.bone_size / f * (.6 + math.random() * .4 )
			local dim_x = dim_y * nonos_ratio
			return dim_x,dim_y
		end
		local function get_y()
			local y = math.random() * .4
			return y
		end
		local function get_dim_cercle()
			local circle_res = 6 + math.random() * 2 
			local dim = circle_res * 2 / f
			return dim
		end

		self:set_bu_value( "radius", rad+0.01 )
   
		local step = .5 --relate to nb of rigid here
		local dim_x,dim_y
		local function add_nonos( id, x,y,z )
			local bind = get_bind( id )
			dim_x,dim_y = get_dim_xy()
			local mask = {}
			local b_data = false
			for u=1,dim_x do
				for v=1, dim_y do
					local what = 4
					local c = aaa.img.get_component_uv(	bind, (u-1)/dim_x,(v-1)/dim_y,	what, true, false )
					if c~=0 then
						b_data = true
					end
					--c = 1
					table.insert( mask, c )
				end
			end
			if not b_data then
				self:print( "we got no data from image (probably nothing on cpu)" )
			end
			flex.append_box(		bdd, x,y + get_y(),z,	rx,ry,rz,	vx,vy,vz,	dim_x,dim_y,layer_z_nb,		rad,		mass, true, rig_stiff , true, false, 0, id, mask )
		end

		local function add_object( v, y )
			local r = math.random()
			if r > 0.75 then
				--self:print( "Nonos 1" )
				add_nonos( nonos1_id,  v,y,0 )
			elseif r > 0.5 then
				--self:print( "Nonos 2" )
				add_nonos( nonos2_id,  v,y,0 )
			else
				--self:print( "Cylinder" )
				local dim = get_dim_cercle()
				flex.append_cylinder(	bdd, v,y+get_y(),0,	rx,ry,rz,	vx,vy,vz,	dim,layer_z_nb,				rad,		mass, true, rig_stiff, true, false, 0, baballe_id )
			end
		end

		for v=-2,2,step do
			add_object( v, 10 )
			add_object( v, 8 )
			add_object( v, 7 )
		end

		local dim
		for v=-3.0,3.0,step do
			add_object( v,   3.5 )
			add_object( v+2, 5   )
			add_object( v,   14  )
		end	
	end	
	flex.create_end( bdd )
	
	--uncomment to auto restart
	--param.set( self.ref.restart_trig, true )
end