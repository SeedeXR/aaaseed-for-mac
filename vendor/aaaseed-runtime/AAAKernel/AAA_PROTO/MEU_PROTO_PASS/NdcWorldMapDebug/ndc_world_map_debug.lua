
function meu:define_meu_infos()
	return { author = "Mâa",
			tags = { "texture", "renderpass" },
			help = 	{
					"Use to debug (by visualising) the world coordonate texture (or map)",
					"Output to the Fbo Debug"
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

	ix,iy = self:define_ui_ndc()

	local i = 1;

	iy = 4
	SY = 2.5
	for i=1,2 do
		self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Tex_"..i, i	)
		iy = iy + SY
	end
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "debug" )
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




