
function meu:define_meu_infos()
	return { author = "mrvux Mâa",
			tags = { "2d", "texture", "renderpass", "draw", "imageprocessing", "unfinished" },
			help = 	{
					"Use to add Fog based on depth, height, sun position and color",
					"Used for Monaco Aquarium",
					"Some option could be move to other fog MEUs (debug mode ?)",
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

	ix,iy = self:define_ui_ndc( nil, true )
	bu = self:add_selector(	{ix,iy,	8,SY},		"VDebug" )
		bu:set_nb_min_0( 4 )
		bu:set_target_param( sha:get_ref_frag_int(1) )
		bu:set_item_text(	1,	"No", "", "Debug", "+Tex" )
	--bu = self:add_button(	{ix+4,	iy-SY,	SY, SY },		"VDebug", 			sha:get_ref_frag_int(1), nil,	false )
	iy = iy + SY + DY

	local iy_beg = iy

	bu = self:add_slider_two(	{ix,iy,		SX,SY},	"Distance", 	self,	"dist_beg", "dist_end",		1, 5, 0, 32 )
	iy = iy + SY
	bu = self:add_slider(		{ix,iy,		4,SY},	"Density",		self,	"gamma", 1, 0, 1 )
	iy = iy + SY
	bu = self:add_sliders_xyz(	{ix,iy,		SX,SY},	"OBegin",		self.dist_o_beg, 	false,	5	)
	iy = iy + SY
	bu = self:add_sliders_xyz(	{ix,iy,		SX,SY},	"OEnd",			self.dist_o_end, 	false,	5	)
	iy = iy + SY + DY

--	iy = iy + SY
	bu = self:add_slider(		{ix,iy,		2,SYC},	"Horizon",		self, "y_center", 5, -10, 10 ):set_color_back("y")
	bu = self:add_rgbf(			{ix+2,iy,	6,SYC}, "Top_Color",	false )
		self.bu_col_top = bu
	iy = iy + SYC
	bu = self:add_slider(		{ix,iy,		2,SYC},	"Range", 		self, "y_range", 5, 0, 10 ):set_color_back("y")
	bu = self:add_rgbf(			{ix+2,iy,	6,SYC}, "Bottom_Color", false )
		self.bu_col_bottom  = bu
	iy = iy + SYC


	bu = self:add_slider(		{ix,iy,		4,SYC},	"Horizon_Gamma",		sha:get_ref_frag_float(12), nil, 1, 0.2, 5 ):set_color_back("y")

	iy = iy + SY

	ix,iy = 9,3
	bu = self:add_slider(		{ix,iy,		4,SY}, "Sun", sha:get_ref_frag_float(8), nil, 0, 0, 1 )
	iy = iy + SY

--	bu = self:add_sliders_xyz(	{ix,iy,		SX,SY},	"Sun",	self.sun_pos, 	false,	20	)
	bu = self:add_slider(		{ix,iy,				6,SY},		"Sun_LR",	self, "sun_lr",		0,	-180, 180	)
	bu = self:add_slider(		{ix+6,iy-SY*1.5,	2,SY*2.5},	"Sun_Azi",	self, "sun_azi",	0,	0, 90	)
	iy = iy + SY

	self.bu_sun_col = self:add_rgbf(	{ix,iy,	SX,SYC}, "Sun_Color", false )
	iy = iy + SYC

	bu = self:add_slider_two(	{ix,iy,		SX,SY},	"Sun_Range", 	self,	"sun_beg", "sun_end",		0, 1, 0, 1 )

	iy = iy + SY + DY
	self:define_ndc_src(		{9,iy,		8,7 },	{"Tex",	"Depth"} )
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "fog" )
	local sha = self:add_shading()
	sha:set_save_frag_float(	false,	6, 12 )
	sha:set_save_frag_vec4(		false,	1, 6 )
	self.dist_o_beg	= { x=0, y=0, z=0 }
	self.dist_o_end	= { x=0, y=0, z=0 }
	self.sun_pos	= { x=0, y=0, z=0 }
end

function meu:draw()
	self:do_fbo()
	local sha = self:get_shading()
	self:draw_layers_begin()
 		self:draw_layer( 1 )
		self:draw_layer( 2 )
 	self:draw_layers_end()
end

function meu:update()
	local sha = self:get_shading()

	local v = self.dist_o_beg
	local d = self.dist_beg
	local xb,yb,zb = d+v.x, d+v.y, d+v.z
	sha:set_frag_vec4( 1, xb, yb, zb )
	v = self.dist_o_end
	d = self.dist_end
	local xe,ye,ze = d+v.x, d+v.y, d+v.z
	sha:set_frag_vec4( 2, 1./(xe-xb), 1./(ye-yb), 1./(ze-zb) )

	sha:set_frag_float( 10, self.y_center + self.y_range * .5 )
	sha:set_frag_float( 11, self.y_center - self.y_range * .5 )

	local r,g,b
	r,g,b = self.bu_col_top:get_rgb()
	sha:set_frag_vec4( 3, r,g,b, self.top_density )
	r,g,b = self.bu_col_bottom:get_rgb()
	sha:set_frag_vec4( 4, r,g,b, self.bottom_density )

	local f = math.pi / 180
	local a_lr = self.sun_lr * f
	local azi = self.sun_azi * f
	local y = math.sin( azi )
	local x = math.cos( azi )
	local z = x * math.sin( a_lr )
	x = x * math.cos( a_lr )
	sha:set_frag_vec4( 5, x, y, z )
	sha:set_frag_float( 6, self.sun_beg )
	sha:set_frag_float( 7, self.sun_end )
	r,g,b = self.bu_sun_col:get_rgb()
	sha:set_frag_vec4( 6, r,g,b )

	sha:set_frag_float( 9, math.pow( self.gamma, 2 ) )
end

function meu:get_preset_nb()	return 32	end

