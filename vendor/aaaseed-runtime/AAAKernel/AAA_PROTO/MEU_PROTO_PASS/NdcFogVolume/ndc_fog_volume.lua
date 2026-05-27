
function meu:define_meu_infos()
	return { author = "mrvux Mâa",
			tags = { "2d", "texture", "renderpass", "draw", "unfinished" },
			help = 	{
					"Add volumic Fog",
					"Done for Monaco Aquarium but unused",
					"Need to be documented",
					"Output to the Fbo FogVol"
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
	local SY = .8
	local DY = .2

	self:add_camera()
	
	ix,iy = self:define_ui_ndc()

	local i = 1;
	bu = self:add_slider(	{ix,iy,	8,SY}, "color R", sha:get_ref_frag_float(1), nil, .5, 0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "color G", sha:get_ref_frag_float(2), nil, .5, 0, 1 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "color B", sha:get_ref_frag_float(3), nil, .5, 0, 1 )
	iy = iy + SY

	bu = self:add_slider(	{ix,iy,	8,SY}, "start", sha:get_ref_frag_float(4), nil, .5, 0, 100 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "density", sha:get_ref_frag_float(5), nil, .5, 0, 10 )
	iy = iy + SY

	bu = self:add_slider(	{ix,iy,	8,SY}, "noise Freqency", sha:get_ref_frag_float(6), nil, .5, 0, 100 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "noise Offset X", sha:get_ref_frag_float(7), nil, 0, 0, 100 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "noise Offset Y", sha:get_ref_frag_float(8), nil, 0, 0, 100 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "noise Offset Z", sha:get_ref_frag_float(9), nil, 0, 0, 100 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "noise Strength", sha:get_ref_frag_float(10), nil, .5, 0, 10 )
	iy = iy + SY
	bu = self:add_slider(	{ix,iy,	8,SY}, "noise density Factor", sha:get_ref_frag_float(11), nil, .5, 0, 1 )
	iy = iy + SY

	iy = 4
	SY = 2.5
	for i=1,2 do
		self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Tex_"..i, i	)
		iy = iy + SY
	end
end

function meu:init()
	local ref = self.ref
	self:set_meu_fbo( "fogVol" )
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




