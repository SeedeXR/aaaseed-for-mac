function meu:define_ui()
	local ref = self.ref
	local bu
	local ix,iy = 1,3
	local SY = 1
	local DY = .2
	self:add_camera()

	self:add_text(	    {ix,iy,8,SY},		"text" )
    iy = iy + SY + DY
    self:add_text_info(	{ix,iy,8,SY},		"text Info" )
    iy = iy + SY + DY

	iy = iy + SY + DY

	self:add_slider( {ix,iy,	8,1},  "ox", self, "ox" ):set_min_max( -4,4 )
	iy = iy + SY + DY
	self:add_slider( {ix,iy,	8,1},  "oy", self, "oy" ):set_min_max( -4,4 )
	iy = iy + SY + DY

end

function meu:update_ui()
	local mus = app:get_gp():get_mus_down()
	local bus = mus.__bus_mus
	bus.__x_offset_exp = self.ox
	bus.__y_offset_exp = self.oy
end

function meu:draw()
	self:draw_layers_begin()

		self:draw_layers()

	self:draw_layers_end()
end
