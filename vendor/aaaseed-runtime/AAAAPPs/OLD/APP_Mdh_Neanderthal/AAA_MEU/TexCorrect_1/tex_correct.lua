--todo encapsulate

function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_camera()
	self:add_bu_texture_target_unit()

	self:add_size_uvf_video()
	self:add_trz()

	self:add_monitor()

	local ix = 1
	local iy = 6
	bu = self:add_slider(	{ix,iy,		8,1}, "Black",	self,"black", 	0, 0,1  )
	bu = self:add_slider(	{ix,iy+1,	8,1}, "White",	self,"white", 	1, 0,1  )
end

--function meu:update()
--end
function meu:init()
	self:add_shading()
end

function meu:draw()
	local sha = self:get_shading()
	sha:set_frag_float_1_2( self.black, 1. / ( self.white - self.black ) )
	MEU.draw( self )
	--sha:set_frag_float_1( inter )
end
