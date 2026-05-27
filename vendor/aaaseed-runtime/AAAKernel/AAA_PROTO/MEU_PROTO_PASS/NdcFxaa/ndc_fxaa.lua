
function meu:define_meu_infos()
	return { author = "mrvux Mâa",
			tags = { "2d", "texture", "core", "coregraphic", "renderpass", "imageprocessing", "unfinished" },
			help = 	{
					"Do an Fxaa pass (Fast approximate anti-aliasing)",
					"used often as the last pass before presenting the image to a human",
					"Output to the Fbo Fxaa"
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

	self:define_ndc_src(	{9,iy,		8,6 },	{"Tex_1"} )
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "fxaa" ) 
	self:add_shading()
		:set_save_frag_float(	false,	1, 2 )
		:set_save_frag_int(		false,	1, 1 )
end

function meu:draw()
	self:do_fbo()
	local sha = self:get_shading()
 	local sx,sy = self:get_texture_size()
	if sx then
		sha:set_frag_float_1( 1 / sx )
		sha:set_frag_float_2( 1 / sy )
	end

 	self:draw_layers_begin()
 		self:draw_layer( 1 )
		sha:set_vert_int_1( self.b_draw and 0 or 1 )
		self:draw_layer( 2 )
 	self:draw_layers_end()
end




