function meu:define_meu_infos()
	return { author = "mrvux Mâa",
			tags = { "2d", "texture", "renderpass", "draw", "unfinished" },
			help = 	{
					"Use to add Caustic",
					"Done for Monaco Aquarium but unused, need to be finished and documented or trashed",
					"Output to the Fbo Caustic"
					}
		}
end

function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SX = 8
	local SY = .9
	local SYC = SY * 1.5
	local DY = .2

	self:add_camera()

	ix,iy = self:define_ui_ndc( nil, true )

	local i = 1;
	local bu = self:add_selector(	{ix,iy,	8,SY}, "Mode" )
		bu:set_nb( 5 )
		bu:set_item_text( 1, "No", "Debug", "Debug 2", "Add", "Sub" )
		bu:set_target_param( sha:get_ref_frag_int(1) )
	iy = iy  + SY

	bu = self:add_rgbf(			{ix,iy,		5,SYC}, "Color", false )
		ui.bu_rgb = bu
	iy = iy + SYC

	bu = self:add_slider(		{ix,iy,		8,SY}, "intensity",		self, "intensity", 0, 0, 4 ):add_values_def( .5 )
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,		4,SY}, "scale", 		self, "scale", 1, 0, 2 )
	bu = self:add_slider(		{ix+4,iy,	4,SY}, "ratio", 		self, "ratio", 1, 0, 2 )
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,		8,SY}, "iteration nb",   sha:get_ref_frag_int(2), nil, 5, 1, 32 )
		bu:set_value_type_integer( true )
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,		8,SY}, "normal factor", sha:get_ref_frag_float(7), nil, .5, 0, 1 )

	iy = iy + SY
	self:add_button(			{ix,iy},			"Speed active"	):set_value(false):set_text_visible( false ):set_target_lua( self, "b_speed" )
	self:add_slider(			{ix+1,iy,	7,SY},  "Speed"			):set_min_max( -1, 1 ):set_target_lua( self, "speed" )

	iy = iy + SY
	bu = self:add_sliders_xyz(	{ix,iy,		SX,SY},	"Direction",	self.dir, 	false,	1 )

	self:define_ndc_src(		{9,2.5,		8,12 },	{"Tex",	"Depth", "Normal"} )
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "caustic" )
	local sha = self:add_shading()
	sha:set_save_frag_float(	false,	1,8 )
	sha:set_save_frag_int(		false,	1,2 )
	sha:set_save_frag_vec4(		false,	1,1 )

	self.dir = { x=0, y=0, z=0 }
	self.dir_v3 = V3.new()	--void creating tab every frame
end


function meu:draw()
	local ui = self.ui
	self:do_fbo()

	local sha = self:get_shading()
	local dir = self.dir
	local dir_v3 = self.dir_v3
	self:draw_layers_begin()
 		self:draw_layer( 1 )

		V3.set( dir_v3, dir.x, dir.y, dir.z )
		V3.normalize( dir_v3 )
		sha:set_frag_vec4( 1, dir_v3[1], dir_v3[2], dir_v3[3], 1 )
		--sha:set_frag_vec4( 1, dir.x, dir.y, dir.z, 1 )
		local t = self.time or 0
		if self.b_speed then t = t + aaa.time.dt * self.speed end
		self.time = t
		sha:set_frag_float( 8, t )

		local r,g,b = ui.bu_rgb:get_rgb()
		sha:set_frag_float_1_3( r,g,b )
		sha:set_frag_float_4( 1. / ( self.intensity * .005 ) )
		sha:set_frag_float_5_6( self.scale, self.scale*self.ratio )

		self:draw_layer( 2 )
 	self:draw_layers_end()
end




