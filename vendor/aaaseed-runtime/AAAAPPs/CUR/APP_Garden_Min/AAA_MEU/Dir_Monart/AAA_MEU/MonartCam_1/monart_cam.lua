function meu:define_ui()
	self.__boids_meus = nil

	local ref = self.ref
	local bu
	local par
	local ui = self.ui

	self:add_camera()

	local ix = 1
	local iy = 1.5
	local SY = 1

	self:add_trig(	{ix,		iy,			4, nil}, "Restart" ):set_target_lua( self, "b_restart" )
	iy = iy + SY*1.5
end
