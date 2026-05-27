
function meu:define_meu_infos()
	return { author = "mrvux Mâa",
			tags = { "2d", "texture", "renderpass", "draw", "unfinished" },
			help = 	{
					"Add God rays the sun rays that appears underwater, in smoke or fog ...",
					"Done for Monaco Aquarium but unused",
					"Need to be documented",
					"Output to the Fbo GodRays"
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
	bu = self:add_slider(	{ix,iy,	8,SY}, "screen x", 				sha:get_ref_frag_float(1), nil,	0,	-1,1	)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "screen y", 				sha:get_ref_frag_float(2), nil,	0,	-1,1	)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "illumination decay", 	sha:get_ref_frag_float(3), nil,	.5, 0,1	)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "density", 				sha:get_ref_frag_float(4), nil,	.5, 0,1	)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "weight", 				sha:get_ref_frag_float(5), nil,	.5, 0,1	)
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "decay", 				sha:get_ref_frag_float(6), nil,	.5,	0,1	)
	iy = iy + SY

	iy = 4
	SY = 2.5
	for i=1,1 do
		self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Tex_"..i, i	)
		iy = iy + SY
	end
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "godrays" )
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




