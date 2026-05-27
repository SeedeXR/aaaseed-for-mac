function meu:define_meu_infos()
	return { author = "mrvux Mâa",
			tags = { "2d", "texture", "renderpass", "imageprocessing", "unfinished" },
			help = 	{
					"Use to add Bloom",
					"Done for Monaco Aquarium but unused, need to be finished and documented or trashed",
					"Output to the current fbo, but there is an Fbo_Bloom MEU in Monaqua"
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

	self:add_shading_ui( 	{1,1,	8} )

	iy = 3
	bu = self:add_button(	{ix,iy,	SY,SY},	"Draw", 	self, "b_draw", false )
	iy = iy + SY

	local i = 1;
	bu = self:add_slider(	{ix,iy,	8,SY},	"spread", 		sha:get_ref_frag_float(1), nil, 1, 0, 100 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY},	"intensity", 	sha:get_ref_frag_float(2), nil, 2, 0, 100 )
	iy = iy + SY

	iy = 4
	SY = 2.5
	for i=1,1 do
		self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Tex_"..i, i	)
		iy = iy + SY
	end
end

function meu:init()
	self:add_shading()
end


function meu:draw()
	local sha = self:get_shading()
	self:draw_layers_begin()
	 	self:draw_layer( 1 )
		sha:set_vert_int_1( self.b_draw and 0 or 1 )
		self:draw_layer( 2 )
 	self:draw_layers_end()
end




