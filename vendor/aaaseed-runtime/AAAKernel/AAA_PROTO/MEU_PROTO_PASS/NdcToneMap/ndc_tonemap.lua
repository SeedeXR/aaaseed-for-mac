function meu:define_meu_infos()
	return { author = "Romain Mâa",
			tags = { "2d", "texture", "renderpass", "imageprocessing"},
			help = 	{
					"Post processing MEU doing color correction in a very complex and \"professional\" way",
					"Done for the Monaco aquarium but used because too difficult to master",
					"Use MEU NdcPost for more simplicity",
					"Dociment and eventually add as a Tab in NdcPost",
					"Output to the Fbo ToneMap"
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
	local DY = .2

	self:add_camera()
	
	ix,iy = self:define_ui_ndc()
	local y_beg = iy

	local i = 1;

	-- exposure and kelvins parameters
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "kelvin degrees", 	sha:get_ref_frag_float(1), nil, 5500, 1000, 6500 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "gamma", 			sha:get_ref_frag_float(2), nil, 2.2, 0.0000001, 3 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "exposure fstops", 	sha:get_ref_frag_float(3), nil, 0, -10, 10 )
	-- iy = iy + SY

	-- tonemap parameters
	bu = self:add_slider(	{ix,iy,	8,SY}, "shoulder strength", sha:get_ref_frag_float(1), nil, .5,  0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "linear strength", 	sha:get_ref_frag_float(2), nil, .5,  0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "linear angle", 		sha:get_ref_frag_float(3), nil, .5,  0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "toe strength", 		sha:get_ref_frag_float(4), nil, .5,  0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "toe numerator", 	sha:get_ref_frag_float(5), nil, .5,  0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "toe denominator", 	sha:get_ref_frag_float(6), nil, .5,  0, 1 )

	ix, iy = 9, y_beg
	bu = self:add_slider(	{ix,iy,	8,SY}, "white point", 		sha:get_ref_frag_float(7), nil, .5,  0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "exposure bias", 	sha:get_ref_frag_float(8), nil, .5,  0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "inverse gamma", 	sha:get_ref_frag_float(9), nil, .5,  0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "keying", 			sha:get_ref_frag_float(10), nil, .5, 0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "average", 			sha:get_ref_frag_float(11), nil, .5, 0, 1 )
	iy = iy + SY

	iy = 2.5
	self:define_ndc_src(	{9,iy,	8,4 },	{"Tex_1"} )
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "tonemap" )
	self:add_shading()
end


function meu:draw()
	self:do_fbo()
	local sha = self:get_shading()
	self:draw_layers_begin()
 		self:draw_layer( 1 )
		sha:set_vert_int_1( self.b_draw and 0 or 1 )
		self:draw_layer( 2 )
	self:draw_layers_end()
end




