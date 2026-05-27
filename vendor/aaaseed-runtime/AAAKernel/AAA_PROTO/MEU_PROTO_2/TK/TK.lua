function meu:define_meu_infos( )
	return { 	author = "Mâa",
				tags = { "2d", "3d", "Vj", "Core", "CoreGraphic", "Draw", "procedural", "Sound", "Geometry", "Point", "Input" },
				help =	{	"This MEU is inspired by the Transcam electronic device created by Christian Amoretti",
							"  it generate visuals using directly the audio signal shape (or eventually its Fourrier Transform).",
							"Quite rich and complex generator now,",
							"  It started as digital version of one input of Christian Amoretti's analog TransCam,",
							"  and evolve during countless Mixing session with Christian an DJ Mtee.",
							"As it create geometry (in particular points) it is quite nice to use the MEU Voronoi just after.",
							"FFT option Unusable for Now (2025 Nov Mâa)"
						}
			}
end

function meu:get_preset_nb()	return 16	end
function meu:draw_icon()
	gol.set_line_width( 3 )
	local x = -.5
	local dx = .045
	gol.draw_line_strip_2d(	x, 			0,
							x+dx,		.2,
							x+dx*2, 	-.2,
							x+dx*3, 	.1,
							x+dx*4, 	0.,
							x+dx*5, 	.1,
							x+dx*6, 	0.
						)
end

--todo
-- draw plain button
-- edge before / after : should split in one more layer
-- draw Plain line point ?
-- draw line with polygon
-- disymetrie deformer bottom top
-- normalisation position

-- 2023 July at LaBigue Mâa changed it
--	so clamp_zero = ON TraxLoc05Out ;
--	and abs = ON TraxLoc07Out ;
--	can plug anymore and have to be replace (abs_clamp param) or abandonned

function meu:update_ui()
	local ui = self.ui
	local ref = self.ref
	local pgb = param.get_bool

	ui.bu_nb_u					:interpolate_alpha_bu(	self.b_nb_u_define )
	ui.bu_softness_range		:interpolate_alpha_bu(	self.b_softness )

	local a = ui.bu_sv_edge		:interpolate_alpha_bu(	self.b_edge )
	ui.bu_color_edge			:set_alpha_bu(a)

	ui.bu_color_particle		:interpolate_alpha_bu(	self.b_particle )

	ui.bu_th					:interpolate_alpha_bu( 	pgb( ref.abs_clamp ) )

	a = ui.bu_data_interval		:interpolate_alpha_bu(	pgb( ref.def_data_sound_process	) )
	ui.bu_data_strobe			:set_alpha_bu(a)

	ui.bu_data_filter_value		:interpolate_alpha_bu(	pgb( ref.def_data_filter ) )

	a = ui.bu_cylinder_active	:interpolate_alpha_bu(	pgb( ref.def_geo_active ) )
	ui.bu_cylinder_turn			:set_alpha_bu(a)
	ui.bu_cylinder_amount		:set_alpha_bu(a)
	ui.bu_cylinder_cone_angle	:set_alpha_bu(a)

	a = ui.bu_side_amount		:interpolate_alpha_bu(	pgb( ref.def_geo_side ) )
	ui.bu_side_number			:set_alpha_bu(a)

	ui.bu_spiral_factor			:interpolate_alpha_bu(	pgb( ref.def_geo_spiral ) )
end
	

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local par
	local ix, iy
	local SY = .88
	local SYC = SY * 1.2
	local DY = .2

	local sha = self:get_shading( 1 )

	ix,iy = 1,1

	self:add_shading_ui(		{ix,iy,			8,SY}		)

	bu = self:add_button( 		{ix+5,iy,		3,SY},		"FFT", 			ref.def_data, "data_src" ):set_min_max(3,2)
	iy = iy + SY + DY

	local SX = 3
	bu = self:add_slider(		{ix,iy,			SX,SY}, 	"Size",			self, "size",				8,		0,8 )
		:set_color_back( "u" )
		:add_values_def( 1,2,3,4,5,6,7 )		
	bu = self:add_slider(		{ix+SX,iy,	 	8-SX,SY},	"Sample Nb",	self, "sample_nb",	512,	1,64*1024 )
		:set_value_type_integer( true )
		:set_color_back( "u" )
	iy = iy + SY
	bu = self:add_button(		{ix+3,iy,		SY,SY}, 	"Nb U define",	self, "b_nb_u_define", 			false )	--todo bug at +1
		:set_text_visible( false )
	bu = self:add_slider(		{ix+3+SY,iy,	5-SY,SY}, 	"Nb U",			self, "nb_u", 			2,		2,512 )	--todo bug at +1
		:set_value_type_integer( true )
		:set_color_back( "u" )
		ui.bu_nb_u = bu
	iy = iy + SY + DY

	SX = 2.5
	bu = self:add_button(		{ix,iy,			SY,SY}, 	"Draw.Solid",		self, "b_draw_solid",		true )
		:set_text_draw( false )
	bu = self:add_button(		{ix+4-SX,iy,	SX,SY},		"Softness",			self, "b_softness",			false )
		--:set_text_visible(false)
	bu = self:add_slider_two(	{ix+4,iy,		4,SY},		"Softness Range",	self, "softness_min", "softness_max",	.1,.9,	0,1 )
		:set_text( "Range" )
		:set_min_max_strict( true )
		:add_values_def( .1, .2, .4, .5, .6, .8, .9 )
		ui.bu_softness_range = bu

	iy = iy + SY
	self:add_bu_texture_target_unit( 	{ix,iy, 		4,SY*4}		)
	self:add_mapping_by_side(	{ix+4,iy+SY*.5,	4,SY*2}		)
	iy = iy + SY*4
	bu = self:add_rgbfa(		{ix,iy,			8,SYC},		"Color", 			false )
		ui.bu_color = bu
	iy = iy + SYC + DY

	bu = self:add_button(		{ix,iy,			SY,SY}, 	"Edge On",		self,"b_edge",		false )
		:set_text_draw( false )
	ref.render_edge = self:get_obj_down( "render_edge" )
		bu = self:add_slider(	{ix+SY,iy,		5-SY,SY}, 	"Edge Ep",		ref.render_edge, "line_size",	1,	0,10 )
		self.ui.bu_sv_edge = bu
	bu = self:add_slider(		{ix+5,iy,		3,SY}, 		"Nb V",			ref.grid, "nb_v", 		2,		2,10 )
		:set_color_back( "v" )

	iy = iy + SY
	bu = self:add_rgbfa(		{ix,iy,			8,SYC}, 	"Edge ", 		 false )
		ui.bu_color_edge = bu
	iy = iy + SYC + DY

	bu = self:add_button(		{ix,iy,			SY,SY}, 	"Particle",		self, "b_particle",		false )
		--bu:set_text_draw( false )
	iy = iy + SY
	bu = self:add_rgbfa(		{ix,iy,			8,SYC}, 	"Particle Color", 	false )
		ui.bu_color_particle = bu
	iy = iy + SYC + DY

	self:add_rendering( 		{ix,16-SY*2-DY,	8,SY*2}		)
--	iy = iy + SY*2 + DY

--todo	self:add_size_uvf_video()

	ix,iy = 9,1

	self:add_camera(		{ix,iy} )
	iy = iy + SY + DY

	bu = self:add_slider(	{ix,iy,		5,SY}, 	"Gain",				self, 		"strenght", 	1,	0,10 )
	ui.bu_gain = bu
	bu = self:add_slider(	{ix+5,iy,	3,SY},	"offset",			ref.def_data, "offset",		0,	-2, 2 )
	iy = iy + SY
	bu = self:add_selector(	{ix,iy,		5,SY},	"abs_clamp" )	--notext not save in preset
		bu:set_nb_min_0( 3 )
		bu:set_item_text( 2, "ABS", "Clamp" )
		bu:set_draw_by_value( 0, "false" )
		bu:set_target_param( ref.abs_clamp )
		bu:set_text_draw( false )
	bu = self:add_slider(	{ix+5,iy,	3,SY},	"abs_clamp_value",	ref.def_data, "abs_clamp_value",	0	,-1,1 ):set_text( "Th" )
		ui.bu_th = bu
	iy = iy + SY
	bu = self:add_button(	{ix,iy,	 	2,SY},	"Filter",			ref.def_data, "data_filter"			)
	bu = self:add_slider(	{ix+2,iy,	6,SY},	"Filter Value",		ref.def_data, "data_filter_value",	.9,	.9,.9999 )
		ui.bu_data_filter_value = bu
	iy = iy + SY + DY
	
	bu = self:add_selector(	{ix,iy,		8,SY},	"sound_process" )	--notext not save in preset
		bu:set_nb_min_0( 5 )
		bu:set_item_text( 1, "No", "Average", "Max", "Decay", "Inter" )
		bu:set_draw_by_value( 0, "false" )
		par = param.get_ref( ref.def_data, "sound_process" )
		bu:set_target_param( par )
		bu:set_text_draw( false )
	iy = iy + SY

	bu = self:add_slider(	{ix,iy,	 	4,SY},		"Interval",		ref.def_data, "interval",	16,	0,32 )
		ui.bu_data_interval = bu
	bu = self:add_slider(	{ix+4,iy,	4,SY},		"Strobe",		ref.def_data, "Strobe",		0,	0,32 )
		ui.bu_data_strobe = bu
	iy = iy + SY + DY

	bu = self:add_button(	{ix,iy,		SX,SY},		"Sym",			ref.def_data, "abs",		false )
--	bu = self:add_button(	{ix+3,iy,	3,SY},		"Clamp",		ref.def_data, "clamp_zero",	false )

	bu = self:add_slider(	{ix+SX,iy,	6-SX,SY}, 	"Thickness",	self,	  	"size_v",		.1,	0,1 )
		:set_color_back( "v" )
	bu = self:add_button(	{ix+6,iy,	 2,SY},		"Force 0",		ref.def_data, "force_zero" )
	self.ui.bu_sv = bu
	iy = iy + SY + DY

	bu = self:add_button(	{ix,iy,		2,SY},		"Cylinder",		ref.def_geo, "active",		false ):set_text( "O" )
	bu = self:add_slider(	{ix+2,iy,	3,SY},		"Radius",		ref.def_geo, "radius",		1,	-4,4 )
		ui.bu_cylinder_active = bu
	bu = self:add_slider(	{ix+5,iy,	3,SY},		"Turn",			self, 	"turn",				1,	0,16 )
		ui.bu_cylinder_turn = bu
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		2,SY},		"Inter",		ref.def_geo, "amount",		1,	0,1 )
		ui.bu_cylinder_amount = bu
	bu = self:add_slider(	{ix+2,iy,	3,SY},		"Angle",		ref.def_geo, "cone_angle",	0,	0,1 )
		:add_values_def( .25, .75 )
		ui.bu_cylinder_cone_angle = bu
	iy = iy + SY + DY

	bu = self:add_button(	{ix,iy,		2,SY},		"Side",			ref.def_geo, "side",			false )
	bu = self:add_slider(	{ix+2,iy,	3,SY},		"Side amount",	ref.def_geo, "side_amount",		1,	-1,2 ):set_text("Amount")
		ui.bu_side_amount = bu
	bu = self:add_slider(	{ix+5,iy,	3,SY},		"Side NB",		ref.def_geo, "side_nb",			3,	2,32 )
		ui.bu_side_number = bu
	iy = iy + SY + DY

	bu = self:add_button(	{ix,iy,		2,SY},		"Spiral",			ref.def_geo, "spiral",			false )
	bu = self:add_slider(	{ix+5,iy,	3,SY},		"Spiral Factor",	self, "spiral_factor",	0,	-4,4 ):set_text("Factor")
		ui.bu_spiral_factor = bu
	iy = iy + SY + DY


	ref.tr1 = self:get_obj_down( "tr1" )
	bu = self:add_slider(	{ix,iy,		4,SY},		"X",			ref.tr1, "translate_x", 	0,	-3, 3 ):set_color_back("x")
	bu = self:add_slider(	{ix+4,iy,	4,SY},		"Y",			ref.tr1, "translate_y", 	0,	-2, 2 ):set_color_back("y")
	iy = iy + SY
	local SX = 8/3
	bu = self:add_slider(	{ix,iy,		SX,SY},		"RotX",			ref.tr1, "rotate_x",		0,	-1, 1 ):set_color_back("x")
	bu = self:add_slider(	{ix+SX,iy,	SX,SY},		"RotY",			ref.tr1, "rotate_y",		0,	-1, 1 ):set_color_back("y")
	bu = self:add_slider(	{ix+SX*2,iy,SX,SY},		"RotZ",			ref.tr1, "rotate_z",		0,	-1, 1 ):set_color_back("Z")
	iy = iy + SY + DY

end

function meu:init()
	local ref = self.ref
	local pgr = param.get_ref 

	ref.use_bdd_solid = self:get_layer_ref_table(1).use_bdd

	param.set( self:get_obj_down("layer_edge"), "active", true )

	ref.grid = self:get_obj_down( "grid" )
		ref.grid_nb_u = pgr( ref.grid, "nb_u" )
		ref.grid_draw_mode = pgr( ref.grid, "draw_mode" )

	ref.def_data = self:get_obj_down( "def" )
		ref.def_strenght			= pgr( ref.def_data, "strenght" )
		ref.def_sample_nb			= pgr( ref.def_data, "sample_nb" )
		ref.abs_clamp				= pgr( ref.def_data, "abs_clamp" )
		ref.def_data_filter 		= pgr( ref.def_data, "data_filter")
		ref.def_data_sound_process	= pgr( ref.def_data, "sound_process" )
				
	ref.def_geo = self:get_obj_down_no_error( "def_cyl" )
	if ref.def_geo then 
		ref.def_geo_u_factor		= pgr( ref.def_geo, "u_factor" )
		ref.def_geo_spiral_factor	= pgr( ref.def_geo, "spiral_factor" )
		ref.def_geo_active			= pgr( ref.def_geo, "active")
		ref.def_geo_side   			= pgr( ref.def_geo, "side" )
		ref.def_geo_spiral 			= pgr( ref.def_geo, "spiral" )
	end

	ref.model_size_u = pgr( ref.model, "size_u" )
	ref.model_size_v = pgr( ref.model, "size_v" )
	local sha = self:add_shading()

	ref.bdd_part = self:get_layer_bdd(3)
		ref.emission_box_size_v = pgr( ref.bdd_part, "emission_box_size_v" )
		ref.restart_trig		= pgr( ref.bdd_part, "restart_trig" )

	sha:set_save_frag_float( false, 1,2 )
--	sha:set_save_frag_int( false, 3,3 )
end


function meu:update()
	local ref = self.ref
	local ps = param.set  
	local sha = self:get_shading()
	local min, max
	if self.b_softness then		min, max = self.softness_min, self.softness_max
	else						min, max = 0,1
	end
	sha:set_frag_float_1_2( min,max )

	local s = self.strenght
	s = (s>=0 and s or -s) * s 
	--local s = self.strenght
	ps( ref.def_strenght, s )
	ps( ref.def_sample_nb, self.sample_nb )
	ps( ref.grid_nb_u, self.b_nb_u_define and self. nb_u or self.sample_nb )
	
	s = self.size
	ps( ref.model_size_u, s )
	ps( ref.def_geo_u_factor, self.turn / s )	--todo take from model if we add a scale one day
	ps( ref.def_geo_spiral_factor, self.spiral_factor / self.turn )

	s = self.size_v
	ps( ref.model_size_v, s )
	ps( ref.emission_box_size_v, s*.5 )
end

function meu:draw()
	local ui = self.ui
	local ref = self.ref
	local ps = param.set
	
	local ma = self:get_alpha() 
 	self:draw_layers_begin()
		local r,g,b, a

		if self.b_draw_solid then
			r,g,b, a = ui.bu_color:get_rgba()	
			gol.color( r,g,b, a*ma )
			ps( ref.grid_draw_mode, "regular" )
			ps( ref.use_bdd_solid, 2 ) 
		else
			ps( ref.use_bdd_solid, 3 ) 
		end
		self:draw_layer( 1 )

		if self.b_edge then
			r,g,b, a = ui.bu_color_edge:get_rgba()	
			gol.color( r,g,b, a*ma )
			ps( ref.grid_draw_mode, "line_u" )
			self:draw_layer( 2 )
		end

		if self.b_particle then
			r,g,b, a = ui.bu_color_particle:get_rgba()	
			gol.color( r,g,b, a*ma )
			self:draw_layer( 3 )
			--todo still a glitch but don't function before too, refine also by adding particle params
			if not self.b_particle_last then
				ps( ref.restart_trig, true )
			end
		end
		self.b_particle_last = self.b_particle
		
	self:draw_layers_end()
end