
function meu:define_meu_infos()
	return { author = "mrvux, Mâa",
			tags = { "2d", "texture", "renderpass", "imageprocessing", "draw", "unfinished" },
			help = {
					"Use to add Fog based on depth, height, sun position and color",
					"  Done for Monaco Aquarium but unused, Meu FogMona was preferred",
					"  Need to be documented and extended using FogMona eventually",
					"Output to the Fbo Fog"
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
	local SY = .9
	local SYC = SY * 1.5
	local SX = 8
	local DY = .2

	self:add_camera()
	
	ix,iy = self:define_ui_ndc()

	local iy_beg = iy
	bu = self:add_slider(	{ix,iy,		4,SY}, "start", 			sha:get_ref_frag_float(1), nil, .5, 0,100 )
	bu = self:add_slider(	{ix+4,iy,	4,SY}, "Depth/Distance",	sha:get_ref_frag_float(9), nil, 0,	0,1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		2,SYC},	"Top Density",		self, "top_density",	1, 0,5 )
	bu = self:add_rgbf(		{ix+2,iy,	6,SYC}, "Top Color",		false )
		self.bu_col_top = bu
	iy = iy + SYC
	bu = self:add_slider(	{ix,iy,		2,SYC},	"Bottom Density", 	self, "bottom_density", 1, 0,5 )
	bu = self:add_rgbf(		{ix+2,iy,	6,SYC}, "Bottom Color", 	false )
		self.bu_col_bottom  = bu
	iy = iy + SYC + DY

	bu = self:add_slider(	{ix,iy,		4,SY},	"y top",			sha:get_ref_frag_float(10), nil, 5, 0,20 ):set_color_back("y")
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,		4,SY},	"y bottom",			sha:get_ref_frag_float(11), nil, 1, 0,20 ):set_color_back("y")
	bu = self:add_slider(	{ix+4,iy,	4,SY},	"y gamma",			sha:get_ref_frag_float(12), nil, 1, .1,10 )
	iy = iy + SY

	ix,iy = 9,2.5
	bu = self:add_slider(	{ix,iy,		4,SY},	"Sun Amount",		sha:get_ref_frag_float(8),	nil, 0,	0,1 )
	iy = iy + SY
	bu = self:add_rgbf(		{ix,iy,		SX,SYC}, "Sun Color", false )
		self.bu_sun_col = bu
	iy = iy + SYC
	bu = self:add_sliders_xyz(	{ix,iy,	SX,SY},	"Sun Dir", self.sun_dir, false,	2.5	)
	iy = iy + SY + DY

	--todo add density switch in shader
--	self:define_ndc_src(	{9,iy,		8,8.5},	{ "Tex", "Depth", "Density" } )
	self:define_ndc_src(	{9,iy,		8,8.5}, { "Tex", "Depth" } )
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "fog" )
	self:add_shading()
	self.sun_dir = { x=0, y=0, z=0 }
end


function meu:draw()
	self:do_fbo()
	local sha = self:get_shading()
--  	self:draw_layers_begin()
 		self:draw_layer( 1 )
		sha:set_vert_int_1( self.b_draw and 0 or 1 )
		self:draw_layer( 2 )
 	self:draw_layers_end()
end


function meu:update()
	local sha = self:get_shading()
	local r,g,b
	r,g,b = self.bu_col_bottom:get_rgb()
	sha:set_frag_vec4( 1, r,g,b, self.bottom_density )
	r,g,b = self.bu_col_top:get_rgb()
	sha:set_frag_vec4( 2, r,g,b, self.top_density )

	r,g,b = self.bu_sun_col:get_rgb()
	sha:set_frag_float_2_4( r,g,b )

	local dir = self.sun_dir
	sha:set_frag_float_5_7( dir.x, -dir.y, dir.z )
end

function meu:get_preset_nb()	return 32	end

