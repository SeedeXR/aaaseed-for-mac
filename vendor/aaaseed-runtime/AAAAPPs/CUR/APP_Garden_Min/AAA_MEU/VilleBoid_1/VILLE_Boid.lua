function meu:define_ui()
	local ref = self.ref
	local bu
	local par

	self:add_rgb()
	self:add_camera()
	self:add_mapping_and_blending()
	self:add_bu_texture_target_unit()
	self:add_multiple_uvf()
end