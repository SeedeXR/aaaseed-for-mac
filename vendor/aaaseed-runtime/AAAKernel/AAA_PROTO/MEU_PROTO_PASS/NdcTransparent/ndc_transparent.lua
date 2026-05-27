function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "2d", "texture", "renderpass", "imageprocessing", "unfinished", "Depreciated" },
			help = 	{
					"the role of this MEU is unclear used in Monaco aquarium before rendering transparent objects",
					"Need to be documented and could probably be replaced by a MEU TexCopy (a texture copy)",
					"Output to the Fbo Transparent"
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

	local i = 1;
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "color R", sha:get_ref_frag_float(1), nil, .5, 0, 1 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "color G", sha:get_ref_frag_float(2), nil, .5, 0, 1 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "color B", sha:get_ref_frag_float(3), nil, .5, 0, 1 )
	-- iy = iy + SY

	-- bu = self:add_slider(	{ix,iy,	8,SY}, "start", sha:get_ref_frag_float(4), nil, .5, 0, 100 )
	-- iy = iy + SY
	-- bu = self:add_slider(	{ix,iy,	8,SY}, "density", sha:get_ref_frag_float(5), nil, .5, 0, 100 )
	-- iy = iy + SY

	iy = 2.5
	SY = 6

	self:define_ndc_src(	 {9,iy, nil,SY},	{"Src"} )
	iy = iy + SY

end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "transparent" )
	self:add_shading()
end


function meu:draw()
	self:do_fbo()
 	self:draw_layers_begin()
 		self:draw_layer( 1 )
		if self.b_draw then
			self:draw_layer( 2 )
		end
 	self:draw_layers_end()
end




