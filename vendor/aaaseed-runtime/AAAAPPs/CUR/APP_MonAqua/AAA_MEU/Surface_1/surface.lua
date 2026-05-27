
function meu:define_ui()
	local ref = self.ref
	local ui = self.ui
	local bu
	local sha = self:get_shading()

	local ix = 1
	local iy = 1
	local SY = .9
	local DY = .2

	self:add_shading_ui( {1,1,	8} )

	iy = 3
	bu = self:add_button(	{ix,iy,	SY,SY},	"aaa_fu_int_0", 	self, "b_ex", false )
	iy = iy + SY

	--shitty name to help understand
	--method 1 to pass to the shader
	bu = self:add_slider(	{ix,iy,	8,SY},	"aaa_fu_float_0", 	sha:get_ref_frag_float(1), nil, .5,0,1 )

	-- iy = 4
	-- SY = 2.5
	-- for i=1,1 do
	-- 	self:add_bu_texture_target_unit( {9,iy, nil,SY}, "Tex_"..i, i )
	-- 	iy = iy + SY
	-- end
end

function meu:init()
	local ref = self.ref
	self:add_shading()
end


function meu:draw()
	local sha = self:get_shading()

 	self:draw_layers_begin()
		--method 2 to pass to the shader
		sha:set_frag_int_1( self.b_ex and 1 or 0 )
 		self:draw_layer( 1 )
 	self:draw_layers_end()
end




