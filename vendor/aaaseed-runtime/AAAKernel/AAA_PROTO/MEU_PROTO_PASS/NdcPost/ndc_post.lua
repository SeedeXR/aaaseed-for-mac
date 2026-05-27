
function meu:define_meu_infos()
	return { author = "Mâa Romain",
			tags = { "2d", "texture", "renderpass", "Imageprocessing", "unfinished" },
			help = 	{
					"Post processing MEU doing color correction (Kelvin, Gamma, Exposure)",
					"include also a vertical color fade with used in Monaco aquarium",
					"Need to be documented and color fade extended to 4 directions, and eventually renamed",
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
	local SYC = SY * 1.5
	local DY = .2

	self:add_camera()

	ix, iy = self:define_ui_ndc()


	bu = self:add_rgbf(	{ix,iy,	8,SYC}, "Color", false )
		ui.bu_col  = bu
	iy = iy + SYC
	-- exposure and kelvins parameters
	bu = self:add_slider(	{ix,iy,	8,SY}, "Kelvin", 		sha:get_ref_frag_float(1), nil, 5500, 1000, 6500 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "Gamma", 		sha:get_ref_frag_float(2), nil, 1, 0.0, 4 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "Exposure", 		sha:get_ref_frag_float(3), nil, 0, 0, 4 )
	iy = iy + SY*2

	ix,iy = 9,2.5
	self:define_ndc_src(	{ix,iy,	8,6 },	{"Tex_1"} )
	iy = iy + 6 + DY
	
	bu = self:add_rgbf(		{ix,iy,	8,SYC}, "Fade_Color",	false )
		ui.bu_col_fade  = bu
	iy = iy + SYC
	bu = self:add_slider(	{ix,iy,	8,SY}, "Fade", 			self, "fade", 		0, 0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "Fade_Range", 	self, "fade_range", .1, 0, 1 )
	iy = iy + SY
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "tonemap" )
	self:add_shading()
		:set_save_vert_int(		false,	1,1		)
		:set_save_frag_float(	false,	1,3		)
		:set_save_frag_float(	false,	9,10	)
		:set_save_frag_vec4(	false,	1,2		)
end


function meu:draw()
	local ui = self.ui
	self:do_fbo()
	local sha = self:get_shading()
 	local r,g,b = ui.bu_col:get_rgb()
	sha:set_frag_vec4( 1, r,g,b )
	r,g,b = ui.bu_col_fade:get_rgb()
	sha:set_frag_vec4( 2, r,g,b )

	local size = 1 + self.fade_range
	local top = size*self.fade
	sha:set_frag_float( 9, top )
	sha:set_frag_float( 10, top -  self.fade_range)

 	self:draw_layers_begin()
 		self:draw_layer( 1 )
		sha:set_vert_int_1( self.b_draw and 0 or 1 )
		self:draw_layer( 2 )
	self:draw_layers_end()
end

function meu:get_preset_nb()	return 32	end



