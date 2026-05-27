function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgbf(	{1,8.2 } )
	self:add_mapping_by_side()

	self:add_bu_texture_target_unit()
	self:add_mapping_hexa()
	self:add_nb_uv()

	self:add_camera()

	self:add_monitor()

end
